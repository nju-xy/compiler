%option yylineno
%{
    #include "syntax.tab.h"
    #include "common.h"
    #define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno;
    void multiline_comment();
%}

delim   [ \n\t\r]
ws      {delim}+
letter  [A-Za-z]
letter_  [A-Za-z_]
digit   [0-9]
alpha   {letter_}|{digit}
id      {letter_}{alpha}*
    // int包含 十进制数字 (除了0以外都不是0开头), 八进制数字(0开头), 十六进制(0x开头)
not_in_dec  {letter_}
wrong_dec   [1-9][0-9]*{not_in_dec}{alpha}*
dec         0|[1-9][0-9]*
not_in_oct  {letter_}|[89]
wrong_oct   0[0-7]*{not_in_oct}{alpha}*
oct         0[0-7]+
not_in_hex  [G-Zg-z_]
wrong_hex   0[xX][0-9a-fA-F]*{not_in_hex}{alpha}*
hex         0[xX][0-9a-fA-F]+
    // float分为指数形式和非指数形式; 非指数形式小数点前后都要有数字; 指数形式基数部分小数点前后至少有一个有数字, 指数部分由正负号(可选)和一串数字(0-9)组成
    // 错误float(这里只识别一部分, 只考虑出现数字和.和eE的情况)
    //1. 指数形式, E后面没有数字
wrong_float     {digit}+\.{digit}*[Ee]|{digit}*\.{digit}+[Ee]
float           {digit}+\.{digit}+|{digit}+\.{digit}*[Ee][+-]?{digit}+|{digit}*\.{digit}+[Ee][+-]?{digit}+
    // 注释分成两种, 单行注释直接正则表达式匹配, 多行匹配前半段, 之后input找到后半段为止
comment     \/\/.*
lcomment    \/\*
%%
    /*规则部分： 模式 {操作}*/
{ws}        {                           }
{comment}   {   // printf("COMMENT ");     
            }
{lcomment}  {   // printf("COMMENT ");  
                multiline_comment();        
            }
{wrong_dec} {   printf("Error type A at Line %d: Wrong int(dec) \'%s\'\n", yylineno, yytext); }
{dec}       {   int ret;    
                sscanf(yytext, "%d", &ret);
                // printf("INT(DEC):%d ", ret); 
                yylval.node = create_node("INT", yylineno, 1); 
                yylval.node->val.type_int = ret;
                return INT;
            }
{wrong_hex} {   printf("Error type A at Line %d: Wrong int(hex) \'%s\'\n", yylineno, yytext); }
{hex}       {   int ret;    
                sscanf(yytext, "%x", &ret);
                // printf("INT(HEX):%d ", ret);  
                yylval.node = create_node("INT", yylineno, 1); 
                yylval.node->val.type_int = ret;
                return INT;
            }
{wrong_oct} {   printf("Error type A at Line %d: Wrong int(oct) \'%s\'\n", yylineno, yytext); }
{oct}       {   int ret;    
                sscanf(yytext, "%o", &ret);
                // printf("INT(OCT):%d ", ret);   
                yylval.node = create_node("INT", yylineno, 1); 
                yylval.node->val.type_int = ret;
                return INT;
            }
{wrong_float} { printf("Error type A at Line %d: Wrong float \'%s\'\n", yylineno, yytext); }
{float}     {   float ret;    
                sscanf(yytext, "%f", &ret);
                //printf("FLOAT:%f ", ret);    
                yylval.node = create_node("FLOAT", yylineno, 1); 
                yylval.node->val.type_float = ret;
                // yylval.node->val.type_float = atof(yytext);
                return FLOAT;
            }
";"         {   //printf("SEMI "); 
                yylval.node = create_node("SEMI", yylineno, 1); 
                return SEMI;
            }
","         {   //printf("COMMA "); 
                yylval.node = create_node("COMMA", yylineno, 1); 
                return COMMA;    
            }
"="         {   //printf("ASSIGNOP ");  
                yylval.node = create_node("ASSIGNOP", yylineno, 1); 
                return ASSIGNOP;    
            }
"<"         {   //printf("RELOP ");
                yylval.node = create_node("RELOP", yylineno, 1); 
                yylval.node->val.type_relop = LT; 
                return RELOP;    
            }
">"         {   //printf("RELOP ");
                yylval.node = create_node("RELOP", yylineno, 1); 
                yylval.node->val.type_relop = GT; 
                return RELOP;    
            }
