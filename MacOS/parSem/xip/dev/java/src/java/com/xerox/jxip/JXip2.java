/*
 *   Xerox Research Centre Europe - Grenoble Laboratory
 *           project XIP (Xerox Incremental Parser)
 *  Copyright (C) 2004 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved
 *
 */
package com.xerox.jxip;

import java.io.*;
import java.nio.channels.FileChannel;
import java.util.Collections;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Description of the Class
 *
 * @author grondeau @created April 5, 2005
 */
public class JXip2 {

    private Vector<Integer> applyedRuleNbVect;
    private Hashtable<String, Hashtable> listOfOp;
    private Hashtable<String, String> listOfOpdetail;
    private Vector<GramVariable> listVariable;
    private Vector<GramDependency> listDependencies;
    private Vector<GramCategory> listCategories;
    private Vector<GramFeature> listFeatures;
    // the key is a fileIndex(int) and the value is the fileName where the feature is declared
    private Hashtable<Integer, String> listOfFeatureFileName;
    // the key is a fileIndex(int) and the value is the fileName where the category is declared
    private Hashtable<Integer, String> listOfCategoryFileName;
    // the key is a fileIndex(int) and the value is the fileName where the dependency is declared
    private Hashtable<Integer, String> listOfDependencyFileName;
    private Vector<Vector<GramFeature>> listFeaturesMgt;

    /**
     * Constructor for the JXip2 object
     */
    public JXip2() {
        applyedRuleNbVect = new Vector<Integer>();
        listOfOp = new Hashtable<String, Hashtable>();
    }
    //------------------------------------------------------------------------

    private native int XipSetEnvVariableImplementation(String var,String val)
            throws Exception;

    /**
     * Set an environment variable
     *
     * @param var is the environment variable name
     * @param val is the new environment variable value
     * @return 
     * @exception Exception
     */
    public synchronized int XipSetEnvVariable(String var,String val) throws Exception {
        return XipSetEnvVariableImplementation(var, val);
    }
    //------------------------------------------------------------------------

    private native String XipGetEnvVariableImplementation(String var)
            throws Exception;

    /**
     * Get an environment variable
     *
     * @param var is the environment variable name
     * @return 
     * @exception Exception
     */
    public synchronized String XipGetEnvVariable(String var) throws Exception {
        return XipGetEnvVariableImplementation(var);
    }
    //------------------------------------------------------------------------

    private native int KifExecuteProgramImplementation(String filename,String args)
            throws Exception;

    /**
     * Execute a KiF program
     *
     * @param filename of the KiF program to execute
     * @param args arguments as a string, where each parameter is separated with a space
     * @return 
     * @exception Exception
     */
    public synchronized int KifExecuteProgram(String filename,String args) throws Exception {
        return KifExecuteProgramImplementation(filename, args);
    }
    //------------------------------------------------------------------------

    private native int KifExecuteCodeProgramImplementation(String code,String args)
            throws Exception;

    /**
     * Execute a KiF program from String
     *
     * @param code of the KiF program to execute
     * @param args arguments as a string, where each parameter is separated with a space
     * @return 
     * @exception Exception
     */
    public synchronized int KifExecuteCodeProgram(String code,String args) throws Exception {
        return KifExecuteCodeProgramImplementation(code, args);
    }
    //------------------------------------------------------------------------

    private native String KifExecuteFunctionImplementation(String funcname,String[] parameters)
            throws Exception;

    /**
     * Execute a KiF function, which has been loaded in a KifExecuteProgram 
     *
     * @param funcname the KiF function name
     * @param parameters the KiF function parameters as a vector of strings. Each will be mapped over the function arguments.
     * @return 
     * @exception Exception
     */
    public synchronized String KifExecuteFunction(String funcname,String[] parameters) throws Exception {
        return KifExecuteFunctionImplementation(funcname, parameters);
    }
    //------------------------------------------------------------------------

    private native String KifExecuteFunctionImplementationIndex(int kifhandler,String funcname,String[] parameters)
            throws Exception;

    /**
     * Execute a KiF function, which has been loaded in a KifExecuteProgram 
     *
     * @param kifhandler
     * @param funcname the KiF function name
     * @param parameters the KiF function parameters as a vector of strings. Each will be mapped over the function arguments.
     * @return 
     * @exception Exception
     */
    public synchronized String KifExecuteFunctionFrom(int kifhandler,String funcname,String[] parameters) throws Exception {
        return KifExecuteFunctionImplementationIndex(kifhandler,funcname, parameters);
    }
    //------------------------------------------------------------------------
    private native void KifExecuteProgramParseurImplementation(int gHandler,String filename,String args)
            throws Exception;

    /**
     * Execute a KiF program from within a given grammar
     *
     * @param gHandler
     * @param filename of the KiF program to execute in a given grammar
     * @param args arguments as a string, where each parameter is separated with a space
     * @exception Exception
     */
    public synchronized void KifExecuteProgramParseur(int gHandler,String filename,String args) throws Exception {
        KifExecuteProgramParseurImplementation(gHandler,filename, args);
    }
    //------------------------------------------------------------------------
    private native void KifExecuteCodeProgramParseurImplementation(int gHandler,String code,String args)
            throws Exception;

    /**
     * Execute a KiF program from string within a given grammar
     *
     * @param gHandler
     * @param code of the KiF program to execute in a given grammar
     * @param args arguments as a string, where each parameter is separated with a space
     * @exception Exception
     */
    public synchronized void KifExecuteCodeProgramParseur(int gHandler,String code,String args) throws Exception {
        KifExecuteCodeProgramParseurImplementation(gHandler,code, args);
    }
    //------------------------------------------------------------------------

    private native String KifExecuteFunctionParseurImplementation(int gHandler,String funcname,String[] parameters)
            throws Exception;

    /**
     * Execute a KiF function, which has been loaded in a KifExecuteProgram for a given grammar
     *
     * @param gHandler
     * @param funcname the KiF function name
     * @param parameters the KiF function parameters as a vector of strings. Each will be mapped over the function arguments.
     * @return 
     * @exception Exception
     */
    public synchronized String KifExecuteFunctionParseur(int gHandler,String funcname,String[] parameters) throws Exception {
        return KifExecuteFunctionParseurImplementation(gHandler,funcname, parameters);
    }
    //------------------------------------------------------------------------

    private native void XipSetRuleLimitImplementation(int gHandler, int ruleNb)
            throws Exception;

