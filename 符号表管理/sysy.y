%define parse.error verbose
%locations
%{
#include "stdio.h"
#include "math.h"
#include "string.h"
#include "def.h"
extern int yylineno;
extern char *yytext;
extern FILE *yyin;
extern void yyerror(const char* fmt, ...);
extern void display(struct node*,int);
extern int yylex();
extern char filename[50];

%}

%union {
  int    type_int;
  float  type_float;
  char   type_id[32];
  struct node *ptr;
};

//  %type 定义非终结符的语义值类型
%type  <ptr> program ExtDefList ExtDef Specifier VoidType ExtDecList FuncDec CompSt VarList VarDec ParamDec Stmt StmDefList Def DecList Exp Args ForArgs ForArg Term Arrays

//% token 定义终结符的语义值类型
%token <type_int> INT           // 指定INT字面量的语义值是type_int，有词法分析得到的数值
%token <type_id> ID RELOP TYPE VOID   // 指定ID,RELOP的语义值是type_id，有词法分析得到的标识符字符串, RELOP relation operator
%token <type_float> FLOAT       // 指定FLOAT字面量的语义值是type_float，有词法分析得到的数值

%token CONST RETURN IF ELSE FOR WHILE DO BREAK CONTINUE
%token LP RP LB RB LC RC COMMA SEMICOLON QUESTION COLON
//用bison对该文件编译时，带参数-d，生成的exp.tab.h中给这些单词进行编码，可在lex.l中包含parser.tab.h使用这些单词种类码
%token NOT ASSIGN MINUS ADD MUL DIV MOD AND OR UMINUS SELF_ADD SELF_MINUS

%left ASSIGN
%left OR AND
%left RELOP
%left ADD MINUS MOD
%left MUL DIV
%right NOT

%nonassoc LOWER_THEN_ELSE
%nonassoc ELSE

%%

/* 不再显示语法树；请添加适当的功能，遍历语法树，登记符号表，并在进入和退出作用域时，显示符号表的内容 */
program: ExtDefList  {/*printf("CompUnit:\n");*/ dfsAst($1, 0, 0, 0, 0); }
         ;

ExtDefList: {$$=NULL;}
          | ExtDef ExtDefList  {$$=mknode(EXT_DEF_LIST,$1,$2,NULL,yylineno);}  //每一个EXTDEFLIST的结点，其第1棵子树对应一个外部变量声明或函数
          ;

ExtDef: Specifier ExtDecList SEMICOLON  {$$=mknode(EXT_VAR_DEF,$1,$2,NULL,yylineno);}  //该结点对应一个外部变量声明
      | CONST Specifier ExtDecList SEMICOLON  {$$=mknode(EXT_CONST_VAR_DEF,$2,$3,NULL,yylineno);}  //该结点对应一个const外部变量声明
      | Specifier FuncDec CompSt  {$$=mknode(FUNC_DEF,$1,$2,$3,yylineno);}  //该结点对应一个函数定义
      | VoidType FuncDec CompSt  {$$=mknode(FUNC_DEF,$1,$2,$3,yylineno);}  //该结点对应一个函数定义
      | error SEMICOLON  {$$=NULL; }
      ;

Specifier: TYPE  {$$=mknode(TYPE,NULL,NULL,NULL,yylineno);strcpy($$->type_id,$1);$$->type=!strcmp($1,"int")?INT:FLOAT;}   
         ; 

VoidType: VOID {$$=mknode(TYPE,NULL,NULL,NULL,yylineno);strcpy($$->type_id,$1);$$->type=VOID;}  

ExtDecList: VarDec  {$$=$1;}  //每一个EXT_DECLIST的结点，其第一棵子树对应一个变量名(ID类型的结点),第二棵子树对应剩下的外部变量名
          | VarDec COMMA ExtDecList  {$$=mknode(EXT_DEC_LIST,$1,$3,NULL,yylineno);}
          ;

Term: ID Arrays {$$=mknode(TERM,$2,NULL,NULL,yylineno);strcpy($$->type_id,$1);}
    ;

Arrays: {$$=NULL;}
      | LB Exp RB Arrays {$$=mknode(ARRAYS,$2,$4,NULL,yylineno);}
      ;

VarDec: Term {$$=$1;}
      | ID ASSIGN Exp  {$$=mknode(VAR_DEC,$3,NULL,NULL,yylineno);strcpy($$->type_id,$1);}
      ;

FuncDec: ID LP VarList RP  {$$=mknode(FUNC_DEC,$3,NULL,NULL,yylineno);strcpy($$->type_id,$1);}  //函数名存放在$$->type_id
       | ID LP RP  {$$=mknode(FUNC_DEC,NULL,NULL,NULL,yylineno);strcpy($$->type_id,$1);}  //函数名存放在$$->type_id
       ;  
VarList: ParamDec  {$$=mknode(PARAM_LIST,$1,NULL,NULL,yylineno);}
       | ParamDec COMMA VarList  {$$=mknode(PARAM_LIST,$1,$3,NULL,yylineno);}
       ;
ParamDec: Specifier VarDec  {$$=mknode(PARAM_DEC,$1,$2,NULL,yylineno);}
        ;

CompSt: LC StmDefList RC  {$$=mknode(COMP_STM,$2,NULL,NULL,yylineno);}
      ;
