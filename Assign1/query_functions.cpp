#include "post.h"

vector<pair<int,char*> > vector_search_sol;
vector<int > freq = vector<int>();

posting_query* post_union(posting_query* post_1, posting_query* post_2){
    posting_query* result = new posting_query();
    int i=0,j=0;
    while( (i<post_1->docs.size()) && (j<post_2->docs.size()) ){
        doc* temp = new doc();
        if(post_1->docs[i]->doc_id == post_2->docs[j]->doc_id){
            temp->doc_id = post_1->docs[i]->doc_id;
            int k=0,l=0;
            while( (k<post_1->docs[i]->para_id.size()) && (l<post_2->docs[j]->para_id.size()) ){
                if( post_1->docs[i]->para_id[k] == post_2->docs[j]->para_id[l] ){
                    temp->para_id.push_back(post_1->docs[i]->para_id[k]);
                    temp->para_count.push_back(max(post_1->docs[i]->para_count[k],post_2->docs[j]->para_count[l]));
                    k++;
                    l++;
                }
                else if(post_1->docs[i]->para_id[k] < post_2->docs[j]->para_id[l]){
                    temp->para_id.push_back(post_1->docs[i]->para_id[k]);
                    temp->para_count.push_back(post_1->docs[i]->para_count[k]);
                    k++;
                }
                else{
                    temp->para_id.push_back(post_2->docs[j]->para_id[l]);
                    temp->para_count.push_back(post_2->docs[j]->para_count[l]);
                    l++;
                }
            }
            while(k<post_1->docs[i]->para_id.size()){
                temp->para_id.push_back(post_1->docs[i]->para_id[k]);
                temp->para_count.push_back(post_1->docs[i]->para_count[k]);
                k++;
            }
            while(l<post_2->docs[j]->para_id.size()){
                temp->para_id.push_back(post_2->docs[j]->para_id[l]);
                temp->para_count.push_back(post_2->docs[j]->para_count[l]);
                l++;
            }
            i++;
            j++;
        }
        else if(post_1->docs[i]->doc_id < post_2->docs[j]->doc_id){
            temp->doc_id = post_1->docs[i]->doc_id;
            int k=0;
            for(k=0;k<post_1->docs[i]->para_id.size();k++){
                temp->para_id.push_back(post_1->docs[i]->para_id[k]);
                temp->para_count.push_back(post_1->docs[i]->para_count[k]);
            }
            i++;
        }
        else{
            temp->doc_id = post_2->docs[j]->doc_id;
            int k=0;
            for(k=0;k<post_2->docs[j]->para_id.size();k++){
                temp->para_id.push_back(post_2->docs[j]->para_id[k]);
                temp->para_count.push_back(post_2->docs[j]->para_count[k]);
            }
            j++;
        }
        result->docs.push_back(temp); // PB done in all three cases so doin at end
    }
    while(i<post_1->docs.size()){
        doc* temp = new doc();
        temp->doc_id = post_1->docs[i]->doc_id;
        int k=0;
        for(k=0;k<post_1->docs[i]->para_id.size();k++){
            temp->para_id.push_back(post_1->docs[i]->para_id[k]);
            temp->para_count.push_back(post_1->docs[i]->para_count[k]);
        }
        result->docs.push_back(temp);
        i++;
    }
    while(j<post_2->docs.size()){
        doc* temp = new doc();
        temp->doc_id = post_2->docs[j]->doc_id;
        int k=0;
        for(k=0;k<post_2->docs[j]->para_id.size();k++){
            temp->para_id.push_back(post_2->docs[j]->para_id[k]);
            temp->para_count.push_back(post_2->docs[j]->para_count[k]);
        }
        result->docs.push_back(temp);
        j++;
    }
    return result;   
}

posting_query* post_intersection(posting_query* post_1, posting_query* post_2){
    // O(n+m) can be done better in max(n,m)*log(min(n,m)) time with binary search
    posting_query* result = new posting_query();
    int i=0,j=0;
    while( (i<post_1->docs.size()) && (j<post_2->docs.size()) ){
        if( post_1->docs[i]->doc_id == post_2->docs[j]->doc_id ){
            doc* temp = new doc();
            temp->doc_id = post_1->docs[i]->doc_id;
            int k=0,l=0;
            while( (k<post_1->docs[i]->para_id.size()) && (l<post_2->docs[j]->para_id.size()) ){
                if( post_1->docs[i]->para_id[k] == post_2->docs[j]->para_id[l] ){
                    temp->para_id.push_back(post_1->docs[i]->para_id[k]);
                    temp->para_count.push_back(min(post_1->docs[i]->para_count[k],post_2->docs[j]->para_count[l]));
                    k++;
                    l++;
                }
                else if(post_1->docs[i]->para_id[k] < post_2->docs[j]->para_id[l]){
                        k++;
                }
                else{
                        l++;
                }
            }
            result->docs.push_back(temp);
            i++;
            j++;
        }
        else if(post_1->docs[i]->doc_id < post_2->docs[j]->doc_id){
                i++;
        }
        else{
                j++;
        }
    }
    return result;
}

