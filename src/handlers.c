#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "os.h"
#include "helpers.h"
#include "handlers.h"

static int map_cr_to_lf(int ch);
static int read_emu_cstr(uint32_t addr, char* out, size_t out_sz);


// INC
void inc_handler(const char* reg) {
    uint32_t* r = get_register(reg, 4);
    if (!r) { printf("INC: must be 32-bit reg (%s)\n", reg); return; }
    (*r)++;
}
void incw_handler(const char* reg) {
    uint16_t* r = get_register(reg, 2);
    if (!r) { printf("INCW: must be 16-bit reg (%s)\n", reg); return; }
    (*r)++;
}
void incb_handler(const char* reg) {
    uint8_t* r = get_register(reg, 1);
    if (!r) { printf("INCB: must be 8-bit reg (%s)\n", reg); return; }
    (*r)++;
}

// DEC
void dec_handler(const char* reg) {
    uint32_t* r = get_register(reg, 4);
    if (!r) { printf("DEC: must be 32-bit reg (%s)\n", reg); return; }
    (*r)--;
}
void decw_handler(const char* reg) {
    uint16_t* r = get_register(reg, 2);
    if (!r) { printf("DECW: must be 16-bit reg (%s)\n", reg); return; }
    (*r)--;
}
void decb_handler(const char* reg) {
    uint8_t* r = get_register(reg, 1);
    if (!r) { printf("DECB: must be 8-bit reg (%s)\n", reg); return; }
    (*r)--;
}

void showd(const char* reg) {
    uint32_t* r = get_register(reg, 4);
    if (r) printf("%s = 0x%08X (%u)\n", reg, *r, *r);
    else printf("showd: %s is not a dword register\n", reg);
}

void showw(const char* reg) {
    uint16_t* r = get_register(reg, 2);
    if (r) printf("%s = 0x%04X (%u)\n", reg, *r, *r);
    else printf("showw: %s is not a word register\n", reg);
}

void showb(const char* reg) {
    uint8_t* r = get_register(reg, 1);
    if (r) printf("%s = 0x%02X (%u) (%c)\n", reg, *r, *r, *r);
    else printf("showb: %s is not a byte register\n", reg);
}

// Show signed dword (32 bits)
void showsd(const char* reg) {
    int32_t* r = get_register(reg, 4);
    if (r) printf("%s = 0x%08X (%d)\n", reg, *r, *r);
    else printf("showsd: %s is not a dword register\n", reg);
}

// Show signed word (16 bits)
void showsw(const char* reg) {
    int16_t* r = get_register(reg, 2);
    if (r) printf("%s = 0x%04X (%d)\n", reg, (uint16_t) *r, (int16_t) *r);
    else printf("showsw: %s is not a word register\n", reg);
}

// Show signed byte (8 bits)
void showsb(const char* reg) {
    int8_t* r = get_register(reg, 1);
    if (r) printf("%s = 0x%02X (%d)\n", reg, (uint8_t)*r, (int8_t)*r);
    else printf("showsb: %s is not a byte register\n", reg);
}

void showc(const char* arg) {
    if (is_mem_addr(arg)){
        uint8_t r = find_mem8_value_unsigned(find_mem_addr(arg));
        printf("%s = 0x%02X (%c)\n", arg, r, r);
        showb_mem(find_mem_addr(arg));
        return;
    }
    else if (get_register(arg,1)){
        uint8_t* r = get_register(arg, 1);
        printf("%s = 0x%02X (%c)\n", arg, *r, *r);
        return;
    }
    else printf("showc: %s is not a character register\n", arg);
}

// Unsigned
void showd_mem(int addr) {
    printf("MEM[%d] = 0x%08X (%u)\n", addr, *((uint32_t*)&memory[addr]), *((uint32_t*)&memory[addr]));
}
void showw_mem(int addr) {
    printf("MEM[%d] = 0x%04X (%u)\n", addr, *((uint16_t*)&memory[addr]), *((uint16_t*)&memory[addr]));
}
void showb_mem(int addr) {
    printf("MEM[%d] = 0x%02X (%u) (%c)\n", addr, memory[addr], memory[addr], memory[addr]);
}

// Signed
void showsd_mem(int addr) {
    printf("MEM[%d] = 0x%08X (%d)\n", addr, *((int32_t*)&memory[addr]), *((int32_t*)&memory[addr]));
}
void showsw_mem(int addr) {
    printf("MEM[%d] = 0x%04X (%d)\n", addr, *((int16_t*)&memory[addr]), *((int16_t*)&memory[addr]));
}
void showsb_mem(int addr) {
    printf("MEM[%d] = 0x%02X (%d)\n", addr, *((int8_t*)&memory[addr]), *((int8_t*)&memory[addr]));
}

void and_handler(const char* dest, const char* src1, const char* src2) {
    uint32_t* d_reg = get_register(dest, 4);
    if (!d_reg) { printf("AND: dest must be 32-bit register (%s)\n", dest); return; }
    uint32_t v1 = find_any_value(src1);
    uint32_t v2 = find_any_value(src2);
    *d_reg = v1 & v2;
}

