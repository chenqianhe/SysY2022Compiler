parser: sysy.tab.c lex.yy.c ast.c
	gcc sysy.tab.c lex.yy.c ast.c -o parser

sysy.tab.c: sysy.y
	bison -d sysy.y

lex.yy.c: sysy.l
	flex sysy.l

clean:
	rm parser
	rm sysy.tab.c
	rm sysy.tab.h
	rm lex.yy.c