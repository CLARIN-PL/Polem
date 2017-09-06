#include <iostream>
#include <libcorpus2/tagsetmanager.h>
#include <fstream>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "CascadeLemmatizer.h"

using namespace std;

CascadeLemmatizer assembleLemmatizer(string pathname, const Corpus2::Tagset &tagset) {

    string line;
    vector<UnicodeString> vecLastNames;

    ifstream lastNames("nelexicon2_nam_liv_person_last.txt");
    while (getline(lastNames, line)) {
        vecLastNames.emplace_back(line.substr(line.find("\t") + 1).c_str());
    }
    lastNames.close();
    //loading dictionaries to namlivlemmatizer

    ifstream firstNames("nelexicon2_nam_liv_person_first.txt");
    while (getline(firstNames, line)) {
        vecLastNames.emplace_back(line.substr(line.find("\t") + 1).c_str());
    }
    firstNames.close();

    vector<UnicodeString> vecNamLoc;

    ifstream namLoc("nelexicon2-infobox-nam_loc.txt");
    while (getline(namLoc, line)) {
        vecNamLoc.emplace_back(line.substr(line.find("\t") + 1).c_str());
    }
    namLoc.close();
    //loading dictionary for namloclemmatizer

    ifstream dictFile("nelexicon2_wikipedia-infobox-forms-with-bases-filtered.txt");

    map<UnicodeString, pair<UnicodeString, UnicodeString> > dictionaryItems;

    while (getline(dictFile, line)) {
        UnicodeString dictCat, dictOrth, dictLemma;
        dictCat = line.substr(0, line.find("\t")).c_str();
        dictOrth = line.substr(line.find("\t") + 1).substr(0,
                                                           line.substr(line.find("\t") + 1).find_last_of("\t")).c_str();
        dictLemma = line.substr(line.rfind("\t") + 1).c_str();
        dictionaryItems[dictOrth] = make_pair(dictCat, dictLemma);

    }
    dictFile.close();
    //loading dict for dictionary lemmatizers
    //dictionary items - key - unicode string value - pair < uni string, uni string >
    //orth, pair < category , lemma >

    Inflection inflection = Inflection(vecLastNames);
    inflection.loadInflectionRules("inflection_nam_liv_person_last.txt");
    Inflection inflectionNamLoc = Inflection(vecNamLoc);
    inflectionNamLoc.loadInflectionRules("inflection_nam_loc.txt");
    //loading rules for inflection

    morfeusz::Morfeusz *generator = morfeusz::Morfeusz::createInstance(morfeusz::GENERATE_ONLY);

    return CascadeLemmatizer(pathname, tagset, generator, dictionaryItems, inflection,
                             inflectionNamLoc);

}


double acc(int tru, int fals) {
    if (tru + fals > 0) {
        return (double) tru * 100 / (tru + fals);
    } else {
        return 0.0;
    }
}

void
printResults(ofstream &output, map<string, pair<int, int> > tfByMethod, map<string, pair<int, int> > tfByCategory) {
    int cats = 0, catf = 0;
    int ms = 0, mf = 0;
    for (map<string, pair<int, int> >::iterator it = tfByCategory.begin(); it != tfByCategory.end(); ++it) {
        cout << it->first << "\t" << " Success: " << it->second.first << "\t" << " Fail: " << it->second.second << "\t"
             << " Percentage: " << acc(it->second.first, it->second.second) << endl;
        output << it->first << "\t" << " Success: " << it->second.first << "\t" << " Fail: " << it->second.second
               << "\t" << " Percentage: " << acc(it->second.first, it->second.second) << endl;
        cats = cats + it->second.first;
        catf = catf + it->second.second;
    }
    for (map<string, pair<int, int> >::iterator it = tfByMethod.begin(); it != tfByMethod.end(); ++it) {
        cout << it->first << "\t" << " Success: " << it->second.first << "\t" << " Fail: " << it->second.second << "\t"
             << " Percentage: " << acc(it->second.first, it->second.second) << endl;
        output << it->first << "\t" << " Success: " << it->second.first << "\t" << " Fail: " << it->second.second
               << "\t" << " Percentage: " << acc(it->second.first, it->second.second) << endl;
        ms = ms + it->second.first;
        mf = mf + it->second.second;
    }
    output << "Success: " << cats << " Failure: " << catf << endl;
    output << "Percentage: " << acc(cats, catf) << "%" << endl;
    cout << cats << " " << catf << endl;
    cout << acc(cats, catf) << endl;
}

string trim(string &line) {

    size_t first = line.find_first_not_of(" ");
    size_t last = line.find_last_not_of(" ");
    if (first != -1 && last != -1) {
        return line.substr(first, (last - first + 1));
    } else if (first != -1) {
        return line.substr(first, line.length());
    } else if (last != -1) {
        return line.substr(0, last);
    } else return line;
}


