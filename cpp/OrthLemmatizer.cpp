//
// Created by gkubon on 07/08/17.
//

#include "OrthLemmatizer.h"


using namespace std;


icu::UnicodeString OrthLemmatizer::lemmatize(std::vector<std::vector<std::string> > keyword) {
    UnicodeString lemma;

    for(vector<vector<string> >::iterator it=keyword.begin();it!=keyword.end();++it){
                if(it!=keyword.begin()){
                    lemma.append(" ");
                }
                lemma.append(it->front().c_str());
    }
    return lemma;
}
