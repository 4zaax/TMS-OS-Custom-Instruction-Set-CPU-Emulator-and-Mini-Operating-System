#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "ctype.h"

#define MEMORY_SIZE 1000 // RAM
#define MAX_LINES 500   // OS supports programs of maximum 500 lines
#define LINE_LEN 512
#define MAX_VARIABLES 1000
#define STACK_SIZE 1000  // OS stack size

int input_memory[MEMORY_SIZE];
int call_stack[STACK_SIZE];
int sp = -1; //stack pointer
FILE *file;

typedef struct {
    char line[LINE_LEN];
    char label[20];
} LineEntry;

typedef struct {
    char name[20];
    int value;
} Variable;

#define MAX_VAR_NAME 32

Variable variables[1000];
int variable_count = 0;
LineEntry program[MAX_LINES];
int program_size = 0;


int main() {
    file = open_file("../terminal.txt");
    if (file == NULL) printf("Shutting down OS\n");else  printf("boot file loaded successfully\n");
    load_program(file);
    fclose(file);
    printf("%s\n",program[165].label);
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
    if ( *current_line == 177){ ///////////////////////////////////////////////////////////////////////delete///////////////////
        printf("\n");
    }
    if (first_char(line) == '#') return 0;
    char command[30],arg1[30],arg2[30],arg3[30];
    int tokens = sscanf(program[*current_line].line, "%s %s %s %s", command, arg1, arg2, arg3);
    if (tokens < 1) return -1;
    if (strcasecmp(command,"exit") == 0)
        return 1;
    else if (strcasecmp(command,"AS") == 0)
        pre_assign(arg1,arg2);
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
    else if (strcasecmp(command,"AND") == 0)
        AND(arg1, arg2, arg3);
    else if (strcasecmp(command,"OR") == 0)
        OR(arg1, arg2, arg3);
    else if (strcasecmp(command,"XOR") == 0)
        XOR(arg1, arg2, arg3);
    else if (strcasecmp(command,"NOT") == 0)
        NOT(arg1,arg2);
    else if (strcasecmp(command,"SHL") == 0)
        SHL(arg1, arg2, arg3);
    else if (strcasecmp(command,"SHR") == 0)
        SHR(arg1, arg2, arg3);
    else if (strcasecmp(command,"INC") == 0)
        increment(arg1);
    else if (strcasecmp(command,"DEC") == 0)
        decrement(arg1);
    else if (strcasecmp(command,"GET") == 0)
        get_input(arg1);
    else if (strcasecmp(command,"ADD") == 0)
        addition(arg1, arg2, arg3);
    else if (strcasecmp(command,"SUB") == 0)
        subtract(arg1, arg2, arg3);
    else if (strcasecmp(command,"MUL") == 0)
        multiply(arg1, arg2, arg3);
    else if (strcasecmp(command,"DIV") == 0)
        divide(arg1, arg2, arg3);
    else if (strcasecmp(command,"SHOW") == 0)
        show(arg1);
    else if (strcasecmp(command,"SHOWC") == 0)
        showc(arg1);
    else if (strcasecmp(command,"MOD") == 0)
        modulo(arg1, arg2, arg3);
    else if (strcasecmp(command,"JMP") == 0){
        int arglen = strlen(arg2);
        if (tokens == 2)
            go2(arg1, current_line);
        else if (arglen == 1 && arg2[0] == '1')
            go2(arg1,current_line);
        else if (find_variable_index(arg2)!=-1 && find_variable_value(arg2)!=0){
            go2(arg1,current_line);}
        else if (find_variable_index(arg2)==-1)printf("check: no flag for jump!\n");
    }
    else if (strcasecmp(command,"RET") == 0) {
        int arglen = strlen(arg1);
        if (tokens == 1) {
            *current_line = pop() - 1;
        } else if (arglen == 1 && arg1[0] == '1') {
            *current_line = pop() - 1;
        } else if (find_variable_index(arg1) != -1 && find_variable_value(arg1) != 0) {
            *current_line = pop() - 1;
        } else if (find_variable_index(arg1) == -1)
            printf("check: no flag was found to return!\n");
    }

    else if (strcasecmp(command,"PUSH") == 0)
        user_push(arg1);
    else if (strcasecmp(command,"CALL") == 0){
        push(*current_line + 1);// Save return point
        go2(arg1,current_line);}
    else if (strcasecmp(command,"POP") == 0)
        user_pop(arg1);
    else if (strcasecmp(command,"MOV") == 0)
        move(arg1,arg2);
        // optional commands :
    else if (strcasecmp(command,"READ") == 0)
        read_mem(arg1);
    else if (strcasecmp(command,"SHOWSTR") == 0)
        show_str(arg1);
    else if (strcasecmp(command,"IF") == 0)
        if_command(arg1, arg2, current_line);
    else if (strcasecmp(command,"IFN") == 0)
        if_not_command(arg1, arg2, current_line);
    else if (strcasecmp(command,"RUN") == 0)
        run_app(arg1, arg2, current_line);
    else if (strcasecmp(command,"JE") == 0)
       conditional_jmp(command, arg1, arg2,arg3, current_line);
    else
        printf("Unknown command: %s\n",command);
    return 0;
}


