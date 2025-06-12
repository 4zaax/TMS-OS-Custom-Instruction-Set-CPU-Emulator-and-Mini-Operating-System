#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "ctype.h"

#define MEMORY_SIZE 200 // store our variables
#define MAX_LINES 500   // os supports programs of maximum 500 lines
#define LINE_LEN 256
#define MAX_VARIABLES 1000
#define STACK_SIZE 100  // os stack size

char input_memory[1000];
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

    file = open_file("../boot.txt");
    if (file == NULL) printf("Shutting down OS\n");else  printf("boot file loaded successfully\n");
    load_program(file);
    fclose(file);
    for (int current_line = 1 ; current_line <= program_size; current_line++){
        if(program[current_line].line[0] == '\0')continue;
        check(program[current_line].line, &current_line);
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
    char command[10],arg1[10],arg2[10],arg3[10];
    int tokens = sscanf(program[*current_line].line, "%s %s %s %s", command, arg3, arg1, arg2);
    if (tokens < 1) return -1;

    if (strcasecmp(command,"AS") == 0){
         int value1;
         if(arg1[0]<='9' && arg1[0]>='0') {value1= atoi(arg1);  assign(arg3, value1);}
         else if ( find_variable_index(arg1)!=-1 ){value1= find_variable_value(arg1);  assign(arg3, value1);}
         else{ printf("value not found");}

    }
    else if (strcasecmp(command,"EQ") == 0)
        equal(arg1, arg2, arg3);
    else if (strcasecmp(command,"NEQ") == 0)
        not_equal(arg1, arg2, arg3);
    else if (strcasecmp(command,"GT") == 0)
        is_greater(arg1, arg2, arg3);
    else if (strcasecmp(command,"EQ") == 0)
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
        NOT(arg1);
    else if (strcasecmp(command,"SHL") == 0)
        SHL(arg1, arg2, arg3);
    else if (strcasecmp(command,"SHR") == 0)
        SHR(arg1, arg2, arg3);
    else if (strcasecmp(command,"INC") == 0)
        increment(arg3);
    else if (strcasecmp(command,"INC") == 0)
        decrement(arg3);
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
    else if (strcasecmp(command,"GMP") == 0){
        if (arg1[0]<='9' && arg3[0]>='1') go2(arg1,current_line);
        else if(find_variable_index(arg3)!=-1 && find_variable_value(arg3)!=0) go2(arg1,current_line);
    }
    else if (strcasecmp(command,"RET") == 0)
        *current_line = pop();
    else if (strcasecmp(command,"PUSH") == 0)
        user_push(arg1);
    else if (strcasecmp(command,"POP") == 0)
        user_pop(arg1);
    else
        printf("Unknown command: %s\n",command);
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
    if(find_variable_index(v)!=-1) printf("%d", find_variable_value(v));
    else { printf("%c",v);}
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
    int value1, value2;
    if (var1[0] <= '9' && var1[0] >= '0') value1 = atoi(var1);
    else value1 = find_variable_value(var1);
    if (var2[0] <= '9' && var2[0] >= '0') value2 = atoi(var2);
    else value2 = find_variable_value(var2);
    int result = (value2 == value1);
    assign(var3, result);

}
void is_greater( char var1[], char var2[], char var3[]) {
    int value1, value2;
    if (var1[0] <= '9' && var1[0] >= '0') value1 = atoi(var1);
    else value1 = find_variable_value(var1);
    if (var2[0] <= '9' && var2[0] >= '0') value2 = atoi(var2);
    else value2 = find_variable_value(var2);
    int result = (value1 > value2 );
    assign(var3, result);
}
void is_greater_equal( char var1[], char var2[], char var3[]) {
    int value1, value2;
    if (var1[0] <= '9' && var1[0] >= '0') value1 = atoi(var1);
    else value1 = find_variable_value(var1);
    if (var2[0] <= '9' && var2[0] >= '0') value2 = atoi(var2);
    else value2 = find_variable_value(var2);
    int result = (value1 >= value2);
    assign(var3, result);
}
void is_less_than( char var1[], char var2[], char var3[]) {
    int value1, value2;
    if (var1[0] <= '9' && var1[0] >= '0') value1 = atoi(var1);
    else value1 = find_variable_value(var1);
    if (var2[0] <= '9' && var2[0] >= '0') value2 = atoi(var2);
    else value2 = find_variable_value(var2);
    int result = (value1 < value2);
    assign(var3, result);
}
void is_less_than_equal( char var1[], char var2[], char var3[]) {
    int value1, value2;
    if (var1[0] <= '9' && var1[0] >= '0') value1 = atoi(var1);
    else value1 = find_variable_value(var1);
    if (var2[0] <= '9' && var2[0] >= '0') value2 = atoi(var2);
    else value2 = find_variable_value(var2);
    int result = (value1 <= value2);
    assign(var3, result);
}
void not_equal( char var1[], char var2[], char var3[]) {
    int value1, value2;
    if (var1[0] <= '9' && var1[0] >= '0') value1 = atoi(var1);
    else value1 = find_variable_value(var1);
    if (var2[0] <= '9' && var2[0] >= '0') value2 = atoi(var2);
    else value2 = find_variable_value(var2);
    int result = (value2 != value1);
    assign(var3, result);

}
void AND( char var1[], char var2[], char var3[]) {
    int value1, value2;
    if (var1[0] <= '9' && var1[0] >= '0') value1 = atoi(var1);
    else value1 = find_variable_value(var1);
    if (var2[0] <= '9' && var2[0] >= '0') value2 = atoi(var2);
    else value2 = find_variable_value(var2);
    int result = value1 & value2;
    assign(var3, result);

}
void SHL( char var1[],char var2[],  char var3[]) {
    int value1, value2;
    if (var1[0] <= '9' && var1[0] >= '0') value1 = atoi(var1);
    else {value1 = find_variable_value(var1);}
    if (var2[0] <= '9' && var2[0] >= '0') value2 = atoi(var2);
    else value2 = find_variable_value(var2);
    int result = value1 << value2;
    assign(var3, result);

}
void SHR( char var1[],char var2[],  char var3[]) {
    int value1, value2;
    if (var1[0] <= '9' && var1[0] >= '0') value1 = atoi(var1);
    else value1 = find_variable_value(var1);
    if (var2[0] <= '9' && var2[0] >= '0') value2 = atoi(var2);
    else value2 = find_variable_value(var2);
    if (value2 < 0) {
        printf("error\n ");
        return;
    }
    int result = value1 >> value2;
    assign(var3, result);

}

