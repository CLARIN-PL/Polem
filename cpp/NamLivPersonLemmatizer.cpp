//
// Created by gkubon on 01/08/17.
//

#include "NamLivPersonLemmatizer.h"
#include "CascadeLemmatizer.h"



using namespace std;



NamLivPersonLemmatizer::NamLivPersonLemmatizer(
            std::map<icu::UnicodeString, std::pair<icu::UnicodeString, icu::UnicodeString> > dictionaryItems,
            Inflection inflection) : inflection(inflection) {


    this->categories.push_back("nam_liv_person");
    this->categories.push_back("nam_liv_person_last");
    this->categories.push_back("nam_liv_person_first");


    this->copyOrths.push_back("św");
    this->copyOrths.push_back(".");

    this->forceSingleBases.push_back("Maja");

    this->inflectionSeparators.push_back("-");
    this->inflectionSeparators.push_back("'");

    this->inflectionEndings.push_back("a");
    this->inflectionEndings.push_back("cie");
    this->inflectionEndings.push_back("em");
    this->inflectionEndings.push_back("s");
    this->inflectionEndings.push_back("ego");

    for(map<UnicodeString,pair<UnicodeString,UnicodeString>>::iterator it = dictionaryItems.begin(); it!=dictionaryItems.end(); ++it){
        if(it->second.first=="nam_liv_person"){
            vector<UnicodeString> orths = Inflection::split(it->first," ");
            vector<UnicodeString> lemmas = Inflection::split(it->second.second," ");
            if(orths.size()==lemmas.size()){
                for(int i = 0; i < lemmas.size(); ++i){
                    this->names[orths[i].toLower()]=lemmas[i];
                }
            }
        }
    }

    this->inflection = inflection;

}

icu::UnicodeString
NamLivPersonLemmatizer::lemmatize(std::vector<std::vector<std::string> > keyword, std::string category) {

    if(find(this->categories.begin(),this->categories.end(),category.c_str())==this->categories.end()){
        return "";
    }

    UnicodeString orth;
    for(int i = 0; i < keyword.size(); ++i){
        orth.append(keyword[i][0].c_str());
    }

    UnicodeString name;

    if(keyword.size()==1&&find(this->forceSingleBases.begin(),this->forceSingleBases.end(),keyword[0][1].c_str())!=this->forceSingleBases.end()){
        globalMethod="NamLivPersonLemmatizer::Dictionary";
        return keyword[0][1].c_str();
    }


    if(keyword.size()==3&&find(this->inflectionSeparators.begin(),this->inflectionSeparators.end(),keyword[1][0].c_str())!=this->inflectionSeparators.end()){

        UnicodeString tmp = keyword[2][0].c_str();
        if(find(this->inflectionEndings.begin(),this->inflectionEndings.end(),tmp.toLower())!=this->inflectionEndings.end()) {
            return keyword[0][0].c_str();
        }
    }


    for(int i = 0; i < keyword.size(); ++i){
        orth = keyword[i][0].c_str();
        if((orth.length()==1&&orth.toUpper()==orth)||find(this->copyOrths.begin(),this->copyOrths.end(),orth.toLower())!=this->copyOrths.end()){
            name.append(orth);
        }else if(this->names.find(orth.toLower())!=this->names.end()){
           if(i>0)name.append(" ");
                name.append(this->names[orth.toLower()]);
        }else {
            name = "";
            break;
        }
    }

    if(name==""){
        for(int i = 0; i < keyword.size(); ++i){

            UnicodeString lemma = this->inflection.generate_base(keyword[i][2].c_str(),keyword[i][0].c_str());
            if(lemma==""){
                return lemma;
            }
            if(i>0){
                name.append(" ");
            }
            name.append(lemma);
        }
        globalMethod="NamLivPersonLemmatizer::Inflection";
        return name;
    }else{
        globalMethod="NamLivPersonLemmatizer::Dictionary";
        return name;
    }

}



