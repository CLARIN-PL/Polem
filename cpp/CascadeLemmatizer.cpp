//
// Created by gkubon on 18/07/17.
//

#include "CascadeLemmatizer.h"
#include <armadillo>


using namespace std;


string globalMethod = "";

CascadeLemmatizer::CascadeLemmatizer(string pathname, Corpus2::Tagset tagset, morfeusz::Morfeusz* generator,map<UnicodeString,
        pair<UnicodeString, UnicodeString> > dictionaryItems,Inflection inflection,Inflection inflectionNamLoc):
            nelexLemmatizer("nelexicon2_wikipedia-infobox-forms-with-bases-filtered.txt",true),
                ruleLemmatizer(std::move(pathname), std::move(tagset),generator,true, true),
                    morfGeoLemmatizer("sgjp-20160310-geograficzne.tab", false),
                        namLivPersonLemmatizer(std::move(dictionaryItems), std::move(inflection)),
                            namLocLemmatizer(std::move(inflectionNamLoc)),
                                orthLemmatizer(){



}

icu::UnicodeString CascadeLemmatizer::lemmatize(std::vector<std::vector<std::string>> kw, std::string kw_category) {


    globalMethod = "";

    UnicodeString lemma = this->nelexLemmatizer.lemmatize(kw,kw_category);

    if(lemma=="") {
        lemma = this->morfGeoLemmatizer.lemmatize(kw,kw_category);
    }else{//
        globalMethod = "DictionaryLemmatizer::NelexiconInfobox";

    }

    if(lemma==""){
        lemma = this->namLivPersonLemmatizer.lemmatize(kw,kw_category);
    }else if(globalMethod!="DictionaryLemmatizer::NelexiconInfobox"){
        globalMethod = "DictionaryLemmatizer::MorfGeo";
    }

    if(lemma==""){
        lemma = this->namLocLemmatizer.lemmatize(kw,kw_category);
    }

    if(lemma==""){
        lemma = this->ruleLemmatizer.lemmatize(kw,kw_category);
    }else if(globalMethod.find("DictionaryLemmatizer")==string::npos
                &&globalMethod.find("NamLivPerson")==string::npos){
        globalMethod = "NamLocLemmatizer::Inflection";
    }


    if(lemma=="") {
        lemma = this->orthLemmatizer.lemmatize(kw);
    }
    if (lemma != "" && globalMethod.empty()) {
        globalMethod = "OrthLemmatizer";
    }


    if (lemma.indexOf("ii") != -1 && lemma.indexOf(" ") == -1) {
        lemma.findAndReplace("ii", "ia");
    }


    if (lemma.indexOf(" ") == -1 && lemma.endsWith("ego")) {
        lemma.findAndReplace("ego", "");
    }

    if (lemma.endsWith("scy") && kw_category.find("nam_liv_person") == string::npos) {
        lemma.findAndReplace("scy", "ski");
    }

    if (lemma.endsWith("ę")) {
        lemma.findAndReplace("ę", "a");
    }

    if (lemma.endsWith("ą")) {
        lemma.findAndReplace("ą", "a");
    }

    if (kw_category == "nam_loc_gpe_city" && lemma.endsWith("u")) {
        string view;
        lemma.toUTF8String(view);
        lemma = lemma.tempSubString(0, lemma.length() - 1);
        cout << "";
    }

    if (kw_category == "nam_liv_god" && lemma.endsWith("em")) {
        lemma.findAndReplace("em", "");
    }

    if (lemma.endsWith("owi")) {
        lemma.findAndReplace("owi", "");
    }

    return lemma;
}