    /**
     * parse until the rule number
     *
     * @param gHandler a handler to a grammar
     * @param ruleNb
     * @exception Exception
     */
    public synchronized void XipSetRuleLimit(int gHandler, int ruleNb) throws Exception {
        //System.out.println("JXip2::XipSetRuleLimit() ruleNb = " + ruleNb);
        XipSetRuleLimitImplementation(gHandler, ruleNb);
    }
    //------------------------------------------------------------------------

    private native boolean xipDeactivateRuleSpaceImplementation(int gHandler,
            String label) throws Exception;

    /**
     * Deactivate a specific rule space
     *
     * @param gHandler a handler to a grammar
     * @param label the label
     * @return
     * @exception Exception
     * @deprecated
     */
    //TODO must be remove
    public synchronized boolean XipAddBlockingRuleSpace(int gHandler, String label) throws Exception {
        return xipDeactivateRuleSpaceImplementation(gHandler, label);
    }

    /**
     * Deactivate a specific rule space
     *
     * @param gHandler a handler to a grammar
     * @param label the label (ie: xx.yy.zz)
     * @return true if no error
     * @exception Exception
     */
    public synchronized boolean deactivateRuleSpace(int gHandler, String label) throws Exception {
        return xipDeactivateRuleSpaceImplementation(gHandler, label);
    }
    //------------------------------------------------------------------------

    private native boolean xipActivateRuleSpaceImplementation(int gHandler,
            String label, boolean full) throws Exception;

    /**
     * Activate a specific rule space
     *
     * @param gHandler a handler to a grammar
     * @param label the label
     * @param full
     * @return true if no error
     * @exception Exception
     * @deprecated
     */
    //TODO must be remove
    public synchronized boolean XipRemoveBlockingRuleSpace(int gHandler,
            String label, boolean full) throws Exception {
        return xipActivateRuleSpaceImplementation(gHandler, label, full);
    }

    /**
     * Activate a specific rule space
     *
     * @param gHandler a handler to a grammar
     * @param label the label (ie: xx.yy.zz)
     * @param full full=true to activate up to the root
     * @return true if no error
     * @exception Exception
     */
    public synchronized boolean activateRuleSpace(int gHandler,
            String label, boolean full) throws Exception {
        return xipActivateRuleSpaceImplementation(gHandler, label, full);
    }
    //------------------------------------------------------------------------

    private native boolean XipSetLabelRuleLimitImplementation(int gHandler,
            String label) throws Exception;

    /**
     * parse until the label
     *
     * @param gHandler a handler to a grammar
     * @param label the label
     * @return
     * @exception Exception
     */
    public synchronized boolean XipSetLabelRuleLimit(int gHandler,
            String label) throws Exception {
        //System.out.println("JXip2::XipSetLabelRuleLimit() label = " + label);
        return XipSetLabelRuleLimitImplementation(gHandler, label);
    }
    //------------------------------------------------------------------------

    private native void XipReturnRulesImplementation(int gHandler) throws Exception;

    /**
     * parse until the rule number and return the vector of rules applied
     *
     * @param gHandler a handler to a grammar
     * @return
     * @exception Exception
     */
    public synchronized Vector<Integer> XipReturnRules(int gHandler) throws Exception {
        //System.out.println("JXip2::XipReturnRules() ");
        applyedRuleNbVect = new Vector<Integer>();
        XipReturnRulesImplementation(gHandler);
        return applyedRuleNbVect;
    }

    /**
     * add new applied rule number to the vector
     *
     * This callback method is called by the C++ method
     * JXipImp2::XipReturnRulesImplementation()
     *
     * @param ruleNb
     */
    private void XipReturnRulesCB(int ruleNb) {
        //System.out.println("JXip2::XipReturnRulesCB ruleNb = " + ruleNb);
        applyedRuleNbVect.add(new Integer(ruleNb));
    }
    //------------------------------------------------------------------------

    private native boolean XipIsGrammarCryptedImplementation(int gHandler) throws Exception;

    /**
     * @param gHandler a handler to a grammar
     * @return
     * @exception Exception
     */
    public synchronized boolean XipIsGrammarCrypted(int gHandler) throws Exception {
        //System.out.println("JXip2::XipIsGrammarCrypted() ");
        return XipIsGrammarCryptedImplementation(gHandler);
    }

    //------------------------------------------------------------------------
    private native void ListOfOperationsImplementation() throws Exception;

    /**
     * Return the list of operations provided for the scripts
     *
     * @return
     * @exception Exception
     */
    public synchronized Hashtable<String, Hashtable> ListOfOperations() throws Exception {
        //System.out.println("JXip2::ListOfOperations() ");
        listOfOp = new Hashtable<String, Hashtable>();
        ListOfOperationsImplementation();
        return listOfOp;
    }

    /**
     *
     * This callback method is called by the C++ method
     * JXipImp2::ListOfOperationsImplementation()
     *
     * @param key
     * @param value
     */
    private void listOfOperationsCB1(String key, String value) {
        //System.out.println("JXip2::ListOfOperationsCB1 key = " + key + " value = " + value);
        listOfOpdetail.put(key, value);
    }

    /**
     *
     * This callback method is called by the C++ method
     * JXipImp2::ListOfOperationsImplementation()
     *
     * @param key
     */
    private void listOfOperationsCB2(String key) {
        //System.out.println("JXip2::ListOfOperationsCB2 key = " + key);
        int ind = key.indexOf('.');
        listOfOpdetail = new Hashtable<String, String>();
        listOfOp.put(key.substring(ind + 2), listOfOpdetail);
    }

    //------------------------------------------------------------------------
    private native void parseToDBImplementation(String grmfile, String machine,
            String db, String user, String password, String fileName,
            boolean storeSentence, boolean cleanDB) throws Exception;

    /**
     * parse a file and store the result in a DB
     *
     * @param grmfile grm file name
     * @param machine dataBase server name
     * @param db the database name
     * @param user login on the DB
     * @param password password
     * @param fileName file to parse
     * @param storeSentence true=>
     * @param cleanDB true => clean the DB before parsing this file, otherwise
     * add the result to the D
     * @exception Exception
     */
    public synchronized void parseToDB(String grmfile, String machine,
            String db, String user, String password, String fileName,
            boolean storeSentence, boolean cleanDB) throws Exception {
        parseToDBImplementation(grmfile, machine, db, user, password,
                fileName, storeSentence, cleanDB);
    }