StmDefList: {$$=NULL; }  
          | Stmt StmDefList  {$$=mknode(STM_DEF_LIST,$1,$2,NULL,yylineno);}
          | Def StmDefList  {$$=mknode(STM_DEF_LIST,$1,$2,NULL,yylineno);}
          ;
Stmt: Exp SEMICOLON          {$$=mknode(EXP_STMT,$1,NULL,NULL,yylineno);}
    | CompSt                 {$$=$1;}      //复合语句结点直接最为语句结点，不再生成新的结点
    | RETURN Exp SEMICOLON   {$$=mknode(RETURN,$2,NULL,NULL,yylineno);}
    | RETURN SEMICOLON       {$$=mknode(RETURN,NULL,NULL,NULL,yylineno);}
    | IF LP Exp RP Stmt %prec LOWER_THEN_ELSE   {$$=mknode(IF_THEN,$3,$5,NULL,yylineno);}
    | IF LP Exp RP Stmt ELSE Stmt   {$$=mknode(IF_THEN_ELSE,$3,$5,$7,yylineno);}
    | WHILE LP Exp RP Stmt   {$$=mknode(WHILE,$3,$5,NULL,yylineno);}
    | CONTINUE SEMICOLON     {$$=mknode(CONTINUE_STMT,NULL,NULL,NULL,yylineno);}
    | BREAK SEMICOLON        {$$=mknode(BREAK_STMT,NULL,NULL,NULL,yylineno);}
    | FOR ForArgs Stmt       {$$=mknode(FOR_STMT,$2,$3,NULL,yylineno);}
    ;

ForArgs: LP ForArg SEMICOLON ForArg SEMICOLON ForArg RP  {$$=mknode(FOR_ARGS,$2,$4,$6,yylineno);}
       ;

ForArg: {$$=NULL;}  
      | Exp  {$$=$1;}
      ;

Def: Specifier DecList SEMICOLON {$$=mknode(VAR_DEF,$1,$2,NULL,yylineno);}
   ;
DecList: VarDec  {$$=mknode(DEC_LIST,$1,NULL,NULL,yylineno);}
       | VarDec COMMA DecList  {$$=mknode(DEC_LIST,$1,$3,NULL,yylineno);}
       ;

Exp: Exp ASSIGN Exp  {$$=mknode(ASSIGN,$1,$3,NULL,yylineno);strcpy($$->type_id,"ASSIGN");}//$$结点type_id空置未用，正好存放运算符
   | Exp AND Exp     {$$=mknode(AND,$1,$3,NULL,yylineno);strcpy($$->type_id,"AND");}
   | Exp OR Exp      {$$=mknode(OR,$1,$3,NULL,yylineno);strcpy($$->type_id,"OR");}
   | Exp RELOP Exp   {$$=mknode(RELOP,$1,$3,NULL,yylineno);strcpy($$->type_id,$2);}  //词法分析关系运算符号自身值保存在$2中
   | Exp ADD Exp     {$$=mknode(ADD,$1,$3,NULL,yylineno);strcpy($$->type_id,"ADD");}
   | Exp MINUS Exp   {$$=mknode(MINUS,$1,$3,NULL,yylineno);strcpy($$->type_id,"MINUS");}
   | Exp MUL Exp     {$$=mknode(MUL,$1,$3,NULL,yylineno);strcpy($$->type_id,"MUL");}
   | Exp DIV Exp     {$$=mknode(DIV,$1,$3,NULL,yylineno);strcpy($$->type_id,"DIV");}
   | Exp MOD Exp     {$$=mknode(MOD,$1,$3,NULL,yylineno);strcpy($$->type_id,"MOD");}
   | Term SELF_ADD   {$$=mknode(SELF_ADD_EXP,$1,NULL,NULL,yylineno);}
   | Term SELF_MINUS {$$=mknode(SELF_MINUS_EXP,$1,NULL,NULL,yylineno);}
   | LP Exp RP       {$$=$2;}
   | MINUS Exp %prec UMINUS   {$$=mknode(UMINUS,$2,NULL,NULL,yylineno);strcpy($$->type_id,"UMINUS");}
   | NOT Exp         {$$=mknode(NOT,$2,NULL,NULL,yylineno);strcpy($$->type_id,"NOT");}
   | ID LP Args RP   {$$=mknode(FUNC_CALL,$3,NULL,NULL,yylineno);strcpy($$->type_id,$1);}
   | ID LP RP        {$$=mknode(FUNC_CALL,NULL,NULL,NULL,yylineno);strcpy($$->type_id,$1);}
   | Term            {$$=$1;}
   | INT             {$$=mknode(INT,NULL,NULL,NULL,yylineno);$$->type_int=$1;$$->type=INT;}
   | FLOAT           {$$=mknode(FLOAT,NULL,NULL,NULL,yylineno);$$->type_float=$1;$$->type=FLOAT;}
   ;

Args: Exp COMMA Args  {$$=mknode(ARGS,$1,$3,NULL,yylineno);}
    | Exp             {$$=mknode(ARGS,$1,NULL,NULL,yylineno);}
    ;
       
%%

int main(int argc, char *argv[]) {
  yyin = fopen(argv[1],"r");
  if (!yyin) return 0;
  yylineno = 1;
  strcpy(filename,strrchr(argv[1],'/')+1);
  yyparse();
  return 0;
}

#include<stdarg.h>

void yyerror(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "%s:%d:%d ", filename,yylloc.first_line,yylloc.first_column);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, ".\n");
}	