#ifndef UNTITLED_MAIN_H
#define UNTITLED_MAIN_H

#endif //UNTITLED_MAIN_H
FILE* open_file(char address[]);
int check(char* line, int* current_line);
char first_char(char* line);
int get_var_id(char variable[]);
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
void NOT( char var1[]);
int find_variable_value(const char *name) ;
void increment(char arg3[]);
void decrement(char arg3[]);
int find_variable_index(const char *name);
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
