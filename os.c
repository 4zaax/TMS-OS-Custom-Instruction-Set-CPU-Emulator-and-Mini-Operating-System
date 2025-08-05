#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "main.h"
#include "stdint.h"
#include "helpers.h"
#include "handlers.h"
#include <windows.h>

#define MEMORY_SIZE 128000 // RAM
#define LINE_LEN 512
#define SKIP_LINE_COUNT 5

uint8_t memory[MEMORY_SIZE];
uint8_t reg_mem[48]; // 40 bytes (10 * 4 bytes) -> 10 extended registers
int reg_mem2[30];
uint8_t flags_mem[4]; // ZF, SF, CF, OF (one byte each)

LabelEntry label_table[MAX_LABELS];
int label_count = 0;
int terminal_size = 0;
////////////////////////////////////////////////////////////////
//////////////////       setting flags       ///////////////////
////////////////////////////////////////////////////////////////
uint8_t* ZF = &flags_mem[0];
uint8_t* SF = &flags_mem[1];
uint8_t* CF = &flags_mem[2];
uint8_t* OF = &flags_mem[3];


////////////////////////////////////////////////////////////////
////////////////// GENERAL PURPOSE REGISTERS ///////////////////
////////////////////////////////////////////////////////////////

// EAX
#define EAX_OFFSET 0
uint32_t* eax = (uint32_t*)&reg_mem[EAX_OFFSET];
uint16_t* ax  = (uint16_t*)&reg_mem[EAX_OFFSET];
uint8_t*  al  = (uint8_t*)&reg_mem[EAX_OFFSET];
uint8_t*  ah  = (uint8_t*)&reg_mem[EAX_OFFSET + 1];

// EBX
#define EBX_OFFSET 4
uint32_t* ebx = (uint32_t*)&reg_mem[EBX_OFFSET];
uint16_t* bx  = (uint16_t*)&reg_mem[EBX_OFFSET];
uint8_t*  bl  = (uint8_t*)&reg_mem[EBX_OFFSET];
uint8_t*  bh  = (uint8_t*)&reg_mem[EBX_OFFSET + 1];

// ECX
#define ECX_OFFSET 8
uint32_t* ecx = (uint32_t*)&reg_mem[ECX_OFFSET];
uint16_t* cx  = (uint16_t*)&reg_mem[ECX_OFFSET];
uint8_t*  cl  = (uint8_t*)&reg_mem[ECX_OFFSET];
uint8_t*  ch  = (uint8_t*)&reg_mem[ECX_OFFSET + 1];

// EDX
#define EDX_OFFSET 12
uint32_t* edx = (uint32_t*)&reg_mem[EDX_OFFSET];
uint16_t* dx  = (uint16_t*)&reg_mem[EDX_OFFSET];
uint8_t*  dl  = (uint8_t*)&reg_mem[EDX_OFFSET];
uint8_t*  dh  = (uint8_t*)&reg_mem[EDX_OFFSET + 1];


uint32_t* esi = (uint32_t*)&reg_mem[16];
uint32_t* edi = (uint32_t*)&reg_mem[20];
uint32_t* ebp = (uint32_t*)&reg_mem[24];
uint32_t* esp = (uint32_t*)&reg_mem[28];
uint32_t* ip = (uint32_t*)&reg_mem[32];
uint32_t* cs = (uint32_t*)&reg_mem[36];
uint32_t* ss  = (uint32_t*)&reg_mem[40];
uint32_t* ds  = (uint32_t*)&reg_mem[44];

