/*
   Copyright (C) Wrocław University of Science and Technology (PWr), 2017-2018.
   Grzegorz Kuboń, Michał Marcińczuk.

   Part of Polem project.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
 */

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
