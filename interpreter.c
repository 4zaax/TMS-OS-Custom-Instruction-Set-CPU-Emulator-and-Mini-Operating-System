#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "ctype.h"
#include "stdint.h"

#define MEMORY_SIZE 128000 // RAM
#define MAX_LINES 500   // OS supports programs of maximum 500 lines
#define LINE_LEN 512
#define MAX_VARIABLES 1000

uint8_t reg_mem[40]; // 40 bytes (10 * 4 bytes) -> 10 extended registers
uint8_t flags_mem[4]; // ZF, SF, CF, OF (one byte each)

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

uint8_t memory[MEMORY_SIZE];
FILE *file;

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
    else if (strcasecmp(command,"CALL") == 0){
        push(*current_line + 1);// Save return point
        go2(arg1,current_line);}


    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////     optional commands /////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    else if (strcasecmp(command,"READ") == 0)
        read_mem(arg1);
    else if (strcasecmp(command,"SHOWSTR") == 0)
        show_str(arg1);
    else if (strcasecmp(command,"IF") == 0)
        if_command(arg1, arg2, current_line);
    else if (strcasecmp(command,"IFN") == 0)
        if_not_command(arg1, arg2, current_line);



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

// INC
void inc_handler(const char* reg) {
    uint32_t* r = get_register(reg, 4);
    if (!r) { printf("INC: must be 32-bit reg (%s)\n", reg); return; }
    (*r)++;
}
void incw_handler(const char* reg) {
    uint16_t* r = get_register(reg, 2);
    if (!r) { printf("INCW: must be 16-bit reg (%s)\n", reg); return; }
    (*r)++;
}
void incb_handler(const char* reg) {
    uint8_t* r = get_register(reg, 1);
    if (!r) { printf("INCB: must be 8-bit reg (%s)\n", reg); return; }
    (*r)++;
}

// DEC
void dec_handler(const char* reg) {
    uint32_t* r = get_register(reg, 4);
    if (!r) { printf("DEC: must be 32-bit reg (%s)\n", reg); return; }
    (*r)--;
}
void decw_handler(const char* reg) {
    uint16_t* r = get_register(reg, 2);
    if (!r) { printf("DECW: must be 16-bit reg (%s)\n", reg); return; }
    (*r)--;
}
void decb_handler(const char* reg) {
    uint8_t* r = get_register(reg, 1);
    if (!r) { printf("DECB: must be 8-bit reg (%s)\n", reg); return; }
    (*r)--;
}

