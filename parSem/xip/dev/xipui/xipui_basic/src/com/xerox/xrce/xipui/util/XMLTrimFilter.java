/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * XMLTrimFilter.java
 *
 * Created on May 3, 2006, 2:28 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.util;

import org.xml.sax.helpers.*;
import org.xml.sax.*;
import java.io.*;

/**
 *
 * @author vgrassau
 */
public class XMLTrimFilter extends XMLFilterImpl {

    private CharArrayWriter contents = new CharArrayWriter();

    public XMLTrimFilter(XMLReader parent) {
        super(parent);
    }

    public void startElement(String uri, String localName, String qName, Attributes atts) throws SAXException {
        writeContents();
        super.startElement(uri, localName, qName, atts);
    }

    public void characters(char ch[], int start, int length) {
        contents.write(ch, start, length);
    }

    public void endElement(String uri, String localName, String qName) throws SAXException {
        writeContents();
        super.endElement(uri, localName, qName);
    }

    public void ignorableWhitespace(char ch[], int start, int length) {
    }

    private void writeContents() throws SAXException {
        char ch[] = contents.toCharArray();
        if (!isWhiteSpace(ch)) {
            super.characters(ch, 0, ch.length);
        }
        contents.reset();
    }

    private boolean isWhiteSpace(char ch[]) {
        for (int i = 0; i < ch.length; i++) {
            if (!Character.isWhitespace(ch[i])) {
                return false;
            }
        }
        return true;
    }
} 


    

