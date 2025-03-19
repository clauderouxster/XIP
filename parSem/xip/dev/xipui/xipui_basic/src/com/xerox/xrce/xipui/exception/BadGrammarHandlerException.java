/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * BadGrammarHandlerException.java
 *
 * Created on July 6, 2006, 4:46 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.exception;

/**
 *
 * @author vgrassau
 */
public class BadGrammarHandlerException extends Exception {

    private int grammarHandler = -1;

    /** Creates a new instance of BadGrammarHandlerException
     * @param num 
     */
    public BadGrammarHandlerException(int num) {
        grammarHandler = num;
    }

    public String toString() {
        return "Bad grammar handler : " + grammarHandler;
    }

    public int getGrammarHandler() {
        return grammarHandler;
    }
}
