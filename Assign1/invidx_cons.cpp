#include "post.h"

int stemming = 0;
int stopword = 0;

string cleaned_folder_path = "./temp-docs/";
string named_entity_path   = "./temp-docs-named/";

map<string, posting_creation *> dictionary;

vector<string> doc_names; // doc_ids and para_ids start from 1
vector<int> doc_lengths; // doc_ids and para_ids start from 1

void parse_document(string filename, int doc_id){
    string filepath = cleaned_folder_path + filename;
    ifstream infile(filepath);
    string line;
    int line_num = 1;
    int words = 0;
    while (getline(infile, line))
    {
        istringstream iss(line);
        string word;
        while(iss >> word)
        {
            words++;
            if ( dictionary.find(word) == dictionary.end() ) {
                posting_creation* temp = new posting_creation();
                doc* tdoc = new doc();
                tdoc->doc_id = doc_id;
                tdoc->para_id.push_back(line_num);
                tdoc->para_count.push_back(1);
                temp->docs.push_back(tdoc);
                dictionary[word] = temp;
            }
            else{
                posting_creation* temp = dictionary[word];
                if( (temp->docs.size() == 0) || (temp->docs[temp->docs.size()-1]->doc_id != doc_id) ){
                    doc* tdoc = new doc();
                    tdoc->doc_id = doc_id;
                    tdoc->para_id.push_back(line_num);
                    tdoc->para_count.push_back(1);
                    temp->docs.push_back(tdoc);
                }
                else{
                    doc* tdoc = temp->docs[temp->docs.size()-1];
                    if( (tdoc->para_id.size() == 0) || (tdoc->para_id[tdoc->para_id.size()-1] != line_num)){
                        tdoc->para_id.push_back(line_num);
                        tdoc->para_count.push_back(1);
                    }
                    else{
                        tdoc->para_count[tdoc->para_id.size()-1] += 1;
                    }
                }
            }
        }

        line_num++;
    }
    doc_lengths.push_back(words);              
    infile.close();
}

void parse_document_named(string filename, int doc_id){
    string filepath = named_entity_path + filename;
    ifstream infile(filepath);
    string line;
    int line_num = 1;
    while (getline(infile, line))
    {
        istringstream iss(line);
        string word;
        while(iss >> word)
        {
            if ( dictionary.find(word) == dictionary.end() ) {
                posting_creation* temp = new posting_creation();
                doc* tdoc = new doc();
                tdoc->doc_id = doc_id;
                tdoc->para_id.push_back(line_num);
                tdoc->para_count.push_back(1);
                temp->named_docs.push_back(tdoc);
                dictionary[word] = temp;
            }
            else{
                posting_creation* temp = dictionary[word];
                if( (temp->named_docs.size() == 0) || (temp->named_docs[temp->named_docs.size()-1]->doc_id != doc_id) ){
                    doc* tdoc = new doc();
                    tdoc->doc_id = doc_id;
                    tdoc->para_id.push_back(line_num);
                    tdoc->para_count.push_back(1);
                    temp->named_docs.push_back(tdoc);
                }
                else{
                    doc* tdoc = temp->named_docs[temp->named_docs.size()-1];
                    if( (tdoc->para_id.size() == 0) || (tdoc->para_id[tdoc->para_id.size()-1] != line_num)){
                        tdoc->para_id.push_back(line_num);
                        tdoc->para_count.push_back(1);
                    }
                    else{
                        tdoc->para_count[tdoc->para_id.size()-1] += 1;
                    }
                }
            }
        }

        line_num++;
    }
    infile.close();
}

void parse_directory(const char *dir_name){
	DIR *dir;
	struct dirent *ent;
    int doc_id = 1;
	if ((dir = opendir (dir_name)) != NULL) {
	    while ((ent = readdir (dir)) != NULL){
			if(strlen(ent->d_name)==1 && ent->d_name[0]=='.')
				continue;
			if(strlen(ent->d_name)==2 && ent->d_name[0]=='.' && ent->d_name[1]=='.')
				continue;
			string str(ent->d_name);
			// cout<<ent->d_name<<endl;
			parse_document(str, doc_id);
			parse_document_named(str, doc_id);  
            doc_names.push_back(str);          
            doc_id++;
		}
		closedir (dir);
	} 
	else{
		exit(0);
	}
}


void args_help(){
        cout <<
                "--stemming <0,1>:           Set number of program\n"
                "--stopword <0,1>:           Set number of program\n"
                "--help:              Show help\n";
        exit(-1);
}

