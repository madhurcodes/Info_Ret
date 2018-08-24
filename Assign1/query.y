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
%type<query_list> mid_unit
%type<query_list> expression;
%token<query> TOKEN
%token OP CP
%token EOL

%left AND OR
%left AST
%right NAMED

%%

full: 
	| fullexpression  full {};

fullexpression:  expression EOL{
				// write_output($1, dat, config, q_out);
				write_output_and_scores($1, dat, config, q_out);				
				fputs("\n",q_out);
				fputs("\n",q_out);
				// cout<<"--"<<endl;				
				delete($1);
			};
expression: mid_unit {
			$$ = $1;
			// cout<<"-I-"<<endl;
		}
	|	expression AND expression {
			$$ = post_intersection($1,$3);
			// cout<<"-AD-"<<endl;
			delete($1);
			delete($3);
		}
	|	expression OR expression {
			$$ = post_union($1,$3);
			// cout<<"-OR-"<<endl;
			delete($1);
			delete($3);
		}
	|	OP expression CP {$$ = $2;
						// cout<<"-P-"<<endl;
						}
	;

mid_unit: unit {$$ = $1;}
	| mid_unit unit {
			$$ = post_intersection($1,$2);
			// cout<<"-ORR-"<<endl;
			delete($1);
			delete($2);
	};

unit: TOKEN {
			$$ = token_to_posting_list($1, config, dat, 0);
			// cout<<"-T-"<<endl;
			free($1);
		}

	| NAMED TOKEN {
			$$ = token_to_posting_list($2, config, dat, 1);
			// cout<<"-N-"<<endl;
			free($2);
		}
	| TOKEN AST {
			$$ = prefix_token_to_posting_list($1, config, dat, 0);
			// cout<<"-T-"<<endl;
			free($1);
		}
	| NAMED TOKEN AST {
			$$ = prefix_token_to_posting_list($2, config, dat, 1);
			// cout<<"-N-"<<endl;
			free($2);
		};

%%

int main(int argc, char* argv[])
{
	config = process_query_args(argc, argv);
	// If query file does not have endline at EOF
	FILE *mod_q = fopen(config->query_file.c_str(), "r+");
	fseek(mod_q, -1, SEEK_END);
	if (fgetc(mod_q) != '\n'){
		fseek(mod_q, 0, SEEK_END);
		fputc('\n',mod_q);				
	}
	fclose(mod_q);

	FILE *q_in = fopen(config->query_file.c_str(), "r");
	q_out = fopen(config->out_file.c_str(), "w");
	yyin = q_in;

	dat = load_mem_data(config);

	yyparse();
	fclose(q_in);
	fclose(q_out);
	// cout<<"\nClosed files"<<endl;
	return 0;
}
