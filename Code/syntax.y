%locations
%{
    /* for debugging */
    // #define YYDEBUG 1
    // int yydebug = 1;
    
    #include "common.h"
    #include "lex.yy.c"
%}

/* declare types */
%union {
    Node_t * node;
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
    | error SEMI { 
        yyerror("Wrong ExtDef"); 
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
        yyerror("Wrong StructSpecifier"); 
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
    | VarDec LB error RB { 
        yyerror("Wrong VarDec"); 
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
        yyerror("Wrong FunDec"); 
    }
    ;
VarList: ParamDec COMMA VarList {
        $$ = create_node("VarList", @1.first_line, 0);
        add_children($$, 3, $1, $2, $3);
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
    | LC error RC { 
        yyerror("Wrong CompSt"); 
    }
    ;
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
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {
        $$ = create_node("Stmt", @1.first_line, 0);
        add_children($$, 5, $1, $2, $3, $4, $5);
    }
    | IF LP Exp RP Stmt ELSE Stmt {
        $$ = create_node("Stmt", @1.first_line, 0);
        add_children($$, 7, $1, $2, $3, $4, $5, $6, $7);
    }
    | IF LP error RP Stmt %prec LOWER_THAN_ELSE { 
        yyerror("Wrong If condition"); 
    }
    | IF LP error RP Stmt ELSE Stmt { 
        yyerror("Wrong If condition"); 
    }
    | WHILE LP Exp RP Stmt {
        $$ = create_node("Stmt", @1.first_line, 0);
        add_children($$, 5, $1, $2, $3, $4, $5);
    }
    | WHILE LP error RP Stmt {
        yyerror("Wrong while condition");
    }
    | RETURN error SEMI  {
        yyerror("Wrong return value");
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
    | Specifier error SEMI  {
        yyerror("Wrong Def");
    }
    | error SEMI {
        yyerror("Wrong Def or Stmt");
    }
    ;
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
        yyerror("Wrong exp in ()");
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
        yyerror("Wrong function call");
    }
    | Exp LB Exp RB {
        $$ = create_node("Exp", @1.first_line, 0);
        add_children($$, 4, $1, $2, $3, $4);
    }
    | Exp LB error RB {
        yyerror("Wrong array reference");
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
void yyerror(char *msg) {
    error_flag = 1;
    fprintf(stderr, "Error type B at Line %d: %s\n", yylloc.first_line, msg);
}