    //------------------------------------------------------------------------
    /**
     * Gets the fileIndex in the listOfFileName
     *
     * @param fileName
     * @param listOfFileName
     * @return The fileIndex value
     */
    private int getFileIndex(String fileName, Hashtable<Integer, String> listOfFileName) {
        int fileIndex = 0;
        if (listOfFileName.containsValue(fileName)) {
            for (Enumeration enum1 = listOfFileName.keys(); enum1.hasMoreElements();) {
                Integer key = (Integer) enum1.nextElement();
                fileIndex = key.intValue();
                String fileN = listOfFileName.get(key);
                if (fileN.compareTo(fileName) == 0) {
                    break;
                }
            }
            //System.out.println("JXip2::getFileIndex FOUND fileIndex = " + fileIndex +
            //		" fileName = " + listOfFileName.get(fileIndex));
        } else {
            fileIndex = listOfFileName.size() + 1;
            if (fileName.compareTo("nofile") == 0) {
                fileName = "";
            }
            listOfFileName.put(new Integer(fileIndex), fileName);
            //System.out.println("JXip2::getFileIndex ADDED fileIndex = " + fileIndex +
            //		" fileName = " + listOfFileName.get(fileIndex));
        }
        return fileIndex;
    }
    String text;
    int previousFileIndex;

    /**
     *
     *
     * @param fileName
     * @param regExpr
     * @param fileIndex
     * @param caseSensitive
     * @return
     */
    private int computefileOffset(String regExpr, String fileName,
            int fileIndex, boolean caseSensitive) {
        //System.out.println("computefileOffset " + regExpr + " " + fileIndex + " " + fileName);
        int offset = 0;
        if (fileName == null) {
            return -1;
        }
        if (fileName.compareTo("") == 0) {
            return -1;
        }
        try {
            if (previousFileIndex != fileIndex) {
                // read all the file only if it is in a other file
                //System.out.println("READING: " + fileName);
                BufferedReader br = new BufferedReader(new FileReader(fileName));
                String line = "";
                text = "";
                while ((line = br.readLine()) != null) {
                    text += line + "\n";
                }
                //System.out.println(text);
            }
            previousFileIndex = fileIndex;
            Pattern pattern;
            //now find the pattern
            if (caseSensitive) {
                pattern = Pattern.compile(regExpr);
            } else {
                pattern = Pattern.compile(regExpr, Pattern.CASE_INSENSITIVE);
            }
            Matcher matcher = pattern.matcher(text);
            if (matcher.find()) {
                offset = matcher.start();
                //System.out.println("    FOUND " + regExpr + "\t"
                // + offset + "\t" + fileName + " " + caseSensitive);
            } else {
                offset = 0;
                System.out.println("NOT FOUND " + regExpr + "\t"
                        + offset + "\t" + fileName + " " + caseSensitive);
            }
        } catch (Exception ex) {
            System.out.println("computefileOffset Exception: "
                    + ex.getMessage() + " " + regExpr + " " + fileName);
            offset = 0;
        }
        //System.out.println("computefileOffset " + regExpr + " " + offset + " " + fileName);
        return offset;
    }

    //------------------------------------------------------------------------
    private native void xipGetListCategoriesImplementation(int gHandler) throws Exception;

    /**
     *
     *
     * @param gHandler
     * @return return a Vector of GramCategory
     * @exception Exception
     */
    @SuppressWarnings("unchecked")
    public synchronized Vector<GramCategory> xipGetListCategories(int gHandler)
            throws Exception {
        listCategories = new Vector<GramCategory>();
        // this setting is required for computefileOffset()
        previousFileIndex = -1;
        listOfCategoryFileName = new Hashtable<Integer, String>();
        xipGetListCategoriesImplementation(gHandler);
        // remove pseudo category
        listCategories.remove(0);
        Collections.sort(listCategories);
        return listCategories;
    }

    /**
     *
     * This callback method is called by the C++ method
     * JXipImp2::xipGetListCategoriesImplementation()
     *
     * @param catName
     * @param fileName
     */
    private void xipGetListCategoriesCB(String catName, String fileName) {
        //System.out.println("JXip2::xipGetListCategoriesCB name = " + catName + " fileName = " + fileName);
        if (catName.compareTo("?") == 0) {
            return;
        }
        int fileIndex = getFileIndex(fileName, listOfCategoryFileName);
        String regExpr = "[ \\t]*" + catName + "[ \\t]*=[ \\t]*\\[";
        int offset = computefileOffset(regExpr,
                listOfCategoryFileName.get(new Integer(fileIndex)), fileIndex, false);
        GramCategory gramCategory = new GramCategory(catName,
                fileIndex, listOfCategoryFileName, offset);
        listCategories.add(gramCategory);
    }
    //------------------------------------------------------------------------

    private native void xipGetListDependenciesImplementation(int gHandler)
            throws Exception;

    /**
     *
     *
     * @param gHandler
     * @return Return a Vector of GramDependency
     * @exception Exception
     */
    public synchronized Vector<GramDependency> xipGetListDependencies(int gHandler)
            throws Exception {
        listDependencies = new Vector<GramDependency>();
        // this setting is required for computefileOffset()
        previousFileIndex = -1;
        listOfDependencyFileName = new Hashtable<Integer, String>();
        xipGetListDependenciesImplementation(gHandler);
        // remove pseudo dependency
        listDependencies.remove(0);
        Collections.sort(listDependencies);
        return listDependencies;
    }

    /**
     *
     * This callback method is called by the C++ method
     * JXipImp2::xipGetListDependenciesImplementation()
     *
     * @param depName
     * @param fileName
     */
    private void xipGetListDependenciesCB(String depName, String fileName) {
        //System.out.println("\nJXip2::xipGetListDependenciesCB name = " + depName + " fileName = " + fileName);
        int fileIndex = getFileIndex(fileName, listOfDependencyFileName);
        String regExpr = "[ \\t]*" + depName + "[ \\t]*[,=\\.\\n][ \\t]*";
        int offset = computefileOffset(regExpr, listOfDependencyFileName.get(
                new Integer(fileIndex)), fileIndex, false);
        GramDependency gramDependency = new GramDependency(depName, fileIndex,
                listOfDependencyFileName, offset);
        listDependencies.add(gramDependency);
    }
    //------------------------------------------------------------------------

    private native void xipGetListVariablesImplementation(int gHandler) throws Exception;

    /**
     *
     *
     * @param gHandler
     * @return Return a Vector of GramVariable
     * @exception Exception
     */
    @SuppressWarnings("unchecked")
    public synchronized Vector<GramVariable> xipGetListVariables(int gHandler) throws Exception {
        listVariable = new Vector<GramVariable>();
        xipGetListVariablesImplementation(gHandler);
        Collections.sort(listVariable);
        return listVariable;
    }