void andw_handler(const char* dest, const char* src1, const char* src2) {
    uint16_t* d_reg = get_register(dest, 2);
    if (!d_reg) { printf("ANDW: dest must be 16-bit register (%s)\n", dest); return; }
    uint16_t v1 = (uint16_t)find_any_value(src1);
    uint16_t v2 = (uint16_t)find_any_value(src2);
    *d_reg = v1 & v2;
}

void andb_handler(const char* dest, const char* src1, const char* src2) {
    uint8_t* d_reg = get_register(dest, 1);
    if (!d_reg) { printf("ANDB: dest must be 8-bit register (%s)\n", dest); return; }
    uint8_t v1 = (uint8_t)find_any_value(src1);
    uint8_t v2 = (uint8_t)find_any_value(src2);
    *d_reg = v1 & v2;
}

// OR
void or_handler(const char* dest, const char* src1, const char* src2) {
    uint32_t* d = get_register(dest, 4);
    if (!d) { printf("OR: dest must be 32-bit reg\n"); return; }
    *d = find_any_value(src1) | find_any_value(src2);
}
void orw_handler(const char* d, const char* s1, const char* s2) {
    uint16_t* r = get_register(d, 2);
    if (!r) { printf("ORW: dest must be 16-bit reg\n"); return; }
    *r = (uint16_t)find_any_value(s1) | (uint16_t)find_any_value(s2);
}
void orb_handler(const char* d, const char* s1, const char* s2) {
    uint8_t* r = get_register(d, 1);
    if (!r) { printf("ORB: dest must be 8-bit reg\n"); return; }
    *r = (uint8_t)find_any_value(s1) | (uint8_t)find_any_value(s2);
}

// XOR
void xor_handler(const char* dest, const char* src1, const char* src2) {
    uint32_t* d = get_register(dest, 4);
    if (!d) { printf("XOR: dest must be 32-bit reg\n"); return; }
    *d = find_any_value(src1) ^ find_any_value(src2);
}
void xorw_handler(const char* d, const char* s1, const char* s2) {
    uint16_t* r = get_register(d, 2);
    if (!r) { printf("XORW: dest must be 16-bit reg\n"); return; }
    *r = (uint16_t)find_any_value(s1) ^ (uint16_t)find_any_value(s2);
}
void xorb_handler(const char* d, const char* s1, const char* s2) {
    uint8_t* r = get_register(d, 1);
    if (!r) { printf("XORB: dest must be 8-bit reg\n"); return; }
    *r = (uint8_t)find_any_value(s1) ^ (uint8_t)find_any_value(s2);
}
void not_handler(const char* dest, const char* src) {
    uint32_t* d = get_register(dest, 4);
    if (!d) { printf("NOT: dest must be 32-bit reg\n"); return; }
    *d = ~find_any_value(src);
}
void notw_handler(const char* dest, const char* src) {
    uint16_t* d = get_register(dest, 2);
    if (!d) { printf("NOTW: dest must be 16-bit reg\n"); return; }
    *d = ~(uint16_t)find_any_value(src);
}
void notb_handler(const char* dest, const char* src) {
    uint8_t* d = get_register(dest, 1);
    if (!d) { printf("NOTB: dest must be 8-bit reg\n"); return; }
    *d = ~(uint8_t)find_any_value(src);
}
void shl_handler(const char* dest, const char* src1, const char* src2) {
    uint32_t* d = get_register(dest, 4);
    if (!d) { printf("SHL: dest must be 32-bit reg\n"); return; }
    *d = find_any_value(src1) << find_any_value(src2);
}
void shlw_handler(const char* d, const char* s1, const char* s2) {
    uint16_t* r = get_register(d, 2);
    if (!r) { printf("SHLW: dest must be 16-bit reg\n"); return; }
    *r = (uint16_t)find_any_value(s1) << (uint16_t)find_any_value(s2);
}
void shlb_handler(const char* d, const char* s1, const char* s2) {
    uint8_t* r = get_register(d, 1);
    if (!r) { printf("SHLB: dest must be 8-bit reg\n"); return; }
    *r = (uint8_t)find_any_value(s1) << (uint8_t)find_any_value(s2);
}
void shr_handler(const char* dest, const char* src1, const char* src2) {
    uint32_t* d = get_register(dest, 4);
    if (!d) { printf("SHR: dest must be 32-bit reg\n"); return; }
    *d = find_any_value(src1) >> find_any_value(src2);
}
void shrw_handler(const char* d, const char* s1, const char* s2) {
    uint16_t* r = get_register(d, 2);
    if (!r) { printf("SHRW: dest must be 16-bit reg\n"); return; }
    *r = (uint16_t)find_any_value(s1) >> (uint16_t)find_any_value(s2);
}
void shrb_handler(const char* d, const char* s1, const char* s2) {
    uint8_t* r = get_register(d, 1);
    if (!r) { printf("SHRB: dest must be 8-bit reg\n"); return; }
    *r = (uint8_t)find_any_value(s1) >> (uint8_t)find_any_value(s2);
}




