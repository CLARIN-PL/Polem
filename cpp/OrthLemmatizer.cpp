//
// Created by gkubon on 07/08/17.
//

#include <iostream>
#include "OrthLemmatizer.h"


using namespace std;


icu::UnicodeString OrthLemmatizer::lemmatize(std::vector<std::vector<std::string> > keyword) {
    UnicodeString lemma;


    //orth lemmatizer returns input

    for(int i = 0 ; i < keyword.size() ; ++i){
        lemma.append(keyword[i][0].c_str());
        lemma.append(" ");
    }
    lemma = lemma.trim();
    return lemma;

}
