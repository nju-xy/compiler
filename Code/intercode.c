#include "common.h"
int temp_cnt = 0, inter_var_cnt = 0, label_cnt = 0;
// FILE * fp_intercode;
InterCode* ir_head = NULL, * ir_tail = NULL;
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
    // new_op->pa_func = NULL;
    // new_op->offset = -1;
    new_op->kind = CONSTANT;
    new_op->type = new_type_int();
    new_op->int_value = val;
    new_op->pre = NOTHING;
    return new_op;
}

Operand* copy_operand(Operand* op) {
    Operand* new_op = (Operand*)malloc(sizeof(Operand));
    // new_op->pa_func = NULL;
    // new_op->offset = -1;
    new_op->kind = op->kind;
    new_op->type = op->type;
    new_op->int_value = op->int_value;
    new_op->pre = NOTHING;
    return new_op;
}

// Operand* new_operand_float(float val) {
//     Operand* new_op = (Operand*)malloc(sizeof(Operand));
//     new_op->pa_func = NULL;
//     new_op->offset = -1;
//     new_op->kind = CONSTANT_FLOAT;
//     new_op->type = new_type_float();
//     new_op->float_value = val;
//     // new_op->pre = NOTHING;
//     return new_op;
// }

Operand* new_operand_temp_var(Type* type) {
    Operand* new_op = (Operand*)malloc(sizeof(Operand));
    // new_op->pa_func = NULL;
    // new_op->offset = -1;
    new_op->kind = VARIABLE_T;
    new_op->type = type;
    new_op->var_no = new_temp_no();
    new_op->pre = NOTHING;
    return new_op;
}

Operand* new_operand_temp_addr(Type* type) {
    Operand* new_op = (Operand*)malloc(sizeof(Operand));
    // new_op->pa_func = NULL;
    // new_op->offset = -1;
    new_op->kind = ADDRESS_T;
    new_op->type = type;
    new_op->var_no = new_temp_no();
    new_op->pre = NOTHING;
    return new_op;
}

Operand* new_operand_var(int var_no, Type* type) {
    Operand* new_op = (Operand*)malloc(sizeof(Operand));
    // new_op->pa_func = NULL;
    // new_op->offset = -1;
    new_op->kind = VARIABLE_V;
    new_op->type = type;
    new_op->var_no = var_no;
    new_op->pre = NOTHING;
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
        code->next = NULL;
    }
}

char* operand_name(Operand* op) {
    assert(op);
    int sz = 0;
    int temp = op->var_no;
    while(temp) {
        sz ++;
        temp /= 10;
    }
    char* name = NULL;
    if(op->kind == VARIABLE_T || op->kind == ADDRESS_T) {
        name = (char*)malloc(sizeof(char) * (sz + 4));
        if(op->pre == NOTHING)
            sprintf(name, "t%d", op->var_no);
        else if(op->pre == PRE_STAR)
            sprintf(name, "*t%d", op->var_no);
        else if(op->pre == PRE_AND)
            sprintf(name, "&t%d", op->var_no);
    }
    else if(op->kind == VARIABLE_V || op->kind == ADDRESS_V){
         name = (char*)malloc(sizeof(char) * (sz + 4));
        if(op->pre == NOTHING)
            sprintf(name, "v%d", op->var_no);
        else if(op->pre == PRE_STAR)
            sprintf(name, "*v%d", op->var_no);
        else if(op->pre == PRE_AND)
            sprintf(name, "&v%d", op->var_no);
    }
    else if(op->kind == CONSTANT) {
        name = (char*)malloc(33);
        assert(op->pre == NOTHING);
        snprintf(name, 32, "#%d", op->int_value);
    }
    // else if(op->kind == CONSTANT_FLOAT) {
    //     name = (char*)malloc(65);
    //     snprintf(name, 64, "#%f", op->float_value);
    // }
    else {
        TODO();
    }
    return name;
}

