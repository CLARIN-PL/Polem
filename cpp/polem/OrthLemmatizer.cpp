/*
   Copyright (C) Wrocław University of Science and Technology (PWr), 2017-2018.
   Grzegorz Kuboń, Michał Marcińczuk.

   Part of Polem project.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
 */

#include <iostream>
#include "OrthLemmatizer.h"

using namespace std;

icu::UnicodeString OrthLemmatizer::lemmatize(std::vector<std::vector<icu::UnicodeString> > keyword) {
    UnicodeString lemma;
    for(auto& i:keyword){
        lemma.append(i[0]);
        if(i[3]=="True"){
            lemma.append(' ');
        }
    }
    return lemma.trim();

}
