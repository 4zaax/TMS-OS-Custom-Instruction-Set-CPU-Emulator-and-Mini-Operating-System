#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "ctype.h"
#include "stdint.h"
#include "helpers.h"
#include "handlers.h"

#define MEMORY_SIZE 128000 // RAM
#define MAX_LINES 500   // OS supports programs of maximum 500 lines
#define LINE_LEN 512
#define MAX_VARIABLES 1000

uint8_t memory[MEMORY_SIZE];
uint8_t reg_mem[40]; // 40 bytes (10 * 4 bytes) -> 10 extended registers
uint8_t flags_mem[4]; // ZF, SF, CF, OF (one byte each)
FILE *file;

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

////////////////////////////////////////////////////////////////
////////////////// INDEX & POINTER REGISTERS ///////////////////
////////////////////////////////////////////////////////////////

uint32_t* esi = (uint32_t*)&reg_mem[16];
uint32_t* edi = (uint32_t*)&reg_mem[20];
uint32_t* ebp = (uint32_t*)&reg_mem[24];
uint32_t* esp = (uint32_t*)&reg_mem[28];
uint32_t* ip = (uint32_t*)&reg_mem[32];
uint32_t* cs = (uint32_t*)&reg_mem[36];



typedef struct {
    char line[LINE_LEN];
    char label[20];
} LineEntry;

typedef struct {
    char name[20];
    int value;
} Variable;

Variable variables[1000];
int variable_count = 0;
LineEntry program[MAX_LINES];
int program_size = 0;


int main() {
    *esp = MEMORY_SIZE;
    file = open_file("../boot.txt");
    if (file == NULL) printf("Shutting down OS\n");else  printf("boot file loaded successfully\n");
    load_program(file);
    fclose(file);
    for (int current_line = 1 ; current_line <= program_size; current_line++){
        int result=check(program[current_line].line, &current_line);
        if(result==1)break;
    }
}


FILE* open_file(char address[]){
    FILE *file = fopen(address, "r");
    if (file == NULL) {
        printf("Failed to open boot file %s.\n",address);
        return NULL;
    }
    return file;
}

int check(char *line, int *current_line){

    if (first_char(line) == '#') return 0;
    char command[30],arg1[30],arg2[30],arg3[30];
    int tokens = sscanf(program[*current_line].line, "%s %s %s %s", command, arg1, arg2, arg3);
    if (tokens < 1) return -1;
    if (strcasecmp(command,"exit") == 0){
        return 1;
    }
    else if (strcasecmp(command,"AS") == 0)
        pre_assign(arg1,arg2);
    else if (strcasecmp(command, "MOV") == 0)
        mov_handler(arg1, arg2);
    else if (strcasecmp(command, "MOVB") == 0)
        movb_handler(arg1, arg2);
    else if (strcasecmp(command, "MOVW") == 0)
        movw_handler(arg1, arg2);
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
    else if (strcasecmp(command, "SHOWD_MEM") == 0)
        showd_mem(find_int_value(arg1));
    else if (strcasecmp(command, "SHOWW_MEM") == 0)
        showw_mem(find_int_value(arg1));
    else if (strcasecmp(command, "SHOWB_MEM") == 0)
        showb_mem(find_int_value(arg1));
    else if (strcasecmp(command, "SHOWSD_MEM") == 0)
        showsd_mem(find_int_value(arg1));
    else if (strcasecmp(command, "SHOWSW_MEM") == 0)
        showsw_mem(find_int_value(arg1));
    else if (strcasecmp(command, "SHOWSB_MEM") == 0)
        showsb_mem(find_int_value(arg1));
    else if (strcasecmp(command, "SHOW_ALL_REGS") == 0)
        show_all_registers();
    else if (strcasecmp(command,"EQ") == 0)
        equal(arg1, arg2, arg3);
    else if (strcasecmp(command,"NEQ") == 0)
        not_equal(arg1, arg2, arg3);
    else if (strcasecmp(command,"GT") == 0)
        is_greater(arg1, arg2, arg3);
    else if (strcasecmp(command,"GTE") == 0)
        is_greater_equal(arg1, arg2, arg3);
    else if (strcasecmp(command,"LS") == 0)
        is_less_than(arg1, arg2, arg3);
    else if (strcasecmp(command,"LSE") == 0)
        is_less_than_equal(arg1, arg2, arg3);

    else if (strcasecmp(command,"GET") == 0)
        get_input(arg1);
    else if (strcasecmp(command, "ADD") == 0)
        add_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "ADDB") == 0)
        addb_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "ADDW") == 0)
        addw_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "SUB") == 0)
        sub_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "SUBW") == 0)
        subw_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "SUBB") == 0)
        subb_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "MUL") == 0)
        mul_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "IMUL") == 0)
        imul_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "MULW") == 0)
        mulw_handler(arg1, arg2, arg3);
    else if (strcasecmp(command, "MULB") == 0)
        mulb_handler(arg1, arg2, arg3);
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

    else if (strcasecmp(command,"SHOW") == 0)
        showd(arg1);
    else if (strcasecmp(command,"SHOWC") == 0)
        showc(arg1);
    else if (strcasecmp(command, "SHOW_FLAGS") == 0)
        show_flags();
    else if (strcasecmp(command, "CLEAR_FLAGS") == 0)
        clear_flags();

    else if (strcasecmp(command,"MOD") == 0)
        modulo(arg1, arg2, arg3);
    else if (strcasecmp(command,"JMP") == 0){
        int arglen = strlen(arg2);
        if (tokens == 2)
            go2(arg1, current_line);
        else if (arglen == 1 && arg2[0] == '1')
            go2(arg1,current_line);
        else if (get_any_value(arg2)!=0){
            go2(arg1,current_line);}
    }
    else if (strcasecmp(command, "JE") == 0)
        je_handler(arg1, current_line);
    else if (strcasecmp(command, "JNE") == 0)
        jne_handler(arg1, current_line);
    else if (strcasecmp(command, "JG") == 0)
        jg_handler(arg1, current_line);
    else if (strcasecmp(command, "JL") == 0)
        jl_handler(arg1, current_line);

    else if (strcasecmp(command,"CALL") == 0)
        call_handler(arg1, current_line);
    else if (strcasecmp(command,"RET") == 0) {
        int arglen = strlen(arg1);
        if (tokens == 1) {
            *current_line = pop() - 1;
        } else if (is_num(arg1) && atoi(arg1) != 0){
            *current_line = pop() -1;
        } else if (arglen == 1 && arg1[0] == '1') {
            *current_line = pop() - 1;
        } else if (find_variable_index(arg1) != -1 && find_variable_value(arg1) != 0) {
            *current_line = pop() - 1;
        } else if (find_variable_index(arg1) == -1)
            printf("check: no flag was found to return!\n");
    }



    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////     optional commands /////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    else if (strcasecmp(command,"READ") == 0)
        read_mem(arg1);
    else if (strcasecmp(command,"SHOWSTR") == 0)
        show_str(arg1);



    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////     system commands   /////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    else if (strcasecmp(command,"clear") == 0)
        clear_screen();
    else if (strcasecmp(command,"run") == 0)
        run_app(arg1,arg2,current_line);
    else
        printf("Unknown command: %s\n",command);
    return 0;
}





