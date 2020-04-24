#ifndef __VARIABLE_TYPE_H__
#define __VARIABLE_TYPE_H__


// 定义表示结构体的链表的结点
typedef struct FieldList_ FieldList;
typedef struct Type_ Type;

struct FieldList_
{
    char* name;
    Type* type;
    FieldList* next;
};


// 定义符号的类型
struct Type_
{
    enum { 
        BASIC, ARRAY, STRUCTURE
        } kind;
    union {
        enum{ BASIC_INT, BASIC_FLOAT } basic;
        struct {
            Type* elem;
            int size;
        } array;
        FieldList* field;
    };
};

// // 定义函数参数
// typedef struct Para_ Para;
// struct Para_
// {
//     Type* type;
//     Para* next;
// };

// 定义函数
typedef struct Func_ Func;
struct Func_
{
    int lineno;
    Type* ret_type;
    FieldList* para;
    int def; // 函数是否已经定义（用于判断是否有函数申明了但是未定义的错误）
};

Type* new_type_int();
Type* new_type_float();
Type* new_type_array(Type* elem, int num);
Type* new_type_struct();
void print_type(Type* elem);
int same_type(Type* t1, Type* t2);

Func* new_func(Type* ret_type, int lineno, int declare);
void print_func(Func* func);
// FieldList* new_para(Type* type, FieldList* next, char* name);
FieldList* add_field(FieldList* field_list, Type* type, char* name, int in_para, int lineno);
FieldList* new_field(Type* type, char* name);
int same_func(Func* func1, Func* func2);
int same_struct(FieldList* field1, FieldList* field2);
Type* find_field(Type* type, char* name);

void print_func_table();
void print_struct_table();
int cnt;

#endif