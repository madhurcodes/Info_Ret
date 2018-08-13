%{
	#include "query_functions.cpp"
	int MAX_LEN;
	FILE *q_out;
	memory_data* dat;
	query_config* config;
	extern FILE* yyin;
	extern int yylex ();
	extern void yyerror ( char *);
%}

%code requires {
    #include "post.h"
}

%union {
	char* query;
	posting_query* query_list;
}

%type<query_list> unit
%type<query_list> expression;
%token<query> TOKEN
%token END_OF_FILE
%token OP CP
%token EOL

%left AND OR
%right NAMED

%%

full:
	| fullexpression full {}
	| END_OF_FILE {
		cout<<"DONE"<<endl;
		};

fullexpression: expression EOL {
				write_output($1, dat, config, q_out);
				fputs("\n",q_out);
				fputs("\n",q_out);
				cout<<"--"<<endl;				
				delete($1);
			};
expression: unit {
			$$ = $1;
			cout<<"-I-"<<endl;
		}
	|	expression AND expression {
			$$ = post_intersection($1,$3);
			cout<<"-AD-"<<endl;
			delete($1);
			delete($3);
		}
	|	expression OR expression {
			$$ = post_union($1,$3);
			cout<<"-OR-"<<endl;
			delete($1);
			delete($3);
		}
	|	OP expression CP {$$ = $2;
						cout<<"-P-"<<endl;}
	;


unit: TOKEN {
			$$ = token_to_posting_list($1, config, dat, 0);
			cout<<"-T-"<<endl;
			free($1);
		}

	| NAMED TOKEN {
			$$ = token_to_posting_list($2, config, dat, 1);
			cout<<"-N-"<<endl;
			free($2);
		}
	;

%%

int main(int argc, char* argv[])
{
	config = process_query_args(argc, argv);
	FILE *q_in = fopen(config->query_file.c_str(), "r");
	q_out = fopen(config->out_file.c_str(), "w");
	yyin = q_in;

	dat = load_mem_data(config);

	yyparse();
	fclose(q_in);
	fclose(q_out);
	cout<<"\nClosed files"<<endl;
	return 0;

}
