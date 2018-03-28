/*
   Copyright (C) Wrocław University of Science and Technology (PWr), 2017-2018.
   Grzegorz Kuboń, Michał Marcińczuk.

   Part of Polem project.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
 */

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
    std::vector<std::pair<std::string, boost::shared_ptr<Wccl::FunctionalOperator> > > wccl_operators;
    bool useOrthForOov;

    icu::UnicodeString generate(Corpus2::Sentence::Ptr sentence, std::map<int, std::vector<std::string> > operations,
                                std::vector<icu::UnicodeString> spaces, std::string kw_category);

public:
    RuleLemmatizer(std::string tagset, morfeusz::Morfeusz *generator, bool fix,
                   bool useOrthForOov, std::string datafiles);

    icu::UnicodeString lemmatize(std::vector <std::vector<icu::UnicodeString> > kw, std::string kw_category, bool debug);


};


#endif //LEMATYZATOR_RULELEMMATIZER_H