int main(int argc, char *argv[]) {

    string pathname = argv[1];
    string argTagset = argv[2];
    UnicodeString cs = argv[3];
    bool caseInsensitive = cs.toLower() == "true" != 0;
    UnicodeString ss = argv[4];
    bool spaceInsensitive = ss.toLower() == "true" != 0;
    //processing arguments
    // pathname to file to lemmatize, tagset, case sensitive, space sensitive

    map<string, pair<int, int> > tfByMethod;
    map<string, pair<int, int> > tfByCategory;
    //initializing structures to keep data about correctness of lemmatization
    // string is keyword, pair of ints is amount of success and failure in lemmatization

    const Corpus2::Tagset &tagset = Corpus2::get_named_tagset(argTagset);
    CascadeLemmatizer cascadeLemmatizer = assembleLemmatizer(pathname, tagset);


    ifstream infile(pathname.c_str());

    int line_no = 0;
    if (pathname.find("/") != string::npos) {
        pathname = pathname.substr(pathname.find("/") + 1);
    }
    string outname = "Lemmatized-";
    if (!caseInsensitive) {
        outname.append("cs-");
    }
    if (!spaceInsensitive) {
        outname.append("ss-");
    }
    outname.append(pathname);
    //creating name for output file

    ofstream output;
    output.open(outname);

    string line;
    while (getline(infile, line, '\n')) {
        //line by line reading file
        //keyword \t orth \t base \t tag \t spaces \t category \n

        line = trim(line);
        vector<string> fields;
        boost::split(fields, line, boost::is_any_of("\t"));

        if (fields.size() < 4)continue;
        string keyword = fields[0];
        string keyword_orth = fields[1];
        string keyword_base = fields[2];
        string keyword_ctag = fields[3];

        string keyword_spaces;
        if (fields.size() < 5) keyword_spaces = "";
        else {
            keyword_spaces = fields[4];
            keyword_spaces = trim(keyword_spaces);
        }

        string keyword_category;
        if (fields.size() < 6) keyword_category = "";
        else {
            keyword_category = fields[5];
            keyword_category = trim(keyword_category);
        }

        keyword = trim(keyword);
        keyword_orth = trim(keyword_orth);
        keyword_base = trim(keyword_base);
        keyword_ctag = trim(keyword_ctag);

        vector<string> orths;
        boost::split(orths, keyword_orth, boost::is_any_of(" "));
        vector<string> bases;
        boost::split(bases, keyword_base, boost::is_any_of(" "));
        vector<string> ctags;
        boost::split(ctags, keyword_ctag, boost::is_any_of(" "));
        vector<string> spaces;
        boost::split(spaces, keyword_spaces, boost::is_any_of(" "));
        //multiword orth

        vector<vector<string> > kw;
        for (int i = 0; i < orths.size(); i++) {
            string booltmp;
            if (spaces[i] == "True") booltmp.push_back('t');
            else booltmp = "f";
            vector<string> row;
            row.push_back(orths[i]);
            row.push_back(bases[i]);
            row.push_back(ctags[i]);
            row.push_back(booltmp);
            kw.push_back(row);
        }
        //processing keyword into input to lemmatizer as
        //orth1, base1, tag1, space1  <- vector
        //orth2, base2, tag2, space2 and so on
        UnicodeString lemma = cascadeLemmatizer.lemmatize(kw, keyword_category);

        UnicodeString keywrd = keyword.c_str();
        string lemmaprnt;
        lemma.toUTF8String(lemmaprnt);

        if (caseInsensitive) {
            lemma = lemma.toLower();
            keywrd = keywrd.toLower();
        }
        if (spaceInsensitive) {
            lemma.findAndReplace(" ", "");
            keywrd.findAndReplace(" ", "");
        }
        //handling case and space sensitivity options

        if (lemma == keywrd) { // success
            if (tfByMethod.count(globalMethod) > 0) { // method found in structure
                tfByMethod[globalMethod].first++;      //increasing success amount
            } else {
                tfByMethod.insert(make_pair(globalMethod, make_pair(1, 0)));
            }
            if (tfByCategory.count(keyword_category) > 0) {
                tfByCategory[keyword_category].first++;
            } else {
                tfByCategory.insert(make_pair(keyword_category, make_pair(1, 0)));
            }
            output.clear();
            // cout << line_no << "\t\t" << "TRUE" << "\t\t" << lemmaprnt << "\t\t" << keyword << "\t\t"
            //      << keyword_category << "\t\t" << globalMethod << "\t\t" << keyword_ctag << endl;
            output << line_no << "\t" << "TRUE" << "\t" << lemmaprnt << "\t" << keyword << "\t" << keyword_category
                   << "\t" << globalMethod << "\t" << keyword_ctag << endl;
        } else { // failure
            if (tfByMethod.count(globalMethod) > 0) {
                tfByMethod[globalMethod].second++;
            } else {
                tfByMethod.insert(make_pair(globalMethod, make_pair(0, 1)));
            }
            if (tfByCategory.count(keyword_category) > 0) {
                tfByCategory[keyword_category].second++;
            } else {
                tfByCategory.insert(make_pair(keyword_category, make_pair(0, 1)));
            }
            output.clear();
            cout << line_no << "\t\t" << "FALSE" << "\t\t" << lemmaprnt << "\t\t" << keyword << "\t\t"
                 << keyword_category << "\t\t" << globalMethod << "\t\t" << keyword_ctag << endl;
            output << line_no << "\t" << "FALSE" << "\t" << lemmaprnt << "\t" << keyword << "\t" << keyword_category
                   << "\t" << globalMethod << "\t" << keyword_ctag << endl;
        }
        line_no++;
    }
    printResults(output, tfByMethod, tfByCategory);

    infile.close();
    output.close();

    return 0;
}

