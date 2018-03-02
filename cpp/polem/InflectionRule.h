#ifndef LEMATYZATOR_INFLECTION_RULE_H
#define LEMATYZATOR_INFLECTION_RULE_H
#include <string>
#include <map>
#include <vector>
#include <set>
#include <unicode/unistr.h>


class InflectionRule {

public:
    icu::UnicodeString ctag;
    icu::UnicodeString formEnding;
    icu::UnicodeString normEnding;
    std::size_t count;
    double confidence;
};


#endif //LEMATYZATOR_INFLECTION_RULE_H
