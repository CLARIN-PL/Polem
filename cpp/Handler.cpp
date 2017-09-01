//
// Created by gkubon on 01/09/17.
//

#include "Handler.h"
#include "CascadeLemmatizer.h"


icu::UnicodeString
Handler::filter(std::vector<std::vector<std::string>> kw, icu::UnicodeString lemma, std::string kw_category) {


    lemma = lemma.trim();
    if (globalMethod == "NamLivPersonLemmatizer::Dictionary" &&
        (lemma.indexOf("Teofil") != -1 || lemma.indexOf("Jan") != -1)) {
        for (auto it:kw) {
            if (it[0].find("J") == 0 && it[2].find(":m") != std::string::npos && lemma.indexOf("Teofil") != -1) {
                std::string view;
                lemma.toUTF8String(view);
                lemma.findAndReplace("Teofil", "Józef");
                lemma.toUTF8String(view);
                break;
            }
            if (it[0].find("Michał") != std::string::npos && it[2].find(":m") != std::string::npos
                && lemma.indexOf("Jan") != -1) {
                bool m = it[0].front() == ('M');
                std::string view;
                lemma.toUTF8String(view);
                lemma.findAndReplace("Jan", "Michał");
                lemma.toUTF8String(view);
                break;
            }
        }
    } else if (globalMethod.find("RuleLemmatizer") == 0) {


        if ((lemma.indexOf("´") > 1 && kw.size() == 1) ||
            (lemma.indexOf("´") > 1 && kw.size() > 1 && lemma.indexOf("´") > lemma.lastIndexOf(" "))) {
            lemma = lemma.tempSubStringBetween(0, lemma.indexOf("´"));
        } else if ((lemma.indexOf("'") > 1 && kw.size() == 1) ||
                   (lemma.indexOf("'") > 1 && kw.size() > 1 && lemma.indexOf("'") - 2 > lemma.lastIndexOf(" "))) {
            lemma = lemma.tempSubStringBetween(0, lemma.indexOf("'"));
        } else if (lemma.indexOf("’") > -1 && kw_category.find("nam_pro") != 0) {
            UnicodeString part1 = lemma.tempSubStringBetween(0, lemma.indexOf("’"));
            UnicodeString part2 = lemma.tempSubStringBetween(lemma.indexOf("’"), lemma.length());
            if (part2.indexOf(" ") != -1) {
                part2 = part2.tempSubStringBetween(part2.indexOf(" "), part2.length());
                lemma = part1.append(part2);
            } else {
                lemma = part1;
            }
        }

        if (lemma.indexOf(",") != -1) {
            lemma.findAndReplace(" ,", ",");
            lemma.findAndReplace(",", ", ");
        } else if (lemma.indexOf(":") != -1) {
            lemma.findAndReplace(" :", ":");
            lemma.findAndReplace(":", ": ");
        } else if (lemma.indexOf("'") != -1) {
            lemma.findAndReplace(" '", "'");
            lemma.findAndReplace("'", "' ");
        } else if (lemma.indexOf("+") != -1) {
            lemma.findAndReplace(" +", "+");
            lemma.findAndReplace("+", "+ ");
        } else if (lemma.indexOf(".") != -1) {
            lemma.findAndReplace(" .", ".");
        } else if (lemma.indexOf("-") != -1) {
            lemma.findAndReplace(" -", "-");
        } else if (lemma.indexOf("’") != -1) {
            lemma.findAndReplace(" ’", "’");
        } else if (lemma.indexOf("\"") != -1 && lemma.lastIndexOf("\"") != lemma.indexOf("\"")) {
            UnicodeString part = "";
            lemma.extractBetween(lemma.indexOf("\""), lemma.lastIndexOf("\""), part);
            part.findAndReplace(0, 3, " ", "");
            part.findAndReplace(part.length() - 3, part.length(), " ", "");
            part.insert(0, " ");
            lemma.handleReplaceBetween(lemma.indexOf("\""), lemma.lastIndexOf("\""), part);
        }


    }

    if (lemma.indexOf(" -") != -1) {
        lemma.findAndReplace(" -", "-");
    } else if (lemma.indexOf(" .") != -1) {
        lemma.findAndReplace(" .", ".");
    } else if (lemma.indexOf(" +") != -1) {
        lemma.findAndReplace(" +", "+");
    } else if (lemma.indexOf(" ?") != -1) {
        lemma.findAndReplace(" ?", "?");
    } else if (lemma.indexOf(" )") != -1) {
        lemma.findAndReplace(" )", ")");
    } else if (lemma.indexOf(" :") != -1) {
        lemma.findAndReplace(" :", ":");
    } else if (lemma.indexOf(" \"") != -1) {
        lemma.findAndReplace(" \"", "\"");
    }

    if (lemma.indexOf("- ") != -1) {
        lemma.findAndReplace("- ", "-");
    } else if (lemma.indexOf("# ") != -1) {
        lemma.findAndReplace("# ", "#");
    } else if (lemma.indexOf("+ ") != -1) {
        lemma.findAndReplace("+ ", "+");
    } else if (lemma.indexOf("( ") != -1) {
        lemma.findAndReplace("( ", "(");
    } else if (lemma.indexOf("\" ") != -1) {
        lemma.findAndReplace("\" ", "\"");
    }


    if (lemma.indexOf("ii") != -1 && lemma.indexOf(" ") == -1) {
        lemma.findAndReplace("ii", "ia");
    }


    if (lemma.indexOf(" ") == -1 && lemma.endsWith("ego")) {
        lemma.findAndReplace("ego", "");
    }

    if (lemma.endsWith("scy") && kw_category.find("nam_liv_person") == std::string::npos) {
        lemma.findAndReplace("scy", "ski");
    }

    if (lemma.endsWith("ę")) {
        lemma.findAndReplace("ę", "a");
    }

    if (lemma.endsWith("ą")) {
        lemma.findAndReplace("ą", "a");
    }

    if (kw_category == "nam_loc_gpe_city" && lemma.endsWith("u")) {
        lemma = lemma.tempSubString(0, lemma.length() - 1);
    }

    if (kw_category == "nam_liv_god" && lemma.endsWith("em")) {
        lemma.findAndReplace("em", "");
    }

    if (lemma.endsWith("owi")) {
        lemma.findAndReplace("owi", "");
    }

    lemma.findAndReplace("  ", " ");
    return lemma;

}