posting_query* post_load(query_config* config, int offset, int named){
    FILE* index = fopen(config->idx_file.c_str(), "rb");
    fseek(index, offset, SEEK_SET);
    int term_id;
    fread(&term_id,4,1,index);
    if(named == 1){
        int real_offset;
        fread(&real_offset,4,1,index);
        fseek(index, real_offset, SEEK_SET);
    }
    else if(named == 0){
        fseek(index, offset+8, SEEK_SET);
    }
    else{
        cout<<"ERROR";
        exit(-1);
    }
    // seeked to beginning of posting list (first thingy shud be num_docs)
    posting_query* result = new posting_query();
    int i,j,k;
    int num_docs;
    fread(&num_docs,4,1,index);
    for(i=0;i<num_docs;i++){
        doc* temp = new doc();
        int doc_id, num_paras;
        fread(&doc_id,4,1,index);
        temp->doc_id = doc_id;
        fread(&num_paras,4,1,index);
        for(j=0;j<num_paras;j++){
            int para_count, para_id;
            fread(&para_count,4,1,index);
            fread(&para_id,4,1,index);
            temp->para_id.push_back(para_id);
            temp->para_count.push_back(para_count);
        }
        result->docs.push_back(temp);
    }
    fclose(index);
    return result;
}

posting_query* token_to_posting_list(char* tok, query_config* config, memory_data* dat, int named){
    string word(tok);
    // do stemming and stopword
    
    string command;
    stringstream buffer;
    buffer << "python3 process_query.py " << dat->stemming << " " << dat->stopword << " " << word;
    command = buffer.str();
    system(command.c_str());

    ifstream pipe("my_pipe.txt");
    // string newword((istreambuf_iterator<char>(pipe)),
    //              istreambuf_iterator<char>());
    stringstream read_buffer;
    read_buffer << pipe.rdbuf();
    string newword = read_buffer.str();

    posting_query* temp;
    if ( dat->dictionary.find(newword) == dat->dictionary.end() ) {
        temp = new posting_query();
        freq.push_back(1);
    }
    else{
        int offset = dat->dictionary[newword].second;
        freq.push_back(dat->dictionary[newword].first);
        temp = post_load(config,offset,named);
    }
    return temp;
}

posting_query* prefix_token_to_posting_list(char* tok, query_config* config, memory_data* dat, int named){
    string word(tok);
    // do stemming and stopword
    
    string command;
    stringstream buffer;
    buffer << "python3 process_query.py " << dat->stemming << " " << dat->stopword << " " << word;
    command = buffer.str();
    system(command.c_str());

    ifstream pipe("my_pipe.txt");
    // string newword((istreambuf_iterator<char>(pipe)),
    //              istreambuf_iterator<char>());
    stringstream read_buffer;
    read_buffer << pipe.rdbuf();
    string newword = read_buffer.str();
    if ( dat->dictionary.find(newword) == dat->dictionary.end() ) {
        freq.push_back(1);
    }
    else{
        freq.push_back(dat->dictionary[newword].first);
    }

    // prefix search
    posting_query *res, *temp, *del;   
    int offset; 
    map<string, pair<int,int> >::iterator matched = dat->dictionary.lower_bound(newword);    
    // map<string, pair<int,int> >::iterator matched = lower_bound(dat->dictionary.begin(),dat->dictionary.end(),newword);
    res = new posting_query();
    while(matched != dat->dictionary.end()){
        if((newword.length() <= matched->first.length()) && (equal(newword.begin(),newword.end(),matched->first.begin()))){
            offset = dat->dictionary[matched->first].second;
            temp = post_load(config,offset,named);
            del = res;
            res = post_union(res,temp);
            delete(temp);
            delete(del);
            matched++;
        }
        else{
            break;
        }
    }
    return res;
}

memory_data* load_mem_data(query_config* config){

    ifstream infile(config->dict_file);
    string line;
    int df,off,i=0,j=0,size=0;
    memory_data* dat = new memory_data();
    while (getline(infile, line)){
        istringstream iss(line);
        string token;
        getline(iss, token, ':');
        string temp;
        getline(iss, temp, ':');
        df = stoi(temp);
        getline(iss, temp, ':');
        off = stoi(temp);
        dat->dictionary.insert(make_pair(token,make_pair(df,off)));
    }
    infile.close();

    FILE* index = fopen(config->idx_file.c_str(), "rb");
    fread(&(dat->stemming),4,1,index);
    fread(&(dat->stopword),4,1,index);
    string dummy = string("");
    dat->doc_names.push_back(dummy);
    dat->doc_lengths.push_back(-1);
    int num_docs;
    fread(&num_docs,4,1,index);
    for(i=0;i<num_docs;i++){
        fread(&j,4,1,index);
        if(j!=i+1){
            cout<<"ERROR";
            exit(-1);
        }
        fread(&j,4,1,index);
        dat->doc_lengths.push_back(j);
        fread(&size,4,1,index);
        string temp;
        temp.resize(size);
        fread(&temp[0],size,1,index);
        dat->doc_names.push_back(temp);      
    }
    fclose(index);

    return dat;
}

