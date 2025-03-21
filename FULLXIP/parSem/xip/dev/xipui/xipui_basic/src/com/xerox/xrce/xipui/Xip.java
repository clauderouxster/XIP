/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 */
package com.xerox.xrce.xipui;

import com.xerox.jxip.*;
import com.xerox.xrce.xipui.exception.BadGrammarHandlerException;
import java.util.*;
import org.apache.log4j.*;
import java.io.*;

/**
 * Appels des procedures xip.
 * @author Vianney Grassaud
 */
public class Xip extends com.xerox.jxip.Const {

    private int column = Xip.COL;
    private JXip jXip;
    private JXip2 jXip2;
    /**  map for a short description of each flag */
    private Map<Number, String> flagMap;
    private XipResult xipResult;
    private Logger log;

    /** Creates a new instance of Xip 
     * @throws java.lang.Exception
     * @throws java.lang.Error 
     */
    public Xip() throws Exception, Error {
        flagMap = new Hashtable<Number, String>();
        try {
            log = Logger.getLogger("xip");
            jXip = new JXip();
            jXip2 = new JXip2();
            setFlagMap();
        } catch (ExceptionInInitializerError e) {
            log.error("Xip:Xip() ExceptionInInitializerError", e);
            throw e;
        } catch (Exception e) {
            log.error("Xip:Xip() Exception", e);
            throw e;
        }
    }

    public void setColumn(int col) {
        if (col > 0) {
            this.column = col;
        } else {
            col = COL;
        }
    }

    private void setFlagMap() {
        flagMap.put(COL, "COL");
        flagMap.put(DEPENDENCYRULE, "Dependency rule");
        flagMap.put(DISPLAY_CATEGORY, "category");
        flagMap.put(DISPLAY_CHUNK_TREE, "chunk tree");
        flagMap.put(DISPLAY_CONVERSION_UTF8, "output utf8");
        flagMap.put(DISPLAY_DEPENDENCY_BY_CREATION, "dependency by creation");
        flagMap.put(DISPLAY_DEPENDENCY_BY_NAME, "dependency by name");
        flagMap.put(DISPLAY_DEPENDENCY_BY_NODE, "dependency by node");
        flagMap.put(DISPLAY_DEPENDENCY_FEATURE_VALUE, "dependency feature");
        flagMap.put(DISPLAY_DEPENDENCY_NUMBER, "dependency number");
        flagMap.put(DISPLAY_ENTREE, "entree");
        flagMap.put(DISPLAY_EXECUTE_TOKEN, "token");
        flagMap.put(DISPLAY_EXECUTION_ERROR, "error");
        flagMap.put(DISPLAY_FULL, "full");
        flagMap.put(DISPLAY_LEMMA, "lemma");
        flagMap.put(DISPLAY_MARKUP, "markup");
        flagMap.put(DISPLAY_MATHEMATICS, "mathematics");
        flagMap.put(DISPLAY_MERGE_XML_SUBTREE, "xml subtree");
        flagMap.put(DISPLAY_NONE, "none");
        flagMap.put(DISPLAY_OFFSET, "offset");
        flagMap.put(DISPLAY_REDUCED, "reduced");
        flagMap.put(DISPLAY_SENTENCE_NUMBER, "rule number");
        flagMap.put(DISPLAY_SENTENCE, "sentence");
        flagMap.put(DISPLAY_SURFACE, "surface");
        flagMap.put(DISPLAY_TREE_PHRASE, "tree phrase");
        flagMap.put(DISPLAY_TREE_COLUMN, "tree column");
        flagMap.put(DISPLAY_TREE, "tree");
        flagMap.put(DISPLAY_WORDNUM, "word number");
    }

    public void setXipResult(XipResult xipResult) {
        this.xipResult = xipResult;
    }

    /**
     *Return the JXip Object. (basic xip API parser)
     *@return JXIP
     */
    public synchronized JXip getJXip() {
        return this.jXip;
    }

