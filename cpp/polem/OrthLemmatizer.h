//
// Created by gkubon on 07/08/17.
//

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
