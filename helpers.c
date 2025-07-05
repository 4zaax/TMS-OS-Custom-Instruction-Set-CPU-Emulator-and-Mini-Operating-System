#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "helpers.h"
#include "ctype.h"
#include "stdint.h"


uint32_t get_any_value(const char* s) {
    void* ptr;
    if ((ptr = get_register(s, 4))) return *((uint32_t*)ptr);
    if ((ptr = get_register(s, 2))) return *((uint16_t*)ptr);
    if ((ptr = get_register(s, 1))) return *((uint8_t*)ptr);
    if (is_immediate(s)) return find_int_value((char*)s);
    printf("Unknown operand: %s\n", s);
    return 0;
}

void show_all_registers() {
    printf("EAX: 0x%08X  EBX: 0x%08X  ECX: 0x%08X  EDX: 0x%08X\n",
           *eax, *ebx, *ecx, *edx);
    printf("ESI: 0x%08X  EDI: 0x%08X  EBP: 0x%08X  ESP: 0x%08X\n",
           *esi, *edi, *ebp, *esp);
    printf("IP : 0x%08X  CS : 0x%08X\n",
           *ip, *cs);
}

void clear_flags() {
    for (int i = 0; i < 4; ++i)
        flags_mem[i] = 0;
}

void show_flags() {
    printf("ZF: %d  SF: %d  CF: %d  OF: %d\n", *ZF, *SF, *CF, *OF);
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


int find_int_value(char* arg){
    int value;
    // AS v0 10
    if (is_num(arg))
        value = atoi(arg);
        // AS v0 's'
    else if (is_char(arg) == 1)
        value = char_to_i(arg);
    else if (is_char(arg) == 2)
        value = ' ';
        // AS v0 [100] || AS v0 [v1]
    else if (is_mem_addr(arg)){
        int addr = find_mem_addr(arg);
        value = memory[addr];
    }
        // AS v0 v1
    else printf("AS: an error occurred while assigning.\n",arg);
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

void read_mem(const char* arg1){
    int addr = find_mem_addr(arg1);
    printf("data in memory house %d : %d\n",addr,memory[addr]);
}

void show_str(char* arg1){
    int idx = find_mem_addr(arg1);
    int i;
    for (i = idx; memory[i] != 0 ;i++){
        if (idx > MEMORY_SIZE){
            printf("show_str: index our of range\n");
            return;
        }
        printf("%c", memory[i]);
    }
}

int is_num(char* arg){
    int len = strlen(arg);
    if (len == 1 && !isdigit(arg[0])){
        return 0;
    }
    char* c = arg;
    for(; isspace(*c); c++);
    if (!(isdigit(*c) || *c == '-' || *c == '+'))
        return 0;
    for(c++;*c != '\0' ;c++){
        if (!isdigit(*c))
            return 0;
    }
    return 1;
}

int is_mem_addr(char* arg){
    return arg[0] == '[' && arg[strlen(arg)-1] == ']';
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