///////////////////////////////////////////////////////////////
// pre_assign helps the language with multiple assign choices//
///////////////////////////////////////////////////////////////

void pre_assign(char* arg1, char* arg2){
    int assign_value = find_int_value(arg2);
    assign(arg1, assign_value);
}


void assign(const char *name, int value) {
    if (is_mem_addr(name)){
        int addr = find_mem_addr(name);
        if (addr >= MEMORY_SIZE)
            printf("Assign: memory index out of range.\n");
        else
            memory[addr] = value;
    }
    else { // value should be assigned to register
        int index= find_variable_index(name);
        if (index != -1)
            variables[index].value = value;
        else {
            if (variable_count < MAX_VARIABLES) {
                strcpy(variables[variable_count].name, name);
                variables[variable_count].value = value;
                variable_count++;
            }
            else
                printf("Assign: register count exceeds limits\n");
        }
    }
}

int find_variable_value(const char *name) {
    int idx = find_variable_index(name);
    if (idx != -1) return variables[idx].value;
    printf("find_variable_value: variable %s not found\n", name);;
    return 0;
}

 void equal( char var1[], char var2[], char var3[]) {
    int value2, value3;
    value2 = find_int_value(var2);
    value3 = find_int_value(var3);
    if (value2 == value3){
        assign(var1, 1);
        return;
    }
    assign(var1, 0);
}
void is_greater( char var1[], char var2[], char var3[]) {
    int value2, value3;
    if (var2[0] <= '9' && var2[0] >= '0') value2 = atoi(var2);
    else value2 = find_variable_value(var2);
    if (var3[0] <= '9' && var3[0] >= '0') value3 = atoi(var3);
    else value3 = find_variable_value(var3);
    int result = (value2 > value3 );
    assign(var1, result);
}
void is_greater_equal(char var1[], char var2[], char var3[]) {
    int value2, value3;
    if (var2[0] <= '9' && var2[0] >= '0') value2 = atoi(var2);
    else value2 = find_variable_value(var2);
    if (var3[0] <= '9' && var3[0] >= '0') value3 = atoi(var3);
    else value3 = find_variable_value(var3);
    int result = (value2 >= value3);
    assign(var1, result);
}
void is_less_than(char var1[], char var2[], char var3[]) {
    int value2, value3;
    if (var2[0] <= '9' && var2[0] >= '0') value2 = atoi(var2);
    else value2 = find_variable_value(var2);
    if (var3[0] <= '9' && var3[0] >= '0') value3 = atoi(var3);
    else value3 = find_variable_value(var3);
    int result = (value2 < value3);
    assign(var1, result);
}
void is_less_than_equal(char var1[], char var2[], char var3[]) {
    int value2, value3;
    if (var2[0] <= '9' && var2[0] >= '0') value2 = atoi(var2);
    else value2 = find_variable_value(var2);
    if (var3[0] <= '9' && var3[0] >= '0') value3 = atoi(var3);
    else value3 = find_variable_value(var3);
    int result = (value2 <= value3);
    assign(var1, result);
}
void not_equal(char var1[], char var2[], char var3[]) {
    int value2, value3;
    if (var2[0] <= '9' && var2[0] >= '0'){ value2 = atoi(var2);}
    else if(var2[0]=='[') {value2=memory[find_mem_addr(var2)];}
    else{ value2 = find_variable_value(var2);}
    if (var3[0] <= '9' && var3[0] >= '0'){ value3 = atoi(var3);}
    else if(var3[0]=='[') {value3=memory[find_mem_addr(var3)];}
    else {value3 = find_variable_value(var3);}
    int result = (value3 != value2);
    if (var1[0]=='[') {memory[find_mem_addr(var1)]=result;return;}
    assign(var1, result);

}


