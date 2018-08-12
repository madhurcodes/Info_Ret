#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <dirent.h>
#include <cstdio>
#include <map>
#include <sstream>
#include <fstream>
#include <cstring>

using namespace std;

typedef struct {
    int docid;
    vector<int> paraid;
    vector<int> paracount;
} doc;

typedef struct {
    vector<doc *> docs;
    vector<doc *> named_docs;
} posting_creation;

typedef struct {
    vector<doc *> docs;
} posting_query;

typedef struct {
    string query_file;
    int cutoff;
    string out_file;
    string idx_file;
    string dict_file;
} query_config;

