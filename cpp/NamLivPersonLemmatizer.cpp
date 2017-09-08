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

    //Formy tekstowe do skopiowania
    this->copyOrths.push_back("św");
    this->copyOrths.push_back(".");
    //lista form bazowych, dla nazw jednowyrazowych, dla których ma być wymuszona forma
    this->forceSingleBases.push_back("Maja");
    //Zbiór separatorów oddzielających formę bazową od końcówki formy odmienionej
    this->inflectionSeparators.push_back("-");
    this->inflectionSeparators.push_back("'");
    this->inflectionSeparators.push_back("’");
    this->inflectionSeparators.push_back(" ");
    this->inflectionSeparators.push_back("´");
    //Zbiór końcówek form odmienionych, które mogą wystąpić po separatorze
    this->inflectionEndings.push_back("a");
    this->inflectionEndings.push_back("cie");
    this->inflectionEndings.push_back("em");
    this->inflectionEndings.push_back("s");
    this->inflectionEndings.push_back("ego");
    this->inflectionEndings.push_back("u");
    this->inflectionEndings.push_back("owi");

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
NamLivPersonLemmatizer::lemmatize(std::vector<std::vector<icu::UnicodeString> > keyword, std::string category) {

    if(find(this->categories.begin(),this->categories.end(),category.c_str())==this->categories.end()){
        return "";
    }

    UnicodeString orth;
    for(int i = 0; i < keyword.size(); ++i){
        if (keyword[i][3] == "True") {
            orth.append(keyword[i][0] + " ");
        } else orth.append(keyword[i][0]);
    }

    UnicodeString name;

    // Wymuszone lematy dla jednowyrazowych nazw
    if (keyword.size() == 1 && find(this->forceSingleBases.begin(), this->forceSingleBases.end(), keyword[0][1]) !=
                               this->forceSingleBases.end()) {
        globalMethod="NamLivPersonLemmatizer::Dictionary";
        return keyword[0][1];
    }

    if (keyword.size() == 3) {
        //Lematyzacja nazw z końcówkami typu 'a -a
        if (find(this->inflectionSeparators.begin(), this->inflectionSeparators.end(), keyword[1][0]) !=
            this->inflectionSeparators.end()) {

            UnicodeString tmp = keyword[2][0];
            if (find(this->inflectionEndings.begin(), this->inflectionEndings.end(), tmp.toLower()) !=
                this->inflectionEndings.end()) {
                return keyword[0][0];
            }
        }
    }

    for(int i = 0; i < keyword.size(); ++i){

        if (i + 2 < keyword.size() &&
            find(this->inflectionSeparators.begin(), this->inflectionSeparators.end(), keyword[i + 1][0]) !=
            this->inflectionSeparators.end()) {

            UnicodeString tmp = keyword[i + 2][0];
            if (find(this->inflectionEndings.begin(), this->inflectionEndings.end(), tmp.toLower()) !=
                this->inflectionEndings.end()) {
                name.append(keyword[i][0]);
                i = i + 2;
                continue;
            }
        }

        orth = keyword[i][0];
        if((orth.length()==1&&orth.toUpper()==orth)||find(this->copyOrths.begin(),this->copyOrths.end(),orth.toLower())!=this->copyOrths.end()){
            name.append(orth);
            if (keyword[i][3] == "True")name.append(" ");
        }else if(this->names.find(orth.toLower())!=this->names.end()){
            name.append(this->names[orth.toLower()]);
            if (keyword[i][3] == "True")name.append(" ");
        }else {
            name = "";
            break;
        }
    }


    if(name==""){
        for(int i = 0; i < keyword.size(); ++i){
            string view1, view2;
            keyword.at(i).at(2).toUTF8String(view1);
            keyword[i][0].toUTF8String(view2);
            UnicodeString lemma = this->inflection.generate_base(keyword.at(i).at(2), keyword.at(i).at(0));
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



