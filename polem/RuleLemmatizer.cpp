/*
   Copyright (C) Wrocław University of Science and Technology (PWr), 2017-2018.
   Grzegorz Kuboń, Michał Marcińczuk.

   Part of Polem project.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
 */

#include <unicode/unistr.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include "RuleLemmatizer.h"
#include "../pugixml-1.8/src/pugixml.hpp"
#include "CascadeLemmatizer.h"
#include <libcorpus2/io/reader.h>
#include <libcorpus2/tagging.h>
#include <libwccl/values/strset.h>
#include <libwccl/parser/Parser.h>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace pugi;


/**
Moduł do lematyzacji w oparciu o ręcznie opracowane reguły. Reguły ze zbioru wykonywane są
        sekwencyjnie i dla pierwszej reguły, która zostanie dopasowana do frazy, generowany jest lemat
        poprzez zastosowanie określonej dla tej reguły transformacji.

Przykład reguły lematyzacji (z pliku resources/lemmatization-rules.xml):

<rule name="SubstAdj_Pl">
           and(
                   inter(nmb[0], {pl}),
                           inter(class[1],{adj,ppas,pact}),
inter(class[0],{subst,ger,depr}),
agrpp(0,1,{nmb,gnd,cas}),
setvar($Pos1, 0),
setvar($Pos2, 1),
setvar($s:Pos1mod, ["cas=nom"]),
setvar($s:Pos2mod, ["cas=nom"])
)
</rule>
*/


/**
Podczas inicjalizacji wczytywane są reguły z pliku rules_filename, a następnie kompilowane.
@param rules_filename: plik XML z regułami lematyzacji,
@param tagset: tagset w jakim definiowane będą reguły lematyzacji,
@param form_generator: obiekt będący generatorem form odmienionych
@param debug: tryb debug (domyślnie False). W trybie debug wypisywane są ścieżki do plików
    pomocnicznych i braki w generatorze form odmienionych.
@param words_translations: słownik do podmiany słów
*/
RuleLemmatizer::RuleLemmatizer(string tagset, morfeusz::Morfeusz *generator, bool fix,
                               bool useOrthForOov, std::string fileWithRulesPath) {
    this->generator = generator;
    this->useOrthForOov = useOrthForOov;
    this->tagset = Corpus2::get_named_tagset(tagset);

    xml_document doc;
    std::string files = fileWithRulesPath;
    doc.load_file(files.c_str(), parse_default | parse_declaration);
    xml_node rules = doc.child("rules");

    ostringstream rulesContent;

    // Wczytaj reguły wccl i operatory
    int rule_no = 0;
    for (xml_node rule = rules.child("rule"); rule; rule = rule.next_sibling("rule")) {

        string name = rule.attribute("name").value();
        this->rule_categories[name + "-0"] = rule.attribute("category").value();
        if (fix || (name.find("Fix") == string::npos)) {
            std::string ruleString = rule.child("wccl").child_value();
            ruleString = ruleString.substr(0,ruleString.find_last_of("\n"));
            ruleString = ruleString.substr(0,ruleString.find_last_of("\n"));
            for(xml_node set = rule.child("transformations").child("set"); set; set = set.next_sibling("set")){
                int index = set.attribute("index").as_int();
                std::string cas = set.attribute("cas").value();
                std::string gnd = set.attribute("gnd").value();
                std::string nmb = set.attribute("nmb").value();
                ruleString.append(",\n\t\t\tsetvar($s:Pos"+std::to_string(index+1)+"mod, [\"");
                if(cas!=""){
                    ruleString.append("cas=" + cas+"\",");
                }
                if(gnd!=""){
                    ruleString.append("\"gnd="+gnd+"\",");
                }
                if(nmb!=""){
                    ruleString.append("\"nmb="+nmb+"\",");
                }
                ruleString = ruleString.substr(0,ruleString.find_last_of(','));
                ruleString.append("])");
           }

            if (rule_no > 0) {
                rulesContent << "\n";
            }
            if(ruleString.find("//or,")!=string::npos){
                ruleString.replace(ruleString.find("//or,"),5,",//or");
            }
            rulesContent << "@b:\"" << name << "\" (" << ruleString << "\n\t\t)\n\t\t)";
        }
        rule_no++;
    }
    Wccl::Parser parser = Wccl::Parser(tagset);
    Wccl::FunctionalOpSequence::name_op_v_t fos = parser.parseWcclFile(rulesContent.str())->gen_all_op_pairs();
    this->wccl_operators = fos;
}