void NOT( char var1[]) {
    int value1 ;
    if (var1[0] <= '9' && var1[0] >= '0') value1 = atoi(var1);
    else value1 = find_variable_value(var1);
    int result = ~value1;
    assign(var1, result);
}

void OR( char var1[],  char var2[], char var3[]) {
    int value1,value2;
    if (var1[0]<='9' &&  var1[0]>='0') value1= atoi(var1);
    else value1 = find_variable_value(var1);
    if (var2[0]<='9' &&  var2[0]>='0') value2= atoi(var2);
    else value2= find_variable_value(var2);
    int result=value1 | value2;
    assign(var3,result);
}
void XOR( char var1[],  char var2[], char var3[]) {
    int value1, value2;
    if (var1[0] <= '9' && var1[0] >= '0') value1 = atoi(var1);
    else value1 = find_variable_value(var1);
    if (var2[0] <= '9' && var2[0] >= '0') value2 = atoi(var2);
    else value2 = find_variable_value(var2);
    int result = value1 ^ value1;
    assign(var3, result);
}
void equal_test(char var1[], char value[], char destination[]){
    int source_id = get_var_id(var1);
    int source = var[source_id];
    int val = atoi(value);
    int result;
    source == val ? (result = 1):(result = 0);
    int dest_id = get_var_id(destination);
    var[dest_id] = result;
}

void addition(char var1[], char var2[], char var3[]){
    int value1,value2;
    if (var1[0]<='9' &&  var1[0]>='0') value1= atoi(var1);
    else value1 = find_variable_value(var1);
    if (var2[0]<='9' &&  var2[0]>='0') value2= atoi(var2);
    else value2= find_variable_value(var2);
    int result=value1+value2;
    assign(var3,result);
}

void subtract(char var1[], char var2[], char var3[]){
    int value1,value2;
    if (var1[0]<='9' &&  var1[0]>='0') value1= atoi(var1);
    else value1 = find_variable_value(var1);
    if (var2[0]<='9' &&  var2[0]>='0') value2= atoi(var2);
    else value2= find_variable_value(var2);
    int result=value1-value2;
    assign(var3,result);
}

void multiply(char var1[], char var2[], char var3[]){
    int value1,value2;
    if (var1[0]<='9' &&  var1[0]>='0') value1= atoi(var1);
    else value1 = find_variable_value(var1);
    if (var2[0]<='9' &&  var2[0]>='0') value2= atoi(var2);
    else value2= find_variable_value(var2);
    int result=value2*value1;
    assign(var3,result);
}

void divide(char var1[], char var2[], char var3[]){
    int value1,value2;
    if (var1[0]<='9' &&  var1[0]>='0') value1= atoi(var1);
    else value1 = find_variable_value(var1);
    if (var2[0]<='9' &&  var2[0]>='0') value2= atoi(var2);
    else value2= find_variable_value(var2);
    if(value2==0) printf("Division by zero");
    else{
        int result=value1/value2;
        assign(var3,result);
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
    int value1,value2;
    if (var1[0]<='9' &&  var1[0]>='0') value1= atoi(var1);
    else value1 = find_variable_value(var1);
    if (var2[0]<='9' &&  var2[0]>='0') value2= atoi(var2);
    else value2= find_variable_value(var2);
    int result=value2%value1;
    assign(var3,result);
}


void load_program(FILE *file) {
    char buffer[LINE_LEN];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
        if (buffer[0] == '\0' || buffer[0] == '#') continue;
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
        push(*current_line + 1); // Save return point
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

int find_mem_addr(char* arg){
    if (arg[0] != '[') return -1;
    char* end_bracket = strchr(arg, ']');
    if(!end_bracket) return -1;
    int len = end_bracket - arg -1;
    char* c = arg;
    int addr = 0;
    for(c = arg+1; isdigit(*c); c++){
        addr = (addr * 10) + ((*c)-'0');
    }
    return addr;
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