// PUSH dword (4 bytes)
void push_handler(const char* src) {
    uint32_t val = find_any_value(src);
    if (*esp < 4) { printf("PUSH: stack overflow\n"); return; }
    *esp -= 4;
    *((uint32_t*)&memory[*esp]) = val;
}

// PUSH word (2 bytes)
void pushw_handler(const char* src) {
    uint16_t val = (uint16_t)find_any_value(src);
    if (*esp < 2) { printf("PUSHW: stack overflow\n"); return; }
    *esp -= 2;
    *((uint16_t*)&memory[*esp]) = val;
}

// PUSH byte (1 byte)
void pushb_handler(const char* src) {
    uint8_t val = (uint8_t)find_any_value(src);
    if (*esp < 1) { printf("PUSHB: stack overflow\n"); return; }
    *esp -= 1;
    *((uint8_t*)&memory[*esp]) = val;
}

// POP dword (4 bytes)
void pop_handler(const char* dest) {
    if (*esp > MEMORY_SIZE - 4) { printf("POP: stack underflow\n"); return; }
    uint32_t* d = get_register(dest, 4);
    if (!d) { printf("POP: dest must be 32-bit reg (%s)\n", dest); return; }
    *d = *((uint32_t*)&memory[*esp]);
    *esp += 4;
}

// POP word (2 bytes)
void popw_handler(const char* dest) {
    if (*esp > MEMORY_SIZE - 2) { printf("POPW: stack underflow\n"); return; }
    uint16_t* d = get_register(dest, 2);
    if (!d) { printf("POPW: dest must be 16-bit reg (%s)\n", dest); return; }
    *d = *((uint16_t*)&memory[*esp]);
    *esp += 2;
}

// POP byte (1 byte)
void popb_handler(const char* dest) {
    if (*esp > MEMORY_SIZE - 1) { printf("POPB: stack underflow\n"); return; }
    uint8_t* d = get_register(dest, 1);
    if (!d) { printf("POPB: dest must be 8-bit reg (%s)\n", dest); return; }
    *d = *((uint8_t*)&memory[*esp]);
    *esp += 1;
}

void mov_handler (char* dest, char* src) {
    uint32_t* d_reg = get_register(dest, 4);
    uint32_t* s_reg = get_register(src, 4);

    // dest is 4bytes register, src is memory
    if (d_reg && is_mem_addr(src)) {
        int addr = find_mem_addr(src);
        if (addr >= *ss){
            fflush(stdout);
            fflush(stdout); fprintf(stdout, "Warning : You are changing out of range memory.\n");
        }
        *d_reg = *((uint32_t*)&memory[addr]);
        return;
    }
    // dest is memory, src is 4byte register
    if (is_mem_addr(dest) && s_reg) {
        int addr = find_mem_addr(dest);
        if (addr >= *ss){
            fflush(stdout);
            if (*cs != 0){
                fprintf(stderr, "Warning : You are changing out of range memory.\n");
            }
        }
        *((uint32_t*)&memory[addr]) = *s_reg;
        return;
    }
    // Register to Register
    if (d_reg && s_reg) {
        *d_reg = *s_reg;
        return;
    }
    //anything else that yields a value (label, immediate)
    if (d_reg) {
        uint32_t value = find_any_value(src);
        *d_reg = value;
        return;
    }
    // memory destination and label/immediate source
    if (is_mem_addr(dest)) {
        int addr = find_mem_addr(dest);
        if (addr >= *ss && *cs != 0){
            fflush(stdout);
            fprintf(stderr, "Warning : You are changing out of range memory.\n");
        }
        *((uint32_t*)&memory[addr]) = find_any_value(src);
        return;
    }
    printf("MOV: Invalid operands (%s, %s)\n", dest, src);
}



void movb_handler(char* dest, char* src) {
    uint8_t* d_reg = get_register(dest, 1);
    uint8_t* s_reg = get_register(src, 1);

    // dest is register, src is memory
    if (d_reg && is_mem_addr(src)) {
        int addr = find_mem_addr(src);
        if (addr >= *ss){
            fflush(stdout);
            if (*cs != 0){
                fprintf(stderr, "Warning : You are changing out of range memory.\n");
            }
        }
        *d_reg = memory[addr];
        return;
    }
    // dest is memory, src is register
    if (is_mem_addr((char*)dest) && s_reg) {
        int addr = find_mem_addr(dest);
        if (addr >= *ss){
            fflush(stdout);
            if (*cs != 0){
                fprintf(stderr, "Warning : You are changing out of range memory.\n");
            }
        }
        memory[addr] = *s_reg;
        return;
    }
    // immediate to register
    if (d_reg && is_immediate(src)) {
        *d_reg = (uint8_t)find_any_value(src);
        return;
    }
    // immediate to memory
    if (is_mem_addr(dest) && is_immediate(src)) {
        int addr = find_mem_addr(dest);
        if (addr >= *ss){
            fflush(stdout);
            if (*cs != 0){
                fprintf(stderr, "Warning : You are changing out of range memory.\n");
            }
        }
        memory[addr] = (uint8_t)find_any_value(src);
        return;
    }
    // register to register
    if (d_reg && s_reg) {
        *d_reg = *s_reg;
        return;
    }
    printf("MOVB: Invalid operands (%s, %s)\n", dest, src);
}

