%option noyywrap
%{
#include "sysy.tab.h"
#include "string.h"
#include "def.h"
int yycolumn=1;
#define YY_USER_ACTION    	yylloc.first_line=yylloc.last_line=yylineno; \
	yylloc.first_column=yycolumn;	yylloc.last_column=yycolumn+yyleng-1; yycolumn+=yyleng;
typedef union {
	int type_int;
	int type_float;
	char type_id[32];
	struct node *ptr;
} YYLVAL;
#define YYSTYPE YYLVAL

%}
%option yylineno

id   [A-Za-z_][A-Za-z0-9_]*  
int    [0-9]+
float  ([0-9]*\.[0-9]+)|([0-9]+\.)

%%
"//".* {}
"/*"([^\*]|(\*)*[^\*/])*(\*)*"*/"  {}

{int}        {yylval.type_int=atoi(yytext); return INT;}
{float}      {yylval.type_float=atof(yytext); return FLOAT;}
"void"  {strcpy(yylval.type_id,  yytext);return VOID;}
"int"        {strcpy(yylval.type_id,  yytext);return TYPE;}
"float"      {strcpy(yylval.type_id,  yytext);return TYPE;}
"continue"   {return CONTINUE;}
"break"   {return BREAK;}
"return"     {return RETURN;}
"if"         {return IF;}
"else"       {return ELSE;}
"for"       {return FOR;}
"while"      {return WHILE;}

{id}       {strcpy(yylval.type_id,  yytext); return ID;/*由于关键字的形式也符合表示符的规则，所以把关键字的处理全部放在标识符的前面，优先识别*/}
";"			{return SEMI;}
","			{return COMMA;}
">"|"<"|">="|"<="|"=="|"!=" {strcpy(yylval.type_id, yytext);;return RELOP;}
"="			{return ASSIGNOP;}
"+"			{return PLUS;}
"++"                    {return SELFPLUS;}
"-"			{return MINUS;}
"*"			{return STAR;}
"/"			{return DIV;}
"%"                     {return MOD;}
"&&"         {return AND;}
"||"            {return OR;}
"!"			{return NOT;}
"[" {printf("%s : LB\n", yytext); return LB;}
"]" {printf("%s : RB\n", yytext); return RB;}
"("			{return LP;}
")"			{return RP;}
"{"			{return LC;}
"}"			{return RC;}
[\n]           {yycolumn=1;}   
[ \r\t]          {}   
. {printf("Error type A :Mysterious character \"%s\"\n\t at Line %d\n",yytext,yylineno);}
%%