    /**
     *
     * This callback method is called by the C++ method
     * JXipImp2::xipGetListVariablesImplementation()
     *
     * @param varName
     * @param type
     */
    private void xipGetListVariablesCB(String varName, int type) {
        //System.out.println("JXip2::xipGetListVariablesCB varName = " + varName + " type = " + type);
        GramVariable gramVariable = new GramVariable(varName, type);
        listVariable.add(gramVariable);
    }
    //------------------------------------------------------------------------

    private native void xipGetFeatureTreeImplementation(int gHandler) throws Exception;

    /**
     *
     *
     * @param gHandler
     * @return
     * @exception Exception
     */
    public synchronized Vector<GramFeature> xipGetFeatureTree(int gHandler) throws Exception {
        listFeaturesMgt = new Vector<Vector<GramFeature>>();
        listFeatures = new Vector<GramFeature>();
        listFeaturesMgt.add(listFeatures);
        listOfFeatureFileName = new Hashtable<Integer, String>();
        // this setting is required for computefileOffset()
        previousFileIndex = -1;
        //System.out.println("listFeaturesMgt.size() = " + listFeaturesMgt.size());
        xipGetFeatureTreeImplementation(gHandler);
        int indexLast = listFeaturesMgt.size();
        //System.out.println("xipGetFeatureTree indexLast = " + indexLast);
        listFeatures = listFeaturesMgt.get(indexLast - 1);
        listFeaturesMgt.remove(indexLast - 1);
        return listFeatures;
    }

    /**
     *
     *
     * @param name
     * @return
     */
    private String buildFeatureRegExpr(String name) {
        return "[ \\t]*" + name + "[ \\t]*[:,]*[ \\t]*";
    }

    /**
     *
     * This callback method is called by the C++ method
     * JXipImp2::constructionFeatureTree()
     *
     * @param name feature name
     * @param value feature value
     * @param fileIndex file index
     */
    private void xipGetFeatureTreeCB1(String name, String value, int fileIndex) {
        //System.out.println("\txipGetFeatureTreeCB1() " + name + " = " + value);
        //String regExpr = name + "";
        String regExpr = buildFeatureRegExpr(name);
        int offset = computefileOffset(regExpr,
                listOfFeatureFileName.get(new Integer(fileIndex)), fileIndex, false);
        GramFeature gramFeature1 = new GramFeature(value, null, fileIndex,
                listOfFeatureFileName, offset);
        Vector<GramFeature> child = new Vector<GramFeature>();
        child.add(gramFeature1);
        GramFeature gramFeature2 = new GramFeature(name, child, fileIndex,
                listOfFeatureFileName, offset);
        listFeatures.add(gramFeature2);
    }

    /**
     *
     * This callback method is called by the C++ method
     * JXipImp2::constructionFeatureTree()
     *
     * @param name feature name
     */
    private void xipGetFeatureTreeCB2(String name) {
        //System.out.println("\nxipGetFeatureTreeCB2() " + name);
        listFeaturesMgt.add(listFeatures);
        //System.out.println("xipGetFeatureTreeCB2 listFeaturesMgt.size() = " + listFeaturesMgt.size());
        listFeatures = new Vector<GramFeature>();
    }

    /**
     * This callback method is called by the C++ method
     * JXipImp2::constructionFeatureTree()
     *
     * @param name feature name
     * @param fileIndex file index
     */
    private void xipGetFeatureTreeCB3(String name, int fileIndex) {
        //System.out.println("\nxipGetFeatureTreeCB3() " + name + " " + listFeatures + "\n");
        //String regExpr = name + "";
        String regExpr = buildFeatureRegExpr(name);
        int offset = computefileOffset(regExpr,
                listOfFeatureFileName.get(new Integer(fileIndex)), fileIndex, false);
        GramFeature gramFeature = new GramFeature(name, listFeatures,
                fileIndex, listOfFeatureFileName, offset);
        int indexLast = listFeaturesMgt.size();
        listFeatures = listFeaturesMgt.get(indexLast - 1);
        listFeatures.add(gramFeature);
        listFeaturesMgt.remove(indexLast - 1);
        //System.out.println("xipGetFeatureTreeCB3 listFeaturesMgt.size() = " + listFeaturesMgt.size());
    }

    /**
     * Save the file name for some feature declarations
     *
     * This callback method is called by the C++ method
     * JXipImp2::constructionFeatureTree()
     *
     * @param fileName file name of feature declaration
     * @param fileIndex file index
     */
    private void xipGetFeatureTreeCB4(String fileName, int fileIndex) {
        //System.out.println("xipGetFeatureTreeCB4() fileName = " + fileName + " fileIndex = " + fileIndex);
        listOfFeatureFileName.put(new Integer(fileIndex), fileName);
    }

    /**
     * test
     *
     * @param listFeat
     * @param level
     */
    void displayTreeFeature(Vector listFeat, int level) {
        //System.out.println("displayTreeFeature() listFeat = " + listFeat);
        //System.out.println("displayTreeFeature() listFeat.size() = " + listFeat.size());
        for (Enumeration enumeration = listFeat.elements(); enumeration.hasMoreElements();) {
            GramFeature gramFeature = (GramFeature) enumeration.nextElement();
            Vector child = gramFeature.getChild();
            //System.out.println("displayTreeFeature() child = " + child + " name = " + gramFeature.getName());
            String str = "";
            for (int i = 0; i < level; i++) {
                str += " ";
            }
            str += gramFeature.getName();
            //str += "[" + gramFeature.getFileName() + " " + gramFeature.getFileOffset() + "]";
            if (child != null) {
                str += "(";
                System.out.print(str);
                displayTreeFeature(child, level + 2);
                System.out.println(")");
            }
        }
    }

    //------------------------------------------------------------------------
    private native String xipGrammarPathnamesImplementation(int gHandler)
            throws Exception;
    private Vector<String> featureFilesTmp;
    private Vector<String> lexiconFilesTmp;
    private Vector<String> ruleFilesTmp;
    private Vector<String> paramFilesTmp;
    private Vector<String> scriptFilesTmp;
    private Vector<String> diversFilesTmp;

