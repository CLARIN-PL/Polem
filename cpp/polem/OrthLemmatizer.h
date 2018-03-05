/*
   Copyright (C) Wrocław University of Science and Technology (PWr), 2017-2018.
   Grzegorz Kuboń, Michał Marcińczuk.

   Part of Polem project.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
 */

#ifndef LEMATYZATOR_ORTHLEMMATIZER_H
#define LEMATYZATOR_ORTHLEMMATIZER_H
#include <vector>
#include <string>
#include <unicode/unistr.h>


class OrthLemmatizer {
public:
    icu::UnicodeString lemmatize(std::vector<std::vector<icu::UnicodeString> > keyword);
};


#endif //LEMATYZATOR_ORTHLEMMATIZER_H
