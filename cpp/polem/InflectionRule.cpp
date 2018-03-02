#include "InflectionRule.h"

using namespace std;

/**
 * Creates a specific form for given form according to the inflection rule.
 * @param form
 * @return
 */
icu::UnicodeString InflectionRule::inflectForm(icu::UnicodeString form) {
    icu::UnicodeString newForm = form;
    if (!this->formEnding.isEmpty()) {
        newForm = form.tempSubStringBetween(0, form.lastIndexOf(this->formEnding)) + this->normEnding;
    }
    return newForm;
}