    /**
     *
     *
     * @param gHandler
     * @param featureFiles
     * @param lexiconFiles
     * @param ruleFiles
     * @param paramFiles
     * @param scriptFiles
     * @param diversFiles
     * @return language grammar file(english.xip)
     * @exception Exception
     */
    public synchronized String xipGrammarPathnames(int gHandler,
            Vector<String> featureFiles, Vector<String> lexiconFiles,
            Vector<String> ruleFiles, Vector<String> paramFiles,
            Vector<String> scriptFiles, Vector<String> diversFiles) throws Exception {
        featureFilesTmp = featureFiles;
        lexiconFilesTmp = lexiconFiles;
        ruleFilesTmp = ruleFiles;
        paramFilesTmp = paramFiles;
        scriptFilesTmp = scriptFiles;
        diversFilesTmp = diversFiles;
        return xipGrammarPathnamesImplementation(gHandler);
    }

    /**
     *
     * This callback method is called by the C++ method
     * JXipImp2::xipGrammarPathnamesImplementation()
     *
     * @param name
     * @param fileName
     */
    private void xipGrammarPathnamesCB(String name, String fileName) {
        //System.out.println("xipGrammarPathnamesCB() name = " + name + " fileName = " + fileName);
        if (name.compareTo("feature_pathnames") == 0) {
            featureFilesTmp.add(fileName);
        } else if (name.compareTo("lexicon_pathnames") == 0) {
            lexiconFilesTmp.add(fileName);
        } else if (name.compareTo("rule_pathnames") == 0) {
            ruleFilesTmp.add(fileName);
        } else if (name.compareTo("param_pathnames") == 0) {
            paramFilesTmp.add(fileName);
        } else if (name.compareTo("scripts_pathnames") == 0) {
            scriptFilesTmp.add(fileName);
        } else if (name.compareTo("divers_pathnames") == 0) {
            diversFilesTmp.add(fileName);
        } else {
            System.out.println("xipGrammarPathnamesCB() ERRROR");
        }
    }

    //------------------------------------------------------------------------
    /**
     * decrypte the grammar files specified by the grm file. THIS METHOD IS NOT
     * IMPLEMENTED.
     *
     * @param grmFileName the grm file name
     * @param gHandler grammar handler
     * @param destinationDir the directory where the grammar files will be
     * stored
     * @throws java.lang.Exception
     */
    public void decrypteGrammarFiles(String grmFileName, int gHandler,
            String destinationDir) throws Exception {
        throw new UnsupportedOperationException();
    }

    /**
     * Copy and encrypt the grammar files specified by the grm file, preserving
     * directory structure.
     *
     * @param grmFileName the grm file name
     * @param destinationDir the directory where the grammar files will be
     * copied
     * @throws Exception
     */
    public void copyAndEncryptGrammar(String grmFileName, String destinationDir) throws Exception {
        xipCopyAndEncryptGrammarImplementation(grmFileName, destinationDir);
    }

    private native void xipCopyAndEncryptGrammarImplementation(String grmFileName,
            String destinationDir) throws Exception;

    /**
     * copy the grammar files specified by the grm file. Flattens directory
     * structure and rewrites language.xip accordingly. To preserve directory
     * structure and encrypt, see copyAndEncryptGrammar.
     *
     * @param grmFileName the grm file name
     * @param gHandler grammar handler
     * @param destinationDir the directory where the grammar files will be
     * copied
     * @param crypte if true then crypte the grammar files. ENCRYPTION NOT YET
     * IMPLEMENTED.
     * @throws java.lang.Exception
     */
    public void copyAndCrypteGrammarFiles(String grmFileName, int gHandler,
            String destinationDir, boolean crypte) throws Exception {
        if (crypte) {
            throw new UnsupportedOperationException("encryption is not yet implemented in copyAndCrypteGrammarFiles.");
        }
        Vector<String> gramFiles = new Vector<String>();
        gramFiles.add(grmFileName);
        Vector<String> scriptFiles = new Vector<String>();
        String langGramFile = xipGrammarPathnames(gHandler, gramFiles, gramFiles,
                gramFiles, gramFiles, scriptFiles, gramFiles);
        gramFiles.add(langGramFile);
        File dir = new File(destinationDir);
        if (dir.exists()) {
            File[] listOfFiles = dir.listFiles();
            for (int i = 0; i < listOfFiles.length; i++) {
                listOfFiles[i].delete();
            }
            dir.delete();
        }
        dir.mkdir();
        //copy the grammar files
        for (Enumeration enumeration = gramFiles.elements(); enumeration.hasMoreElements();) {
            String fileName = (String) enumeration.nextElement();
            File fromFile = new File(fileName);
            File toFile = new File(destinationDir + File.separatorChar + fromFile.getName());
            FileChannel sourceChannel = new FileInputStream(fromFile).getChannel();
            FileChannel destinationChannel = new FileOutputStream(toFile).getChannel();
            sourceChannel.transferTo(0, sourceChannel.size(), destinationChannel);
            sourceChannel.close();
            destinationChannel.close();
        }
        String encoding = "8859_1";
        //remove file hierachy in the grm file
        File grmFile = new File(grmFileName);
        File newGrmFile = new File(destinationDir + File.separatorChar + grmFile.getName());
        BufferedReader inGrm = new BufferedReader(
                new InputStreamReader(new FileInputStream(grmFile), encoding));
        BufferedWriter outGrm = new BufferedWriter(
                new OutputStreamWriter(new FileOutputStream(newGrmFile), encoding));
        Pattern pattern1;
        Pattern pattern2;
        Matcher matcher;
        String output;
        //FEAT=$GRM_PATHNAME/../../FEATUREFILES replace: by FEAT=$GRM_PATHNAME
        pattern1 = Pattern.compile("(\\w[ \t]*)=[ \t]*(\\$GRM_PATHNAME).*");
        //$GRM_PATHNAME/../../NTMFILES/HMMFILES/hmm-1-std  replace by: $GRM_PATHNAME/hmm-1-std
        pattern2 = Pattern.compile("(\\$GRM_PATHNAME).*/([\\w\\-\\+\\_])");
        String input = inGrm.readLine();
        while (input != null) {
            matcher = pattern1.matcher(input);
            output = matcher.replaceAll("$1=$2");
            matcher = pattern2.matcher(output);
            output = matcher.replaceAll("$1/$2");
            output += "\n";
            outGrm.write(output, 0, output.length());
            input = inGrm.readLine();
        }
        inGrm.close();
        outGrm.close();
        //remove file hierachy in the script files( ntmscript, ...)
        for (Enumeration enumeration = scriptFiles.elements(); enumeration.hasMoreElements();) {
            String fileName = (String) enumeration.nextElement();
            //File scriptFile = new File(destinationDir + File.separatorChar + "ntmscript");
            File oldScriptFile = new File(fileName);
            File scriptFile = new File(destinationDir + File.separatorChar + oldScriptFile.getName());
            BufferedReader inScript = new BufferedReader(
                    new InputStreamReader(new FileInputStream(oldScriptFile), encoding));
            BufferedWriter outScript = new BufferedWriter(
                    new OutputStreamWriter(new FileOutputStream(scriptFile), encoding));
            //lex2 $NTM_ROOT/SOURCEFST/FORENTITIES/SpecialStrings.fst
            // replace by: lex2  $NTM_ROOT/SpecialStrings.fst
            pattern1 = Pattern.compile("(\\w[ \t]*)\\$.*/([\\w\\-\\+\\_]*\\.fst)");
            input = inScript.readLine();
            int lineCnt = 0;
            while (input != null) {
                matcher = pattern1.matcher(input);
                output = matcher.replaceAll("$1\\$NTM_ROOT/$2");
                output += "\n";
                outScript.write(output, 0, output.length());
                input = inScript.readLine();
            }
            inScript.close();
            outScript.close();
        }
        if (crypte) {
            //crypte the grammar files
            //change grammar: in gram:
        }
    }