void movw_handler(char* dest, char* src) {
    uint16_t* d_reg = get_register(dest, 2);
    uint16_t* s_reg = get_register(src, 2);

    // dest is register, src is memory
    if (d_reg && is_mem_addr(src)) {
        int addr = find_mem_addr(src);
        if (addr >= *ss){
            fflush(stdout);
            if (*cs != 0){
                fprintf(stderr, "Warning : You are changing out of range memory.\n");
            }
        }
        *d_reg = *((uint16_t*)&memory[addr]);
        return;
    }
    // dest is memory, src is register
    if (is_mem_addr(dest) && s_reg) {
        int addr = find_mem_addr((char*)dest);
        if (addr >= *ss){
            fflush(stdout);
            if (*cs != 0){
                fprintf(stderr, "Warning : You are changing out of range memory.\n");
            }
        }
        *((uint16_t*)&memory[addr]) = *s_reg;
        return;
    }
    // immediate to register
    if (d_reg && is_immediate(src)) {
        *d_reg = (uint16_t)find_any_value((char*)src);
        return;
    }
    // immediate to memory
    if (is_mem_addr((char*)dest) && is_immediate(src)) {
        int addr = find_mem_addr((char*)dest);
        if (addr >= *ss){
            fflush(stdout);
            fprintf(stderr, "Warning : You are changing out of range memory.\n");
        }
        *((uint16_t*)&memory[addr]) = (uint16_t)find_any_value((char*)src);
        return;
    }
    // register to register
    if (d_reg && s_reg) {
        *d_reg = *s_reg;
        return;
    }
    printf("MOVW: Invalid operands (%s, %s)\n", dest, src);
}

void add_handler(const char* dest, const char* src1, const char* src2) {
    uint32_t* d_reg = get_register(dest, 4);
    if (!d_reg) {
        printf("ADD: dest must be 32-bit register (%s)\n", dest);
        return;
    }
    uint32_t val1 = find_any_value(src1);
    uint32_t val2 = find_any_value(src2);
    uint32_t res = val1 + val2;
    *d_reg =res;
    set_flags_add32(val1, val2, res);
}

void addw_handler(const char* dest, const char* src1, const char* src2) {
    uint16_t* d_reg = get_register(dest, 2);
    if (!d_reg) {
        printf("ADDW: dest must be 16-bit register (%s)\n", dest);
        return;
    }
    uint16_t val1 = (uint16_t)find_any_value(src1);
    uint16_t val2 = (uint16_t)find_any_value(src2);
    uint16_t res = val1 + val2;
    *d_reg = res;
    set_flags_add16(val1, val2, res);
}

void addb_handler(const char* dest, const char* src1, const char* src2) {
    uint8_t* d_reg = get_register(dest, 1);
    if (!d_reg) {
        printf("ADDB: dest must be 8-bit register (%s)\n", dest);
        return;
    }
    uint8_t val1 = (uint8_t)find_any_value(src1);
    uint8_t val2 = (uint8_t)find_any_value(src2);
    uint8_t res = val1 + val2;
    *d_reg = res;
    set_flags_add8(val1, val2, res);
}

void sub_handler(const char* dest, const char* src1, const char* src2) {
    uint32_t* d_reg = get_register(dest, 4);
    if (!d_reg) {
        printf("SUB: dest must be 32-bit register (%s)\n", dest);
        return;
    }
    uint32_t val1 = find_any_value(src1);
    uint32_t val2 = find_any_value(src2);
    uint32_t res = val1 - val2;
    *d_reg = res;
    set_flags_sub32(val1, val2, res);
}

void subw_handler(const char* dest, const char* src1, const char* src2) {
    uint16_t* d_reg = get_register(dest, 2);
    if (!d_reg) {
        printf("SUBW: dest must be 16-bit register (%s)\n", dest);
        return;
    }
    uint16_t val1 = (uint16_t)find_any_value(src1);
    uint16_t val2 = (uint16_t)find_any_value(src2);
    uint16_t res = val1 - val2;
    *d_reg = res;
    set_flags_sub16(val1, val2, res);
}

