#ifndef UNTITLED_MAIN_H
#define UNTITLED_MAIN_H
#include "stdint.h"
#endif //UNTITLED_MAIN_H
FILE* open_file(char address[]);
int check(char* line, int* current_line);
char first_char(char* line);
void pre_assign(char* arg1, char* arg2);
void assign(const char *name, int value);
void not_equal( char var1[], char var2[], char var3[]);
void SHR( char var1[],char var2[],  char var3[]);
void get_input(char arg1[]);
void equal( char var1[], char var2[], char var3[]);
void set_char_var(const char *name, char value);
int find_char_var(const char *name);
void is_greater( char var1[], char var2[], char var3[]);
void is_greater_equal( char var1[], char var2[], char var3[]);
void is_less_than( char var1[], char var2[], char var3[]);
void is_less_than_equal( char var1[], char var2[], char var3[]);
void SHL( char var1[],char var2[],  char var3[]);

// Bitwise
void and_handler(const char*, const char*, const char*);
void andw_handler(const char*, const char*, const char*);
void andb_handler(const char*, const char*, const char*);

void or_handler(const char*, const char*, const char*);
void orw_handler(const char*, const char*, const char*);
void orb_handler(const char*, const char*, const char*);

void xor_handler(const char*, const char*, const char*);
void xorw_handler(const char*, const char*, const char*);
void xorb_handler(const char*, const char*, const char*);

void not_handler(const char*, const char*);
void notw_handler(const char*, const char*);
void notb_handler(const char*, const char*);

// Shifts
void shl_handler(const char*, const char*, const char*);
void shlw_handler(const char*, const char*, const char*);
void shlb_handler(const char*, const char*, const char*);

void shr_handler(const char*, const char*, const char*);
void shrw_handler(const char*, const char*, const char*);
void shrb_handler(const char*, const char*, const char*);


void OR( char var1[],  char var2[], char var3[]);
void XOR( char var1[],  char var2[], char var3[]);
void NOT( char var1[], char var2[]);
void increment(char arg3[]);
void decrement(char arg3[]);
void equal_test(char var1[], char value[], char destination[]);
void addition(char var1[], char var2[], char destination[]);
void subtract(char var1[], char var2[], char destination[]);
void multiply(char var1[], char var2[], char destination[]);
void divide(char var1[], char var2[], char destination[]);
void modulo(char var1[], char var2[], char destination[]);
void showd(const char* reg);
void showw(const char* reg);
void showb(const char* reg);
void showc(const char* reg);
void show_all_registers();
void showd_mem(int addr);
void showw_mem(int addr);
void showb_mem(int addr);
void mov_handler (char* dest, char* src);
void movb_handler(char* dest, char* src);
void movw_handler(char* dest, char* src);

// Generic value fetcher
uint32_t get_any_value(const char* s);

// ADD: dword/word/byte
void add_handler(const char* dest, const char* src1, const char* src2);    // 32-bit (dword)
void addw_handler(const char* dest, const char* src1, const char* src2);   // 16-bit (word)
void addb_handler(const char* dest, const char* src1, const char* src2);   // 8-bit  (byte)

// SUB: dword/word/byte
void sub_handler(const char* dest, const char* src1, const char* src2);    // 32-bit (dword)
void subw_handler(const char* dest, const char* src1, const char* src2);   // 16-bit (word)
void subb_handler(const char* dest, const char* src1, const char* src2);   // 8-bit  (byte)

// Unsigned multiplication handlers
void mul_handler(const char* dest, const char* src1, const char* src2);    // 32-bit
void mulw_handler(const char* dest, const char* src1, const char* src2);   // 16-bit
void mulb_handler(const char* dest, const char* src1, const char* src2);   // 8-bit

// Signed multiplication handlers
void imul_handler(const char* dest, const char* src1, const char* src2);   // 32-bit
void imulw_handler(const char* dest, const char* src1, const char* src2);  // 16-bit
void imulb_handler(const char* dest, const char* src1, const char* src2);  // 8-bit

// Unsigned division
void div_handler(const char* dest, const char* src1, const char* src2);    // 32-bit
void divw_handler(const char* dest, const char* src1, const char* src2);   // 16-bit
void divb_handler(const char* dest, const char* src1, const char* src2);   // 8-bit

// Signed division
void idiv_handler(const char* dest, const char* src1, const char* src2);   // 32-bit
void idivw_handler(const char* dest, const char* src1, const char* src2);  // 16-bit
void idivb_handler(const char* dest, const char* src1, const char* src2);  // 8-bit



void load_program(FILE *file);
void lstrip(char* line);
void push(int val);
int pop();
int find_label(const char *label);
void go2(const char *label, int *current_line);
void user_push(char var[]);
void user_pop(char var[]);
void move(char* dest, char* src);
void read_mem(const char* arg1);
void show_str(char* arg1);
void if_command(char* arg1,char* arg2, int* current_line);
void if_not_command(char* arg1,char* arg2, int* current_line);
int find_int_value(char* arg);
int find_variable_value(const char *name) ;
int find_variable_index(const char *name);
int find_mem_addr(char* arg);
int* find_var_addr(char* name);
int is_num(char* arg);
int is_char(char* arg);
int is_mem_addr(char* arg);
int is_immediate(const char* s);
int char_to_i(char* arg);
void run_app(char* arg1, char* arg2, int* current_line);
void conditional_jmp(char* command, char* arg1, char* arg2, char* arg3, int* current_line);
void clear_screen();
void* get_register(const char* reg, int size);