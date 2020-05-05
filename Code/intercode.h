#ifndef __INTERCODE_H__
#define __INTERCODE_H__

typedef struct Operand_ Operand;

struct Operand_ {
    enum { VARIABLE_V, VARIABLE_T, CONSTANT, ADDRESS_V, ADDRESS_T } kind; // 操作符本身的类型
    Type* type;
    union {
        int var_no;
        int int_value;
        // float float_value;
    };
    // 操作符前面要不要加&或者*
    enum { NOTHING, PRE_AND, PRE_STAR } pre;
};

typedef struct InterCode_ InterCode;
struct InterCode_
{
    enum { INTER_ASSIGN, INTER_ADD, INTER_SUB, INTER_MUL, INTER_DIV, INTER_PARAM, INTER_FUNCTION, INTER_CALL, INTER_ARG, INTER_RETURN, INTER_READ, INTER_WRITE, INTER_LABEL, INTER_GOTO, INTER_IF_GOTO, INTER_DEC, INTER_LEFT_POINTER, INTER_RIGHT_POINTER, INTER_ADDR } kind; // 19个
    union {
        Operand* op; 
        // ARG, RET, WRITE不改op
        // READ 改op
        struct { Operand *left, *right; }; 
        // ASSIGN, RIGHT_POINTER, ADDR 改op
        // LEFT_POINTET 不改left
        struct { Operand *result, *op1, *op2; };// ADD, SUB, MUL, DIV都改result
        int var_no; // PARAM
        int label; // LABEL, GOTO
        struct { Operand *op1, *op2; int label; int relop;} if_goto; // IF_GOTO
        struct { char* func_name; Operand* ret; }; // FUNC, CALL
        struct { int var_no, width; } dec; // DEC
        // ...
    };
    struct InterCode_ *prev, *next; 
};

// typedef struct InterCodes_ InterCodes;
// struct InterCodes_ { 
//     InterCode code;
//     struct InterCodes_ *prev, *next; 
// };
InterCode* ir_head;
InterCode* ir_tail;
int temp_cnt, inter_var_cnt, label_cnt;
int new_var_no();
int new_temp_no();
int new_label();
void intercode_init();
char* operand_name(Operand* op);
// Operand* new_operand_float(float val);
Operand* new_operand_int(int val);
Operand* new_operand_temp_var(Type* type);
Operand* new_operand_temp_addr(Type* type);
Operand* new_operand_var(int var_no, Type* type);
void add_code(InterCode* code);

void gen_code_plus(Operand* op, Operand* op1, Operand* op2);
void gen_code_minus(Operand* op, Operand* op1, Operand* op2);
void gen_code_star(Operand* op, Operand* op1, Operand* op2);
void gen_code_div(Operand* op, Operand* op1, Operand* op2);
void gen_code_assign(Operand* op1, Operand* op2);
void gen_code_addr(Operand* op1, Operand* op2);
void gen_code_param(int var_no);
void gen_code_func(char* func_name);
void gen_code_call(Operand* op, char* func_name);
void gen_code_arg(Operand* op);
void gen_code_neg(Operand* op, Operand* op1);
void gen_code_read(Operand* op);
void gen_code_write(Operand* op);

void gen_code_right_pointer(Operand* op1, Operand* op2);
void gen_code_left_pointer(Operand* op1, Operand* op2);

void gen_code_return(Operand* op);
void gen_code_label(int label);
void gen_code_goto(int label);
void gen_code_if_goto(Operand* operand1, int relop, Operand* operand2, int label);
void gen_code_dec(int var_no, int width);

void ir_optimizer();

FILE * fp_intercode;
#endif