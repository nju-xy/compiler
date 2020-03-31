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


#endif