void subb_handler(const char* dest, const char* src1, const char* src2) {
    uint8_t* d_reg = get_register(dest, 1);
    if (!d_reg) {
        printf("SUBB: dest must be 8-bit register (%s)\n", dest);
        return;
    }
    uint8_t val1 = (uint8_t)find_any_value(src1);
    uint8_t val2 = (uint8_t)find_any_value(src2);
    uint8_t res = val1 - val2;
    *d_reg = res;
    set_flags_sub8(val1, val2, res);
}

// Unsigned Multiplication
void mul_handler(const char* dest, const char* src1, const char* src2) {
    uint32_t* d_reg = get_register(dest, 4);
    if (!d_reg) { printf("MUL: dest must be 32-bit register\n"); return; }
    uint32_t v1 = (uint32_t)find_any_value(src1);
    uint32_t v2 = (uint32_t)find_any_value(src2);
    uint64_t result = (uint64_t)v1 * (uint64_t)v2;
    *d_reg = (uint32_t)result;
    set_flags_mul32(v1, v2, (uint32_t)result);
}

void mulw_handler(const char* dest, const char* src1, const char* src2) {
    uint16_t* d_reg = get_register(dest, 2);
    if (!d_reg) { printf("MULW: dest must be 16-bit register\n"); return; }
    uint16_t v1 = (uint16_t)find_any_value(src1);
    uint16_t v2 = (uint16_t)find_any_value(src2);
    uint32_t result = (uint32_t)v1 * (uint32_t)v2;
    *d_reg = (uint16_t)result;
    set_flags_mul16(v1, v2, (uint16_t)result);
}

void mulb_handler(const char* dest, const char* src1, const char* src2) {
    uint8_t* d_reg = get_register(dest, 1);
    if (!d_reg) { printf("MULB: dest must be 8-bit register\n"); return; }
    uint8_t v1 = (uint8_t)find_any_value(src1);
    uint8_t v2 = (uint8_t)find_any_value(src2);
    uint16_t result = (uint16_t)v1 * (uint16_t)v2;
    *d_reg = (uint8_t)result;
    set_flags_mul8(v1, v2, (uint8_t)result);
}

// Signed multiply
void imul_handler(const char* dest, const char* src1, const char* src2) {
    int32_t* d_reg = get_register(dest, 4);
    if (!d_reg) { printf("IMUL: dest must be 32-bit register\n"); return; }
    int32_t v1 = (int32_t)find_any_value(src1);
    int32_t v2 = (int32_t)find_any_value(src2);
    int64_t result = (int64_t)v1 * (int64_t)v2;
    *d_reg = (int32_t)result;
    set_flags_imul32(v1, v2, (int32_t)result);
}

void imulw_handler(const char* dest, const char* src1, const char* src2) {
    int16_t* d_reg = get_register(dest, 2);
    if (!d_reg) { printf("IMULW: dest must be 16-bit register\n"); return; }
    int16_t v1 = (int16_t)find_any_value(src1);
    int16_t v2 = (int16_t)find_any_value(src2);
    int32_t result = (int32_t)v1 * (int32_t)v2;
    *d_reg = (int16_t)result;
    set_flags_imul16(v1, v2, (int16_t)result);
}

void imulb_handler(const char* dest, const char* src1, const char* src2) {
    int8_t* d_reg = get_register(dest, 1);
    if (!d_reg) { printf("IMULB: dest must be 8-bit register\n"); return; }
    int8_t v1 = (int8_t)find_any_value(src1);
    int8_t v2 = (int8_t)find_any_value(src2);
    int16_t result = (int16_t)v1 * (int16_t)v2;
    *d_reg = (int8_t)result;
    set_flags_imul8(v1, v2, (int8_t)result);
}

// 32-bit unsigned division: dest = src1 / src2, EDX = src1 % src2
void div_handler(const char* dest, const char* src1, const char* src2) {
    uint32_t* d_reg = get_register(dest, 4);
    uint32_t v1 = find_any_value(src1);
    uint32_t v2 = find_any_value(src2);
    if (!d_reg) { printf("DIV: dest must be 32-bit register (%s)\n", dest); return; }
    if (v2 == 0) { printf("DIV: divide by zero\n"); return; }
    *d_reg = v1 / v2;
    *edx   = v1 % v2;
}

// 16-bit unsigned division: dest = src1 / src2, DX = src1 % src2
void divw_handler(const char* dest, const char* src1, const char* src2) {
    uint16_t* d_reg = get_register(dest, 2);
    uint16_t v1 = (uint16_t)find_any_value(src1);
    uint16_t v2 = (uint16_t)find_any_value(src2);
    if (!d_reg) { printf("DIVW: dest must be 16-bit register (%s)\n", dest); return; }
    if (v2 == 0) { printf("DIVW: divide by zero\n"); return; }
    *d_reg = v1 / v2;
    *dx    = v1 % v2;
}

