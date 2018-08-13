import nltk
import sys

from nltk.tokenize import RegexpTokenizer
from nltk.stem.porter import PorterStemmer
from nltk.corpus import stopwords

tokenizer = RegexpTokenizer(r'[A-Za-z]+|[0-9]+')
en_stop = set(stopwords.words('english'))
p_stemmer = PorterStemmer()

stemming = int(sys.argv[1])
stopword = int(sys.argv[2])
query = sys.argv[3]

with open("my_pipe.txt","w") as pipe:
    toky = tokenizer.tokenize(query)
    # print("\nToky is - ",toky," \n")
    tok = toky[0]

    if stopword:
        if tok in en_stop:
            tok = ""
    if stemming:
        tok = p_stemmer.stem(tok)
    pipe.write(tok)