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

    std::map<UnicodeString, UnicodeString> dictionaryItems;
    bool typed;
public:
    DictionaryLemmatizer(std::string name, bool typed);

    icu::UnicodeString
    lemmatize(std::vector <std::vector<icu::UnicodeString>> keyword, std::string category, bool debug);

};


#endif //LEMATYZATOR_DICTIONARYLEMMATIZER_H
