#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

#define MEMORY_SIZE 200 // store our variables
#define MAX_LINES 500   // os supports programs of maximum 500 lines
#define LINE_LEN 256    // each line of program should be less than 256 characters
#define STACK_SIZE 100  // os stack size

int var[MEMORY_SIZE];
int call_stack[STACK_SIZE];
int sp = -1; //stack pointer
FILE *file;

typedef struct {
    char line[LINE_LEN];
    char label[20];
} LineEntry;

LineEntry program[MAX_LINES];
int program_size = 0;

int main() {

    file = open_file("../boot.txt");
    if (file == NULL) printf("Shutting down OS\n");else  printf("boot file loaded successfully\n");
    load_program(file);
    fclose(file);
    for (int current_line = 1 ; current_line <= program_size; current_line++){
        if(program[current_line].line[0] == '\0')continue;
        char command[10],arg1[10],arg2[10],arg3[10];
        int tokens = sscanf(program[current_line].line, "%s %s %s %s", command, arg1, arg2, arg3);
        if (tokens < 1)continue;
        check(command, arg1, arg2, arg3, &current_line);
    }

    file = open_file("../boot.txt");
    char line[1024];
}


FILE* open_file(char address[]){
    FILE *file = fopen(address, "r");
    if (file == NULL) {
        printf("Failed to open boot file %s.\n",address);
        return NULL;
    }
    return file;
}

int check(char command[], char arg1[], char arg2[], char arg3[], int *current_line){
    if (strcmp(command,"as") == 0)
        assign(arg1, arg2);
    else if (strcmp(command,"com") == 0)
        compare(arg1, arg2, arg3);
    else if (strcmp(command,"eq") == 0)
        equal_test(arg1, arg2, arg3);
    else if (strcmp(command,"add") == 0)
        addition(arg1, arg2, arg3);
    else if (strcmp(command,"sub") == 0)
        subtract(arg1, arg2, arg3);
    else if (strcmp(command,"mul") == 0)
        multiply(arg1, arg2, arg3);
    else if (strcmp(command,"div") == 0)
        divide(arg1, arg2, arg3);
    else if (strcmp(command,"show") == 0)
        show(arg1);
    else if (strcmp(command,"mod") == 0)
        modulo(arg1, arg2, arg3);
    else if (strcmp(command,"goto") == 0)
        go2(arg1,current_line);
    else if (strcmp(command,"return") == 0)
        *current_line = pop();
    else
        printf("Unknown command: %s\n",command);
}

void assign(char destination[], char value[]){
    int var_id = get_var_id(destination);
    int val = atoi(value);
    if (var_id >= 0 && var_id < MEMORY_SIZE){
        var[var_id] = val;
    }
}

int get_var_id(char variable[]){
    if (variable[0] != 'v') return -1;
    return atoi(variable + 1);
}

void compare(char var1[], char value[], char destination[]){
    int source_id = get_var_id(var1);
    int source = var[source_id];
    int val = atoi(value);
    int result;
    if (source > val)
        result = 1;
    else if (source < val)
        result = -1;
    else
        result = 0;
    int dest_id = get_var_id(destination);
    var[dest_id] = result;
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

void addition(char var1[], char var2[], char destination[]){
    int var1_id = get_var_id(var1);
    int var2_id = get_var_id(var2);
    int var3_id = get_var_id(destination);
    var[var3_id] = (var[var1_id]+var[var2_id]);
}

void subtract(char var1[], char var2[], char destination[]){
    int var1_id = get_var_id(var1);
    int var2_id = get_var_id(var2);
    int var3_id = get_var_id(destination);
    var[var3_id] = (var[var1_id]-var[var2_id]);
}

void multiply(char var1[], char var2[], char destination[]){
    int var1_id = get_var_id(var1);
    int var2_id = get_var_id(var2);
    int var3_id = get_var_id(destination);
    var[var3_id] = (var[var1_id]*var[var2_id]);
}

void divide(char var1[], char var2[], char destination[]){
    int var1_id= get_var_id(var1);
    int var2_id = get_var_id(var2);
    int var3_id = get_var_id(destination);
    if (var[var2_id] == 0)
        printf("Division by zero\n");
    else
        var[var3_id] = (var[var1_id]/var[var2_id]);
}
void modulo(char var1[], char var2[], char destination[]){
    int var1_id= get_var_id(var1);
    int var2_id = get_var_id(var2);
    int var3_id = get_var_id(destination);
    if (var[var2_id] == 0)
        printf("Modulo by zero\n");
    else
        var[var3_id] = (var[var1_id]%var[var2_id]);
}
void show(char variable[]){ // makes debugging easier
    int var_id = get_var_id(variable);
    printf("%s value is: %d \n",variable, var[var_id]);
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