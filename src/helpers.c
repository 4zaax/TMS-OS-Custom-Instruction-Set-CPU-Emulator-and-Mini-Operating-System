#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "os.h"
#include "helpers.h"

void* get_register(const char* reg, int size) {
    if (size == 4) {
        if      (strcasecmp(reg, "eax") == 0) return eax;
        else if (strcasecmp(reg, "ebx") == 0) return ebx;
        else if (strcasecmp(reg, "ecx") == 0) return ecx;
        else if (strcasecmp(reg, "edx") == 0) return edx;
        else if (strcasecmp(reg, "esi") == 0) return esi;
        else if (strcasecmp(reg, "edi") == 0) return edi;
        else if (strcasecmp(reg, "ebp") == 0) return ebp;
        else if (strcasecmp(reg, "esp") == 0) return esp;
        else if (strcasecmp(reg, "ip") == 0)  return ip;
        else if (strcasecmp(reg, "cs") == 0)  return cs;
        else if (strcasecmp(reg, "ss") == 0)  return ss;
        else if (strcasecmp(reg, "ds") == 0)  return ds;
        else if (strcasecmp(reg, "hp") == 0)  return hp;
        else if (strcasecmp(reg, "v0") == 0) return v0;
        else if (strcasecmp(reg, "v1") == 0) return v1;
        else if (strcasecmp(reg, "v2") == 0) return v2;
        else if (strcasecmp(reg, "v3") == 0) return v3;
        else if (strcasecmp(reg, "v4") == 0) return v4;
        else if (strcasecmp(reg, "v5") == 0) return v5;
        else if (strcasecmp(reg, "v6") == 0) return v6;
        else if (strcasecmp(reg, "v7") == 0) return v7;
        else if (strcasecmp(reg, "v8") == 0) return v8;
        else if (strcasecmp(reg, "v9") == 0) return v9;
        else if (strcasecmp(reg, "v10") == 0) return v10;
        else if (strcasecmp(reg, "v11") == 0) return v11;
        else if (strcasecmp(reg, "v12") == 0) return v12;
        else if (strcasecmp(reg, "v13") == 0) return v13;
        else if (strcasecmp(reg, "v14") == 0) return v14;
        else if (strcasecmp(reg, "v15") == 0) return v15;
        else if (strcasecmp(reg, "v16") == 0) return v16;
        else if (strcasecmp(reg, "v17") == 0) return v17;
        else if (strcasecmp(reg, "v18") == 0) return v18;
        else if (strcasecmp(reg, "v19") == 0) return v19;
        else if (strcasecmp(reg, "v20") == 0) return v20;
        else if (strcasecmp(reg, "v21") == 0) return v21;
        else if (strcasecmp(reg, "v22") == 0) return v22;
        else if (strcasecmp(reg, "v23") == 0) return v23;
        else if (strcasecmp(reg, "v24") == 0) return v24;
        else if (strcasecmp(reg, "v25") == 0) return v25;
        else if (strcasecmp(reg, "v26") == 0) return v26;
        else if (strcasecmp(reg, "v27") == 0) return v27;
        else if (strcasecmp(reg, "v28") == 0) return v28;
        else if (strcasecmp(reg, "v29") == 0) return v29;
    }
    if (size == 2) {
        if      (strcasecmp(reg, "ax") == 0) return ax;
        else if (strcasecmp(reg, "bx") == 0) return bx;
        else if (strcasecmp(reg, "cx") == 0) return cx;
        else if (strcasecmp(reg, "dx") == 0) return dx;
    }
    if (size == 1) {
        if      (strcasecmp(reg, "al") == 0) return al;
        else if (strcasecmp(reg, "ah") == 0) return ah;
        else if (strcasecmp(reg, "bl") == 0) return bl;
        else if (strcasecmp(reg, "bh") == 0) return bh;
        else if (strcasecmp(reg, "cl") == 0) return cl;
        else if (strcasecmp(reg, "ch") == 0) return ch;
        else if (strcasecmp(reg, "dl") == 0) return dl;
        else if (strcasecmp(reg, "dh") == 0) return dh;
    }
    return NULL; // Not a known register
}

