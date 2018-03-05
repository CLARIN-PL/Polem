/*
   Copyright (C) Wrocław University of Science and Technology (PWr), 2017-2018.
   Grzegorz Kuboń, Michał Marcińczuk.

   Part of Polem project.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
 */

#include "Inflection.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <unicode/regex.h>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;

/**
 * Create Inflection object for rules located in a file pathname
 * @param known_bases List of known base forms
 * @param pathname Path to a file with rules
 * @return Inflection object with loaded rules
 */
Inflection Inflection::createForRulesInFile(std::vector<UnicodeString> known_bases, std::string pathname){
    Inflection inflection = Inflection(known_bases);
    inflection.loadInflectionRules(pathname);
    return inflection;
}

Inflection::Inflection(vector<UnicodeString> known_bases){
    for(auto& i:known_bases){
        this->known_bases_set.insert(i.toLower());
    }
}

/**
 * Load inflection patterns represented in the form of: 'ctag form_ending norm_ending count conf'
 * For example:
 * subst:sg:gen:m1 ka ek 53 0.8
 *
 * @param pathname
 */
void Inflection::loadInflectionRules(std::string pathname) {
    ifstream rules(pathname);
    string line;
    while(getline(rules,line)) {
        size_t pos = line.find("  ");
        std::string nline = line.substr(0, pos) + " # " + line.substr(pos+2);
        std::vector<std::string> parts;
        boost::algorithm::split(parts, line, boost::is_any_of(" "));

        if ( parts.size() != 5 ){
            cout << "Niepoprawna linia" << endl;
            continue;
        }

        InflectionRule infrule = InflectionRule();
        infrule.ctag = parts[0].c_str();
        infrule.formEnding = parts[1].c_str();
        infrule.normEnding = parts[2]=="#" ? "" : parts[2].c_str();
        infrule.count = stoi(parts[3]);
        infrule.confidence = stof(parts[4]);
        this->inflections[infrule.ctag][infrule.formEnding].emplace_back(infrule);
    }
    rules.close();
}

/**
 *
 * @param ctag Expected morphological tag of the generated form
 * @param form Base form of the token
 * @return
 */
UnicodeString Inflection::generate_base(icu::UnicodeString ctag, icu::UnicodeString form) {
    UnicodeString base = this->_generate_base(ctag,form);
    if ( base != "" ) {
        return base;
    }

    if(ctag=="subst:sg:gen:m1" && !(base = this->_generate_base("subst:sg:acc:m1",form)).isEmpty()){
        return base;
    }

    if(ctag=="subst:sg:gen:m3" && !(base = this->_generate_base("subst:sg:acc:m3",form)).isEmpty()){
        return base;
    }

    if(ctag=="subst:sg:acc:m1" && !(base = this->_generate_base("subst:sg:gen:m1",form)).isEmpty()){
        return base;
    }

    if(ctag.endsWith(":n") && !(base = this->_generate_base(ctag.findAndReplace(":n",":m1"),form)).isEmpty()){
        return base;
    }

    if(ctag.endsWith(":m3") && !(base = this->_generate_base(ctag.findAndReplace(":m3",":m1"),form)).isEmpty()){
        return base;
    }

    // Generic method for mainly to handle multi-word forms
    if(ctag.indexOf(":nom")==-1 && !(base = this->_generate_base("",form)).isEmpty()){
        return base;
    }

    return base;
}

