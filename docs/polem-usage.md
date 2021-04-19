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