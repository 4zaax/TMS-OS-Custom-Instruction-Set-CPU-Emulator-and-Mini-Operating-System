# The TMS language

TMS is the assembly-style language that every program in TMS-OS is written in,
including the terminal itself. Source files (`.tms`) are not compiled: the OS
copies their text into emulated RAM and the CPU emulator in `src/` fetches,
decodes and executes them one line at a time.

- [Program layout](#program-layout)
- [Registers and flags](#registers-and-flags)
- [Operands](#operands)
- [Data definitions](#data-definitions)
- [Instructions](#instructions)
- [Interrupts (system calls)](#interrupts-system-calls)
- [Memory map](#memory-map)
- [Gotchas](#gotchas)

## Program layout

```asm
_initial:
MOV cs 90000
MOV ds 95000
MOV ss 97000
MOV esp 99996

# The smallest TMS program: print a string and give control back.
msg: db "Hello from a TMS program!" 10 0

mov esi msg
movb ah 0x09
int 21h
hlt
```

- **The first five lines are the header.** `_initial:` must be the first line,
  followed by four lines that set the code segment (`cs`), data segment (`ds`),
  stack segment (`ss`) and stack pointer (`esp`). The loader executes the header
  and does not copy it into the code segment.
- Everything else is copied into RAM: `db` lines go to the data segment and all
  other lines go to the code segment. Execution starts at the first line after
  the header.
- One instruction per line: `COMMAND arg1 arg2 arg3`, separated by spaces.
- Commands, registers and labels are case-insensitive.
- A line ending in `:` is a label. Lines starting with `#` or `;` are comments.
- A program ends with `HLT`. If it was started by another program, control
  returns to the caller; if it was the boot program, the machine powers off.

## Registers and flags

| Registers | Size | Notes |
|---|---|---|
| `eax` `ebx` `ecx` `edx` | 32-bit | `ax`/`bx`/`cx`/`dx` are the low 16 bits; `al`/`ah`, `bl`/`bh`, `cl`/`ch`, `dl`/`dh` the low/high bytes |
| `esi` `edi` | 32-bit | used as string pointers by the interrupts |
| `ebp` `esp` | 32-bit | `esp` is the stack pointer; the stack grows down |
| `cs` `ds` `ss` | 32-bit | segment bases (absolute addresses in RAM) |
| `ip` | 32-bit | instruction pointer, relative to `cs` |
| `hp` | 32-bit | heap pointer: set by the loader to the first free byte after the data segment |
| `v0` … `v29` | 32-bit | thirty general-purpose "variable" registers |

Flags: `ZF` (zero), `SF` (sign), `CF` (carry/borrow), `OF` (signed overflow).

## Operands

| Form | Example | Meaning |
|---|---|---|
| register | `eax`, `bl`, `v7` | the register's value |
| decimal | `42`, `-3` | immediate number |
| hexadecimal | `0x2A`, `2Ah` | immediate number |
| character | `'a'`, `'\n'`, `'\0'` | ASCII code (`\n \t \r \b \f \v \0 \\ \'` supported) |
| label | `msg`, `loop` | absolute address of a data or code label |
| `[number]` | `[100]` | memory at `ds + 100` |
| `[register]` | `[esi]`, `[v2]` | memory at the absolute address held in the register |
| `[label]` | `[counter]` | memory at the label's address |

Reading `[...]` as a value with the 32-bit instructions reads 4 bytes
(little-endian); use `MOVB`/`MOVW` for single bytes or words.

## Data definitions

```asm
msg_hello: db "hello" 10 0
digits:    db '0', '1', 2, 3
```

`msg_hello` is the string `hello`, a newline and a terminating zero; in
`digits`, characters and numbers are stored as single bytes.

Each `db` line may contain double-quoted strings, numbers and `'c'`
characters separated by spaces or commas. All `db` lines, wherever they
appear in the file, are packed one after another into the data segment, so a
string without a trailing `0` simply continues into the next `db` line (the
terminal uses this for its multi-line help text).

## Instructions

Most instructions come in three sizes: no suffix = 32-bit (dword), `W` =
16-bit (word), `B` = 8-bit (byte). The destination must be a register of that
size unless noted.

### Data movement

| Instruction | Effect |
|---|---|
| `MOV dst src` | `dst` is a 32-bit register or `[mem]`; `src` is anything above |
| `MOVW dst src`, `MOVB dst src` | 16/8-bit moves between registers, memory and immediates |
| `PUSH src` / `PUSHW` / `PUSHB` | push a value |
| `POP reg` / `POPW` / `POPB` | pop into a register |

### Arithmetic and logic (three operands: `OP dst a b` means `dst = a OP b`)

| Instruction | Effect |
|---|---|
| `ADD` `SUB` (+`W`/`B`) | sets `ZF SF CF OF` |
| `MUL` `IMUL` (+`W`/`B`) | unsigned / signed multiply, sets `CF OF` on overflow |
| `DIV` `IDIV` (+`W`/`B`) | unsigned / signed divide; the remainder goes to `EDX` (`DX`/`DL`) |
| `AND` `OR` `XOR` `SHL` `SHR` (+`W`/`B`) | bitwise ops and shifts (`SHL dst value count`) |
| `NOT dst src` (+`W`/`B`) | bitwise not |
| `INC reg` `DEC reg` (+`W`/`B`) | add / subtract one |

### Comparisons

`EQ` `NEQ` `GT` `GTE` `LS` `LSE` take `dst a b` and set the **byte** register
`dst` to `1` or `0`. Comparisons are signed 32-bit.

```asm
gte bl v1 10
jmp too_big bl
```

### Control flow

| Instruction | Effect |
|---|---|
| `JMP label [cond]` | jump; with a condition, only if `cond` is exactly `1` |
| `CALL label [cond]` | push the return address and jump (conditionally, like `JMP`) |
| `RET [cond]` | return (conditionally, like `JMP`) |
| `JE` `JNE` `JG` `JL label` | jump on the flags left by `ADD`/`SUB` |
| `HLT` | end the program |

There is no `CMP`: compare with `EQ`/`GT`/... into a byte register and use it
as the condition of `JMP`, `CALL` or `RET`.

### System and debugging

| Instruction | Effect |
|---|---|
| `INT n` | software interrupt (see below) |
| `CLS` | clear the screen |
| `SHOW reg` | print a 32-bit register as hex and signed decimal |
| `SHOWD` `SHOWW` `SHOWB` / `SHOWSD` `SHOWSW` `SHOWSB` | print a register unsigned / signed |
| `SHOWD_MEM addr` … `SHOWSB_MEM addr` | print memory at an address value |
| `SHOW_ALL_REGS`, `SHOWF`, `CLF` | dump registers, show flags, clear flags |

## Interrupts (system calls)

| Call | Function |
|---|---|
| `INT 21h`, `AH=02h` | print the character in `DL` |
| `INT 21h`, `AH=08h` | read one character from the keyboard into `AL` |
| `INT 21h`, `AH=09h` | print the 0-terminated string at address `ESI` |
| `INT 21h`, `AH=4Bh` | **exec**: load and run the program whose path is the string at `ESI` |
| `INT 10h`, `AH=00h` | clear the screen |

`AH=4Bh` is how the terminal starts apps. The OS saves every register and the
caller's label table, runs the child's header, loads it into its own segments
and executes it. When the child halts, the caller's state is restored and it
continues after the `INT`. Children can exec other programs the same way.

When standard input ends, `AH=08h` returns a newline and the machine powers
off, which makes it easy to drive the OS from a script.

## Memory map

The machine has 1,024,000 bytes of RAM. There is no virtual memory: each
program picks its own segments in its header, so programs that may be in
memory at the same time must not overlap. The bundled programs use:

| Program | `cs` | `ds` | `ss` … `esp` | Other |
|---|---|---|---|---|
| `terminal` | 0 | 10000 | 13000 … 19998 | command history grows from `hp` |
| `reverse` | 30000 | 33000 | 35000 … 40000 | |
| `calc` | 40000 | 50000 | 55000 … 60000 | |
| `fact` | 60000 | 70000 | 72000 … 80000 | |
| `hello` | 90000 | 95000 | 97000 … 99996 | |
| `sum`, `fib`, `arrays` | 256000 | 306000 | 336000 … 360000 | `fib` keeps its table at 31000, `arrays` at 200000 |

Moving data to or from memory at or above a program's stack segment prints a
warning, which catches most runaway pointers.

## Gotchas

- **No inline comments.** `call foo # comment` is read as `call` with extra
  operands. Put comments on their own line.
- **No space character literal.** Operands are split on whitespace, so write
  `32` instead of `' '`.
- Lines are at most 511 characters.
- Code is stored as text, so the code segment needs as many bytes as the
  source (indentation included) and must not run into the data segment.
- `[100]` is relative to `ds`, but `[v2]` and `[label]` are absolute.
