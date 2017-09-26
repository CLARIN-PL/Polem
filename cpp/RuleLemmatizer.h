//
// Created by gkubon on 19/07/17.
//

#ifndef LEMATYZATOR_RULELEMMATIZER_H
#define LEMATYZATOR_RULELEMMATIZER_H

#include <map>
#include <vector>
#include "string"
#include <libcorpus2/tagset.h>
#include <libcorpus2/tagsetmanager.h>
#include <libcorpus2/sentence.h>
#include <morfeusz2.h>
#include <libwccl/ops/operator.h>


class RuleLemmatizer {

    Corpus2::Tagset tagset;
    morfeusz::Morfeusz *generator;
    std::map< std::string, std::string > rule_categories;
    std::vector<std::pair<std::string, boost::shared_ptr<Wccl::FunctionalOperator>>> wccl_operators;
    bool useOrthForOov;

    double evaluateSharedTag(const std::string &basic_string, std::string basicString);

    icu::UnicodeString generate(Corpus2::Sentence::Ptr sentence, std::map<int, std::vector<std::string> > operations,
                                std::vector<icu::UnicodeString> spaces, std::string kw_category);

public:
    RuleLemmatizer(std::string rulespathname, Corpus2::Tagset tagset, morfeusz::Morfeusz *generator, bool fix,
                   bool useOrthForOov);

    icu::UnicodeString lemmatize(std::vector<std::vector<icu::UnicodeString>> kw, std::string kw_category);


};


#endif //LEMATYZATOR_RULELEMMATIZER_H
