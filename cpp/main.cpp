#include <iostream>
#include <libcorpus2/tagsetmanager.h>
#include <fstream>
#include <unicode/regex.h>
#include <iomanip>
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
printResults(ofstream &output, map<string, pair<int, int> > tfByMethod, map<string, pair<int, int> > tfByCategory,
             int count) {
    int cats = 0, catf = 0;
    int ms = 0, mf = 0;
    output << "------------------------------------------------------------" << endl;
    output << "# Evaluation by method" << endl;
    output << "%------------------------------------------------------------" << endl;
    output << "\\hline" << endl;
    output << "True &     False &     Acc &     Method &&     Coverage \\" << endl;
    output << "%------------------------------------------------------------" << endl;
    output << "\\hline" << endl;
    cout << "------------------------------------------------------------" << endl;
    cout << "# Evaluation by method" << endl;
    cout << "%------------------------------------------------------------" << endl;
    cout << "\\hline" << endl;
    cout << "True &     False &     Acc &     Method &&     Coverage \\" << endl;
    cout << "%------------------------------------------------------------" << endl;
    cout << "\\hline" << endl;

    for (map<string, pair<int, int> >::iterator it = tfByMethod.begin(); it != tfByMethod.end(); ++it) {
        output << it->second.first << " &     " << it->second.second << " &     "
               << acc(it->second.first, it->second.second) << " &     " << it->first << " &     "
               << acc(it->second.first + it->second.second, count) << "%" << "\\" << endl;
        cout << it->second.first << " &     " << it->second.second << " &     "
             << acc(it->second.first, it->second.second) << " &     " << it->first << " &     "
             << acc(it->second.first + it->second.second, count) << "%" << "\\" << endl;
        ms = ms + it->second.first;
        mf = mf + it->second.second;
    }
    output << "%------------------------------------------------------------" << endl;
    output << "\\hline" << endl;
    output << ms << " &     " << mf << " &     " << acc(ms, mf) << "% &     Total                    \\" << endl;
    output << "%------------------------------------------------------------" << endl;
    output << "\\hline" << endl;
    output << "------------------------------------------------------------" << endl;
    output << "# Evaluation by keyword category" << endl;
    output << "%------------------------------------------------------------" << endl;
    output << "\\hline" << endl;
    output << "True &     False &     Acc &     Method &&     Coverage \\" << endl;
    output << "%------------------------------------------------------------" << endl;
    output << "\\hline" << endl;
    cout << "%------------------------------------------------------------" << endl;
    cout << "\\hline" << endl;
    cout << ms << " &     " << mf << " &     " << acc(ms, mf) << "% &     Total                    \\" << endl;
    cout << "%------------------------------------------------------------" << endl;
    cout << "\\hline" << endl;
    cout << "------------------------------------------------------------" << endl;
    cout << "# Evaluation by keyword category" << endl;
    cout << "%------------------------------------------------------------" << endl;
    cout << "\\hline" << endl;
    cout << "True &     False &     Acc &     Method &&     Coverage \\" << endl;
    cout << "%------------------------------------------------------------" << endl;
    cout << "\\hline" << endl;
    for (map<string, pair<int, int> >::iterator it = tfByCategory.begin(); it != tfByCategory.end(); ++it) {
        output << it->second.first << " &     " << it->second.second << " &     "
               << acc(it->second.first, it->second.second) << " &     " << it->first << " &     "
               << acc(it->second.first + it->second.second, count) << "%" << "\\" << endl;
        cout << it->second.first << " &     " << it->second.second << " &     "
             << acc(it->second.first, it->second.second) << " &     " << it->first << " &     "
             << acc(it->second.first + it->second.second, count) << "%" << "\\" << endl;
        cats = cats + it->second.first;
        catf = catf + it->second.second;
    }
    output << "%------------------------------------------------------------" << endl;
    output << "\\hline" << endl;
    output << cats << " &     " << catf << " &     " << acc(cats, catf) << "% &     Total                    \\"
           << endl;
    output << "%------------------------------------------------------------" << endl;
    output << "\\hline" << endl;
    cout << "%------------------------------------------------------------" << endl;
    cout << "\\hline" << endl;
    cout << cats << " &     " << catf << " &     " << acc(cats, catf) << "% &     Total                    \\" << endl;
    cout << "%------------------------------------------------------------" << endl;
    cout << "\\hline" << endl;
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

    int line_no = 1;
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

    output << setprecision(4) << "Line" << "\t" << "Correct" << "\t" << "Orth" << "\t" << "Lemma" << "\t" << "Expected"
           << "\t" << "Category" << "\t" << "Method" << "\t" << "Bases" << "\t" << "Ctags" << endl;
    cout << setprecision(4) << "Line" << "\t" << "Correct" << "\t" << "Orth" << "\t" << "Lemma" << "\t" << "Expected"
         << "\t" << "Category" << "\t" << "Method" << "\t" << "Bases" << "\t" << "Ctags" << endl;

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

        if (line_no == 292) {
            cout << "";
        }

        UnicodeString lemma = cascadeLemmatizer.lemmatize(kwrd_orth, kwrd_base, kwrd_ctag, kwrd_spaces, kwrd_category);

        //string view = globalMethod;
        string lemmaprnt;
        lemma.toUTF8String(lemmaprnt);
        string kwrdprnt;
        string ctagprnt;
        string orthprnt;
        string basesprnt;
        while (kwrd.indexOf("  ") != -1 || kwrd.indexOf("\t") != -1) {
            kwrd.findAndReplace("  ", " ");
            kwrd.findAndReplace("\t", " ");
        }
        kwrd.toUTF8String(kwrdprnt);
        kwrd_ctag.toUTF8String(ctagprnt);
        kwrd_orth.toUTF8String(orthprnt);
        kwrd_base.toUTF8String(basesprnt);

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
            output << "[" << line_no << "]" << "\t" << "True" << "\t" << orthprnt << "\t" << lemmaprnt << "\t"
                   << kwrdprnt << "\t" << kwrd_category
                   << "\t" << globalMethod << "\t" << basesprnt << "\t" << ctagprnt << endl;
            cout << "[" << line_no << "]" << "\t" << "True" << "\t" << orthprnt << "\t" << lemmaprnt << "\t" << kwrdprnt
                 << "\t" << kwrd_category
                 << "\t" << globalMethod << "\t" << basesprnt << "\t" << ctagprnt << endl;
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
            output << "[" << line_no << "]" << "\t" << "False" << "\t" << orthprnt << "\t" << lemmaprnt << "\t"
                   << kwrdprnt << "\t" << kwrd_category
                   << "\t" << globalMethod << "\t" << basesprnt << "\t" << ctagprnt << endl;
            cout << "[" << line_no << "]" << "\t" << "False" << "\t" << orthprnt << "\t" << lemmaprnt << "\t"
                 << kwrdprnt << "\t" << kwrd_category
                 << "\t" << globalMethod << "\t" << basesprnt << "\t" << ctagprnt << endl;

        }
        line_no++;
    }
    printResults(output, tfByMethod, tfByCategory, line_no);

    infile.close();
    output.close();

    return 0;
}

