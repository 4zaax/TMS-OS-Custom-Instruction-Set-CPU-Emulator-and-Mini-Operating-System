//
// Created by 4zaax on 17/06/2025.
//
#include "stdio.h"
#include "stdlib.h"
int main(){
    char* command = "MOV";
    int start_index = 190;
    char* mssg = "run calculator";
    char* c = mssg;
    for(c = mssg; *c != '\0'; c++, start_index++){
        printf("%s \t [%d] \t '%c'\n",command, start_index, *c);
    }
    printf("%s \t [%d] \t %d\n",command, start_index, '\0');
}


