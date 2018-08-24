%{
	#include "query_functions.cpp"
	int MAX_LEN;
	FILE *q_out;
	memory_data* dat;
	query_config* config;
	extern FILE* yyin;
	extern int yylex ();
	extern void yyerror ( char *);
	extern vector<pair<int,char*> > vector_search_sol;
%}

%code requires {
    #include "post.h"
}

%union {
	char* query;
	vector<pair<int,char*> > *query_list;    
	pair<int,char*> *termy;
}


%type<termy> unit
%type<query_list> mid_unit
%token<query> TOKEN
%token OP CP
%token EOL

%left AND OR
%left AST
%right NAMED

%%

full: 
	| fullexpression  full {};

fullexpression:  mid_unit EOL{
				// write_output($1, dat, config, q_out);
				vec_write_output_and_scores(dat, config, q_out);				
				fputs("\n",q_out);
				fputs("\n",q_out);
			};

mid_unit: unit {
				vector<pair<int,char*> > temp_vec = vector<pair<int,char*> >();
				$$ = &temp_vec;
				// (*$$).push_back((*$1));
				}
	| mid_unit unit {
            // wrong, change
			// (*$1).push_back((*$2));
			// (*$$) = (*$1);
	};

unit: TOKEN {
			// $$ = token_to_posting_list($1, config, dat, 0);
			pair<int,char*> temp_pair = pair<int,char*>(1,$1);
			vector_search_sol.push_back(temp_pair);
			// $$ = &temp_pair;
		}

	| NAMED TOKEN {
			pair<int,char*> temp_pair = pair<int,char*>(2,$2);
			// $$ = &temp_pair;
			vector_search_sol.push_back(temp_pair);			
			// $$ = token_to_posting_list($2, config, dat, 1);
		}
	| TOKEN AST {
			pair<int,char*> temp_pair = pair<int,char*>(3,$1);
			// $$ = &temp_pair;
			vector_search_sol.push_back(temp_pair);			
			// $$ = prefix_token_to_posting_list($1, config, dat, 0);
		}
	| NAMED TOKEN AST {
			pair<int,char*> temp_pair = pair<int,char*>(4,$2);
			// $$ = &temp_pair;
			vector_search_sol.push_back(temp_pair);			
			// $$ = prefix_token_to_posting_list($2, config, dat, 1);
		};

%%

int main(int argc, char* argv[])
{
	config = process_query_args(argc, argv);
    string command;
    stringstream buffer;
    // Note : make temporary query file so I can edit it before yacc parsing
    buffer << "cp " << config->query_file.c_str() << " temp_query.txt";
    command = buffer.str();
    system(command.c_str());

	FILE *mod_q = fopen("temp_query.txt", "r+");
	fseek(mod_q, -1, SEEK_END);
	if (fgetc(mod_q) != '\n'){
		fseek(mod_q, 0, SEEK_END);
		fputc('\n',mod_q);				
	}
	fclose(mod_q);

	mod_q = fopen("temp_query.txt", "r+");
    fseek(mod_q, 0, SEEK_END);
    int i=0, fsize = ftell(mod_q);
    fseek(mod_q, 0, SEEK_SET);
    char temp;
    while(i<fsize){
        temp = fgetc(mod_q);
        if((temp=='|')||(temp=='&')||(temp==')')||(temp=='(')){
        	fseek(mod_q, -1, SEEK_CUR);			
            fputc(' ',mod_q);
        }
        // fseek(mod_q, 1, SEEK_CUR);
        i++;
    }
	fclose(mod_q);

	FILE *q_in = fopen("temp_query.txt", "r");
	q_out = fopen(config->out_file.c_str(), "w");
	yyin = q_in;

	dat = load_mem_data(config);

	yyparse();
	fclose(q_in);
	fclose(q_out);
	// cout<<"\nClosed files"<<endl;
	return 0;
}
