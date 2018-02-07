#include <iostream>
#include <libcorpus2/tagsetmanager.h>
#include <fstream>
#include <unicode/regex.h>
#include <iomanip>
#include "CascadeLemmatizer.h"
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;

double acc(int tru, int fals) {
    if (tru + fals > 0) {
        return (double) tru * 100 / (tru + fals);
    } else {
        return 0.0;
    }
}

void
printResults(ofstream &output, const string &title, map<string, pair<int, int> > tfs, int count) {
    int ms = 0, mf = 0;

    output << setfill('-')<<setw(60)<<"-"<<setfill(' ')<<endl;
    output <<"# "<<title << endl;
    output << '%'<<setfill('-')<<setw(60)<<"-"<<setfill(' ')<<endl;
    output << "\\hline" << endl;
    output<< boost::format("%5s & %5s & %6s & %-30s & %s \\\\") % "True" % "False" %"Acc" % "Method" % "Coverage"<<endl;
    output << '%'<<setfill('-')<<setw(60)<<"-"<<setfill(' ')<<endl;
    output << "\\hline" << endl;

    cout << setfill('-')<<setw(60)<<"-"<<setfill(' ')<<endl;
    cout <<"# "<<title << endl;
    cout << '%'<<setfill('-')<<setw(60)<<"-"<<setfill(' ')<<endl;
    cout << "\\hline" << endl;
    cout<< boost::format("%5s & %5s & %6s & %-30s & %s \\\\") % "True" % "False" %"Acc" % "Method" % "Coverage"<<endl;
    cout << '%'<<setfill('-')<<setw(60)<<"-"<<setfill(' ')<<endl;
    cout << "\\hline" << endl;

    //for (map<string, pair<int, int> >::iterator it = tfs.begin(); it != tfs.end(); ++it) {
    for(auto& it:tfs){
        UnicodeString cat = it.first.c_str();
        cat.findAndReplace("_","\\_");
        string print;
        cat.toUTF8String(print);
        output << boost::format(R"(%5d & %5d & %6.2f\%% & %-30s & %6.2f\%% \\)") % it.second.first
                  % it.second.second % acc(it.second.first,it.second.second) % print
                  % acc(it.second.first+it.second.second,count)<<endl;
        cout << boost::format(R"(%5d & %5d & %6.2f\%% & %-30s & %6.2f\%% \\)") % it.second.first
                  % it.second.second % acc(it.second.first,it.second.second) % print
                  % acc(it.second.first+it.second.second,count)<<endl;

        ms = ms + it.second.first;
        mf = mf + it.second.second;
    }
    output << '%'<<setfill('-')<<setw(60)<<"-"<<setfill(' ')<<endl;
    output << "\\hline" << endl;
    output << boost::format(R"(%5d & %5d & %6.2f\%% & %-30s \\)") % ms % mf % acc(ms,mf) % "Total"<<endl;
    output << "%------------------------------------------------------------" << endl;
    output << "\\hline" << endl;

    cout << '%'<<setfill('-')<<setw(60)<<"-"<<setfill(' ')<<endl;
    cout << "\\hline" << endl;
    cout << boost::format(R"(%5d & %5d & %6.2f\%% & %-30s \\)") % ms % mf % acc(ms,mf) % "Total"<<endl;
    cout << "%------------------------------------------------------------" << endl;
    cout << "\\hline" << endl;
}


