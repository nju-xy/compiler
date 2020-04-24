#include "common.h"

void sem_error(int no, int lineno, char* err_msg) {
    printf("Error type %d at Line %d: %s.\n", no, lineno, err_msg);
}

void semantic_analyzer(Syntax_Tree_Node_t * root) {
    symbol_table_init();
    semantic_Program(root);
    check_func_table();
    // print_func_table();
    // print_struct_table();
}

void check_func_table() {
    Symbol* sym = scope_func->first_symbol;
    while(sym) {
        if(!sym->func) {
            // Log("%d", sym->kind);
            assert(0);
        }
        if(!sym->func->def) {
            sem_error(18, sym->lineno, "函数进行了声明,但没有被定义");
        }
        sym = sym->next_in_scope;
    }
}

// High-level Definitions
void semantic_Program(Syntax_Tree_Node_t * node) {
    // Program -> ExtDefList
    assert(node);
    assert(strcmp(node->name, "Program") == 0);
    if(node->first_child) {
        semantic_ExtDefList(node->first_child);
    }
    // 否则是空程序
}

void semantic_ExtDefList(Syntax_Tree_Node_t * node) {
    // ExtDefList -> ExtDef ExtDefList 
    //             | ExtDef
    assert(node);
    assert(strcmp(node->name, "ExtDefList") == 0);
    semantic_ExtDef(node->first_child);
    if(node->first_child->next_sibling)
        semantic_ExtDefList(node->first_child->next_sibling);
}

void semantic_ExtDef(Syntax_Tree_Node_t * node) {
    // ExtDef -> Specifier ExtDecList SEMI
    //         | Specifier SEMI
    //         | Specifier FunDec CompSt
    //         | Specifier FunDec SEMI
    // 函数定义、全局变量定义、只有类型的废话
    assert(node);
    assert(strcmp(node->name, "ExtDef") == 0);
    // 类型
    Type* type = semantic_Specifier(node->first_child);
    if(!type)
        return;
    if(strcmp(node->first_child->next_sibling->name, "ExtDecList") == 0) {
        // 定义全局变量
        semantic_ExtDecList(node->first_child->next_sibling, type);
    }
    else if(strcmp(node->first_child->next_sibling->name, "FunDec") == 0) {
        if(strcmp(nth_child(node, 2)->name, "SEMI") == 0) {
            // 函数声明 Specifier FunDec SEMI
            add_scope();
            semantic_FunDec(node->first_child->next_sibling, type, 1);
            delete_scope(1);
        }
        else { // 函数定义 Specifier FunDec CompSt
            add_scope();
            last_ret_type = type;
            semantic_FunDec(node->first_child->next_sibling, type, 0);
            semantic_CompSt(nth_child(node, 2));
            last_ret_type = NULL;
            delete_scope(1);
        }
    }
}

// Specifiers
Type* semantic_Specifier(Syntax_Tree_Node_t * node) {
    // Specifier -> TYPE
    //             | StructSpecifier 
    // 类型（非数组）：可能是int, float, 结构体
    assert(node);
    assert(strcmp(node->name, "Specifier") == 0);
    if(strcmp(node->first_child->name, "TYPE") == 0) {
        if(node->first_child->val.type_type == TYPE_INT) {
            return new_type_int();
        }
        else {
            return new_type_float();
        }
    }
    else {
        return semantic_StructSpecifier(node->first_child);
    }
}

