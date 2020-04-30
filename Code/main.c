#include "common.h"

extern int yyrestart(FILE * f);
extern int yyparse();

void init() {
    error_flag = 0;
    symbol_table_init();
}

int main(int argc, char ** argv) {
    if(argc <= 2) return 1;
    FILE * f = fopen(argv[1], "r");
    fp_intercode = fopen(argv[2], "w");
    // fp_intercode = fopen("../intercode/test.ir", "w");
    // FILE * f = fopen("../Test/test.cmm", "r");
    // FILE * f = fopen("../../实验测试文件/L2/Tests_advanced/tests/A-10.0.cmm", "r");
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
    }
    return 0;
}