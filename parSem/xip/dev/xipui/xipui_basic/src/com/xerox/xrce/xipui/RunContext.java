/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 ****************************************************************************************
 * Copyright (c) 2005 Xerox Corporation                                                 *
 * Xerox Research Centre Europe - Grenoble                                              *
 * All Rights Reserved                                                                  *
 * Copyright protection claimed includes all forms and matters of copyrightable         *
 * material and information now allowed by statutory or judicial law or                 *
 * hereinafter granted, including without limitation, material generated from           *
 * the software programs which are displayed on the screen such as icons,               *
 * screen display looks, etc. All rights reserved. No part of this document             *
 * may be reproduced, stored in a retrieval system, or transmitted, in any              *
 * form or by any means, electronic, mechanical, photocopied, recorded, or              *
 * otherwise, without prior written consent from XEROX corporation.                     *
 ****************************************************************************************
 */
package com.xerox.xrce.xipui;

import com.xerox.xrce.xipui.util.Context;
import com.xerox.xrce.xipui.util.XML;
import java.util.*;
import java.io.*;
import org.dom4j.*;

/**
 * Cette classe est vraiment a l'etat de prototype, et doit etre modifiee. Afin de faire une reelle gestion de contexte
 *
 * @author vgrassau
 */
public class RunContext extends Context {

    public static final String CORPUS = "CORPUS";
    public static final String ID_RESULT = "NAME_RESULT";
    public static final String REQUEST = "REQUEST";
    public static final String xml_mode = "xml_mode";
    public static final String single_mode = "single_mode";
    public static final String hmm_mode = "hmm_mode";
    public static final String indent_file = "indent_file";
    public static final String ntm_only = "ntm_only";
    public static final String column = "column";
    public static final String grammar_limit = "grammar_limit";
    private Hashtable displayTable;
    private File setupFile;
    private Vector keysToKeep;
    private int gHandler;
    private Xip xip;

    /** Creates a new instance of RunContext
     * @param xip
     * @param setupFile 
     */
    public RunContext(Xip xip, File setupFile) {
        super();
        this.xip = xip;
        this.setupFile = setupFile;
        displayTable = new Hashtable();
        keysToKeep = new Vector();
        keysToKeep.add(ntm_only);
        keysToKeep.add(single_mode);
        keysToKeep.add(hmm_mode);
        keysToKeep.add(indent_file);
        keysToKeep.add(xml_mode);
        keysToKeep.add(grammar_limit);
        keysToKeep.add(column);
        setOption(new Long(Xip.DISPLAY_LEMMA), new Boolean(true));
        setOption(new Long(Xip.DISPLAY_SURFACE), new Boolean(false));
        //      setOption(new Long(Xip.DISPLAY_MARKUP), new Boolean(false));
        //     setOption(new Long(Xip.DISPLAY_ENTREE), new Boolean(false));
        setOption(new Long(Xip.DISPLAY_CATEGORY), new Boolean(false));
        setOption(new Long(Xip.DISPLAY_REDUCED), new Boolean(true));
        setOption(new Long(Xip.DISPLAY_FULL), new Boolean(false));
        setOption(new Long(Xip.DISPLAY_OFFSET), new Boolean(false));
        setOption(new Long(Xip.DISPLAY_WORDNUM), new Boolean(false));
        setOption(new Long(Xip.DISPLAY_SENTENCE), new Boolean(true));
        //    setOption(new Long(Xip.DISPLAY_NONE), new Boolean(false));
        setOption(new Long(Xip.DISPLAY_DEPENDENCY_BY_NAME), new Boolean(false));
        setOption(new Long(Xip.DISPLAY_DEPENDENCY_BY_CREATION), new Boolean(true));
        setOption(new Long(Xip.DISPLAY_DEPENDENCY_BY_NODE), new Boolean(false));
        setOption(new Long(Xip.DISPLAY_TREE), new Boolean(true));
//        setOption(new Long(Xip.UTF8_INPUT),new Boolean(inputUtf8.isSelected()));
//        setOption(new Long(Xip.DISPLAY_TREE_PHRASE), new Boolean(false));
        setOption(new Long(Xip.DISPLAY_TREE_COLUMN), new Boolean(true));
//        setOption(new Long(Xip.DISPLAY_MERGE_XML_SUBTREE), new Boolean(false));
//        setOption(new Long(Xip.DISPLAY_CONVERSION_UTF8),new Boolean(utf8.isSelected()));
//        setOption(new Long(Xip.DISPLAY_EXECUTION_ERROR), new Boolean(false));
        setOption(new Long(Xip.DISPLAY_MATHEMATICS), new Boolean(false));
        setOption(new Long(Xip.DISPLAY_DEPENDENCY_NUMBER), new Boolean(true));
        //      setOption(new Long(Xip.DISPLAY_EXECUTE_TOKEN), new Boolean(false));
        setOption(new Long(Xip.DISPLAY_SENTENCE_NUMBER), new Boolean(true));
        setOption(new Long(Xip.DISPLAY_CHUNK_TREE), new Boolean(true));
        setProperty(xml_mode, "0");
        setProperty(hmm_mode, "false");
        setProperty(ntm_only, "false");
        setProperty(indent_file, "false");
        setProperty(single_mode, "true");
        setProperty(grammar_limit, "-1");
        setProperty(column, String.valueOf(xip.getColumn()));
    }

