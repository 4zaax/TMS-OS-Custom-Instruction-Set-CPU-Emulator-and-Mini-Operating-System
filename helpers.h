// helpers.h
#ifndef HELPERS_H
#define HELPERS_H

#define MEMORY_SIZE 128000 // RAM
#define LINE_LEN 512

#include "stdint.h"

extern uint8_t memory[];
extern uint8_t reg_mem[];
extern uint8_t flags_mem[];
extern uint8_t* ZF;
extern uint8_t* SF;
extern uint8_t* CF;
extern uint8_t* OF;
#define EAX_OFFSET 0
extern uint32_t* eax;
extern uint16_t* ax ;
extern uint8_t*  al ;
extern uint8_t*  ah ;
#define EBX_OFFSET 4
extern uint32_t* ebx;
extern uint16_t* bx ;
extern uint8_t*  bl ;
extern uint8_t*  bh ;
#define ECX_OFFSET 8
extern uint32_t* ecx;
extern uint16_t* cx ;
extern uint8_t*  cl ;
extern uint8_t*  ch ;
// EDX
#define EDX_OFFSET 12
extern uint32_t* edx;
extern uint16_t* dx;
extern uint8_t*  dl;
extern uint8_t*  dh;

extern uint32_t* esi;
extern uint32_t* edi;
extern uint32_t* ebp;
extern uint32_t* esp;
extern uint32_t* ip;
extern uint32_t* cs;
extern uint32_t* ss;
extern uint32_t* ds;

void* get_register(const char* reg, int size);
// Generic value fetcher for any type of register
uint32_t find_any_value(const char* s);
int find_mem32_value(int addr);
uint8_t find_mem8_value_unsigned(int addr);
int find_immediate_value(const char* s);

// Easier Debug
void show_all_registers();
void clear_flags();
void show_flags();

// Stack
void push_int(int value);
int pop_int();

// Type Recognition
int is_num(char* arg);
int is_mem_addr(char* arg);
int is_char(char* arg);
int is_immediate(const char* s);




// General
int char_to_i(char* arg);
int find_mem_addr(char* arg);
void my_strcpy(char *des,char *src,int len);
int my_atoi(const char* s);


#endif