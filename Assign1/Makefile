search: query_functions.cpp invidx_cons.cpp query.l query.y
	bison -d query.y
	bison -d vec_query.y
	g++ -g -c query.tab.c -o query.tab.o
	g++ -g -c vec_query.tab.c -o vec_query.tab.o
	flex  query.l
	g++ -g -c lex.yy.c -o lex.yy.o
	g++ -g lex.yy.o query.tab.o -o boolsearch
	g++ -g lex.yy.o vec_query.tab.o -o vecsearch
	g++ -g invidx_cons.cpp -o invidx
clean:
	rm -f vec_query.tab.o vec_query.tab.c temp_query.txt vecsearch *.idx *.dict query.tab.c query.tab.h lex.yy.c query.tab.o lex.yy.o my_pipe.txt boolsearch invidx
