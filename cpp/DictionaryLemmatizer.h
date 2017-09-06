//
// Created by gkubon on 31/07/17.
//

#ifndef LEMATYZATOR_DICTIONARYLEMMATIZER_H
#define LEMATYZATOR_DICTIONARYLEMMATIZER_H
#include <string>
#include <unicode/unistr.h>
#include <map>
#include <vector>


class DictionaryLemmatizer {

    std::map<UnicodeString,std::pair<UnicodeString,UnicodeString> > dictionaryItems;

public:
    DictionaryLemmatizer(std::string name, bool typed);

    icu::UnicodeString lemmatize(std::vector<std::vector<std::string> > keyword, std::string category);

};


#endif //LEMATYZATOR_DICTIONARYLEMMATIZER_H