void increment(char arg3[]){
    int result=find_variable_value(arg3);
    result+=1;
    assign(arg3,result);
};
void decrement(char arg3[]){
    int result=find_variable_value(arg3);
    result-=1;
    assign(arg3,result);
}
int find_variable_index(const char *name) {
    for (int i = 0; i < variable_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void show( char v[]){
    printf("%s value: %d\n",v, find_int_value(v)); /////////////////////delete %s part later
}
void showc( char v[]){
    printf("%c", find_int_value(v));
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
        value = input_memory[addr];
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
            input_memory[addr] = value;
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
//    else if(var2[0]=='[') {value2=input_memory[find_mem_addr(var2)];}
//    else{ value2 = find_variable_value(var2);}
//    if (var3[0] <= '9' && var3[0] >= '0'){ value3 = atoi(var3);}
//    else if(var3[0]=='[') {value3=input_memory[find_mem_addr(var3)];}
//    else {value3 = find_variable_value(var3);}
//    int result = (value3 == value2);
//    if (var1[0]=='[') {input_memory[find_mem_addr(var1)]=result;return;}
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
    else if(var2[0]=='[') {value2=input_memory[find_mem_addr(var2)];}
    else{ value2 = find_variable_value(var2);}
    if (var3[0] <= '9' && var3[0] >= '0'){ value3 = atoi(var3);}
    else if(var3[0]=='[') {value3=input_memory[find_mem_addr(var3)];}
    else {value3 = find_variable_value(var3);}
    int result = (value3 != value2);
    if (var1[0]=='[') {input_memory[find_mem_addr(var1)]=result;return;}
    assign(var1, result);

}
void AND(char var1[], char var2[], char var3[]) {
    int value2, value3;
    if (var2[0] <= '9' && var2[0] >= '0') value2 = atoi(var2);
    else value2 = find_variable_value(var2);
    if (var3[0] <= '9' && var3[0] >= '0') value3 = atoi(var3);
    else value3 = find_variable_value(var3);
    int result = value2 & value3;
    assign(var1, result);

}
void SHL(char var1[], char var2[], char var3[]) {
    int value2, value3;
    if (var2[0] <= '9' && var2[0] >= '0') value2 = atoi(var2);
    else {value2 = find_variable_value(var2);}
    if (var3[0] <= '9' && var3[0] >= '0') value3 = atoi(var3);
    else value3 = find_variable_value(var3);
    int result = value2 << value3;
    assign(var1, result);

}
void SHR(char var1[], char var2[], char var3[]) {
    int value2, value3;
    if (var2[0] <= '9' && var2[0] >= '0') value2 = atoi(var2);
    else value2 = find_variable_value(var2);
    if (var3[0] <= '9' && var3[0] >= '0') value3 = atoi(var3);
    else value3 = find_variable_value(var3);
    if (value3 < 0) {
        printf("error\n ");
        return;
    }
    int result = value2 >> value3;
    assign(var1, result);

}

void NOT(char var1[], char var2[]){
    int value2 ;
    if (var2[0] <= '9' && var2[0] >= '0') value2 = atoi(var2);
    else value2 = find_variable_value(var2);
    int result = ~value2;
    assign(var1, result);
}

void OR( char var1[],  char var2[], char var3[]) {
    int value2,value3;
    if (var2[0]<='9' &&  var2[0]>='0') value2= atoi(var2);
    else value2 = find_variable_value(var2);
    if (var3[0]<='9' &&  var3[0]>='0') value3= atoi(var3);
    else value3= find_variable_value(var3);
    int result = value2 | value3;
    assign(var1,result);
}
void XOR( char var1[],  char var2[], char var3[]) {
    int value2, value3;
    if (var2[0] <= '9' && var2[0] >= '0') value2 = atoi(var2);
    else value2 = find_variable_value(var2);
    if (var3[0] <= '9' && var3[0] >= '0') value3 = atoi(var3);
    else value3 = find_variable_value(var3);
    int result = value2 ^ value3;
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

void lstrip(char* line){
    char* trimmed = line;
    while(isspace((unsigned char)* trimmed))
        trimmed++;
    strcpy(line, trimmed);
}

void push(int val) {
    if (sp < STACK_SIZE - 1)
        call_stack[++sp] = val;
    else
        printf("Stack Overflow\n");
}

int pop() {
    if (sp >= 0)
        return call_stack[sp--];
    else {
        printf("Stack Underflow\n");
        return -1;
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

void user_push(char var[]){
    int val;
    if (isdigit(var[0]))
        val = atoi(var);
    else
        val = find_variable_value(var);
    push(val);
}

void user_pop(char var[]){
    int value = pop();
    assign(var,value);
}

void move(char* dest, char* src){
    int value = find_int_value(src);
    if (is_mem_addr(dest)) { // destination is memory_address
        int idx = find_mem_addr(dest);
        input_memory[idx] = value;
    }
    else { // destination is register
        assign(dest, value);
    }
}

void read_mem(const char* arg1){
    int addr = find_mem_addr(arg1);
    printf("data in memory house %d : %d\n",addr,input_memory[addr]);
}

void show_str(char* arg1){
    int idx = find_mem_addr(arg1);
    int i;
    for (i = idx; input_memory[i] != 0 ;i++){
        if (idx > MEMORY_SIZE){
            printf("show_str: index our of range\n");
            return;
        }
        printf("%c", input_memory[i]);
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
    int i;
    for (i = 0; i < len; i++){
        if(!isdigit(*(arg+i)))
            return 0;
    }
    return 1;
}

int is_mem_addr(char* arg){
    int len = strlen(arg);
    if ((arg[0] == '[')&&(arg[len-1] == ']'))
        return 1;
    return 0;
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