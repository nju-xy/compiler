#include "common.h"
void intercode_init() {
    temp_cnt = 0;
    inter_var_cnt = 0;
    label_cnt = 0;
    ir_head = NULL;
    ir_tail = NULL;
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
    return new_op;
}

Operand* new_operand_float(float val) {
    Operand* new_op = (Operand*)malloc(sizeof(Operand));
    new_op->kind = CONSTANT_FLOAT;
    new_op->type = new_type_float();
    new_op->float_value = val;
    return new_op;
}

Operand* new_operand_temp_var(Type* type) {
    Operand* new_op = (Operand*)malloc(sizeof(Operand));
    new_op->kind = VARIABLE_T;
    new_op->type = type;
    new_op->var_no = new_temp_no();
    return new_op;
}

Operand* new_operand_temp_addr(Type* type) {
    Operand* new_op = (Operand*)malloc(sizeof(Operand));
    new_op->kind = ADDRESS_T;
    new_op->type = type;
    new_op->var_no = new_temp_no();
    return new_op;
}

Operand* new_operand_var(int var_no, Type* type) {
    Operand* new_op = (Operand*)malloc(sizeof(Operand));
    new_op->kind = VARIABLE_V;
    new_op->type = type;
    new_op->var_no = var_no;
    return new_op;
}

void add_code(InterCode* code) {
    if(ir_head == NULL) {
        ir_head = ir_tail = code;
    }
    else {
        ir_tail->next = code;
        code->prev = ir_tail;
        ir_tail = code;
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
    if(op->kind == VARIABLE_T || op->kind == ADDRESS_T) {
        name = (char*)malloc(sizeof(char) * (sz + 3));
        sprintf(name, "t%d", op->var_no);
    }
    else if(op->kind == VARIABLE_V || op->kind == ADDRESS_V){
        name = (char*)malloc(sizeof(char) * (sz + 3));
        sprintf(name, "v%d", op->var_no);
    }
    else if(op->kind == CONSTANT_INT) {
        name = (char*)malloc(33);
        snprintf(name, 32, "#%d", op->int_value);
    }
    else if(op->kind == CONSTANT_FLOAT) {
        name = (char*)malloc(65);
        snprintf(name, 64, "#%f", op->float_value);
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
    //Log("%s := %s + %s", operand_name(op), operand_name(op1), operand_name(op2));
    // fprintf(fp_intercode, "%s := %s + %s\n", operand_name(code->result), operand_name(code->op1), operand_name(code->op2));
}

void gen_code_minus(Operand* op, Operand* op1, Operand* op2) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_SUB;
    code->result = op;
    code->op1 = op1;
    code->op2 = op2;
    add_code(code);
    //Log("%s := %s - %s", operand_name(op), operand_name(op1), operand_name(op2));
    // fprintf(fp_intercode, "%s := %s - %s\n", operand_name(code->result), operand_name(code->op1), operand_name(code->op2));
}

void gen_code_star(Operand* op, Operand* op1, Operand* op2) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_MUL;
    code->result = op;
    code->op1 = op1;
    code->op2 = op2;
    add_code(code);
    //Log("%s := %s * %s", operand_name(op), operand_name(op1), operand_name(op2));
    // fprintf(fp_intercode, "%s := %s * %s\n", operand_name(code->result), operand_name(code->op1), operand_name(code->op2));
}

void gen_code_div(Operand* op, Operand* op1, Operand* op2) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_DIV;
    code->result = op;
    code->op1 = op1;
    code->op2 = op2;
    add_code(code);
    //Log("%s := %s / %s", operand_name(op), operand_name(op1), operand_name(op2));
    // fprintf(fp_intercode, "%s := %s / %s\n", operand_name(code->result), operand_name(code->op1), operand_name(code->op2));
}

void gen_code_param(int var_no) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_PARAM;
    code->var_no = var_no;
    add_code(code);
    //Log("PARAM v%d", var_no);
    // fprintf(fp_intercode, "PARAM v%d\n", code->var_no);
}

