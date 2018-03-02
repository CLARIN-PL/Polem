#ifndef LEMATYZATOR_INFLECTION_H
#define LEMATYZATOR_INFLECTION_H
#include <string>
#include <map>
#include <vector>
#include <set>
#include <unicode/unistr.h>
#include "InflectionRule.h"


class Inflection {
    std::map<UnicodeString,std::vector<InflectionRule > > inflections;
    std::set<icu::UnicodeString> known_bases_set;

    UnicodeString _generate_base(icu::UnicodeString ctag ,icu::UnicodeString form);

public:
    explicit Inflection(std::vector<icu::UnicodeString> known_bases);
    void loadInflectionRules(std::string pathname);
    UnicodeString generate_base(icu::UnicodeString ctag, icu::UnicodeString form);

    static Inflection createForRulesInFile(std::vector<UnicodeString> known_bases, std::string pathname);
};


#endif //LEMATYZATOR_INFLECTION_H