int main(int argc, const char *argv[]) {

    try {

        boost::program_options::options_description desc("Parameters");
        desc.add_options()
                ("help,h", "Print help message")
                ("pathname,p", boost::program_options::value<string>(), "Pathname to input file - REQUIRED")
                ("files,f", boost::program_options::value<string>(), "Full pathname to data files for lemmatizer, required when lemmatizer is not installed")
                ("tagset,t", boost::program_options::value<string>(), "Tagset to use (Currently only NKJP)- OPTIONAL")
                ("case-sensitive,c", "Case sensitive evaluation - OPTIONAL")
                ("space-sensitive,s", "Whitespace sensitive evaluation - OPTIONAL")
                ("debug,d", "Additional debug prints");

        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);


        string pathname;
        string argTagset;
        bool caseInsensitive = true;
        bool spaceInsensitive = true;
        bool debug = false;
        string datafiles = "/usr/local/share/polem/";

        if (vm.count("help")) {
            cout << desc << endl;
            return 0;
        } else {
            boost::program_options::notify(vm);
            pathname = vm["pathname"].as<string>();
            if (vm.count("tagset")) {
                argTagset = vm["tagset"].as<string>();
            } else {
                argTagset = "nkjp";
            }
            if (vm.count("case-sensitive")) {
                caseInsensitive = false;
            }
            if (vm.count("space-sensitive")) {
                spaceInsensitive = false;
            }
            if (vm.count("debug")) {
                debug = true;
            }
            if (vm.count("files")) {
                datafiles = vm["files"].as<string>();
            }
        }

        if(!boost::algorithm::ends_with(datafiles,"/")){
            datafiles = datafiles+"/";
        }
        if(datafiles.find("/")!=0){
            datafiles = "/"+datafiles;
        }


        cout<<"Looking for data files in : " + datafiles <<endl;
        //processing arguments
        // pathname to file to lemmatize, tagset, case sensitive, space sensitive

        map<string, pair<int, int> > tfByMethod;
        map<string, pair<int, int> > tfByCategory;
        //initializing structures to keep data about correctness of lemmatization
        // string is keyword, pair of ints is amount of success and failure in lemmatization

        //const Corpus2::Tagset &tagset = Corpus2::get_named_tagset(argTagset);
        boost::optional<CascadeLemmatizer> cascadeLemmatizer;

        try {
            cascadeLemmatizer = CascadeLemmatizer::assembleLemmatizer(datafiles);
        } catch (std::exception& e) {
            std::cout << "Couldnt build Lemmatizer instance" << endl;
            std::cout << "Please specify the directory of data files with -f parameter" << endl;
            std::cout << e.what() <<endl;
            return 0;
        }

        ifstream infile(pathname.c_str());

        int line_no = 1;
        if (pathname.find('/') != string::npos) {
            pathname = pathname.substr(pathname.find('/') + 1);
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

        output << setprecision(4) << "Line" << "\t" << "Correct" << "\t" << "Orth" << "\t" << "Lemma" << "\t"
               << "Expected"
               << "\t" << "Category" << "\t" << "Method" << "\t" << "Bases" << "\t" << "Ctags" << endl;
        cout << setprecision(4) << "Line" << "\t" << "Correct" << "\t" << "Orth" << "\t" << "Lemma" << "\t"
             << "Expected"
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


            UnicodeString lemma = cascadeLemmatizer->lemmatize(kwrd_orth, kwrd_base, kwrd_ctag, kwrd_spaces,
                                                               kwrd_category, debug);

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
                output << "[" << setw(4) << line_no << "]" << "\t" << "True" << "\t" << orthprnt << "\t" << lemmaprnt
                       << "\t"
                       << kwrdprnt << "\t" << kwrd_category
                       << "\t" << globalMethod << "\t" << basesprnt << " \t" << ctagprnt << "\n";
                cout << "[" << setw(4) << line_no << "]" << "\t" << "True" << "\t" << orthprnt << "\t" << lemmaprnt
                     << "\t" << kwrdprnt
                     << "\t" << kwrd_category
                     << "\t" << globalMethod << "\t" << basesprnt << " \t" << ctagprnt << "\n";
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
                output << "[" << setw(4) << line_no << "]" << "\t" << "False" << "\t" << orthprnt << "\t" << lemmaprnt
                       << "\t"
                       << kwrdprnt << "\t" << kwrd_category
                       << "\t" << globalMethod << "\t" << basesprnt << " \t" << ctagprnt << "\n";
                cout << "[" << setw(4) << line_no << "]" << "\t" << "False" << "\t" << orthprnt << "\t" << lemmaprnt
                     << "\t"
                     << kwrdprnt << "\t" << kwrd_category
                     << "\t" << globalMethod << "\t" << basesprnt << " \t" << ctagprnt << "\n";

            }
            line_no++;
        }
        printResults(output, "Evaluation by method", tfByMethod, line_no);
        printResults(output, "Evaluation by keyword category", tfByCategory, line_no);
        infile.close();
        output.close();


    }catch(std::logic_error &err){
        std::cout << "No or insufficient parameters given"<<endl;
        std::cout << "Run with -h to see help" <<endl;
        std::cout << err.what();
    }

    return 0;
}