    //------------------------------------------------------------------------
    private native boolean xipSetTaggerModeImplementation(int gHandler,
            boolean mode) throws Exception;

    /**
     * set HMM mode only
     *
     * @param gHandler
     * @param mode
     * @return
     * @exception Exception
     */
    public synchronized boolean xipSetTaggerMode(int gHandler, boolean mode)
            throws Exception {
        return xipSetTaggerModeImplementation(gHandler, mode);
    }
    //------------------------------------------------------------------------
    private Hashtable<String, java.lang.Object> gramInfo;

    /**
     * @param gHandler
     * @exception Exception
     */
    private native void xipGrammarInformationImplementation(int gHandler)
            throws Exception;

    /**
     * get Grammar information
     *
     * @param gHandler
     * @return
     * @exception Exception
     */
    public synchronized Hashtable<String, java.lang.Object> xipGrammarInformation(
            int gHandler) throws Exception {
        gramInfo = new Hashtable<String, java.lang.Object>();
        xipGrammarInformationImplementation(gHandler);
        return gramInfo;
    }

    /**
     *
     * This callback method is called by the C++ method
     * JXipImp2::xipGrammarInformationImplementation()
     *
     * @param infoName
     * @param infoValue
     */
    private void xipGrammarInformationCB1(String infoName, String infoValue) {
        gramInfo.put(infoName, infoValue);
    }

    /**
     *
     * This callback method is called by the C++ method
     * JXipImp2::xipGrammarInformationImplementation()
     *
     * @param infoName
     * @param infoValue
     */
    private void xipGrammarInformationCB2(String infoName, int infoValue) {
        gramInfo.put(infoName, new Integer(infoValue));
    }

    //------------------------------------------------------------------------
    private native int xipIndentFileImplementation(int gHandler, boolean mode)
            throws Exception;

    /**
     * set indent mode
     *
     * @param gHandler
     * @param mode true to set indent mode, false otherwise
     * @return false if error, true otherwise
     * @exception Exception
     */
    public synchronized boolean xipIndentFile(int gHandler, boolean mode)
            throws Exception {
        int res = xipIndentFileImplementation(gHandler, mode);
        if (res == -1) {
            return false;
        } else {
            return true;
        }
    }

    //------------------------------------------------------------------------
    private native int xipSetResultDirectoryImplementation(int gHandler,
            String dir) throws Exception;

    /**
     * set the XML result directory
     *
     * @param gHandler
     * @param dir
     * @return false if error, true otherwise
     * @exception Exception
     */
    public synchronized boolean xipSetResultDirectory(int gHandler,
            String dir) throws Exception {
        int res = xipSetResultDirectoryImplementation(gHandler, dir);
        if (res == -1) {
            return false;
        } else {
            return true;
        }
    }
    //------------------------------------------------------------------------

    private native void xmlUnitExtractionImplementation(int gHandler,
            String fileName) throws Exception;
    /**
     * Extract the XML bloc to parse from an xml input file
     *
     * @param gHandler
     * @param fileName
     * @return Return a Vector of (String)
     * @exception Exception
     */
    private Vector<String> xmlUnit;

    /**
     *
     * @param gHandler
     * @param fileName
     * @return
     * @throws Exception
     */
    public synchronized Vector<String> xmlUnitExtraction(int gHandler,
            String fileName) throws Exception {
        xmlUnit = new Vector<String>();
        xmlUnitExtractionImplementation(gHandler, fileName);
        return xmlUnit;
    }

    /**
     * @param xmlUnitElem one xmlUnit(String) from the xml file
     */
    private void xmlUnitExtractionImplementationCB(String xmlUnitElem) {
        //System.out.println("JXip2::xmlUnitExtractionImplementationCB xmlUnitElem = " + xmlUnitElem);
        xmlUnit.add(xmlUnitElem);
    }

    //------------------------------------------------------------------------
    private native void xipLayerRenumberingImplementation(int gHandler,
            int idRule) throws Exception;

    /**
     * layer renumbering for a file
     *
     * @param gHandler
     * @param idRule
     * @exception Exception
     */
    public synchronized void xipLayerRenumbering(int gHandler, int idRule) throws Exception {
        xipLayerRenumberingImplementation(gHandler, idRule);
    }

    //------------------------------------------------------------------------
    private native String xipGetLabelsImplementation(int gHandler,
            String labelName) throws Exception;

    /**
     * return the label used in the grammar for the surface
     *
     * @param gHandler grammar handler
     * @return the surface label
     * @exception Exception
     */
    public synchronized String xipGetLabelsSurface(int gHandler) throws Exception {
        return xipGetLabelsImplementation(gHandler, "surface");
    }

    /**
     * return the label used in the grammar for the lemme
     *
     * @param gHandler grammar handler
     * @return the lemme label
     * @exception Exception
     */
    public synchronized String xipGetLabelsLemme(int gHandler) throws Exception {
        return xipGetLabelsImplementation(gHandler, "lemme");
    }

    //------------------------------------------------------------------------
    private native void xipGetDictionaryVariableImplementation(int gHandler,
            String varName) throws Exception;
    private Hashtable<String, String> nameValue;

    /**
     * layer renumbering for a file
     *
     * @param gHandler grammar handler
     * @param varName variable name
     * @param nameValue
     * @exception Exception
     */
    public synchronized void xipGetDictionaryVariable(int gHandler,
            String varName, Hashtable<String, String> nameValue) throws Exception {
        this.nameValue = nameValue;
        xipGetDictionaryVariableImplementation(gHandler, varName);
    }

