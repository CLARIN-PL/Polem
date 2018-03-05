/*
   Copyright (C) Wrocław University of Science and Technology (PWr), 2017-2018.
   Grzegorz Kuboń, Michał Marcińczuk.

   Part of Polem project.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
 */

#ifndef LEMATYZATOR_INFLECTION_H
#define LEMATYZATOR_INFLECTION_H
#include <string>
#include <map>
#include <vector>
#include <set>
#include <unicode/unistr.h>
#include "InflectionRule.h"


class Inflection {
    std::map<UnicodeString,std::map<UnicodeString,std::vector<InflectionRule > > > inflections;
    std::set<icu::UnicodeString> known_bases_set;

    UnicodeString _generate_base(icu::UnicodeString ctag ,icu::UnicodeString form);

public:
    explicit Inflection(std::vector<icu::UnicodeString> known_bases);
    void loadInflectionRules(std::string pathname);
    UnicodeString generate_base(icu::UnicodeString ctag, icu::UnicodeString form);

    static Inflection createForRulesInFile(std::vector<UnicodeString> known_bases, std::string pathname);
};


#endif //LEMATYZATOR_INFLECTION_H
