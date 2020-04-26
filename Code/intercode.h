#ifndef __INTERCODE_H__
#define __INTERCODE_H__

typedef struct Operand_ Operand;

struct Operand_ {
    enum { VARIABLE_V, VARIABLE_T, CONSTANT_INT, CONSTANT_FLOAT, ADDRESS} kind;
    Type* type;
    union {
        int var_no;
        int int_value;
        float float_value;
        // ...
    };
};

typedef struct InterCode_ InterCode;
struct InterCode_
{
    enum { INTER_ASSIGN, INTER_ADD, INTER_SUB, INTER_MUL, INTER_DIV, INTER_PARAM, INTER_FUNCTION, INTER_CALL, INTER_ARG, INTER_RETURN, INTER_READ, INTER_WRITE } kind;
    union {
        struct { Operand *right, *left; };
        struct { Operand *result, *op1, *op2; };
        int var_no;
        struct { char* func_name; Operand* ret; };
        Operand* arg;
        // ...
    };
    struct InterCode_ *prev, *next; 
};

// typedef struct InterCodes_ InterCodes;
// struct InterCodes_ { 
//     InterCode code;
//     struct InterCodes_ *prev, *next; 
// };
InterCode* code_head;
InterCode* code_tail;
int temp_cnt, inter_var_cnt, label_cnt;
int new_var_no();
int new_temp_no();
void intercode_init();
Operand* new_operand_float(float val);
Operand* new_operand_int(int val);
Operand* new_operand_temp_var(Type* type);
Operand* new_operand_var(int var_no, Type* type);
void add_code(InterCode* code);

void gen_code_plus(Operand* op, Operand* op1, Operand* op2);
void gen_code_minus(Operand* op, Operand* op1, Operand* op2);
void gen_code_star(Operand* op, Operand* op1, Operand* op2);
void gen_code_div(Operand* op, Operand* op1, Operand* op2);
void gen_code_assign(Operand* op1, Operand* op2);
void gen_code_param(int var_no);
void gen_code_func(char* func_name);
void gen_code_call(Operand* op, char* func_name);
void gen_code_arg(Operand* op);
void gen_code_neg(Operand* op, Operand* op1);
void gen_code_read(Operand* op);
void gen_code_write(Operand* op);


void gen_code_return(Operand* op);



FILE * fp_intercode;

#endif