import g419.polem.CascadeLemmatizer;
import g419.polem.WrapLem;

public class Main {

    public static void main(String[] args) {
        System.loadLibrary("PolemJava");
        CascadeLemmatizer casc = CascadeLemmatizer.assembleLemmatizer();
        System.out.println(casc.lemmatizeS("Lidze światowej","liga światowy","subst:sg:loc:f adj:sg:loc:f:pos","True False","nam_eve_human_sport",false));
        System.out.println(WrapLem.getGlobalMethod());
    }
}
