Polem
=====
Polem is a tool for multi-word phrase lemmatization for Polish language. It was desgined to lemmatize multi-word noun common phrases and categorized named entities. More information about the algorithm can be found in the following article:

```bibtex
@inproceedings{DBLP:conf/ranlp/Marcinczuk17,
  author    = {Michal Marcinczuk},
  editor    = {Ruslan Mitkov and
               Galia Angelova},
  title     = {Lemmatization of Multi-word Common Noun Phrases and Named Entities
               in Polish},
  booktitle = {Proceedings of the International Conference Recent Advances in Natural
               Language Processing, {RANLP} 2017, Varna, Bulgaria, September 2 -
               8, 2017},
  pages     = {483--491},
  publisher = {{INCOMA} Ltd.},
  year      = {2017},
  url       = {https://doi.org/10.26615/978-954-452-049-6_064},
  doi       = {10.26615/978-954-452-049-6_064},
  timestamp = {Tue, 09 Jan 2018 14:09:59 +0100},
  biburl    = {http://dblp.org/rec/bib/conf/ranlp/Marcinczuk17},
  bibsource = {dblp computer science bibliography, http://dblp.org}
}
```

Contributors
-------------
* Michał Marcińczuk <<marcinczuk@gmail.com>> -- originator of the lemmatization algorithm and contact person,
* Grzegorz Kuboń -- implementation.

Acknowledgments
---------------
Work financed as part of the investment in the CLARIN-PL research infrastructure funded by the Polish Ministry of Science and Higher Education


Installation
============

Below is an instruction how to install the tool on a Linux.
Polem is provided as a source code package and must be compiled. The build system is based on CMake.
Builded system provides executable that is installed to bin folder and shared library libpolem-dev.so that can be used in C/C++.

Requirements
------------

Before running the build system, you need to ensure you have the following libraries installed with headers.

* Boost (tested with 1.58)
   ```bash
   sudo apt-get install libboost1.58-all-dev
   ```
* ICU library (tested with 55.1; Ubuntu package libicu-dev)
   ```bash
   sudo apt-get install libicu-dev
   ```
* CMake 3.2 or later
   ```bash
   sudo apt-get install cmake
   ```
* Corpus2
   ```bash
   git clone git@nlp.pwr.wroc.pl:corpus2
   cd corpus2
   mkdir bin
   cd bin
   cmake ..
   make
   sudo make install
   sudo ldconfig
   ```
* Morfeusz2 form generator
    ```bash
    wget -O morfeusz2-2.0.0-Linux-amd64.deb https://nextcloud.clarin-pl.eu/index.php/s/VVIvx4w20azcWbp/download
    sudo dpkg -i morfeusz2-2.0.0-Linux-amd64.deb
    ``` 


Polem
-----

Having installed the dependencies, install Polem:

```bash
mkdir polem/cpp/build
cd polem/cpp/build
cmake ..
make
sudo make install
```

Usage
====
```bash
polem -h
```

```bash
Parameters:
  -h [ --help ]             Print help message
  -p [ --pathname ] arg     Pathname to input file - REQUIRED
  -f [ --files ] arg        Full pathname to data files for lemmatizer, 
                            required when lemmatizer is not installed
  -t [ --tagset ] arg       Tagset to use (Currently only NKJP)- OPTIONAL
  -c [ --case-sensitive ]   Case sensitive evaluation - OPTIONAL
  -s [ --space-sensitive ]  Whitespace sensitive evaluation - OPTIONAL
  -d [ --debug ]            Additional debug prints
```

Testing
-------

The `./data/` folder contains train and test sets. To evaluate the Polem lemmatization model on the gold standard data, run the following command:

```bash
polem -p data/kpwr-1.1-fixed-keywords-test.txt
```

