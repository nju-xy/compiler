%locations
%error-verbose
%{
    /* for debugging */
    // #define YYDEBUG 1
    // int yydebug = 1;
    
    #include "common.h"
    #include "lex.yy.c"
%}

/* declare types */
%union {
    Syntax_Tree_Node_t * node;
}


/* declared tokens */
%token <node> INT FLOAT ID SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR DOT NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE

/* declare non-terminals */
%type <node> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Def DecList Dec Exp Args

/* 结合性和优先级, 越下面优先级越高 */
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
Program: ExtDefList {  
            $$ = root = create_node("Program", @1.first_line, 0);
            add_children($$, 1, $1);
        }
    ;
ExtDefList: /* empty */ {
        $$ = NULL;
        @$.first_line = yylineno;
    }
    | ExtDef ExtDefList {
        $$ = create_node("ExtDefList", @1.first_line, 0);
        add_children($$, 2, $1, $2);
    }
    ;
ExtDef: Specifier ExtDecList SEMI {
        $$ = create_node("ExtDef", @1.first_line, 0);
        add_children($$, 3, $1, $2, $3);
    }
    | Specifier SEMI {
        $$ = create_node("ExtDef", @1.first_line, 0);
        add_children($$, 2, $1, $2);
    }
    | Specifier FunDec CompSt {
        $$ = create_node("ExtDef", @1.first_line, 0);
        add_children($$, 3, $1, $2, $3);
    }
    | Specifier error CompSt {
        yyerror2("Wrong ExtDef"); 
    }
    | error FunDec CompSt {
        yyerror2("Wrong ExtDef"); 
    }
    | error CompSt {
        yyerror2("Wrong ExtDef"); 
    }
    | error SEMI { 
        yyerror2("Wrong ExtDef"); 
    }
    | error ExtDecList SEMI {
        yyerror2("Wrong ExtDef"); 
    }
    | Specifier error SEMI {
        yyerror2("Wrong ExtDef"); 
    }
    ;
ExtDecList: VarDec {
        $$ = create_node("ExtDecList", @1.first_line, 0);
        add_children($$, 1, $1);
    }
    | VarDec COMMA ExtDecList {
        $$ = create_node("ExtDecList", @1.first_line, 0);
        add_children($$, 3, $1, $2, $3);
    }
    ;

/* Specifiers */
Specifier: TYPE {
        $$ = create_node("Specifier", @1.first_line, 0);
        add_children($$, 1, $1);
    }
    | StructSpecifier {
        $$ = create_node("Specifier", @1.first_line, 0);
        add_children($$, 1, $1);
    }
    ;
StructSpecifier: STRUCT OptTag LC DefList RC {
        $$ = create_node("StructSpecifier", @1.first_line, 0);
        add_children($$, 5, $1, $2, $3, $4, $5);
    }
    | STRUCT Tag {
        $$ = create_node("StructSpecifier", @1.first_line, 0);
        add_children($$, 2, $1, $2);
    }
    | STRUCT OptTag LC error RC { 
        yyerror2("Wrong StructSpecifier"); 
    }
    ;
OptTag: /* empty */ {
        $$ = NULL;
    }
    | ID {
        $$ = create_node("OptTag", @1.first_line, 0);
        add_children($$, 1, $1);
    }
    ;
Tag: ID {
        $$ = create_node("Tag", @1.first_line, 0);
        add_children($$, 1, $1);
    }
    ;

/* Declarators */
VarDec: ID {
        $$ = create_node("VarDec", @1.first_line, 0);
        add_children($$, 1, $1);
    }
    | VarDec LB INT RB {
        $$ = create_node("VarDec", @1.first_line, 0);
        add_children($$, 4, $1, $2, $3, $4);
    }
    ;
FunDec: ID LP VarList RP {
        $$ = create_node("FunDec", @1.first_line, 0);
        add_children($$, 4, $1, $2, $3, $4);
    }
    | ID LP RP {
        $$ = create_node("FunDec", @1.first_line, 0);
        add_children($$, 3, $1, $2, $3);
    }
    | ID LP error RP { 
        yyerror2("Wrong FunDec"); 
    }
    | error LP VarList RP {
        yyerror2("Wrong FunDec"); 
    }
    ;