// 8-bit unsigned division: dest = src1 / src2, DL = src1 % src2
void divb_handler(const char* dest, const char* src1, const char* src2) {
    uint8_t* d_reg = get_register(dest, 1);
    uint8_t v1 = (uint8_t)find_any_value(src1);
    uint8_t v2 = (uint8_t)find_any_value(src2);
    if (!d_reg) { printf("DIVB: dest must be 8-bit register (%s)\n", dest); return; }
    if (v2 == 0) { printf("DIVB: divide by zero\n"); return; }
    *d_reg = v1 / v2;
    *dl    = v1 % v2;
}

// 32-bit signed division: dest = src1 / src2, EDX = src1 % src2
void idiv_handler(const char* dest, const char* src1, const char* src2) {
    int32_t* d_reg = get_register(dest, 4);
    int32_t v1 = (int32_t)find_any_value(src1);
    int32_t v2 = (int32_t)find_any_value(src2);
    if (!d_reg) { printf("IDIV: dest must be 32-bit register (%s)\n", dest); return; }
    if (v2 == 0) { printf("IDIV: divide by zero\n"); return; }
    *d_reg = v1 / v2;
    *edx   = v1 % v2;
}

// 16-bit signed division: dest = src1 / src2, DX = src1 % src2
void idivw_handler(const char* dest, const char* src1, const char* src2) {
    int16_t* d_reg = get_register(dest, 2);
    int16_t v1 = (int16_t)find_any_value(src1);
    int16_t v2 = (int16_t)find_any_value(src2);
    if (!d_reg) { printf("IDIVW: dest must be 16-bit register (%s)\n", dest); return; }
    if (v2 == 0) { printf("IDIVW: divide by zero\n"); return; }
    *d_reg = v1 / v2;
    *dx    = v1 % v2;
}

// 8-bit signed division: dest = src1 / src2, DL = src1 % src2
void idivb_handler(const char* dest, const char* src1, const char* src2) {
    int8_t* d_reg = get_register(dest, 1);
    int8_t v1 = (int8_t)find_any_value(src1);
    int8_t v2 = (int8_t)find_any_value(src2);
    if (!d_reg) { printf("IDIVB: dest must be 8-bit register (%s)\n", dest); return; }
    if (v2 == 0) { printf("IDIVB: divide by zero\n"); return; }
    *d_reg = v1 / v2;
    *dl    = v1 % v2;
}

// 32-bit Add
void set_flags_add32(uint32_t a, uint32_t b, uint32_t result) {
    *ZF = (result == 0);
    *SF = ((int32_t)result < 0);
    *CF = (result < a); // carry-out for unsigned add
    *OF = (((a ^ result) & (b ^ result)) >> 31) & 1; // signed overflow
}

// 16-bit Add
void set_flags_add16(uint16_t a, uint16_t b, uint16_t result) {
    *ZF = (result == 0);
    *SF = ((int16_t)result < 0);
    *CF = (result < a);
    *OF = (((a ^ result) & (b ^ result)) >> 15) & 1;
}

// 8-bit Add
void set_flags_add8(uint8_t a, uint8_t b, uint8_t result) {
    *ZF = (result == 0);
    *SF = ((int8_t)result < 0);
    *CF = (result < a);
    *OF = (((a ^ result) & (b ^ result)) >> 7) & 1;
}

// 32-bit Subtract
void set_flags_sub32(uint32_t a, uint32_t b, uint32_t result) {
    *ZF = (result == 0);
    *SF = ((int32_t)result < 0);
    *CF = (a < b); // borrow
    *OF = (((a ^ b) & (a ^ result)) >> 31) & 1;
}

// 16-bit Subtract
void set_flags_sub16(uint16_t a, uint16_t b, uint16_t result) {
    *ZF = (result == 0);
    *SF = ((int16_t)result < 0);
    *CF = (a < b);
    *OF = (((a ^ b) & (a ^ result)) >> 15) & 1;
}

// 8-bit Subtract
void set_flags_sub8(uint8_t a, uint8_t b, uint8_t result) {
    *ZF = (result == 0);
    *SF = ((int8_t)result < 0);
    *CF = (a < b);
    *OF = (((a ^ b) & (a ^ result)) >> 7) & 1;
}

// Unsigned MUL Flags (ZF/SF untouched, CF/OF set)
void set_flags_mul32(uint32_t op1, uint32_t op2, uint32_t result) {
    uint64_t full = (uint64_t)op1 * (uint64_t)op2;
    if ((full >> 32) != 0) {
        *CF = 1; *OF = 1;
    } else {
        *CF = 0; *OF = 0;
    }
}

void set_flags_mul16(uint16_t op1, uint16_t op2, uint16_t result) {
    uint32_t full = (uint32_t)op1 * (uint32_t)op2;
    if ((full >> 16) != 0) {
        *CF = 1; *OF = 1;
    } else {
        *CF = 0; *OF = 0;
    }
}

void set_flags_mul8(uint8_t op1, uint8_t op2, uint8_t result) {
    uint16_t full = (uint16_t)op1 * (uint16_t)op2;
    if ((full >> 8) != 0) {
        *CF = 1; *OF = 1;
    } else {
        *CF = 0; *OF = 0;
    }
}