uint32_t find_any_value(const char* s) {
    void* ptr;
    if ((ptr = get_register(s, 4))) return *((uint32_t*)ptr);
    if ((ptr = get_register(s, 2))) return *((uint16_t*)ptr);
    if ((ptr = get_register(s, 1))) return *((uint8_t*)ptr);
    if (is_immediate(s)) return find_immediate_value(s);
    if (is_mem_addr(s)) return find_mem32_value(find_mem_addr(s));
    int label_idx = find_label_index(s);
    if (label_idx >= 0) {
        if (label_table[label_idx].is_data)
            return label_table[label_idx].offset; // Data label: return address (for MOV esi msg, etc.)
        else
            return label_table[label_idx].offset; // Code label: usually for JMP
    }
    printf("Unknown operand: %s\n", s);
    return 0;
}

int find_immediate_value(const char* s){
    if(is_num(s))
        return my_atoi(s);   // Use your new function here
    else if (is_char(s))
        return char_to_i(s);
    printf("find_immediate_value: invalid operand\n");
    return 0;
}

int find_mem_addr(const char* arg){
    if ((*arg) != '[') return -1;
    char* end_bracket = strchr(arg, ']');
    if(!end_bracket) return -1;
    int len = end_bracket-arg-1;
    char *index= (char*) malloc(sizeof (char) * (len+1));
    my_strcpy(index, arg+1, len);
    // New logic:
    // Check if it's a number or register/label
    if (is_num(index)) {
        int offset = atoi(index);
        free(index);
        return *ds + offset;
    } else {
        // Try to resolve as register
        void* reg_ptr = get_register(index, 4); // supports 32-bit reg, like hp, eax, etc.
        if (reg_ptr) {
            int addr = *(uint32_t*)reg_ptr;
            free(index);
            return addr;
        }
        // Try label
        int label_idx = find_label_index(index);
        if (label_idx >= 0) {
            int addr = label_table[label_idx].offset;
            free(index);
            return addr;
        }
    }
    free(index);
    printf("find_mem_addr: invalid address expression '%s'\n", arg);
    return -1;
}

int find_mem32_value(int addr) {
    if (addr < 0 || addr > MEMORY_SIZE - 4) {
        printf("find_mem32_value: invalid address\n");
        return 0;
    }
    return *((int32_t*)&memory[addr]);
}

uint8_t find_mem8_value_unsigned(int addr) {
    if (addr < 0 || addr > MEMORY_SIZE - 1) {
        printf("find_mem8_value: invalid address\n");
        return 0;
    }
    return *((uint8_t*)&memory[addr]);
}

void show_all_registers(void) {
    printf("EAX: 0x%08X  EBX: 0x%08X  ECX: 0x%08X  EDX: 0x%08X\n",
           *eax, *ebx, *ecx, *edx);
    printf("ESI: 0x%08X  EDI: 0x%08X  EBP: 0x%08X  ESP: 0x%08X\n",
           *esi, *edi, *ebp, *esp);
    printf("IP : 0x%08X  CS : 0x%08X\n",
           *ip, *cs);
}

void show_flags(void) {
    printf("ZF: %d  SF: %d  CF: %d  OF: %d\n", *ZF, *SF, *CF, *OF);
}

void clear_flags(void) {
    for (int i = 0; i < 4; ++i)
        flags_mem[i] = 0;
}

// Push a 32-bit integer onto stack (memory[] stack, managed by *esp)
void push_int(int value) {
    if (*esp < 4) {
        printf("push_int: stack overflow\n");
        return;
    }
    *esp -= 4;
    *((uint32_t*)&memory[*esp]) = (uint32_t)value;
}

// Pop a 32-bit integer from stack
int pop_int(void) {
    if (*esp > MEMORY_SIZE - 4) {
        printf("pop_int: stack underflow\n");
        return -1;
    }
    int value = *((uint32_t*)&memory[*esp]);
    *esp += 4;
    return value;
}

void lstrip(char* line){
    char* trimmed = line;
    while(isspace((unsigned char)* trimmed))
        trimmed++;
    memmove(line, trimmed, strlen(trimmed) + 1);
}

