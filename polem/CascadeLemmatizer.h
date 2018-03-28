/*
   Copyright (C) Wrocław University of Science and Technology (PWr), 2017-2018.
   Grzegorz Kuboń, Michał Marcińczuk.

   Part of Polem project.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
 */

#ifndef LEMATYZATOR_CASCADELEMMATIZER_H
#define LEMATYZATOR_CASCADELEMMATIZER_H

#include "RuleLemmatizer.h"
#include "DictionaryLemmatizer.h"
#include "NamLivPersonLemmatizer.h"
#include "NamLocLemmatizer.h"
#include "OrthLemmatizer.h"
#include <unicode/uconfig.h>
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

    std::vector<std::vector<UnicodeString> > chopInput(UnicodeString kwrd_orth, UnicodeString kwrd_base,
                                                      UnicodeString kwrd_ctag, UnicodeString kwrd_spaces);

    CascadeLemmatizer(std::string tagset, morfeusz::Morfeusz *generator,
                      std::map<UnicodeString, std::pair<UnicodeString, UnicodeString> > dictionaryItems,
                      Inflection inflection, Inflection inflectionNamLoc, std::string datafiles);

    icu::UnicodeString lemmatizeCascade(std::vector<std::vector<UnicodeString> >, std::string category, bool debug);

public:
    static CascadeLemmatizer assembleLemmatizer();

    static CascadeLemmatizer assembleLemmatizer(std::string datafiles);

    static void addLinesFromFileToVector(std::string path, std::vector<UnicodeString> &dict);

    UnicodeString
    lemmatize(UnicodeString kwrd_orth, UnicodeString kwrd_base, UnicodeString kwrd_ctag, UnicodeString kwrd_spaces,
              std::string category, bool debug);

    UnicodeString
    lemmatize(UnicodeString kwrd_orth, UnicodeString kwrd_base, UnicodeString kwrd_ctag, UnicodeString kwrd_spaces,
              bool debug);

    UnicodeString lemmatize(UnicodeString kwrd_orth, UnicodeString kwrd_base, UnicodeString kwrd_ctag, bool debug);

    std::string
    lemmatizeS(std::string kwrd_orth, std::string kwrd_base, std::string kwrd_ctag, std::string kwrd_spaces,
                std::string category, bool debug);

    std::string
    lemmatizeS(std::string kwrd_orth, std::string kwrd_base, std::string kwrd_ctag, std::string kwrd_spaces, bool debug);

    std::string
    lemmatizeS(std::string kwrd_orth, std::string kwrd_base, std::string kwrd_ctag, bool debug);


};


#endif //LEMATYZATOR_CASCADELEMMATIZER_H
