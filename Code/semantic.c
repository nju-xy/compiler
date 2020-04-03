#include "common.h"

void sem_error(int no, int lineno, char* err_msg) {
    printf("Error type %d at Line %d: %s.\n", no, lineno, err_msg);
}

void semantic_analyzer(Syntax_Tree_Node_t * root) {
    symbol_table_init();
    semantic_Program(root);
    check_func_table();
    print_func_table();
    print_struct_table();
}

void check_func_table() {
    Symbol* sym = scope_func->first_symbol;
    while(sym) {
        if(!sym->func) {
            Log("%d", sym->kind);
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
    // ExtDefList -> ExtDef ExtDefList | epsilon
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
            delete_scope();
        }
        else { // 函数定义 Specifier FunDec CompSt
            add_scope();
            semantic_FunDec(node->first_child->next_sibling, type, 0);
            semantic_CompSt(nth_child(node, 2));
            delete_scope();
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
    //                  | STRUCT OptTag LC DefList RC
    //                  | STRUCT Tag
    // 结构体的定义或者声明
    assert(node);
    assert(strcmp(node->name, "StructSpecifier") == 0);
    if(strcmp(nth_child(node, 1)->name, "LC") == 0) {
        // 创建匿名结构体
        add_scope();
        semantic_DefList(nth_child(node, 2), 0);
        delete_scope();
        return new_type_struct();
    }
    else if(strcmp(nth_child(node, 1)->name, "OptTag") == 0) { 
        // 创建有名字的结构体
        add_scope();
        char* name = semantic_OptTag(nth_child(node, 1));
        semantic_DefList(nth_child(node, 3), 0);
        Type* type = new_type_struct();
        add_struct(type, name, node->lineno);
        delete_scope();
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
    if(nth_child(node, 1)) {
        int num = nth_child(node, 2)->val.type_int;
        Type* new_type = new_type_array(type, num);
        return semantic_VarDec(nth_child(node, 0), new_type, struct_para_var);
    }
    else {
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

    if(strcmp(node->first_child->name, "Exp") == 0) {
        semantic_Exp(node->first_child);
    }
    else if(strcmp(node->first_child->name, "CompSt") == 0) {
        add_scope();
        semantic_CompSt(node->first_child);
        delete_scope();
    }
    else if(strcmp(node->first_child->name, "RETURN") == 0) {
        semantic_Exp(node->first_child->next_sibling);
    }
    else if(strcmp(node->first_child->name, "IF") == 0) {
        semantic_Exp(nth_child(node, 2));
        if(nth_child(node, 5)) {
            semantic_Stmt(nth_child(node, 6));
        }
    }
    else if(strcmp(node->first_child->name, "WHILE") == 0) {
        semantic_Exp(nth_child(node, 2));
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

    semantic_VarDec(node->first_child, type, struct_para_var);
    if(node->first_child->next_sibling) {
        if(struct_para_var == 0) {
            sem_error(15, node->lineno, "结构体定义时对域进行初始化");
        }
        else
            semantic_Exp(nth_child(node, 2));
    }
    // add_variable(field->type, field->name, node->lineno, struct_para_var);
}

// Expressions
void semantic_Exp(Syntax_Tree_Node_t * node) {
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
    // TODO();
}