    public void setColumn(String s) {
        int num = getColumn();
        try {
            num = Integer.parseInt(s);
        } catch (Exception e) {
            num = getColumn();
        }
        setColumn(num);
    }

    public void setColumn(int col) {
        xip.setColumn(col);
        setProperty(column, String.valueOf(xip.getColumn()));
    }

    public int getColumn() {
        return xip.getColumn();
    }

    /**
     * set a rule limit for the Grammar,
     * this rule limit is stored in the run context
     *
     * @param ruleNumber 
     * @throws java.lang.Exception 
     * @see Xip#setRuleLimit(int,int)
     * @see unsetGrammarLimit()
     * @see getGrammarLimit()
     */
    public void setGrammarLimit(int ruleNumber) throws Exception {
        setProperty(grammar_limit, String.valueOf(ruleNumber));
        xip.setRuleLimit(getGhandler(), ruleNumber);
    }

    /**
     *Unset the grammar limit
     *call the method setGrammarLimit with -1 value for the rule number
     *@throws java.lang.Exception 
     * @see setGrammarLimit(int)
     */
    public void unsetGrammarLimit() throws Exception {
        setGrammarLimit(-1);
    }

    /**
     *This method sets the grammar limit with the value of the last call
     *setGrammarLimit(int) method
     * @throws java.lang.Exception 
     */
    public void applyGrammarLimit() throws Exception {
        setGrammarLimit(getGrammarLimit());
    }

    /**
     *Return the number of the rule to reach, if no limit was done then
     *return -1.
     *@return a rule number or -1 if all of the grammar rules  have to be applied.
     *
     */
    public int getGrammarLimit() {
        String limit = (String) getProperty(grammar_limit);
        return Integer.parseInt(limit);
    }

    /*** set the ghander to load options with xip
     * @param gHandler
     */
    public void setGHandler(int gHandler) {
        this.gHandler = gHandler;
        loadOptions();
    }

    /**
     *Return the ghandler used
     *@return gHandler of the grammar
     */
    public int getGhandler() {
        return this.gHandler;
    }

    /**
     *set The parsing in NTM only option
     *@param b true for enabled false for disabled
     */
    public void setNtmOnly(boolean b) {
        setProperty(ntm_only, String.valueOf(b));
    }

    /**
     *Set the mode to use for parse :
     *@param mode the mode to use XML_NONE, XML_OUTPUT or XML_INSERT
     *see the constants declare in Xip class
     */
    public void setXMLMode(char mode) {
        int modeInt = (int) mode;
        setProperty(xml_mode, String.valueOf(modeInt));

    }

    /**
     *Return the mode XML to use for the parsing
     *@return XML_NONE, XML_OUTPUT, XMLINSERT
     *see the Xip class to know the values
     */
    public char getXMLMode() {
        String mode = (String) getProperty(xml_mode);
        return (char) Integer.parseInt(mode);
    }

    /**
     *Checks if the parsing is in a XML_OUTPUT
     *@return true is in XML_OUTPUT false otherwise
     */
    public boolean isXmlOutput() {
        return getXMLMode() == Xip.XML_OUTPUT;
    }

    /**
     *set the indentfile option
     *@param b true for enabled
     *@return true if xip set correctly the option
     *@exception Exception
     */
    public boolean setIndentFile(boolean b) throws Exception {
        setProperty(indent_file, String.valueOf(b));
        return xip.setIndentFile(getGhandler(), b);
    }

    /**
     *Check if the indent file option is enabled
     *@return true if enabled false otherwise
     */
    public boolean isIndentFile() {
        String b = (String) getProperty(indent_file);
        return Boolean.parseBoolean(b);
    }

