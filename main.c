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
    if (strcmp(command,"give") == 0)
        assign(arg1, arg2);
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
