#include "common.h"

void symbol_table_init() {
    last_ret_type = NULL;
    nr_scope = 0;
    add_scope(); // 第0层给函数用
    scope_func = scope_head;
    add_scope(); // 第1层结构体用
    scope_struct = scope_head;
    add_scope(); // 第2层全局变量用
    memset(hash_table, 0, sizeof(hash_table));
}

unsigned int hash(char* name) {
    unsigned int val = 0, i;
    for(; *name; ++name) {
        val = (val << 2) + *name;
        if(i = val & ~SIZE_OF_HASH)
            val = (val ^ (i >> 12)) & SIZE_OF_HASH;
    }
    return val;
}

void add_scope() {
    Scope* cur = (Scope*)malloc(sizeof(Scope));
    cur->no = nr_scope;
    nr_scope++;
    cur->next = scope_head;
    scope_head = cur;
}

void delete_scope() {
    assert(scope_head);
    Scope* cur = scope_head;

    Symbol* sym = cur->first_symbol;
    while(sym) {
        assert(hash_table[hash(sym->name)] == sym);
        // 删除的一定是在哈希表的头部
        hash_table[hash(sym->name)] = sym->next_in_hash;
        sym = sym->next_in_scope;
    }
    scope_head = scope_head->next;
    nr_scope--;
    free(cur);
}

void print_scope(Scope* head) {
    if(!head) {
        return;
    }
    Log("%d", head->no);
    print_scope(head->next);
}

void add_func_into_table(Func* func, char* name) {
    Symbol* sym = (Symbol*)malloc(sizeof(Symbol));
    sym->name = name;
    sym->kind = symbol_FUNC;
    sym->func = func;
    sym->scope_num = 0;
    // sym->lineno = sym->func->lineno;
    add_sym_into_table(sym);
}

void add_func(Func* func, int lineno, char* name) {
    Symbol* old_func_sym = find_func(name);
    if(old_func_sym == NULL) {
        add_func_into_table(func, name);
    }
    else if(old_func_sym->func->def && func->def)
        sem_error(4, lineno, "函数出现重复定义");
    else if(!same_func(func, old_func_sym->func)) {
        if(func->def) {
            sem_error(19, lineno, "函数定义与之前的声明冲突");
        }
        else {
            if(old_func_sym->func->def) 
                sem_error(19, lineno, "函数声明与之前的定义冲突");
            else
                sem_error(19, lineno, "函数的多次声明互相冲突");
        }
    }
    else if(func->def) { // 定义了之前申明的函数
        old_func_sym->func->def = 1;
    }
}

Symbol* find_func(char* name) {
    Symbol* sym = hash_table[hash(name)];
    while(sym) {
        // Log("find %d, %s", sym->kind == symbol_FUNC, sym->name);
        if(sym->kind == symbol_FUNC && strcmp(name, sym->name) == 0)
            return sym;
        sym = sym->next_in_hash;
    }
    // Log("find nothing");
    return NULL;
}

// 结构体相关
void add_struct(Type* type, char* name, int lineno) {
    // Log("add struct");
    Symbol* old_sym = find_struct_or_variable(name);
    if(old_sym == NULL) {
        add_struct_into_table(type, name, lineno);
    }
    else {
        if(old_sym->kind == symbol_STRUCTURE)
            sem_error(16, lineno, "结构体的名字与前面定义过的结构体或变量的名字重复");
        else if(old_sym->kind == symbol_VARIABLE)
            sem_error(16, lineno, "结构体的名字与前面定义过的变量的名字重复");
        else {
            // 结构体和函数重名，不管
        }
    }
    // Log("After add struct");
}

Symbol* find_struct_or_variable(char* name) {
    Symbol* sym = hash_table[hash(name)];
    while(sym) {
        // Log("find %d, %s", sym->kind == symbol_FUNC, sym->name);
        if(sym->kind != symbol_FUNC && strcmp(name, sym->name) == 0)
            return sym;
        sym = sym->next_in_hash;
    }
    // Log("find nothing");
    return NULL;
}

void add_struct_into_table(Type* type, char* name, int lineno) {
    Symbol* sym = (Symbol*)malloc(sizeof(Symbol));
    sym->name = name;
    sym->kind = symbol_STRUCTURE;
    sym->type = type;
    sym->scope_num = 1;
    sym->lineno = lineno;
    add_sym_into_table(sym);
}


void add_sym_into_table(Symbol* sym) {
    // 横向
    if(!hash_table[hash(sym->name)]) {
        hash_table[hash(sym->name)] = sym;
    }
    else {
        sym->next_in_hash = hash_table[hash(sym->name)];
        hash_table[hash(sym->name)] = sym;
    }
    // 纵向
    if(sym->kind == symbol_FUNC) {
        sym->next_in_scope = scope_func->first_symbol;
        scope_func->first_symbol = sym;
    }
    else if(sym->kind == symbol_STRUCTURE) {
        sym->next_in_scope = scope_struct->first_symbol;
        scope_struct->first_symbol = sym;
    }
    else {
        sym->next_in_scope = scope_head->first_symbol;
        scope_head->first_symbol = sym;
    }
}

void add_variable(Type* type, char* name, int lineno, int struct_para_var) {
    // 变量和函数重名，不管
    Symbol* old_sym = find_struct_or_variable(name);
    if(old_sym == NULL) {
        add_variable_into_table(type, name, lineno);
    }
    else {
        if(struct_para_var == 0) {
            // 正在添加的是结构体中的域
            if(old_sym->kind == symbol_VARIABLE && old_sym->scope_num == CUR_SCOPE) {
                sem_error(15, lineno, "结构体中域名重复定义");
                Log("%s, %s", old_sym->name, name);
            }
            else {
                add_variable_into_table(type, name, lineno);
            }
        }
        else if(struct_para_var == 0) {
            if(old_sym->kind == symbol_VARIABLE && old_sym->scope_num == CUR_SCOPE) {
                sem_error(3, lineno, "变量（函数参数）出现重复定义");    
            }
            else {
                add_variable_into_table(type, name, lineno);
            }
        }
        else {
            if(old_sym->kind == symbol_STRUCTURE) {
                sem_error(3, lineno, "变量与前面定义过的结构体名字重复");
            }
            else if(old_sym->kind == symbol_VARIABLE && old_sym->scope_num == CUR_SCOPE) {
                sem_error(3, lineno, "同一作用域内变量出现重复定义");
            }
            else {
                add_variable_into_table(type, name, lineno);
            }
        }

    }
}

void add_variable_into_table(Type* type, char* name, int lineno) {
    // Log("new variable: %s", name);
    // print_type(type);
    Symbol* sym = (Symbol*)malloc(sizeof(Symbol));
    sym->name = name;
    sym->kind = symbol_VARIABLE;
    sym->type = type;
    sym->scope_num = nr_scope - 1;
    sym->lineno = lineno;
    add_sym_into_table(sym);
}