Output:
```bash
(skipped)
[ 994]	True	XV BK	XV BK	xv bk		RuleLemmatizer:SubstAdj_Flex-0	xv bk 	adj:sg:nom:m3:pos subst:sg:nom:m3
[ 995]	True	Copyright Alliance	copyright Alliance	copyright alliance		RuleLemmatizer:SubstSubst_Agr12-nmb-gnd-cas-0	copyright alliance 	subst:sg:nom:m3 subst:sg:nom:m3
[ 996]	True	Nierozsądne śluby	nierozsądne śluby	nierozsądne śluby		RuleLemmatizer:SubstAdj_Flex-0	nierozsądny ślub 	adj:pl:nom:m3:pos subst:pl:nom:m3
[ 997]	False	Stacje telewizyjne	stacje telewizyjne	stacja telewizyjna		RuleLemmatizer:SubstAdj_Agr12-nmb-gnd-cas-0	stacja telewizyjny 	subst:pl:nom:f adj:pl:nom:f:pos
[ 998]	True	Prawo budowlane	prawo budowlane	prawo budowlane		RuleLemmatizer:SubstAdj_Agr12-nmb-gnd-cas-0	prawo budowlany 	subst:sg:nom:n adj:sg:nom:n:pos
[ 999]	True	gminy żydowskiej	gmina żydowska	gmina żydowska		RuleLemmatizer:SubstAdj_Agr12-nmb-gnd-cas-0	gmina żydowski 	subst:sg:gen:f adj:sg:gen:f:pos
[1000]	True	Turniej finałowy	turniej finałowy	turniej finałowy		RuleLemmatizer:SubstAdj_Agr12-nmb-gnd-cas-0	turniej finałowy 	subst:sg:nom:m3 adj:sg:nom:m3:pos
------------------------------------------------------------
# Evaluation by method
%------------------------------------------------------------
\hline
 True & False &    Acc & Method                         & Coverage \\
%------------------------------------------------------------
\hline
   43 &     6 &  87.76\% & OrthLemmatizer                 &   4.67\% \\
   12 &     0 & 100.00\% & RuleLemmatizer:AdjSubstAdj\_Agr123-nmb-gnd-cas-0 &   1.18\% \\
    6 &     0 & 100.00\% & RuleLemmatizer:AdjSubstSubst\_Gen-0 &   0.60\% \\
    8 &     1 &  88.89\% & RuleLemmatizer:AdjSubstTail-0  &   0.89\% \\
    3 &     0 & 100.00\% & RuleLemmatizer:Num2-0          &   0.30\% \\
    9 &     1 &  90.00\% & RuleLemmatizer:Num3-0          &   0.99\% \\
    1 &     0 & 100.00\% & RuleLemmatizer:NumAndNum-0     &   0.10\% \\
    1 &     0 & 100.00\% & RuleLemmatizer:SubstAdjAdjFlex-0 &   0.10\% \\
    1 &     0 & 100.00\% & RuleLemmatizer:SubstAdjSubstAdj\_Gen-0 &   0.10\% \\
    8 &     0 & 100.00\% & RuleLemmatizer:SubstAdjSubst\_Agr12-nmb-gnd-cas-0 &   0.79\% \\
    1 &     0 & 100.00\% & RuleLemmatizer:SubstAdjSubst\_Agr123case-0 &   0.10\% \\
    6 &     0 & 100.00\% & RuleLemmatizer:SubstAdjSubst\_Agr23-nmb-ger-cas-0 &   0.60\% \\
   36 &     8 &  81.82\% & RuleLemmatizer:SubstAdjTail-0  &   4.21\% \\
    2 &     0 & 100.00\% & RuleLemmatizer:SubstAdj\_Agr12-nmb-cas\_Sg\_FixGndN-0 &   0.20\% \\
  232 &    37 &  86.25\% & RuleLemmatizer:SubstAdj\_Agr12-nmb-gnd-cas-0 &  21.18\% \\
   42 &     1 &  97.67\% & RuleLemmatizer:SubstAdj\_Flex-0 &   4.12\% \\
    2 &     3 &  40.00\% & RuleLemmatizer:SubstAdj\_SgPl\_FixNmbPl-0 &   0.50\% \\
    8 &     2 &  80.00\% & RuleLemmatizer:SubstAndSubs-0  &   0.99\% \\
    1 &     0 & 100.00\% & RuleLemmatizer:SubstAndSubstAdj\_Agr134-nmb-gnd-cas-0 &   0.10\% \\
    6 &     4 &  60.00\% & RuleLemmatizer:SubstHyphenSubst-0 &   0.99\% \\
   26 &     3 &  89.66\% & RuleLemmatizer:SubstSubstAdj-0 &   2.82\% \\
   19 &     0 & 100.00\% & RuleLemmatizer:SubstSubstSubst\_GenGen-0 &   1.86\% \\
  142 &     6 &  95.95\% & RuleLemmatizer:SubstSubst\_Agr12-nmb-gnd-cas-0 &  12.88\% \\
  123 &    12 &  91.11\% & RuleLemmatizer:SubstSubst\_GenSg\_Fix-0 &  11.88\% \\
    4 &    11 &  26.67\% & RuleLemmatizer:SubstSubst\_Gen\_Fix-0 &   1.48\% \\
  140 &    23 &  85.89\% & RuleLemmatizer:SubstTail-0     &  14.00\% \\
%------------------------------------------------------------
\hline
  882 &   118 &  88.20\% & Total                          \\
%------------------------------------------------------------
\hline
------------------------------------------------------------
# Evaluation by keyword category
%------------------------------------------------------------
\hline
 True & False &    Acc & Method                         & Coverage \\
%------------------------------------------------------------
\hline
  882 &   118 &  88.20\% &                                &  49.98\% \\
%------------------------------------------------------------
\hline
  882 &   118 &  88.20\% & Total                          \\
%------------------------------------------------------------

```

