#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ctype.h"
#include "stdint.h"
#include "main.h"
#include "helpers.h"
#include "handlers.h"

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

int find_variable_index(const char *name) {
    for (int i = 0; i < variable_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
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
    if (r) printf("%s = 0x%02X (%u)\n", reg, *r, *r);
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
    if (r) printf("%s = 0x%04X (%d)\n", reg, *r, *r);
    else printf("showsw: %s is not a word register\n", reg);
}

// Show signed byte (8 bits)
void showsb(const char* reg) {
    int8_t* r = get_register(reg, 1);
    if (r) printf("%s = 0x%02X (%d)\n", reg, *r, *r);
    else printf("showsb: %s is not a byte register\n", reg);
}

void showc(const char* reg) {
    uint8_t* r = get_register(reg, 1);
    if (r) printf("%s = 0x%02X (%c)\n", reg, *r, *r);
    else printf("showc: %s is not a character register\n", reg);
}

// Unsigned
void showd_mem(int addr) {
    printf("MEM[%d] = 0x%08X (%u)\n", addr, *((uint32_t*)&memory[addr]), *((uint32_t*)&memory[addr]));
}
void showw_mem(int addr) {
    printf("MEM[%d] = 0x%04X (%u)\n", addr, *((uint16_t*)&memory[addr]), *((uint16_t*)&memory[addr]));
}
void showb_mem(int addr) {
    printf("MEM[%d] = 0x%02X (%u)\n", addr, memory[addr], memory[addr]);
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
    uint32_t v1 = get_any_value(src1);
    uint32_t v2 = get_any_value(src2);
    *d_reg = v1 & v2;
}

void andw_handler(const char* dest, const char* src1, const char* src2) {
    uint16_t* d_reg = get_register(dest, 2);
    if (!d_reg) { printf("ANDW: dest must be 16-bit register (%s)\n", dest); return; }
    uint16_t v1 = (uint16_t)get_any_value(src1);
    uint16_t v2 = (uint16_t)get_any_value(src2);
    *d_reg = v1 & v2;
}

void andb_handler(const char* dest, const char* src1, const char* src2) {
    uint8_t* d_reg = get_register(dest, 1);
    if (!d_reg) { printf("ANDB: dest must be 8-bit register (%s)\n", dest); return; }
    uint8_t v1 = (uint8_t)get_any_value(src1);
    uint8_t v2 = (uint8_t)get_any_value(src2);
    *d_reg = v1 & v2;
}

// OR
void or_handler(const char* dest, const char* src1, const char* src2) {
    uint32_t* d = get_register(dest, 4);
    if (!d) { printf("OR: dest must be 32-bit reg\n"); return; }
    *d = get_any_value(src1) | get_any_value(src2);
}
void orw_handler(const char* d, const char* s1, const char* s2) {
    uint16_t* r = get_register(d, 2);
    if (!r) { printf("ORW: dest must be 16-bit reg\n"); return; }
    *r = (uint16_t)get_any_value(s1) | (uint16_t)get_any_value(s2);
}
void orb_handler(const char* d, const char* s1, const char* s2) {
    uint8_t* r = get_register(d, 1);
    if (!r) { printf("ORB: dest must be 8-bit reg\n"); return; }
    *r = (uint8_t)get_any_value(s1) | (uint8_t)get_any_value(s2);
}

// XOR
void xor_handler(const char* dest, const char* src1, const char* src2) {
    uint32_t* d = get_register(dest, 4);
    if (!d) { printf("XOR: dest must be 32-bit reg\n"); return; }
    *d = get_any_value(src1) ^ get_any_value(src2);
}
void xorw_handler(const char* d, const char* s1, const char* s2) {
    uint16_t* r = get_register(d, 2);
    if (!r) { printf("XORW: dest must be 16-bit reg\n"); return; }
    *r = (uint16_t)get_any_value(s1) ^ (uint16_t)get_any_value(s2);
}
void xorb_handler(const char* d, const char* s1, const char* s2) {
    uint8_t* r = get_register(d, 1);
    if (!r) { printf("XORB: dest must be 8-bit reg\n"); return; }
    *r = (uint8_t)get_any_value(s1) ^ (uint8_t)get_any_value(s2);
}
void not_handler(const char* dest, const char* src) {
    uint32_t* d = get_register(dest, 4);
    if (!d) { printf("NOT: dest must be 32-bit reg\n"); return; }
    *d = ~get_any_value(src);
}
void notw_handler(const char* dest, const char* src) {
    uint16_t* d = get_register(dest, 2);
    if (!d) { printf("NOTW: dest must be 16-bit reg\n"); return; }
    *d = ~(uint16_t)get_any_value(src);
}
void notb_handler(const char* dest, const char* src) {
    uint8_t* d = get_register(dest, 1);
    if (!d) { printf("NOTB: dest must be 8-bit reg\n"); return; }
    *d = ~(uint8_t)get_any_value(src);
}
void shl_handler(const char* dest, const char* src1, const char* src2) {
    uint32_t* d = get_register(dest, 4);
    if (!d) { printf("SHL: dest must be 32-bit reg\n"); return; }
    *d = get_any_value(src1) << get_any_value(src2);
}
void shlw_handler(const char* d, const char* s1, const char* s2) {
    uint16_t* r = get_register(d, 2);
    if (!r) { printf("SHLW: dest must be 16-bit reg\n"); return; }
    *r = (uint16_t)get_any_value(s1) << (uint16_t)get_any_value(s2);
}
void shlb_handler(const char* d, const char* s1, const char* s2) {
    uint8_t* r = get_register(d, 1);
    if (!r) { printf("SHLB: dest must be 8-bit reg\n"); return; }
    *r = (uint8_t)get_any_value(s1) << (uint8_t)get_any_value(s2);
}
void shr_handler(const char* dest, const char* src1, const char* src2) {
    uint32_t* d = get_register(dest, 4);
    if (!d) { printf("SHR: dest must be 32-bit reg\n"); return; }
    *d = get_any_value(src1) >> get_any_value(src2);
}
void shrw_handler(const char* d, const char* s1, const char* s2) {
    uint16_t* r = get_register(d, 2);
    if (!r) { printf("SHRW: dest must be 16-bit reg\n"); return; }
    *r = (uint16_t)get_any_value(s1) >> (uint16_t)get_any_value(s2);
}
void shrb_handler(const char* d, const char* s1, const char* s2) {
    uint8_t* r = get_register(d, 1);
    if (!r) { printf("SHRB: dest must be 8-bit reg\n"); return; }
    *r = (uint8_t)get_any_value(s1) >> (uint8_t)get_any_value(s2);
}

void my_strcpy(char *des,char * sourc,int len){
    for (int i=0;i<len;i++) {
        *(des + i) = *(sourc + i);
    }
    *(des+len)='\0';
}
int find_mem_addr(char* arg){
    if ((*arg) != '[') return -1;
    char* end_bracket = strchr(arg, ']');
    if(!end_bracket) return -1;
    int len = end_bracket-arg-1;
    int index;
    char *c= (char*) malloc(sizeof (char) * (len+1));
    my_strcpy(c,arg+1,len);

    if (is_num(c))
        index= atoi(c);
    else
        index = find_variable_value(c);
    return index;
}

// PUSH dword (4 bytes)
void push_handler(const char* src) {
    uint32_t val = get_any_value(src);
    if (*esp < 4) { printf("PUSH: stack overflow\n"); return; }
    *esp -= 4;
    *((uint32_t*)&memory[*esp]) = val;
}

// PUSH word (2 bytes)
void pushw_handler(const char* src) {
    uint16_t val = (uint16_t)get_any_value(src);
    if (*esp < 2) { printf("PUSHW: stack overflow\n"); return; }
    *esp -= 2;
    *((uint16_t*)&memory[*esp]) = val;
}

// PUSH byte (1 byte)
void pushb_handler(const char* src) {
    uint8_t val = (uint8_t)get_any_value(src);
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

    // Case 3: dest is 4bytes register, src is memory
    if (d_reg && is_mem_addr(src)) {
        int addr = find_mem_addr(src);
        *d_reg = *((uint32_t*)&memory[addr]);
        return;
    }
    // Case 4: dest is memory, src is 4byte register
    if (is_mem_addr(dest) && s_reg) {
        int addr = find_mem_addr(dest);
        *((uint32_t*)&memory[addr]) = *s_reg;
        return;
    }
    // Case 5: immediate to 4 bytes register
    if (d_reg && is_immediate(src)) {

        *d_reg = find_int_value(src);
        return;
    }
    // Case 6: immediate to memory
    if (is_mem_addr(dest) && is_immediate(src)) {
        int addr = find_mem_addr(dest);
        *((uint32_t*)&memory[addr]) = find_int_value(src);
        return;
    }
    // Register to Register
    if (d_reg && s_reg) {
        *d_reg = *s_reg;
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
        *d_reg = memory[addr];
        return;
    }
    // dest is memory, src is register
    if (is_mem_addr((char*)dest) && s_reg) {
        int addr = find_mem_addr(dest);
        memory[addr] = *s_reg;
        return;
    }
    // immediate to register
    if (d_reg && is_immediate(src)) {
        *d_reg = (uint8_t)find_int_value(src);
        return;
    }
    // immediate to memory
    if (is_mem_addr(dest) && is_immediate(src)) {
        int addr = find_mem_addr(dest);
        memory[addr] = (uint8_t)find_int_value(src);
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
        *d_reg = *((uint16_t*)&memory[addr]);
        return;
    }
    // dest is memory, src is register
    if (is_mem_addr(dest) && s_reg) {
        int addr = find_mem_addr((char*)dest);
        *((uint16_t*)&memory[addr]) = *s_reg;
        return;
    }
    // immediate to register
    if (d_reg && is_immediate(src)) {
        *d_reg = (uint16_t)find_int_value((char*)src);
        return;
    }
    // immediate to memory
    if (is_mem_addr((char*)dest) && is_immediate(src)) {
        int addr = find_mem_addr((char*)dest);
        *((uint16_t*)&memory[addr]) = (uint16_t)find_int_value((char*)src);
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
    uint32_t val1 = get_any_value(src1);
    uint32_t val2 = get_any_value(src2);
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
    uint16_t val1 = (uint16_t)get_any_value(src1);
    uint16_t val2 = (uint16_t)get_any_value(src2);
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
    uint8_t val1 = (uint8_t)get_any_value(src1);
    uint8_t val2 = (uint8_t)get_any_value(src2);
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
    uint32_t val1 = get_any_value(src1);
    uint32_t val2 = get_any_value(src2);
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
    uint16_t val1 = (uint16_t)get_any_value(src1);
    uint16_t val2 = (uint16_t)get_any_value(src2);
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
    uint8_t val1 = (uint8_t)get_any_value(src1);
    uint8_t val2 = (uint8_t)get_any_value(src2);
    uint8_t res = val1 - val2;
    *d_reg = res;
    set_flags_sub8(val1, val2, res);
}

// Unsigned Multiplication
void mul_handler(const char* dest, const char* src1, const char* src2) {
    uint32_t* d_reg = get_register(dest, 4);
    if (!d_reg) { printf("MUL: dest must be 32-bit register\n"); return; }
    uint32_t v1 = (uint32_t)get_any_value(src1);
    uint32_t v2 = (uint32_t)get_any_value(src2);
    uint64_t result = (uint64_t)v1 * (uint64_t)v2;
    *d_reg = (uint32_t)result;
    set_flags_mul32(v1, v2, (uint32_t)result);
}

void mulw_handler(const char* dest, const char* src1, const char* src2) {
    uint16_t* d_reg = get_register(dest, 2);
    if (!d_reg) { printf("MULW: dest must be 16-bit register\n"); return; }
    uint16_t v1 = (uint16_t)get_any_value(src1);
    uint16_t v2 = (uint16_t)get_any_value(src2);
    uint32_t result = (uint32_t)v1 * (uint32_t)v2;
    *d_reg = (uint16_t)result;
    set_flags_mul16(v1, v2, (uint16_t)result);
}

void mulb_handler(const char* dest, const char* src1, const char* src2) {
    uint8_t* d_reg = get_register(dest, 1);
    if (!d_reg) { printf("MULB: dest must be 8-bit register\n"); return; }
    uint8_t v1 = (uint8_t)get_any_value(src1);
    uint8_t v2 = (uint8_t)get_any_value(src2);
    uint16_t result = (uint16_t)v1 * (uint16_t)v2;
    *d_reg = (uint8_t)result;
    set_flags_mul8(v1, v2, (uint8_t)result);
}

// Signed multiply
void imul_handler(const char* dest, const char* src1, const char* src2) {
    int32_t* d_reg = get_register(dest, 4);
    if (!d_reg) { printf("IMUL: dest must be 32-bit register\n"); return; }
    int32_t v1 = (int32_t)get_any_value(src1);
    int32_t v2 = (int32_t)get_any_value(src2);
    int64_t result = (int64_t)v1 * (int64_t)v2;
    *d_reg = (int32_t)result;
    set_flags_imul32(v1, v2, (int32_t)result);
}

void imulw_handler(const char* dest, const char* src1, const char* src2) {
    int16_t* d_reg = get_register(dest, 2);
    if (!d_reg) { printf("IMULW: dest must be 16-bit register\n"); return; }
    int16_t v1 = (int16_t)get_any_value(src1);
    int16_t v2 = (int16_t)get_any_value(src2);
    int32_t result = (int32_t)v1 * (int32_t)v2;
    *d_reg = (int16_t)result;
    set_flags_imul16(v1, v2, (int16_t)result);
}

void imulb_handler(const char* dest, const char* src1, const char* src2) {
    int8_t* d_reg = get_register(dest, 1);
    if (!d_reg) { printf("IMULB: dest must be 8-bit register\n"); return; }
    int8_t v1 = (int8_t)get_any_value(src1);
    int8_t v2 = (int8_t)get_any_value(src2);
    int16_t result = (int16_t)v1 * (int16_t)v2;
    *d_reg = (int8_t)result;
    set_flags_imul8(v1, v2, (int8_t)result);
}

// 32-bit unsigned division: dest = src1 / src2, EDX = src1 % src2
void div_handler(const char* dest, const char* src1, const char* src2) {
    uint32_t* d_reg = get_register(dest, 4);
    uint32_t v1 = get_any_value(src1);
    uint32_t v2 = get_any_value(src2);
    if (!d_reg) { printf("DIV: dest must be 32-bit register (%s)\n", dest); return; }
    if (v2 == 0) { printf("DIV: divide by zero\n"); return; }
    *d_reg = v1 / v2;
    *edx   = v1 % v2;
}

// 16-bit unsigned division: dest = src1 / src2, DX = src1 % src2
void divw_handler(const char* dest, const char* src1, const char* src2) {
    uint16_t* d_reg = get_register(dest, 2);
    uint16_t v1 = (uint16_t)get_any_value(src1);
    uint16_t v2 = (uint16_t)get_any_value(src2);
    if (!d_reg) { printf("DIVW: dest must be 16-bit register (%s)\n", dest); return; }
    if (v2 == 0) { printf("DIVW: divide by zero\n"); return; }
    *d_reg = v1 / v2;
    *dx    = v1 % v2;
}

// 8-bit unsigned division: dest = src1 / src2, DL = src1 % src2
void divb_handler(const char* dest, const char* src1, const char* src2) {
    uint8_t* d_reg = get_register(dest, 1);
    uint8_t v1 = (uint8_t)get_any_value(src1);
    uint8_t v2 = (uint8_t)get_any_value(src2);
    if (!d_reg) { printf("DIVB: dest must be 8-bit register (%s)\n", dest); return; }
    if (v2 == 0) { printf("DIVB: divide by zero\n"); return; }
    *d_reg = v1 / v2;
    *dl    = v1 % v2;
}

// 32-bit signed division: dest = src1 / src2, EDX = src1 % src2
void idiv_handler(const char* dest, const char* src1, const char* src2) {
    int32_t* d_reg = get_register(dest, 4);
    int32_t v1 = (int32_t)get_any_value(src1);
    int32_t v2 = (int32_t)get_any_value(src2);
    if (!d_reg) { printf("IDIV: dest must be 32-bit register (%s)\n", dest); return; }
    if (v2 == 0) { printf("IDIV: divide by zero\n"); return; }
    *d_reg = v1 / v2;
    *edx   = v1 % v2;
}

// 16-bit signed division: dest = src1 / src2, DX = src1 % src2
void idivw_handler(const char* dest, const char* src1, const char* src2) {
    int16_t* d_reg = get_register(dest, 2);
    int16_t v1 = (int16_t)get_any_value(src1);
    int16_t v2 = (int16_t)get_any_value(src2);
    if (!d_reg) { printf("IDIVW: dest must be 16-bit register (%s)\n", dest); return; }
    if (v2 == 0) { printf("IDIVW: divide by zero\n"); return; }
    *d_reg = v1 / v2;
    *dx    = v1 % v2;
}

// 8-bit signed division: dest = src1 / src2, DL = src1 % src2
void idivb_handler(const char* dest, const char* src1, const char* src2) {
    int8_t* d_reg = get_register(dest, 1);
    int8_t v1 = (int8_t)get_any_value(src1);
    int8_t v2 = (int8_t)get_any_value(src2);
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

void je_handler(const char* label, int* current_line) {
    if (*ZF) go2(label, current_line);
}
void jne_handler(const char* label, int* current_line) {
    if (!*ZF) go2(label, current_line);
}
void jg_handler(const char* label, int* current_line) {
    if (!*ZF && !*SF) go2(label, current_line); // Not zero and not negative
}
void jl_handler(const char* label, int* current_line) {
    if (*SF) go2(label, current_line); // Sign flag set
}

void call_handler(const char* label, int* current_line) {
    push_int(*current_line + 1); // Save return point (pushes on stack!)
    go2(label, current_line);
}

void ret_handler(int* current_line) {
    int ret_line = pop_int(); // Pop the return address (should match your stack implementation)
    if (ret_line > 0)
        *current_line = ret_line - 1; // Will be incremented after the loop
}



