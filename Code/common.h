#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

void yyerror(const char *msg);
void yyerror2(const char *msg);
int yylex();

typedef struct Node_t
{
    struct Node_t * first_child;
    struct Node_t * next_sibling;
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
} Node_t;

void draw_tree(Node_t *node, int retract);
Node_t * create_node(const char * name, int lineno, int terminal);
void add_children(Node_t * father, int num, ...);
void print_node(Node_t * node);

Node_t * root;
int error_flag;

#endif
