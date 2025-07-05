// helpers.h
#ifndef HELPERS_H
#define HELPERS_H

#define MEMORY_SIZE 128000 // RAM
#define MAX_LINES 500   // OS supports programs of maximum 500 lines
#define LINE_LEN 512
#define MAX_VARIABLES 1000

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

void clear_flags();
void show_flags();
void push_int(int value);
int pop_int();
int is_num(char* arg);
int is_char(char* arg);
int is_mem_addr(char* arg);
int is_immediate(const char* s);
int char_to_i(char* arg);
// Generic value fetcher for any type of register
uint32_t get_any_value(const char* s);


#endif