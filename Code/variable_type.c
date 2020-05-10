#include "common.h"
extern Scope* scope_head, * scope_struct, * scope_func;
Type* new_type_int() {
    Type* new_int = (Type*)malloc(sizeof(Type));
    new_int->kind = BASIC;
    new_int->basic = BASIC_INT;
    new_int->width = 4;
    return new_int;
}

Type* new_type_float() {
    Type* new_float = (Type*)malloc(sizeof(Type));
    new_float->kind = BASIC;
    new_float->basic = BASIC_FLOAT;
    new_float->width = 4;
    return new_float;
}

Type* new_type_array(Type* elem, int num) {
    Type* new_arr = (Type*)malloc(sizeof(Type));
    new_arr->kind = ARRAY;
    new_arr->array.elem = elem;
    new_arr->array.size = num;
    new_arr->width = elem->width * num;
    return new_arr;
}

Type* new_type_struct() {
    Type* new_struct = (Type*)malloc(sizeof(Type));
    new_struct->kind = STRUCTURE;
    // 域都存在此时的符号表的最新一层作用域内
    Symbol* sym = scope_head->first_symbol;
    // FieldList* field = NULL;
    new_struct->field = NULL;
    new_struct->width = 0;
    while(sym) {
        // FieldList* next_field = new_field(sym->type, sym->name);
        // next_field->shift = new_struct->width;
        // new_struct->width += sym->type->width;
        // if(!field) {
        //     new_struct->field = field = next_field;
        // }
        // else {
        //     field->next = next_field;
        //     field = next_field;
        // }
        FieldList* field = new_field(sym->type, sym->name);
        field->next = new_struct->field;
        new_struct->field = field;
        sym = sym->next_in_scope;
    }
    int shift = 0;
    for(FieldList* field = new_struct->field; field; field = field->next) {
        field->shift = shift;
        shift += field->type->width;
    }
    new_struct->width = shift;
    return new_struct;
}

void print_type(Type* elem) {
    if(!elem) {
        Log("NonType");
        return;
    }
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
        Log("struct: ");
        FieldList* field = elem->field;
        while(field) {
            Log("name: %s", field->name);
            print_type(field->type);
            field = field->next;
        }
    }
}

int same_type(Type* t1, Type* t2) {
    assert(t1);
    assert(t2);
    // Log("嘤");
    if((void*)t1 == (void*)t2)
        return 1;
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
        return same_struct(t1->field, t2->field);
    }
    return 1;
}

FieldList* new_field(Type* type, char* name) {
    FieldList* para = (FieldList*)malloc(sizeof(FieldList));
    para->shift = 0;
    para->type = type;
    para->next = NULL;
    para->name = name;
    return para;
}

// FieldList* add_field(FieldList* field_list, Type* type, char* name, int in_para, int lineno) { 
//     //Log("before add_field, %d", lineno);
//     // 在field_list的结尾加上参数一个(type, name)的field
//     FieldList* field = (FieldList*)malloc(sizeof(FieldList));
//     field->type = type;
//     field->next = NULL;
//     field->name = name;

//     assert(field_list);

//     FieldList* cur = field_list;
//     while(1) {
//         if(!in_para) { // 在结构体里面，重名就要报错的
//             if(cur->name && strcmp(cur->name, name) == 0) {
//                 sem_error(15, lineno, "同一个结构体中域名重复定义");
//                 return field_list;
//             }
//         }
//         if(!cur->next)
//             break;
//         cur = cur->next;
//     }
    
//     cur->next = field;
//     // field->next = field_list;
//     // field_list = field;
//     // Log("after add_field");
//     return field_list;
// }

Func* new_func(Type* ret_type, int lineno, int declare) {
    // 参数都存在此时的符号表的最新一层作用域内
    Func* func = (Func*)malloc(sizeof(Func));
    func->ret_type = ret_type;
    func->lineno = lineno;
    if(declare)
        func->def = 0;
    else 
        func->def = 1;
    Symbol* sym = scope_head->first_symbol;

    func->para = NULL;
    while(sym) {
        sym->is_param = 1;
        FieldList* new_para = new_field(sym->type, sym->name);
        if(!func->para) {
            func->para = new_para;
        }
        else { // 注意新的参数加在头部
            new_para->next = func->para;
            func->para = new_para;
        }
        sym = sym->next_in_scope;
    }
    return func;
}


void print_func_table() {
    Symbol* sym = scope_func->first_symbol;
    while(sym) {
        Log("Func name: %s", sym->name);
        // print_func(sym->func);
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


int same_struct(FieldList* field1, FieldList* field2) {
    // 判断两个结构体内部是否相同（不包含名字）
    FieldList* p1 = field1;
    FieldList* p2 = field2;
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


void print_struct_table() {
    Log("struct table:");
    Symbol* sym = scope_struct->first_symbol;
    while(sym) {
        Log("struct name: %s", sym->name);
        //print_type(sym->type);
        sym = sym->next_in_scope;
    }
}

FieldList* find_field(Type* type, char* name) {
    assert(type->kind == STRUCTURE);
    FieldList* field = type->field;
    while(field) {
        // Log("%s %d", field->name, field->shift);
        if(strcmp(field->name, name) == 0)
            return field;
        field = field->next;
    }
    return NULL;
}