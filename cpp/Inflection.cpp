//
// Created by gkubon on 01/08/17.
//

#include "Inflection.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <unicode/regex.h>

using namespace std;


Inflection::Inflection(vector<UnicodeString> known_bases){

    UnicodeString tmp;
    for(int i = 0; i < known_bases.size(); ++i){
        tmp = known_bases[i].toLower();
        this->known_bases.push_back(tmp);

    }
}


UnicodeString Inflection::_generate_base(UnicodeString ctagInf, UnicodeString form) {



    UnicodeString forms[10];
    UErrorCode status =  U_ZERO_ERROR;
    RegexMatcher m("\\s+", 0, status);
    int split1 = m.split(form, forms, 10, status);


    UnicodeString base ="";
    UnicodeString lem;

    if (this->inflections.find(ctagInf) != this->inflections.end() && split1 > 1) {
        map<UnicodeString, int> part_candidates;
        for (int i = 0; i < split1; i++) {
            part_candidates.insert(make_pair(forms[i], 0));
        }
        if (ctagInf == "") {
            for(map<UnicodeString,vector<vector<UnicodeString> > >::iterator it = this->inflections.begin(); it != this->inflections.end(); ++it) {
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
        }else{
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
        for(int i = 0; i < part_candidates.size(); ++i){
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
    } else if (this->inflections.find(ctagInf) != this->inflections.end() && split1 == 1) {
        int maxCount = 0;
        UnicodeString maxEndingForm = "";
        UnicodeString possible_form;
        for (vector<vector<UnicodeString> >::iterator it = this->inflections[ctagInf].begin();
             it != this->inflections[ctagInf].end(); ++it) {
            string ends;
            (*it).front().toUTF8String(ends);
            int c =form.indexOf((*it).front());
            if (form.endsWith((*it)[0])) {

                if ((*it)[0] == "") {
                    possible_form = form;
                } else {

                    possible_form = form.tempSubStringBetween(0, form.lastIndexOf((*it)[0]));
                    possible_form.append((*it)[1]);

                }
                string tmp, tmp1, tmp2, tmp3, tmp4;
                (*it)[0].toUTF8String(tmp);
                possible_form.toUTF8String(tmp1);
                (*it)[1].toUTF8String(tmp2);
                string comp;
                (*it)[2].toUTF8String(comp);
                ctagInf.toUTF8String(tmp3);
                form.toUTF8String(tmp4);

                if (this->known_bases.end() !=
                    find(this->known_bases.begin(), this->known_bases.end(), possible_form.toLower())
                    && stoi(comp) > maxCount) {
                    //  &&(*it)[0].length()>maxEndingForm.length()){
                    //||(((*it)[0].length()==maxEndingForm.length()&&stoi(comp)>maxCount))){

                    maxCount = stoi(comp);
                    maxEndingForm = (*it)[0];
                    base = possible_form;
                }
            }
            base = base.tempSubStringBetween(0, 1).toUpper().append(base.tempSubStringBetween(1, base.length()));
        }
        if (base == "") {
            maxCount=0;
            maxEndingForm="";

            for (vector<vector<UnicodeString> >::iterator it = this->inflections[ctagInf].begin();
                 it != this->inflections[ctagInf].end(); ++it) {
                string comp;
                (*it)[2].toUTF8String(comp);
                if(form.endsWith((*it)[0])
                   && stoi(comp) > 30
                   && (*it)[0].length() > 1) {
                    string show;
                    possible_form = form.tempSubString(0, form.length() - it->front().length() + 1);
                    possible_form.toUTF8String(show);
                    if (stoi(comp) > maxCount) {
                        maxCount = stoi(comp);
                        base = possible_form;
                    }
                }
            }
        }

    } else if (ctagInf == ""
               && split1 == 1) {
        int maxCount = 0;
        UnicodeString maxEndingForm = "";
        for(map<UnicodeString,vector<vector<UnicodeString> > >::iterator it = this->inflections.begin(); it != this->inflections.end(); ++it) {
            for (vector<vector<UnicodeString> >::iterator it2 = it->second.begin();
                 it2 != it->second.end(); ++it2) {
                string comp;
                (*it2)[2].toUTF8String(comp);
                if(form.endsWith((*it2)[0])//){
                   && stoi(comp) > 900
                   && (*it2)[0].length() > 1) {
                    UnicodeString possible_form;
                    if((*it2)[0]==""){
                        possible_form = form;
                    }else{
                        possible_form = form.tempSubString(0,form.length()-(*it2)[0].length());
                        possible_form.append((*it2)[1]);
                    }
                    if(stoi(comp)>maxCount){
                        string view;
                        maxCount = stoi(comp);
                        possible_form.toUTF8String(view);
                        base = possible_form;
                    }
                }
            }
        }

    }
    return base;
}


void Inflection::loadInflectionRules(std::string pathname) {

    ifstream rules(pathname);

    string line;


    while(getline(rules,line)) {
        UnicodeString to_split = line.c_str();
        UnicodeString delimiter = " ";
        vector<UnicodeString> tmp = split(to_split,delimiter);
        if(tmp.size()>3){
            UnicodeString ctag = tmp[0];
            vector<UnicodeString> row;
            row.push_back(tmp[1]);
            row.push_back(tmp[2]);
            row.push_back(tmp[3]);


            this->inflections[ctag].push_back(row);

        }else{
            cout<<"Zła linia"<<endl;
        }
    }
    rules.close();
}

UnicodeString Inflection::generate_base(icu::UnicodeString ctag, icu::UnicodeString form) {

    string tmp;
    UnicodeString base = this->_generate_base(ctag,form);
    base.toUTF8String(tmp);
    if(base==""&&ctag=="subst:sg:gen:m1"){
        base = this->_generate_base("subst:sg:acc:m1",form);
    }else if(base==""&&ctag=="subst:sg:gen:m3"){
        base = this->_generate_base("subst:sg:acc:m3",form);
    }else if(base==""&&ctag=="subst:sg:acc:m1"){
        base = this->_generate_base("subst:sg:gen:m1",form);
    }else if(base==""&&ctag.endsWith(":n")){
        base = this->_generate_base(ctag.findAndReplace(":n",":m1"),form);
    }else if(base==""&&ctag.endsWith(":n")){
        base = this->_generate_base(ctag.findAndReplace(":n",":f"),form);
    }else if(base==""&&ctag.endsWith(":m3")){
        base = this->_generate_base(ctag.findAndReplace(":m3",":m1"),form);
    }

    if(base==""
        &&ctag.indexOf(":nom")==-1){
        base = this->_generate_base("",form);
    }
    return base;
}

std::vector<icu::UnicodeString> Inflection::split(icu::UnicodeString to_split, icu::UnicodeString delimiter) {

    UnicodeString res[100];
    UErrorCode status =  U_ZERO_ERROR;
    RegexMatcher m ("\\s+",0,status);
    int numwords = m.split(to_split,res,10,status);

    vector<UnicodeString> ret;

    for(int i = 0; i < numwords; ++i){
        ret.push_back(res[i]);
    }

    return ret;
}
