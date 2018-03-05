/*
   Copyright (C) Wrocław University of Science and Technology (PWr), 2017-2018.
   Grzegorz Kuboń, Michał Marcińczuk.

   Part of Polem project.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
 */

#ifndef LEMATYZATOR_NAMLOCLEMMATIZER_H
#define LEMATYZATOR_NAMLOCLEMMATIZER_H


#include "Inflection.h"

class NamLocLemmatizer {
public:

    explicit NamLocLemmatizer(Inflection inflection);

    icu::UnicodeString
    lemmatize(std::vector <std::vector<icu::UnicodeString>> keyword, std::string category, bool debug);

    Inflection inflection;
};


#endif //LEMATYZATOR_NAMLOCLEMMATIZER_H