void gen_code_func(char* func_name) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_FUNCTION;
    code->func_name = func_name;
    add_code(code);
    //Log("FUNCTION %s :", func_name);
    // fprintf(fp_intercode, "FUNCTION %s :\n", code->func_name);
}

void gen_code_assign(Operand* op1, Operand* op2) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_ASSIGN;
    code->left = op1;
    code->right = op2; 
    add_code(code);
    //Log("%s := %s", operand_name(op1), operand_name(op2));
    // fprintf(fp_intercode, "%s := %s\n", operand_name(code->left), operand_name(code->right));
}

void gen_code_right_pointer(Operand* op1, Operand* op2) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_RIGHT_POINTER;
    code->left = op1;
    code->right = op2; 
    add_code(code);
    //Log("%s := *%s", operand_name(op1), operand_name(op2));
    // fprintf(fp_intercode, "%s := *%s\n", operand_name(code->left), operand_name(code->right));
}

void gen_code_left_pointer(Operand* op1, Operand* op2) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_LEFT_POINTER;
    code->left = op1;
    code->right = op2; 
    add_code(code);
    //Log("*%s := %s", operand_name(op1), operand_name(op2));
    // fprintf(fp_intercode, "*%s := %s\n", operand_name(code->left), operand_name(code->right));
}

void gen_code_addr(Operand* op1, Operand* op2) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_ADDR;
    code->left = op1;
    code->right = op2; 
    add_code(code);
    //Log("%s := &%s", operand_name(op1), operand_name(op2));
    // fprintf(fp_intercode, "%s := &%s\n", operand_name(code->left), operand_name(code->right));
}

void gen_code_call(Operand* op, char* func_name) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_CALL;
    code->func_name = func_name;
    code->ret = op;
    add_code(code);
    //Log("%s := CALL %s", operand_name(op), func_name);
    // fprintf(fp_intercode, "%s := CALL %s\n", operand_name(code->ret), code->func_name);
}

void gen_code_arg(Operand* op) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_ARG;
    code->op = op;
    add_code(code);
    //Log("ARG %s", operand_name(op));
    // fprintf(fp_intercode, "ARG %s\n", operand_name(code->op));
}

void gen_code_return(Operand* op) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_RETURN;
    code->op = op;
    add_code(code);
    //Log("RETURN %s", operand_name(op));
    // fprintf(fp_intercode, "RETURN %s\n", operand_name(code->op));
}

void gen_code_read(Operand* op) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_READ;
    code->op = op;
    add_code(code);
    //Log("READ %s", operand_name(op));
    // fprintf(fp_intercode, "READ %s\n", operand_name(code->op));
}

void gen_code_write(Operand* op) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_WRITE;
    code->op = op;
    add_code(code);
    //Log("WRITE %s", operand_name(op));
    // fprintf(fp_intercode, "WRITE %s\n", operand_name(code->op));
}

void gen_code_label(int label) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_LABEL;
    code->label = label;
    add_code(code);
    //Log("LABEL label%d :", label);
    // fprintf(fp_intercode, "LABEL label%d :\n", code->label);
}

void gen_code_goto(int label) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_GOTO;
    code->label = label;
    add_code(code);
    //Log("GOTO label%d", label);
    // fprintf(fp_intercode, "GOTO label%d\n", code->label);
}

char* relop_name(int relop) {
    if(relop == 0) 
        return "<";
    else if(relop == 1)
        return ">";
    else if(relop == 2)
        return "<=";
    else if(relop == 3)
        return ">=";
    else if(relop == 4)
        return "==";
    else if(relop == 5)
        return "!=";
    return "NULL";
}

void gen_code_if_goto(Operand* op1, int relop, Operand* op2, int label) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_IF_GOTO;
    code->if_goto.label = label;
    code->if_goto.relop = relop;
    code->if_goto.op1 = op1;
    code->if_goto.op2 = op2;
    add_code(code);
    //Log("IF %s %s %s GOTO label%d", operand_name(op1), relop_name(relop), operand_name(op2), label);
    // fprintf(fp_intercode, "IF %s %s %s GOTO label%d\n", operand_name(code->if_goto.op1), relop_name(code->if_goto.relop), operand_name(code->if_goto.op2), code->if_goto.label);
}