void gen_code_plus(Operand* op, Operand* op1, Operand* op2) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    memset(code, 0, sizeof(InterCode));
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
    memset(code, 0, sizeof(InterCode));
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
    memset(code, 0, sizeof(InterCode));
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
    memset(code, 0, sizeof(InterCode));
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
    memset(code, 0, sizeof(InterCode));
    code->kind = INTER_PARAM;
    code->var_no = var_no;
    add_code(code);
    //Log("PARAM v%d", var_no);
    // fprintf(fp_intercode, "PARAM v%d\n", code->var_no);
}

void gen_code_func(char* func_name) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    memset(code, 0, sizeof(InterCode));
    code->kind = INTER_FUNCTION;
    code->func_name = func_name;

    add_code(code);
    //Log("FUNCTION %s :", func_name);
    // fprintf(fp_intercode, "FUNCTION %s :\n", code->func_name);
}

void gen_code_assign(Operand* op1, Operand* op2) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    memset(code, 0, sizeof(InterCode));
    code->kind = INTER_ASSIGN;
    code->left = op1;
    code->right = op2; 
    add_code(code);
    //Log("%s := %s", operand_name(op1), operand_name(op2));
    // fprintf(fp_intercode, "%s := %s\n", operand_name(code->left), operand_name(code->right));
}

void gen_code_right_pointer(Operand* op1, Operand* op2) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    memset(code, 0, sizeof(InterCode));
    code->kind = INTER_ASSIGN;
    code->left = op1;
    code->right = copy_operand(op2);
    if(code->right->pre == NOTHING) {
        code->right->pre = PRE_STAR;
    }
    else if(code->right->pre == PRE_AND) {
        code->right->pre = NOTHING;
    }
    else {
        assert(0);
    }
    if(code->right->kind == ADDRESS_T) {
        code->right->kind = VARIABLE_T;
    }
    else if(code->right->kind == ADDRESS_V) {
        code->right->kind = VARIABLE_V;
    }
    else {
        assert(0);
    }
    add_code(code);
    // Log("%s := *%s", operand_name(op1), operand_name(op2));
    // fprintf(fp_intercode, "%s := *%s\n", operand_name(code->left), operand_name(code->right));
}

void gen_code_left_pointer(Operand* op1, Operand* op2) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    memset(code, 0, sizeof(InterCode));
    code->kind = INTER_LEFT_POINTER;
    assert(op1->kind == ADDRESS_T || op1->kind == ADDRESS_V);
    assert(op1->pre == NOTHING);
    code->left = op1;
    code->right = op2; 
    add_code(code);
    // Log("*%s := %s", operand_name(op1), operand_name(op2));
    // fprintf(fp_intercode, "*%s := %s\n", operand_name(code->left), operand_name(code->right));
}

void gen_code_addr(Operand* op1, Operand* op2) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    memset(code, 0, sizeof(InterCode));
    code->kind = INTER_ASSIGN;
    code->left = op1;
    code->right = copy_operand(op2); 
    if(code->right->pre == NOTHING) {
        code->right->pre = PRE_AND;
    }
    else if(code->right->pre == PRE_STAR) {
        code->right->pre = NOTHING;
    }
    else {
        assert(0);
    }
    if(code->right->kind == VARIABLE_T) {
        code->right->kind = ADDRESS_T;
    }
    else if(code->right->kind == VARIABLE_V) {
        code->right->kind = ADDRESS_V;
    }
    else {
        assert(0);
    }
    add_code(code);
    //Log("%s := &%s", operand_name(op1), operand_name(op2));
    // fprintf(fp_intercode, "%s := &%s\n", operand_name(code->left), operand_name(code->right));
}

void gen_code_call(Operand* op, char* func_name) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    memset(code, 0, sizeof(InterCode));
    code->kind = INTER_CALL;
    code->func_name = func_name;
    code->ret = op;
    add_code(code);
    //Log("%s := CALL %s", operand_name(op), func_name);
    // fprintf(fp_intercode, "%s := CALL %s\n", operand_name(code->ret), code->func_name);
}

void gen_code_arg(Operand* op) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    memset(code, 0, sizeof(InterCode));
    code->kind = INTER_ARG;
    code->op = op;
    add_code(code);
    //Log("ARG %s", operand_name(op));
    // fprintf(fp_intercode, "ARG %s\n", operand_name(code->op));
}