void write_output(posting_query* q, memory_data* dat, query_config* config, FILE* q_out){
    int i,num_docs;
    for(i=0;i<q->docs.size();i++){
        string docname = dat->doc_names[q->docs[i]->doc_id];
        fprintf(q_out, "%s %d\n",docname.c_str(),2);
    }
}

float score_doc(doc* d){
    float s = 0;
    int i = 0;
    for(i==0;i<d->para_id.size();i++){
        s += pow(2,-d->para_id[i]);
    }
    return s;
}

void write_output_and_scores(posting_query* q, memory_data* dat, query_config* config, FILE* q_out){
    int i,num_docs;
    vector<pair<float,string> > os;
    for(i=0;i<q->docs.size();i++){
        os.push_back(make_pair(score_doc(q->docs[i]),dat->doc_names[q->docs[i]->doc_id]));
    }
    sort(os.begin(), os.end(), [](const pair<float,string> &a, const pair<float,string> &b){
        return a.first > b.first;
    });
    for(i=0;(i<config->cutoff) && (i<os.size());i++){
        fprintf(q_out, "%s %f\n",os[i].second.c_str(),os[i].first);
    }
    // string docname = dat->doc_names[q->docs[i]->doc_id];
    // fprintf(q_out, "%s %d\n",docname.c_str(),2);
}

void vec_write_output_and_scores(memory_data* dat, query_config* config, FILE* q_out){
    vector<pair<int,char*> > qq = vector_search_sol;
    freq.clear();
    vector<posting_query *> lop = vector<posting_query *>();
    int i,j,k;
    for(i=0;i<qq.size();i++){
        if(qq[i].first==1){
            lop.push_back(token_to_posting_list(qq[i].second, config, dat, 0));
        }
        else if(qq[i].first==2){
            lop.push_back(token_to_posting_list(qq[i].second, config, dat, 1));
        }
        else if(qq[i].first==3){
            lop.push_back(prefix_token_to_posting_list(qq[i].second, config, dat, 0));
        }
        else if(qq[i].first==4){
            lop.push_back(prefix_token_to_posting_list(qq[i].second, config, dat, 1));
        }
        else{
            cout<<"err";
            exit(-1);
        }
    }
    map<int,float> scores = map<int,float>();
    for(i=0;i<lop.size();i++){
        for(j=0;j<lop[i]->docs.size();j++){
            int doc_id = lop[i]->docs[j]->doc_id;
            float temp_score = 0;
            for(k=0;k<lop[i]->docs[j]->para_id.size();k++){
                // zone weight applied to term frequency 
                temp_score += lop[i]->docs[j]->para_count[k]*pow(2,-(lop[i]->docs[j]->para_id[k]));
            }
            temp_score = log10(1+temp_score);
            // string temp_q(qq[i].second);
            // int df = dat->dictionary[temp_q].first;
            int df = freq[i];
            if(df<1){
                df = 1;
            }
            // idf scaling
            temp_score  = temp_score*log10(1+((1.0*dat->doc_names.size())/df));
            if ( scores.find(doc_id) == scores.end() ) {
                scores[doc_id] = temp_score;
            }
            else{
                scores[doc_id] += temp_score;                
            }
        }
    }
    
    map<int, float>::iterator it;
    vector<pair<float,string> > os;   
    //  norm scaling and sort by scores
    for (it = scores.begin(); it != scores.end(); it++){
        os.push_back(make_pair(it->second/dat->doc_lengths[it->first],dat->doc_names[it->first]));        
    }
    sort(os.begin(), os.end(), [](const pair<float,string> &a, const pair<float,string> &b){
        return a.first > b.first;
    });
    for(i=0;(i<config->cutoff) && (i<os.size());i++){
        fprintf(q_out, "%s %f\n",os[i].second.c_str(),os[i].first);
    }
    vector_search_sol.clear();
}

void args_help(){
        cout <<
                "Too lazy\n"
                "--help:              Show help\n";
        exit(-1);
}

query_config* process_query_args(int argc, char** argv){

    query_config* config = new query_config();
    config->cutoff = 10;

    const char* const short_opts = "q:c:o:i:d:h";
    const option long_opts[] = {
            {"query", required_argument, nullptr, 'q'},
            {"cutoff", required_argument, nullptr, 'c'},
            {"output", required_argument, nullptr, 'o'},
            {"index", required_argument, nullptr, 'i'},
            {"dict", required_argument, nullptr, 'd'},
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
        case 'q':
            config->query_file = optarg;
            break;
        case 'o':
            config->out_file = optarg;
            break;
        case 'i':
            config->idx_file = optarg;
            break;
        case 'd':
            config->dict_file = optarg;
            break;
        case 'c':
            config->cutoff = stoi(optarg);
            break;
        case 'h': // -h or --help
        case '?': 
        default:
            args_help();
            break;
        }
    }
    return config;
}