// Signed IMUL Flags (ZF/SF untouched, CF/OF set)
void set_flags_imul32(int32_t op1, int32_t op2, int32_t result) {
    int64_t full = (int64_t)op1 * (int64_t)op2;
    if ((full > INT32_MAX) || (full < INT32_MIN)) {
        *CF = 1; *OF = 1;
    } else {
        *CF = 0; *OF = 0;
    }
}

void set_flags_imul16(int16_t op1, int16_t op2, int16_t result) {
    int32_t full = (int32_t)op1 * (int32_t)op2;
    if ((full > INT16_MAX) || (full < INT16_MIN)) {
        *CF = 1; *OF = 1;
    } else {
        *CF = 0; *OF = 0;
    }
}

void set_flags_imul8(int8_t op1, int8_t op2, int8_t result) {
    int16_t full = (int16_t)op1 * (int16_t)op2;
    if ((full > INT8_MAX) || (full < INT8_MIN)) {
        *CF = 1; *OF = 1;
    } else {
        *CF = 0; *OF = 0;
    }
}


void jmp_handler(const char* label, const char* reg) {
    int condition = find_any_value(reg);
    int label_offset = find_label_offset(label);
    if (label_offset >= 0 && condition == 1) {
        *ip = label_offset - *cs ;
    }else if (condition == 0){
        return;
    } else {
        printf("Unknown label: %s\n", label);
    }
}
void je_handler(const char* label) {
    if (*ZF) {
        int label_offset = find_label_offset(label);
        if (label_offset >= 0)
            *ip = label_offset - *cs;
        else
            printf("Unknown label: %s\n", label);
    }
}

void jne_handler(const char* label) {
    if (!*ZF) {
        int label_offset = find_label_offset(label);
        if (label_offset >= 0)
            *ip = label_offset - *cs;
        else
            printf("Unknown label: %s\n", label);
    }
}

void jg_handler(const char* label) {
    // JG (jump if greater, signed): ZF==0 && SF==OF
    if (!*ZF && (*SF == *OF)) {
        int label_offset = find_label_offset(label);
        if (label_offset >= 0)
            *ip = label_offset - *cs;
        else
            printf("Unknown label: %s\n", label);
    }
}

void jl_handler(const char* label) {
    // JL (jump if less, signed): SF != OF
    if (*SF != *OF) {
        int label_offset = find_label_offset(label);
        if (label_offset >= 0)
            *ip = label_offset - *cs;
        else
            printf("Unknown label: %s\n", label);
    }
}

void call_handler(const char* label) {
    int label_offset = find_label_offset(label);
    if (label_offset < 0) {
        printf("Unknown label: %s\n", label);
        return;
    }
    // Push the *next* instruction's offset onto stack (where we return)
    push_int(*ip);
    *ip = label_offset - *cs;
}

void ret_handler(const char* cond) {
    int condition = find_any_value(cond);
    if (condition == 1){
        int ret_addr = pop_int();
        if (ret_addr >= 0 && ret_addr < MEMORY_SIZE) {
            *ip = ret_addr ;
        } else {
            printf("RET: invalid return address %d\n", ret_addr);
        }
    }
}

// Equality: dest = (src1 == src2) ? 1 : 0
void eq_handler(const char* dest, const char* src1, const char* src2) {
    uint8_t* d = get_register(dest, 1);
    if (!d) { printf("EQ: dest must be 1-byte reg (%s)\n", dest); return; }
    *d = (find_any_value(src1) == find_any_value(src2)) ? 1 : 0;
}

// Not Equal: dest = (src1 != src2) ? 1 : 0
void neq_handler(const char* dest, const char* src1, const char* src2) {
    uint8_t* d = get_register(dest, 1);
    if (!d) { printf("NEQ: dest must be 1-byte reg (%s)\n", dest); return; }
    *d = (find_any_value(src1) != find_any_value(src2)) ? 1 : 0;
}

// Greater Than: dest = (src1 > src2) ? 1 : 0
void gt_handler(const char* dest, const char* src1, const char* src2) {
    uint8_t* d = get_register(dest, 1);
    if (!d) { printf("GT: dest must be 1-byte reg (%s)\n", dest); return; }
    *d = ((int32_t)find_any_value(src1) > (int32_t)find_any_value(src2)) ? 1 : 0;
}

// Greater Than or Equal: dest = (src1 >= src2) ? 1 : 0
void gte_handler(const char* dest, const char* src1, const char* src2) {
    uint8_t* d = get_register(dest, 1);
    if (!d) { printf("GTE: dest must be 1-byte reg (%s)\n", dest); return; }
    *d = ((int32_t)find_any_value(src1) >= (int32_t)find_any_value(src2)) ? 1 : 0;
}

