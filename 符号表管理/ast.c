#include "def.h"
int GLOBAL_PARA_NUM = 0;
int IS_SHOW = 1;

struct node * mknode(int kind,struct node *first,struct node *second, struct node *third,int pos ) {
  struct node *T=(struct node *)malloc(sizeof(struct node));
  T->kind=kind;
  T->ptr[0]=first;
  T->ptr[1]=second;
  T->ptr[2]=third;
  T->pos=pos;
  return T;
}

void AddSymbol(struct node *T, char varType[30], int isFunc, int isPara, int paramnum)
{
	int tn = symbolTable.index;
	strcpy(symbolTable.symbols[tn].name, T->type_id);
	if (symbol_scope_TX.top == 0 && isPara == 0)
	{
		symbolTable.symbols[tn].level = 0;
	}
	else
	{
		int t_idx = symbol_scope_TX.TX[symbol_scope_TX.top - 1];
		symbolTable.symbols[tn].level = symbolTable.symbols[t_idx].level + 1;
	}
	strcpy(symbolTable.symbols[tn].type, varType);
	if (isFunc == 1)
	{
		symbolTable.symbols[tn].flag = 'F';
	}
	else if (isPara == 1)
	{
		symbolTable.symbols[tn].flag = 'P';
	}
	else if (symbolTable.symbols[tn].level == 0)
	{
		symbolTable.symbols[tn].flag = 'V';
	}
	else
	{
		symbolTable.symbols[tn].flag = 'V';
	}
	symbolTable.symbols[tn].paramnum = paramnum;
	symbolTable.index++;
}

