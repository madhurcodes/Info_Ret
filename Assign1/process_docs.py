import numpy as np
import nltk
import sys
import os
import shutil

from nltk.tokenize import RegexpTokenizer
from nltk.stem.porter import PorterStemmer
from nltk.corpus import stopwords
from nltk.chunk import conlltags2tree, tree2conlltags

tokenizer = RegexpTokenizer(r'[A-Za-z]+|[0-9]+')
en_stop = set(stopwords.words('english'))
p_stemmer = PorterStemmer()

stemming = int(sys.argv[2])
stopword = int(sys.argv[4])
directory = sys.argv[5]
indexfile = sys.argv[6]

# stemming = 1
# stopword = 1
# directory = "./daat"
# indexfile = "ind_test"


#print("Stemming ", stemming, " inf ", indexfile)

t_dir = "./temp-docs"
n_dir = "./temp-docs-named"
if os.path.exists(t_dir):
    shutil.rmtree(t_dir)
if os.path.exists(n_dir):
    shutil.rmtree(n_dir)

os.makedirs(t_dir)
os.makedirs(n_dir)

for filename in os.listdir(directory):
    with open(directory+"/" + filename, 'r') as i_f, open(t_dir+"/"+filename,'w') as o_f, open(n_dir+"/"+filename,'w') as on_f:
        for line in i_f:
            # line = line.lower()
            tok = tokenizer.tokenize(line)
            pos = nltk.pos_tag(tok)
            ner = tree2conlltags(nltk.ne_chunk(pos))
            tok_n = []
            for chunk in ner:
                if (chunk[2] != 'O'):
                    tok_n.append(chunk[0])
            # print("sent - ", line, " ner - ",ner," tokn ",tok_n,"\n")

            if stopword:
                tok = [token for token in tok if token not in en_stop]
                tok_n = [token for token in tok_n if token not in en_stop]
            if stemming:
                #Note : python stemming makes all words lowercase
                tok = [p_stemmer.stem(token) for token in tok]
                tok_n = [p_stemmer.stem(token) for token in tok_n]
            line = " ".join(tok) + "\n"
            line_n = " ".join(tok_n) + "\n"
            line = line.lower()
            line_n = line_n.lower()
            # print("--- ",line_n,"\n")
            o_f.write(line)
            on_f.write(line_n)