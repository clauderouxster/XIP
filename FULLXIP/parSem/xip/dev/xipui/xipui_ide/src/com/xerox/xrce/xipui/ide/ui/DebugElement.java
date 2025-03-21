/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * DebugElement.java
 *
 * Created on March 1, 2006, 9:58 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.ide.ui;

import com.xerox.xrce.xipui.CorpusElement;
import com.xerox.xrce.xipui.util.FileTools;
import java.io.*;

/**
 *
 * @author vgrassau
 */
public class DebugElement {

    /**
     *CorpusElement contenant la phrase a deboguer, util pour connaitre l'origine de la phrase
     */
    private CorpusElement ce;
    /**
     *Pharse a deboguer
     */
    private String sentence;
    /**
     *global index
     */
    private int index;
    /**
     *This index is the number of the XipUnit for a CorpusElement and not a global index
     */
    private int unitIndex;

    /** 
     * Creates a new instance of DebugElement 
     * @param ce the corpus element parsed
     * @param sentence the sentence parsed (this sentence can be just a description or the  XipUnit leaves 
     *        of the real sentence parsed )
     * @param index the global index, not the index of the sentence
     */
    public DebugElement(CorpusElement ce, String sentence, int index) {
        this.index = index;
        this.ce = ce;
        this.sentence = sentence;
    }

    /**
     *Index for the sentence in the CorpusElement, i.e. the number of the Xip unit.
     *@param num the number of the xip unit for the CorpusElement
     *
     */
    public void setUnitIndex(int num) {
        this.unitIndex = num;
    }

    /**
     *Return the unit index of this debug element 
     *@return a the XipUnit index for the CorpusElement containg this debug element
     *
     */
    public int getUnitIndex() {
        return this.unitIndex;
    }

    /**
     *Return the CorpusElement
     *@return a CorpusElement
     */
    public CorpusElement getCorpusElement() {
        return this.ce;
    }

    /**
     *Return the file name containing this debug element 
     *@return a filename
     *@see com.xerox.xrce.xipui.CorpusElement#getFile()
     */
    public String getFileName() {
        return getCorpusElement().getFile().getName();
    }

    /**
     *Return the encoding to use
     *@return the encoding of the CorpusElement containing this sentence
     */
    public String getEncoding() {
        return getCorpusElement().getEncoding();
    }

    /**
     *Return the sentence for this DebugElement, it's only a 
     *view of the sentence, not really the sentence that will be parsed.
     *Don't use this method to have a String and parse this String. 
     *@return a String representation of the sentence.
     */
    public String getSentence() {
        return this.sentence;
    }

    /**
     *Return the global index
     *@return a number
     */
    public int getIndex() {
        return this.index;
    }

    /**
     *Return a String representation of this debug element
     *@return a string containing the global index and the file name 
     */
    public String toString() {
        String name = this.getFileName();
        if (name.startsWith("input_")) {
            return getIndex() + " : input.txt";
        } else {
            return getIndex() + " : " + name;
        }
    }

    /**
     * This method return a temporary File containg only the sentence to be parsed.
     * The file created will be deleted at the end of the application. 
     * The file is also written using the encoding attribute of a CorpusElement. 
     * As, a xip analysis can be link with the context of a the CorpusElement, and, the 
     * getSentence method doesn't return the real sentence, you should not use this method. 
     * Use the com.xerox.xrce.xipui.ide.DebugXip object instead with the debug(int, DebugElement) method. 
     *@return the File containing only one sentence 
     * 
     * @throws java.lang.Exception 
     * @deprecated
     */
    public File getFileForParse() throws Exception {
        File tmp = File.createTempFile(ce.getFile().getName() + getIndex(), ".txt");
        tmp.deleteOnExit();
        FileTools.storeString(getSentence(), tmp.getAbsolutePath(), ce.getEncoding());
        return tmp;
    }

    public boolean equals(Object o) {
        if (o instanceof DebugElement) {
            DebugElement de = (DebugElement) o;
            return getSentence().equals(de.getSentence());
        } else {
            return false;
        }
    }
}
