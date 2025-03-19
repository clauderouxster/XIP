/*
 * Copyright Xerox 2009
 */

package com.xerox.xrce.xip2jena.corpus;

import com.xerox.xrce.xip2jena.corpus.Corpus;
import java.io.File;
import java.util.Iterator;
import java.util.List;

/**
 * A partial implementation of the Corpus interface for corpora that are
 * stored one file per document.
 */
public abstract class FilesCorpus implements Corpus {
    private List<File> files;

    /* This initialization is in an init method rather than the
     * constructor because subclasses' constructors need to call
     * this class's constructor before they have a chance to build
     * the file list.
     */
    protected void init(List<File> files) {
        this.files = files;
    }

    @Override
    public Iterator<Document> iterator() {
        if (files == null) {
            throw new IllegalStateException("FilesCorpus not initialized");
        }
        return new Iterator<Document>() {
            private Iterator<File> fileIterator;
            { fileIterator = files.iterator(); }
            @Override
            public boolean hasNext() {
                return fileIterator.hasNext();
            }

            @Override
            public Document next() {
                return new FileDocument(fileIterator.next());
            }

            @Override
            public void remove() {
                throw new UnsupportedOperationException();
            }
        };
    }
}
