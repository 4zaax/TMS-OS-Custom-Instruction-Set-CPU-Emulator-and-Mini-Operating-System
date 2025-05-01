FILE* open_file(char address[]);
int check(char command[], char arg1[], char arg2[], char arg3[]);
int get_var_id(char variable[]);
void assign(char value[], char destination[]);
int compare(char var1[], char value[], char destination[]);
int equal_test(char var1[], char value[], char destination[]);
void addition(char var1[], char var2[], char destination[]);
void subtract(char var1[], char var2[], char destination[]);
void multiply(char var1[], char var2[], char destination[]);
void divide(char var1[], char var2[], char destination[]);
void modulo(char var1[], char var2[], char destination[]);
void show(char variable[]);
void load_program(FILE *file);
void push(int val);
int pop();
int find_label(const char *label);