/**
 *
 * @param kw tablica krotek reprezentujących tokeny. Krotka składa się z trzech elementów: (orth, base, ctag)
 * @param kw_category
 * @param debug
 * @return
 */
icu::UnicodeString
RuleLemmatizer::lemmatize(std::vector <std::vector<icu::UnicodeString>> kw, std::string kw_category, bool debug) {
    if (debug) {
        cout << "Entering Rule lemmatizer" << endl;
    }

    std::stringstream sbuf;
    vector<icu::UnicodeString> spaces;
    for (auto &word:kw) {
        string a;
        word[0].toUTF8String(a);
        a.append("\t");
        word[1].toUTF8String(a);
        a.append("\t");
        word[2].toUTF8String(a);
        sbuf << a << endl;
        spaces.push_back(word[3]);
    }
    sbuf << endl;
    sbuf.flush();

    boost::shared_ptr<Corpus2::TokenReader> rdr;
    try {
        rdr = Corpus2::TokenReader::create_stream_reader("iob-chan", this->tagset, sbuf);
    } catch (Corpus2::Corpus2Error &e) {
        cout << e.what() << endl;
    }
    boost::shared_ptr<Corpus2::Sentence> sentence;
    try {
        sentence = rdr->get_next_sentence();
    } catch (Corpus2::Corpus2Error &e) {
        if (debug) {
            cout << "Exiting Rule lemmatizer" << endl;
        }
        return "";
    }
    
    //Odpal operatory i znajdź dopasowania
    for(auto& it : this->wccl_operators){

        string rcat = this->rule_categories[it.first];

        if (!rcat.empty() && kw_category != rcat) {
            continue;
        }

        Wccl::SentenceContext sc(sentence->clone_shared());
        sc.goto_start();
        string res = it.second->base_apply(sc)->to_string(this->tagset);

        //Spradź, czy reguła została dopasowana i zostały dopasowane wszystkie tokeny
        if (res == "True" && it.second->to_string(this->tagset).find("Pos" + to_string(sentence->tokens().size())) != string::npos ) {
            //Tablica indeksów względnych tokenów tworzących słowo kluczowe
            //W przypadku fraz o wolnym szyku kolejność słów w lemacie może
            //być inna niż we frazie wejściowej
            map<int, vector<string> > operationss;

            int pos = 1;
            //Szukamy operatorów dla dopasowanej reguły
            unsigned long found = it.second->to_string(this->tagset).find("Pos" + std::to_string(pos));
            while (found != string::npos) {
                vector<string> operations;

                if (it.second->to_string(this->tagset).find("Pos" + to_string(pos) + "mod") != string::npos) {
                    string op = it.second->to_string(this->tagset).substr(
                            it.second->to_string(this->tagset).find("Pos" + to_string(pos) + "mod"));
                    op = op.substr(op.find('['));
                    op = op.substr(0,op.find(']'));
                    boost::split(operations,op,boost::is_any_of(","));
                }
                operationss.insert(make_pair(pos, operations));


                ++pos;
                found = it.second->to_string(this->tagset).find("Pos" + to_string(pos));
            }
            icu::UnicodeString lemma = this->generate(sentence, operationss, spaces, kw_category);
            if (lemma != "") {
                globalMethod = "RuleLemmatizer:" + it.first;
            }
            return lemma;
        }
        //Dopasowany zostaje tylko początek
        if (res == "True" && it.second->to_string(this->tagset).find("PosTail") != string::npos) {

            map<int, vector<string> > operationss;

            for (int i = 1; i <= sentence->tokens().size(); i++) {
                vector<string> operations;
                if (it.second->to_string(this->tagset).find("Pos" + std::to_string(i) + "mod") != string::npos) {

                    size_t beginning = it.second->to_string(this->tagset).find("Pos" + std::to_string(i) + "mod");
                    string val = it.second->to_string(this->tagset).substr(beginning + 10);
                    size_t end = val.find("])\n");
                    val = val.substr(1, end - 2);
                    long oppNum = count(val.begin(), val.end(), ',');
                    if (0 < oppNum) {
                        for (int i = 0; i < oppNum; i++) {
                            string op = val.substr(0, val.find(','));
                            val = val.substr(val.find(','));
                            operations.push_back(op);
                        }
                    } else {
                        operations.push_back(val);
                    }
                } else {
                    if (i > 2 &&
                        kw[i - 1][2].indexOf("adj") != -1 &&
                        kw[i - 3][2].indexOf("adv") != -1 &&
                        kw[i - 2][2] == "-") {
                        operations.emplace_back("cas=nom");
                    } else {
                        operations.emplace_back("");
                    }
                }
                operationss.insert(make_pair(i, operations));
            }

            icu::UnicodeString lemma = this->generate(sentence, operationss, spaces, kw_category);
            if (lemma != "") {
                globalMethod = "RuleLemmatizer:" + it.first;
            }
            return lemma;
        }
    }
    if (debug) {
        cout << "Exiting Rule lemmatizer" << endl;
    }
    return "";
}

