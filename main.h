#ifndef UNTITLED_MAIN_H
#define UNTITLED_MAIN_H

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
void AND( char var1[],  char var2[], char var3[]);
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
void show(char v[]);
void load_program(FILE *file);
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
int char_to_i(char* arg);