void gen_code_return(Operand* op) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    memset(code, 0, sizeof(InterCode));
    code->kind = INTER_RETURN;
    code->op = op;
    add_code(code);
    //Log("RETURN %s", operand_name(op));
    // fprintf(fp_intercode, "RETURN %s\n", operand_name(code->op));
}

void gen_code_read(Operand* op) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    memset(code, 0, sizeof(InterCode));
    code->kind = INTER_READ;
    code->op = op;
    add_code(code);
    //Log("READ %s", operand_name(op));
    // fprintf(fp_intercode, "READ %s\n", operand_name(code->op));
}

void gen_code_write(Operand* op) {
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    memset(code, 0, sizeof(InterCode));
    code->kind = INTER_WRITE;
    code->op = op;
    add_code(code);
    //Log("WRITE %s", operand_name(op));
    // fprintf(fp_intercode, "WRITE %s\n", operand_name(code->op));
}

void gen_code_label(int label) {
    if(label <= 0)
        assert(0);
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    memset(code, 0, sizeof(InterCode));
    code->kind = INTER_LABEL;
    code->label = label;
    add_code(code);
    //Log("LABEL label%d :", label);
    // fprintf(fp_intercode, "LABEL label%d :\n", code->label);
}

void gen_code_goto(int label) {
    if(label <= 0)
        assert(0);
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    memset(code, 0, sizeof(InterCode));
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
        return "!=";
    else if(relop == 3)
        return "==";
    else if(relop == 4)
        return "<=";
    else if(relop == 5)
        return ">=";
    return "NULL";
}

void gen_code_if_goto(Operand* op1, int relop, Operand* op2, int label) {
    if(label <= 0)
        assert(0);
    InterCode* code = (InterCode*)malloc(sizeof(InterCode));
    memset(code, 0, sizeof(InterCode));
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
    memset(code, 0, sizeof(InterCode));
    code->kind = INTER_DEC;
    code->dec.var_no = var_no;
    code->dec.width = width;
    add_code(code);
    //Log("DEC v%d %d", var_no, width);
    // fprintf(fp_intercode, "DEC v%d %d\n", code->dec.var_no, code->dec.width);
}