Type* semantic_StructSpecifier(Syntax_Tree_Node_t * node) {
    // StructSpecifier -> STRUCT LC DefList RC
    //                  | STRUCT LC RC
    //                  | STRUCT OptTag LC DefList RC
    //                  | STRUCT OptTag LC RC
    //                  | STRUCT Tag
    // 结构体的定义或者声明
    assert(node);
    assert(strcmp(node->name, "StructSpecifier") == 0);
    if(strcmp(nth_child(node, 1)->name, "LC") == 0) {
        // 创建匿名结构体
        add_scope();
        if(nth_child(node, 3)) 
            semantic_DefList(nth_child(node, 2), 0);
        Type* type = new_type_struct();
        delete_scope(0);
        return type;
    }
    else if(strcmp(nth_child(node, 1)->name, "OptTag") == 0) { 
        // 创建有名字的结构体
        add_scope();
        char* name = semantic_OptTag(nth_child(node, 1));
        if(nth_child(node, 4)) 
            semantic_DefList(nth_child(node, 3), 0);
        Type* type = new_type_struct();
        add_struct(type, name, node->lineno);
        delete_scope(0);
        return type;
    }
    else { // 创建新的结构体变量
        char* name = semantic_Tag(nth_child(node, 1));
        Symbol* sym = find_struct_or_variable(name);
        if(!sym) {
            sem_error(17, node->lineno, "直接使用未定义过的结构体来定义变量");
            return NULL;
        }
        assert(sym->type->kind == STRUCTURE);
        return sym->type;
    }
}

char* semantic_OptTag(Syntax_Tree_Node_t * node) {
    // OptTag -> ID
    assert(node);
    assert(strcmp(node->name, "OptTag") == 0);
    return node->first_child->val.id_name;
}

char* semantic_Tag(Syntax_Tree_Node_t * node) {
    // Tag -> ID
    assert(node);
    assert(strcmp(node->name, "Tag") == 0);
    return node->first_child->val.id_name;
}

void semantic_ExtDecList(Syntax_Tree_Node_t * node, Type* type) {
    // ExtDecList -> VarDec
    //             | VarDec COMMA ExtDecList
    // 全局变量的列表，比如int a, b, c;中的a, b, c
    assert(node);
    assert(strcmp(node->name, "ExtDecList") == 0);
    semantic_VarDec(node->first_child, type, 2);
    // add_variable(field->type, field->name, node->lineno, 2);
    if(nth_child(node, 1)) {
        semantic_ExtDecList(nth_child(node, 2), type);
    }
}


// Declarators
Type* semantic_VarDec(Syntax_Tree_Node_t * node, Type* type, int struct_para_var) {
    // VarDec -> ID
    //         | VarDec LB INT RB
    // 一个变量（有可能是数组），要么在函数的参数列表中，要么在局部or全局变量的定义中
    // 返回值是变量VarDec的类型和名字组成的field
    assert(node);
    assert(strcmp(node->name, "VarDec") == 0);
    if(nth_child(node, 1)) { // VarDec LB INT RB
        int num = nth_child(node, 2)->val.type_int;
        Type* new_type = new_type_array(type, num);
        return semantic_VarDec(nth_child(node, 0), new_type, struct_para_var);
    }
    else { // ID
        char* name = node->first_child->val.id_name;
        add_variable(type, name, node->lineno, struct_para_var);
        return type;
    }
}


void semantic_FunDec(Syntax_Tree_Node_t * node, Type* ret_type, int declare) {
    // FunDec -> ID LP VarList RP
    //         | ID LP RP
    // 函数定义or声明中，类型后面的部分，即函数名和参数
    assert(node);
    assert(strcmp(node->name, "FunDec") == 0);
    if(nth_child(node, 3)) {
        semantic_VarList(nth_child(node, 2));
    }

    Func* func = new_func(ret_type, node->lineno, declare);
    add_func(func, node->lineno, node->first_child->val.id_name);
}

void semantic_VarList(Syntax_Tree_Node_t * node) {
    // VarList -> ParamDec COMMA VarList
    //          | ParamDec
    // 函数定义或者声明时的参数列表
    assert(node);
    assert(strcmp(node->name, "VarList") == 0);
    semantic_ParamDec(node->first_child);
    if(nth_child(node, 1)) {
        semantic_VarList(nth_child(node, 2));
    }
}

void semantic_ParamDec(Syntax_Tree_Node_t * node) {
    // VarList -> Specifier VarDec
    // 函数定义或声明中的一个参数
    assert(node);
    assert(strcmp(node->name, "ParamDec") == 0);
    Type* type = semantic_Specifier(node->first_child);
    if(type)
        semantic_VarDec(node->first_child->next_sibling, type, 1);
}