C++ API
=======

Overview
--------

To use a lemmatizer in code include `CascadeLemmatizer.h`.

`CascadeLemmatizer.h`

```cpp
static CascadeLemmatizer::assembleLemmatizer() :
static function that creates different lemmatizers and connect them together into CascadeLemmatizer object.
```

`UnicodeString lemmatize(UnicodeString kwrd_orth, UnicodeString kwrd_base, UnicodeString kwrd_ctag, UnicodeString kwrd_spaces, std::string category)`:
Function that takes orth, base, ctag, spaces and category of a phrase to find and return lemmatised phrase.
If lemmatised phrase can't be found, function return orth (1st parameter). Spaces parameter determines if there is a space in phrase after each word.
Spaces and category are optional. Variants of the `lemmatize` method:

* 3 parameters: orths, bases, ctags,
* 4 parameters: orths, bases, ctags, spaces,
* 5 parameters: orths, bases, ctags, spaces, semantic category.

Example usage :

```cpp
CascadeLemmatizer lemmatizer = CascadeLemmatizer::assembleLemmatizer();

UnicodeString lemma1 = lemmatizer.lemmatize("Lidze światowej","liga światowy","subst:sg:loc:f adj:sg:loc:f:pos");

UnicodeString lemma2 = lemmatizer.lemmatize("Lidze światowej","liga światowy","subst:sg:loc:f adj:sg:loc:f:pos","True False");

UnicodeString lemma3 = lemmatizer.lemmatize("Lidze światowej","liga światowy","subst:sg:loc:f adj:sg:loc:f:pos","True False","nam_eve_human_sport");
```

Import in a project
--------------------

There is FindPolem.cmake script provided in `polem/cpp/cmake/Modules/` that can be used to include Polem in the cpp project.

Inclusion in the `CMakeLists.txt` file:

```cmake
find_package(Polem REQUIRED)
include_directories(${Polem_INCLUDE_DIR})
set(LIBS ${LIBS} ${Polem_LIBRARY})
```

Java wrapper
============

Overview
--------

After building the source code a `PolemJava.jar` file is created in the `polem/cpp/build` folder. The jar contains two classes:
* `g419.polem.CascadeLemmatizer` -- provides a static method called `assembleLemmatizer()` which create a default lemmatizer. The lemmatizer has a method called `lemmatizeS` to generate a lemma for given phrase.
* `g419.polem.WrapLem` -- provides a statis method to obtain the method used to generate the lemma for the last call of `lemmatizeS`.

Sample Java file
----------------

The `javawrap` folder contains a sample java file named `Main.java` which presents who to use the Java wrapper. Below is an instruction how to run in:

1. Compile and install Polem (see section Installation)
2. Compile `Main.java`
   ```bash
   cd polem/javawrap
   javac -cp ../build/PolemJava.jar Main.java
   ```
3. Run `Main.class`
   ```bash
   java -Djava.library.path=/usr/local/lib/ -cp .:../build/PolemJava.jar Main
   ```

Usage in the code
-----------------
```java
import g419.polem.CascadeLemmatizer;
import g419.polem.WrapLem;

public class Main {

    public static void main(String[] args) {
        // Load the Polem library
        System.loadLibrary("PolemJava");
        // Create the lemmatize
        CascadeLemmatizer casc = CascadeLemmatizer.assembleLemmatizer();
        // Call the lemmatize method
        System.out.println(casc.lemmatizeS("Lidze światowej","liga światowy","subst:sg:loc:f adj:sg:loc:f:pos","True False","nam_eve_human_sport",false));
        // Print what method was used to generate the lemma
        System.out.println(WrapLem.getGlobalMethod());
    }
}
```


License
=======
Copyright (C) Wrocław University of Science and Technology (PWr), 2017-2018. 
All rights reserved.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <<http://www.gnu.org/licenses/>>.