VarList: ParamDec COMMA VarList {
        $$ = create_node("VarList", @1.first_line, 0);
        add_children($$, 3, $1, $2, $3);
    }
    | error COMMA VarList {
        yyerror2("Wrong VarList"); 
    }
    | ParamDec {
        $$ = create_node("VarList", @1.first_line, 0);
        add_children($$, 1, $1);
    }
    ;
ParamDec: Specifier VarDec {
        $$ = create_node("ParamDec", @1.first_line, 0);
        add_children($$, 2, $1, $2);
    }
    ;

/* Statements */
/* 这里碰到的问题是某些难以判断的error SEMI应该是Stmt还是Def(暂时先都当做Def) */
CompSt: LC DefList StmtList RC {
        $$ = create_node("CompSt", @1.first_line, 0);
        add_children($$, 4, $1, $2, $3, $4);
    }
    | LC DefList error RC { 
        yyerror2("Wrong CompSt"); 
    }
    ;
    /* 这里删除了 LC error RC */
StmtList: /* empty */ {
        $$ = NULL;
    }
    | Stmt StmtList  {
        $$ = create_node("StmtList", @1.first_line, 0);
        add_children($$, 2, $1, $2);
    }
    ;
Stmt: Exp SEMI {
        $$ = create_node("Stmt", @1.first_line, 0);
        add_children($$, 2, $1, $2);
    }
    | CompSt {
        $$ = create_node("Stmt", @1.first_line, 0);
        add_children($$, 1, $1);
    }
    | RETURN Exp SEMI {
        $$ = create_node("Stmt", @1.first_line, 0);
        add_children($$, 3, $1, $2, $3);
    }
    | RETURN error SEMI  {
        yyerror2("Wrong return value");
    }
    | RETURN Exp error  {
        yyerror2("Missing SEMI after return");
    }
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {
        $$ = create_node("Stmt", @1.first_line, 0);
        add_children($$, 5, $1, $2, $3, $4, $5);
    }
    | IF LP error RP Stmt %prec LOWER_THAN_ELSE { 
        yyerror2("Wrong If condition"); 
    }
    | IF LP Exp RP error %prec LOWER_THAN_ELSE { 
        yyerror2("Wrong If condition"); 
    }
    | IF LP Exp RP Stmt ELSE Stmt {
        $$ = create_node("Stmt", @1.first_line, 0);
        add_children($$, 7, $1, $2, $3, $4, $5, $6, $7);
    }
    | IF LP error RP Stmt ELSE Stmt { 
        yyerror2("Wrong If condition"); 
    }
    | IF LP Exp RP error ELSE Stmt {
        yyerror2("Wrong if stmt");
    }
    | IF LP Exp RP Stmt ELSE error {
        yyerror2("Wrong else stmt");
    }
    | WHILE LP Exp RP Stmt {
        $$ = create_node("Stmt", @1.first_line, 0);
        add_children($$, 5, $1, $2, $3, $4, $5);
    }
    | WHILE LP error RP Stmt {
        yyerror2("Wrong while condition");
    }
    | WHILE LP Exp RP error {
        yyerror2("Wrong while condition");
    }
    | error SEMI {
        yyerror2("Wrong stmt");
    }
    ;

/* Local Definitions */
DefList: /* empty */  {
        $$ = NULL;
    }
    | Def DefList  {
        $$ = create_node("DefList", @1.first_line, 0);
        add_children($$, 2, $1, $2);
    }
    ;
Def: Specifier DecList SEMI {
        $$ = create_node("Def", @1.first_line, 0);
        add_children($$, 3, $1, $2, $3);
    }
    | Specifier DecList error SEMI {
        yyerror2("Wrong Def");
    }
    | Specifier error SEMI  {
        yyerror2("Wrong Def");
    }
    ;
    /* 删掉了 
    | error DecList SEMI {
        yyerror2("Wrong Def");
    }
    | error SEMI {
        yyerror2("Wrong Def");
    }
    */
