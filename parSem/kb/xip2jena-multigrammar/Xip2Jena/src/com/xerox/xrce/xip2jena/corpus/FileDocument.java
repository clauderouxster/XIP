/*
 * Copyright Xerox 2009
 */

package com.xerox.xrce.xip2jena.corpus;

import java.io.File;

public class FileDocument implements Document {

    private File file;

    public FileDocument(File file) {
        this.file = file;
    }

    @Override
    public File getFile() {
        return file;
    }

}