// Statements
void semantic_CompSt(Syntax_Tree_Node_t * node) {
    // CompSt -> LC DefList StmtList RC
    //         | LC StmtList RC
    //         | LC DefList RC
    //         | LC RC
    // 大括号包起来的部分，可能在while语句中，函数定义中，if语句中（不在结构体声明中）
    assert(node);
    assert(strcmp(node->name, "CompSt") == 0);
    if(strcmp(nth_child(node, 1)->name, "DefList") == 0) {
        semantic_DefList(nth_child(node, 1), 2);
    }
    else if(strcmp(nth_child(node, 1)->name, "StmtList") == 0) {
        semantic_StmtList(nth_child(node, 1));
    }
    if(nth_child(node, 2) && strcmp(nth_child(node, 2)->name, "StmtList") == 0) {
        semantic_StmtList(nth_child(node, 2));
    }
}

void semantic_StmtList(Syntax_Tree_Node_t * node) {
    // StmtList -> Stmt
    //           | Stmt StmtList
    // 函数里面Stmt的列表
    assert(node);
    assert(strcmp(node->name, "StmtList") == 0);
    
    semantic_Stmt(node->first_child);

    if(nth_child(node, 1)) {
        semantic_StmtList(nth_child(node, 1));
    }
}

void semantic_Stmt(Syntax_Tree_Node_t * node) {
    // Stmt -> Exp SEMI
    //       | CompSt
    //       | RETURN Exp SEMI
    //       | IF LP Exp RP Stmt
    //       | IF LP Exp RP Stmt ELSE Stmt
    //       | WHILE LP EXP RP Stmt
    // 函数里面Stmt
    assert(node);
    assert(strcmp(node->name, "Stmt") == 0);

    if(strcmp(node->first_child->name, "Exp") == 0) { // Exp SEMI
        semantic_Exp(node->first_child);
    }
    else if(strcmp(node->first_child->name, "CompSt") == 0) { 
        add_scope();
        semantic_CompSt(node->first_child);
        delete_scope(1);
    }
    else if(strcmp(node->first_child->name, "RETURN") == 0) {
        Type* type = semantic_Exp(node->first_child->next_sibling);
        if(!type || !last_ret_type || !same_type(type, last_ret_type)) {
            sem_error(8, node->lineno, "return语句的返回类型与函数定义的返回类型不匹配");
        }
    }
    else if(strcmp(node->first_child->name, "IF") == 0) {
        // IF LP Exp RP Stmt
        // IF LP Exp RP Stmt ELSE Stmt
        Type* type = semantic_Exp(nth_child(node, 2));
        if(!type || type->kind != BASIC || type->basic != BASIC_INT) {
            sem_error(7, node->lineno, "操作数类型与操作符IF不匹配");
        }
        semantic_Stmt(nth_child(node, 4));
        if(nth_child(node, 5))
            semantic_Stmt(nth_child(node, 6));
    }
    else if(strcmp(node->first_child->name, "WHILE") == 0) {
        // WHILE LP EXP RP Stmt
        Type* type = semantic_Exp(nth_child(node, 2));
        if(!type || type->kind != BASIC || type->basic != BASIC_INT) {
            sem_error(7, node->lineno, "操作数类型与操作符WHILE不匹配");
        }
        semantic_Stmt(nth_child(node, 4));
    }
    
}

// Local Definitions
void semantic_DefList(Syntax_Tree_Node_t * node, int struct_para_var) {
    // DefList -> Def
    //          | Def DefList
    // 函数or结构体里面局部变量定义的列表
    assert(node);
    assert(strcmp(node->name, "DefList") == 0);
    
    semantic_Def(node->first_child, struct_para_var);

    if(nth_child(node, 1)) {
       semantic_DefList(nth_child(node, 1), struct_para_var);
    }
}

void semantic_Def(Syntax_Tree_Node_t * node, int struct_para_var) {
    // Def -> Specifier DecList SEMI
    // 函数or结构体里面的一行局部变量定义
    assert(node);
    assert(strcmp(node->name, "Def") == 0);

    Type* type = semantic_Specifier(node->first_child);
    if(type)
        semantic_DecList(node->first_child->next_sibling, type, struct_para_var);
}