void process_construction_args(int argc, char** argv){
    const char* const short_opts = "s:t:h";
    const option long_opts[] = {
            {"stemming", required_argument, nullptr, 's'},
            {"stopwordremoval", required_argument, nullptr, 't'},
            {"help", no_argument, nullptr, 'h'},
            {nullptr, no_argument, nullptr, 0}
    };


    while (true)
    {
        auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

        if (opt == -1)
            break;
        
        switch (opt)
        {
        case 's':
            stemming = stoi(optarg);
            if((stemming!=0) && (stemming !=1)){
                cout<<"Error invalid input for stemming"<<endl;
                exit(-1);
            }
            break;

        case 't':
            stopword = stoi(optarg);
            if((stopword!=0) && (stopword !=1)){
                cout<<"Error invalid input for stopword"<<endl;
                exit(-1);
            }
            break;

        case 'h': // -h or --help
        case '?': 
        default:
            args_help();
            break;
        }
    }
}

void write_files(string ind){
    string idx_filename = ind + string(".idx") ;
    string dict_filename = ind + string(".dict") ;
    FILE* out_index = fopen(idx_filename.c_str(),"wb");
    int i=0,j=0,wr=0;
    wr = stemming;
    fwrite(&wr,4,1,out_index);
    wr = stopword;
    fwrite(&wr,4,1,out_index);
    wr = doc_names.size();
    fwrite(&wr,4,1,out_index);
    int temp_doc_id = 1;
    for(i=0;i<doc_names.size();i++){
        wr = temp_doc_id;
        fwrite(&wr,4,1,out_index);
        wr = doc_lengths[i];
        fwrite(&wr,4,1,out_index);        
        wr = doc_names[i].size();
        fwrite(&wr,4,1,out_index);
        fwrite(doc_names[i].c_str(),wr,1,out_index);
        temp_doc_id++;
    }
    vector<int> offs; // starts from 1 (termid)
    offs.push_back(-1); //filler
    vector<int> df;
    df.push_back(-1);
    map<string, posting_creation *>::iterator it;
    int term_id = 1;
    for (it = dictionary.begin(); it != dictionary.end(); it++){
        posting_creation* temp = it->second;
        offs.push_back(ftell(out_index));
        df.push_back(temp->docs.size());
        wr = term_id;
        fwrite(&wr,4,1,out_index);
        int address_named = ftell(out_index);
        wr = 0;
        fwrite(&wr,4,1,out_index);
        wr = temp->docs.size();
        fwrite(&wr,4,1,out_index);
        for(i=0;i<temp->docs.size();i++){
            wr = temp->docs[i]->doc_id;
            fwrite(&wr,4,1,out_index);
            wr = temp->docs[i]->para_id.size();
            fwrite(&wr,4,1,out_index);
            for(j=0;j<temp->docs[i]->para_id.size();j++){
                wr = temp->docs[i]->para_count[j];
                fwrite(&wr,4,1,out_index);
                wr = temp->docs[i]->para_id[j];
                fwrite(&wr,4,1,out_index);
            }
        }

        int address_temp = ftell(out_index);
        fseek(out_index, address_named, SEEK_SET);
        wr = address_temp;
        fwrite(&wr,4,1,out_index);
        fseek(out_index, address_temp, SEEK_SET);

        wr = temp->named_docs.size();
        fwrite(&wr,4,1,out_index);
        for(i=0;i<temp->named_docs.size();i++){
            wr = temp->named_docs[i]->doc_id;
            fwrite(&wr,4,1,out_index);
            wr = temp->named_docs[i]->para_id.size();
            fwrite(&wr,4,1,out_index);
            for(j=0;j<temp->named_docs[i]->para_id.size();j++){
                wr = temp->named_docs[i]->para_count[j];
                fwrite(&wr,4,1,out_index);
                wr = temp->named_docs[i]->para_id[j];
                fwrite(&wr,4,1,out_index);
            }
        }
        term_id++;
    }
    fclose(out_index);

    ofstream out_dict(dict_filename);
    int term_id_it = 1;
    for (it = dictionary.begin(); it != dictionary.end(); it++){
        out_dict << it->first;
        out_dict << ":";
        out_dict << df[term_id_it];
        out_dict << ":";
        out_dict << offs[term_id_it];
        out_dict << "\n";
        term_id_it++;
    }
    out_dict.close();
}

int main(int argc, char **argv){
    process_construction_args(argc, argv);
    int i=0,j=0;
    string directory = argv[argc-2];
    string indexfile = argv[argc-1];
    string command;
    stringstream buffer;
    // Note : python stemming makes all words lowercase
    buffer << "python3 process_docs.py stemming " << stemming << " stopword " << stopword << " "<< directory <<" "<< indexfile;
    command = buffer.str();
    system(command.c_str());
    // cout<<"Stemming is "<<stemming<<" stopword is "<<stopword<<" directory is "<<directory<<" and indexfile is "<<indexfile<<endl;
    parse_directory(directory.c_str());
    write_files(indexfile);
    return 0;
}