void display(struct node *T, int indent)  {  //对抽象语法树的先根遍历
  int i = 1;
  struct node *T0;
  if (T) {
	  switch (T->kind) {
	  case EXT_DEF_LIST:
      display(T->ptr[0], indent);  //显示该外部定义列表中的第一个
      display(T->ptr[1], indent);  //显示该外部定义列表中的其它外部定义
      break;
    case EXT_VAR_DEF:
      printf("%*c外部变量定义:\n", indent, ' ');
      display(T->ptr[0], indent+3);  //显示外部变量类型
      printf("%*c定义的变量:\n",indent+3,' ');
      display(T->ptr[1], indent+6);  //显示变量列表
      break;
    case EXT_CONST_VAR_DEF:
      printf("%*c外部常量定义:\n", indent, ' ');
      display(T->ptr[0], indent+3);  //显示外部常量类型
      printf("%*c定义的常量:\n",indent+3,' ');
      display(T->ptr[1], indent+6);  //显示常量列表
      break;
    case TERM:
      if (T->ptr[0]) {
        printf("%*c数组: ", indent, ' ');
      } else {
        printf("%*cID: ", indent, ' ');
      }
      printf("%s\n", T->type_id);
      display(T->ptr[0], indent+3);
      break;
    case ARRAYS:
      T0 = T;
      i = 1;
      while (T0 != NULL) {
        printf("%*c第%d维: \n", indent, ' ', i++);
        display(T0->ptr[0], indent+3);
        T0 = T0->ptr[1];
      }
      break;
	  case TYPE:
	  case VOID:
      printf("%*c类型: %s\n",indent,' ',T->type_id);
      break;
    case EXT_DEC_LIST:
      display(T->ptr[0], indent);  //依次显示外部变量名，
      display(T->ptr[1], indent);  //后续还有相同的，仅显示语法树此处理代码可以和类似代码合并
      break;
    case VAR_DEC:
      printf("%*c变量名: %s\n",indent,' ',T->type_id);
      if (T->ptr[0]) {
        printf("%*c它的初值:\n",indent+3,' ');
        display(T->ptr[0], indent+6);  //表达式初值
      }
      break;
    case FUNC_DEF:
      printf("%*c函数定义：\n",indent,' ');
      display(T->ptr[0],indent+3);      //显示函数返回类型
      display(T->ptr[1],indent+3);      //显示函数名和参数
      display(T->ptr[2],indent+3);      //显示函数体
      break;
	  case FUNC_DEC:
      printf("%*c函数名：%s\n",indent,' ',T->type_id);
      if (T->ptr[0]) {
        printf("%*c函数形参：\n",indent,' ');
        display(T->ptr[0],indent+3);  //显示函数参数列表
        }
      else printf("%*c无参函数\n",indent+3,' ');
      break;
	  case PARAM_LIST:    
      display(T->ptr[0],indent);     //依次显示全部参数类型和名称，
      display(T->ptr[1],indent);
      break;
  	case PARAM_DEC:     
      printf("%*c类型：%s, 参数名：%s\n", indent,' ', T->ptr[0]->type==INT?"int": "float",T->ptr[1]->type_id);
      break;
	  case EXP_STMT:
      printf("%*c表达式语句：\n",indent,' ');
      display(T->ptr[0],indent+3);
      break;
	  case RETURN:
      if (T->ptr[0]) {
        printf("%*c返回语句：\n",indent,' ');
        display(T->ptr[0], indent+3);
      } else {
        printf("%*c空返回语句\n",indent,' ');
      }
      break;
    case CONTINUE_STMT:
      printf("%*c继续语句\n",indent,' ');
      break;
    case BREAK_STMT:
      printf("%*c打断语句\n",indent,' ');
      break;
    case COMP_STM:
      if (T->ptr[0]) {
        printf("%*c复合语句: \n",indent,' ');
        display(T->ptr[0],indent+3);
      } else {
        printf("%*c空复合语句\n",indent,' ');
      }
      break;
	  case STM_DEF_LIST:
      display(T->ptr[0],indent);      //显示第一条语句
      display(T->ptr[1],indent);        //显示剩下语句
      break;
	  case WHILE:
      printf("%*c循环语句：\n",indent,' ');
      printf("%*c循环条件：\n",indent+3,' ');
      display(T->ptr[0],indent+6);      //显示循环条件
      printf("%*c循环体：\n",indent+3,' ');
      display(T->ptr[1],indent+6);      //显示循环体
      break;
    case FOR_STMT:
	    printf("%*cfor 循环语句: \n",indent,' ');
      display(T->ptr[0], indent+3);
      printf("%*c循环体: \n",indent+3,' ');
      display(T->ptr[1], indent+6);
      break;
    case FOR_ARGS:
	    printf("%*cfor 循环起始表达式: \n",indent,' ');
      if (T->ptr[0]) {
        display(T->ptr[0], indent+3);
      } else {
	      printf("%*c无\n",indent+3,' ');
      }
      printf("%*cfor 循环条件表达式: \n",indent,' ');
      if (T->ptr[1]) {
        display(T->ptr[1], indent+3);
      } else {
	      printf("%*c无\n",indent+3,' ');
      }
      printf("%*cfor 循环第三表达式: \n",indent,' ');
      if (T->ptr[2]) {
        display(T->ptr[2], indent+3);
      } else {
	      printf("%*c无\n",indent+3,' ');
      }
      break;
    case IF_THEN:
      printf("%*c条件语句(IF_THEN)：\n", indent, ' ');
      printf("%*c条件：\n", indent + 3, ' ');
      display(T->ptr[0], indent + 6); //显示条件
      printf("%*cIF子句：\n", indent + 3, ' ');
      display(T->ptr[1], indent + 6); //显示if子句
      break;
    case IF_THEN_ELSE:
      printf("%*c条件语句(IF_THEN_ELSE)：\n", indent, ' ');
      printf("%*c条件：\n", indent + 3, ' ');
      display(T->ptr[0], indent + 6); //显示条件
      printf("%*cIF子句：\n", indent + 3, ' ');
      display(T->ptr[1], indent + 6); //显示if子句
      printf("%*cELSE子句：\n", indent + 3, ' ');
      display(T->ptr[2], indent + 6); //显示else子句
      break;
    case VAR_DEF:
      printf("%*c局部变量定义: \n",indent,' ');
      display(T->ptr[0], indent+3);   //显示变量类型
      printf("%*c定义的变量:\n",indent+3,' ');
      display(T->ptr[1], indent+6);   //显示该定义的全部变量名
      break;
    case DEC_LIST:
      display(T->ptr[0], indent);
      display(T->ptr[1], indent);
      break;
    case NODE_ID:
      printf("%*cID： %s\n", indent, ' ', T->type_id);
      break;
    case INT:
      printf("%*cINT：%d\n", indent, ' ', T->type_int);
      break;
    case FLOAT:
      printf("%*cFLAOT：%f\n", indent, ' ', T->type_float);
      break;
    case ASSIGN:
    case AND:
    case OR:
    case RELOP:
    case ADD:
    case MINUS:
    case MUL:
    case DIV:
    case MOD:
      printf("%*c%s\n", indent, ' ', T->type_id);
      display(T->ptr[0], indent + 3);
      display(T->ptr[1], indent + 3);
      break;
    case NOT:
    case UMINUS:
      printf("%*c%s\n", indent, ' ', T->type_id);
      display(T->ptr[0], indent + 3);
      break;
    case FUNC_CALL:
      printf("%*c函数调用：\n", indent, ' ');
      printf("%*c函数名：%s\n", indent + 3, ' ', T->type_id);
      display(T->ptr[0], indent + 3);
      break;
    case ARGS:
      i = 1;
      while (T) { // ARGS表示实际参数表达式序列结点，其第一棵子树为其一个实际参数表达式，第二棵子树为剩下的。
        struct node *T0 = T->ptr[0];
        printf("%*c第%d个实际参数表达式：\n", indent, ' ', i++);
        display(T0, indent + 3);
        T = T->ptr[1];
      }
      //                  printf("%*c第%d个实际参数表达式：\n",indent,' ',i);
      //                  display(T,indent+3);
      // printf("\n");
      break;
    case SELF_ADD_EXP:
      printf("%*c后置自增: \n",indent,' ');
      printf("%*c变量: %s\n", indent+3,' ', T->type_id);
      break;
    case SELF_MINUS_EXP:
      printf("%*c后置自减: \n",indent,' ');
      printf("%*c变量: %s\n", indent+3,' ' , T->type_id);
      break;
    }
  }
}

