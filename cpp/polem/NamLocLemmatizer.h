//
// Created by gkubon on 04/08/17.
//

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
