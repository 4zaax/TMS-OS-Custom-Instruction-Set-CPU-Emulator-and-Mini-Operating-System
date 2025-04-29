#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

#define MEMORY_SIZE 200
int var[MEMORY_SIZE];

FILE *file;

int main() {

    file = open_file("../boot.txt");
    if (file == NULL) printf("Shutting down OS\n");else  printf("boot file loaded successfully\n");

    char line[1024];

    while (fgets(line, sizeof(line), file/*stdin*/)) {
        line[strcspn(line, "\n")] = '\0'; //replacing delimiter with null terminator
        char command[10], arg1[10], arg2[10], arg3[10];
        int tokens = sscanf(line, "%s %s %s %s", command, arg1, arg2, arg3);
        if (tokens < 1) continue;
        check(command,arg1,arg2,arg3);
    }
    fclose(file);
    return 0;
}


FILE* open_file(char address[]){
    FILE *file = fopen(address, "r");
    if (file == NULL) {
        printf("Failed to open boot file %s.\n",address);
        return NULL;
    }
    return file;
}

int check(char command[], char arg1[], char arg2[], char arg3[]){
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

int compare(char var1[], char value[], char destination[]){
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

int equal_test(char var1[], char value[], char destination[]){
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