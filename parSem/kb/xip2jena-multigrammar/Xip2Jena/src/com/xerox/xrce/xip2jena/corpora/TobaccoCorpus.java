/*
 * Copyright Xerox 2009
 */
package com.xerox.xrce.xip2jena.corpora;

import com.xerox.xrce.xip2jena.corpus.FilesCorpus;
import com.xerox.xrce.xip2jena.Util;
import common.Options;
import java.io.File;
import java.util.ArrayList;
import java.util.List;

public class TobaccoCorpus extends FilesCorpus {
    public TobaccoCorpus() {
        List<File> infiles = new ArrayList<File>();
        infiles.addAll(Util.listFiles(new File("/opt/WPT/wptparsem/ToolsLitigation/Knowledge_base/Corpora/Tobacco_clean_strict")));
        init(infiles);
    }
    @Override
    public common.corpora.Corpus getTemporalCorpus(Options o) {
        return new TobaccoTemporalCorpus(o);
    }
}

class TobaccoTemporalCorpus extends common.corpora.Corpus {
    public TobaccoTemporalCorpus(Options o){}

    /**
     * No DCT for multi-file Tobacco corpus
     */
}