    /**
     * Change the value of a native environment variable
     * @param  name   the environement variable name
     * @param  value  The new value
    @throws java.lang.Exception  
     */
    public synchronized void setEnv(String name, String value) throws Exception {
        try {
            jXip.setEnv(name, value);
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     * Get the value of a native environment variable
     *
     * @param  name  The environment variable name.
     * @return       The enviromnent variable value, null if the variable not found or an execption was caugth
     */
    public synchronized String getEnv(String name) {
        String value = null;
        try {
            value = jXip.getEnv(name);
        } catch (Exception e) {
            log.error("Error, can't get the variable : " + name, e);

        }
        return value;
    }

    /**
     *  load grammar files using a grm file
     *
     * @param  grmFileName    the grm file name
     * @param  enableXml      when set to one, this parameter tells XIP that 
     *                         the input complies with the XIP XML DTD
     * @return                a handler to a grammar or -1 if error
     * @exception  Exception  Description of the Exception
     */
    public synchronized int loadGrammar(String grmFileName, int enableXml) throws Exception {
        return loadGrammar(grmFileName, enableXml, true);
    }

    /**
     * @param  grmFileName    the grm file name
     * @param  enableXml      when set to one, this parameter tells XIP that the input complies with
     * the XIP XML DTD
     * @param  loadFst        if true loads the FST otherwise parse only the grammar file without
     * loading FST
     * @return                a handler to a grammar or -1 if error
     * @exception  Exception  Description of the Exception
     */
    public synchronized int loadGrammar(String grmFileName, int enableXml, boolean loadFst)
            throws Exception {
        return jXip.XipGrmFile(grmFileName, enableXml, loadFst);
    }

    /**
     *  Reload a grammar
     *
     * @param  gHandler         the handler to the grammar to reload
     * @param  reloadParameter  if true then the paramter files will be reloaded 
     *                otherwise they will be not reloaded
     * @param  grmFileName      the grm file of the grammar to reload
     * @return                  a handler to the grammar.
     * @throws com.xerox.xrce.xipui.exception.BadGrammarHandlerException 
     * @throws Exception
     */
    public synchronized int reloadGrammar(int gHandler, boolean reloadParameter, String grmFileName)
            throws BadGrammarHandlerException, Exception {
        XipUtilities.checkHandler(gHandler);
        return jXip.XipReloadGrammar(gHandler, reloadParameter, grmFileName);
    }

    /**
     * Extract the XML bloc to parse from an xml input file
     *
     * @param  gHandler  Description of the Parameter
     * @param  fileName  Description of the Parameter
     * @param encoding  
     * @return           Return a Vector of (String) or null if an Exception was caugth
     */
    public synchronized Vector xmlUnitExtraction(int gHandler, String fileName, String encoding) {
        Vector sentences = null;
        try {
            log.info("xmlUnitExtraction with : " + gHandler + " on " + fileName);
            XipUtilities.checkHandler(gHandler);
            if (XipUtilities.isUTF8(encoding)) {
                XipUtilities.setUTF8(this, gHandler, true);
            }
            sentences = jXip2.xmlUnitExtraction(gHandler, fileName);
            XipUtilities.setUTF8(this, gHandler, false);
        } catch (Exception e) {
            log.error("error", e);
        }
        return sentences;
    }

    /**
     *  set a int variable
     *
     * @param  gHandler       a handler to the grammar.
     * @param  variableName   the variable name
     * @param  value          Description of the Parameter
     * @return                return -1 if error, 0 otherwise
     * @throws java.lang.Exception  
     */
    public synchronized int setIntVariable(int gHandler, String variableName, int value) throws Exception {
        try {
            XipUtilities.checkHandler(gHandler);
            return jXip.XipSetIntVariable(gHandler, variableName, value);
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     *  set a variable
     *
     * @param  gHandler       a handler to the grammar.
     * @param  variableName   the variable name
     * @param  variableValue  the variable value
     * @return                return -1 if error, 0 otherwise
     * @throws java.lang.Exception  
     */
    public synchronized int setStringVariable(int gHandler, String variableName, String variableValue) throws Exception {
        try {
            XipUtilities.checkHandler(gHandler);
            return jXip.XipSetStringVariable(gHandler, variableName, variableValue);
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     *  get a variable string value
     *
     * @param  gHandler      a handler to the grammar.
     * @param  variableName  the variable name
     * @return
     * @throws java.lang.Exception  
     */
    public synchronized String getStringVariable(int gHandler, String variableName) throws Exception {
        try {
            XipUtilities.checkHandler(gHandler);
            return jXip.XipGetStringVariable(gHandler, variableName);
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     *  Description of the Method
     *
     * @param  gHandler  a handler to the grammar.
     * @return           a Vector of alias name (String)
     * @throws java.lang.Exception  
     */
    public synchronized Vector<String> getAlias(int gHandler) throws Exception {
        try {
            XipUtilities.checkHandler(gHandler);
            return jXip.XipAliasList(gHandler);
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     * Load a parameter file in a specific grammar
     *
     * @param  gHandler  a handler to a grammar
     * @param  fileName  a parameter file name
     * @return           return -1 if error, 0 otherwise
     * @throws java.lang.Exception  
     */
    public synchronized int loadParameter(int gHandler, File fileName) throws Exception {
        try {
            XipUtilities.checkHandler(gHandler);
            return jXip.XipParameterFile(gHandler, fileName.getAbsolutePath());
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     * Select a display mode
     *
     *
     * @param gHandler     a handler to a grammar
     * @param displayMode  the dispaly mode value
     * @throws java.lang.Exception  
     */
    public synchronized void setDisplayMode(int gHandler, long displayMode) throws Exception {
        try {
            XipUtilities.checkHandler(gHandler);
            jXip.XipSetDisplayMode(gHandler, displayMode, getColumn());
        } catch (Exception e) {
            throw e;
        }
    }

    public int getColumn() {
        return this.column;
    }

    /**
     * Get version about the current XIP C++ Library used.
     *
     * @return    The version value
     * @throws java.lang.Exception  
     */
    public synchronized String getVersion() throws Exception {
        try {
            return jXip.GetVersion();
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     *  Gets the licenseCountDown attribute of the JXip object
     *
     * @param  gHandler  a handler to a grammar
     * @return           The licenseCountDown value
     * @throws java.lang.Exception  
     */
    public synchronized int getLicense(int gHandler) throws Exception {
        try {
            XipUtilities.checkHandler(gHandler);
            return jXip.XipLicense(gHandler);
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     *  parse ASCII string
     *
     * @param  gHandler       a handler to a grammar
     * @param  textInput      string to parse
     * @param  xml            XML_NONE=0, XML_OUTPUT=1,XML_INSERT=2
     * @param  apply_grammar  Description of the Parameter
     * @return                Description of the Return Value
     * @throws java.lang.Exception  
     */
    public synchronized String parseString(int gHandler, String textInput, char xml, boolean apply_grammar) throws Exception {
        try {
            XipUtilities.checkHandler(gHandler);
            char apply = (char) 1;
            if (!apply_grammar) {
                apply = (char) 0;
            }
            return jXip.parseString(gHandler, textInput, xml, apply);
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     *  Convert the textInput string to iso-8859-1 before parsing
     *
     * @param  gHandler       a handler to a grammar
     * @param  textInput      string to parse
     * @param charset  
     * @param  xml            XML_NONE=0, XML_OUTPUT=1,XML_INSERT=2
     * @param  apply_grammar  Description of the Parameter
     * @return                Description of the Return Value
     * @exception  Exception  Description of the Exception
     */
    public synchronized String parseStringCharSet(int gHandler, String textInput, String charset, char xml, boolean apply_grammar) throws Exception {
        try {
            XipUtilities.checkHandler(gHandler);
            char apply = (char) 1;
            if (!apply_grammar) {
                apply = (char) 0;
            }
            return jXip.parseStringCharSet(gHandler, textInput, charset, xml, apply);
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     * Parse one String.
     *
     * @param  gHandler   a handler to a grammar
     * @param  textInput  String to parse.
     * @return            String        A XML result as a String.
     * @throws java.lang.Exception  
     */
    public synchronized String parseStringXml(int gHandler, String textInput) throws Exception {

        XipUtilities.checkHandler(gHandler);
        return jXip.parseStringXml(gHandler, textInput);

    }

    /**
     *Parse a given XML file, and return the result in a String
     *Xip'll use the useguide of the grammar, if no userguide exists, then XIP'll use the default userguide.
     *
     *If the input file is in UTF-8, then the method <code>XipUtilities.setUTF8(this,gHandler, true)</code> is called before parsing and
     *<code> XipUtilities.setUTF8(this,gHandler, true)</code> after parsing.
     *
     *@param gHandler the grammar handler to use
     *@param input the file to parse
     *@param depth the depth to use for read the XML document use 0 or 2.
     *@param xml the XML mode to use : XML_INSERT, XML_NONE, XML_OUTPUT
     *@param apply if the grammar sould be apply, otherwise parsing until tagger
     *@param encoding the encoding of the input file, the outpu file will have the same encoding
     *@return a String containg the analysis, the String is in the same encoding than the input
     *@throws java.lang.Exception  
     *@see parseFile(int,File,File,char,boolean,String)
     *@see parseFileXML(int,File,File,int,char,boolean,String)
     */
    public synchronized String parseFileXmlOS(int gHandler, File input, int depth, char xml, boolean apply, String encoding) throws Exception {

        XipUtilities.checkHandler(gHandler);
        char apply_ = (char) 1;
        if (!apply) {
            apply_ = (char) 0;
        }
        if (XipUtilities.isUTF8(encoding)) {
            XipUtilities.setUTF8(this, gHandler, true);
        }
        byte[] result = jXip.XipParseFileXmlOS(gHandler, input.getAbsolutePath(), depth, xml, apply_).getBytes();
        XipUtilities.setUTF8(this, gHandler, false);
        return new String(result, encoding);

    }

    /**
     *  Parse one String.
     *
     * @param  gHandler  a handler to a grammar
     * @param  input     text to parse
     * @param  depth     Description of the Parameter
     * @param  xml       XML_NONE=0, XML_OUTPUT=1,XML_INSERT=2
     * @param  apply     Description of the Parameter
     * @return           Description of the Return Value
     * @throws java.lang.Exception  
     * 
     */
    public synchronized String parseStringXmlOS(int gHandler, String input, int depth, char xml, boolean apply) throws Exception {

        XipUtilities.checkHandler(gHandler);
        char apply_ = (char) 1;
        if (!apply) {
            apply_ = (char) 0;
        }
        return jXip.parseStringXmlOS(gHandler, input, depth, xml, apply_);

    }

    /**
     * Parse one String.
     * depth is set to 1, xml is set to XML_INSERT, and apply is set to APPLY_GRAMMAR
     *
     * @param  gHandler  a handler to a grammar
     * @param  input     text to parse
     * @return           String        A XML result as a String.
     * @throws java.lang.Exception  
     * 
     */
    public synchronized String parseStringXmlOS(int gHandler, String input) throws Exception {

        XipUtilities.checkHandler(gHandler);
        return jXip.parseStringXmlOS(gHandler, input);

    }

    /**
     *  Description of the Method
     *
     * @param  gHandler    Description of the Parameter
     * @param  extraction  Description of the Parameter
     * @throws java.lang.Exception  
     * 
     */
    public synchronized void setDependencyExtraction(int gHandler, boolean extraction) throws Exception {

        XipUtilities.checkHandler(gHandler);
        jXip.XipSetDependencyExtraction(gHandler, extraction);

    }

    /**
     *  Description of the Method
     *
     * @param  gHandler  Description of the Parameter
     * @throws java.lang.Exception  
     * 
     */
    public synchronized void freeGrammar(int gHandler) throws Exception {

        log.info("Free grammar : " + gHandler);
        XipUtilities.checkHandler(gHandler);
        jXip.XipFreeGrammar(gHandler);

    }

    /**
     * Return the last XipResult that was created by a Xipx object e.g.
     * @return a XipResult, if no xipResult was set, return null
     * @see Xipx#getXipResult()
     */
    public synchronized XipResult getXipResult() {
        return this.xipResult;
    }

    /**
     * Add display flag
     *
     *
     * @param gHandler       a handler to a grammar
     * @param displayFlag    the display flag
     * @exception Exception  Description of the Exception
     */
    public synchronized void addFlag(int gHandler, long displayFlag) throws Exception {
        try {
            String descFlag = flagMap.get(displayFlag);
            if (descFlag == null) {
                descFlag = String.valueOf(displayFlag);
            }

            //   if ( !testFlag(gHandler,displayFlag)) {
            // log.info("Add Flag : " + descFlag + " column : " + getColumn() + " handler : " + gHandler);
            jXip.XipAddFlagDisplay(gHandler, displayFlag, getColumn());
            // }
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     *  remove a display flag
     *
     * @param  gHandler       a handler to a grammar
     * @param  displayFlag    the display flag
     * @exception  Exception  Description of the Exception
     */
    public synchronized void removeFlag(int gHandler, long displayFlag) throws Exception {
        try {
            String descFlag = flagMap.get(displayFlag);
            if (descFlag == null) {
                descFlag = String.valueOf(displayFlag);
            }

            if (testFlag(gHandler, displayFlag)) {
                log.info("remove flag : " + descFlag + " handler : " + gHandler);
                jXip.XipRemoveFlagDisplay(gHandler, displayFlag);
            }
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     *  test a display flag
     *
     * @param  gHandler       a handler to a grammar
     * @param  displayFlag    the display flag
     * @return                true or false
     * @exception  Exception  Description of the Exception
     */
    public synchronized boolean testFlag(int gHandler, long displayFlag) throws Exception {
        try {
            // log.info("Test flag Display : "  + displayFlag +  " for " + gHandler);
            XipUtilities.checkHandler(gHandler);
            return jXip.XipTestFlagDisplay(gHandler, displayFlag);
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     * Parse until the rule number
     *
     * @param  gHandler  a handler to a grammar
     * @param  ruleNb    Description of the Parameter
     * @throws java.lang.Exception  
     * 
     */
    public synchronized void setRuleLimit(int gHandler, int ruleNb) throws Exception {
        //log.info("Grammar : " + gHandler + " rule limit : " + ruleNb);
        XipUtilities.checkHandler(gHandler);
        jXip2.XipSetRuleLimit(gHandler, ruleNb);

    }

    /**
     *@param gHandler
     *@param index
     * @throws java.lang.Exception  
     */
    public synchronized void setRuleRankNumberLimit(int gHandler, int index) throws Exception {
        log.info("Grammar : " + gHandler + " rule rank limit : " + index);
        XipUtilities.checkHandler(gHandler);
        jXip2.xipSetRuleRankNumberLimit(gHandler, index);
    }

    /**
     * Parse until the label
     *
     * @param  gHandler  a handler to a grammar
     * @param  label     the label
     * @throws java.lang.Exception  
     * 
     */
    public synchronized void setLabelRuleLimit(int gHandler, String label) throws Exception {

        XipUtilities.checkHandler(gHandler);
        jXip2.XipSetLabelRuleLimit(gHandler, label);

    }

    /**
     *  parse until the rule number and return the vector of rules applyed
     *
     * @param  gHandler  a handler to a grammar
     * @return           Description of the Return Value
     * @throws java.lang.Exception  
     * 
     */
    public synchronized Vector getAppliedRules(int gHandler) throws Exception {

        XipUtilities.checkHandler(gHandler);
        Vector v = jXip2.XipReturnRules(gHandler);
        if (v != null) {
            log.info("Get applied rules for " + gHandler + " size " + v.size());
        } else {
            log.error("error");
        }
        return v;

    }

    /**
     * @param  gHandler  a handler to a grammar
     * @return           Description of the Return Value
     * @throws java.lang.Exception  
     * 
     */
    public synchronized boolean isGrammarCrypted(int gHandler) throws Exception {
        XipUtilities.checkHandler(gHandler);
        return jXip2.XipIsGrammarCrypted(gHandler);
    }

    /**
     * Return the list of operations provided for the scripts
     *
     * @return  a Map with some descrption about xip syntaxe and methods...
     * @throws java.lang.Exception  
     * 
     */
    public synchronized Hashtable getAvailableOperations() throws Exception {

        return jXip2.ListOfOperations();

    }

    /**
     *Parse a given file, and write the result in a output file.
     *If the input file is in UTF-8, then the method <code>XipUtilities.setUTF8(this,gHandler, true)</code> is called before parsing and
     *<code> XipUtilities.setUTF8(this,gHandler, true)</code> after parsing.
     *
     *@param gHandler the grammar handler to use
     *@param input the file to parse
     *@param output the result
     *@param xml the XML mode to use : XML_INSERT, XML_NONE, XML_OUTPUT
     *@param apply if the grammar sould be apply, otherwise parsing until tagger
     *@param encoding the encoding of the input file, the outpu file will have the same encoding
     *@return true if the file was parsed, false otherwise
     *@throws java.lang.Exception  
     *@see parseFileXML(int,File,File,int,char,boolean,String)
     *@see parseFileXMLOS(int,File,int,char,boolean,String)
     */
    public synchronized boolean parseFile(int gHandler, File input, File output, char xml, boolean apply, String encoding) throws Exception {
        log.info("XipParseFileToFile with gHandler " + gHandler + " from " + input + " with encoding " + encoding);
        XipUtilities.checkHandler(gHandler);
        char apply_ = (char) 1;
        if (!apply) {
            apply_ = (char) 0;
        }
        if (XipUtilities.isUTF8(encoding)) {
            XipUtilities.setUTF8(this, gHandler, true);
        }
        int res = jXip.XipParseFileToFile(gHandler, input.getAbsolutePath(), output.getAbsolutePath(), xml, apply_);
        XipUtilities.setUTF8(this, gHandler, false);
        return (res == 0);
    }

    /**
     *Parse a given XML file, and write the result in a output file.
     *Xip'll use the useguide of the grammar, if no userguide exists, then XIP'll use the default userguide.
     *
     *If the input file is in UTF-8, then the method <code>XipUtilities.setUTF8(this,gHandler, true)</code> is called before parsing and
     *<code> XipUtilities.setUTF8(this,gHandler, true)</code> after parsing.
     *
     *@param gHandler the grammar handler to use
     *@param input the file to parse
     *@param output the result
     *@param depth the depth to use for read the XML document use 0 or 2.
     *@param xml the XML mode to use : XML_INSERT, XML_NONE, XML_OUTPUT
     *@param apply if the grammar sould be apply, otherwise parsing until tagger
     *@param encoding the encoding of the input file, the outpu file will have the same encoding
     *@return true if the file was parsed, false otherwise
     *@throws java.lang.Exception  
     *@see parseFile(int,File,File,char,boolean,String)
     *@see parseFileXMLOS(int,File,int,char,boolean,String)
     */
    public synchronized boolean parseFileXML(int gHandler, File input, File output, int depth, char xml, boolean apply, String encoding) throws Exception {
        XipUtilities.checkHandler(gHandler);
        if (XipUtilities.isUTF8(encoding)) {
            XipUtilities.setUTF8(this, gHandler, true);
        }
        char apply_ = (char) 1;
        if (!apply) {
            apply_ = (char) 0;
        }
        int res = jXip.XipParseFileXMLToFile(gHandler, input.getAbsolutePath(), output.getAbsolutePath(), depth, xml, apply_);
        XipUtilities.setUTF8(this, gHandler, false);
        return (res == 0);
    }

    /**
     *  Return a vector of category name (String)
     *
     * @param  gHandler       Description of the Parameter
     * @return                Return a vector of category name (String)
     * @exception  Exception  Description of the Exception
     */
    public synchronized Vector<String> getNameCategories(int gHandler) throws Exception {
        XipUtilities.checkHandler(gHandler);
        return jXip.XipGetListCategories(gHandler);
    }

    /**
     *  Return a vector of dependency name (String)
     *
     * @param  gHandler       Description of the Parameter
     * @return                Return a vector of dependency name (String)
     * @exception  Exception  Description of the Exception
     */
    public synchronized Vector<String> getNameDependencies(int gHandler) throws Exception {
        XipUtilities.checkHandler(gHandler);
        return jXip.XipGetListDependencies(gHandler);
    }

    /**
     * 
     * @param gHandler
     * @return
     * @throws java.lang.Exception
     */
    public synchronized Vector<GramDependency> getListDependencies(int gHandler) throws Exception {
        XipUtilities.checkHandler(gHandler);
        return jXip2.xipGetListDependencies(gHandler);
    }

    /**
     *  Description of the Method
     *
     * @param  gHandler  Description of the Parameter
     * @return           return a Vector of GramCategory
     * @throws java.lang.Exception  
     * 
     */
    public synchronized Vector<GramCategory> getCategories(int gHandler) throws Exception {
        XipUtilities.checkHandler(gHandler);
        return jXip2.xipGetListCategories(gHandler);
    }

    /**
     *  Description of the Method
     *
     * @param  gHandler  Description of the Parameter
     * @return           Return a Vector of GramVariable
     * @throws java.lang.Exception  
     * 
     */
    public synchronized Vector<GramVariable> getVariables(int gHandler) throws Exception {
        XipUtilities.checkHandler(gHandler);
        return jXip2.xipGetListVariables(gHandler);
    }

    /**
     *  Description of the Method
     *
     * @param  gHandler  Description of the Parameter
     * @return           Return a Vector of GramDependency
     * @throws java.lang.Exception 
     */
    public synchronized Vector<GramDependency> getDependencies(int gHandler) throws Exception {
        XipUtilities.checkHandler(gHandler);
        return jXip2.xipGetListDependencies(gHandler);
    }

    /**
     *  Reset and clear the error message buffer
     */
    public synchronized void initErrorMsg() {
        jXip.InitErrorMsg();
    }

    /**
     *  return the error message of the previous xip function call
     *
     * @return    The errorMsg value
     */
    public synchronized String getErrorMsg() {
        String msg = jXip.getErrorMsg();
        jXip.InitErrorMsg();
        return msg;
    }

    /**
     *Return the feature tree of grammar
     *@param gHandler 
     * @return a Vector containing the declared features.
     *@throws java.lang.Exception 
     * 
     */
    public synchronized Vector getFeatureTree(int gHandler) throws Exception {
        XipUtilities.checkHandler(gHandler);
        return jXip2.xipGetFeatureTree(gHandler);
    }

    /**
     *  Return the encoding charset of the source (FST)
     *
     * @param  gHandler       Grammar handler
     * @return                the encoding charset: "UTF8_ENCODING" | "LATIN_ENCODING"
     * @exception  Exception
     */
    public synchronized String getSourceCharset(int gHandler) throws Exception {
        XipUtilities.checkHandler(gHandler);
        return jXip.XipGetSourceCharset(gHandler);
    }

    /**
     *  Return the encoding charset of the grammar source files
     *
     * @param  gHandler       Grammar handler
     * @return                the encoding charset: "UTF8_ENCODING" | "LATIN_ENCODING"
     * @exception  Exception
     */
    public synchronized String getGrammarCharset(int gHandler) throws Exception {
        XipUtilities.checkHandler(gHandler);
        return jXip.XipGetGrammarCharset(gHandler);
    }

    /**
     *
    @param gHandler 
    @param features 
    @param lexicons 
    @param rules 
    @param parameters 
    @param scripts 
    @param divers 
    @return 
    @throws java.lang.Exception 
     */
    public synchronized String grammarFiles(int gHandler, Vector<String> features, Vector<String> lexicons, Vector<String> rules, Vector<String> parameters, Vector<String> scripts, Vector<String> divers) throws Exception {
        XipUtilities.checkHandler(gHandler);
        return jXip2.xipGrammarPathnames(gHandler, features, lexicons, rules, parameters, scripts, divers);
    }

    /**
     *
    @param gHandler 
    @param with 
    @throws java.lang.Exception 
     */
    public synchronized void setTaggerMode(int gHandler, boolean with) throws Exception {
        XipUtilities.checkHandler(gHandler);
        jXip2.xipSetTaggerMode(gHandler, with);
    }

    /**
     *  set indent mode
     *
     * @param  gHandler  Description of the Parameter
     * @param  mode      true to set indent mode, false otherwise
     * @return           false if error, true otherwise
     * @throws java.lang.Exception 
     */
    public synchronized boolean setIndentFile(int gHandler, boolean mode) throws Exception {
        log.info("xip indent File : " + mode + " for " + gHandler);
        XipUtilities.checkHandler(gHandler);
        return jXip2.xipIndentFile(gHandler, mode);
    }

    /**
     *  get Grammar information
     *
     * @param  gHandler  Description of the Parameter
     * @return           Description of the Return Value
     * @throws java.lang.Exception  
     */
    public synchronized Hashtable grammarInformation(int gHandler) throws Exception {
        XipUtilities.checkHandler(gHandler);
        return jXip2.xipGrammarInformation(gHandler);
    }

    /**
     * define the path for any output file
     *
     * @param  gHandler
     * @param  path      the Path for any output file
     * @return           true if ok, false if error
     */
    public synchronized boolean setWorkingPath(int gHandler, String path) {
        boolean success = false;
        try {
            XipUtilities.checkHandler(gHandler);
            success = jXip.xipSetWorkingFilesPath(gHandler, path);
        } catch (Exception e) {
            log.error("ERROR", e);
            success = false;
        }
        return success;
    }

    /**
     * layer renumbering for a file
     *
     * @param  gHandler  Description of the Parameter
     * @param  idRule    Description of the Parameter
     */
    public synchronized void renumberLayer(int gHandler, int idRule) {
        try {
            XipUtilities.checkHandler(gHandler);
            jXip2.xipLayerRenumbering(gHandler, idRule);
        } catch (Exception e) {
            log.fatal("error", e);
        }
    }

    /**
     *  return the label used in the grammar for the surface
     *
     * @param  gHandler  grammar handler
     * @return           the surface label
     * @throws java.lang.Exception  
     */
    public synchronized String getLabelsSurface(int gHandler) throws Exception {
        return jXip2.xipGetLabelsSurface(gHandler);
    }

    /**
     *  return the label used in the grammar for the lemme
     *
     * @param  gHandler  grammar handler
     * @return           the lemme label
     * @throws java.lang.Exception  
     */
    public synchronized String getLabelsLemma(int gHandler) throws Exception {
        return jXip2.xipGetLabelsLemme(gHandler);
    }

    /**
     *Copy the files used by a grammar to a directory
     *@param grmFileName the grammar file
     *@param gHandler the grammar loaded by xip
     *@param destinationDir the directory to copy the files
     *@param crypte to crypt the files.
     *@throws Exception
     */
    public void copyAndCrypteGrammarFiles(String grmFileName, int gHandler, String destinationDir, boolean crypte) throws Exception {
        jXip2.copyAndCrypteGrammarFiles(grmFileName, gHandler, destinationDir, crypte);
    }

    /**
     *Decrypt the files of a grammar and copy these file to a directory
     *@param grmFile the grammar file
     *@param gHandler the grammar loaded by xip
     *@param destinationDir the direcoty to decrypt the file
     *@throws Exception if problem
     */
    public void decryptGrammar(String grmFile, int gHandler, String destinationDir) throws Exception {
        jXip2.decrypteGrammarFiles(grmFile, gHandler, destinationDir);
    }

    /**
     *sets the maximum of XipUnit that xip can create for one document. 
     *@param gHandler the grammar handler
     *@param indexMax the index of max XipUnit to reach, use -1 to ensure the creation of all XipUnit per document.
     *@throws Exception if error
     */
    public void setMaxXipUnit(int gHandler, int indexMax) throws Exception {
        jXip2.xipSetMaxSentence(gHandler, indexMax);
    }

    /**
     *Return a string representation of XIP
     *@return the version of the XIP parser null if an Exception was caught
     */
    @Override
    public String toString() {
        String version = null;
        try {
            version = getVersion();
        } catch (Exception e) {
            log.error("Can't get the version", e);
        }
        return version;
    }
}
