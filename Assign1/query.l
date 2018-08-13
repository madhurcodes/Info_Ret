%{
	int SynErr;
	#include "post.h"
	#include "query.tab.h"
	void yyerror ( char *);
	int yyparse ( void );
%}

%option noyywrap

%%
"(" {
		return OP;
	}

")" {
		return CP;
	}

"&" {
		return AND;
	}
"|" {
		return OR;
	}
"N:" {
	return NAMED;
	}
"*" {
	return AST;
	}
[a-zA-Z0-9]+ {
	char* tok = strdup(yytext);	
	yylval.query = tok;
	return TOKEN;
	}

[ \t\r]+ {}
\n 	{
		// cout<<"EOL Tokenized"<<endl;
		return EOL;
	}

.	{
		SynErr = 1;
		printf("Lex error");
		exit(-1);
	}


%%


void yyerror(char* str){
	fprintf(yyout, "Syn Err here - %s\n",str);
}