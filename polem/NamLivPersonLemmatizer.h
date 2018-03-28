/*
   Copyright (C) Wrocław University of Science and Technology (PWr), 2017-2018.
   Grzegorz Kuboń, Michał Marcińczuk.

   Part of Polem project.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
 */

#ifndef LEMATYZATOR_NAMLIVPERSONLEMMATIZER_H
#define LEMATYZATOR_NAMLIVPERSONLEMMATIZER_H

#include <string>
#include <vector>
#include <map>
#include <unicode/unistr.h>
#include "Inflection.h"

class NamLivPersonLemmatizer {

    std::vector<icu::UnicodeString> categories;
    std::vector<icu::UnicodeString> copyOrths;
    std::vector<icu::UnicodeString> forceSingleBases;
    std::vector<icu::UnicodeString> inflectionSeparators;
    std::vector<icu::UnicodeString> inflectionEndings;
    std::map<UnicodeString, std::vector<UnicodeString> > names;
    std::map<UnicodeString, UnicodeString> fullnames;


public:
    NamLivPersonLemmatizer(std::map<icu::UnicodeString,std::pair<icu::UnicodeString,icu::UnicodeString> > dictionaryItems,Inflection inflection);

    icu::UnicodeString
    lemmatize(std::vector <std::vector<icu::UnicodeString> > keyword, std::string category, bool debug);
    Inflection inflection;// = Inflection(std::vector<UnicodeString>());

};


#endif //LEMATYZATOR_NAMLIVPERSONLEMMATIZER_H
