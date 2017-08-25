//
// Created by gkubon on 19/07/17.
//
#include <unicode/unistr.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include "RuleLemmatizer.h"
#include "pugixml-1.8/src/pugixml.hpp"
#include "CascadeLemmatizer.h"
#include <libcorpus2/io/reader.h>
#include <libcorpus2/tagging.h>
#include <libwccl/values/strset.h>
#include <libwccl/parser/Parser.h>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace pugi;



RuleLemmatizer::RuleLemmatizer(string rulespathname,Corpus2::Tagset tagset,morfeusz::Morfeusz *generator,bool fix,bool useOrthForOov) {

    this->generator=generator;
    this->useOrthForOov = useOrthForOov;
    this->tagset=tagset;

    xml_document doc;
    doc.load_file("/home/gkubon/Documents/gk/polem/lemmatization-rules-azon2.xml",parse_default|parse_declaration);
    xml_node rules = doc.child("rules");

    char temp[] = "/tmp/fileXXXXXX";
    int fd;


    fd=mkstemp(temp);

    fstream file;
    file.open(temp);


    int rule_no=0;
    for(xml_node rule = rules.child("rule"); rule; rule = rule.next_sibling("rule")){

        string name = rule.attribute("name").value();
        this->rule_categories[name+"-0"] = rule.attribute("category").value();
        if(fix||(name.find("Fix")==string::npos)){
            if(rule_no>0){
                file<<"\n";
            }
            file<<"@b:\""<<name<<"\" ("<<rule.child_value()<<")";
            cout<<name<<endl;
        }
        rule_no++;
    }
    file.flush();

    Wccl::Parser parser = Wccl::Parser(tagset);

    Wccl::FunctionalOpSequence::name_op_v_t fos = parser.parseWcclFileFromPath(temp)->gen_all_op_pairs();
    file.close();
    this->wccl_operators=fos;

}

icu::UnicodeString RuleLemmatizer::lemmatize(std::vector<std::vector<std::string>> kw, std::string kw_category) {


    char temp[] = "/tmp/fileXXXXXXX";
    int fd;

    fd=mkstemp(temp);

    fstream file;
    file.open(temp);

    vector<string> spaces;
    for(auto &word:kw){
        string a = word[0]+"\t"+word[1]+"\t"+word[2]+"\n";
        file<<a.c_str();
        spaces.push_back(word[3]);
    }
    file<<endl;

    vector<string> lemmas;

    boost::shared_ptr<Corpus2::TokenReader> rdr = Corpus2::TokenReader::create_path_reader("iob-chan", this->tagset, temp);
    boost::shared_ptr<Corpus2::Sentence> sentence= rdr ->get_next_sentence();

    for(vector<pair<string,boost::shared_ptr<Wccl::FunctionalOperator>>>::iterator it=this->wccl_operators.begin();
        it!=this->wccl_operators.end();++it){

        string rcat = this->rule_categories[it->first];

        if(rcat!=""&&kw_category!=rcat){
            continue;
        }

        Wccl::SentenceContext sc(sentence->clone_shared());
        sc.goto_start();
        string res = it->second->base_apply(sc)->to_string(this->tagset);

                  if(res=="True"&&it->second->to_raw_string().find("Pos"+sentence->size())!=string::npos){

                      map<int,vector<string> > operationss;

                      int pos = 1;

                      unsigned long found = it->second->to_string(this->tagset).find("Pos"+std::to_string(pos));
                      while(found!=string::npos){
                              vector<string> operations;

                              if(it->second->to_string(this->tagset).find("Pos"+to_string(pos)+"mod")!=string::npos){
                                  string op = it->second->to_string(this->tagset).substr(it->second->to_string(this->tagset).find("Pos"+to_string(pos)+"mod"));
                                  op = op.substr(op.find("["));
                                  op = op.substr(0,op.find("]"));
                                  string tmp;
                                  while(tmp.compare(op)){
                                      tmp = op.substr(0,op.find(","));
                                      op = op.substr(op.find(",")+1);

                                      operations.push_back(tmp);
                                  }
                              }
                                operationss.insert(make_pair(pos,operations));


                              pos++;
                              found = it->second->to_string(this->tagset).find("Pos"+to_string(pos));
                          }
                          icu::UnicodeString lemma;
                          lemma = this->generate(sentence,operationss,spaces,kw_category);
                      if(lemma=="")goto second;
                      string err;
                      lemma.toUTF8String(err);
                      file.close();

                      lemmas.push_back(err);
                      lemmas.push_back(it->first.c_str());
                      }
        second:   if(res == "True" && it->second->to_string(this->tagset).find("PosTail")!=string::npos){

                        map<int,vector<string> > operationss;

                        for(int i = 1;i<=sentence->tokens().size();i++){
                              vector<string> operations;
                              if(it->second->to_string(this->tagset).find("Pos"+std::to_string(i)+"mod")!=string::npos){

                                  size_t beginning = it->second->to_string(this->tagset).find("Pos"+std::to_string(i)+"mod");
                                  string val = it->second->to_string(this->tagset).substr(beginning+10);
                                  size_t end = val.find("])\n");
                                  val = val.substr(1,end-2);
                                  int oppNum = count(val.begin(),val.end(),',');
                                  if(oppNum>0){
                                      for(int i =0; i<oppNum;i++){
                                          string op = val.substr(0,val.find(","));
                                          val = val.substr(val.find(","));
                                          operations.push_back(op);
                                      }
                                  }else{
                                      operations.push_back(val);
                                  }
                              }else{
                                  if(i>2 &&
                                     kw[i-1][2].find("adj")!=string::npos &&
                                          kw[i-3][2].find("adv")!=string::npos &&
                                              kw[i-2][2]=="-"){
                                      operations.push_back("cas=nom");
                                  }else{
                                      operations.push_back("");
                                  }
                              }
                              operationss.insert(make_pair(i,operations));
                        }

                        icu::UnicodeString lemma;
                        lemma = this->generate(sentence,operationss,spaces,kw_category);
                        if(lemma=="")continue;
                        string err;
                        lemma.toUTF8String(err);
                        file.close();
                        lemmas.push_back(err);
                        lemmas.push_back(it->first.c_str());
                    }



    }
    file.close();
    if(lemmas.size()>0){
        unsigned long counter = 0;
        int index=0;
        for(int i = 1; i < lemmas.size() ; i=i+2){

            //TODO ZNALEZC LEPSZY ALGORYTM WYBIERANIA REGUŁY
            if(lemmas[i].size()>14){
                counter=lemmas[i].size();
               index = i-1;
            }
        }
        globalMethod = "RuleLemmatizer::"+lemmas[index+1];
        return lemmas[index].c_str();
    }

    return "";
}