// Less Than: dest = (src1 < src2) ? 1 : 0
void ls_handler(const char* dest, const char* src1, const char* src2) {
    uint8_t* d = get_register(dest, 1);
    if (!d) { printf("LS: dest must be 1-byte reg (%s)\n", dest); return; }
    *d = ((int32_t)find_any_value(src1) < (int32_t)find_any_value(src2)) ? 1 : 0;
}

// Less Than or Equal: dest = (src1 <= src2) ? 1 : 0
void lse_handler(const char* dest, const char* src1, const char* src2) {
    uint8_t* d = get_register(dest, 1);
    if (!d) { printf("LSE: dest must be 1-byte reg (%s)\n", dest); return; }
    *d = ((int32_t)find_any_value(src1) <= (int32_t)find_any_value(src2)) ? 1 : 0;
}

/*
 * INT handler (for "INT xx")
 *
 *   INT 21h  AH=02h  print the character in DL
 *            AH=08h  read one character from stdin into AL
 *            AH=09h  print the 0-terminated string at [ESI]
 *            AH=4Bh  load and run the program whose path is at [ESI]
 *                    (the caller's registers and labels are restored when
 *                    the child halts)
 *   INT 10h  AH=00h  clear the screen
 */
void int_handler(const char* arg) {
    int int_num = 0;
    if (is_num((char*)arg)) {
        int_num = my_atoi(arg);
    } else {
        printf("INT: invalid interrupt number (%s)\n", arg);
        return;
    }

    if (int_num == 0x21) { // INT 21h
        uint8_t ah_val = *ah;  // AH is already exposed from reg_mem
        if (ah_val == 0x02) {  // Display character (DL)
            putchar(*dl);
            fflush(stdout);
        }
        else if (ah_val == 0x08) { // Get character from input, return in AL
            int ch = getchar();
            if (ch == EOF) {        // end of input: power the machine off
                power_off = 1;
                *al = '\n';
            } else {
                *al = (uint8_t)map_cr_to_lf(ch);
            }
        }
        else if (ah_val == 0x09) { // Print 0-terminated string at [ESI]
            uint32_t addr = *esi;
            if (addr >= MEMORY_SIZE) {
                printf("INT 21h/09: ESI out of bounds (0x%08X) %d \n", addr, addr);
            } else {
                while (addr < MEMORY_SIZE && memory[addr]) {
                    putchar(memory[addr++]);
                }
                fflush(stdout);
            }
        }
        else if (ah_val == 0x4B) {
            char path[512];
            if (!read_emu_cstr(*esi, path, sizeof(path)) || path[0] == '\0') {
                printf("INT 21h/4B: empty or invalid filename at [ESI]\n");
            } else {
                /* save CPU state (all named registers and v0..v29) */
                uint8_t saved_regs[REG_MEM_SIZE];
                int saved_v[V_REG_COUNT];
                memcpy(saved_regs, reg_mem, sizeof(saved_regs));
                memcpy(saved_v, reg_mem2, sizeof(saved_v));
                /* save caller labels */
                int saved_label_count = label_count;
                size_t L = sizeof(label_table);
                void* label_backup = malloc(L);
                if (!label_backup) { puts("exec: OOM backing up labels"); return; }
                memcpy(label_backup, label_table, L);

                /* clean label space for child */
                label_count = 0;               // <- critical
                // memset(label_table, 0, L);  // (optional)

                int ok = registers_initializer(path);         // runs _initial:
                if (ok != -1) {
                    int image_size = load_program_data(path); // loads code + scans labels
                    if (image_size > 0) {
                        *ip = 0;                              // <- start child from 0
                        execute_program(image_size);
                    } else {
                        printf("exec load failed: %s\n", path);
                    }
                }

                /* restore labels + CPU state */
                memcpy(label_table, label_backup, L);
                label_count = saved_label_count;
                free(label_backup);

                memcpy(reg_mem, saved_regs, sizeof(saved_regs));
                memcpy(reg_mem2, saved_v, sizeof(saved_v));
            }
        }
        else {
            printf("INT 21h: unknown function in AH = 0x%02X\n", ah_val);
            fflush(stdout);
        }
    } else if (int_num == 0x10) {                 // <<-- NEW
        uint8_t ah_val = *ah;
        if (ah_val == 0x00) {                   // AH=00: CLS
            clear_screen();
        } else {
            printf("INT 10h: unknown AH=0x%02X\n", ah_val);
        }
    } else {
        printf("INT: unknown interrupt 0x%02X\n", int_num);
        fflush(stdout);
    }
}

static int map_cr_to_lf(int ch) {
    return ch == 13 ? 10 : ch;
}

static int read_emu_cstr(uint32_t addr, char* out, size_t out_sz) {
    if (addr >= MEMORY_SIZE || out_sz == 0) return 0;
    size_t i = 0;
    while (i + 1 < out_sz && addr < MEMORY_SIZE) {
        unsigned char b = memory[addr++];
        if (b == 0 || b == 10) break;
        out[i++] = (char)b;
    }
    out[i] = '\0';
    return 1;
}
