#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

#include "variable_type.h"

// 定义符号表项
typedef struct Symbol_ Symbol;
struct Symbol_
{
    char* name;
    int scope_num;
    int lineno;
    enum {
        symbol_FUNC, symbol_STRUCTURE, symbol_VARIABLE
        } kind;
    union {
        Type* type;
        Func* func;
    };
    Symbol* next_in_hash; // 哈希表同一个格子内的下一个符号
    Symbol* next_in_scope; // 同一个作用域里的下一个符号
};

// 定义作用域
typedef struct Scope_ Scope;
struct Scope_ 
{
    int no; // 作用域编号
    Scope* next; // 下一个作用域
    Symbol* first_symbol; // 这个作用域中的首个符号
};

Scope* scope_head;
Scope* scope_struct;
Scope* scope_func;

// 哈希
#define SIZE_OF_HASH 0x3fff
Symbol * hash_table[SIZE_OF_HASH];
unsigned int hash(char* name);

// 函数相关
Type* last_ret_type;
void add_func(Func* func, int lineno, char* name);
Symbol* find_func(char* name);
void add_func_into_table(Func* func, char* name);

// 结构体相关
void add_struct(Type* type, char* name, int lineno);
Symbol* find_struct_or_variable(char* name);
void add_struct_into_table(Type* type, char* name, int lineno);

// 变量相关
void add_variable(Type* type, char* name, int lineno, int struct_para_var);
void add_variable_into_table(Type* type, char* name, int lineno);

// 作用域相关
void symbol_table_init();
void add_scope();
void delete_scope();
void print_scope(Scope* head);

// 符号表的加入
void add_sym_into_table(Symbol* sym);

int nr_scope;
#define CUR_SCOPE (nr_scope - 1)

#endif