UnicodeString Inflection::_generate_base(UnicodeString ctagInf, UnicodeString form) {
    UnicodeString forms[10];
    UErrorCode status = U_ZERO_ERROR;
    RegexMatcher m("\\s+", 0, status);
    int tokenCount = m.split(form, forms, 10, status);

    UnicodeString base = "";
    UnicodeString lem;

    if ( tokenCount > 1 ){
    //Lematyzacja frazy wielowyrazowej bez uwzględniania frekwencji alternacji
    /*if (this->inflections.find(ctagInf) != this->inflections.end() && split1 > 1) {
        map<UnicodeString, int> part_candidates;
        for (int i = 0; i < split1; i++) {
            part_candidates.insert(make_pair(forms[i], 0));
        }
        if (ctagInf == "") {
            for (map<UnicodeString, vector<vector<UnicodeString> > >::iterator it = this->inflections.begin();
                 it != this->inflections.end(); ++it) {
                for (vector<vector<UnicodeString> >::iterator it2 = it->second.begin();
                     it2 != it->second.end(); ++it2) {
                    int i = -1;
                    for (int ii = 0; ii < split1; ii++) {
                        i++;
                        if (forms[ii].endsWith(it2->front())) {
                            if (it2->front() == "") {
                                lem = forms[ii];
                            } else {
                                lem = forms[ii].tempSubString(0, forms[ii].length() - it2->front().length());

                            }
                            lem.append((*it2)[1]);
                            string comp;
                            (*it2)[2].toUTF8String(comp);
                            if (part_candidates.find(lem) == part_candidates.end() ||
                                part_candidates.find(lem)->second < stoi(comp)) {
                                part_candidates.find(lem)->second = stoi(comp);
                            }
                        }
                    }
                }
            }
        } else {
            for (vector<vector<UnicodeString> >::iterator it = this->inflections[ctagInf].begin();
                 it != this->inflections[ctagInf].end(); ++it) {
                int i = -1;
                for (int ii = 0; ii < split1; ii++) {
                    i++;
                    if (forms[ii].endsWith(it->front())) {
                        if (it->front() == "") {
                            lem = forms[ii];
                        } else {
                            lem = forms[ii].tempSubString(0, forms[ii].length() - it->front().length());
                        }
                        lem.append((*it)[1]);
                        string comp;
                        (*it)[2].toUTF8String(comp);
                        if (part_candidates.find(lem) == part_candidates.end() ||
                            part_candidates.find(lem)->second < stoi(comp)) {
                            part_candidates.find(lem)->second = stoi(comp);
                        }
                    }
                }
            }

        }
        vector<UnicodeString> candidates;
        for (int i = 0; i < part_candidates.size(); ++i) {
            vector<UnicodeString> new_candidates;
            for (map<UnicodeString, int>::iterator it = part_candidates.begin(); it != part_candidates.end(); ++it) {
                for (vector<UnicodeString>::iterator it2 = candidates.begin(); it2 != candidates.end(); ++it2) {
                    new_candidates.push_back((*it2) + " " + it->first);
                }
            }
            candidates = new_candidates;
        }
        for (vector<UnicodeString>::iterator it = candidates.begin(); it != candidates.end(); ++it) {
            if (find(this->known_bases.begin(), this->known_bases.end(), it->toLower()) != this->known_bases.end()) {
                return it->trim();
            }
        }

        return "";
        */
    } else if (this->inflections.find(ctagInf) != this->inflections.end() ) {
        /** Lematyzacja pojedynczego słowa z wyborem tej alternacji, która jest najbardziej liczna */
        int maxCount = 0;
        int maxEndingLength = 0;
        UnicodeString possible_form;
        for(auto& its : this->inflections[ctagInf]){
            if (form.endsWith(its.first)) {
                for (auto& it : its.second){
                    possible_form = it.inflectForm(form);
                    UnicodeString lower = possible_form;
                    lower.toLower();
                    if (this->known_bases_set.find(lower) != this->known_bases_set.end()
                        && (it.formEnding.length() > maxEndingLength || (it.formEnding.length() == maxEndingLength && it.count > maxCount))) {
                        maxCount = it.count;
                        maxEndingLength = it.formEnding.length();
                        base = possible_form;
                    }
                }
            }
        }
        if (base.isEmpty()) {
            maxCount = 0;
            for (auto &its:this->inflections[ctagInf]) {
                if (form.endsWith(its.first)) {
                    for (auto &it : its.second) {
                        if (it.count > 30 && it.formEnding.length() > 1 && it.count > maxCount) {
                            maxCount = it.count;
                            base = it.inflectForm(form);
                        }
                    }
                }
            }
        }
    } else if (ctagInf == "" ) {
        // ToDo: Consider if this is worth keeping
//        // Generowanie dla pustego tagu morfologicznego
//        int maxCount = 0;
//        for(auto& itc:this->inflections ){
//            for (auto& its : itc.second){
//                if (form.endsWith(its.first)) {
//                    for (auto &it : its.second) {
//                        if (it.count > 30 && it.formEnding.length() > 1 && it.count > maxCount) {
//                            base = it.inflectForm(form);
//                            maxCount = it.count;
//                        }
//                    }
//                }
//            }
//        }
    }
    return base;
}
