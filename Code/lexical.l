    /*定义部分： 名字 正则定义*/
%option yylineno
%{
    void multiline_comment();    
%}
delim   [ \t\n]
ws      {delim}+
letter  [A-Za-z]
letter_  [A-Za-z_]
digit   [0-9]
id      {letter_}({letter_}|{digit})*
relop   ">"|"<"|">="|"<="|"=="|"!="
type    "int"|"float"
int     {digit}+|0x[0-9a-fA-F]+
    // int包含 十进制数字 (除了0以外都不是0开头), 八进制数字(0开头), 十六进制(0x开头)
float   {digit}+\.{digit}+|{digit}+\.{digit}*[Ee][+-]?{digit}+|{digit}*\.{digit}+[Ee][+-]?{digit}+
    // float分为指数形式和非指数形式; 非指数形式小数点前后都要有数字; 指数形式基数部分小数点前后至少有一个有数字, 指数部分由正负号(可选)和一串数字(0-9)组成
comment \/\/.*
lcomment \/\*
    // 注释分成两种, 单行注释直接正则表达式匹配, 多行匹配前半段, 之后input找到后半段为止
%%
    /*规则部分： 模式 {操作}*/
{ws}        {                           }
{comment}   {   printf("COMMENT ");     }
{lcomment}  {   printf("COMMENT ");  multiline_comment();   }
{int}       {   printf("INT ");         }
{float}     {   printf("FLOAT ");       }
";"         {   printf("SEMI ");        }
","         {   printf("COMMA ");       }
"="         {   printf("ASSIGNOP ");    }
{relop}     {   printf("RELOP ");       }
"+"         {   printf("PLUS ");        }
"-"         {   printf("MINUS ");       }
"*"         {   printf("STAR ");        }
"/"         {   printf("DIV ");         }
"&&"        {   printf("AND ");         }
"||"        {   printf("OR ");          }
"."         {   printf("DOT ");         }
"!"         {   printf("NOT ");         }
{type}      {   printf("TYPE ");        }
"("         {   printf("LP ");          }
")"         {   printf("RP ");          }
"["         {   printf("LB ");          }
"]"         {   printf("RB ");          }
"{"         {   printf("LC ");          }
"}"         {   printf("RC ");          }
"struct"    {   printf("STRUCT ");      }
"return"    {   printf("RETURN ");      }
"if"        {   printf("IF ");          }
"else"      {   printf("ELSE ");        }
"while"     {   printf("WHILE ");       }
{id}        {   printf("ID ");          }
.           {   printf("\nError type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); } 

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