// Removes trailing whitespace (including '\r' from CRLF files) in place
void rstrip(char* line){
    size_t n = strlen(line);
    while (n && isspace((unsigned char)line[n-1]))
        line[--n] = '\0';
}

char first_char(char* line){
    char* c;
    for(c = line; ((*c == ' ') || (*c == '\t')) && *c != '\0'; c++);
    return *c;
}


int is_num(const char* arg) {
    int len = strlen(arg);
    // Check for 0x... or ...h style
    if (len > 2 && arg[0] == '0' && (arg[1] == 'x' || arg[1] == 'X')) return 1;
    if (len > 1 && (arg[len-1] == 'h' || arg[len-1] == 'H')) {
        // Must be valid hex digits before the h
        for (int i = 0; i < len-1; i++) {
            if (!isxdigit((unsigned char)arg[i])) return 0;
        }
        return 1;
    }
    // Decimal number
    // (old implementation)
    if (len == 1 && !isdigit((unsigned char)arg[0])) return 0;
    const char* c = arg;
    for(; isspace((unsigned char)*c); c++);
    if (!(isdigit((unsigned char)*c) || *c == '-' || *c == '+')) return 0;
    for(c++; *c != '\0' ; c++) {
        if (!isdigit((unsigned char)*c)) return 0;
    }
    return 1;
}

int is_mem_addr(const char* arg){
    return (arg[0] == '[' && arg[strlen(arg)-1] == ']') && (strlen(arg) > 2);
}

int is_immediate(const char* s) {
    return (is_num(s) || is_char(s));
}

int is_char(const char* arg){
    int len = strlen(arg);
    if (len == 1 && arg[0] == '\'')
        return 2;
    if (len == 3){
        if (arg[0] == '\'' && arg[2] == '\'')
            return 1;
        return 0;
    }
    else if (len == 4){
        if(arg[0] == '\'' && arg[3] == '\'' && arg[1] == '\\')
            return 1;
        return 0;
    }
    return 0;
}
int char_to_i(const char* arg){
    int len = strlen(arg);
    if (len == 3)
        return arg[1];
    else if (len == 4) {
        if (strcmp(arg, "'\\n'") == 0)
            return '\n';
        else if (strcmp(arg, "'\\t'") == 0)
            return '\t';
        else if (strcmp(arg, "'\\r'") == 0)
            return '\r';
        else if (strcmp(arg, "'\\b'") == 0)
            return '\b';
        else if (strcmp(arg, "'\\f'") == 0)
            return '\f';
        else if (strcmp(arg, "'\\v'") == 0)
            return '\v';
        else if (strcmp(arg, "'\\0'") == 0)
            return '\0';
        else if (strcmp(arg, "'\\\\'") == 0)
            return '\\';
        else if (strcmp(arg, "'\\''") == 0)
            return '\'';
    }
    printf("char_to_i: Invalid character, couldn't convert to integer.\n");
    return 0;
}

void my_strcpy(char *des, const char *src, int len){
    for (int i=0;i<len;i++) {
        *(des + i) = *(src + i);
    }
    *(des+len)='\0';
}

int my_atoi(const char* s) {
    // Handle 0x or 0X prefix (standard C hex notation)
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        return (int)strtol(s, NULL, 16);
    }
    // Handle 'h' or 'H' (assembly style: 2Ah, FFh)
    int len = strlen(s);
    if ((len > 1) && (s[len-1] == 'h' || s[len-1] == 'H')) {
        char buf[32];
        if (len-1 >= (int)sizeof(buf)) {  // too long, fallback to decimal
            return atoi(s);
        }
        memcpy(buf, s, len-1); // Copy everything except 'h'
        buf[len-1] = '\0';
        return (int)strtol(buf, NULL, 16);
    }
    // Otherwise, treat as decimal
    return atoi(s);
}

void watch_memory(unsigned char *mem, int start, int count) {
    for (int i = 0; i < count; i++) {
        int index = start + i;
        unsigned char byte = mem[index];
        printf("memory[%d] = 0x%02X (%c)\n", index, byte,
               isprint(byte) ? byte : '.');
    }
}