void addition(char var1[],  char var2[], char var3[]){
    int value2,value3;
    if (var2[0]<='9' &&  var2[0]>='0') value2= atoi(var2);
    else value2 = find_variable_value(var2);
    if (var3[0]<='9' &&  var3[0]>='0') value3= atoi(var3);
    else value3= find_variable_value(var3);
    int result=value2+value3;
    assign(var1,result);
}

void subtract(char var1[],  char var2[], char var3[]){
    int value2,value3;
    if (var2[0]<='9' &&  var2[0]>='0') value2= atoi(var2);
    else value2 = find_variable_value(var2);
    if (var3[0]<='9' &&  var3[0]>='0') value3= atoi(var3);
    else value3= find_variable_value(var3);
    int result=value2-value3;
    assign(var1,result);
}

void multiply(char var1[],  char var2[], char var3[]){
    int value2,value3;
    if (var2[0]<='9' &&  var2[0]>='0') value2= atoi(var2);
    else value2 = find_variable_value(var2);
    if (var3[0]<='9' &&  var3[0]>='0') value3= atoi(var3);
    else value3= find_variable_value(var3);
    int result=value3*value2;
    assign(var1,result);
}

void divide(char var1[],  char var2[], char var3[]){
    int value2,value3;
    if (var2[0]<='9' &&  var2[0]>='0') value2= atoi(var2);
    else value2 = find_variable_value(var2);
    if (var3[0]<='9' &&  var3[0]>='0') value3= atoi(var3);
    else value3= find_variable_value(var3);
    if(value3==0) printf("Division by zero");
    else{
        int result=value2/value3;
        assign(var1,result);
    }

}

void get_input(char arg1[]){
    int c=getchar();
    assign(arg1, c);
}

void modulo(char var1[], char var2[], char var3[]){
    int value2,value3;
    if (var2[0]<='9' &&  var2[0]>='0') value2= atoi(var2);
    else value2 = find_variable_value(var2);
    if (var3[0]<='9' &&  var3[0]>='0') value3= atoi(var3);
    else value3= find_variable_value(var3);
    int result=value2%value3;
    assign(var1,result);
}


void load_program(FILE *file) {
    char buffer[LINE_LEN];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
        lstrip(buffer);
        // Check if it's a label
        char *colon = strchr(buffer, ':');
        program_size++;
        if (colon && colon == buffer + strlen(buffer) - 1) {
            // It’s a label like L1:
            strncpy(program[program_size].label, buffer, colon - buffer);
            program[program_size].label[colon - buffer] = '\0';
            program[program_size].line[0] = '\0'; // No command
        } else {
            program[program_size].label[0] = '\0'; // No label
            strcpy(program[program_size].line, buffer);
        }
    }
}

int find_label(const char *label) {
    for (int i = 0; i <= program_size; ++i) {
        if (strcasecmp(program[i].label, label) == 0)
            return i;
    }
    return -1;
}

void go2(const char *label, int *current_line){
    int target_line = find_label(label);
    if (target_line != -1) {
        *current_line = target_line;
    } else {
        printf("Label not found: %s\n", label);
    }
}



void run_app(char* filename, char* arg2, int* current_line) {
    int flag = find_int_value(arg2);
    if (flag == 0) return;
    FILE* f = fopen(filename, "r");
    if (!f) {
        printf("run: file not found: %s\n", filename);
        return;
    }
    // Save current interpreter state (if needed)
    int saved_program_size = program_size;
    LineEntry saved_program[MAX_LINES];
    memcpy(saved_program, program, sizeof(program));

    // Load the new program
    program_size = 0;
    load_program(f);
    fclose(f);

    // Run the loaded program
    int run_line;
    for (run_line = 1; run_line <= program_size; run_line++) {
        int result = check(program[run_line].line, &run_line);
        if (result == 1) break; // allow the called app to "exit"
    }

    // Restore previous program (the shell)
    memcpy(program, saved_program, sizeof(program));
    program_size = saved_program_size;
    // Return to next line in shell after RUN
}

void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}




