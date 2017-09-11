#include <iostream>
#include <libcorpus2/tagsetmanager.h>
#include <fstream>
#include <unicode/regex.h>
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
    cout << "Success: " << cats << " Failure: " << catf << endl;
    cout << "Percentage: " << acc(cats, catf) << "%" << endl;
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
        UnicodeString lin = line.c_str();

        UErrorCode status = U_ZERO_ERROR;
        RegexMatcher m("\\t", 0, status);
        const int maxWords = 6;
        UnicodeString fields[6];

        int numWords = m.split(lin, fields, maxWords, status);

        if (numWords < 4)continue;
        UnicodeString kwrd = fields[0].trim();
        UnicodeString kwrd_orth = fields[1].trim();
        UnicodeString kwrd_base = fields[2].trim();
        UnicodeString kwrd_ctag = fields[3].trim();
        UnicodeString kwrd_spaces;
        if (numWords < 5) kwrd_spaces = "";
        else {
            kwrd_spaces = fields[4].trim();
        }
        string kwrd_category;
        if (numWords < 6) kwrd_category = "";
        else {
            fields[5].findAndReplace("\tnull", "");
            fields[5].trim().toUTF8String(kwrd_category);
        }

        UnicodeString lemma = cascadeLemmatizer.lemmatize(kwrd_orth, kwrd_base, kwrd_ctag, kwrd_spaces, kwrd_category);

        //string view = globalMethod;
        string lemmaprnt;
        lemma.toUTF8String(lemmaprnt);
        string kwrdprnt, ctagprnt;
        while (kwrd.indexOf("  ") != -1 || kwrd.indexOf("\t") != -1) {
            kwrd.findAndReplace("  ", " ");
            kwrd.findAndReplace("\t", " ");
        }
        kwrd.toUTF8String(kwrdprnt);
        kwrd_ctag.toUTF8String(ctagprnt);

        if (caseInsensitive) {
            lemma = lemma.toLower();
            kwrd = kwrd.toLower();
        }
        if (spaceInsensitive) {
            lemma.findAndReplace(" ", "");
            kwrd.findAndReplace(" ", "");
        }

        if (lemma == kwrd) { // success
            if (tfByMethod.count(globalMethod) > 0) { // method found in structure
                tfByMethod[globalMethod].first++;      //increasing success amount
            } else {
                tfByMethod.insert(make_pair(globalMethod, make_pair(1, 0)));
            }
            if (tfByCategory.count(kwrd_category) > 0) {
                tfByCategory[kwrd_category].first++;
            } else {
                tfByCategory.insert(make_pair(kwrd_category, make_pair(1, 0)));
            }
            output.clear();
            cout << line_no << "\t" << "TRUE" << "\t" << lemmaprnt << "\t" << kwrdprnt << "\t" << kwrd_category
                 << "\t" << globalMethod << "\t" << ctagprnt << endl;
            output << line_no << "\t" << "TRUE" << "\t" << lemmaprnt << "\t" << kwrdprnt << "\t" << kwrd_category
                   << "\t" << globalMethod << "\t" << ctagprnt << endl;
        } else { // failure
            if (tfByMethod.count(globalMethod) > 0) {
                tfByMethod[globalMethod].second++;
            } else {
                tfByMethod.insert(make_pair(globalMethod, make_pair(0, 1)));
            }
            if (tfByCategory.count(kwrd_category) > 0) {
                tfByCategory[kwrd_category].second++;
            } else {
                tfByCategory.insert(make_pair(kwrd_category, make_pair(0, 1)));
            }
            output.clear();
            cout << line_no << "\t\t" << "FALSE" << "\t\t" << lemmaprnt << "\t\t" << kwrdprnt << "\t\t"
                 << kwrd_category << "\t\t" << globalMethod << "\t\t" << ctagprnt << endl;
            output << line_no << "\t" << "FALSE" << "\t" << lemmaprnt << "\t" << kwrdprnt << "\t" << kwrd_category
                   << "\t" << globalMethod << "\t" << ctagprnt << endl;
        }
        line_no++;
    }
    printResults(output, tfByMethod, tfByCategory);

    infile.close();
    output.close();

    return 0;
}

