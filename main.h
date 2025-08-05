#ifndef MAIN_H
#define MAIN_H
#include "stdint.h"
#define MEMORY_SIZE 128000 // RAM
#define LINE_LEN 512

extern uint8_t* ZF;
extern uint8_t* SF;
extern uint8_t* CF;
extern uint8_t* OF;
extern int* v0;
extern int* v1;
extern int* v2;
extern int* v3;
extern int* v4;
extern int* v5;
extern int* v6;
extern int* v7;
extern int* v8;
extern int* v9;
extern int* v10;
extern int* v11;
extern int* v12;
extern int* v13;
extern int* v14;
extern int* v15;
extern int* v16;
extern int* v17;
extern int* v18;
extern int* v19;
extern int* v20;
extern int* v21;
extern int* v22;
extern int* v23;
extern int* v24;
extern int* v25;
extern int* v26;
extern int* v27;
extern int* v28;
extern int* v29;

#define MAX_LABELS 1024

typedef struct {
    char name[32];
    int offset;
    int is_data; // 1 for data, 0 for code
} LabelEntry;

extern LabelEntry label_table[MAX_LABELS];
extern int label_count;

int load_program_data(const char* directory);
void load_data_segment(FILE* file);
int registers_initializer (const char* directory);
uint32_t load_program_linear(FILE* file);
int execute_program(int prog_size);
int fetch_instruction(char* out, int prog_size);

FILE* open_file(char address[]);
int check(char* line, int* current_line);
char first_char(char* line);
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
int* find_var_addr(char* name);
void run_app(char* arg1, char* arg2, int* current_line);
void conditional_jmp(char* command, char* arg1, char* arg2, char* arg3, int* current_line);
void clear_screen();
int parse_and_execute(char* line);
void scan_labels(int prog_size);
int find_label_offset(const char* name);
int find_label_index(const char* name);


#endif
