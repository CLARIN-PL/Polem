//
// Created by gkubon on 18/07/17.
//

#include "CascadeLemmatizer.h"
#include <unicode/regex.h>
#include <fstream>


using namespace std;


string globalMethod;


CascadeLemmatizer CascadeLemmatizer::assembleLemmatizer(std::string datafiles) {

    string line;
    vector<UnicodeString> vecLastNames;

    ifstream lastNames(datafiles+"nelexicon2_nam_liv_person_last.txt");
    while (getline(lastNames, line)) {
        vecLastNames.emplace_back(line.substr(line.find('\t') + 1).c_str());
    }
    lastNames.close();
    //loading dictionaries to namlivlemmatizer

    ifstream firstNames(datafiles+"nelexicon2_nam_liv_person_first.txt");
    while (getline(firstNames, line)) {
        vecLastNames.emplace_back(line.substr(line.find('\t') + 1).c_str());
    }
    firstNames.close();

    vector<UnicodeString> vecNamLoc;

    ifstream namLoc(datafiles+"nelexicon2-infobox-nam_loc.txt");
    while (getline(namLoc, line)) {
        vecNamLoc.emplace_back(line.substr(line.find('\t') + 1).c_str());
    }
    namLoc.close();
    //loading dictionary for namloclemmatizer

    ifstream dictFile(datafiles+"nelexicon2_wikipedia-infobox-forms-with-bases-filtered.txt");

    map<UnicodeString, pair<UnicodeString, UnicodeString> > dictionaryItems;

    while (getline(dictFile, line)) {
        UnicodeString dictCat, dictOrth, dictLemma;
        dictCat = line.substr(0, line.find('\t')).c_str();
        dictOrth = line.substr(line.find('\t') + 1).substr(0,
                                                           line.substr(line.find('\t') + 1).find_last_of('\t')).c_str();
        dictLemma = line.substr(line.rfind('\t') + 1).c_str();
        dictionaryItems[dictOrth] = make_pair(dictCat, dictLemma);

    }
    dictFile.close();
    //loading dict for dictionary lemmatizers
    //dictionary items - key - unicode string value - pair < uni string, uni string >
    //orth, pair < category , lemma >

    Inflection inflection = Inflection(vecLastNames);
    inflection.loadInflectionRules(datafiles+"inflection_nam_liv_person_last.txt");
    Inflection inflectionNamLoc = Inflection(vecNamLoc);
    inflectionNamLoc.loadInflectionRules(datafiles+"inflection_nam_loc.txt");
    //loading rules for inflection


    morfeusz::Morfeusz *generator = morfeusz::Morfeusz::createInstance(morfeusz::GENERATE_ONLY);


    return CascadeLemmatizer("nkjp", generator, dictionaryItems, inflection,
                             inflectionNamLoc,datafiles);

}

CascadeLemmatizer CascadeLemmatizer::assembleLemmatizer() {
    return CascadeLemmatizer::assembleLemmatizer("/usr/local/share/polem/");
}

