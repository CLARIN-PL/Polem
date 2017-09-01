//
// Created by gkubon on 01/09/17.
//

#ifndef POLEM_HANDLER_H
#define POLEM_HANDLER_H

#include <unicode/unistr.h>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <libcorpus2/sentence.h>

class Handler {

public:
    static icu::UnicodeString
    filter(std::vector<std::vector<std::string>> kw, icu::UnicodeString lemma, std::string kw_category);

    static UnicodeString
    sensitivityModule(std::vector<UnicodeString> lemmas, boost::shared_ptr<Corpus2::Sentence> sentence,
                      std::string kw_category);
};


#endif //POLEM_HANDLER_H