void semantic_DecList(Syntax_Tree_Node_t * node, Type* type, int struct_para_var) {
    // DecList -> Dec
    //          | Dec COMMA DecList
    // 函数or结构体内局部变量声明中以逗号间隔的变量列表
    assert(node);
    assert(strcmp(node->name, "DecList") == 0);

    semantic_Dec(node->first_child, type, struct_para_var);
    
    if(node->first_child->next_sibling) {
        semantic_DecList(nth_child(node, 2), type, struct_para_var);
    }
}

void semantic_Dec(Syntax_Tree_Node_t * node, Type* type, int struct_para_var) {
    // Dec -> VarDec
    //     | VarDec ASSIGNOP Exp
    // 一个变量or变量+赋值
    assert(node);
    assert(strcmp(node->name, "Dec") == 0);

    Type* type1 = semantic_VarDec(node->first_child, type, struct_para_var);
    if(node->first_child->next_sibling) {
        if(struct_para_var == 0) { // 结构体
            sem_error(15, node->lineno, "结构体定义时对域进行初始化");
        }
        else { // 变量
            Type* type2 = semantic_Exp(nth_child(node, 2));
            if(!type1 || !type2) {
                sem_error(7, node->lineno, "由于之前的错误，赋值号两边的表达式类型不匹配");
            }
            else if(!same_type(type1, type2)) {
                sem_error(5, node->lineno, "赋值号两边的表达式类型不匹配");
            }
        }
    }
    // add_variable(field->type, field->name, node->lineno, struct_para_var);
}

// Expressions
int semantic_Args(Syntax_Tree_Node_t * node, FieldList* para) {
    // Args -> Exp COMMA Args
    //       | Exp
    // 匹配就返回1，不匹配就返回0；
    if(!para) {
        //Log("参数多了");
        return 0; 
    }
    if(!same_type(semantic_Exp(node->first_child), para->type)) {
        //Log("参数类型不匹配");
        return 0;
    }
    if(nth_child(node, 1)) { // Exp COMMA Args
        // 后面的匹配了就是匹配了
        return semantic_Args(nth_child(node, 2), para->next);
    }
    else { // Exp
        if(para->next) {
            //Log("参数少了");
            return 0;
        }
    }
    return 1;
}

