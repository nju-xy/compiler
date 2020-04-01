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
        FieldList* structure;
    };
};

// 定义函数参数
typedef struct Para_ Para;
struct Para_
{
    Type* type;
    Para* next;
};

// 定义函数
typedef struct Func_ Func;
struct Func_
{
    char* name;
    Type* ret_type;
    Para* para;
};

Type* new_type_int();
Type* new_type_float();
Type* new_type_array(Type* elem, int num);
// int cal_size(Type* elem);
void print_type(Type* elem);

void new_func(Type* ret_type, char* name, Para* para, int declare);
void print_func(Func* func);
Para* new_para(Type* type, Para* next);

#endif