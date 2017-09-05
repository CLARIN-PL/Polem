//
// Created by gkubon on 31/07/17.
//

#include "DictionaryLemmatizer.h"
#include "CascadeLemmatizer.h"
#include <fstream>


using namespace std;



DictionaryLemmatizer::DictionaryLemmatizer(std::string pathname,
                                           bool typed) {
    //constructor stores dictionary in its own structure
    ifstream dictFile(pathname);

    string line;

    while(getline(dictFile,line)){
        UnicodeString dictLemma,dictOrth,dictCat;

        if(typed){

            dictCat=line.substr(0,line.find("\t")).c_str();
            dictOrth.append(line.substr(line.find("\t")+1).substr(0,line.substr(line.find("\t")+1).find_last_of("\t")).c_str());
            dictLemma=line.substr(line.rfind("\t")+1).c_str();

        }else{

            dictOrth=line.substr(0,line.find("\t")).c_str();
            UnicodeString temp = line.substr(line.find("\t")+1).c_str();
            dictLemma.append(temp.tempSubString(0,temp.indexOf("\t")));
            temp = temp.tempSubString(temp.indexOf("\t")+1);
            dictCat.append(temp.tempSubString(0,temp.indexOf("\t")));

        }

        this->dictionaryItems[dictOrth]=make_pair(dictLemma,dictCat);

    }
    dictFile.close();

}

icu::UnicodeString DictionaryLemmatizer::lemmatize(std::vector<std::vector<std::string> > keyword,
                                                   std::string category) {


    //dictionary lemmatizer checks dictionary files for a lemma for given orth
    icu::UnicodeString orth ="";
    icu::UnicodeString key = "";

    for(int i = 0; i < keyword.size(); i ++){
        orth.append(keyword[i][0].c_str());
        if(keyword[i][3]=="t"){
            orth.append(" ");
        }
    }

    if(category==""||category.find("nam_loc")!=0){
        return  "";
    }

    key.append(orth.trim());


    if(this->dictionaryItems.count(key)>0) {
        if(this->dictionaryItems[key].first.indexOf(":")!=-1){
            return this->dictionaryItems[key].first.tempSubStringBetween(0,this->dictionaryItems[key].first.indexOf(":"));
        }
        return this->dictionaryItems[key].first;
    }else return "";


}