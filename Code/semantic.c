#include "common.h"

void semantic_analyzer(Syntax_Tree_Node_t * root) {
    symbol_table_init();
    semantic_Program(root);
    // check_func_table();
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
    assert(node);
    assert(strcmp(node->name, "ExtDef") == 0);
    // 类型
    Type* ret_type = semantic_Specifier(node->first_child);
    
    // Log("%s, %s, %d", node->first_child->name, node->first_child->next_sibling->name, node->lineno);
    Log("Specifier type");
    print_type(ret_type);
    
    if(strcmp(node->first_child->next_sibling->name, "ExtDecList") == 0) {
        // 定义全局变量
        semantic_ExtDecList(node->first_child->next_sibling);
    }
    else if(strcmp(node->first_child->next_sibling->name, "FunDec") == 0) {
        if(strcmp(nth_child(node, 2)->name, "SEMI") == 0) {
            // 函数声明
            semantic_FunDec(node->first_child->next_sibling);
        }
        else { // 函数定义
            semantic_FunDec(node->first_child->next_sibling);
            semantic_CompSt(nth_child(node, 2));
        }
    }
}

// Specifiers
Type* semantic_Specifier(Syntax_Tree_Node_t * node) {
    // Specifier -> TYPE
    //             | StructSpecifier 
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
        semantic_StructSpecifier(node->first_child);
    }
}

void semantic_StructSpecifier(Syntax_Tree_Node_t * node) {
    // StructSpecifier -> STRUCT LC DefList RC
    //                  | STRUCT OptTag LC DefList RC
    //                  | STRUCT Tag
    assert(node);
    assert(strcmp(node->name, "StructSpecifier") == 0);
}

void semantic_OptTag(Syntax_Tree_Node_t * node) {
    // OptTag -> ID
    assert(node);
    assert(strcmp(node->name, "OptTag") == 0);
}

void semantic_Tag(Syntax_Tree_Node_t * node) {
    // Tag -> ID
    assert(node);
    assert(strcmp(node->name, "OptTag") == 0);
}

void semantic_ExtDecList(Syntax_Tree_Node_t * node) {
    // ExtDecList -> VarDec
    //             | VarDec COMMA ExtDecList
    assert(node);
    assert(strcmp(node->name, "ExtDecList") == 0);
    semantic_VarDec(node->first_child);
    if(nth_child(node, 1)) {
        semantic_ExtDecList(nth_child(node, 2));
    }
}

// Declarators
void semantic_VarDec(Syntax_Tree_Node_t * node) {
    // VarDec -> ID
    //         | ID LB INT RB
    assert(node);
    assert(strcmp(node->name, "VarDec") == 0);

    if(nth_child(node, 1)) {

    }
}

void semantic_FunDec(Syntax_Tree_Node_t * node) {
    // FunDec -> ID LP VarList RP
    //         | ID LP RP
    assert(node);
    assert(strcmp(node->name, "FunDec") == 0);
    
    if(nth_child(node, 3)) {
        semantic_VarList(nth_child(node, 2));
    }
}

void semantic_VarList(Syntax_Tree_Node_t * node) {
    // VarList -> ParamDec COMMA VarList
    //          | ParamDec
    assert(node);
    assert(strcmp(node->name, "VarList") == 0);
    semantic_ParamDec(node->first_child);
    if(nth_child(node, 1)) {
        semantic_VarList(nth_child(node, 2));
    }
}

void semantic_ParamDec(Syntax_Tree_Node_t * node) {
    // VarList -> Specifier VarDec
    assert(node);
    assert(strcmp(node->name, "ParamDec") == 0);
    semantic_Specifier(node->first_child);
    semantic_VarDec(node->first_child->next_sibling);
}

// Statements
void semantic_CompSt(Syntax_Tree_Node_t * node) {
    // CompSt -> LC DefList StmtList RC
    //         | LC StmtList RC
    //         | LC DefList RC
    //         | LC RC
    assert(node);
    assert(strcmp(node->name, "CompSt") == 0);
    if(strcmp(nth_child(node, 1)->name, "DefList") == 0) {
        semantic_DefList(nth_child(node, 1));
    }
    else if(strcmp(nth_child(node, 1)->name, "StmtList") == 0) {
        semantic_StmtList(nth_child(node, 1));
    }
    if(strcmp(nth_child(node, 2)->name, "StmtList") == 0) {
        semantic_StmtList(nth_child(node, 2));
    }
}

void semantic_StmtList(Syntax_Tree_Node_t * node) {
    // StmtList -> Stmt
    //           | Stmt StmtList
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
    assert(node);
    assert(strcmp(node->name, "Stmt") == 0);

    if(strcmp(node->first_child->name, "Exp") == 0) {
        semantic_Exp(node->first_child);
    }
    else if(strcmp(node->first_child->name, "CompSt") == 0) {
        semantic_CompSt(node->first_child);
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
void semantic_DefList(Syntax_Tree_Node_t * node) {
    // DefList -> Def
    //          | Def DefList
    assert(node);
    assert(strcmp(node->name, "DefList") == 0);
    
    semantic_Def(node->first_child);

    if(nth_child(node, 1)) {
        semantic_DefList(nth_child(node, 1));
    }
}

void semantic_Def(Syntax_Tree_Node_t * node) {
    // Def -> Specifier DecList SEMI
    assert(node);
    assert(strcmp(node->name, "Def") == 0);

    semantic_Specifier(node->first_child);
    semantic_DecList(node->first_child->next_sibling);
}

void semantic_DecList(Syntax_Tree_Node_t * node) {
    // DecList -> Dec
    //          | Dec COMMA DecList
    assert(node);
    assert(strcmp(node->name, "DecList") == 0);

    semantic_Dec(node->first_child);
    
    if(node->first_child->next_sibling) {
        semantic_DecList(nth_child(node, 2));
    }
}

void semantic_Dec(Syntax_Tree_Node_t * node) {
    // DecList -> VarDec
    //          | VarDec ASSIGNOP Exp
    assert(node);
    assert(strcmp(node->name, "Dec") == 0);

    semantic_VarDec(node->first_child);
    
    if(node->first_child->next_sibling) {
        semantic_Exp(nth_child(node, 2));
    }
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
    assert(node);
    assert(strcmp(node->name, "Exp") == 0);
    // TODO();
}