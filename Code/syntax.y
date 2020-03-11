%locations
%{
    #include <stdio.h>
    void yyerror(char *msg);
    extern int yylex();
    #define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno; 
    /* for debugging */
    // #define YYDEBUG 1
    // int yydebug = 1;
%}

/* declare types*/
%union {
    int type_int;
    float type_float;
    double type_double;
    enum {
        TYPE_INT, TYPE_FLOAT
    } type_type;
    enum {
        LT, GT, LE, GE, EQ, NE
    } type_relop;
}

/*declared tokens*/
%token <type_int> INT
%token <type_float> FLOAT
%token ID
%token SEMI COMMA ASSIGNOP
%token <type_relop> RELOP
%token PLUS MINUS STAR DIV AND OR DOT NOT
%token <type_type> TYPE
%token LP RP LB RB LC RC
%token STRUCT RETURN IF ELSE WHILE

/* 结合性和优先级 (这里还缺了MINUS作为取负的时候) */
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left MINUS PLUS
%left STAR DIV
%right NEG NOT
%left LP RP LB RB DOT

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
/* High-level Definitions */
Program: ExtDefList
    ;
ExtDefList: /* empty */
    | ExtDef ExtDefList
    ;
ExtDef: Specifier ExtDecList SEMI
    | Specifier SEMI
    | Specifier FunDec CompSt
    ;
ExtDecList: VarDec
    | VarDec COMMA ExtDecList
    ;

/* Specifiers */
Specifier: TYPE
    | StructSpecifier
    ;
StructSpecifier: STRUCT OptTag LC DefList RC
    | STRUCT Tag
    ;
OptTag: /* empty */
    | ID
    ;
Tag: ID
    ;

/* Declarators */
VarDec: ID
    | VarDec LB INT RB
    ;
FunDec: ID LP VarList RP
    | ID LP RP
    ;
VarList: ParamDec COMMA VarList
    | ParamDec
    ;
ParamDec: Specifier VarDec
    ;

/* Statements */
CompSt: LC DefList StmtList RC
    ;
StmtList: /* empty */
    | Stmt StmtList
    ;
Stmt: Exp SEMI
    | CompSt
    | RETURN Exp SEMI
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE
    | IF LP Exp RP Stmt ELSE Stmt
    | WHILE LP Exp RP Stmt
    ;

/* Local Definitions */
DefList: /* empty */
    | Def DefList
    ;
Def: Specifier DecList SEMI
    ;
DecList: Dec
    | Dec COMMA DecList
    ;
Dec: VarDec
    | VarDec ASSIGNOP Exp
    ;

/* Expressions */
Exp: Exp ASSIGNOP Exp
    | Exp AND Exp 
    | Exp OR Exp
    | Exp RELOP Exp
    | Exp PLUS Exp
    | Exp MINUS Exp
    | Exp STAR Exp
    | Exp DIV Exp
    | LP Exp RP
    | MINUS Exp %prec NEG
    | NOT Exp
    | ID LP Args RP
    | ID LP RP
    | Exp LB Exp RB
    | Exp DOT ID
    | ID
    | INT
    | FLOAT
    ;
Args: Exp COMMA Args
    | Exp
    ;

%%
#include "lex.yy.c"
void yyerror(char *msg) {
    fprintf(stderr, "Error type B at Line %d: %s\n", yylloc.first_line, msg);
}