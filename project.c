#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_VARIABLES 1000
#define MAX_VARIABLES 100
#include <ctype.h>

void replace_list(char* original, int start, int end, char y) {
    char temp[1024];
    strncpy(temp, original, start);
    temp[start] = '\0';
    char str_y[2] = {y, '\0'};
    strcat(temp, str_y);
    strcat(temp, original + end + 1);
    strcpy(original, temp);
}
int is_number(const char* str) {
    int number=0;
    int i = 0;
    if (str[0] == '\0') return 0;
    while (str[i] != '\0') {
        if (!isdigit(str[i])){
            return 0;
        }
        else{number=(number*10)+(str[i]-'0');
        }
        i++;
    }
    return number;
}


typedef struct {
    char name[50];
    char value[100];
} Variable;
Variable variables[MAX_VARIABLES];
int var_count = 0;


void get_condition(char condition[],char line[]){
    int i=0;
    int k=0;
    int count_1=0;
    int count_2=0;
    while (line[i]!='\0'){
        if(line[i]=='('){
            count_1++;}
        else if(line[i]==')'){count_2++;}
        if(count_1>0  &&  count_1!=count_2){
            if(line[i]!='(' || k!=0 ){condition[k++]=line[i];}
        }
        i++;
    }
    condition[k]='\0';
}


int find_variable(char* name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return i;

        }
    }
    return -1;
}



void set_variable(char* name, char* value) {
    int index = find_variable(name);
    if (index == -1) {
        strcpy(variables[var_count].name, name);
        strcpy(variables[var_count].value, value);
        var_count++;
    } else {
        strcpy(variables[index].value, value);
    }
}
char* get_variable(char* name) {
    int index = find_variable(name);
    if (index == -1) {
        return NULL;
    } else {
        char* value = variables[index].value;
        return value;

    }
}
char check_coundition(char condition[]){
    char list[100];
    int k=0;
    char var1[50];
    char var2[50];
    char operator[30];
    int count1=0;
    int index=0;
    int count2=0;
    for  (int i=0 ;condition[i]!='\0';i++){
        if(condition[i] =='(' ){ count1++;}
        else if(condition[i]==')'){count2++;}
         if(count1>0  && count2!=count1){
             if (condition[i]!='(' || k!=0){
                 list[k++]=condition[i];
             }
             else if(condition[i]=='(' && k==0){ index=i;
                 }
         }

        else if (count1==count2 && count1!=0){


            list[k]='\0';char y=(check_coundition(list));
        k=0;

        count1=0;
        count2=0;

        replace_list(condition,index,i,y);
             i=index;

        }

        else if (condition[i+1]=='\0'){

            int var1_int;
            int var2_int;

            sscanf(condition, "%s %s %s",var1,operator,var2 );
            if (find_variable(var1)!=-1){
                 strcpy(var1,get_variable(var1));
                }
            if(find_variable(var2)!=-1){

               strcpy(var2,(get_variable(var2)));
                }
            var1_int= is_number(var1);
            var2_int= is_number(var2);

            if (strcmp(operator, "==") == 0 && (var2_int!=0) &&(var1_int!=0)) {return ((var1_int == var2_int)+'0');}
            else if (strcmp(operator, "==") == 0 && (var2_int==0) &&(var1_int==0)) {;return ((!strcmp(var1,var2))+'0');}
            else if (strcmp(operator, "!=") == 0 && (var2_int!=0) &&(var1_int!=0)) {return ((var1_int != var2_int)+'0');}
            else if (strcmp(operator, "!=") == 0 && (var2_int==0) &&(var1_int==0)) {return (!(strcmp(var1,var2))+'0');}
            else if (strcmp(operator, "<") == 0 ) {return ((var1_int < var2_int)+'0');}
            else if (strcmp(operator, "<=") == 0 ) {return ((var1_int <= var2_int)+'0');}
            else if (strcmp(operator, ">") == 0 ) { return ((var1_int > var2_int)+'0');}
            else if (strcmp(operator, ">=") == 0 ) {return ((var1_int >= var2_int)+'0');}
            else if (strcmp(operator, "and") == 0 ) {return ((var1_int && var2_int)+'0');}
            else if (strcmp(operator, "or") == 0 ) {return ((var1_int  || var2_int)+'0');}
            break;
        }
    }

}
int main(){

    int number[1000];
    FILE *fp = fopen("input.txt", "r");
    if (fp == NULL) {
        printf("nope");
        return 1;
    }
    char line[2560];
    while (fgets(line, sizeof(line), fp)){

        line[strcspn(line, "\n")] = '\0';
        if (strncmp(line, "set", 3) == 0){

            char value[50];
            char name[50];
            sscanf(line, "set %s %s", &name, &value);

            set_variable( name,  value);
        }

        else if (strncmp(line, "print", 5) == 0){

            char list[1001];
            sscanf(line, "%*[^(](%[^)]",list);


            int count=0;
            for(int i=0;list[i]!='\0';i++){
                if(list[i]=='\"'){count++;}
                else if (count%2==1){ printf("%c",list[i]);}
                else if(list[i]==' '){ printf(" ");}
                else {
                    char namc1[50];
                    int k=0;
                    while(list[i]!='\0' && list[i]!=',' && list[i]!=' ' && list[i]!='\"'){
                        namc1[k++]=list[i++];
                    }
                    namc1[k]='\0';
                    if(k==0){continue;}

                    if ((is_number(get_variable(namc1)))!=0){

                        printf("%d", is_number(get_variable(namc1)));
                    }else { printf("%s", get_variable(namc1));}

                    if(list[i]=='\0'){break;}
                }
            }
            printf("\n");
        }

        else if(strncmp(line,"if",2) == 0){

            char condition[1000];
            get_condition(condition,line);

            if (check_coundition(condition)=='1') {

                continue;

                }
            else{
                while (fgets(line, sizeof(line), fp)){
                    if (line[0]=='}'){break;}
                }


            }
            }

        }
    fclose(fp);
    return 0;
    }
