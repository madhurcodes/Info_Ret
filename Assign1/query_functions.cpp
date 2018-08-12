#include "post.h"

posting_query* post_union(posting_query* post_1, posting_query* post_2){
        
        
}

posting_query* post_intersection(posting_query* post_1, posting_query* post_2){
    

}

posting_query* post_load(){

}

void args_help(){
        cout <<
                "Too lazy\n"
                "--help:              Show help\n";
        exit(-1);
}

void process_query_args(int argc, char** argv){

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
}