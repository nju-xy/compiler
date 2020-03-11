%option yylineno
%{
    #include <stdio.h>
    void multiline_comment();
%}
delim   [ \t\n\r]
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
                yylval.type_int = ret;
                return INT;
            }
{wrong_hex} {   printf("Error type A at Line %d: Wrong int(hex) \'%s\'\n", yylineno, yytext); }
{hex}       {   int ret;    
                sscanf(yytext, "%x", &ret);
                // printf("INT(HEX):%d ", ret);  
                yylval.type_int = ret; 
                return INT;
            }
{wrong_oct} {   printf("Error type A at Line %d: Wrong int(oct) \'%s\'\n", yylineno, yytext); }
{oct}       {   int ret;    
                sscanf(yytext, "%o", &ret);
                // printf("INT(OCT):%d ", ret);   
                yylval.type_int = ret;  
                return INT;
            }
{wrong_float} { printf("Error type A at Line %d: Wrong float \'%s\'\n", yylineno, yytext); }
{float}     {   float ret;    
                sscanf(yytext, "%f", &ret);
                //printf("FLOAT:%f ", ret);    
                yylval.type_float = ret;
                return FLOAT;
            }
";"         {   //printf("SEMI ");
                return SEMI;
            }
","         {   //printf("COMMA ");
                return COMMA;    
            }
"="         {   //printf("ASSIGNOP "); 
                return ASSIGNOP;    
            }
"<"         {   //printf("RELOP ");
                yylval.type_relop = LT;
                return RELOP;    
            }
">"         {   //printf("RELOP ");
                yylval.type_relop = GT;
                return RELOP;    
            }
"<="        {   //printf("RELOP ");
                yylval.type_relop = LE;
                return RELOP;    
            }
">="        {   //printf("RELOP "); 
                yylval.type_relop = GE;
                return RELOP;    
            }
"=="        {   //printf("RELOP ");
                yylval.type_relop = EQ;
                return RELOP;    
            }
"!="        {   //printf("RELOP ");
                yylval.type_relop = NE;
                return RELOP;    
            }
"+"         {   //printf("PLUS ");
                return PLUS;    
            }
"-"         {   //printf("MINUS ");
                return MINUS;    
            }
"*"         {   //printf("STAR ");               
                return STAR;    
            }
"/"         {   //printf("DIV ");       
                return DIV;
            }
"&&"        {   //printf("AND ");       
                return AND;    
            }
"||"        {   //printf("OR ");       
                return OR;    
            }
"."         {   //printf("DOT ");       
                return DOT;    
            }
"!"         {   //printf("NOT ");       
                return NOT;    
            }
"int"       {   //printf("TYPE ");  
                yylval.type_type = TYPE_INT;      
                return TYPE;    
            }
"float"     {   //printf("TYPE ");   
                yylval.type_type = TYPE_FLOAT;     
                return TYPE;    
            }
"("         {   //printf("LP ");       
                return LP;    
            }
")"         {   //printf("RP ");       
                return RP;    
            }
"["         {   //printf("LB ");       
                return LB;    
            }
"]"         {   //printf("RB ");       
                return RB;    
            }
"{"         {   //printf("LC ");       
                return LC;    
            }
"}"         {   //printf("RC ");       
                return RC;    
            }
"struct"    {   //printf("STRUCT ");       
                return STRUCT;    
            }
"return"    {   //printf("RETURN ");       
                return RETURN;    
            }
"if"        {   //printf("IF ");       
                return IF;    
            }
"else"      {   //printf("ELSE ");       
                return ELSE;    
            }
"while"     {   //printf("WHILE ");       
                return WHILE;    
            }
{id}        {   //printf("ID ");       
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