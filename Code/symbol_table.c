#include "common.h"

void symbol_table_init() {
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
    scope_head = scope_head->next;
    nr_scope--;
    free(cur);
}

void print_scope(Scope* head) {
    if(!head) {
        printf("\n");
        return;
    }
    printf("%d ", head->no);
    print_scope(head->next);
}

void add_func_into_table(Func* func) {
    Symbol* sym = (Symbol*)malloc(sizeof(Symbol));
    sym->name = func->name;
    sym->kind = symbol_FUNC;
    sym->func = func;
    sym->scope_num = 0;
    sym->lineno = sym->func->lineno;
    add_sym_into_table(sym);
}

void add_sym_into_table(Symbol* sym) {
    if(!hash_table[hash(sym->name)]) {
        hash_table[hash(sym->name)] = sym;
    }
    else {
        sym->next_in_hash = hash_table[hash(sym->name)];
        hash_table[hash(sym->name)] = sym;
    }
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

void add_func(Func* func, int lineno) {
    Func* old_func = find_func(func->name);
    if(old_func == NULL) {
        add_func_into_table(func);
    }
    else if(old_func->def && func->def)
        sem_error(4, lineno, "函数出现重复定义");
    else if(!same_func(func, old_func)) {
        if(func->def) {
            sem_error(19, lineno, "函数定义与之前的声明冲突");
        }
        else {
            if(old_func->def) 
                sem_error(19, lineno, "函数声明与之前的定义冲突");
            else
                sem_error(19, lineno, "函数的多次声明互相冲突");
        }
    }
    else if(func->def) { // 定义了之前申明的函数
        old_func->def = 1;
    }
}

Func* find_func(char* name) {
    Symbol* sym = hash_table[hash(name)];
    while(sym) {
        // Log("find %d, %s", sym->kind == symbol_FUNC, sym->name);
        if(sym->kind == symbol_FUNC && strcmp(name, sym->name) == 0)
            return sym->func;
        sym = sym->next_in_hash;
    }
    // Log("find nothing");
    return NULL;
}