int find_variable_index(const char *name) {
    for (int i = 0; i < variable_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void showd(const char* reg) {
    uint32_t* r = get_register(reg, 4);
    if (r) printf("%s = 0x%08X (%u)\n", reg, *r, *r);
    else printf("showd: %s is not a dword register\n", reg);
}

void showw(const char* reg) {
    uint16_t* r = get_register(reg, 2);
    if (r) printf("%s = 0x%04X (%u)\n", reg, *r, *r);
    else printf("showw: %s is not a word register\n", reg);
}

void showb(const char* reg) {
    uint8_t* r = get_register(reg, 1);
    if (r) printf("%s = 0x%02X (%u)\n", reg, *r, *r);
    else printf("showb: %s is not a byte register\n", reg);
}

// Show signed dword (32 bits)
void showsd(const char* reg) {
    int32_t* r = get_register(reg, 4);
    if (r) printf("%s = 0x%08X (%d)\n", reg, *r, *r);
    else printf("showsd: %s is not a dword register\n", reg);
}

// Show signed word (16 bits)
void showsw(const char* reg) {
    int16_t* r = get_register(reg, 2);
    if (r) printf("%s = 0x%04X (%d)\n", reg, *r, *r);
    else printf("showsw: %s is not a word register\n", reg);
}

// Show signed byte (8 bits)
void showsb(const char* reg) {
    int8_t* r = get_register(reg, 1);
    if (r) printf("%s = 0x%02X (%d)\n", reg, *r, *r);
    else printf("showsb: %s is not a byte register\n", reg);
}

void showc(const char* reg) {
    uint8_t* r = get_register(reg, 1);
    if (r) printf("%s = 0x%02X (%c)\n", reg, *r, *r);
    else printf("showc: %s is not a character register\n", reg);
}

// Unsigned
void showd_mem(int addr) {
    printf("MEM[%d] = 0x%08X (%u)\n", addr, *((uint32_t*)&memory[addr]), *((uint32_t*)&memory[addr]));
}
void showw_mem(int addr) {
    printf("MEM[%d] = 0x%04X (%u)\n", addr, *((uint16_t*)&memory[addr]), *((uint16_t*)&memory[addr]));
}
void showb_mem(int addr) {
    printf("MEM[%d] = 0x%02X (%u)\n", addr, memory[addr], memory[addr]);
}

// Signed
void showsd_mem(int addr) {
    printf("MEM[%d] = 0x%08X (%d)\n", addr, *((int32_t*)&memory[addr]), *((int32_t*)&memory[addr]));
}
void showsw_mem(int addr) {
    printf("MEM[%d] = 0x%04X (%d)\n", addr, *((int16_t*)&memory[addr]), *((int16_t*)&memory[addr]));
}
void showsb_mem(int addr) {
    printf("MEM[%d] = 0x%02X (%d)\n", addr, *((int8_t*)&memory[addr]), *((int8_t*)&memory[addr]));
}

void show_all_registers() {
    printf("EAX: 0x%08X  EBX: 0x%08X  ECX: 0x%08X  EDX: 0x%08X\n",
           *eax, *ebx, *ecx, *edx);
    printf("ESI: 0x%08X  EDI: 0x%08X  EBP: 0x%08X  ESP: 0x%08X\n",
           *esi, *edi, *ebp, *esp);
    printf("IP : 0x%08X  CS : 0x%08X\n",
           *ip, *cs);
}

///////////////////////////////////////////////////////////////
// pre_assign helps the language with multiple assign choices//
///////////////////////////////////////////////////////////////

void pre_assign(char* arg1, char* arg2){
    int assign_value = find_int_value(arg2);
    assign(arg1, assign_value);
}

int find_int_value(char* arg){
    int value;
    // AS v0 10
    if (is_num(arg))
        value = atoi(arg);
        // AS v0 's'
    else if (is_char(arg) == 1)
        value = char_to_i(arg);
    else if (is_char(arg) == 2)
        value = ' ';
        // AS v0 [100] || AS v0 [v1]
    else if (is_mem_addr(arg)){
        int addr = find_mem_addr(arg);
        value = memory[addr];
    }
        // AS v0 v1
    else if (find_variable_index(arg) !=-1){
        value = find_variable_value(arg);
        assign(arg, value);
    }
    else printf("AS: an error occurred while assigning.\n",arg);
    return value;
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

//void equal( char var1[], char var2[], char var3[]) {
//    int value2, value3;
//    if (var2[0] <= '9' && var2[0] >= '0'){ value2 = atoi(var2);}
//    else if(var2[0]=='[') {value2=memory[find_mem_addr(var2)];}
//    else{ value2 = find_variable_value(var2);}
//    if (var3[0] <= '9' && var3[0] >= '0'){ value3 = atoi(var3);}
//    else if(var3[0]=='[') {value3=memory[find_mem_addr(var3)];}
//    else {value3 = find_variable_value(var3);}
//    int result = (value3 == value2);
//    if (var1[0]=='[') {memory[find_mem_addr(var1)]=result;return;}
//    assign(var1, result);
//}
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
void and_handler(const char* dest, const char* src1, const char* src2) {
    uint32_t* d_reg = get_register(dest, 4);
    if (!d_reg) { printf("AND: dest must be 32-bit register (%s)\n", dest); return; }
    uint32_t v1 = get_any_value(src1);
    uint32_t v2 = get_any_value(src2);
    *d_reg = v1 & v2;
}

void andw_handler(const char* dest, const char* src1, const char* src2) {
    uint16_t* d_reg = get_register(dest, 2);
    if (!d_reg) { printf("ANDW: dest must be 16-bit register (%s)\n", dest); return; }
    uint16_t v1 = (uint16_t)get_any_value(src1);
    uint16_t v2 = (uint16_t)get_any_value(src2);
    *d_reg = v1 & v2;
}

void andb_handler(const char* dest, const char* src1, const char* src2) {
    uint8_t* d_reg = get_register(dest, 1);
    if (!d_reg) { printf("ANDB: dest must be 8-bit register (%s)\n", dest); return; }
    uint8_t v1 = (uint8_t)get_any_value(src1);
    uint8_t v2 = (uint8_t)get_any_value(src2);
    *d_reg = v1 & v2;
}

// OR
void or_handler(const char* dest, const char* src1, const char* src2) {
    uint32_t* d = get_register(dest, 4);
    if (!d) { printf("OR: dest must be 32-bit reg\n"); return; }
    *d = get_any_value(src1) | get_any_value(src2);
}
void orw_handler(const char* d, const char* s1, const char* s2) {
    uint16_t* r = get_register(d, 2);
    if (!r) { printf("ORW: dest must be 16-bit reg\n"); return; }
    *r = (uint16_t)get_any_value(s1) | (uint16_t)get_any_value(s2);
}
void orb_handler(const char* d, const char* s1, const char* s2) {
    uint8_t* r = get_register(d, 1);
    if (!r) { printf("ORB: dest must be 8-bit reg\n"); return; }
    *r = (uint8_t)get_any_value(s1) | (uint8_t)get_any_value(s2);
}

// XOR
void xor_handler(const char* dest, const char* src1, const char* src2) {
    uint32_t* d = get_register(dest, 4);
    if (!d) { printf("XOR: dest must be 32-bit reg\n"); return; }
    *d = get_any_value(src1) ^ get_any_value(src2);
}
void xorw_handler(const char* d, const char* s1, const char* s2) {
    uint16_t* r = get_register(d, 2);
    if (!r) { printf("XORW: dest must be 16-bit reg\n"); return; }
    *r = (uint16_t)get_any_value(s1) ^ (uint16_t)get_any_value(s2);
}
void xorb_handler(const char* d, const char* s1, const char* s2) {
    uint8_t* r = get_register(d, 1);
    if (!r) { printf("XORB: dest must be 8-bit reg\n"); return; }
    *r = (uint8_t)get_any_value(s1) ^ (uint8_t)get_any_value(s2);
}
void not_handler(const char* dest, const char* src) {
    uint32_t* d = get_register(dest, 4);
    if (!d) { printf("NOT: dest must be 32-bit reg\n"); return; }
    *d = ~get_any_value(src);
}
void notw_handler(const char* dest, const char* src) {
    uint16_t* d = get_register(dest, 2);
    if (!d) { printf("NOTW: dest must be 16-bit reg\n"); return; }
    *d = ~(uint16_t)get_any_value(src);
}
void notb_handler(const char* dest, const char* src) {
    uint8_t* d = get_register(dest, 1);
    if (!d) { printf("NOTB: dest must be 8-bit reg\n"); return; }
    *d = ~(uint8_t)get_any_value(src);
}
void shl_handler(const char* dest, const char* src1, const char* src2) {
    uint32_t* d = get_register(dest, 4);
    if (!d) { printf("SHL: dest must be 32-bit reg\n"); return; }
    *d = get_any_value(src1) << get_any_value(src2);
}
void shlw_handler(const char* d, const char* s1, const char* s2) {
    uint16_t* r = get_register(d, 2);
    if (!r) { printf("SHLW: dest must be 16-bit reg\n"); return; }
    *r = (uint16_t)get_any_value(s1) << (uint16_t)get_any_value(s2);
}
void shlb_handler(const char* d, const char* s1, const char* s2) {
    uint8_t* r = get_register(d, 1);
    if (!r) { printf("SHLB: dest must be 8-bit reg\n"); return; }
    *r = (uint8_t)get_any_value(s1) << (uint8_t)get_any_value(s2);
}
void shr_handler(const char* dest, const char* src1, const char* src2) {
    uint32_t* d = get_register(dest, 4);
    if (!d) { printf("SHR: dest must be 32-bit reg\n"); return; }
    *d = get_any_value(src1) >> get_any_value(src2);
}
void shrw_handler(const char* d, const char* s1, const char* s2) {
    uint16_t* r = get_register(d, 2);
    if (!r) { printf("SHRW: dest must be 16-bit reg\n"); return; }
    *r = (uint16_t)get_any_value(s1) >> (uint16_t)get_any_value(s2);
}
void shrb_handler(const char* d, const char* s1, const char* s2) {
    uint8_t* r = get_register(d, 1);
    if (!r) { printf("SHRB: dest must be 8-bit reg\n"); return; }
    *r = (uint8_t)get_any_value(s1) >> (uint8_t)get_any_value(s2);
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

void lstrip(char* line){
    char* trimmed = line;
    while(isspace((unsigned char)* trimmed))
        trimmed++;
    strcpy(line, trimmed);
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

char first_char(char* line){
    char* c;
    for(c = line; ((*c == ' ') || (*c == '\t')) && *c != '\0'; c++);
    return *c;
}
void my_strcpy(char *des,char * sourc,int len){
    for (int i=0;i<len;i++) {
        *(des + i) = *(sourc + i);
    }
    *(des+len)='\0';
}
int find_mem_addr(char* arg){
    if ((*arg) != '[') return -1;
    char* end_bracket = strchr(arg, ']');
    if(!end_bracket) return -1;
    int len = end_bracket-arg-1;
    int index;
    char *c= (char*) malloc(sizeof (char) * (len+1));
    my_strcpy(c,arg+1,len);

    if (is_num(c))
        index= atoi(c);
    else
        index = find_variable_value(c);
    return index;
}

// For RET/CALL, saves/restores line numbers on stack
void push(int val) {
    if (*esp < 4) { printf("PUSH: stack overflow\n"); return; }
    *esp -= 4;
    *((uint32_t*)&memory[*esp]) = val;
}

int pop() {
    if (*esp > MEMORY_SIZE - 4) { printf("POP: stack underflow\n"); return -1; }
    int val = *((uint32_t*)&memory[*esp]);
    *esp += 4;
    return val;
}

// Let's assume your stack base is at the *end* of memory[] (like x86).
// ESP starts at MEMORY_SIZE (bytes). ESP points to the top of the stack.

// PUSH dword (4 bytes)
void push_handler(const char* src) {
    uint32_t val = get_any_value(src);
    if (*esp < 4) { printf("PUSH: stack overflow\n"); return; }
    *esp -= 4;
    *((uint32_t*)&memory[*esp]) = val;
}

// PUSH word (2 bytes)
void pushw_handler(const char* src) {
    uint16_t val = (uint16_t)get_any_value(src);
    if (*esp < 2) { printf("PUSHW: stack overflow\n"); return; }
    *esp -= 2;
    *((uint16_t*)&memory[*esp]) = val;
}

// PUSH byte (1 byte)
void pushb_handler(const char* src) {
    uint8_t val = (uint8_t)get_any_value(src);
    if (*esp < 1) { printf("PUSHB: stack overflow\n"); return; }
    *esp -= 1;
    *((uint8_t*)&memory[*esp]) = val;
}

// POP dword (4 bytes)
void pop_handler(const char* dest) {
    if (*esp > MEMORY_SIZE - 4) { printf("POP: stack underflow\n"); return; }
    uint32_t* d = get_register(dest, 4);
    if (!d) { printf("POP: dest must be 32-bit reg (%s)\n", dest); return; }
    *d = *((uint32_t*)&memory[*esp]);
    *esp += 4;
}

// POP word (2 bytes)
void popw_handler(const char* dest) {
    if (*esp > MEMORY_SIZE - 2) { printf("POPW: stack underflow\n"); return; }
    uint16_t* d = get_register(dest, 2);
    if (!d) { printf("POPW: dest must be 16-bit reg (%s)\n", dest); return; }
    *d = *((uint16_t*)&memory[*esp]);
    *esp += 2;
}

// POP byte (1 byte)
void popb_handler(const char* dest) {
    if (*esp > MEMORY_SIZE - 1) { printf("POPB: stack underflow\n"); return; }
    uint8_t* d = get_register(dest, 1);
    if (!d) { printf("POPB: dest must be 8-bit reg (%s)\n", dest); return; }
    *d = *((uint8_t*)&memory[*esp]);
    *esp += 1;
}

void move(char* dest, char* src){
    int value = find_int_value(src);
    if (is_mem_addr(dest)) { // destination is memory_address
        int offset = find_variable_value("ecs");
        int idx = find_mem_addr(dest);
        memory[idx] = value;
    }
    else { // destination is register
        assign(dest, value);
    }
}

void read_mem(const char* arg1){
    int addr = find_mem_addr(arg1);
    printf("data in memory house %d : %d\n",addr,memory[addr]);
}

void show_str(char* arg1){
    int idx = find_mem_addr(arg1);
    int i;
    for (i = idx; memory[i] != 0 ;i++){
        if (idx > MEMORY_SIZE){
            printf("show_str: index our of range\n");
            return;
        }
        printf("%c", memory[i]);
    }
}

void if_command(char* arg1,char* arg2, int* current_line){
    if (is_num(arg1)){
        int res = atoi(arg1);
        if (res)
            go2(arg2, current_line);
    }
    else {
        int res = find_variable_value(arg1);
        if (res)
            go2(arg2, current_line);
    }
}

void if_not_command (char* arg1,char* arg2, int* current_line){
    if (is_num(arg1)){
        int res = atoi(arg1);
        if (!res)
            go2(arg2, current_line);
    }
    else {
        int res = find_variable_value(arg1);
        if (!res)
            go2(arg2, current_line);
    }
}

int is_num(char* arg){
    int len = strlen(arg);
    if (len == 1 && !isdigit(arg[0])){
        return 0;
    }
    char* c = arg;
    for(; isspace(*c); c++);
    if (!(isdigit(*c) || *c == '-' || *c == '+'))
        return 0;
    for(c++;*c != '\0' ;c++){
        if (!isdigit(*c))
            return 0;
    }
    return 1;
}

int is_mem_addr(char* arg){
    return arg[0] == '[' && arg[strlen(arg)-1] == ']';
}

int is_immediate(const char* s) {
    return (is_num(s) || is_char(s));
}

int is_char(char* arg){
    int len = strlen(arg);
    if (len == 1 && arg[0] == '\'')
        return 2;
    if (len == 3){
        if (arg[0] == '\'' && arg[2] == '\'')
            return 1;
        return 0;
    }
    else if (len == 4){
        if(arg[0] == '\'' && arg[3] == '\'' && arg[1] == '\\')
            return 1;
        return 0;
    }
    return 0;
}

int char_to_i(char* arg){
    int len = strlen(arg);
    if (len == 3)
        return arg[1];
    else if (len == 4)
        if (strcmp(arg, "'\\n'") == 0)
            return '\n';
        else if (strcmp(arg, "'\\t'") == 0)
            return '\t';
        else if (strcmp(arg, "'\\r'") == 0)
            return '\r';
        else if (strcmp(arg, "'\\b'") == 0)
            return '\b';
        else if (strcmp(arg, "'\\f'") == 0)
            return '\f';
        else if (strcmp(arg, "'\\v'") == 0)
            return '\v';
        else if (strcmp(arg, "'\\0'") == 0)
            return '\0';
        else if (strcmp(arg, "'\\'") == 0)
            return '\\';
        else if (strcmp(arg, "'\\''") == 0)
            return '\'';
        else
            printf("char_to_i: Invalid character, couldn't convert to integer.\n");
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

void conditional_jmp(char* command, char* arg1, char* arg2, char* arg3, int* current_line){
    int val2 = find_int_value(arg2);
    int val3 = find_int_value(arg3);
    if (strcasecmp(command, "JE") == 0){
        if (val2 == val3)
            go2(arg1, current_line);
    }
}

void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void* get_register(const char* reg, int size) {
    if (size == 4) {
        if      (strcasecmp(reg, "eax") == 0) return eax;
        else if (strcasecmp(reg, "ebx") == 0) return ebx;
        else if (strcasecmp(reg, "ecx") == 0) return ecx;
        else if (strcasecmp(reg, "edx") == 0) return edx;
        else if (strcasecmp(reg, "esi") == 0) return esi;
        else if (strcasecmp(reg, "edi") == 0) return edi;
        else if (strcasecmp(reg, "ebp") == 0) return ebp;
        else if (strcasecmp(reg, "esp") == 0) return esp;
        else if (strcasecmp(reg, "ip") == 0)  return ip;
        else if (strcasecmp(reg, "cs") == 0)  return cs;
    }
    if (size == 2) {
        if      (strcasecmp(reg, "ax") == 0) return ax;
        else if (strcasecmp(reg, "bx") == 0) return bx;
        else if (strcasecmp(reg, "cx") == 0) return cx;
        else if (strcasecmp(reg, "dx") == 0) return dx;
    }
    if (size == 1) {
        if      (strcasecmp(reg, "al") == 0) return al;
        else if (strcasecmp(reg, "ah") == 0) return ah;
        else if (strcasecmp(reg, "bl") == 0) return bl;
        else if (strcasecmp(reg, "bh") == 0) return bh;
        else if (strcasecmp(reg, "cl") == 0) return cl;
        else if (strcasecmp(reg, "ch") == 0) return ch;
        else if (strcasecmp(reg, "dl") == 0) return dl;
        else if (strcasecmp(reg, "dh") == 0) return dh;
    }
    return NULL; // Not a known register
}


void mov_handler (char* dest, char* src) {

    uint32_t* d_reg = get_register(dest, 4);
    uint32_t* s_reg = get_register(src, 4);

    // Case 3: dest is 4bytes register, src is memory
    if (d_reg && is_mem_addr(src)) {
        int addr = find_mem_addr(src);
        *d_reg = *((uint32_t*)&memory[addr]);
        return;
    }
    // Case 4: dest is memory, src is 4byte register
    if (is_mem_addr(dest) && s_reg) {
        int addr = find_mem_addr(dest);
        *((uint32_t*)&memory[addr]) = *s_reg;
        return;
    }
    // Case 5: immediate to 4 bytes register
    if (d_reg && is_immediate(src)) {

        *d_reg = find_int_value(src);
        return;
    }
    // Case 6: immediate to memory
    if (is_mem_addr(dest) && is_immediate(src)) {
        int addr = find_mem_addr(dest);
        *((uint32_t*)&memory[addr]) = find_int_value(src);
        return;
    }
    // Register to Register
    if (d_reg && s_reg) {
        *d_reg = *s_reg;
        return;
    }

    printf("MOV: Invalid operands (%s, %s)\n", dest, src);
}

void movb_handler(char* dest, char* src) {
    uint8_t* d_reg = get_register(dest, 1);
    uint8_t* s_reg = get_register(src, 1);

    // dest is register, src is memory
    if (d_reg && is_mem_addr(src)) {
        int addr = find_mem_addr(src);
        *d_reg = memory[addr];
        return;
    }
    // dest is memory, src is register
    if (is_mem_addr((char*)dest) && s_reg) {
        int addr = find_mem_addr(dest);
        memory[addr] = *s_reg;
        return;
    }
    // immediate to register
    if (d_reg && is_immediate(src)) {
        *d_reg = (uint8_t)find_int_value(src);
        return;
    }
    // immediate to memory
    if (is_mem_addr(dest) && is_immediate(src)) {
        int addr = find_mem_addr(dest);
        memory[addr] = (uint8_t)find_int_value(src);
        return;
    }
    // register to register
    if (d_reg && s_reg) {
        *d_reg = *s_reg;
        return;
    }
    printf("MOVB: Invalid operands (%s, %s)\n", dest, src);
}

void movw_handler(char* dest, char* src) {
    uint16_t* d_reg = get_register(dest, 2);
    uint16_t* s_reg = get_register(src, 2);

    // dest is register, src is memory
    if (d_reg && is_mem_addr(src)) {
        int addr = find_mem_addr(src);
        *d_reg = *((uint16_t*)&memory[addr]);
        return;
    }
    // dest is memory, src is register
    if (is_mem_addr(dest) && s_reg) {
        int addr = find_mem_addr((char*)dest);
        *((uint16_t*)&memory[addr]) = *s_reg;
        return;
    }
    // immediate to register
    if (d_reg && is_immediate(src)) {
        *d_reg = (uint16_t)find_int_value((char*)src);
        return;
    }
    // immediate to memory
    if (is_mem_addr((char*)dest) && is_immediate(src)) {
        int addr = find_mem_addr((char*)dest);
        *((uint16_t*)&memory[addr]) = (uint16_t)find_int_value((char*)src);
        return;
    }
    // register to register
    if (d_reg && s_reg) {
        *d_reg = *s_reg;
        return;
    }
    printf("MOVW: Invalid operands (%s, %s)\n", dest, src);
}

void add_handler(const char* dest, const char* src1, const char* src2) {
    uint32_t* d_reg = get_register(dest, 4);
    if (!d_reg) {
        printf("ADD: dest must be 32-bit register (%s)\n", dest);
        return;
    }
    uint32_t val1 = get_any_value(src1);
    uint32_t val2 = get_any_value(src2);
    uint32_t res = val1 + val2;
    *d_reg =res;
    set_flags_add32(val1, val2, res);
}

void addw_handler(const char* dest, const char* src1, const char* src2) {
    uint16_t* d_reg = get_register(dest, 2);
    if (!d_reg) {
        printf("ADDW: dest must be 16-bit register (%s)\n", dest);
        return;
    }
    uint16_t val1 = (uint16_t)get_any_value(src1);
    uint16_t val2 = (uint16_t)get_any_value(src2);
    uint16_t res = val1 + val2;
    *d_reg = res;
    set_flags_add16(val1, val2, res);
}

void addb_handler(const char* dest, const char* src1, const char* src2) {
    uint8_t* d_reg = get_register(dest, 1);
    if (!d_reg) {
        printf("ADDB: dest must be 8-bit register (%s)\n", dest);
        return;
    }
    uint8_t val1 = (uint8_t)get_any_value(src1);
    uint8_t val2 = (uint8_t)get_any_value(src2);
    uint8_t res = val1 + val2;
    *d_reg = res;
    set_flags_add8(val1, val2, res);
}

void sub_handler(const char* dest, const char* src1, const char* src2) {
    uint32_t* d_reg = get_register(dest, 4);
    if (!d_reg) {
        printf("SUB: dest must be 32-bit register (%s)\n", dest);
        return;
    }
    uint32_t val1 = get_any_value(src1);
    uint32_t val2 = get_any_value(src2);
    uint32_t res = val1 - val2;
    *d_reg = res;
    set_flags_sub32(val1, val2, res);
}

void subw_handler(const char* dest, const char* src1, const char* src2) {
    uint16_t* d_reg = get_register(dest, 2);
    if (!d_reg) {
        printf("SUBW: dest must be 16-bit register (%s)\n", dest);
        return;
    }
    uint16_t val1 = (uint16_t)get_any_value(src1);
    uint16_t val2 = (uint16_t)get_any_value(src2);
    uint16_t res = val1 - val2;
    *d_reg = res;
    set_flags_sub16(val1, val2, res);
}

void subb_handler(const char* dest, const char* src1, const char* src2) {
    uint8_t* d_reg = get_register(dest, 1);
    if (!d_reg) {
        printf("SUBB: dest must be 8-bit register (%s)\n", dest);
        return;
    }
    uint8_t val1 = (uint8_t)get_any_value(src1);
    uint8_t val2 = (uint8_t)get_any_value(src2);
    uint8_t res = val1 - val2;
    *d_reg = res;
    set_flags_sub8(val1, val2, res);
}

// Unsigned Multiplication
void mul_handler(const char* dest, const char* src1, const char* src2) {
    uint32_t* d_reg = get_register(dest, 4);
    if (!d_reg) { printf("MUL: dest must be 32-bit register\n"); return; }
    uint32_t v1 = (uint32_t)get_any_value(src1);
    uint32_t v2 = (uint32_t)get_any_value(src2);
    uint64_t result = (uint64_t)v1 * (uint64_t)v2;
    *d_reg = (uint32_t)result;
    set_flags_mul32(v1, v2, (uint32_t)result);
}

void mulw_handler(const char* dest, const char* src1, const char* src2) {
    uint16_t* d_reg = get_register(dest, 2);
    if (!d_reg) { printf("MULW: dest must be 16-bit register\n"); return; }
    uint16_t v1 = (uint16_t)get_any_value(src1);
    uint16_t v2 = (uint16_t)get_any_value(src2);
    uint32_t result = (uint32_t)v1 * (uint32_t)v2;
    *d_reg = (uint16_t)result;
    set_flags_mul16(v1, v2, (uint16_t)result);
}

void mulb_handler(const char* dest, const char* src1, const char* src2) {
    uint8_t* d_reg = get_register(dest, 1);
    if (!d_reg) { printf("MULB: dest must be 8-bit register\n"); return; }
    uint8_t v1 = (uint8_t)get_any_value(src1);
    uint8_t v2 = (uint8_t)get_any_value(src2);
    uint16_t result = (uint16_t)v1 * (uint16_t)v2;
    *d_reg = (uint8_t)result;
    set_flags_mul8(v1, v2, (uint8_t)result);
}

// Signed multiply
void imul_handler(const char* dest, const char* src1, const char* src2) {
    int32_t* d_reg = get_register(dest, 4);
    if (!d_reg) { printf("IMUL: dest must be 32-bit register\n"); return; }
    int32_t v1 = (int32_t)get_any_value(src1);
    int32_t v2 = (int32_t)get_any_value(src2);
    int64_t result = (int64_t)v1 * (int64_t)v2;
    *d_reg = (int32_t)result;
    set_flags_imul32(v1, v2, (int32_t)result);
}

void imulw_handler(const char* dest, const char* src1, const char* src2) {
    int16_t* d_reg = get_register(dest, 2);
    if (!d_reg) { printf("IMULW: dest must be 16-bit register\n"); return; }
    int16_t v1 = (int16_t)get_any_value(src1);
    int16_t v2 = (int16_t)get_any_value(src2);
    int32_t result = (int32_t)v1 * (int32_t)v2;
    *d_reg = (int16_t)result;
    set_flags_imul16(v1, v2, (int16_t)result);
}

void imulb_handler(const char* dest, const char* src1, const char* src2) {
    int8_t* d_reg = get_register(dest, 1);
    if (!d_reg) { printf("IMULB: dest must be 8-bit register\n"); return; }
    int8_t v1 = (int8_t)get_any_value(src1);
    int8_t v2 = (int8_t)get_any_value(src2);
    int16_t result = (int16_t)v1 * (int16_t)v2;
    *d_reg = (int8_t)result;
    set_flags_imul8(v1, v2, (int8_t)result);
}

// 32-bit unsigned division: dest = src1 / src2, EDX = src1 % src2
void div_handler(const char* dest, const char* src1, const char* src2) {
    uint32_t* d_reg = get_register(dest, 4);
    uint32_t v1 = get_any_value(src1);
    uint32_t v2 = get_any_value(src2);
    if (!d_reg) { printf("DIV: dest must be 32-bit register (%s)\n", dest); return; }
    if (v2 == 0) { printf("DIV: divide by zero\n"); return; }
    *d_reg = v1 / v2;
    *edx   = v1 % v2;
}

// 16-bit unsigned division: dest = src1 / src2, DX = src1 % src2
void divw_handler(const char* dest, const char* src1, const char* src2) {
    uint16_t* d_reg = get_register(dest, 2);
    uint16_t v1 = (uint16_t)get_any_value(src1);
    uint16_t v2 = (uint16_t)get_any_value(src2);
    if (!d_reg) { printf("DIVW: dest must be 16-bit register (%s)\n", dest); return; }
    if (v2 == 0) { printf("DIVW: divide by zero\n"); return; }
    *d_reg = v1 / v2;
    *dx    = v1 % v2;
}

// 8-bit unsigned division: dest = src1 / src2, DL = src1 % src2
void divb_handler(const char* dest, const char* src1, const char* src2) {
    uint8_t* d_reg = get_register(dest, 1);
    uint8_t v1 = (uint8_t)get_any_value(src1);
    uint8_t v2 = (uint8_t)get_any_value(src2);
    if (!d_reg) { printf("DIVB: dest must be 8-bit register (%s)\n", dest); return; }
    if (v2 == 0) { printf("DIVB: divide by zero\n"); return; }
    *d_reg = v1 / v2;
    *dl    = v1 % v2;
}

// 32-bit signed division: dest = src1 / src2, EDX = src1 % src2
void idiv_handler(const char* dest, const char* src1, const char* src2) {
    int32_t* d_reg = get_register(dest, 4);
    int32_t v1 = (int32_t)get_any_value(src1);
    int32_t v2 = (int32_t)get_any_value(src2);
    if (!d_reg) { printf("IDIV: dest must be 32-bit register (%s)\n", dest); return; }
    if (v2 == 0) { printf("IDIV: divide by zero\n"); return; }
    *d_reg = v1 / v2;
    *edx   = v1 % v2;
}

// 16-bit signed division: dest = src1 / src2, DX = src1 % src2
void idivw_handler(const char* dest, const char* src1, const char* src2) {
    int16_t* d_reg = get_register(dest, 2);
    int16_t v1 = (int16_t)get_any_value(src1);
    int16_t v2 = (int16_t)get_any_value(src2);
    if (!d_reg) { printf("IDIVW: dest must be 16-bit register (%s)\n", dest); return; }
    if (v2 == 0) { printf("IDIVW: divide by zero\n"); return; }
    *d_reg = v1 / v2;
    *dx    = v1 % v2;
}

// 8-bit signed division: dest = src1 / src2, DL = src1 % src2
void idivb_handler(const char* dest, const char* src1, const char* src2) {
    int8_t* d_reg = get_register(dest, 1);
    int8_t v1 = (int8_t)get_any_value(src1);
    int8_t v2 = (int8_t)get_any_value(src2);
    if (!d_reg) { printf("IDIVB: dest must be 8-bit register (%s)\n", dest); return; }
    if (v2 == 0) { printf("IDIVB: divide by zero\n"); return; }
    *d_reg = v1 / v2;
    *dl    = v1 % v2;
}

// 32-bit Add
void set_flags_add32(uint32_t a, uint32_t b, uint32_t result) {
    *ZF = (result == 0);
    *SF = ((int32_t)result < 0);
    *CF = (result < a); // carry-out for unsigned add
    *OF = (((a ^ result) & (b ^ result)) >> 31) & 1; // signed overflow
}

// 16-bit Add
void set_flags_add16(uint16_t a, uint16_t b, uint16_t result) {
    *ZF = (result == 0);
    *SF = ((int16_t)result < 0);
    *CF = (result < a);
    *OF = (((a ^ result) & (b ^ result)) >> 15) & 1;
}

// 8-bit Add
void set_flags_add8(uint8_t a, uint8_t b, uint8_t result) {
    *ZF = (result == 0);
    *SF = ((int8_t)result < 0);
    *CF = (result < a);
    *OF = (((a ^ result) & (b ^ result)) >> 7) & 1;
}

// 32-bit Subtract
void set_flags_sub32(uint32_t a, uint32_t b, uint32_t result) {
    *ZF = (result == 0);
    *SF = ((int32_t)result < 0);
    *CF = (a < b); // borrow
    *OF = (((a ^ b) & (a ^ result)) >> 31) & 1;
}

// 16-bit Subtract
void set_flags_sub16(uint16_t a, uint16_t b, uint16_t result) {
    *ZF = (result == 0);
    *SF = ((int16_t)result < 0);
    *CF = (a < b);
    *OF = (((a ^ b) & (a ^ result)) >> 15) & 1;
}

// 8-bit Subtract
void set_flags_sub8(uint8_t a, uint8_t b, uint8_t result) {
    *ZF = (result == 0);
    *SF = ((int8_t)result < 0);
    *CF = (a < b);
    *OF = (((a ^ b) & (a ^ result)) >> 7) & 1;
}

// Unsigned MUL Flags (ZF/SF untouched, CF/OF set)
void set_flags_mul32(uint32_t op1, uint32_t op2, uint32_t result) {
    uint64_t full = (uint64_t)op1 * (uint64_t)op2;
    if ((full >> 32) != 0) {
        *CF = 1; *OF = 1;
    } else {
        *CF = 0; *OF = 0;
    }
}

void set_flags_mul16(uint16_t op1, uint16_t op2, uint16_t result) {
    uint32_t full = (uint32_t)op1 * (uint32_t)op2;
    if ((full >> 16) != 0) {
        *CF = 1; *OF = 1;
    } else {
        *CF = 0; *OF = 0;
    }
}

void set_flags_mul8(uint8_t op1, uint8_t op2, uint8_t result) {
    uint16_t full = (uint16_t)op1 * (uint16_t)op2;
    if ((full >> 8) != 0) {
        *CF = 1; *OF = 1;
    } else {
        *CF = 0; *OF = 0;
    }
}

// Signed IMUL Flags (ZF/SF untouched, CF/OF set)
void set_flags_imul32(int32_t op1, int32_t op2, int32_t result) {
    int64_t full = (int64_t)op1 * (int64_t)op2;
    if ((full > INT32_MAX) || (full < INT32_MIN)) {
        *CF = 1; *OF = 1;
    } else {
        *CF = 0; *OF = 0;
    }
}

void set_flags_imul16(int16_t op1, int16_t op2, int16_t result) {
    int32_t full = (int32_t)op1 * (int32_t)op2;
    if ((full > INT16_MAX) || (full < INT16_MIN)) {
        *CF = 1; *OF = 1;
    } else {
        *CF = 0; *OF = 0;
    }
}

void set_flags_imul8(int8_t op1, int8_t op2, int8_t result) {
    int16_t full = (int16_t)op1 * (int16_t)op2;
    if ((full > INT8_MAX) || (full < INT8_MIN)) {
        *CF = 1; *OF = 1;
    } else {
        *CF = 0; *OF = 0;
    }
}

void je_handler(const char* label, int* current_line) {
    if (*ZF) go2(label, current_line);
}
void jne_handler(const char* label, int* current_line) {
    if (!*ZF) go2(label, current_line);
}
void jg_handler(const char* label, int* current_line) {
    if (!*ZF && !*SF) go2(label, current_line); // Not zero and not negative
}
void jl_handler(const char* label, int* current_line) {
    if (*SF) go2(label, current_line); // Sign flag set
}

void call_handler(const char* label, int* current_line) {
    push_handler_int(*current_line + 1); // Save return point (pushes on stack!)
    go2(label, current_line);
}

// For integer-only stack:
void push_handler_int(int value) {
    // Reuse your push logic for integers or make a small wrapper if stack is now in memory.
    // See your stack pointer and stack array logic.
}

void ret_handler(int* current_line) {
    int ret_line = pop_handler_int(); // Pop the return address (should match your stack implementation)
    if (ret_line > 0)
        *current_line = ret_line - 1; // Will be incremented after the loop
}

uint32_t get_any_value(const char* s) {
    void* ptr;
    if ((ptr = get_register(s, 4))) return *((uint32_t*)ptr);
    if ((ptr = get_register(s, 2))) return *((uint16_t*)ptr);
    if ((ptr = get_register(s, 1))) return *((uint8_t*)ptr);
    if (is_immediate(s)) return find_int_value((char*)s);
    printf("Unknown operand: %s\n", s);
    return 0;
}

void clear_flags() {
    for (int i = 0; i < 4; ++i)
        flags_mem[i] = 0;
}

void show_flags() {
    printf("ZF: %d  SF: %d  CF: %d  OF: %d\n", *ZF, *SF, *CF, *OF);
}



