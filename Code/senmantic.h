#ifndef __SENMANTIC_H__
#define __SENMANTIC_H__

#include "syntax_tree.h"

void semantic_analyzer(Syntax_Tree_Node_t * root);
void semantic_Program(Syntax_Tree_Node_t * node);
void semantic_ExtDefList(Syntax_Tree_Node_t * node);
void semantic_ExtDef(Syntax_Tree_Node_t * node);
Type* semantic_Specifier(Syntax_Tree_Node_t * node);
void semantic_ExtDecList(Syntax_Tree_Node_t * node, Type* type);
void semantic_FunDec(Syntax_Tree_Node_t * node, Type* ret_type, int declare);
void semantic_StructSpecifier(Syntax_Tree_Node_t * node);
void semantic_OptTag(Syntax_Tree_Node_t * node);
void semantic_Tag(Syntax_Tree_Node_t * node);
Type* semantic_VarDec(Syntax_Tree_Node_t * node, Type* type);
Para* semantic_VarList(Syntax_Tree_Node_t * node);
Type* semantic_ParamDec(Syntax_Tree_Node_t * node);
void semantic_CompSt(Syntax_Tree_Node_t * node);
void semantic_StmtList(Syntax_Tree_Node_t * node);
void semantic_DefList(Syntax_Tree_Node_t * node);
void semantic_Stmt(Syntax_Tree_Node_t * node);
void semantic_Def(Syntax_Tree_Node_t * node);
void semantic_DecList(Syntax_Tree_Node_t * node, Type* type);
void semantic_Dec(Syntax_Tree_Node_t * node, Type* type);
void semantic_Exp(Syntax_Tree_Node_t * node);


#endif