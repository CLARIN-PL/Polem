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

public:
    CascadeLemmatizer(std::string pathname, Corpus2::Tagset tagset, morfeusz::Morfeusz *generator,
                      std::map<UnicodeString, std::pair<UnicodeString, UnicodeString>> dictionaryItems,
                      Inflection inflection, Inflection inflectionNamLoc);

    icu::UnicodeString lemmatize(std::vector<std::vector<std::string>> kw, std::string kw_category);


};


#endif //LEMATYZATOR_CASCADELEMMATIZER_H