int* v0 = reg_mem2 + 0;
int* v1 = reg_mem2 + 1;
int* v2 = reg_mem2 + 2;
int* v3 = reg_mem2 + 3;
int* v4 = reg_mem2 + 4;
int* v5 = reg_mem2 + 5;
int* v6 = reg_mem2 + 6;
int* v7 = reg_mem2 + 7;
int* v8 = reg_mem2 + 8;
int* v9 = reg_mem2 + 9;
int* v10 = reg_mem2 + 10;
int* v11 = reg_mem2 + 11;
int* v12 = reg_mem2 + 12;
int* v13 = reg_mem2 + 13;
int* v14 = reg_mem2 + 14;
int* v15 = reg_mem2 + 15;
int* v16 = reg_mem2 + 16;
int* v17 = reg_mem2 + 17;
int* v18 = reg_mem2 + 18;
int* v19 = reg_mem2 + 19;
int* v20 = reg_mem2 + 20;
int* v21 = reg_mem2 + 21;
int* v22 = reg_mem2 + 22;
int* v23 = reg_mem2 + 23;
int* v24 = reg_mem2 + 24;
int* v25 = reg_mem2 + 25;
int* v26 = reg_mem2 + 26;
int* v27 = reg_mem2 + 27;
int* v28 = reg_mem2 + 28;
int* v29 = reg_mem2 + 29;


int load_program_data(const char* directory){

    FILE* f = fopen(directory, "r");
    if (!f) { puts("Program not found"); return 1; }


    // 2. Now we should load data segment
    // NOTE : !!!! : make sure initializer has assigned correct value to ds
    load_data_segment(f);
    // 3. Load code segment (rewind file)
    uint32_t prog_size = load_program_linear(f);
    fclose(f);
    scan_labels(prog_size);
}

int registers_initializer (const char* directory) {
    FILE* file = fopen(directory, "r");
    if (!file) fprintf(stderr, "register_initializer: file not found.\n");

    char buffer[LINE_LEN];
    int flag_initializer_validation = 0;
    rewind(file);

    while(fgets(buffer, sizeof (buffer), file) != NULL){

        lstrip(buffer); // skips white spaces

        if((strcasecmp(buffer, "_initial:") == 0) || (strcasecmp(buffer, "_initial:\n") == 0)){
            flag_initializer_validation = 1;
            for (int i = 0; i < 4; i++){
                fgets(buffer, sizeof (buffer), file) != NULL;
                int execution_status = parse_and_execute(buffer);
            }
            *ip = *cs;
        }

        if (flag_initializer_validation){
            return 0;
        } else {
            MessageBox(NULL, "Program should be started with a register initializer label.", "Error", MB_ICONERROR | MB_OK);
            return -1;
        }
    }

}

void load_data_segment(FILE* file) {
    char buffer[LINE_LEN];

    rewind(file); // Rewind to start
    int data_ptr = 0; // Offset from ds

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        // Remove leading whitespace
        lstrip(buffer);

        // Check for label: look for 'label: db'
        char label[32], cmd[8], rest[LINE_LEN];
        if (sscanf(buffer, "%31[^:]: %7s %[^\n]", label, cmd, rest) == 3 && strcasecmp(cmd, "db") == 0) {
            int addr = *ds + data_ptr;

            // Add label to label_table with is_data=1
            if (label_count < MAX_LABELS) {
                strncpy(label_table[label_count].name, label, 31);
                label_table[label_count].name[31] = 0;
                label_table[label_count].offset = addr;
                label_table[label_count].is_data = 1;
                label_count++;
            }

            // Parse the rest for string and/or byte values
            char* p = rest;
            while (*p) {
                // String literal
                if (*p == '"') {
                    p++;
                    while (*p && *p != '"') {
                        memory[addr++] = *p++;
                        data_ptr++;
                    }
                    if (*p == '"') p++;
                }
                    // Comma or whitespace
                else if (*p == ',' || isspace((unsigned char)*p)) {
                    p++;
                }
                    // Numeric or char literal
                else if (*p == '\'') {
                    // Char literal e.g. 'A'
                    p++;
                    memory[addr++] = *p++;
                    data_ptr++;
                    if (*p == '\'') p++;
                }
                else if (isdigit((unsigned char)*p) || *p == '-') {
                    int val;
                    int n = sscanf(p, "%d", &val);
                    memory[addr++] = (uint8_t)val;
                    data_ptr++;
                    // Advance past the number
                    while (*p && (isdigit((unsigned char)*p) || *p == '-')) p++;
                }
                else {
                    // Unknown character, skip
                    p++;
                }
            }
        }
    }
}

