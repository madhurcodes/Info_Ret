%{
	#include "bignumcalc_prog.h"
	#include "bignumcalc.tab.h"
	#include <string.h>
	#include <stdlib.h>

	int SynErr;

%}

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
"," {
		return COMMA;
	}
[0-9]+(\.[0-9]+)? {
	char* numb = strdup(yytext);	
	yylval.stri = numb;
	return NUMBER;
	}

[ \t\r]+ {}
\n 	{
		return EOL;
	}
.	{
		SynErr = 1;
		throw_err();
	}

%%;