    /**
     * This callback method is called by the C++ method
     * JXipImp2::xipGetDictionaryVariableImplementation()
     *
     * @param name
     * @param value
     */
    private void xipGetDictionaryVariableCB(String name, String value) {
        //System.out.println("xipGetDictionaryVariableCB name = " + name + " = " + value);
        nameValue.put(name, value);
    }

    //------------------------------------------------------------------------
    private native void xipSetRuleRankNumberLimitImplementation(int gHandler,
            int index) throws Exception;

    /**
     * set the ruleLimit using the index in the applyed rule list
     *
     * @param gHandler grammar handler
     * @param index the index in the applyed rule list used to get the rulelimit
     * @exception Exception
     */
    public synchronized void xipSetRuleRankNumberLimit(int gHandler, int index) throws Exception {
        xipSetRuleRankNumberLimitImplementation(gHandler, index);
    }

    //------------------------------------------------------------------------
    private native void xipSetMaxSentenceImplementation(int gHandler,
            int maxSentenceNumber) throws Exception;

    /**
     * stop the parsing in this max sentence number
     *
     * @param gHandler grammar handler
     * @param maxSentenceNumber max sentence number
     * @exception Exception
     */
    public synchronized void xipSetMaxSentence(int gHandler,
            int maxSentenceNumber) throws Exception {
        xipSetMaxSentenceImplementation(gHandler, maxSentenceNumber);
    }
    //----------------------------- test ----------------------------------------------------------

    /**
     *
     *
     * @param gHandler
     * @param input
     */
    void parse(int gHandler, String input) throws Exception {
        XipResultManager xipResultManager = new XipResultManager();
        int res = xipResultManager.buildXipResult(gHandler, input);
        if (res != 0) {
            System.out.println("JXip2::parse() buildXipResult ERROR");
            System.exit(-1);
        }

        String out = xipResultManager.getXipOutput();
        System.out.println("\n---- the output ----\n" + out);
        XipResult xipRes = xipResultManager.getXipResult();

        boolean withFeature = true;
        String str = xipRes.toString(withFeature);
        System.out.println("\n ---- the xipResult ----\n" + str);

        String sentences = xipRes.getSentenceString();
        System.out.println("\n ---- the sentences ----\n" + sentences);
    }

    /**
     *
     * @param gHandler
     * @param jxip2
     * @param jxip
     * @exception Exception
     */
    void test0(int gHandler, JXip jxip, JXip2 jxip2) throws Exception {
        boolean mode = true;
        if (jxip2.xipIndentFile(gHandler, mode)) {
            System.out.println("xipIndentFile done to:" + mode);
        } else {
            System.out.println("xipIndentFile ERROR to:" + mode);
        }
        //String input = "/home/grondeau/projets/parSem/xip/dev/GRAMMARS/SPANISH/TEST/basic/Frio/x.txt";
        jxip.XipSetDisplayMode(gHandler, Const.UTF8_INPUT, 80);
        int err1 = jxip.XipParseFileToFile(gHandler, "input.xml", "output", Const.XML_NONE, (char) 1);
        if (err1 != 0) {
            System.out.println("Xip::main() XipParseFileToFile Error");
        }
    }

    /**
     *
     * @param gHandler
     * @param jxip2
     * @param stringToParse
     * @exception Exception
     */
    void test1(int gHandler, JXip2 jxip2, String stringToParse) throws Exception {
        Vector applyedRules;
        int index;
        String dir = "/home/grondeau/tmp";
        if (jxip2.xipSetResultDirectory(gHandler, dir)) {
            System.out.println("xipSetResultDirectory done to:" + dir);
        } else {
            System.out.println("xipSetResultDirectory ERROR to:" + dir);
        }
        boolean mode = true;
        if (jxip2.xipIndentFile(gHandler, mode)) {
            System.out.println("xipIndentFile done to:" + mode);
        } else {
            System.out.println("xipIndentFile ERROR to:" + mode);
        }

        if (jxip2.XipIsGrammarCrypted(gHandler)) {
            System.out.println("\n--------------- JXip2::main() crypted grammar ");
        } else {
            System.out.println("\n--------------- JXip2::main() not crypted grammar ");
        }

        index = 30;
        jxip2.XipSetRuleLimit(gHandler, index);
        jxip2.parse(gHandler, stringToParse);
        applyedRules = jxip2.XipReturnRules(gHandler);
        System.out.println("\n------------------JXip2::main() applyed rules: \n"
                + applyedRules.toString() + "\n");

        index = 858;
        jxip2.XipSetRuleLimit(gHandler, index);
        jxip2.parse(gHandler, stringToParse);
        applyedRules = jxip2.XipReturnRules(gHandler);
        System.out.println("\n-----------------JXip2::main() applyed rules: \n"
                + applyedRules.toString() + "\n");

        String label = "toto";
        if (jxip2.XipSetLabelRuleLimit(gHandler, label)) {
            jxip2.XipSetRuleLimit(gHandler, index);
            jxip2.parse(gHandler, stringToParse);
            applyedRules = jxip2.XipReturnRules(gHandler);
            System.out.println("\n---------------------JXip2::main() applyed rules: \n"
                    + applyedRules.toString() + "\n");
        } else {
            System.out.println("JXip2::main() label unknow: " + label);
        }
        System.out.println("\n---------------- JXip2::main() listOfOperation:\n ");
        Hashtable<String, Hashtable> listOfOperations = jxip2.ListOfOperations();
        for (Enumeration enumeration = listOfOperations.keys(); enumeration.hasMoreElements();) {
            String group = (String) enumeration.nextElement();
            System.out.println(group);
            Hashtable listOfFct = listOfOperations.get(group);
            for (Enumeration enumeration2 = listOfFct.keys(); enumeration2.hasMoreElements();) {
                String fct = (String) enumeration2.nextElement();
                System.out.println("\tt" + fct + "\t\t" + listOfFct.get(fct));
            }
        }
    }

    /**
     * parse a file and stoke result in DB
     *
     * @param grmFile
     * @param jxip2
     * @exception Exception
     */
    void test2(String grmFile, JXip2 jxip2) throws Exception {
        String machine = "bionnassay.grenoble.xrce.xerox.com";
        String db = "litigation";
        String user = "root";
        String password = "root";
        //String fileName = "x:\\projets\\parSem\\xip\\dev\\libStorage\\test\\test1\\myfile.txt";
        String fileName = "../libStorage/test/test1/myfile.txt";
        boolean storeSentence = true;
        boolean cleanDB = true;
        jxip2.parseToDB(grmFile, machine, db, user, password, fileName, storeSentence, cleanDB);
    }