vector<vector<UnicodeString>>
CascadeLemmatizer::chopInput(UnicodeString kwrd_orth, UnicodeString kwrd_base, UnicodeString kwrd_ctag,
                             UnicodeString kwrd_spaces) {

    //kwrd_orth = preprocessOrth(kwrd_orth);

    UErrorCode status = U_ZERO_ERROR;

    RegexMatcher rm("\\s", 0, status);
    UnicodeString check[50];
    int numWords = rm.split(kwrd_orth, check, 50, status);

    UnicodeString bases[numWords];
    rm.split(kwrd_base, bases, numWords, status);
    UnicodeString orths[numWords];
    rm.split(kwrd_orth, orths, numWords, status);
    UnicodeString ctags[numWords];
    rm.split(kwrd_ctag, ctags, numWords, status);
    UnicodeString spaces[numWords];
    if (kwrd_spaces != "") {
        rm.split(kwrd_spaces, spaces, numWords, status);
    } else {
        for (int i = 0; i < numWords; ++i) {
            spaces[i] = "True";
        }
    }
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


CascadeLemmatizer::CascadeLemmatizer(string tagset, morfeusz::Morfeusz *generator,
                                     map<UnicodeString,
                                             pair<UnicodeString, UnicodeString> > dictionaryItems,
                                     Inflection inflection, Inflection inflectionNamLoc, string datafiles) :
        nelexLemmatizer(datafiles+"nelexicon2_wikipedia-infobox-forms-with-bases-filtered.txt",true),
        ruleLemmatizer(std::move(tagset), generator, true, true,datafiles),
        morfGeoLemmatizer(datafiles+"sgjp-20160310-geograficzne.tab", false),
        namLivPersonLemmatizer(std::move(dictionaryItems), std::move(inflection)),
        namLocLemmatizer(std::move(inflectionNamLoc)),
        orthLemmatizer(){

//constructor launches different lemmatizers constructors

}

UnicodeString
CascadeLemmatizer::lemmatize(UnicodeString kwrd_orth, UnicodeString kwrd_base, UnicodeString kwrd_ctag,
                             UnicodeString kwrd_spaces, std::string kw_category, bool debug) {

    vector<vector<UnicodeString>> kw = CascadeLemmatizer::chopInput(kwrd_orth, kwrd_base, kwrd_ctag, kwrd_spaces);
    //processing keyword into input to lemmatizer as
    //orth1, base1, tag1, space1  <- vector
    //orth2, base2, tag2, space2 and so on

    globalMethod = "";
    //indicates what is used to lemmatize

    //launching lemmatizers in order nelex - morfgeo - namliv - namloc - rule - orth
    UnicodeString lemma = this->nelexLemmatizer.lemmatize(kw, kw_category, debug);

    if(lemma=="") {
        lemma = this->morfGeoLemmatizer.lemmatize(kw, kw_category, debug);
    }else{//
        globalMethod = "DictionaryLemmatizer:NelexiconInfobox";

    }

    if(lemma==""){
        lemma = this->namLivPersonLemmatizer.lemmatize(kw, kw_category, debug);
    } else if (globalMethod != "DictionaryLemmatizer:NelexiconInfobox") {
        globalMethod = "DictionaryLemmatizer:MorfeuszGeograficzne";
    }

    if (debug) {
        cout << "Exited NamLivPerson lemmatizer with " << globalMethod << endl;
    }

    if(lemma==""){
        lemma = this->namLocLemmatizer.lemmatize(kw, kw_category, debug);
    }

    if(lemma==""){
        lemma = this->ruleLemmatizer.lemmatize(kw, kw_category, debug);
    }else if(globalMethod.find("DictionaryLemmatizer")==string::npos
                &&globalMethod.find("NamLivPerson")==string::npos){
        globalMethod = "NamLocLemmatizer:Inflection";
        if (debug) {
            cout << "Exited NamLoc lemmatizer with " << globalMethod << endl;
        }
    }

    if (debug) {
        cout << "Exiting Rule lemmatizer with" << globalMethod << endl;
    }

    if(lemma=="") {
        lemma = this->orthLemmatizer.lemmatize(kw);
    }
    if (lemma != "" && globalMethod.empty()) {
        globalMethod = "OrthLemmatizer";
    }

    if (kw.size() == 1 && lemma != "" && !kw_category.empty() &&
        (kw_category.find("nam_adj") == 0 || (kw_category == "nam_loc_gpe_admin1" && kw[0][2].startsWith("adj:")))) {
        lemma.toLower();
    }

//    string err;
//    lemma.toUTF8String(err);
//    lemma = foldOutput(lemma, kw, kw_category);

    //lemma = filter(kw, lemma, kw_category);
    //filter the results, adding few % points

    return lemma.trim();
}

UnicodeString CascadeLemmatizer::lemmatize(UnicodeString kwrd_orth, UnicodeString kwrd_base, UnicodeString kwrd_ctag,
                                           UnicodeString kwrd_spaces, bool debug) {
    return this->lemmatize(kwrd_orth, kwrd_base, kwrd_ctag, kwrd_spaces, "", debug);
}

UnicodeString
CascadeLemmatizer::lemmatize(UnicodeString kwrd_orth, UnicodeString kwrd_base, UnicodeString kwrd_ctag, bool debug) {

    return this->lemmatize(kwrd_orth, kwrd_base, kwrd_ctag, "", "", debug);
}

std::string CascadeLemmatizer::lemmatizeS(std::string kwrd_orth, std::string kwrd_base, std::string kwrd_ctag,
                                            std::string kwrd_spaces, std::string category) {
    std::string out;
    UnicodeString proc = this->lemmatize(kwrd_orth.c_str(), kwrd_base.c_str(), kwrd_ctag.c_str(), kwrd_spaces.c_str(),
                                         category, false);
    proc.toUTF8String(out);
    return out;
}

std::string CascadeLemmatizer::lemmatizeS(std::string kwrd_orth, std::string kwrd_base, std::string kwrd_ctag,
                                            std::string kwrd_spaces) {
    std::string out;
    UnicodeString proc = this->lemmatize(kwrd_orth.c_str(), kwrd_base.c_str(), kwrd_ctag.c_str(), kwrd_spaces.c_str(),
                                         false);
    proc.toUTF8String(out);
    return out;
}

std::string CascadeLemmatizer::lemmatizeS(std::string kwrd_orth, std::string kwrd_base, std::string kwrd_ctag) {
    std::string out;
    UnicodeString proc = this->lemmatize(kwrd_orth.c_str(), kwrd_base.c_str(), kwrd_ctag.c_str(), false);
    proc.toUTF8String(out);
    return out;
}


