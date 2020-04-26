#ifndef __SENMANTIC_H__
#define __SENMANTIC_H__

#include "syntax_tree.h"

void sem_error(int no, int lineno, char* err_msg);
void check_func_table();
void semantic_analyzer(Syntax_Tree_Node_t * root);
void semantic_Program(Syntax_Tree_Node_t * node);
void semantic_ExtDefList(Syntax_Tree_Node_t * node);
void semantic_ExtDef(Syntax_Tree_Node_t * node);
Type* semantic_Specifier(Syntax_Tree_Node_t * node);
void semantic_ExtDecList(Syntax_Tree_Node_t * node, Type* type);
void semantic_FunDec(Syntax_Tree_Node_t * node, Type* ret_type, int declare);
Type* semantic_StructSpecifier(Syntax_Tree_Node_t * node);
char* semantic_OptTag(Syntax_Tree_Node_t * node);
char* semantic_Tag(Syntax_Tree_Node_t * node);
Type* semantic_VarDec(Syntax_Tree_Node_t * node, Type* type, int struct_para_var);
void semantic_VarList(Syntax_Tree_Node_t * node);
void semantic_ParamDec(Syntax_Tree_Node_t * node);
void semantic_CompSt(Syntax_Tree_Node_t * node);
void semantic_StmtList(Syntax_Tree_Node_t * node);
void semantic_DefList(Syntax_Tree_Node_t * node, int struct_para_var);
void semantic_Stmt(Syntax_Tree_Node_t * node, int S_next);
void semantic_Def(Syntax_Tree_Node_t * node, int struct_para_var);
void semantic_DecList(Syntax_Tree_Node_t * node, Type* type, int struct_para_var);
void semantic_Dec(Syntax_Tree_Node_t * node, Type* type, int in_struct);
Operand* semantic_Exp(Syntax_Tree_Node_t * node, int B_true, int B_false);
int semantic_Args(Syntax_Tree_Node_t * node, FieldList* para);
Operand* semantic_read(Syntax_Tree_Node_t * node);
Operand* semantic_write(Syntax_Tree_Node_t * node);

Operand* exp_2_op_algorithm(Syntax_Tree_Node_t * node, int B_true, int B_false);
Operand* exp_2_op_logic(Syntax_Tree_Node_t * node, int B_true, int B_false);
Operand* exp_1_op_algorithm(Syntax_Tree_Node_t * node, int B_true, int B_false);
Operand* exp_1_op_logic(Syntax_Tree_Node_t * node, int B_true, int B_false);
int check_right_value(Syntax_Tree_Node_t* node);

#endif