void print_ir(InterCode* code) {
    assert(code);
    // switch (code->kind)
    // {
    // case INTER_ASSIGN: // 0
    //     /* code */
    //     fprintf(fp_intercode, "%s := %s\n", operand_name(code->left), operand_name(code->right));
    //     break;
    // case INTER_ADD: // 1
    //     /* code */
    //     assert(code->result->pre == NOTHING);
    //     assert(code->op1->pre != PRE_AND || code->op2->pre != PRE_AND);
    //     fprintf(fp_intercode, "%s := %s + %s\n", operand_name(code->result), operand_name(code->op1), operand_name(code->op2));
    //     break;
    // case INTER_SUB: //2
    //     /* code */
    //     assert(code->result->pre == NOTHING);
    //      assert(code->op1->pre != PRE_AND || code->op2->pre != PRE_AND);
    //     fprintf(fp_intercode, "%s := %s - %s\n", operand_name(code->result), operand_name(code->op1), operand_name(code->op2));
    //     break;
    // case INTER_MUL: //3
    //     /* code */
    //     assert(code->result->pre == NOTHING);
    //      assert(code->op1->pre != PRE_AND || code->op2->pre != PRE_AND);
    //     fprintf(fp_intercode, "%s := %s * %s\n", operand_name(code->result), operand_name(code->op1), operand_name(code->op2));
    //     break;
    // case INTER_DIV: //4
    //     /* code */
    //     assert(code->result->pre == NOTHING);
    //      assert(code->op1->pre != PRE_AND || code->op2->pre != PRE_AND);
    //     fprintf(fp_intercode, "%s := %s / %s\n", operand_name(code->result), operand_name(code->op1), operand_name(code->op2));
    //     break;
    // case INTER_PARAM: //5
    //     /* code */
    //     fprintf(fp_intercode, "PARAM v%d\n", code->var_no);
    //     break;
    // case INTER_FUNCTION: //6
    //     /* code */
    //     fprintf(fp_intercode, "FUNCTION %s :\n", code->func_name);
    //     break;
    // case INTER_CALL: //7
    //     /* code */
    //     assert(code->ret->pre == NOTHING);
    //     fprintf(fp_intercode, "%s := CALL %s\n", operand_name(code->ret), code->func_name);
    //     break;
    // case INTER_ARG: //8
    //     /* code */
    //     fprintf(fp_intercode, "ARG %s\n", operand_name(code->op));
    //     break;
    // case INTER_RETURN: //9
    //     /* code */
    //     fprintf(fp_intercode, "RETURN %s\n", operand_name(code->op));
    //     break;
    // case INTER_READ: //10
    //     /* code */
    //     fprintf(fp_intercode, "READ %s\n", operand_name(code->op));
    //     break;
    // case INTER_WRITE: //11
    //     /* code */
    //     assert(code->result->pre != PRE_AND);
    //     fprintf(fp_intercode, "WRITE %s\n", operand_name(code->op));
    //     break;
    // case INTER_LABEL: //12
    //     /* code */
    //     fprintf(fp_intercode, "LABEL label%d :\n", code->label);
    //     break;
    // case INTER_GOTO: //13
    //     /* code */
    //     fprintf(fp_intercode, "GOTO label%d\n", code->label);
    //     break;
    // case INTER_IF_GOTO: //14
    //     /* code */
    //     fprintf(fp_intercode, "IF %s %s %s GOTO label%d\n", operand_name(code->if_goto.op1), relop_name(code->if_goto.relop), operand_name(code->if_goto.op2), code->if_goto.label);
    //     break;
    // case INTER_DEC: //15
    //     /* code */
    //     fprintf(fp_intercode, "DEC v%d %d\n", code->dec.var_no, code->dec.width);
    //     break;
    // case INTER_LEFT_POINTER: //16
    //     /* code */
    //     fprintf(fp_intercode, "*%s := %s\n", operand_name(code->left), operand_name(code->right));
    //     break;
    // default:
    //     // assert(0);
    //     break;
    // }
}

void print_all_ir() {
    InterCode* ir = ir_head;
    while(ir) {
        print_ir(ir);
        ir = ir->next;
    }
}

/*******************优化部分*******************************/
void add_used(Operand* op, int *temp_used, int *var_used) {
    assert(op);
    assert(temp_used);
    assert(var_used);
    if(op->kind == VARIABLE_V || op->kind == ADDRESS_V) {
        var_used[op->var_no] = 1;
    }
    else if(op->kind == VARIABLE_T || op->kind == ADDRESS_T) {
        temp_used[op->var_no] = 1;
    }
}

int unused(Operand* op, int *temp_used, int *var_used) {
    // Log("%s", operand_name(op));
    if((op->kind == VARIABLE_V || op->kind == ADDRESS_V) && var_used[op->var_no] == 0) {
        return 1;
    }
    else if((op->kind == VARIABLE_T || op->kind == ADDRESS_T) && temp_used[op->var_no] == 0) {
        return 1;
    }
    return 0;
}

void delete_ir(InterCode* ir) {
    if(ir->prev) {
        ir->prev->next = ir->next;
    }
    else {
        ir_head = ir->next;
    }
    if(ir->next) {
        ir->next->prev = ir->prev;
    }
    else {
        ir_tail = ir->prev;
    }
}

