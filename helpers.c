#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "main.h"
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
    int label_idx = lookup_label_index(s);
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

int find_mem_addr(char* arg){
    if ((*arg) != '[') return -1;
    char* end_bracket = strchr(arg, ']');
    if(!end_bracket) return -1;
    int len = end_bracket-arg-1;
    char *index= (char*) malloc(sizeof (char) * (len+1));
    my_strcpy(index,arg+1,len);
    return atoi(index);
}

int find_mem32_value(int addr) {
    if (addr < 0 || addr > MEMORY_SIZE - 4) {
        printf("find_mem32_value: invalid address\n");
        return 0;
    }
    return *((int32_t*)&memory[addr]);
}

uint8_t find_mem8_value_unsigned(int addr) {
    if (addr < 0 || addr > MEMORY_SIZE - 4) {
        printf("find_mem32_value: invalid address\n");
        return 0;
    }
    return *((uint8_t*)&memory[addr]);
}

void show_all_registers() {
    printf("EAX: 0x%08X  EBX: 0x%08X  ECX: 0x%08X  EDX: 0x%08X\n",
           *eax, *ebx, *ecx, *edx);
    printf("ESI: 0x%08X  EDI: 0x%08X  EBP: 0x%08X  ESP: 0x%08X\n",
           *esi, *edi, *ebp, *esp);
    printf("IP : 0x%08X  CS : 0x%08X\n",
           *ip, *cs);
}

void show_flags() {
    printf("ZF: %d  SF: %d  CF: %d  OF: %d\n", *ZF, *SF, *CF, *OF);
}

void clear_flags() {
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
int pop_int() {
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
    strcpy(line, trimmed);
}

char first_char(char* line){
    char* c;
    for(c = line; ((*c == ' ') || (*c == '\t')) && *c != '\0'; c++);
    return *c;
}


//int is_num(char* arg){
//    int len = strlen(arg);
//    if (len == 1 && !isdigit(arg[0])){
//        return 0;
//    }
//    char* c = arg;
//    for(; isspace(*c); c++);
//    if (!(isdigit(*c) || *c == '-' || *c == '+'))
//        return 0;
//    for(c++;*c != '\0' ;c++){
//        if (!isdigit(*c))
//            return 0;
//    }
//    return 1;
//}

int is_num(char* arg) {
    int len = strlen(arg);
    // Check for 0x... or ...h style
    if (len > 2 && arg[0] == '0' && (arg[1] == 'x' || arg[1] == 'X')) return 1;
    if (len > 1 && (arg[len-1] == 'h' || arg[len-1] == 'H')) {
        // Must be valid hex digits before the h
        for (int i = 0; i < len-1; i++) {
            if (!isxdigit(arg[i])) return 0;
        }
        return 1;
    }
    // Decimal number
    // (old implementation)
    if (len == 1 && !isdigit(arg[0])) return 0;
    char* c = arg;
    for(; isspace(*c); c++);
    if (!(isdigit(*c) || *c == '-' || *c == '+')) return 0;
    for(c++; *c != '\0' ; c++) {
        if (!isdigit(*c)) return 0;
    }
    return 1;
}

int is_mem_addr(char* arg){
    return (arg[0] == '[' && arg[strlen(arg)-1] == ']') && (strlen(arg) > 2);
}

int is_immediate(const char* s) {
    return (is_num(s) || is_char(s));
}

    int is_char(char* arg){
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

int char_to_i(char* arg){
    int len = strlen(arg);
    if (len == 3)
        return arg[1];
    else if (len == 4)
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
        else if (strcmp(arg, "'\\'") == 0)
            return '\\';
        else if (strcmp(arg, "'\\''") == 0)
            return '\'';
        else
            printf("char_to_i: Invalid character, couldn't convert to integer.\n");
}

void my_strcpy(char *des,char *src,int len){
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
        if (len-1 >= sizeof(buf)) {  // too long, fallback to decimal
            return atoi(s);
        }
        strncpy(buf, s, len-1); // Copy everything except 'h'
        buf[len-1] = '\0';
        return (int)strtol(buf, NULL, 16);
    }
    // Otherwise, treat as decimal
    return atoi(s);
}

