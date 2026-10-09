#ifndef OS_H
#define OS_H

#include <stdio.h>
#include <stdint.h>

/* strcasecmp / strncasecmp live in <strings.h> on POSIX and have
 * different names on MSVC. */
#ifdef _MSC_VER
#define strcasecmp  _stricmp
#define strncasecmp _strnicmp
#else
#include <strings.h>
#endif

#define MEMORY_SIZE 1024000 /* bytes of emulated RAM */
#define LINE_LEN    512
#define REG_MEM_SIZE 52     /* 13 named 32-bit registers, see os.c */

extern uint8_t* ZF;
extern uint8_t* SF;
extern uint8_t* CF;
extern uint8_t* OF;

/* v0..v29: 30 general-purpose 32-bit "variable" registers */
#define V_REG_COUNT 30
extern int reg_mem2[V_REG_COUNT];
extern int* v0;  extern int* v1;  extern int* v2;  extern int* v3;  extern int* v4;
extern int* v5;  extern int* v6;  extern int* v7;  extern int* v8;  extern int* v9;
extern int* v10; extern int* v11; extern int* v12; extern int* v13; extern int* v14;
extern int* v15; extern int* v16; extern int* v17; extern int* v18; extern int* v19;
extern int* v20; extern int* v21; extern int* v22; extern int* v23; extern int* v24;
extern int* v25; extern int* v26; extern int* v27; extern int* v28; extern int* v29;

#define MAX_LABELS 4096

typedef struct {
    char name[64];
    int offset;
    int is_data; // 1 for data, 0 for code
} LabelEntry;

extern LabelEntry label_table[MAX_LABELS];
extern int label_count;

/* Set when the machine should power off (HLT in the boot program or
 * end of input on stdin). */
extern int power_off;

/* Value returned by parse_and_execute / execute_program when a program halts. */
#define EXEC_HALT 401

void print_os_info(void);
void os_sleep_ms(int ms);

int load_program_data(const char* path);
void load_data_segment(FILE* file);
int registers_initializer(const char* path);
uint32_t load_program_linear(FILE* file);
int execute_program(int prog_size);
int fetch_instruction(char* out, int prog_size);

void lstrip(char* line);
void rstrip(char* line);
char first_char(char* line);

void clear_screen(void);
int parse_and_execute(char* line);
void scan_labels(int prog_size);
int find_label_offset(const char* name);
int find_label_index(const char* name);

#endif
