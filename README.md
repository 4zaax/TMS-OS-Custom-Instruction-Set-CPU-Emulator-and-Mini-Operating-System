# TMS-OS

A toy operating system with its own programming language, built from scratch
in C.

TMS-OS is a CPU emulator and program loader written in C, plus a set of
programs written in **TMS**, an assembly-style language we designed for it.
When the machine boots it loads a terminal (itself a TMS program), and the
terminal can launch other TMS apps through a system call, just like a shell
on a real OS.

```
tms:~$ run calc
please enter your numbers and operators like this <int> <operator> <int> (e to exit)
12 + 30
the result value is : 42
e
tms:~$
```

## Highlights

- **CPU emulator in C** with x86-style registers (`eax`, `ax`, `al`/`ah`, …),
  thirty extra 32-bit registers (`v0`–`v29`), flags, a stack and 1 MB of
  byte-addressable RAM.
- **Our own language.** More than 80 instructions: 8/16/32-bit arithmetic,
  bitwise ops, comparisons, conditional jumps, calls and returns, stack
  operations and software interrupts. Programs declare their own code, data
  and stack segments.
- **Loader and labels.** Programs are loaded into RAM as text. The loader
  splits data from code, lays out the data segment and builds a label table,
  and the CPU then fetches and executes one line at a time.
- **System calls.** `INT 21h` provides character and string I/O and an
  **exec** call (`AH=4Bh`) that saves the caller's state, loads a program
  from disk, runs it and returns control afterwards.
- **A terminal written in TMS** with command parsing, command history,
  `echo`, `help`, `run <app>` and more.
- **Apps written in TMS**: a calculator, factorial, Fibonacci, sum, an array
  toolkit (sort, min, max, average) and a string reverser.

## Quick start

You need a C compiler (gcc, clang or MSVC).

```sh
make            # builds ./tmsos
./tmsos         # boots the terminal (see boot.txt)
make test       # runs the smoke tests
```

Without `make`:

```sh
cc -std=c11 -O2 -o tmsos src/os.c src/handlers.c src/helpers.c
```

On Windows you can use the same command with MinGW, or CMake
(`cmake -S . -B build && cmake --build build`). Run the binary from the
repository root so it can find `boot.txt` and `apps/`.

You can also boot straight into any program: `./tmsos apps/calc.tms`.

## A session

```
Welcome to TMS-OS. Type help to see the available commands.
tms:~$ apps
installed apps: hello  calc  fact  fib  sum  reverse  arrays
tms:~$ run fact
Enter your number (0 to exit):
10
3628800
Enter your number (0 to exit):
0
Program finished. Terminal would take back control.
tms:~$ run reverse
Type a line and it will be printed backwards. An empty line exits.
stressed
desserts

bye!
tms:~$ run fib
$/add
Enter your (n):
20
6765
$/exit
tms:~$ hist
Showing commands history: (type clh to clear ram and free up memory)
apps
run fact
run reverse
run fib
hist
tms:~$ hlt
Powering off...
Shutting down OS...
```

## How it works

```
      C emulator (src/)                        TMS programs (apps/)

  +---------------------------+   boots    +--------------+  run calc   +----------+
  | boot.txt -> loader -> RAM |----------->| terminal.tms |------------>| calc.tms |
  | CPU: fetch/decode/execute |            +--------------+ INT 21h/4Bh +----------+
  | INT 21h: I/O + exec       |                   ^                          |
  | INT 10h: clear screen     |                   +--- HLT: caller restored -+
  +---------------------------+
```

1. **Boot** (`src/os.c`): `main` reads `boot.txt` to find the first program,
   by default `apps/terminal.tms`.
2. **Load**: the program's header (`_initial:` and the next four lines)
   sets `cs`, `ds`, `ss` and `esp`. The loader writes every `db` line into
   the data segment, copies the remaining source text into the code segment
   and scans it for labels. `hp` points just past the data and works as a heap.
3. **Execute**: `execute_program` reads the line at `cs + ip`, advances `ip`
   and hands the line to `parse_and_execute`, which dispatches to a handler in
   `src/handlers.c`. Jumps and calls simply rewrite `ip`.
4. **Exec** (`INT 21h`, `AH=4Bh`): the running program puts a file path in
   memory and raises the interrupt. The OS snapshots all registers and the
   label table, boots the child in its own segments, and restores everything
   when the child executes `HLT`. That is how the terminal runs apps, and
   apps could in turn run other apps.

## The TMS language

```asm
_initial:
MOV cs 90000
MOV ds 95000
MOV ss 97000
MOV esp 99996

msg: db "Hello from a TMS program!" 10 0

mov esi msg
movb ah 0x09
int 21h
hlt
```

This is `apps/hello.tms`; start it from the terminal with `run hello`. To
add your own app, drop a `.tms` file into `apps/` and `run` it by name. The full reference, covering registers, operands, every
instruction, the interrupts and the memory map, is in
[docs/LANGUAGE.md](docs/LANGUAGE.md).

## Project layout

```
src/
  os.c          boot, loader, label table, fetch/execute loop, dispatcher
  handlers.c    instruction and interrupt implementations
  helpers.c     operand decoding, registers, stack helpers
apps/
  terminal.tms  the shell the OS boots into
  calc.tms  fact.tms  fib.tms  sum.tms  arrays.tms  reverse.tms  hello.tms
boot.txt        which program to boot
docs/
  LANGUAGE.md          language reference
  helper_functions.pdf our internal notes on the emulator's helper functions
tests/run_tests.sh     boots the OS and drives every app through stdin
```

## Background

We built TMS-OS in 2025 as a team project for a C programming course. The
assignment was to design our own language, simulate an operating system,
write a terminal in that language, and make the terminal able to open other
applications written in the same language.

The language started out as a small high-level scripting language and
gradually turned into the assembly-style instruction set you see now, with a
real loader, segments and a stack; the git history shows that evolution. In
2026 the code was cleaned up for publication. The cleanup added the
cross-platform build and tests, generic `run <app>` support in the terminal,
and fixes for bugs in the emulator and the apps.

## Authors

- Shayan Khodarahimi ([@4zaax](https://github.com/4zaax))
- Tannaz Farokhi ([@TANNAZ84](https://github.com/TANNAZ84))
- Mani Adhami ([@NOT-87](https://github.com/NOT-87))

## License

[MIT](LICENSE)
