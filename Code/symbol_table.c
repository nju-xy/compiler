#include "common.h"

void symbol_test() {
    
}

void symbol_table_init() {
    add_scope(0);
}

void add_scope(int lineno) {
    Scope* cur = (Scope*)malloc(sizeof(Scope));
    cur->lineno = lineno;
    cur->next = scope_head;
    scope_head = cur;
}

void delete_scope() {
    assert(scope_head);
    Scope* cur = scope_head;
    scope_head = scope_head->next;
    free(cur);
}

void print_scope(Scope* head) {
    if(!head) {
        printf("\n");
        return;
    }
    printf("%d ", head->lineno);
    print_scope(head->next);
}