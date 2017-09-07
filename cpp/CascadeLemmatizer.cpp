//
// Created by gkubon on 18/07/17.
//

#include "CascadeLemmatizer.h"
#include <armadillo>
#include <unicode/regex.h>


using namespace std;


string globalMethod = "";

UnicodeString CascadeLemmatizer::preprocessOrth(UnicodeString orth) {
    orth.findAndReplace("-", " - ");
    orth.findAndReplace(".", " . ");
    orth.findAndReplace(",", " , ");
    orth.findAndReplace("?", " ? ");
    orth.findAndReplace("!", " ! ");
    orth.findAndReplace("'", " ' ");
    orth.findAndReplace("’", " ’ ");
    orth.findAndReplace("„", " „ ");
    orth.findAndReplace("”", " ” ");
    orth.findAndReplace("(", " ( ");
    orth.findAndReplace(")", " ) ");
    orth.findAndReplace(":", " : ");
    orth.findAndReplace("@", " @ ");
    orth.findAndReplace("/", " / ");
    orth.findAndReplace("\"", " \" ");
    orth.findAndReplace("–", " – ");
    orth.findAndReplace("´", " ´ ");
    orth.findAndReplace("®", " ® ");
    orth.findAndReplace("+", " + ");
    orth.findAndReplace("_", " _ ");
    orth.findAndReplace("#", " # ");

    while (orth.indexOf("  ") != -1) orth.findAndReplace("  ", " ");
    orth.trim();

    return orth;

}

vector<vector<UnicodeString>>
CascadeLemmatizer::chopInput(UnicodeString kwrd_orth, UnicodeString kwrd_base, UnicodeString kwrd_ctag,
                             UnicodeString kwrd_spaces) {

    kwrd_orth = preprocessOrth(kwrd_orth);
    //kwrd_base = preprocessBase(kwrd_base);
    string view;
    kwrd_orth.toUTF8String(view);

    UErrorCode status = U_ZERO_ERROR;

    RegexMatcher rm("\\s", 0, status);
    UnicodeString check[50];
    int numWords = rm.split(kwrd_ctag, check, 50, status);

    UnicodeString bases[numWords];
    rm.split(kwrd_base, bases, numWords, status);
    UnicodeString orths[numWords];
    rm.split(kwrd_orth, orths, numWords, status);
    UnicodeString ctags[numWords];
    rm.split(kwrd_ctag, ctags, numWords, status);
    UnicodeString spaces[numWords];
    rm.split(kwrd_spaces, spaces, numWords, status);
    //multiword orth

    vector<vector<UnicodeString> > kw;
    for (int i = 0; i < numWords; i++) {
        vector<UnicodeString> row;
        row.emplace_back(orths[i]);
        row.emplace_back(bases[i]);
        row.emplace_back(ctags[i]);
        row.emplace_back(spaces[i]);
        kw.emplace_back(row);
    }

    return kw;

}

UnicodeString CascadeLemmatizer::foldOutput(UnicodeString lemma, vector<vector<UnicodeString>> kw) {
    lemma = preprocessOrth(lemma);

    UErrorCode status = U_ZERO_ERROR;
    RegexMatcher rm("\\s", 0, status);
    UnicodeString check[50];
    int numWords = rm.split(lemma, check, 50, status);
    UnicodeString outlemmas[numWords];
    rm.split(lemma, outlemmas, numWords, status);

    lemma = "";
    for (int i = 0; i < numWords; ++i) {
        if (i >= kw.size()) {
            lemma.trim();
            lemma.append(outlemmas[i].findAndReplace(" ", ""));
            continue;
        }
        string temp;
        kw[i][3].toUTF8String(temp);
        if (temp == "True") {
            lemma.append(outlemmas[i].trim() + " ");
        } else if (temp == "False") {
            lemma.append(outlemmas[i].trim());
        }
    }
    return lemma.trim();

}

CascadeLemmatizer::CascadeLemmatizer(string pathname, Corpus2::Tagset tagset, morfeusz::Morfeusz* generator,map<UnicodeString,
        pair<UnicodeString, UnicodeString> > dictionaryItems,Inflection inflection,Inflection inflectionNamLoc):
            nelexLemmatizer("nelexicon2_wikipedia-infobox-forms-with-bases-filtered.txt",true),
                ruleLemmatizer(std::move(pathname), std::move(tagset),generator,true, true),
                    morfGeoLemmatizer("sgjp-20160310-geograficzne.tab", false),
                        namLivPersonLemmatizer(std::move(dictionaryItems), std::move(inflection)),
                            namLocLemmatizer(std::move(inflectionNamLoc)),
                                orthLemmatizer(){

//constructor launches different lemmatizers constructors

}

UnicodeString
CascadeLemmatizer::lemmatize(UnicodeString kwrd_orth, UnicodeString kwrd_base, UnicodeString kwrd_ctag,
                             UnicodeString kwrd_spaces,
                             std::string kw_category) {

    vector<vector<UnicodeString>> kw = CascadeLemmatizer::chopInput(kwrd_orth, kwrd_base, kwrd_ctag, kwrd_spaces);
    //processing keyword into input to lemmatizer as
    //orth1, base1, tag1, space1  <- vector
    //orth2, base2, tag2, space2 and so on

    globalMethod = "";
    //indicates what is used to lemmatize

    //launching lemmatizers in order morfgeo - nelex - namliv - namloc - rule - orth
    UnicodeString lemma = this->morfGeoLemmatizer.lemmatize(kw, kw_category);

    if(lemma=="") {
        lemma = this->nelexLemmatizer.lemmatize(kw, kw_category);
    }else{//
        globalMethod = "DictionaryLemmatizer::MorfGeo";

    }

    if(lemma==""){
        lemma = this->namLivPersonLemmatizer.lemmatize(kw,kw_category);
    } else if (globalMethod != "DictionaryLemmatizer::MorfGeo") {
        globalMethod = "DictionaryLemmatizer::NelexiconInfobox";
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


    lemma = foldOutput(lemma, kw);

    lemma = Handler::filter(kw, lemma, kw_category);
    //filter the results, adding few % points


    return lemma.trim();
}
