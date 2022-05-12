%option noyywrap
%option yylineno

%{
#include <stdio.h>
enum Token {
    INT=258, FLOAT, VOID, CONST, RETURN, IF, ELSE, FOR, WHILE, DO, BREAK, CONTINUE,
    LP, RP, LB, RB, LC, RC, COMMA, SEMICOLON, QUESTION, COLON,
    MINUS, NOT, TILDE, ASSIGN, ADD, MUL, DIV, MOD, AND, OR,
    EQ, NE, LT, LE, GT, GE,
    ID, INT_LIT, FLOAT_LIT,
    LEX_ERR
};

%}
d [0-9]
c [A-Z_a-z]
w [\t\n ]

hex (\+|\-)?0(X|x)[0-9a-fA-F]+
oct (\+|\-)?0[0-7]+
dec (\+|\-)?[1-9]+{d}*|0

%%
"//".* {}
"/*"([^\*]|(\*)*[^\*/])*(\*)*"*/"  {}
{w}+ {}

"int" {printf("%s : INT\n", yytext); return INT; }
"float" {printf("%s : FLOAT\n", yytext); return FLOAT; }
"void" {printf("%s : VOID\n", yytext); return VOID; }
"const" {printf("%s : CONST\n", yytext); return CONST; }
"return" {printf("%s : RETURN\n", yytext); ; return RETURN; }
"if" {printf("%s : IF\n", yytext); return IF; }
"else" {printf("%s : ELSE\n", yytext); return ELSE; }
"for" {printf("%s : FOR\n", yytext); return FOR; }
"while" {printf("%s : WHILE\n", yytext); return WHILE; }
"do" {printf("%s : DO\n", yytext); return DO; }
"break" {printf("%s : BREAK\n", yytext); return BREAK; }
"continue" {printf("%s : CONTINUE\n", yytext); return CONTINUE; }

"(" {printf("%s : LP\n", yytext); return LP; }
")" {printf("%s : RP\n", yytext); return RP; }
"{" {printf("%s : LC\n", yytext); return LC; }
"}" {printf("%s : RC\n", yytext); return RC; }
"[" {printf("%s : LB\n", yytext); }
"]" {printf("%s : RB\n", yytext); }
"," {printf("%s : COMMA\n", yytext); }
";" {printf("%s : SEMICOLON\n", yytext); }
"?" {printf("%s : QUESTION\n", yytext); }
":" {printf("%s : COLON\n", yytext); }
"!" {printf("%s : NOT\n", yytext); }
"=" {printf("%s : ASSIGN\n", yytext); }
"-" {printf("%s : MINUS\n", yytext); }
"+" {printf("%s : ADD\n", yytext); }
"*" {printf("%s : MUL\n", yytext); }
"/" {printf("%s : DIV\n", yytext); }
"%" {printf("%s : MOD\n", yytext); }
"&&" {printf("%s : AND\n", yytext); }
"||" {printf("%s : OR\n", yytext); }
"==" {printf("%s : EQ\n", yytext); }
"!=" {printf("%s : NE\n", yytext); }
"<" {printf("%s : LT\n", yytext); }
"<=" {printf("%s : LE\n", yytext); }
">" {printf("%s : GT\n", yytext); }
">=" {printf("%s : GE\n", yytext); }


{c}+{d}*{c}*|"_"{c}*{d}*{c}* {printf("%s : ID\n", yytext); return ID; }
0[8-9]+{d}* {printf("Lexical error - line %d : %s\n", yylineno, yytext); }
{hex}|{oct}|{dec} {printf("%s : INT_LIT\n", yytext); return INT_LIT; }
{d}*f {printf("Lexical error - line %d : %s\n", yylineno, yytext); }
({d}+"."{d}*|"."{d}+|{d}+e"-"{d}+)f? {printf("%s : FLOAT_LIT\n", yytext); }

{d}+{c}+{d}*{c}* {printf("Lexical error - line %d : %s\n", yylineno, yytext); }

. {}
%%


int main(int argc, char** argv) {
    if(argc > 1) {
        if(!(yyin = fopen(argv[1], "r"))) {
           perror(argv[1]); 
           return  1; 
        }       


        while(yylex());



        fclose(yyin);
    } else printf("No source file given!\n"); 

    return 0; 
}

