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

Type* new_type_struct(Type* elem, int num) {
    Type* new_str = (Type*)malloc(sizeof(Type));
    new_str->kind = STRUCTURE;
    TODO();
    return new_str;
}

int cal_size(Type* elem) {
    if(elem->kind == BASIC) {
        if(elem->basic == BASIC_INT) 
            return sizeof(int);
        else 
            return sizeof(float);
    }
    else if(elem->kind == ARRAY) {
        return elem->array.size;
    }
    else { // struct
        TODO();
        return 0;
    }
}

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
        FieldList* field = elem->structure;
        while(field) {
            Log("name: %s", field->name);
            print_type(field->type);
            field = field->next;
        }
    }
}

Para* new_para(Type* type, Para* next) {
    Para* para = (Para*)malloc(sizeof(Para));
    para->type = type;
    para->next = next;
    return para;
}

void new_func(Type* ret_type, char* name, Para* para, int declare) {
    Func* func = (Func*)malloc(sizeof(Func));
    func->name = name;
    func->para = para;
    func->ret_type = ret_type;
    print_func(func);
}

void print_func(Func* func) {
    Log("Func name: %s", func->name);
    Log("return type:");
    print_type(func->ret_type);
    Para* para = func->para;
    while(para) {
        Log("Para");
        print_type(para->type);
        para = para->next;
    }
    Log("End func");
}