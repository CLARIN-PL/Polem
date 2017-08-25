//
// Created by gkubon on 04/08/17.
//

#ifndef LEMATYZATOR_NAMLOCLEMMATIZER_H
#define LEMATYZATOR_NAMLOCLEMMATIZER_H


#include "Inflection.h"

class NamLocLemmatizer {
public:

    NamLocLemmatizer(Inflection inflection);

    icu::UnicodeString lemmatize(std::vector<std::vector<std::string> > keyword, std::string category);

    Inflection inflection;

    UnicodeString lemmatize(UnicodeString *keyword, UnicodeString category, int numwords);
};


#endif //LEMATYZATOR_NAMLOCLEMMATIZER_H
