#ifndef __SYNTEX_TREE_H__
#define __SYNTEX_TREE_H__

typedef struct Syntax_Tree_Node_t
{
    struct Syntax_Tree_Node_t * first_child;
    struct Syntax_Tree_Node_t * next_sibling;
    int lineno;
    int terminal;
    char name[20];
    union {
        int type_int;
        float type_float;
        char * id_name;
        // char id_name[105];
        double type_double;
        enum {
            TYPE_INT, TYPE_FLOAT
        } type_type;
        enum {
            LT, GT, LE, GE, EQ, NE
        } type_relop;
    } val;
} Syntax_Tree_Node_t;

void draw_tree(Syntax_Tree_Node_t *node, int retract);
Syntax_Tree_Node_t * create_node(const char * name, int lineno, int terminal);
void add_children(Syntax_Tree_Node_t * father, int num, ...);
void print_node(Syntax_Tree_Node_t * node);

Syntax_Tree_Node_t * root;
int error_flag;

#endif