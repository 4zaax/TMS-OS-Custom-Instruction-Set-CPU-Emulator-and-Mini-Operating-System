typedef struct {
    char name[32];
    int addr;
} Symbol;

Symbol symbol_table[1000];
int symbol_count = 0;