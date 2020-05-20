#include "common.h"

extern int yyrestart(FILE * f);
extern int yyparse();
extern FILE * fp_mips;

void init() {
    error_flag = 0;
    symbol_table_init();
}

int main(int argc, char ** argv) {
    if(argc <= 2) return 1;
    FILE * f = fopen(argv[1], "r");
    fp_mips = fopen(argv[2], "w");
    
    if(!f) {
        perror(argv[1]);
        return 1;
    }
    
    init();
    yyrestart(f);
    yyparse();
    if(!error_flag) {
        // draw_tree(root, 0);
        semantic_analyzer(root);
        gen_mips();
    }
    return 0;
}