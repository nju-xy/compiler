#include "common.h"
void intercode_init() {
    temp_cnt = 0;
    inter_var_cnt = 0;
    label_cnt = 0;
    code_head = NULL;
    code_tail = NULL;
    fp_intercode = fopen("../intercode/test.ir", "w+");
}

int new_temp_no() {
    temp_cnt++;
    return temp_cnt;
}

int new_var_no() {
    inter_var_cnt++;
    return inter_var_cnt;
}

int new_label() {
    label_cnt++;
    return label_cnt;
}

Operand* new_operand_int(int val) {
    Operand* new_op = (Operand*)malloc(sizeof(Operand));
    new_op->kind = CONSTANT_INT;
    new_op->type = new_type_int();
    new_op->int_value = val;
}

Operand* new_operand_float(float val) {
    Operand* new_op = (Operand*)malloc(sizeof(Operand));
    new_op->kind = CONSTANT_FLOAT;
    new_op->type = new_type_float();
    new_op->float_value = val;
}

Operand* new_operand_temp_var(Type* type) {
    Operand* new_op = (Operand*)malloc(sizeof(Operand));
    new_op->kind = VARIABLE_T;
    new_op->type = type;
    new_op->var_no = new_temp_no();
}

Operand* new_operand_var(int var_no, Type* type) {
    Operand* new_op = (Operand*)malloc(sizeof(Operand));
    new_op->kind = VARIABLE_V;
    new_op->type = type;
    new_op->var_no = var_no;
}

void add_code(InterCode* code) {
    if(code_head == NULL) {
        code_head = code_tail = code;
    }
    else {
        code_tail->next = code;
        code->prev = code_tail;
        code_tail = code;
    }
}

char* operand_name(Operand* op) {
    int sz = 0;
    int temp = op->var_no;
    while(temp) {
        sz ++;
        temp /= 10;
    }
    char* name = NULL;
    if(op->kind == VARIABLE_T) {
        name = (char*)malloc((sz + 2) * sizeof(char));
        sprintf(name, "t%d", op->var_no);
    }
    else if(op->kind == VARIABLE_V){
        name = (char*)malloc((sz + 2) * sizeof(char));
        sprintf(name, "v%d", op->var_no);
    }
    else if(op->kind == CONSTANT_INT) {
        name = (char*)malloc(32);
        sprintf(name, "#%d", op->int_value);
    }
    else if(op->kind == CONSTANT_INT) {
        name = (char*)malloc(32);
        sprintf(name, "#%f", op->float_value);
    }
    else {
        TODO();
    }
    return name;
}

void gen_code_plus(Operand* op, Operand* op1, Operand* op2) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_ADD;
    code->result = op;
    code->op1 = op1;
    code->op2 = op2;
    add_code(code);
    Log("%s := %s + %s", operand_name(op), operand_name(op1), operand_name(op2));
    fprintf(fp_intercode, "%s := %s + %s\n", operand_name(op), operand_name(op1), operand_name(op2));
}

void gen_code_minus(Operand* op, Operand* op1, Operand* op2) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_SUB;
    code->result = op;
    code->op1 = op1;
    code->op2 = op2;
    add_code(code);
    Log("%s := %s - %s", operand_name(op), operand_name(op1), operand_name(op2));
    fprintf(fp_intercode, "%s := %s - %s\n", operand_name(op), operand_name(op1), operand_name(op2));
}

void gen_code_star(Operand* op, Operand* op1, Operand* op2) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_MUL;
    code->result = op;
    code->op1 = op1;
    code->op2 = op2;
    add_code(code);
    Log("%s := %s * %s", operand_name(op), operand_name(op1), operand_name(op2));
    fprintf(fp_intercode, "%s := %s * %s\n", operand_name(op), operand_name(op1), operand_name(op2));
}

void gen_code_div(Operand* op, Operand* op1, Operand* op2) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_DIV;
    code->result = op;
    code->op1 = op1;
    code->op2 = op2;
    add_code(code);
    Log("%s := %s / %s", operand_name(op), operand_name(op1), operand_name(op2));
    fprintf(fp_intercode, "%s := %s / %s\n", operand_name(op), operand_name(op1), operand_name(op2));
}

void gen_code_param(int var_no) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_PARAM;
    code->var_no = var_no;
    add_code(code);
    Log("PARAM v%d", var_no);
    fprintf(fp_intercode, "PARAM v%d\n", var_no);
}

void gen_code_func(char* func_name) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_FUNCTION;
    code->func_name = func_name;
    add_code(code);
    Log("FUNCTION %s :", func_name);
    fprintf(fp_intercode, "FUNCTION %s :\n", func_name);
}

void gen_code_assign(Operand* op1, Operand* op2) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_ASSIGN;
    code->left = op1;
    code->right = op2; 
    add_code(code);
    Log("%s := %s", operand_name(op1), operand_name(op2));
    fprintf(fp_intercode, "%s := %s\n", operand_name(op1), operand_name(op2));
}

void gen_code_call(Operand* op, char* func_name) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_CALL;
    code->func_name = func_name;
    code->ret = op;
    add_code(code);
    Log("%s := CALL %s", operand_name(op), func_name);
    fprintf(fp_intercode, "%s := CALL %s\n", operand_name(op), func_name);
}

void gen_code_arg(Operand* op) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_ARG;
    code->arg = op;
    add_code(code);
    Log("ARG %s", operand_name(op));
    fprintf(fp_intercode, "ARG %s\n", operand_name(op));
}

// void gen_code_neg(Operand* op, Operand* op1) {
//     InterCode* code = (InterCode*)malloc(sizeof(InterCode));
//     code->kind = INTER_NEG;
//     code->left = op;
//     code->right = op1;
//     add_code(code);
//     Log("%s := - %s", operand_name(op), operand_name(op1));
//     fprintf(fp_intercode, "%s := - %s\n", operand_name(op), operand_name(op1));
// }

void gen_code_return(Operand* op) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_RETURN;
    code->arg = op;
    add_code(code);
    Log("RETURN %s", operand_name(op));
    fprintf(fp_intercode, "RETURN %s\n", operand_name(op));
}

void gen_code_read(Operand* op) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_READ;
    code->arg = op;
    add_code(code);
    Log("READ %s", operand_name(op));
    fprintf(fp_intercode, "READ %s\n", operand_name(op));
}

void gen_code_write(Operand* op) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_WRITE;
    code->arg = op;
    add_code(code);
    Log("WRITE %s", operand_name(op));
    fprintf(fp_intercode, "WRITE %s\n", operand_name(op));
}