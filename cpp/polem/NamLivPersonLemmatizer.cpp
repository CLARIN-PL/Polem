//
// Created by gkubon on 01/08/17.
//

#include <boost/algorithm/string/split.hpp>
#include <unicode/regex.h>
#include "NamLivPersonLemmatizer.h"
#include "CascadeLemmatizer.h"



using namespace std;



NamLivPersonLemmatizer::NamLivPersonLemmatizer(
            std::map<icu::UnicodeString, std::pair<icu::UnicodeString, icu::UnicodeString> > dictionaryItems,
            Inflection inflection) : inflection(inflection) {


    this->categories.emplace_back("nam_liv_person");
    this->categories.emplace_back("nam_liv_person_last");
    this->categories.emplace_back("nam_liv_person_first");

    //Formy tekstowe do skopiowania
    this->copyOrths.emplace_back("św");
    this->copyOrths.emplace_back(".");
    //lista form bazowych, dla nazw jednowyrazowych, dla których ma być wymuszona forma
    this->forceSingleBases.emplace_back("Maja");
    //Zbiór separatorów oddzielających formę bazową od końcówki formy odmienionej
    this->inflectionSeparators.emplace_back("-");
    this->inflectionSeparators.emplace_back("'");
    //this->inflectionSeparators.push_back("’");
    //this->inflectionSeparators.push_back(" ");
    //this->inflectionSeparators.push_back("´");
    //Zbiór końcówek form odmienionych, które mogą wystąpić po separatorze
    this->inflectionEndings.emplace_back("a");
    this->inflectionEndings.emplace_back("cie");
    this->inflectionEndings.emplace_back("em");
    this->inflectionEndings.emplace_back("s");
    this->inflectionEndings.emplace_back("ego");
    //this->inflectionEndings.push_back("u");
    //this->inflectionEndings.push_back("owi");
    //for(map<UnicodeString,pair<UnicodeString,UnicodeString>>::iterator it = dictionaryItems.begin(); it!=dictionaryItems.end(); ++it){
    for(auto& it:dictionaryItems){
        if(it.second.first=="nam_liv_person"){

            UnicodeString heh = it.first;
            this->fullnames[heh.toLower()] = it.second.second;

            UErrorCode status = U_ZERO_ERROR;
            RegexMatcher rm("\\s", 0, status);
            UnicodeString check[9];

            int numWords = rm.split(it.first, check, 9, status);
            UnicodeString orths[numWords];
            rm.split(it.first, orths, numWords, status);

            int numWordsLem = rm.split(it.second.second, check, 9, status);
            UnicodeString lemmas[numWordsLem];
            rm.split(it.second.second, lemmas, numWordsLem, status);


            if (numWords == numWordsLem) {
                for (int i = 0; i < numWords; ++i) {
                    string check1, check2;
                    orths[i].toLower().toUTF8String(check1);
                    lemmas[i].toUTF8String(check2);
                    this->names[orths[i].toLower()].emplace_back(lemmas[i]);
                }
            }
        }
    }

    this->inflection = inflection;

}

icu::UnicodeString
NamLivPersonLemmatizer::lemmatize(std::vector <std::vector<icu::UnicodeString>> keyword, std::string category,
                                  bool debug) {

    if (debug) {
        cout << "Entering NamLivPerson lemmatizer" << endl;
    }

    if(find(this->categories.begin(),this->categories.end(),category.c_str())==this->categories.end()){
        return "";
    }

    UnicodeString name;

    // Wymuszone lematy dla jednowyrazowych nazw
    if (keyword.size() == 1 && find(this->forceSingleBases.begin(), this->forceSingleBases.end(), keyword[0][1]) !=
                               this->forceSingleBases.end()) {
        globalMethod = "NamLivPersonLemmatizer:SingleBases";
        return keyword[0][1];
    }

    if (keyword.size() == 3) {
        //Lematyzacja nazw z końcówkami typu 'a -a
        if (find(this->inflectionSeparators.begin(), this->inflectionSeparators.end(), keyword[1][0]) !=
            this->inflectionSeparators.end()) {

            UnicodeString tmp = keyword[2][0];
            if (find(this->inflectionEndings.begin(), this->inflectionEndings.end(), tmp.toLower()) !=
                this->inflectionEndings.end()) {
                globalMethod = "NamLivPersonLemmatizer:SeparatorEnding";
                return keyword[0][0];
            }
        }
    }

    UnicodeString key;
    //for (int i = 0; i < keyword.size(); ++i) {
    for(auto& i:keyword){
        key.append(i[0]);
        if (i[3] == "True") {
            key.append(" ");
        }
    }

    if (this->fullnames.find(key.toLower()) != this->fullnames.end()) {
        name = this->fullnames[key.toLower()];
    } else {
        UnicodeString orth;
        //for (int i = 0; i < keyword.size(); ++i) {
        for(auto& i:keyword){
            orth = i[0];

            if ((orth.length() == 1 && orth.charAt(0) >= 'A' && orth.charAt(0) <= 'Z') ||
                find(this->copyOrths.begin(), this->copyOrths.end(), orth.toLower()) != this->copyOrths.end()) {
                name.append(i[0]);
                if (i[3] == "True")name.append(" ");

            } else if (this->names.find(orth.toLower()) != this->names.end()) {

                name.append(this->names[orth.toLower()].front());

                if (i[3] == "True" || this->names[orth.toLower()].front() == "św.")name.append(" ");
            } else {
                name = "";
                break;
            }
        }
    }

    if(name==""){
        for(auto& i:keyword) {
            UnicodeString lemma = this->inflection.generate_base(i[2], i[0]);
            if (lemma == "") {
                if (debug) {
                    cout << "Exiting NamLivPerson lemmatizer" << endl;
                }
                return lemma;
            }
            name.append(lemma);
            name.append(" ");
        }
        name.trim();
        globalMethod="NamLivPersonLemmatizer:Inflection";
        return name;
    }else{
        name.trim();
        globalMethod="NamLivPersonLemmatizer:Dictionary";
        return name;
    }

}



