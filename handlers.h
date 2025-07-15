#ifndef HANDLERS_H
#define HANDLERS_H

#define MEMORY_SIZE 128000 // RAM
#define MAX_LINES 500   // OS supports programs of maximum 500 lines
#define LINE_LEN 512
#define MAX_VARIABLES 1000

extern uint8_t memory[];
extern uint8_t reg_mem[];
extern uint8_t flags_mem[];
extern uint8_t* ZF;
extern uint8_t* SF;
extern uint8_t* CF;
extern uint8_t* OF;
extern uint32_t* esi;
extern uint32_t* edi;
extern uint32_t* ebp;
extern uint32_t* esp;
extern uint32_t* ip;
extern uint32_t* cs;

// Bitwise
void and_handler(const char*, const char*, const char*);
void andw_handler(const char*, const char*, const char*);
void andb_handler(const char*, const char*, const char*);

void or_handler(const char*, const char*, const char*);
void orw_handler(const char*, const char*, const char*);
void orb_handler(const char*, const char*, const char*);

void xor_handler(const char*, const char*, const char*);
void xorw_handler(const char*, const char*, const char*);
void xorb_handler(const char*, const char*, const char*);

void not_handler(const char*, const char*);
void notw_handler(const char*, const char*);
void notb_handler(const char*, const char*);

// Shifts
void shl_handler(const char*, const char*, const char*);
void shlw_handler(const char*, const char*, const char*);
void shlb_handler(const char*, const char*, const char*);

void shr_handler(const char*, const char*, const char*);
void shrw_handler(const char*, const char*, const char*);
void shrb_handler(const char*, const char*, const char*);


void inc_handler(const char* reg);
void incw_handler(const char* reg);
void incb_handler(const char* reg);

void dec_handler(const char* reg);
void decw_handler(const char* reg);
void decb_handler(const char* reg);

void modulo(char var1[], char var2[], char destination[]);

void showd(const char* reg);
void showw(const char* reg);
void showb(const char* reg);
void showc(const char* reg); // shows character value

void showsd(const char* reg);
void showsw(const char* reg);
void showsb(const char* reg);

void showd_mem(int addr);
void showw_mem(int addr);
void showb_mem(int addr);
void showsd_mem(int addr);
void showsw_mem(int addr);
void showsb_mem(int addr);

void mov_handler (char* dest, char* src);
void movb_handler(char* dest, char* src);
void movw_handler(char* dest, char* src);

// ADD: dword/word/byte
void add_handler(const char* dest, const char* src1, const char* src2);    // 32-bit (dword)
void addw_handler(const char* dest, const char* src1, const char* src2);   // 16-bit (word)
void addb_handler(const char* dest, const char* src1, const char* src2);   // 8-bit  (byte)

// SUB: dword/word/byte
void sub_handler(const char* dest, const char* src1, const char* src2);    // 32-bit (dword)
void subw_handler(const char* dest, const char* src1, const char* src2);   // 16-bit (word)
void subb_handler(const char* dest, const char* src1, const char* src2);   // 8-bit  (byte)

// Unsigned multiplication handlers
void mul_handler(const char* dest, const char* src1, const char* src2);    // 32-bit
void mulw_handler(const char* dest, const char* src1, const char* src2);   // 16-bit
void mulb_handler(const char* dest, const char* src1, const char* src2);   // 8-bit

// Signed multiplication handlers
void imul_handler(const char* dest, const char* src1, const char* src2);   // 32-bit
void imulw_handler(const char* dest, const char* src1, const char* src2);  // 16-bit
void imulb_handler(const char* dest, const char* src1, const char* src2);  // 8-bit

// Unsigned division
void div_handler(const char* dest, const char* src1, const char* src2);    // 32-bit
void divw_handler(const char* dest, const char* src1, const char* src2);   // 16-bit
void divb_handler(const char* dest, const char* src1, const char* src2);   // 8-bit

// Signed division
void idiv_handler(const char* dest, const char* src1, const char* src2);   // 32-bit
void idivw_handler(const char* dest, const char* src1, const char* src2);  // 16-bit
void idivb_handler(const char* dest, const char* src1, const char* src2);  // 8-bit

// Stack
void push_handler(const char* src);       // 32-bit
void pushw_handler(const char* src);      // 16-bit
void pushb_handler(const char* src);      // 8-bit

void pop_handler(const char* dest);       // 32-bit
void popw_handler(const char* dest);      // 16-bit
void popb_handler(const char* dest);      // 8-bit

void set_flags_add32(uint32_t, uint32_t, uint32_t);
void set_flags_add16(uint16_t, uint16_t, uint16_t);
void set_flags_add8(uint8_t, uint8_t, uint8_t);

void set_flags_sub32(uint32_t, uint32_t, uint32_t);
void set_flags_sub16(uint16_t, uint16_t, uint16_t);
void set_flags_sub8(uint8_t, uint8_t, uint8_t);

// Unsigned MUL flag setters
void set_flags_mul32(uint32_t op1, uint32_t op2, uint32_t result);
void set_flags_mul16(uint16_t op1, uint16_t op2, uint16_t result);
void set_flags_mul8 (uint8_t  op1, uint8_t  op2, uint8_t  result);

// Signed IMUL flag setters
void set_flags_imul32(int32_t op1, int32_t op2, int32_t result);
void set_flags_imul16(int16_t op1, int16_t op2, int16_t result);
void set_flags_imul8 (int8_t  op1, int8_t  op2, int8_t  result);

void jmp_handler(const char* label,const char* arg, int* current_line, int tokens);
void je_handler(const char* label, int* current_line);
void jne_handler(const char* label, int* current_line);
void jg_handler(const char* label, int* current_line);
void jl_handler(const char* label, int* current_line);

void call_handler(const char* label, int* current_line);
void ret_handler(const char* arg1, int tokens, int* current_line);

#endif