Type* semantic_Exp(Syntax_Tree_Node_t * node) {
    // Exp -> Exp ASSIGNOP Exp
    //      | Exp AND Exp
    //      | Exp OR Exp
    //      | Exp RELOP Exp
    //      | Exp PLUS Exp
    //      | Exp MINUS Exp
    //      | Exp STAR Exp
    //      | Exp DIV Exp
    //      | LP Exp RP
    //      | MINUS Exp
    //      | Not Exp
    //      | ID LP Args RP
    //      | ID LP RP
    //      | Exp LB Exp RB
    //      | Exp DOT ID
    //      | ID
    //      | INT
    //      | FLOAT
    // 表达式
    assert(node);
    assert(strcmp(node->name, "Exp") == 0);
    if(nth_child(node, 1) && strcmp(nth_child(node, 1)->name, "ASSIGNOP") == 0) {
        // Exp ASSIGNOP Exp
        int is_right_value = check_right_value(node->first_child);
        Type* type1 = semantic_Exp(node->first_child);
        Type* type2 = semantic_Exp(nth_child(node, 2));
        // Log("lineno:%d, type1:%d, type2:%d", node->lineno, type1->kind, type2->kind);
        if(is_right_value) {
            sem_error(6, node->lineno, "赋值号左边出现一个只有右值的表达式");
            return type1;
        }
        if(!type1 || !type2) {
            sem_error(5, node->lineno, "由于之前的错误，赋值号两边的表达式类型不匹配");
        }
        else if(!same_type(type1, type2)) {
            sem_error(5, node->lineno, "赋值号两边的表达式类型不匹配");
        }
        return type1;
    }
    else if(nth_child(node, 1) && strcmp(nth_child(node, 1)->name, "AND") == 0) {
        // Exp AND Exp
        return exp_2_op_logic(node);
    }
    else if(nth_child(node, 1) && strcmp(nth_child(node, 1)->name, "OR") == 0) {
        // Exp OR Exp
        return exp_2_op_logic(node);
    }
    else if(nth_child(node, 1) && strcmp(nth_child(node, 1)->name, "RELOP") == 0) {
        // Exp RELOP Exp
        exp_2_op_algorithm(node);
        return new_type_int();
    }
    else if(nth_child(node, 1) && strcmp(nth_child(node, 1)->name, "PLUS") == 0) {
        // Exp PLUS Exp
        return exp_2_op_algorithm(node);
    }
    else if(nth_child(node, 1) && strcmp(nth_child(node, 1)->name, "MINUS") == 0) {
        // Exp MINUS Exp
        return exp_2_op_algorithm(node);
    }
    else if(nth_child(node, 1) && strcmp(nth_child(node, 1)->name, "STAR") == 0) {
        // Exp STAR Exp
        return exp_2_op_algorithm(node);
    }
    else if(nth_child(node, 1) && strcmp(nth_child(node, 1)->name, "DIV") == 0) {
        // Exp DIV Exp
        return exp_2_op_algorithm(node);
    }
    else if(strcmp(node->first_child->name, "LP") == 0) {
        // LP Exp RP
        return semantic_Exp(node->first_child->next_sibling);
    }
    else if(strcmp(node->first_child->name, "MINUS") == 0) {
        // MINUS Exp
        return exp_1_op_algorithm(node);
    }
    else if(strcmp(node->first_child->name, "NOT") == 0) {
        // NOT Exp
        return exp_1_op_logic(node);
    }
    else if(strcmp(node->first_child->name, "ID") == 0) {
        if(!node->first_child->next_sibling) {
            // ID
            Symbol* sym = find_struct_or_variable(node->first_child->val.id_name);
            if(!sym || sym->kind != symbol_VARIABLE) {
                sem_error(1, node->lineno, "变量在使用时未经定义");
                return new_type_int();
            }
            else {
                return sym->type;
            }
        }
        else { // 函数调用
            Symbol* func_sym = find_func(node->first_child->val.id_name);
            if(!func_sym) {
                Symbol* other_sym = find_struct_or_variable(node->first_child->val.id_name);
                if(other_sym) {
                    sem_error(11, node->lineno, "对普通变量使用'(...)'或'()'(函数调用)操作符");
                    return other_sym->type;
                }
                else 
                    sem_error(2, node->lineno, "函数在调用时未经定义");
                return new_type_int();
            }
            
            Func* func = func_sym->func;
            if(nth_child(node, 3)) {
                // ID LP Args RP
                // 有参数的函数调用
                int ret = semantic_Args(nth_child(node, 2), func->para);
                if(!ret) {
                    sem_error(9, node->lineno, "函数调用时实参与形参的数目或类型不匹配");
                }
            }
            else {
                // ID LP RP
                // 没参数的函数调用
                if(func->para != NULL) {
                    sem_error(9, node->lineno, "函数调用时实参与形参的数目或类型不匹配");
                }
            }
            return func->ret_type;
        }
    }
    else if(strcmp(node->first_child->name, "INT") == 0) {
        // INT
        return new_type_int();
    }
    else if(strcmp(node->first_child->name, "FLOAT") == 0) {
        // FLOAT
        return new_type_float();
    }
    else if(nth_child(node, 1) && strcmp(nth_child(node, 1)->name, "DOT") == 0) {
        // Exp DOT ID
        // 结构体
        Type* type = semantic_Exp(node->first_child);
        if(!type || type->kind != STRUCTURE) {
            sem_error(13, node->lineno, "对非结构体型变量使用'.'操作符");
            return type;
        }
        Type* type2 = find_field(type, nth_child(node, 2)->val.id_name);
        if(!type2) {
            sem_error(14, node->lineno, "访问结构体中未定义过的域");
            return new_type_int();
        }
        return type2;
    }
    else if(nth_child(node, 1) && strcmp(nth_child(node, 1)->name, "LB") == 0) {
        // Exp LB Exp RB
        // 数组
        // Log("数组");
        Type* type1 = semantic_Exp(node->first_child);
        Type* type2 = semantic_Exp(nth_child(node, 2));
        if(!type1) {
            sem_error(10, node->lineno, "由于之前的错误，对非数组型变量使用'[...]'(数组访问)操作符");
            return new_type_int();
        }
        else if(type1->kind != ARRAY) {
            sem_error(10, node->lineno, "对非数组型变量使用'[...]'(数组访问)操作符");
            return type1;
        }
        if(!type2) {
            sem_error(12, node->lineno, "由于之前的错误，数组访问操作符'[...]'中出现非整数");
        }
        else if(type2->kind != BASIC || type2->basic != BASIC_INT) {
            sem_error(12, node->lineno, "数组访问操作符'[...]'中出现非整数");
        }
        return type1->array.elem;
        //Log("数组结束");
    }
}

