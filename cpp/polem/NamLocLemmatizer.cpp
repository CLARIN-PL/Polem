/*
   Copyright (C) Wrocław University of Science and Technology (PWr), 2017-2018.
   Grzegorz Kuboń, Michał Marcińczuk.

   Part of Polem project.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
 */

#include "NamLocLemmatizer.h"
#include "CascadeLemmatizer.h"

using namespace std;



NamLocLemmatizer::NamLocLemmatizer(Inflection inflection) : inflection(inflection) {
    this->inflection = inflection;
}

icu::UnicodeString
NamLocLemmatizer::lemmatize(std::vector <std::vector<icu::UnicodeString>> keyword, std::string category, bool debug) {

    if(category.empty()||category.find("nam_loc")!=0){
        return "";
    }
    if (debug) {
        cout << "Entering NamLoc lemmatizer" << endl;
    }
    UnicodeString lemma;
    UnicodeString name;
    //for (vector<vector<UnicodeString> >::iterator it = keyword.begin(); it != keyword.end(); ++it) {
    for (auto& it:keyword){
        lemma = this->inflection.generate_base(it[2], it[0]);
        if(lemma==""){
            if (debug) {
                cout << "Exiting NamLoc lemmatizer" << endl;
            }
            return "";
        }
        if(name!=""){
            name.append(" ");
        }
        name.append(lemma);
    }
    return name;
}



