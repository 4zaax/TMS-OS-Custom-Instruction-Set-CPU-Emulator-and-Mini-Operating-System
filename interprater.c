#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "ctype.h"

#define MEMORY_SIZE 200 // store our variables
#define MAX_LINES 500   // os supports programs of maximum 500 lines
#define LINE_LEN 256
#define MAX_VARIABLES 1000
#define STACK_SIZE 1000  // os stack size

int input_memory[1000];
int var[MEMORY_SIZE];
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

typedef struct {
    char name[MAX_VAR_NAME];
    char value;
} CharVar;


CharVar char_list[1000];
int Characters=0;
Variable variables[1000];
int variable_count = 0;


LineEntry program[MAX_LINES];
int program_size = 0;


int main() {

    file = open_file("boot.txt");
    if (file == NULL) printf("Shutting down OS\n");else  printf("boot file loaded successfully\n");
    load_program(file);
    fclose(file);
    for (int current_line = 1 ; current_line <= program_size; current_line++){
        /*if (current_line == 14){
            printf("\n");
        }*/
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
    char command[20],arg1[20],arg2[20],arg3[20];
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
    else if (strcasecmp(command,"MOD") == 0)
        modulo(arg1, arg2, arg3);
    else if (strcasecmp(command,"JMP") == 0){
        int arglen = strlen(arg2);
        if (tokens == 2){
            go2(arg1, current_line);}
        else if (arglen == 1 && arg2[0] == '1'){
            go2(arg1,current_line);}
        else if (find_variable_index(arg2)!=-1 && find_variable_value(arg2)!=0){
            go2(arg1,current_line);}
        else if (find_variable_index(arg2)==-1){ printf("Flag for jumping not found!\n");}
    }
    else if (strcasecmp(command,"RET") == 0) {
        int arglen = strlen(arg1);
        if (tokens == 1) {
            *current_line = pop() - 1;
        } else if (arglen == 1 && arg1[0] == '1') {
            *current_line = pop() - 1;
        } else if (find_variable_index(arg1) != -1 && find_variable_value(arg1) != 0) {
            *current_line = pop() - 1;
        } else if (find_variable_index(arg2) == -1) { printf("Flag for retern not found!\n"); }
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
    else
        printf("Unknown command1: %s\n",command);
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
    if(find_variable_index(v)!=-1) printf("%s value is %d\n",v, find_variable_value(v));
    else { printf("%c",v);}
}

void pre_assign(char* arg1, char* arg2){
    int value2;
    if (isdigit(arg2[0])) {
        value2 = atoi(arg2);
        assign(arg1, value2);
    }
    else if (strlen(arg2) == 1){
        assign (arg1, arg2[0]);
    }
    else if (find_variable_index(arg2) !=-1){
        value2 = find_variable_value(arg2);
        assign(arg1, value2);
    }
    else printf("AS: an error occurred while assigning.\n",arg2);
}

void assign(const char *name, int value) {
    int index= find_variable_index(name);
    if (index != -1) {
        variables[index].value = value;
    } else {

        if (variable_count < MAX_VARIABLES) {
            strcpy(variables[variable_count].name, name);
            variables[variable_count].value = value;
            variable_count++;
        } else {
            printf("error\n");
        }
    }
}


int find_variable_value(const char *name) {
    int idx = find_variable_index(name);
    if (idx != -1) return variables[idx].value;
    printf("variable not found\n", name);
    return 0;
}



void equal( char var1[], char var2[], char var3[]) {
    int value2, value3;
    if (var2[0] <= '9' && var2[0] >= '0'){ value2 = atoi(var2);}
    else if(var2[0]=='[') {value2=input_memory[find_mem_indx(var2)];}
    else{ value2 = find_variable_value(var2);}
    if (var3[0] <= '9' && var3[0] >= '0'){ value3 = atoi(var3);}
    else if(var3[0]=='[') {value3=input_memory[find_mem_indx(var3)];}
    else {value3 = find_variable_value(var3);}
    int result = (value3 == value2);
    if (var1[0]=='[') {input_memory[find_mem_indx(var1)]=result;return;}
    assign(var1, result);
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
    else if(var2[0]=='[') {value2=input_memory[find_mem_indx(var2)];}
    else{ value2 = find_variable_value(var2);}
    if (var3[0] <= '9' && var3[0] >= '0'){ value3 = atoi(var3);}
    else if(var3[0]=='[') {value3=input_memory[find_mem_indx(var3)];}
    else {value3 = find_variable_value(var3);}
    int result = (value3 != value2);
    if (var1[0]=='[') {input_memory[find_mem_indx(var1)]=result;return;}
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
/*int find_char_var(const char *name) {
    for (int i = 0; i < Characters; i++) {
        if (strcmp(char_list[i].name, name) == 0)
            return i;
    }
    return -1;
}

void set_char_var(const char *name, char value) {
    int idx = find_char_var(name);
    if (idx != -1) {
        char_list[idx].value = value;
    }
    else {
        strcpy(char_list[Characters].name, name);
        char_list[Characters].value = value;
        Characters++;
    }
}
*/

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
    for (int i = 0; i < program_size; ++i) {
        if (strcmp(program[i].label, label) == 0)
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
    for(c = line; c == ' ' && c != '\0'; c++);
    return *c;
}
void my_strcpy(char *des,char * sourc,int len){
    for (int i=0;i<len;i++) {
        *(des + i) = *(sourc + i);
    }
    *(des+len)='\0';
}
int find_mem_indx(char* arg){


    if ((*arg) != '[') return -1;
    char* end_bracket = strchr(arg, ']');
    if(!end_bracket) return -1;
    int len=end_bracket-arg-1;
    int index;
    char *c=(arg+1);
    my_strcpy(c,(arg+1),len);
    if (isdigit(*(arg+1))) {
        index= atoi(c);
    }
    else
        index = find_variable_value(c);

    return index;
}

int* find_var_addr(char* name){
    int idx = find_variable_index(name);
    if (idx != -1) return &(variables[idx].value);
    printf("variable not found\n", name);
    return 0;
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

    int dest_reg_idx = find_variable_index(dest);
    int source_reg_idx = find_variable_index(src);
    int dest_mem_idx = find_mem_indx(dest);
    int source_mem_idx = find_mem_indx(src);
    // mov reg reg
    if (dest_reg_idx != -1 && source_reg_idx != -1){;
        assign(dest, find_variable_value(src));
        return;
    }
        // mov reg [mem]
    else if ( source_mem_idx != -1){

        if (source_mem_idx + 1 > MEMORY_SIZE){
            printf("MOV: assign memory to register failed due to an overflow\n");
            return;
        }
        assign(dest, input_memory[source_mem_idx]);
        return;
    }
        // mov [mem] reg
    else if (source_reg_idx != -1 && dest_mem_idx != -1){

        if (dest_mem_idx + 1 > MEMORY_SIZE){
            printf("MOV: assign register to memory failed due to an overflow\n");
            return;
        }

        input_memory[dest_mem_idx]= find_variable_value(src);
        return;
    }
        // mov reg number
    else if  (dest_reg_idx != -1  ){
        assign(dest, atoi(src));
        return;
    }
        // mov [mem] number
    else if (dest_mem_idx != -1 ){

        if (isdigit(src[0])){
            int val = atoi(src);
            input_memory[dest_mem_idx]=val;
        }
    }
    else
        printf("MOV : invalid syntax\n");
}

void read_mem(const char* arg1){
    int addr = find_mem_indx(arg1);
    printf("data in memory house %d : %d\n",addr,input_memory[addr]);
}

void reg_init (char *name){
    if (find_variable_index(name) != -1)
        printf("reg_init : register already exists\n");
    assign(name, 0);
}

void show_str(char* arg1){
    int idx = find_mem_indx(arg1);
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
    if (is_num(arg1, strlen(arg1))){
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
    if (is_num(arg1, strlen(arg1))){
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

int is_num(char* arg, int len){
    int i;
    for (i = 0; i < len; i++){
        if(!isdigit(*(arg+i)))
            return 0;
    }
    return 1;
}
