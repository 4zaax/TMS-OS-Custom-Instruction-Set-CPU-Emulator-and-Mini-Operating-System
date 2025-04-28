#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // opening boot file which is placed in the parent folder of our .exe program
    FILE *file = fopen("../boot.txt", "r");
    if (file == NULL) {
        printf("Failed to open boot file.\n");
        return 1;
    }

    char line[1024];

    while (fgets(line, sizeof(line), file)) {

        line[strcspn(line, "\n")] = 0;
        char command[10], arg1[10], arg2[10], arg3[10];
        int tokens = sscanf(line, "%s %s %s %s", command, arg1, arg2, arg3);

        printf("Command: %s\n", command);
        if (tokens > 1) printf("Arg1: %s\n", arg1);
        if (tokens > 2) printf("Arg2: %s\n", arg2);
        if (tokens > 3) printf("Arg3: %s\n", arg3);
        printf("----------------------\n");

    }

    fclose(file);
    return 0;
}