    /**
     *Set the HMM only option
     *@param b
     * @throws java.lang.Exception 
     */
    public void setHmmOnly(boolean b) throws Exception {
        setProperty(hmm_mode, String.valueOf(b));
        xip.setTaggerMode(getGhandler(), b);
    }

    /**
     *check if the hmmOnly is enabled
     *@return true if enabled false otherwise
     */
    public boolean isHmmOnly() {
        String b = (String) getProperty(hmm_mode);
        return Boolean.parseBoolean(b);
    }

    /**
     *Set the runcontext in singlemode ie only one parsing
     *is showing.
     *@param b
     */
    public void setSingleMode(boolean b) {
        setProperty(single_mode, String.valueOf(b));
    }

    /**
     *Check if the singleMode is enabled or not
     *@return true if enabled famse otherwise
     */
    public boolean isSingleMode() {
        String b = (String) getProperty(single_mode);
        return Boolean.parseBoolean(b);
    }

    /**
     *check if the ntmOnly is enabled or not
     *@return true if enabled false otherwise
     */
    public boolean isNtmOnly() {
        String b = (String) getProperty(ntm_only);
        return Boolean.parseBoolean(b);
    }

    /**
     *return the table of the display options
     *@return a table with Long object for key and Boolean for values
     */
    public Hashtable getDisplayOptions() {
        return this.displayTable;
    }

    /**
     *Load the runContext setup
     * @return 
     */
    public boolean loadOptions() {
        try {
            if (getGhandler() != -1) {
                Enumeration keys = displayTable.keys();
                while (keys.hasMoreElements()) {
                    Long flag = (Long) keys.nextElement();
                    Boolean value = (Boolean) displayTable.get(flag);
                    if (value.booleanValue()) {
                        xip.addFlag(getGhandler(), flag.longValue());
                    } else {
                        xip.removeFlag(getGhandler(), flag.longValue());
                    }
                }
                setHmmOnly(isHmmOnly());
                setIndentFile(isIndentFile());
                return true;
            } else {
                return false;
            }
        } catch (Exception e) {
            return false;
        }
    }

    /**
     *Set an option for the display
     *@param flag the flag to setup
     *@param value the value for the flag false or true
     */
    public void setOption(Long flag, Boolean value) {
        displayTable.put(flag, value);
    }

    /**
     *Return the value of a flag for the display options
     *@return true of false such as Boolean
     */
    public Boolean getOption(Long flag) {
        return (Boolean) displayTable.get(flag);
    }

    /**
     *Clear the runcontext, clear only parameters used
     *we keep the essential option
     */
    public void clear() {
        Enumeration keys = keys();
        while (keys.hasMoreElements()) {
            Object key = keys.nextElement();
            if (key instanceof String) {
                String skey = (String) key;
                if (!keysToKeep.contains(skey)) {
                    remove(skey);
                }
            }
        }
    }

    /**
     *This method init the context.
     *we read the context from an XML document if it exist
     *otherwise we used the default context created in the constructor
     *@return true if no problem false otherwise.
     *
     *NB : if some options aren't in the XML document they will be
     *set with the default context
     */
    public boolean initContext() {
        try {
            if (setupFile.exists()) {
                XML xml = new XML(setupFile.getAbsolutePath());
                Element root = xml.getDocument().getRootElement();
                Element context = root.element(XMLProject.RUN_CONTEXT_);
                Iterator ite = context.elementIterator(XMLProject.PROPERTY_);
                while (ite.hasNext()) {
                    Element property = (Element) ite.next();
                    String name = property.attributeValue(XMLProject.NAME_);
                    String value = property.attributeValue(XMLProject.VALUE_);
                    if (name != null && value != null) {
                        setProperty(name, value);
                    }
                }
                //set the column
                String s = (String) getProperty(column);
                setColumn(s);
                Element display = context.element(XMLProject.DISPLAY_);
                Iterator iteDisplay = display.elementIterator(XMLProject.PROPERTY_);
                while (iteDisplay.hasNext()) {
                    Element property = (Element) iteDisplay.next();
                    String name = property.attributeValue(XMLProject.NAME_);
                    String value = property.attributeValue(XMLProject.VALUE_);
                    if (name != null && value != null) {
                        setOption(new Long(name), new Boolean(value));
                    }
                }
            }
            return true;
        } catch (Exception e) {
            return false;
        }
    }
}
