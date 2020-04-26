#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

#include "variable_type.h"

// 定义符号表项
typedef struct Symbol_ Symbol;
struct Symbol_
{
    char* name;
    int var_no;
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
Symbol * var_table[SIZE_OF_HASH + 1];
Symbol * global_table[SIZE_OF_HASH + 1];
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
Symbol* add_variable(Type* type, char* name, int lineno, int struct_para_var);
Symbol* add_variable_into_table(Type* type, char* name, int lineno);

// 作用域相关
void symbol_table_init();
void add_scope();
void delete_scope(int if_free);
void print_scope(Scope* head);
// void add_func_read();
// void add_func_write();

// 符号表的加入
void add_sym_into_var_table(Symbol* sym);
void add_sym_into_global_table(Symbol* sym);

// 删除相关
void free_sym(Symbol* sym);
// void free_type(Type* type);
// void free_func(Func* func);
// void free_fieldlist(FieldList* field);

// 作用域标号
int nr_scope;
#define CUR_SCOPE (nr_scope - 1)

#endif