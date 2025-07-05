// helpers.h
#ifndef HELPERS_H
#define HELPERS_H

#define MEMORY_SIZE 128000 // RAM
#define MAX_LINES 500   // OS supports programs of maximum 500 lines
#define LINE_LEN 512
#define MAX_VARIABLES 1000

extern uint8_t memory[];
extern uint8_t reg_mem[];
extern uint8_t flags_mem[];
extern uint8_t* ZF;
extern uint8_t* SF;
extern uint8_t* CF;
extern uint8_t* OF;
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