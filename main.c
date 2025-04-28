#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

FILE *file;

int main() {

    file = open_file("../boot.txt");
    if (file == NULL) printf("Shutting down OS\n");else  printf("boot file loaded successfully\n");
    parse_command();
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

void parse_command(void){
    char line[1024];
    while (fgets(line, sizeof(line), file/*stdin*/)) {
        line[strcspn(line, "\n")] = '\0'; //replacing delimiter with null terminator
        char command[10], arg1[10], arg2[10], arg3[10];
        int tokens = sscanf(line, "%s %s %s %s", command, arg1, arg2, arg3);
    }
}