void gen_code_dec(int var_no, int width) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    code->kind = INTER_DEC;
    code->dec.var_no = var_no;
    code->dec.width = width;
    add_code(code);
    //Log("DEC v%d %d", var_no, width);
    // fprintf(fp_intercode, "DEC v%d %d\n", code->dec.var_no, code->dec.width);
}

void print_ir(InterCode* code) {
    switch (code->kind)
    {
    case INTER_ASSIGN: // 0
        /* code */
        fprintf(fp_intercode, "%s := %s\n", operand_name(code->left), operand_name(code->right));
        break;
    case INTER_ADD: // 1
        /* code */
        fprintf(fp_intercode, "%s := %s + %s\n", operand_name(code->result), operand_name(code->op1), operand_name(code->op2));
        break;
    case INTER_SUB: //2
        /* code */
        fprintf(fp_intercode, "%s := %s - %s\n", operand_name(code->result), operand_name(code->op1), operand_name(code->op2));
        break;
    case INTER_MUL: //3
        /* code */
        fprintf(fp_intercode, "%s := %s * %s\n", operand_name(code->result), operand_name(code->op1), operand_name(code->op2));
        break;
    case INTER_DIV: //4
        /* code */
        fprintf(fp_intercode, "%s := %s / %s\n", operand_name(code->result), operand_name(code->op1), operand_name(code->op2));
        break;
    case INTER_PARAM: //5
        /* code */
        fprintf(fp_intercode, "PARAM v%d\n", code->var_no);
        break;
    case INTER_FUNCTION: //6
        /* code */
        fprintf(fp_intercode, "FUNCTION %s :\n", code->func_name);
        break;
    case INTER_CALL: //7
        /* code */
        fprintf(fp_intercode, "%s := CALL %s\n", operand_name(code->ret), code->func_name);
        break;
    case INTER_ARG: //8
        /* code */
        fprintf(fp_intercode, "ARG %s\n", operand_name(code->op));
        break;
    case INTER_RETURN: //9
        /* code */
        fprintf(fp_intercode, "RETURN %s\n", operand_name(code->op));
        break;
    case INTER_READ: //10
        /* code */
        fprintf(fp_intercode, "READ %s\n", operand_name(code->op));
        break;
    case INTER_WRITE: //11
        /* code */
        fprintf(fp_intercode, "WRITE %s\n", operand_name(code->op));
        break;
    case INTER_LABEL: //12
        /* code */
        fprintf(fp_intercode, "LABEL label%d :\n", code->label);
        break;
    case INTER_GOTO: //13
        /* code */
        fprintf(fp_intercode, "GOTO label%d\n", code->label);
        break;
    case INTER_IF_GOTO: //14
        /* code */
        fprintf(fp_intercode, "IF %s %s %s GOTO label%d\n", operand_name(code->if_goto.op1), relop_name(code->if_goto.relop), operand_name(code->if_goto.op2), code->if_goto.label);
        break;
    case INTER_DEC: //15
        /* code */
        fprintf(fp_intercode, "DEC v%d %d\n", code->dec.var_no, code->dec.width);
        break;
    case INTER_LEFT_POINTER: //16
        /* code */
        fprintf(fp_intercode, "*%s := %s\n", operand_name(code->left), operand_name(code->right));
        break;
    case INTER_RIGHT_POINTER: //17
        /* code */
        fprintf(fp_intercode, "%s := *%s\n", operand_name(code->left), operand_name(code->right));
        break;
    case INTER_ADDR: //18
        /* code */
        fprintf(fp_intercode, "%s := &%s\n", operand_name(code->left), operand_name(code->right));
        break;
    default:
        assert(0);
        break;
    }
}

void print_all_ir() {
    InterCode* ir = ir_head;
    while(ir != NULL) {
        print_ir(ir);
        ir = ir->next;
    }
}

/*******************优化部分*******************************/
void ir_optimizer() {
    print_all_ir();
}