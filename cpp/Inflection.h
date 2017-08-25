//
// Created by gkubon on 01/08/17.
//

#ifndef LEMATYZATOR_INFLECTION_H
#define LEMATYZATOR_INFLECTION_H
#include <string>
#include <map>
#include <vector>
#include <unicode/unistr.h>


class Inflection {
    std::map<UnicodeString,std::vector<std::vector<UnicodeString> > > inflections;
    std::vector<icu::UnicodeString> known_bases;


    UnicodeString _generate_base(icu::UnicodeString ctag ,icu::UnicodeString form);

public:
    explicit Inflection(std::vector<icu::UnicodeString> known_bases);
    void loadInflectionRules(std::string pathname);
    UnicodeString generate_base(icu::UnicodeString ctag, icu::UnicodeString form);

    static std::vector<icu::UnicodeString> split(icu::UnicodeString to_split, icu::UnicodeString delimiter);
};


#endif //LEMATYZATOR_INFLECTION_H
