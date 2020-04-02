#include "common.h"

Type* new_type_int() {
    Type* new_int = (Type*)malloc(sizeof(Type));
    new_int->kind = BASIC;
    new_int->basic = BASIC_INT;
    return new_int;
}

Type* new_type_float() {
    Type* new_float = (Type*)malloc(sizeof(Type));
    new_float->kind = BASIC;
    new_float->basic = BASIC_FLOAT;
    return new_float;
}

Type* new_type_array(Type* elem, int num) {
    Type* new_arr = (Type*)malloc(sizeof(Type));
    new_arr->kind = ARRAY;
    new_arr->array.elem = elem;
    new_arr->array.size = num;
    return new_arr;
}

Type* new_type_struct(FieldList* field) {
    Type* new_struct = (Type*)malloc(sizeof(Type));
    new_struct->kind = STRUCTURE;
    new_struct->field = field;
    return new_struct;
}

// int cal_size(Type* elem) {
//     if(elem->kind == BASIC) {
//         if(elem->basic == BASIC_INT) 
//             return sizeof(int);
//         else 
//             return sizeof(float);
//     }
//     else if(elem->kind == ARRAY) {
//         return elem->array.size;
//     }
//     else { // struct
//         TODO();
//         return 0;
//     }
// }

void print_type(Type* elem) {
    if(elem->kind == BASIC) {
        if(elem->basic == BASIC_INT) 
            Log("int");
        else 
            Log("float");
    }
    else if(elem->kind == ARRAY) {
        Log("array: Size:%d", elem->array.size);
        Log("Type: ");
        print_type(elem->array.elem);
    }
    else { // struct
        Log("struct: \n");
        FieldList* field = elem->field;
        while(field) {
            Log("name: %s", field->name);
            print_type(field->type);
            field = field->next;
        }
    }
}

int same_type(Type* t1, Type* t2) {
    if(t1->kind != t2->kind)
        return 0;
    if(t1->kind == BASIC) {
        if(t1->basic != t2->basic)
            return 0;
    }
    else if(t1->kind == ARRAY) {
        // 只要求elem类型同，不要求size同
        if(!same_type(t1->array.elem, t2->array.elem))
            return 0;
    }
    else {
        FieldList* f1 = t1->field;
        FieldList* f2 = t2->field;
        while(1) {
            if(!f1 && !f2)
                return 1;
            else if(!f1 || !f2)
                return 0;
            if(!same_type(f1->type, f2->type))
                return 0;
            f1 = f1->next;
            f2 = f2->next;
        }
    }
    return 1;
}

FieldList* new_para(Type* type, FieldList* next, char* name) {
    FieldList* para = (FieldList*)malloc(sizeof(FieldList));
    para->type = type;
    para->next = next;
    para->name = name;
    return para;
}

Func* new_func(Type* ret_type, FieldList* para, int lineno, int declare) {
    Func* func = (Func*)malloc(sizeof(Func));
    func->para = para;
    func->ret_type = ret_type;
    func->lineno = lineno;
    if(declare)
        func->def = 0;
    else 
        func->def = 1;
    // print_func(func);
    return func;
}

void print_func_table() {
    Symbol* sym = scope_func->first_symbol;
    while(sym) {
        Log("Func name: %s", sym->name);
        print_func(sym->func);
        sym = sym->next_in_scope;
    }
}

void print_func(Func* func) {
    Log("return type:");
    print_type(func->ret_type);
    FieldList* para = func->para;
    while(para) {
        Log("Para");
        print_type(para->type);
        para = para->next;
    }
    Log("End func");
}


int same_func(Func* func1, Func* func2) {
    // 判断两个函数的类型是否相同（不包含名字）
    if(!same_type(func1->ret_type, func2->ret_type)) 
        return 0;
    FieldList* p1 = func1->para;
    FieldList* p2 = func2->para;
    while(1) {
        if(!p1 && !p2)
            return 1;
        if(!p1 || !p2)
            return 0;
        if(!same_type(p1->type, p2->type))
            return 0;
        p1 = p1->next;
        p2 = p2->next;
    }
    return 1;
}