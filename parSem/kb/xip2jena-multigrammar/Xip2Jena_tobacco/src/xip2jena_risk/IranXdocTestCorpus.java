/*
 * Copyright Xerox 2009
 */

package xip2jena_risk;

import com.xerox.xrce.xip2jena.corpora.IranCorpus;
import java.io.File;
import java.util.ArrayList;
import java.util.List;

public class IranXdocTestCorpus extends IranCorpus {
    public IranXdocTestCorpus() {
        List<File> infiles = new ArrayList<File>();
        infiles.add(new File("/home/nlagos/test_data/iran-corpus/295_20050902"));
        infiles.add(new File("/home/nlagos/test_data/iran-corpus/296_20050903"));
        init(infiles);
    }

}
