#include <iostream>
#include "OrthLemmatizer.h"

using namespace std;

icu::UnicodeString OrthLemmatizer::lemmatize(std::vector<std::vector<icu::UnicodeString> > keyword) {
    UnicodeString lemma;
    for(auto& i:keyword){
        lemma.append(i[0]);
        if(i[3]=="True"){
            lemma.append(' ');
        }
    }
    return lemma.trim();

}
