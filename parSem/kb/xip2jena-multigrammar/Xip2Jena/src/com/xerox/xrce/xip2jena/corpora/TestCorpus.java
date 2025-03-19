/*
 * Copyright Xerox 2009
 */

package com.xerox.xrce.xip2jena.corpora;

import com.xerox.xrce.xip2jena.corpus.FilesCorpus;
import common.Options;
import java.io.File;
import java.util.ArrayList;
import java.util.List;

/** A corpus consisting of a single file with the relative path test/test
 *
 */
public class TestCorpus extends FilesCorpus {
    public TestCorpus() {
        List<File> infiles = new ArrayList<File>(1);
        infiles.add(new File("test/test"));
        init(infiles);
    }
    @Override
    public common.corpora.Corpus getTemporalCorpus(Options o) {
        return new TestTemporalCorpus(o);
    }
}

class TestTemporalCorpus extends common.corpora.Corpus {
    public TestTemporalCorpus(Options o){}

}