icu::UnicodeString RuleLemmatizer::generate(Corpus2::Sentence::Ptr sentence, std::map<int,vector<string> >operations, std::vector<std::string> spaces,
                                std::string kw_category) {

    bool keepuc;

    if(kw_category=="nam"||kw_category.find("nam_")==(size_t)0){//||b!=sentence->tokens().front()->orth()){ //TODO sprawdzić czy 0 nie zmienić na 1
        keepuc=true;
    }else keepuc=false;

    vector<icu::UnicodeString> lemmas;
    vector<bool> lemmaspaces;

    for(map<int,vector<string> >::iterator it = operations.begin();it!=operations.end();++it){

        int position = it->first;
        Corpus2::Token *token = sentence->tokens()[position-1];
        icu::UnicodeString orth = token->orth();
        if(spaces[position-1]=="t"){
            lemmaspaces.push_back(true);
        }else{
            lemmaspaces.push_back(false);
        }

        vector<icu::UnicodeString> forms;

        Corpus2::Tag tag;
        try{
            tag = token->get_preferred_lexeme(this->tagset).tag();
        }catch(exception e){
          //  cout<<e.what();
        }

        for(vector<string>::iterator itt = it->second.begin();itt!=it->second.end();++itt){
            if((*itt)!="") {
                string attr = (*itt).substr(0, (*itt).find("="));
                string val = (*itt).substr((*itt).find("=") + 1);

                attr = attr.substr(attr.find('\"') + 1);
                val = val.substr(0, val.find("\""));
                if (val.find("=") == string::npos) {
                    Corpus2::Tag tag_mask = get_attribute_mask(this->tagset, attr);
                    Corpus2::Tag tag_val = this->tagset.parse_symbol_string(val);
                    tag = Corpus2::with_values_masked(tag, tag_val, tag_mask);
                }
            }else{
                continue;
            }
        }

        icu::UnicodeString base;
        try{
            base = token->get_preferred_lexeme(this->tagset).lemma();

        } catch (exception e){
         //   cout<<e.what();
        }

        vector<morfeusz::MorphInterpretation> res;
        string ctag;
        string basestr;
        try{

             base.toUTF8String(basestr);
             ctag = this->tagset.tag_to_string(tag);

             //int tagId = this->generator->getIdResolver().getTagId(ctag);


             this->generator->generate(basestr,res);


           //  this->generator->generate(basestr,this->generator->getIdResolver().getTagId(ctag),res);
         }catch(morfeusz::MorfeuszException e){

                cout<<e.what()<<endl;
         }

        UnicodeString form="";
        if(res.size()>0){

            int var = 0;
            for(auto kek:res){
                var = evaluateSharedTag(kek.getTag(*generator),ctag);
                if(var==100){
                    form = kek.orth.c_str();
                }

            }
            if(res.size()==1&&res.front().tagId==0&&ctag.find("m1")!=string::npos&&token->orth().endsWith("a")){
                form = res.front().lemma.c_str();
            }

            if(keepuc&&orth!=orth.toLower()){
                form = form.tempSubString(0,1).toUpper().append(form.tempSubString(1,form.length()));
            }
            if(form.indexOf(":")!=(-1)){
                form = form.tempSubStringBetween(0,form.indexOf(":"));
            }
            if(form.indexOf("-")!=(-1)&&position==sentence->tokens().size()-1){
                form = form.tempSubStringBetween(0,form.indexOf("-"));
            }

        }
        if(form==""){
            if(this->useOrthForOov){
                 form = token->orth();
            }else{
                return "";
            }
        }
        if((form.indexOf("'")>1&&sentence->tokens().size()==1)||(form.indexOf("'")>1&&sentence->tokens().size()>1&&position==sentence->tokens().size())){
            form = form.tempSubStringBetween(0,form.indexOf("'"));
        }
        lemmas.push_back(form);


    }
    UnicodeString lemma = "";

    if(sentence->tokens().size()>lemmas.size()){
        for(int i = lemmas.size(); i < sentence->tokens().size(); ++i){
            if(sentence->tokens()[i]->orth().indexOf("’")>-1&&i+2==sentence->tokens().size()){
                break;
            }
            lemmas.push_back(sentence->tokens()[i]->orth());
        }
    }

    for(int i = 0 ; i < lemmas.size() ; ++i){
        if(lemmas[i]==","||lemmas[i]==":"||lemmas[i]=="'"){
            lemma = lemma.trim();
            lemma.append(lemmas[i]);
            lemma.append(" ");
            continue;
        }else if(lemmas[i]=="."||lemmas[i]=="-"||lemmas[i]=="’"){
            lemma = lemma.trim();
            lemma.append(lemmas[i]);
            continue;
        }else if(i+1<lemmas.size()){
            //TODO ???
        }

        //TODO case sensitive \/
        wchar_t abc,bcd,cde,cdeLow,efg;
        abc = sentence->tokens()[i]->orth().charAt(0);
        bcd = lemmas[i].charAt(0);
        cde = sentence->tokens()[i]->orth().charAt(2);
        UnicodeString xyz = sentence->tokens()[i]->orth();
        cdeLow = xyz.toLower().charAt(2);
        efg = lemmas[i].charAt(2);
        UnicodeString ifs = sentence->tokens()[i]->orth();
        if(cde!=efg&&efg==cdeLow){
            lemma.append(lemmas[i].toUpper()+" ");
        }else if(abc!=bcd&&kw_category.find("nam_adj")!=0){
            lemma.append(abc);
            lemma.append(lemmas[i].tempSubStringBetween(1,lemmas[i].length()).trim());
            lemma.append(" ");
        }else{
            lemma.append(lemmas[i].trim());
            lemma.append(" ");
        }
    }

    lemma.trim();
    return lemma;
}

int RuleLemmatizer::evaluateSharedTag(const std::string& generatorTag, std::string cTag) {

    int result=0;
    string abc = generatorTag;
    vector<string> tags;
    boost::split(tags,cTag,boost::is_any_of(":"));
    for(auto kek:tags){
        if(abc.find(":"+kek+":")!=string::npos) result++;
        else if(abc.find("."+kek+":")!=string::npos) result++;
        else if(abc.find(":"+kek+".")!=string::npos) result++;
        else if(abc.find("."+kek+".")!=string::npos) result++;
        else if(abc.find(kek+":")!=string::npos) result++;
        else if(abc.find(kek+".")!=string::npos) result++;
        else if(abc.find(":"+kek)!=string::npos) result++;
        else if(abc.find("."+kek)!=string::npos) result++;
    }

    return (result*100)/(double)tags.size();
}