void delete_unused() {
    int* temp_used = (int*)malloc((temp_cnt + 1) * sizeof(int));
    int* var_used = (int*)malloc((inter_var_cnt + 1) * sizeof(int));
    int* label_used = (int*)malloc((label_cnt + 1) * sizeof(int));
    while(1) {
        memset(temp_used, 0, (temp_cnt + 1) * sizeof(int));
        memset(var_used, 0, (inter_var_cnt + 1) * sizeof(int));
        memset(label_used, 0, (label_cnt + 1) * sizeof(int));
        InterCode* ir = ir_head;
        while(ir) {
            if(ir->kind == INTER_ARG || ir->kind == INTER_WRITE || ir->kind == INTER_RETURN || ir->kind == INTER_READ) {
                add_used(ir->op, temp_used, var_used);
            }
            else if(ir->kind == INTER_LEFT_POINTER) {
                add_used(ir->right, temp_used, var_used);
                add_used(ir->left, temp_used, var_used);
            }
            else if(ir->kind == INTER_ASSIGN) {
                add_used(ir->right, temp_used, var_used);
            }
            else if(ir->kind == INTER_ADD || ir->kind == INTER_SUB || ir->kind == INTER_MUL || ir->kind == INTER_DIV) {
                add_used(ir->op1, temp_used, var_used);
                add_used(ir->op2, temp_used, var_used);
            }
            else if(ir->kind == INTER_PARAM) {
                var_used[ir->var_no] = 1;
            }
            else if(ir->kind == INTER_GOTO) {
                label_used[ir->label] = 1;
            }
            else if(ir->kind == INTER_IF_GOTO) {
                label_used[ir->if_goto.label] = 1;
                add_used(ir->if_goto.op1, temp_used, var_used);
                add_used(ir->if_goto.op2, temp_used, var_used);
            }
            ir = ir->next;
        }
        int ret = 0;
        ir = ir_head;
        while(ir) {
            if(ir->kind == INTER_ASSIGN) {
                if(unused(ir->left, temp_used, var_used)) {
                    delete_ir(ir);
                    ret = 1;
                }
            }
            else if(ir->kind == INTER_ADD || ir->kind == INTER_SUB || ir->kind == INTER_MUL || ir->kind == INTER_DIV) {
                if(unused(ir->result, temp_used, var_used)) {
                    delete_ir(ir);
                    ret = 1;
                }
            }
            else if(ir->kind == INTER_LABEL) {
                if(label_used[ir->label] == 0) {
                    delete_ir(ir);
                    ret = 1;
                }
            }
            else if(ir->kind == INTER_DEC) {
                if(var_used[ir->dec.var_no] == 0) {
                    delete_ir(ir);
                    ret = 1;
                }
            }
            ir = ir->next;
        }
        
        if(ret == 0)
            break;
    }
    free(temp_used);
    free(label_used);
    free(var_used);
}

int same_op(Operand* op1, Operand* op2) {
    if(op1->kind != op2->kind)
        return 0;
    if(op1->kind == CONSTANT)
        return (op1->int_value == op2->int_value);
    // if(op1->kind == CONSTANT_FLOAT)
    //     return (op1->float_value == op2->float_value);
    return (op1->var_no == op2->var_no);
}

void delete_const_op() {
    InterCode* ir = ir_head;
    while(ir) {
        if(ir->kind == INTER_ADD) {
            if(ir->op1->kind == CONSTANT && ir->op2->kind == CONSTANT) {
                ir->kind = INTER_ASSIGN;
                ir->left = ir->result;
                ir->right = new_operand_int(ir->op1->int_value + ir->op2->int_value);
            }
            else if(ir->op1->kind == CONSTANT && ir->op1->int_value == 0) {
                ir->kind = INTER_ASSIGN;
                ir->left = ir->result;
                ir->right = ir->op2;
            }
            else if(ir->op2->kind == CONSTANT && ir->op2->int_value == 0) {
                ir->kind = INTER_ASSIGN;
                ir->left = ir->result;
                ir->right = ir->op1;
            }
        }
        else if(ir->kind == INTER_SUB) {
            if(ir->op1->kind == CONSTANT && ir->op2->kind == CONSTANT) {
                ir->kind = INTER_ASSIGN;
                ir->left = ir->result;
                ir->right = new_operand_int(ir->op1->int_value - ir->op2->int_value);
            }
        }
        else if(ir->kind == INTER_MUL) {
            if(ir->op1->kind == CONSTANT && ir->op2->kind == CONSTANT) {
                ir->kind = INTER_ASSIGN;
                ir->left = ir->result;
                ir->right = new_operand_int(ir->op1->int_value * ir->op2->int_value);
            }
        }
        else if(ir->kind == INTER_DIV) {
            if(ir->op1->kind == CONSTANT && ir->op2->kind == CONSTANT && ir->op2->int_value != 0) {
                ir->kind = INTER_ASSIGN;
                ir->left = ir->result;
                ir->right = new_operand_int(ir->op1->int_value / ir->op2->int_value);
            }
        }
        ir = ir->next;
    }
}

