/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * ParseFileException.java
 *
 * Created on July 10, 2006, 3:21 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.exception;

import com.xerox.xrce.xipui.CorpusElement;

/**
 *
 * @author vgrassau
 */
public class ParseFileException extends Exception {

    private String encoding;
    private String file;
    private String corpusParent;

    /** Creates a new instance of ParseFileException
     * @param e 
     */
    public ParseFileException(CorpusElement e) {
        encoding = e.getEncoding();
        file = e.getFile().getAbsolutePath();
        corpusParent = e.getParent();
    }

    public String getFile() {
        return file;
    }

    public String getCorpusParent() {
        return corpusParent;
    }

    public String getEncoding() {
        return this.encoding;
    }

    public String toString() {
        return "Error in parsing with file : " + file + "\n encoding : " + encoding + "\n parent : " + corpusParent;
    }
}
