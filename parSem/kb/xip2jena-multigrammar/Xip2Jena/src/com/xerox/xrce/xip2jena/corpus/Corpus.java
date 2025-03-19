/*
 * Copyright Xerox 2009
 */

package com.xerox.xrce.xip2jena.corpus;

import common.Options;

public interface Corpus extends Iterable<Document> {
    common.corpora.Corpus getTemporalCorpus(Options o);
}