// 显示当前符号表中的内容,请补充代码
void DisplaySymbolTable()
{
	int i;
	printf("----------------symbol table-----------------------\n");
	printf("%s\t%s\t%s\t%s\t%s\t%s\n",
		   "Index", "Name", "Level", "Type", "Flag", "Param_num");
	printf("---------------------------------------------------\n");
	for (i = 0; i < symbolTable.index; i++)
	{
		struct symbol *symp = &symbolTable.symbols[i];
		if (symp->flag == 'F')
		{
			printf("%d\t%s\t%d\t%s\t%c\t%d\n",
				   i, symp->name, symp->level, symp->type, symp->flag, symp->paramnum);
		}
		else
		{
			printf("%d\t%s\t%d\t%s\t%c\n",
				   i, symp->name, symp->level, symp->type, symp->flag);
		}
	}
	printf("---------------------------------------------------\n");
}

void dfsAst(struct node *T, char varType[33], int isFunc, int isPara, int isDef)
{ //对抽象语法树的先根遍历
	int i = 1;
	struct node *T0;
	char t_str[33];
	if (T)
	{
		switch (T->kind)
		{
		case EXT_DEF_LIST:
			dfsAst(T->ptr[0], varType, 0, 0, 1); //显示该外部定义列表中的第一个
			dfsAst(T->ptr[1], varType, 0, 0, 1); //显示该外部定义列表中的其它外部定义
			break;
		case EXT_VAR_DEF:
			dfsAst(T->ptr[0], varType, 0, 0, 0); //显示外部变量类型
			strcpy(t_str, T->ptr[0]->type == INT ? "int" : "float");
			dfsAst(T->ptr[1], t_str, 0, 0, 1); //显示变量列表
			break;
		case EXT_CONST_VAR_DEF:
			dfsAst(T->ptr[0], varType, 0, 0, 0); //显示外部常量类型
			strcpy(t_str, T->ptr[0]->type_id);
			dfsAst(T->ptr[1], t_str, 0, 0, 1); //显示常量列表
			break;
		case TERM:
			if (isDef)
			{
				// printf("82\n");
				AddSymbol(T, varType, 0, isPara, 0);
				DisplaySymbolTable();
			}
			dfsAst(T->ptr[0], varType, 0, isPara, 0);
			break;
		case ARRAYS:
			T0 = T;
			i = 1;
			while (T0 != NULL)
			{
				dfsAst(T0->ptr[0], varType, 0, isPara, 0);
				T0 = T0->ptr[1];
			}
			break;
		case TYPE:
		case VOID:
			break;
		case EXT_DEC_LIST:
			dfsAst(T->ptr[0], varType, isFunc, isPara, 1); //依次显示外部变量名，
			dfsAst(T->ptr[1], varType, isFunc, isPara, 1); //后续还有相同的，仅显示语法树此处理代码可以和类似代码合并
			break;
		case VAR_DEC:
			if (T->ptr[0])
			{
				dfsAst(T->ptr[0], varType, isFunc, isPara, 0); //表达式初值
			}
			if (isDef)
			{
				AddSymbol(T, varType, isPara, isFunc, 0);
				if (IS_SHOW)
				{
					DisplaySymbolTable();
				}
			}
			break;
		case FUNC_DEF:
			dfsAst(T->ptr[0], varType, isFunc, isPara, 0); //显示函数返回类型
			strcpy(t_str, T->ptr[0]->type == INT ? "int" : "float");
			dfsAst(T->ptr[1], t_str, 1, 0, 1); //显示函数名和参数
			dfsAst(T->ptr[2], t_str, 0, 0, 0); //显示函数体
			break;
		case FUNC_DEC:
			T0 = T->ptr[0];
			int t_paranum = 0;
			strcpy(t_str, "(");
			while (T0 != NULL)
			{
				strcat(t_str, T0->ptr[0]->ptr[0]->type_id);
				strcat(t_str, ",");
				T0 = T0->ptr[1];
				t_paranum++;
			}
			GLOBAL_PARA_NUM = t_paranum;
			if (t_paranum != 0)
			{
				t_str[strlen(t_str) - 1] = ')';
				strcat(varType, t_str);
				AddSymbol(T, varType, 1, 0, t_paranum);
				DisplaySymbolTable();
				dfsAst(T->ptr[0], varType, 0, 1, 1); //显示函数参数列表
			}
			else
			{
				strcat(varType, "(void)");
				AddSymbol(T, varType, 1, 0, t_paranum);
				DisplaySymbolTable();
			}
			break;
		case PARAM_LIST:
			dfsAst(T->ptr[0], varType, 0, 1, 1); //依次显示全部参数类型和名称，
			dfsAst(T->ptr[1], varType, 0, 1, 1);
			break;
		case PARAM_DEC:
			strcpy(t_str, T->ptr[0]->type == INT ? "int" : "float");
			dfsAst(T->ptr[1], t_str, 0, 1, 1);
			break;
		case EXP_STMT:
			dfsAst(T->ptr[0], varType, 0, 0, 0);
			break;
		case RETURN:
			if (T->ptr[0]){
				dfsAst(T->ptr[0], varType, 0, 0, 0);
			}else{
			}
			break;
		case CONTINUE_STMT:
			break;
		case BREAK_STMT:
			break;
		case COMP_STM:
			if (T->ptr[0]){
				symbol_scope_TX.TX[symbol_scope_TX.top] = symbolTable.index - 1 - GLOBAL_PARA_NUM;
				symbol_scope_TX.top++;
				GLOBAL_PARA_NUM = 0;
				dfsAst(T->ptr[0], varType, 0, 0, 0);
				symbol_scope_TX.top--;
				if ((symbolTable.index - 1) != symbol_scope_TX.TX[symbol_scope_TX.top])
				{
					symbolTable.index = symbol_scope_TX.TX[symbol_scope_TX.top] + 1;
					DisplaySymbolTable();
				}
			}else{
				// printf("%*c空复合语句\n", indent, ' ');
			}
			break;
		case STM_DEF_LIST:
			dfsAst(T->ptr[0], varType, 0, 0, 0); //显示第一条语句
			dfsAst(T->ptr[1], varType, 0, 0, 0); //显示剩下语句
			break;
		case WHILE:
			dfsAst(T->ptr[0], varType, 0, 0, 0); //显示循环条件
			dfsAst(T->ptr[1], varType, 0, 0, 0); //显示循环体
			break;
		case FOR_STMT:
			dfsAst(T->ptr[0], varType, 0, 0, 0);
			dfsAst(T->ptr[1], varType, 0, 0, 0);
			break;
		case FOR_ARGS:
			if (T->ptr[0])
			{
				dfsAst(T->ptr[0], varType, 0, 0, 0);
			}else{
			}
			if (T->ptr[1]){
				dfsAst(T->ptr[1], varType, 0, 0, 0);
			}else{
			}
			if (T->ptr[2])
			{
				dfsAst(T->ptr[2], varType, 0, 0, 0);
			}else{
			}
			break;
		case IF_THEN:
			dfsAst(T->ptr[0], varType, 0, 0, 0); //显示条件
			dfsAst(T->ptr[1], varType, 0, 0, 0); //显示if子句
			break;
		case IF_THEN_ELSE:
			dfsAst(T->ptr[0], varType, 0, 0, 0); //显示条件
			dfsAst(T->ptr[1], varType, 0, 0, 0); //显示if子句
			dfsAst(T->ptr[2], varType, 0, 0, 0); //显示else子句
			break;
		case VAR_DEF:
			strcpy(t_str, T->ptr[0]->type == INT ? "int" : "float");
			dfsAst(T->ptr[0], varType, 0, 0, 0); //显示变量类型
			dfsAst(T->ptr[1], t_str, 0, 0, 0); //显示该定义的全部变量名
			break;
		case DEC_LIST:
			if(T->ptr[1]!=NULL){
				IS_SHOW=0;
			}else{
				IS_SHOW=1;
			}
			dfsAst(T->ptr[0], varType, isFunc, isPara, 1);
			dfsAst(T->ptr[1], varType, isFunc, isPara, 1);
			break;
		case NODE_ID:
			break;
		case INT:
			break;
		case FLOAT:
			break;
		case ASSIGN:
		case AND:
		case OR:
		case RELOP:
		case ADD:
		case MINUS:
		case MUL:
		case DIV:
		case MOD:
			dfsAst(T->ptr[0], varType, 0, 0, 0);
			dfsAst(T->ptr[1], varType, 0, 0, 0);
			break;
		case NOT:
		case UMINUS:
			dfsAst(T->ptr[0], varType, 0, 0, 0);
			break;
		case FUNC_CALL:
			dfsAst(T->ptr[0], varType, 0, 0, 0);
			break;
		case ARGS:
			i = 1;
			while (T)
			{ // ARGS表示实际参数表达式序列结点，其第一棵子树为其一个实际参数表达式，第二棵子树为剩下的。
				struct node *T0 = T->ptr[0];
				dfsAst(T0, varType, 0, 0, 0);
				T = T->ptr[1];
			}
			break;
		case SELF_ADD_EXP:
			break;
		case SELF_MINUS_EXP:
			break;
		}
	}
}