    /**
     *
     * @param gHandler
     * @param jxip2
     * @exception Exception
     */
    void test3(int gHandler, JXip2 jxip2) throws Exception {
        Vector listFeat = jxip2.xipGetFeatureTree(gHandler);
        displayTreeFeature(listFeat, 0);
        Vector<GramCategory> listCategories1 = jxip2.xipGetListCategories(gHandler);
        System.out.println("JXip2::xipGetListCategories listCategories = " + listCategories1);
        Vector<GramDependency> listDependencies1 = jxip2.xipGetListDependencies(gHandler);
        System.out.println("JXip2::xipGetListDependencies listDependencies = " + listDependencies1);
        Vector<GramVariable> listVariable1 = jxip2.xipGetListVariables(gHandler);
        System.out.println("JXip2::xipGetListVariables listVariables = " + listVariable1);
    }

    /**
     *
     * @param gHandler
     * @param jxip2
     * @exception Exception
     */
    void test4(int gHandler, JXip2 jxip2) throws Exception {
        String gramFile;
        Vector<String> featureFiles = new Vector<String>();
        Vector<String> lexiconFiles = new Vector<String>();
        Vector<String> ruleFiles = new Vector<String>();
        Vector<String> paramFiles = new Vector<String>();
        Vector<String> scriptFiles = new Vector<String>();
        Vector<String> diversFiles = new Vector<String>();
        gramFile = jxip2.xipGrammarPathnames(gHandler, featureFiles,
                lexiconFiles, ruleFiles, paramFiles, scriptFiles, diversFiles);
        System.out.println("gramFile = " + gramFile);
        System.out.println("featureFiles = " + featureFiles);
        System.out.println("lexiconFiles = " + lexiconFiles);
        System.out.println("ruleFiles = " + ruleFiles);
        System.out.println("paramFiles = " + paramFiles);
        System.out.println("scriptFiles = " + scriptFiles);
        System.out.println("diversFiles = " + diversFiles);
        Hashtable<String, java.lang.Object> grmInfo = jxip2.xipGrammarInformation(gHandler);
        for (Enumeration enum1 = grmInfo.keys(); enum1.hasMoreElements();) {
            String infoName = (String) enum1.nextElement();
            if ((infoName.compareTo("max_words") == 0)
                    || (infoName.compareTo("max_size_word") == 0)
                    || (infoName.compareTo("featurenumber") == 0)
                    || (infoName.compareTo("hmm") == 0)
                    || (infoName.compareTo("reentrance") == 0)
                    || (infoName.compareTo("encodage_lexique") == 0)) {
                int infoValue = ((Integer) grmInfo.get(infoName)).intValue();
                System.out.println(infoName + " = " + infoValue);
            } else {
                String infoValue = (String) grmInfo.get(infoName);
                System.out.println(infoName + " = " + infoValue);
            }
        }
    }

    /**
     *
     * @param gHandler
     * @param jxip2
     * @param fileName
     * @exception Exception
     */
    void test5(int gHandler, String fileName, JXip2 jxip2) throws Exception {
        Vector<String> xmlUnit1 = jxip2.xmlUnitExtraction(gHandler, fileName);
        System.out.println(xmlUnit1.toString());
    }

    void test6(String grmFile, int gHandler) throws Exception {
        String destinationDir = "xxxx";
        boolean crypte = true;
        copyAndCrypteGrammarFiles(grmFile, gHandler, destinationDir, crypte);
    }

    void test7(String grmFile, int gHandler, JXip2 jxip2) throws Exception {
        jxip2.parse(gHandler, "The market has fallen today.");
        Hashtable<String, String> nameValue1 = new Hashtable<String, String>();
        xipGetDictionaryVariable(gHandler, "temp_dict", nameValue1);
        System.out.println("xipGetDictionaryVariable:\n" + nameValue1);
    }

    /**
     *
     *
     * @param args
     */
    public static void main(String[] args) {
        try {
            String input;
            String gram;
            if (args.length != 2) {
                if (DynLoaderFromJar.isWindows()) {
                    gram = "z:\\users\\grondeau\\parSem\\xip\\dev\\GRAMMARS\\ENGLISH\\GRMFILES\\GRAM_GEN\\gram_gen_entit.grm";
                } else {
                    gram = "/opt/ParSem/users/grondeau/parSem/xip/dev/GRAMMARS/ENGLISH/GRMFILES/GRAM_GEN/gram_gen_entit.grm";
                }
                input = "This is a test.";
            } else {
                gram = args[0];
                input = args[1];
            }
            System.out.println("JXip2::main() Loading grammar file: " + gram);
            System.out.println("JXip2::main() parsing the string = " + input);
            //JXip jxip = new JXip();
            JXip jxip = new JXip(null, false); // to be used with netbeans

            JXip2 jxip2 = new JXip2();
            jxip.InitErrorMsg();
            int gHandler = jxip.XipGrmFile(gram, 0, true);
            System.out.println("xipGetLabelsLemme() = " + jxip2.xipGetLabelsLemme(gHandler));
            System.out.println("xipGetLabelsSurface() = " + jxip2.xipGetLabelsSurface(gHandler));
            //jxip2.xipSetTaggerMode(gHandler, true);
            System.out.println("---------------------error message start-----------------------------");
            System.out.println(jxip.getErrorMsg());
            System.out.println("---------------------error message end-----------------------------");
            //jxip2.test0(gHandler, jxip, jxip2);
            //jxip2.test1(gHandler, jxip2, input);
            //jxip2.test2(gram, jxip2);
            jxip2.test3(gHandler, jxip2);
            //jxip2.test4(gHandler, jxip2);
            //jxip2.test5(gHandler, input, jxip2);
            //jxip2.test6(gram, gHandler);
            //jxip2.test7(gram, gHandler, jxip2);
            //jxip2.test0(gHandler, jxip, jxip2);
            //XipResultManager resMa = new XipResultManager();
            //resMa.buildXipResultFromFile(gHandler, "input.xml");
            //jxip2.test5(gHandler, input, jxip2);
            //resMa.buildXipResultFromFile(gHandler, "input.xml");
        } catch (Exception ex) {
            System.out.println("JXip2::main() --------- printStackTrace --------");
            //ex.printStackTrace();
            System.exit(-1);
        }
    }
}
