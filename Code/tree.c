#include "common.h"

Node_t * create_node(const char * name, int lineno, int terminal) {
    Node_t * node = (Node_t *)malloc(sizeof(Node_t));
    node->first_child = NULL;
    node->next_sibling = NULL;
    node->val.type_double = 0;
    node->lineno = lineno;
    node->terminal = terminal;
    strncpy(node->name, name, 15);
    return node;
}

void add_children(Node_t * father, int num, ...) {
    assert(father);
    if(num == 0) return;
    va_list ap;
    va_start(ap, num); 

    
    Node_t * child = va_arg(ap, Node_t *);
    
    // int i = 1;
    // while(!child) {
    //     i++;
    //     if(i > num) {
    //         va_end(ap);
    //         // printf("嘤\n");
    //         return;
    //     }
    //     child = va_arg(ap, Node_t *);
    // }     
    // assert(child);
    // father->first_child = child;
    // for(; i <= num; ++i) {
    //     Node_t *next_sibling = va_arg(ap, Node_t *);
    //     if(next_sibling) {
    //         child->next_sibling = next_sibling;
    //         child = next_sibling;
    //     }
    // }

    if(!child && num == 1) {
        va_end(ap);
        return;
    }
    assert(child);
    // 这里默认有多个子结点的时候, 第一个子结点非空
    father->first_child = child;
    for(int i = 1; i < num; ++i) {
        Node_t *next_sibling = va_arg(ap, Node_t *);
        if(next_sibling) {
            child->next_sibling = next_sibling;
            child = next_sibling;
        }
    }
    
    va_end(ap);
}

void print_node(Node_t *node) {
    printf("%s", node->name);
    if(!node->terminal) {
        printf(" (%d)", node->lineno);
    }
    else if(strncmp(node->name, "ID", 15) == 0) {
        printf(": %s", node->val.id_name);
    }
    else if(strncmp(node->name, "TYPE", 15) == 0) {
        if(node->val.type_type == TYPE_INT)
            printf(": int");
        else if(node->val.type_type == TYPE_FLOAT)
            printf(": float");
    }
    else if(strncmp(node->name, "INT", 15) == 0) {
        printf(": %d", node->val.type_int);
    }
    else if(strncmp(node->name, "FLOAT", 15) == 0) {
        printf(": %f", node->val.type_float);
    }
    printf("\n");
}

void draw_tree(Node_t *node, int retract) { // dfs
    if(!node) return;
    // print the node
    for(int i = 0; i < retract; ++i) {
        printf(" ");
    }
    print_node(node);
    // print the children
    Node_t * child = node->first_child;
    while(child) {
        draw_tree(child, retract + 2);
        child = child->next_sibling;
    }
}