"<="        {   //printf("RELOP ");
                yylval.node = create_node("RELOP", yylineno, 1); 
                yylval.node->val.type_relop = LE; 
                return RELOP;    
            }
">="        {   //printf("RELOP "); 
                yylval.node = create_node("RELOP", yylineno, 1); 
                yylval.node->val.type_relop = GE; 
                return RELOP;    
            }
"=="        {   //printf("RELOP ");
                yylval.node = create_node("RELOP", yylineno, 1); 
                yylval.node->val.type_relop = EQ; 
                return RELOP;    
            }
"!="        {   //printf("RELOP ");
                yylval.node = create_node("RELOP", yylineno, 1); 
                yylval.node->val.type_relop = NE; 
                return RELOP;    
            }
"+"         {   //printf("PLUS "); 
                yylval.node = create_node("PLUS", yylineno, 1); 
                return PLUS;    
            }
"-"         {   //printf("MINUS "); 
                yylval.node = create_node("MINUS", yylineno, 1); 
                return MINUS;    
            }
"*"         {   //printf("STAR ");    
                yylval.node = create_node("STAR", yylineno, 1); 
                return STAR;    
            }
"/"         {   //printf("DIV ");    
                yylval.node = create_node("DIV", yylineno, 1);     
                return DIV;
            }
"&&"        {   //printf("AND ");   
                yylval.node = create_node("AND", yylineno, 1);      
                return AND;    
            }
"||"        {   //printf("OR ");   
                yylval.node = create_node("OR", yylineno, 1);      
                return OR;    
            }
"."         {   //printf("DOT ");  
                yylval.node = create_node("DOT", yylineno, 1);       
                return DOT;    
            }
"!"         {   //printf("NOT ");    
                yylval.node = create_node("NOT", yylineno, 1);     
                return NOT;    
            }
"int"       {   //printf("TYPE ");    
                yylval.node = create_node("TYPE", yylineno, 1);
                yylval.node->val.type_type = TYPE_INT;      
                return TYPE;    
            }
"float"     {   //printf("TYPE ");     
                yylval.node = create_node("TYPE", yylineno, 1);
                yylval.node->val.type_type = TYPE_FLOAT;     
                return TYPE;    
            }
"("         {   //printf("LP ");        
                yylval.node = create_node("LP", yylineno, 1);
                return LP;    
            }
")"         {   //printf("RP ");       
                yylval.node = create_node("RP", yylineno, 1);
                return RP;    
            }
"["         {   //printf("LB ");       
                yylval.node = create_node("LB", yylineno, 1);
                return LB;    
            }
"]"         {   //printf("RB ");     
                yylval.node = create_node("RB", yylineno, 1);
                return RB;    
            }
"{"         {   //printf("LC ");    
                yylval.node = create_node("LC", yylineno, 1);
                return LC;    
            }
"}"         {   //printf("RC ");     
                yylval.node = create_node("RC", yylineno, 1);
                return RC;    
            }
"struct"    {   //printf("STRUCT ");    
                yylval.node = create_node("STRUCT", yylineno, 1);     
                return STRUCT;    
            }
"return"    {   //printf("RETURN ");    
                yylval.node = create_node("RETURN", yylineno, 1);  
                return RETURN;    
            }
"if"        {   //printf("IF ");      
                yylval.node = create_node("IF", yylineno, 1);
                return IF;    
            }
"else"      {   //printf("ELSE ");     
                yylval.node = create_node("ELSE", yylineno, 1);   
                return ELSE;    
            }
"while"     {   //printf("WHILE ");      
                yylval.node = create_node("WHILE", yylineno, 1);
                return WHILE;    
            }
{id}        {   // printf("ID : %s", yytext);        
                yylval.node = create_node("ID", yylineno, 1);
                yylval.node->val.id_name = (char *)malloc(sizeof(yytext));
                strcpy(yylval.node->val.id_name, yytext);
                return ID;    
            }
.           {   printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); } 
%%
    /*用户自定义代码部分*/
void multiline_comment() {
    char pre = '\0';
    char cur = input();
    while(!(pre == '*' && cur == '/')) {
        pre = cur; 
        cur = input();
        if(cur == '\0') {
            printf("\nError type A at Line %d: unterminated comment\n", yylineno);
            break;
        }
    }
}