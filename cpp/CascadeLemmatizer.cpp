//
// Created by gkubon on 18/07/17.
//

#include "CascadeLemmatizer.h"
#include <armadillo>
#include <unicode/regex.h>


using namespace std;


string globalMethod = "";

icu::UnicodeString
CascadeLemmatizer::filter(std::vector<std::vector<icu::UnicodeString>> kw, icu::UnicodeString lemma,
                          std::string kw_category) {


    lemma = lemma.trim();
    if (globalMethod == "NamLivPersonLemmatizer::Dictionary" &&
        (lemma.indexOf("Teofil") != -1 || lemma.indexOf("Jan") != -1
         || lemma.indexOf("teofil") != -1 || lemma.indexOf("jan") != -1)) {
        for (auto it:kw) {
            if (it[0].indexOf("J") == 0 && it[2].indexOf(":m") != -1 &&
                (lemma.indexOf("teofil") != -1 || lemma.indexOf("Teofil") != -1)) {
                lemma.findAndReplace("Teofil", "Józef");
                lemma.findAndReplace("teofil", "Józef");
                break;
            }
            if (it[0].indexOf("Michał") != -1 && it[2].indexOf(":m") != -1
                && (lemma.indexOf("jan") != -1 || lemma.indexOf("Jan") != -1)) {
                lemma.findAndReplace("Jan", "Michał");
                lemma.findAndReplace("jan", "Michał");
                break;
            }
        }
    }
    //handling current dictionary mistakes


    if (lemma.endsWith("ii") && lemma.indexOf(" ") == -1) {
        lemma.findAndReplace("ii", "ia");
    } else if (lemma.indexOf(" ") == -1 && lemma.endsWith("ego")) {
        lemma.findAndReplace("ego", "");
    } else if (lemma.endsWith("scy") && kw_category.find("nam_liv_person") == std::string::npos) {
        lemma.findAndReplace("scy", "ski");
    } else if (lemma.endsWith("ę")) {
        lemma.findAndReplace("ę", "a");
    } else if (lemma.endsWith("ą") || (lemma.indexOf("ą ") != -1 && kw.size() < 5)) {
        lemma.findAndReplace("ą", "a");
    } else if (kw_category.find("nam_loc") == 0 && lemma.endsWith("u")) {
        lemma.extractBetween(0, lemma.length() - 1, lemma);
    } else if (lemma.endsWith("owi")) {
        lemma.findAndReplace("owi", "");
    } else if (lemma.endsWith("em")) {
        lemma.findAndReplace("em", "");
    }
    lemma.findAndReplace(" 's", "'s ");
    if (kw_category == "nam_pro_media_web" &&
        (lemma.indexOf(".pl") != -1 || lemma.indexOf(".com") != -1 || lemma.indexOf(".org") != -1 ||
         lemma.indexOf(".us") != -1)) {
        lemma.findAndReplace(" ", "");
    }
    if (lemma.indexOf(".pl"))

        if (lemma.indexOf("'") + 3 > lemma.length() && lemma.indexOf("'") != -1) {
            lemma.extractBetween(0, lemma.indexOf("'"), lemma);
        }

    if (lemma.indexOf("’") + 3 > lemma.length() && lemma.indexOf("’") != -1) {
        lemma.extractBetween(0, lemma.indexOf("’"), lemma);
    }
    //handling some of phrases that wasnt lemmatized correctly

    while (lemma.indexOf("  ") != -1) {
        lemma.findAndReplace("  ", " ");
    }

    return lemma.trim();

}

UnicodeString CascadeLemmatizer::preprocessOrth(UnicodeString orth) {

    //adding whitespaces between tokens because
    //each special char is a token

    orth.findAndReplace("-", " - ");
    orth.findAndReplace(".", " . ");
    orth.findAndReplace(",", " , ");
    orth.findAndReplace("?", " ? ");
    orth.findAndReplace("!", " ! ");
    //orth.findAndReplace("'", " ' ");
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

    return orth.trim();

}

vector<vector<UnicodeString>>
CascadeLemmatizer::chopInput(UnicodeString kwrd_orth, UnicodeString kwrd_base, UnicodeString kwrd_ctag,
                             UnicodeString kwrd_spaces) {

    //processing input line into values

    kwrd_orth = preprocessOrth(kwrd_orth);

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

UnicodeString CascadeLemmatizer::foldOutput(UnicodeString lemma, vector<vector<UnicodeString>> kw, string kw_category) {

    lemma = preprocessOrth(lemma);

    //reorganizing lemma to get spaces right according to input
    //&&
    //handling lemma case based on input orths

    string view;
    lemma.toUTF8String(view);

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

        int ii = 0;
        for (ii = 0; ii < outlemmas[i].length(); ++ii) {

            wchar_t inlowchar, outlowchar;
            UnicodeString xyz = kw[i][0];
            inlowchar = xyz.toLower().charAt(ii);
            outlowchar = outlemmas[i].toLower().charAt(ii);

            if (kw_category.find("nam_adj") == 0) {
                lemma.append(outlemmas[i].charAt(ii));
            } else if (kw[i][0].charAt(ii) == outlemmas[i].charAt(ii)) {
                lemma.append(outlemmas[i].charAt(ii));
            } else if (outlowchar == inlowchar && kw_category.find("nam_adj") != 0) {
                lemma.append(kw[i][0].charAt(ii));
            } else {
                lemma.append(outlemmas[i].charAt(ii));
            }
        }

        if (kw[i][3] == "True") {
            lemma.append(" ");
        }
    }

    return lemma.trim();

}

CascadeLemmatizer::CascadeLemmatizer(string pathname, Corpus2::Tagset tagset, morfeusz::Morfeusz* generator,map<UnicodeString,
        pair<UnicodeString, UnicodeString> > dictionaryItems,Inflection inflection,Inflection inflectionNamLoc):
        nelexLemmatizer("nelexicon2_wikipedia-infobox-forms-with-bases-filtered.txt",true),
        ruleLemmatizer(std::move(pathname), std::move(tagset), generator, true, true),
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

    lemma = foldOutput(lemma, kw, kw_category);

    lemma = filter(kw, lemma, kw_category);
    //filter the results, adding few % points

    return lemma.trim();
}
