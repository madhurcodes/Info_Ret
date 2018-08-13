search: query_functions.cpp invidx_cons.cpp query.l query.y
	bison -dv query.y
	g++ -g -c query.tab.c -o query.tab.o
	flex  query.l
	g++ -g -c lex.yy.c -o lex.yy.o
	g++ -g lex.yy.o query.tab.o -o boolsearch
	g++ -g invidx_cons.cpp -o invidx

clean:
	rm query.tab.c query.tab.h lex.yy.c query.tab.o lex.yy.o my_pipe.txt
	rm boolsearch invidx
	rm vecsearch