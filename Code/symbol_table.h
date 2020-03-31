#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

#include "variable_type.h"

// 定义符号表项
typedef struct Symbol_ Symbol;
struct Symbol_
{
    char* name;
    Type* type;
    Symbol* next; // 下一个符号
};

// 定义作用域
typedef struct Scope_ Scope;
struct Scope_ 
{
    int lineno; // 开始的行号
    Scope* next; // 下一个作用域
    Symbol* first_symbol; // 这个作用域中的首个符号
};

Scope* scope_head;

// 函数申明

// 测试用
void symbol_test();
void print_scope(Scope* head);

// 作用域相关
void symbol_table_init();
void add_scope(int lineno);
void delete_scope();

#endif