//
// Created by gkubon on 07/08/17.
//

#include <iostream>
#include "OrthLemmatizer.h"


using namespace std;


icu::UnicodeString OrthLemmatizer::lemmatize(std::vector<std::vector<std::string> > keyword) {
    UnicodeString lemma;

    bool first = true;
    //TODO znaki "" / ; ++ w orth \/
    for(int i = 0 ; i < keyword.size() ; ++i){
        if(i>0){
            if(keyword[i-1][0].find("-")==string::npos){
                lemma.append(" ");
            }
        }
        if(keyword[i][0].find("-")!=string::npos||keyword[i][0].find(".")!=string::npos){
            lemma = lemma.trim();
        }
        lemma.append(keyword[i][0].c_str());
    }
    lemma = lemma.trim();
    return lemma;

}
