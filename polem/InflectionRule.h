/*
   Copyright (C) Wrocław University of Science and Technology (PWr), 2017-2018.
   Grzegorz Kuboń, Michał Marcińczuk.

   Part of Polem project.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
 */

#ifndef LEMATYZATOR_INFLECTION_RULE_H
#define LEMATYZATOR_INFLECTION_RULE_H
#include <string>
#include <map>
#include <vector>
#include <set>
#include <unicode/unistr.h>


class InflectionRule {

public:
    icu::UnicodeString ctag;
    icu::UnicodeString formEnding;
    icu::UnicodeString normEnding;
    std::size_t count;
    double confidence;

    icu::UnicodeString inflectForm(icu::UnicodeString form);
};


#endif //LEMATYZATOR_INFLECTION_RULE_H
