#ifndef UNTITLED_MAIN_H
#define UNTITLED_MAIN_H
#include "stdint.h"
#endif //UNTITLED_MAIN_H
#define MEMORY_SIZE 128000 // RAM
#define MAX_LINES 500   // OS supports programs of maximum 500 lines
#define LINE_LEN 512
#define MAX_VARIABLES 1000
extern uint8_t* ZF;
extern uint8_t* SF;
extern uint8_t* CF;
extern uint8_t* OF;

FILE* open_file(char address[]);
int check(char* line, int* current_line);
char first_char(char* line);
void pre_assign(char* arg1, char* arg2);
void assign(const char *name, int value);
void not_equal( char var1[], char var2[], char var3[]);
void SHR( char var1[],char var2[],  char var3[]);
void get_input(char arg1[]);
void equal( char var1[], char var2[], char var3[]);
void is_greater( char var1[], char var2[], char var3[]);
void is_greater_equal( char var1[], char var2[], char var3[]);
void is_less_than( char var1[], char var2[], char var3[]);
void is_less_than_equal( char var1[], char var2[], char var3[]);
void SHL( char var1[],char var2[],  char var3[]);

void load_program(FILE *file);
void lstrip(char* line);
int find_label(const char *label);
void go2(const char *label, int *current_line);
void read_mem(const char* arg1);
void show_str(char* arg1);
int find_int_value(char* arg);
int find_variable_value(const char *name) ;
int find_variable_index(const char *name);
int find_mem_addr(char* arg);
int* find_var_addr(char* name);
void run_app(char* arg1, char* arg2, int* current_line);
void conditional_jmp(char* command, char* arg1, char* arg2, char* arg3, int* current_line);
void clear_screen();
void* get_register(const char* reg, int size);
void show_all_registers();




