//
// Created by gkubon on 18/07/17.
//

#ifndef LEMATYZATOR_CASCADELEMMATIZER_H
#define LEMATYZATOR_CASCADELEMMATIZER_H

#include "RuleLemmatizer.h"
#include "DictionaryLemmatizer.h"
#include "NamLivPersonLemmatizer.h"
#include "NamLocLemmatizer.h"
#include "OrthLemmatizer.h"
#include <string>
#include <vector>


extern std::string globalMethod;

class CascadeLemmatizer{


    RuleLemmatizer ruleLemmatizer;
    DictionaryLemmatizer morfGeoLemmatizer;
    DictionaryLemmatizer nelexLemmatizer;
    NamLivPersonLemmatizer namLivPersonLemmatizer;
    NamLocLemmatizer namLocLemmatizer;
    OrthLemmatizer orthLemmatizer;

    UnicodeString preprocessOrth(UnicodeString orth);

    std::vector<std::vector<UnicodeString>> chopInput(UnicodeString kwrd_orth, UnicodeString kwrd_base,
                                                      UnicodeString kwrd_ctag, UnicodeString kwrd_spaces);

    UnicodeString foldOutput(UnicodeString lemma, std::vector<std::vector<UnicodeString>> kw, std::string kw_category);

    UnicodeString filter(std::vector<std::vector<UnicodeString>> kw, UnicodeString lemma, std::string kw_category);

public:
    CascadeLemmatizer(std::string pathname, Corpus2::Tagset tagset, morfeusz::Morfeusz *generator,
                      std::map<UnicodeString, std::pair<UnicodeString, UnicodeString>> dictionaryItems,
                      Inflection inflection, Inflection inflectionNamLoc);

    UnicodeString
    lemmatize(UnicodeString kwrd_orth, UnicodeString kwrd_base, UnicodeString kwrd_ctag, UnicodeString kwrd_spaces,
              std::string category);


};


#endif //LEMATYZATOR_CASCADELEMMATIZER_H
