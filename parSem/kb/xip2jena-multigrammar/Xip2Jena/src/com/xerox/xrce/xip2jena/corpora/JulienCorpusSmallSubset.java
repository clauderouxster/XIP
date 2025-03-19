/*
 * Copyright Xerox 2009
 */
package com.xerox.xrce.xip2jena.corpora;

import java.io.File;
import java.util.ArrayList;
import java.util.List;


public class JulienCorpusSmallSubset extends JulienCorpus {
    public JulienCorpusSmallSubset() {
        List<File> infiles = new ArrayList<File>();
        File dir = new File("/opt/ParSem/Corpora/ENGLISH/corpus_news/ap/filtr_text/world");
        infiles.add(new File(dir, "AFGHAN_ALWAYS_OPIUM?SITE=MABED&amp;SECTION=HOME&amp;TEMPLATE=DEFAULT"));
        infiles.add(new File(dir, "AFGHAN_DEATH_SENTENCE?SITE=WILAC&amp;SECTION=HOME&amp;TEMPLATE=DEFAULT"));
        infiles.add(new File(dir, "AFGHANISTAN?SITE=KTVB&amp;SECTION=HOME&amp;TEMPLATE=DEFAULT"));
        infiles.add(new File(dir, "AFGHANISTAN?SITE=MAQUI&amp;SECTION=HOME&amp;TEMPLATE=DEFAULT"));
        infiles.add(new File(dir, "AFGHANISTAN?SITE=MOJOP&amp;SECTION=HOME&amp;TEMPLATE=DEFAULT"));
        infiles.add(new File(dir, "AFGHANISTAN?SITE=NCBER&amp;SECTION=HOME&amp;TEMPLATE=DEFAULT"));
        infiles.add(new File(dir, "AFGHANISTAN?SITE=NVREN&amp;SECTION=HOME&amp;TEMPLATE=DEFAULT"));
        infiles.add(new File(dir, "AFGHAN_MARINE_SHOOTING?SITE=NJMOR&amp;SECTION=HOME&amp;TEMPLATE=DEFAULT"));
        infiles.add(new File(dir, "AFGHAN_NATO_COMMANDER?SITE=KYB66&amp;SECTION=HOME&amp;TEMPLATE=DEFAULT"));
        init(infiles);
    }
}