icu::UnicodeString Handler::sensitivityModule(std::vector<icu::UnicodeString> lemmas, Corpus2::Sentence::Ptr sentence,
                                              std::string kw_category) {

    UnicodeString lemma = "";

    for (int i = 0; i < lemmas.size(); ++i) {

        int sameLetter = 0;
        int sameLetterCaseIns = 0;
        int isAlfa = 0;
        std::string a, b;
        lemmas[i].toUTF8String(a);
        sentence->tokens()[i]->orth().toUTF8String(b);
        int ii = 0;
        for (ii = 0; ii < lemmas[i].length() && ii < sentence->tokens()[i]->orth().length(); ++ii) {
            wchar_t inchar, outchar;
            wchar_t inlowchar, outlowchar;
            inchar = sentence->tokens()[i]->orth().charAt(ii);
            outchar = lemmas[i].charAt(ii);
            UnicodeString xyz = sentence->tokens()[i]->orth();
            inlowchar = xyz.toLower().charAt(ii);
            outlowchar = lemmas[i].toLower().charAt(ii);
            if (outlowchar >= 'a' && outlowchar <= 'z') {
                isAlfa++;
            }
            if (kw_category.find("nam_adj") == 0) {
                lemma.append(outchar);
            } else if (inchar == outchar) {
                lemma.append(outchar);
                sameLetter++;
                sameLetterCaseIns++;
            } else if (outlowchar == inlowchar && kw_category.find("nam_adj") != 0) {
                lemma.append(inchar);
                sameLetterCaseIns++;
            } else {
                lemma.append(outchar);
            }
        }
        if (ii < lemmas[i].length()) {
            for (ii; ii < lemmas[i].length(); ++ii) {
                lemma.append(lemmas[i].trim().charAt(ii));
            }
        }
        lemma.append(" ");
    }
    return lemma;
}