// Loader: load program from file into memory at given *cs
// returns program size
uint32_t load_program_linear(FILE* file) {
    char buffer[LINE_LEN];
    int offset = 0;
    rewind(file);
    // skips initializer lines
    for(int i = 0; i < SKIP_LINE_COUNT; i++) {
        fgets(buffer, sizeof(buffer), file);
    }

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        // Check if this is a data definition line
        char label[32], cmd[8];
        if (sscanf(buffer, "%31[^:]: %7s", label, cmd) == 2 && strcasecmp(cmd, "db") == 0) {
            continue; // skip db lines in code segment
        }
        int len = strlen(buffer);
        memcpy(&memory[*cs + offset], buffer, len);
        offset += len;
    }
    return offset;
}

// Scans loaded program in memory, builds label table
void scan_labels(int prog_size) {
    int line_start = *cs;
    char line[LINE_LEN];

    while (line_start < prog_size + *cs) {
        // Read one line from memory
        int i = 0;
        while ((line_start + i < prog_size + *cs) && memory[line_start + i] != '\n') {
            line[i] = memory[line_start + i];
            i++;
        }
        line[i] = '\0';

        // Trim leading spaces
        lstrip(line);

        // Check if line is a label (ends with ':')
        int len = strlen(line);
        if (len > 1 && line[len-1] == ':') {
            if (label_count < MAX_LABELS) {
                strncpy(label_table[label_count].name, line, len-1);
                label_table[label_count].name[len-1] = '\0';  // Remove ':'
                label_table[label_count].offset = line_start;
                label_table[label_count].is_data = 0;
                label_count++;
            }
        }

        // Move to next line (+1 for newline if present)
        line_start += i + (memory[line_start + i] == '\n' ? 1 : 0);
    }
}

int execute_program(int prog_size) {
    char line[LINE_LEN];
    while (*ip < MEMORY_SIZE) {
        int current_line_len = fetch_instruction(line, prog_size);
        *ip += current_line_len ? current_line_len : 1; // now ip is ready to execute next instruction
        int execution_status =  parse_and_execute(line);
        if (execution_status == 401) return 401; // shuts down OS
    }
}

// Reads a line from memory at *ip, returns line length
int fetch_instruction(char* out, int prog_size) {
    int i = 0;
    while ((*ip + i < *cs + prog_size) && (memory[*ip + i] != '\n')) {
        out[i] = memory[*ip + i];
        i++;
    }
    out[i] = '\0';
    return i + (memory[*ip + i] == '\n' ? 1 : 0); // include newline if present
}




int main() {
    memset(reg_mem, 0, sizeof(reg_mem));
    memset(flags_mem, 0, sizeof(flags_mem));
    memset(memory, 0, sizeof(memory));

    // Running Terminal
    // Program should initialize registers at the beginning
    char* directory = "../Apps/terminal.txt";
    int reg_init_res = registers_initializer(directory);
    if (reg_init_res == -1) {
        fprintf(stderr, "There was an error finding your initializer label in program.\n");
        exit(1);}
    // now we write program into the ram :
    terminal_size = load_program_data(directory);

    // Loading second app
    directory = "../Apps/program2.txt";

    reg_init_res = registers_initializer(directory);
    if (reg_init_res == -1) {
        fprintf(stderr, "There was an error finding your initializer label in program.\n");
        exit(1);}
    int program_size = load_program_data(directory);

    registers_initializer("../Apps/terminal.txt");
//    watch_memory(memory, 20000, 100);

    int execution_status = execute_program(terminal_size);
//    watch_memory(memory, 19900, 100);
    fprintf(stderr, "Shutting down OS\n");
    return 0;
}