/**
Generuje lemat dla frazy.
@param sentence: obiekt Sentence reprezentujący frazę
@param positions: kolejność słów ze zdania
@param operations:
@param spaces: informacja, czy przed kolejnymi słowami występuje spacja
*/
icu::UnicodeString RuleLemmatizer::generate(Corpus2::Sentence::Ptr sentence, std::map<int, vector<string> > operations,
                                            std::vector<icu::UnicodeString> spaces,
                                            std::string kw_category) {
    bool keepuc;

    keepuc = kw_category == "nam" || kw_category.find("nam_") == (size_t) 0;

    vector<icu::UnicodeString> lemmas;
    vector<bool> lemmaspaces;
    UnicodeString lemma = "";

    //for (map<int, vector<string> >::iterator it = operations.begin(); it != operations.end(); ++it) {
    for(auto& it:operations){

        int position = it.first;
        Corpus2::Token *token = sentence->tokens()[position - 1];

        icu::UnicodeString orth = token->orth();

        lemmaspaces.emplace_back(spaces[position - 1] == "True" != 0);

        Corpus2::Tag tag;
        try {
            //Wartość początkowa tagu
            tag = token->get_preferred_lexeme(this->tagset).tag();
        } catch (exception &e) {
            //    cout<<e.what();
        }

        //for (vector<string>::iterator itt = it.second.begin(); itt != it.second.end(); ++itt) {
        for(auto& itt:it.second){
            if (!itt.empty()) {

                //szukaj operatorów typu attr=val
                string attr = itt.substr(0, itt.find('='));
                string val = itt.substr(itt.find('=') + 1);

                attr = attr.substr(attr.find('\"') + 1);
                val = val.substr(0, val.find('\"'));
                if (val.find('=') == string::npos) {

                    //Ustawia wartość dla wskazanego atrybutu
                    //tag - tag do modyfikacji
                    //gan - wartość atrybutu
                    //mask - maska atrybuty,
                    Corpus2::Tag tag_mask = get_attribute_mask(this->tagset, attr);
                    Corpus2::Tag tag_val = this->tagset.parse_symbol_string(val);
                    tag = Corpus2::with_values_masked(tag, tag_val, tag_mask);
                }
            }
        }

        icu::UnicodeString base;
        try {
            //Dokonaj transformacji formy bazowej zgodnie z określonymi operatorami
            base = token->get_preferred_lexeme(this->tagset).lemma();
        } catch (exception &e) {
            std::cerr << e.what();
        }

        vector<morfeusz::MorphInterpretation> res;
        string ctag;
        string basestr;
        try {
            int ctagID;
            base.findAndReplace(" ", "");
            base.toUTF8String(basestr);
            ctag = this->tagset.tag_to_string(tag);
            ctagID = this->generator->getIdResolver().getTagId(ctag);
            this->generator->generate(basestr, ctagID, res);
        } catch (morfeusz::MorfeuszException &e) {
            return "";
        }

        UnicodeString form = "";
        if (!res.empty()) {
            UnicodeString capital = orth;
            for (auto &it:res) {
                if (orth.toLower() == it.orth.c_str()) {
                    form = orth;
                    break;
                }
            }
            if (form != orth) {
                form = res.front().orth.c_str();
            }
            if (keepuc && orth.charAt(0) != capital.charAt(0)) {
                UnicodeString tmp = form;
                tmp.toUpper();
                form = tmp.charAt(0) + form.tempSubStringBetween(1, form.length());
            }
        }
        if (form == "") {
            if (this->useOrthForOov) {
                form = token->orth();
            } else {
                return "";
            }
        }
        lemmas.emplace_back(form);
    }
    for (int i = 0; i < lemmas.size(); ++i) {
        lemma.append(lemmas[i]);
        if (lemmaspaces[i]) {
            lemma.append(" ");
        }
    }
    string err = "";
    lemma.toUTF8String(err);

    return lemma;
}
