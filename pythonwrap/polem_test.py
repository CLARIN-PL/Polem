import WrapLem


lem = WrapLem.CascadeLemmatizer.assembleLemmatizer()

orths = "Lidze światowej"
lemmas = "liga światowy"
tags = "subst:sg:loc:f adj:sg:loc:f:pos"

phrase_lemma = lem.lemmatizeS(orths, lemmas, tags, False)

print(f"Orths : {orths}")
print(f"Lemmas: {lemmas}")
print(f"Tags  : {tags}")
print(f"Phrase lemma: {phrase_lemma}")