int parse_and_execute(char* line){
    lstrip(line);
    // Ignore empty lines
    if (line[0] == '\0') return 0;
    // Ignore comment lines
    if (first_char(line) == '#') return 0;
    // Ignore label lines
    int len = strlen(line);
    if (len > 1 && line[len-1] == ':') return 0;

    char command[30],arg1[30],arg2[30],arg3[30];
    int tokens = sscanf(line, "%s %s %s %s", command, arg1, arg2, arg3);
    if (tokens < 1) return -1;

    if (strcasecmp(command,"exit") == 0)
        return 1;
    else if (strcasecmp(command, "MOV") == 0)
        mov_handler(arg1, arg2);
    else if (strcasecmp(command, "MOVW") == 0)
        movw_handler(arg1, arg2);
    else if (strcasecmp(command, "MOVB") == 0)
        movb_handler(arg1, arg2);



    else if (strcasecmp(command, "SHOWD") == 0)
        showd(arg1);
    else if (strcasecmp(command, "SHOWW") == 0)
        showw(arg1);
    else if (strcasecmp(command, "SHOWB") == 0)
        showb(arg1);


    else if (strcasecmp(command, "SHOWSD") == 0)
        showsd(arg1);
    else if (strcasecmp(command, "SHOWSW") == 0)
        showsw(arg1);
    else if (strcasecmp(command, "SHOWSB") == 0)
        showsb(arg1);

    else if (strcasecmp(command,"SHOW") == 0) // exactly the same as SHOWSD -> for ease of use
        showsd(arg1);

    else if (strcasecmp(command, "SHOWD_MEM") == 0)
        showd_mem(find_any_value(arg1));
    else if (strcasecmp(command, "SHOWW_MEM") == 0)
        showw_mem(find_any_value(arg1));
    else if (strcasecmp(command, "SHOWB_MEM") == 0)
        showb_mem(find_any_value(arg1));

    else if (strcasecmp(command, "SHOWSD_MEM") == 0)
        showsd_mem(find_any_value(arg1));
    else if (strcasecmp(command, "SHOWSW_MEM") == 0)
        showsw_mem(find_any_value(arg1));
    else if (strcasecmp(command, "SHOWSB_MEM") == 0)
        showsb_mem(find_any_value(arg1));

    else if (strcasecmp(command,"SHOWC") == 0)
        showc(arg1);

    else if (strcasecmp(command, "SHOW_ALL_REGS") == 0)
        show_all_registers();

    else if (strcasecmp(command, "ADD") == 0)
        add_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "ADDW") == 0)
        addw_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "ADDB") == 0)
        addb_handler(arg1, arg2, arg3);

    else if (strcasecmp(command, "SUB") == 0)
        sub_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "SUBW") == 0)
        subw_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "SUBB") == 0)
        subb_handler(arg1, arg2, arg3);

    else if (strcasecmp(command, "MUL") == 0)
        mul_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "MULW") == 0)
        mulw_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "MULB") == 0)
        mulb_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "IMUL") == 0)
        imul_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "IMULW") == 0)
        imulw_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "IMULB") == 0)
        imulb_handler(arg1, arg2, arg3);

    else if (strcasecmp(command, "DIV") == 0)
        div_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "DIVW") == 0)
        divw_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "DIVB") == 0)
        divb_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "IDIV") == 0)
        idiv_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "IDIVW") == 0)
        idivw_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "IDIVB") == 0)
        idivb_handler(arg1, arg2, arg3);


    else if (strcasecmp(command, "INC") == 0)
        inc_handler(arg1);
    else if (strcasecmp(command, "INCW") == 0)
        incw_handler(arg1);
    else if (strcasecmp(command, "INCB") == 0)
        incb_handler(arg1);

    else if (strcasecmp(command, "DEC") == 0)
        dec_handler(arg1);
    else if (strcasecmp(command, "DECW") == 0)
        decw_handler(arg1);
    else if (strcasecmp(command, "DECB") == 0)
        decb_handler(arg1);


    else if (strcasecmp(command, "AND") == 0)
        and_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "ANDW") == 0)
        andw_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "ANDB") == 0)
        andb_handler(arg1, arg2, arg3);

    else if (strcasecmp(command, "OR") == 0)
        or_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "ORW") == 0)
        orw_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "ORB") == 0)
        orb_handler(arg1, arg2, arg3);

    else if (strcasecmp(command, "XOR") == 0)
        xor_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "XORW") == 0)
        xorw_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "XORB") == 0)
        xorb_handler(arg1, arg2, arg3);

    else if (strcasecmp(command, "NOT") == 0)
        not_handler(arg1, arg2);
    else if (strcasecmp(command, "NOTW") == 0)
        notw_handler(arg1, arg2);
    else if (strcasecmp(command, "NOTB") == 0)
        notb_handler(arg1, arg2);

    else if (strcasecmp(command, "SHL") == 0)
        shl_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "SHLW") == 0)
        shlw_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "SHLB") == 0)
        shlb_handler(arg1, arg2, arg3);

    else if (strcasecmp(command, "SHR") == 0)
        shr_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "SHRW") == 0)
        shrw_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "SHRB") == 0)
        shrb_handler(arg1, arg2, arg3);

    else if (strcasecmp(command, "PUSH") == 0)
        push_handler(arg1);
    else if (strcasecmp(command, "PUSHW") == 0)
        pushw_handler(arg1);
    else if (strcasecmp(command, "PUSHB") == 0)
        pushb_handler(arg1);

    else if (strcasecmp(command, "POP") == 0)
        pop_handler(arg1);
    else if (strcasecmp(command, "POPW") == 0)
        popw_handler(arg1);
    else if (strcasecmp(command, "POPB") == 0)
        popb_handler(arg1);

    else if (strcasecmp(command, "SHOWF") == 0)
        show_flags();
    else if (strcasecmp(command, "CLF") == 0)
        clear_flags();

    else if (strcasecmp(command,"JMP") == 0)
        jmp_handler(arg1);
    else if (strcasecmp(command, "JE") == 0)
        je_handler(arg1);
    else if (strcasecmp(command, "JNE") == 0)
        jne_handler(arg1);
    else if (strcasecmp(command, "JG") == 0)
        jg_handler(arg1);
    else if (strcasecmp(command, "JL") == 0)
        jl_handler(arg1);

    else if (strcasecmp(command,"CALL") == 0)
        call_handler(arg1);
    else if (strcasecmp(command,"RET") == 0)
        ret_handler();

    else if (strcasecmp(command,"EQ") == 0)
        eq_handler(arg1, arg2, arg3);
    else if (strcasecmp(command,"NEQ") == 0)
        neq_handler(arg1, arg2, arg3);
    else if (strcasecmp(command,"GT") == 0)
        gt_handler(arg1, arg2, arg3);
    else if (strcasecmp(command,"GTE") == 0)
        gte_handler(arg1, arg2, arg3);
    else if (strcasecmp(command,"LS") == 0)
        ls_handler(arg1, arg2, arg3);
    else if (strcasecmp(command,"LSE") == 0)
        lse_handler(arg1, arg2, arg3);

    else if (strcasecmp(command, "INT") == 0)
        int_handler(arg1);
    else if (strcasecmp(command,"CLS") == 0)
        clear_screen();
    else if (strcasecmp(command,"HLT") == 0){
        return 401;
    }
    else
        printf("Unknown command: %s\n",command);
    return 0;
}






void clear_screen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Returns offset in program for a given label name, or -1 if not found
int find_label_offset(const char* name) {
    for (int i = 0; i < label_count; ++i) {
        if (strcasecmp(label_table[i].name, name) == 0)
            return label_table[i].offset;
    }
    return -1;
}

// Returns the index of the label in the table, or -1 if not found
int find_label_index(const char* name) {
    for (int i = 0; i < label_count; ++i) {
        if (strcasecmp(label_table[i].name, name) == 0)
            return i;
    }
    return -1;
}



