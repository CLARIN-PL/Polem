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

        if(keyword[i][0].find("-")!=string::npos||keyword[i][0].find(".")!=string::npos||keyword[i][0].find("+")!=string::npos
           ||keyword[i][0].find("?")!=string::npos||keyword[i][0].find(")")!=string::npos||keyword[i][0].find(":")!=string::npos){
            lemma = lemma.trim();
        }else if(!first&&keyword[i][0].find("\"")!=string::npos){
            lemma = lemma.trim();
        }

        lemma.append(keyword[i][0].c_str());

        if(keyword[i][0].find("-")==string::npos&&keyword[i][0].find("#")==string::npos&&keyword[i][0].find("+")==string::npos
           &&keyword[i][0].find("(")==string::npos&&keyword[i][0].find("\"")==string::npos){
            lemma.append(" ");
        }else if(keyword[i][0].find("\"")!=string::npos&&first){
            //lemma.append(" ");
            first = false;
        }else if(keyword[i][0].find("\"")!=string::npos&&!first){
            lemma.append(" ");
        }
    }
    lemma = lemma.trim();
    return lemma;

}
