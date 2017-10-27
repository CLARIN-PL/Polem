//
// Created by gkubon on 07/08/17.
//

#include <iostream>
#include "OrthLemmatizer.h"


using namespace std;


icu::UnicodeString OrthLemmatizer::lemmatize(std::vector<std::vector<icu::UnicodeString> > keyword) {
    UnicodeString lemma;


    //orth lemmatizer returns input

    for(auto& i:keyword){
        lemma.append(i[0]);
        if(i[3]=="True"){
            lemma.append(' ');
        }
    }


    lemma = lemma.trim();
    return lemma;

}