void replace_v(InterCode* ir) {
    // 对于v_i := t_i的，用前面t_i的表达式替换t_i
    // Operand* left = ir->left;
    Operand* right = ir->right;
    InterCode* ir2 = ir->prev;
    if(ir2->kind == INTER_ADD || ir2->kind == INTER_SUB || ir2->kind == INTER_MUL || ir2->kind == INTER_DIV) {
        if(same_op(right, ir2->result)) {
            ir->kind = ir2->kind;
            ir->result = ir->left;
            ir->op1 = ir2->op1;
            ir->op2 = ir2->op2;
        }
    }
}

void replace_t(InterCode* ir) {
    // 对于t_i := xxx的，把后面的t_i换成xxx
    Operand* left = ir->left;
    Operand* right = ir->right;
    InterCode* ir2 = ir->next;
    while(ir2) {
        // 终止条件
        if((ir2->kind == INTER_READ) && same_op(left, ir2->op))
            break;
        else if((ir2->kind == INTER_ASSIGN) && same_op(left, ir2->left))
            break;
        else if((ir2->kind == INTER_ADD || ir2->kind == INTER_SUB || ir2->kind == INTER_MUL || ir2->kind == INTER_DIV) && same_op(left, ir2->result))
            break;
        else if(ir2->kind == INTER_GOTO || ir2->kind == INTER_LABEL || ir2->kind == INTER_RETURN || ir2->kind == INTER_CALL)
            break;
        // else if(ir2->kind == INTER_GOTO || ir2->kind == INTER_IF_GOTO || ir2->kind == INTER_LABEL || ir2->kind == INTER_RETURN)
        //     break;
        // 替换
        if(ir2->kind == INTER_ASSIGN || ir2->kind == INTER_LEFT_POINTER ) {
            if(same_op(left, ir2->right)) {
                ir2->right = right;
            }
        }
        else if(ir2->kind == INTER_ADD || ir2->kind == INTER_SUB || ir2->kind == INTER_MUL || ir2->kind == INTER_DIV) {
            if(same_op(left, ir2->op1)) {
                ir2->op1 = right;
            }
            if(same_op(left, ir2->op2)) {
                ir2->op2 = right;
            }
        }
        else if(ir2->kind == INTER_ARG || ir2->kind == INTER_RETURN || ir2->kind == INTER_WRITE || ir2->kind == INTER_READ) {
            if(same_op(left, ir2->op)) {
                ir2->op = right;
            }
        }
        else if(ir2->kind == INTER_IF_GOTO) {
            if(same_op(left, ir2->if_goto.op1)) {
                ir2->if_goto.op1 = right;
            }
            if(same_op(left, ir2->if_goto.op2)) {
                ir2->if_goto.op2 = right;
            }
            break;
        }
        // else if(ir2->kind == INTER_CALL) {
        //     if(same_op(left, ir2->ret)) {
        //         ir2->ret = right;
        //     }
        // }
        ir2 = ir2->next;
    }
}

void delete_assign() {
    // 去除常数运算
    delete_const_op();
    InterCode* ir = ir_head;
    while(ir) {
        if(ir->kind == INTER_ASSIGN) {
            if(ir->left->kind == VARIABLE_T || ir->left->kind == ADDRESS_T) {
                // 找到所有t_i := xxx的，把所有的t_i换成xxx
                replace_t(ir);
            }
            else if(ir->left->kind == VARIABLE_V || ir->left->kind == ADDRESS_V) {
                // 找到所有v_i := t_i的，把所有的t_i换成xxx
                if(ir->right->pre == NOTHING)
                    replace_v(ir);
            }
        }
        ir = ir->next;
    }
    delete_unused();
}

void ir_optimizer() {
    // 删除那些没有用过的变量、临时变量、label
    delete_unused(); // 73327162 -> 65526747
    // 优化去掉一些不必要的赋值语句
    for(int i = 0; i < 3; ++i) {
        delete_assign();
    }
    
    print_all_ir();
}