Type* exp_2_op_algorithm(Syntax_Tree_Node_t * node) {
    // 指那些只能int和float参加的二元算术运算
    Type* type1 = semantic_Exp(node->first_child);
    Type* type2 = semantic_Exp(nth_child(node, 2));
    // Log("二元算数运算, %d, %d", type1->kind, type2->kind);
    if(!type1 || !type2) {
        sem_error(7, node->lineno, "由于之前的错误，操作数类型与操作符不匹配");
    }
    else if(type1->kind != BASIC || type2->kind != BASIC) {
        sem_error(7, node->lineno, "操作数类型与操作符不匹配，只允许int或float");
    }
    else if(type1->basic != type2->basic) {
        sem_error(7, node->lineno, "操作数类型不匹配，禁止int和float进行运算");
    }
    return type1;
}

Type* exp_2_op_logic(Syntax_Tree_Node_t * node) {
    // 二元逻辑运算
    Type* type1 = semantic_Exp(node->first_child);
    Type* type2 = semantic_Exp(nth_child(node, 2));
    // Log("二元算数运算, %d, %d", type1->kind, type2->kind);
    if(!type1 || !type2) {
        sem_error(7, node->lineno, "由于之前的错误，操作数类型与操作符不匹配");
    }
    else if(type1->kind != BASIC || type2->kind != BASIC || type1->basic != BASIC_INT || type2->basic != BASIC_INT ) {
        sem_error(7, node->lineno, "操作数类型与操作符不匹配，只允许int");
    }
    return type1;
}

Type* exp_1_op_algorithm(Syntax_Tree_Node_t * node) {
    // 指那些只能int和float参加的一元算术运算
    Type* type1 = semantic_Exp(node->first_child->next_sibling);
    // Log("二元算数运算, %d, %d", type1->kind, type2->kind);
    if(!type1) {
        sem_error(7, node->lineno, "由于之前的错误，操作数类型与操作符不匹配");
    }
    else if(type1->kind != BASIC) {
        sem_error(7, node->lineno, "操作数类型与操作符不匹配，只允许int或float");
    }
    return type1;
}

Type* exp_1_op_logic(Syntax_Tree_Node_t * node) {
    // 一元逻辑运算
    Type* type1 = semantic_Exp(node->first_child->next_sibling);
    // Log("二元算数运算, %d, %d", type1->kind, type2->kind);
    if(!type1) {
        sem_error(7, node->lineno, "由于之前的错误，操作数类型与操作符不匹配");
    }
    else if(type1->kind != BASIC || type1->basic != BASIC_INT) {
        sem_error(7, node->lineno, "操作数类型与操作符不匹配，只允许int");
    }
    return type1;
}

int check_right_value(Syntax_Tree_Node_t* node) {
    // 赋值号左边能出现的只有ID、Exp LB Exp RB以及Exp DOT ID
    // 而不能是其它形式的语法单元组合
    // 如果是右值则返回1，否则返回0
    if(strcmp(node->first_child->name, "ID") == 0 && !nth_child(node, 1)) { // ID
        return 0;
    }
    if(nth_child(node, 1) && strcmp(nth_child(node, 1)->name, "LB") == 0) { // Exp LB Exp RB
        return 0;
    }
    if(nth_child(node, 1) && strcmp(nth_child(node, 1)->name, "DOT") == 0) { // Exp DOT ID
        return 0;
    }
    return 1;
}