%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include "bignumcalc_prog.h"
	int MAX_LEN;
	int SynErr;
%}

%union {
	int int_val;
	float float_val;
	char* stri;
	bigint* big_i;
}

%type<big_i> unit
%type<big_i> mult_div expression;
%token<stri> NUMBER
%token AND OR COMMA
%token OP CP
%token EOL

%%

calcexpression:
	|	calcexpression expression EOL {
                            reduce(MAX_LEN/2,$2);
							print_bigint($2);
							del_big($2);
						} ;

expression: mult_div
	|	expression ADD mult_div {
			$$ = add($1,$3,1);
			del_big($1);
			del_big($3);
		}
	|	expression SUB mult_div {
			$$ = sub($1,$3,1);
			del_big($1);
			del_big($3);
		}
	;

mult_div: unit
	|	mult_div MUL unit {
			$$ = mult($1,$3,1);
			del_big($1);
			del_big($3);
		}
	|	mult_div DIV unit {
			$$ = div_big($1,$3);
			del_big($1);
			del_big($3);
		}
	;

unit: NUMBER {
			$$ = conv_str_to_bigint(0,$1);
			free($1);
		}

	| SUB NUMBER {
			$$ = conv_str_to_bigint(1,$2);
			free($2);
		}
	| OP expression CP {$$ = $2;}
	| SQRT unit {
			$$ = big_sqrt($2);
			del_big($2);
		}
	| LOG unit {
			$$ = big_log($2);
			del_big($2);
		}
	| POW OP unit COMMA unit CP {
			$$ = power($3,$5);
			del_big($3);
			del_big($5);
		}
	;

%%

main(int argc, char* argv[])
{
	freopen(argv[1],"r",stdin);
	freopen(argv[2],"w",stdout);
	scanf("%d ",&MAX_LEN);
	MAX_LEN = 2*MAX_LEN;
	yyparse();
}

yyerror(char *s)
{
	SynErr = 1;
	throw_err();
}