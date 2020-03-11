#include <stdio.h>

// extern int yyrestart(FILE * f);
// extern int yyparse();

int main(int argc, char ** argv) {
    if(argc <= 1) return 1;
    FILE * f = fopen(argv[1], "r");
    if(!f) {
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    yyparse();
    return 0;
}
// extern FILE* yyin;
// extern int yylex();
// int main(int argc, char ** argv){
//     if(argc > 1) {
//         if(!(yyin = fopen(argv[1], "r"))) { // 第一个参数为要打开的文件名
//             perror(argv[1]); // 如果打开文件失败，报错
//             return 1;
//         }
//     }
//     while(yylex() != 0); //打开文件成功，就调用yylex()进行词法分析
//     return 0;
// }