DecList: Dec {
        $$ = create_node("DecList", @1.first_line, 0);
        add_children($$, 1, $1);
    }
    | Dec COMMA DecList {
        $$ = create_node("DecList", @1.first_line, 0);
        add_children($$, 3, $1, $2, $3);
    }
    ;
Dec: VarDec {
        $$ = create_node("Dec", @1.first_line, 0);
        add_children($$, 1, $1);
    }
    | VarDec ASSIGNOP Exp {
        $$ = create_node("Dec", @1.first_line, 0);
        add_children($$, 3, $1, $2, $3);
    }
    ;

/* Expressions */
Exp: Exp ASSIGNOP Exp {
        $$ = create_node("Exp", @1.first_line, 0);
        add_children($$, 3, $1, $2, $3);
    }
    | error ASSIGNOP Exp {
        yyerror2("Wrong Exp");
    }
    | Exp AND Exp  {
        $$ = create_node("Exp", @1. first_line, 0);
        add_children($$, 3, $1, $2, $3);
    }
    | Exp OR Exp {
        $$ = create_node("Exp", @1.first_line, 0);
        add_children($$, 3, $1, $2, $3);
    }
    | Exp RELOP Exp {
        $$ = create_node("Exp", @1. first_line, 0);
        add_children($$, 3, $1, $2, $3);
    }
    | Exp PLUS Exp {
        $$ = create_node("Exp", @1.first_line, 0);
        add_children($$, 3, $1, $2, $3);
    }
    | Exp MINUS Exp {
        $$ = create_node("Exp", @1.first_line, 0);
        add_children($$, 3, $1, $2, $3);
    }
    | Exp STAR Exp {
        $$ = create_node("Exp", @1.first_line, 0);
        add_children($$, 3, $1, $2, $3);
    }
    | Exp DIV Exp {
        $$ = create_node("Exp", @1.first_line, 0);
        add_children($$, 3, $1, $2, $3);
    }
    | LP Exp RP {
        $$ = create_node("Exp", @1.first_line, 0);
        add_children($$, 3, $1, $2, $3);
    }
    | LP error RP {
        yyerror2("Wrong exp in ()");
    }
    | MINUS Exp %prec NEG {
        $$ = create_node("Exp", @1.first_line, 0);
        add_children($$, 2, $1, $2);
    }
    | NOT Exp {
        $$ = create_node("Exp", @1.first_line, 0);
        add_children($$, 2, $1, $2);
    }
    | ID LP Args RP {
        $$ = create_node("Exp", @1.first_line, 0);
        add_children($$, 4, $1, $2, $3, $4);
    }
    | ID LP RP {
        $$ = create_node("Exp", @1.first_line, 0);
        add_children($$, 3, $1, $2, $3);
    }
    | ID LP error RP {
        yyerror2("Wrong function call");
    }
    | Exp LB Exp RB {
        $$ = create_node("Exp", @1.first_line, 0);
        add_children($$, 4, $1, $2, $3, $4);
    }
    | Exp DOT ID {
        $$ = create_node("Exp", @1.first_line, 0);
        add_children($$, 3, $1, $2, $3);
    }
    | ID {
        $$ = create_node("Exp", @1.first_line, 0);
        add_children($$, 1, $1);
    }
    | INT {
        $$ = create_node("Exp", @1.first_line, 0);
        add_children($$, 1, $1);
    }
    | FLOAT {
        $$ = create_node("Exp", @1.first_line, 0);
        add_children($$, 1, $1);
    }
    ;
Args: Exp COMMA Args {
        $$ = create_node("Args", @1.first_line, 0);
        add_children($$, 3, $1, $2, $3);
    }
    | Exp {
        $$ = create_node("Args", @1.first_line, 0);
        add_children($$, 1, $1);
    }
    ;

%%
void yyerror(const char *msg) {
    error_flag = 1;
    fprintf(stderr, "Error type B at Line %d: %s.\n", yylloc.first_line, msg);
}

void yyerror2(const char *msg) {
    error_flag = 1;
    //fprintf(stderr, "Error type B at Line %d: %s.\n", yylloc.first_line, msg);
}