/*
 *   Xerox Research Centre Europe - Grenoble Laboratory
 *           project XIP (Xerox Incremental Parser)
 *  Copyright (C) 2004 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved
 *
 */
package com.xerox.jxip;

import java.io.File;
import java.util.Vector;

/**
 * The purpose of this class is to provide a Java implementation of the XIP
 * library.
 *
 * @author grondeau @created April 4, 2005
 */
public class JXip {

    /**
     * Constructor for the JXip object
     *
     * @exception Exception
     */
    public JXip() throws Exception {
        DynLoaderFromJar.loadLibFromJar();
    }

    /**
     * Constructor for the JXip object the libraries inside the jar are not
     * loaded
     *
     * @param libsArr array of dynamic libraries to load
     * @param loadingLibInJar if true the dynamic libraries inside the jar will
     * be also loaded, otherwise they are not loaded
     * @throws java.lang.Exception
     */
    public JXip(String libsArr[], boolean loadingLibInJar) throws Exception {
        String libs[] = {""};
        if (libsArr != null) {
            libs = libsArr;
        } else {
            String rootLib = "";
            if (DynLoaderFromJar.isWindows()) {
                rootLib = "Z:\\users\\grondeau\\parSem\\xip\\dev\\java\\build\\lib\\jXip\\windows64\\sharedLib";
                libs[0] = rootLib + "\\jXip.dll";
            } else {
                rootLib = "/opt/ParSem/users/grondeau/parSem/xip/dev/java/build/lib/jXip/linux64/sharedLib";
                libs[0] = rootLib + "/libjXip.so";
            }
        }
        DynLoaderFromJar.loadLibFromJar(libs, loadingLibInJar);
    }

    //	/**
//	*Constructor for the JXip object
//	*
//	* @param  jarFile        
//	* @exception  Exception  
//	*/
//	public JXip(String jarFile) throws Exception {
//		new DynLoaderFromJar(jarFile);
//	}
    /**
     * This method is used to be sured than the string created by a C++ native
     * method will be duplicated before calling the deleteLocalJStringRef method
     * which tell the GC to garbage this data
     *
     * @param str
     * @return
     */
    public static String duplicateString(String str) {
        if (str != null) {
            return new String(str); //mandatory to use new()
        } else {
            return null;
        }
    }

    /**
     * Returns the canonical pathname string of this abstract pathname.
     *
     * @param fileName
     * @return
     * @exception Exception
     */
    private synchronized String convertToCanonicalFilename(String fileName) throws Exception {
        return new File(fileName).getCanonicalPath();
    }

//    //--------------------------------------------------------------------------------------
//    private native void initXipExceptionImplementation();
//
//    /**
//     * Throw a XipException using the information 
//     * provided by xiperror in the c++ API. This information
//     * will be the parameters of the initXipExceptionImplementationCB method. 
//     * initXipExceptionImplementationCB will be callback by the c++ code
//     * @exception  XipException  
//     */
//    private synchronized void throwXipException() throws XipException {
//        System.out.println("JXip::throwXipException()");
//        initXipExceptionImplementation();
//        throw xipException;
//    }
//    /**
//     * create java Exception
//     *
//     * This callback method is called by the C++ method JXipImp::initXipExceptionImplementation()
//     *
//     * @param  mess   
//     * @param  file   
//     * @param  line   
//     * @param  pos    
//     * @param  timeS  
//     */
//    private void initXipExceptionImplementationCB(String mess, String file,
//            String timeS, long line, long pos) {
//        System.out.println("JXip::initXipExceptionImplementationCB()");
//        xipException = new XipException(mess, file, timeS, line, pos);
//    }
    //------------------------------------------------------------------------
    private native void setEnvImplementation(String variableString) throws Exception;

    /**
     * change the value of a native environment variable
     *
     * @param name the environment variable name
     * @param value The new value
     * @exception Exception
     */
    public synchronized void setEnv(String name, String value) throws Exception {
        setEnvImplementation(name + '=' + value);
    }
    //------------------------------------------------------------------------

    private native String getEnvImplementation(String name) throws Exception;

    /**
     * Get the value of a native environment variable
     *
     * @param name The environment variable name.
     * @return The environment variable value
     * @exception Exception
     */
    public synchronized String getEnv(String name) throws Exception {
        return getEnvImplementation(name);
    }
    //------------------------------------------------------------------------

    private native int XipGrmFileImplementation(String grammarFileName,
            int enableXml, boolean loadFst) throws Exception;

    /**
     * load grammar files using a grm file
     *
     * @param grmFileName the grm file name
     * @param enableXml when set to one, this parameter tells XIP that the input
     * complies with the XIP XML DTD
     * @param loadFst if true loads the FST otherwise parse only the grammar
     * file without loading FST
     * @return a handler to a grammar or -1 if error
     * @exception Exception
     */
    public synchronized int XipGrmFile(String grmFileName, int enableXml,
            boolean loadFst) throws Exception {
        //System.out.println("JXip::XipGrmFile()");
        int gHandler = -1;
        //System.out.println("JXip::XipGrmFile() Loading grm file " + grmFileName);
        //try {
        gHandler = XipGrmFileImplementation(convertToCanonicalFilename(grmFileName),
                enableXml, loadFst);
        //} catch (XipException ex) {
        //System.out.println("XipException: " + ex);
//            throwXipException();
        //}
        return gHandler;
    }

    //------------------------------------------------------------------------
    private native int XipLoadGrammarImplementation(String grammarFileName,
            String ntmFileName, String hmmFileName, int enableXml) throws Exception;

    /**
     * loading grammars, NTM and HMM ressources files.
     *
     * @param grammarFileName the grammar file name (.../english.xip)
     * @param ntmFileName the NTM script file name(.../ntmscript)
     * @param hmmFileName the HMM file name
     * @param enableXml When set to one , this parameter tells XIP that the
     * input complies with the XIP XML DTD
     * @return a handler to the grammar.
     * @exception Exception
     */
    public synchronized int XipLoadGrammar(String grammarFileName, String ntmFileName,
            String hmmFileName, int enableXml) throws Exception {
        int gHandler = -1;
        //try {
        gHandler = XipLoadGrammarImplementation(
                convertToCanonicalFilename(grammarFileName),
                convertToCanonicalFilename(ntmFileName),
                convertToCanonicalFilename(hmmFileName), enableXml);
//        } catch (XipException ex) {
//            throwXipException();
//        }
        return gHandler;
    }
    //------------------------------------------------------------------------

    private native int XipGrammarLoadingImplementation(String grammarFileName,
            String ntmFileName, String hmmFileName, int enableXml) throws Exception;

    /**
     * loading grammars, NTM and HMM ressources files. ressources files assuming
     * that the input files fit with the XIP XML DTD specifications.
     *
     * @param grammarFileName the grammar file name (.../english.xip)
     * @param ntmFileName the NTM script file name(.../ntmscript)
     * @param hmmFileName the HMM file name
     * @param enableXml When set to one , this parameter tells XIP that the
     * input complies with the XIP XML DTD
     * @return a handler to the grammar.
     * @exception Exception
     */
    public synchronized int XipGrammarLoading(String grammarFileName,
            String ntmFileName, String hmmFileName, int enableXml) throws Exception {
        int gHandler = -1;
        //try {
        gHandler = XipGrammarLoadingImplementation(grammarFileName, ntmFileName,
                hmmFileName, enableXml);
//        } catch (XipException ex) {
//            throwXipException();
//        }
        return gHandler;
    }
    //------------------------------------------------------------------------

    private native int XipReloadGrammarImplementation(int gHandler,
            char reloadParameter, String grmFileName) throws Exception;

    /**
     * Reload a grammar
     *
     * @param gHandler the handler to the grammar to reload
     * @param reloadParameter if true then the paramter files will be reloaded
     * otherwise they will be not reloaded
     * @param grmFileName the grm file of the grammar to reload
     * @return a handler to the grammar.
     * @exception Exception
     */
    public synchronized int XipReloadGrammar(int gHandler, boolean reloadParameter,
            String grmFileName) throws Exception {
        int newGHandler = -1;
        char reload = 0;
        if (reloadParameter) {
            //parameter files are reloaded
            reload = 1;
        } else {
            //parameter files are not reloaded
            reload = 0;
        }
//        try {
        newGHandler = XipReloadGrammarImplementation(gHandler, reload, grmFileName);
//        } catch (XipException ex) {
//            throwXipException();
//            return -1;
//        }
        return newGHandler;
    }
    //------------------------------------------------------------------------

    private native int XipCleanXMLAliasImplementation(int gHandler, String alias) throws Exception;

    /**
     * This method clean the data in memory corresponding to the xmltree
     *
     * @param gHandler the handler to the grammar to reload
     * @param alias the alias
     * @return return -1 if error, 0 otherwise
     * @exception Exception
     */
    public synchronized int XipCleanXMLAlias(int gHandler, String alias) throws Exception {
        return XipCleanXMLAliasImplementation(gHandler, alias);
    }
    //------------------------------------------------------------------------

    private native int XipSetIntVariableImplementation(int gHandler, String variableName,
            float variableValue) throws Exception;

    /**
     * set a int variable
     *
     * @param gHandler a handler to the grammar.
     * @param variableName the variable name
     * @param value
     * @return return -1 if error, 0 otherwise
     * @exception Exception
     */
    public synchronized int XipSetIntVariable(int gHandler, String variableName,
            int value) throws Exception {
        float variableValue = (new Integer(value)).floatValue();
        return XipSetIntVariableImplementation(gHandler, variableName, variableValue);
    }
    //------------------------------------------------------------------------

    private native int XipSetStringVariableImplementation(int gHandler,
            String variableName, String variableValue) throws Exception;

    /**
     * set a variable
     *
     * @param gHandler a handler to the grammar.
     * @param variableName the variable name
     * @param variableValue the variable value
     * @return return -1 if error, 0 otherwise
     * @exception Exception
     */
    public synchronized int XipSetStringVariable(int gHandler, String variableName,
            String variableValue) throws Exception {
        return XipSetStringVariableImplementation(gHandler, variableName, variableValue);
    }
    //------------------------------------------------------------------------

    private native String XipGetStringVariableImplementation(int gHandler,
            String variableName) throws Exception;

    /**
     * get a variable string value
     *
     * @param gHandler a handler to the grammar.
     * @param variableName the variable name
     * @return
     * @exception Exception
     */
    public synchronized String XipGetStringVariable(int gHandler,
            String variableName) throws Exception {
        return XipGetStringVariableImplementation(gHandler, variableName);
    }
    //------------------------------------------------------------------------

    private native int XipLoadXMLDataBaseImplementation(int gHandler,
            String fileName, String alias) throws Exception;

    /**
     * load a XML database file
     *
     * @param gHandler a handler to the grammar.
     * @param fileName the XML database file
     * @param alias the alias name to associate with the database file
     * @return return -1 if error, 0 otherwise
     * @exception Exception
     */
    public synchronized int XipLoadXMLDataBase(int gHandler, String fileName,
            String alias) throws Exception {
        return XipLoadXMLDataBaseImplementation(gHandler, fileName, alias);
    }
    //------------------------------------------------------------------------

    private native int XipAliasListImplementation(int gHandler) throws Exception;
    private Vector<String> aliasList = new Vector<String>();

    /**
     * Adds an alias in the aliasList Vector.
     *
     * This callback method is called by the C++ method
     * JXipImp::XipAliasListImplementation()
     *
     * @param alias
     * @exception Exception
     */
    private void addAliasInListCB(String alias) throws Exception {
        //System.out.println("addAliasInListCB() callBack alias = " + alias);
        aliasList.add(alias);
    }

    /**
     * get the list of aliases
     *
     * @param gHandler a handler to the grammar.
     * @return a Vector of alias name (String)
     * @exception Exception
     */
    public synchronized Vector<String> XipAliasList(int gHandler) throws Exception {
        XipAliasListImplementation(gHandler);
        return aliasList;
    }
    //------------------------------------------------------------------------

    private native int XipParameterFileImplementation(int gHandler,
            String fileName) throws Exception;

    /**
     * load a parameter file
     *
     * @param gHandler a handler to a grammar
     * @param fileName a parameter file name
     * @return return -1 if error, 0 otherwise
     * @exception Exception
     */
    public synchronized int XipParameterFile(int gHandler,
            String fileName) throws Exception {
        int err;
        err = XipParameterFileImplementation(gHandler, fileName);
        return err;
    }
    //------------------------------------------------------------------------

    private native void XipSetDisplayModeImplementation(int gHandler,
            long displayMode, int colonne) throws Exception;

    /**
     * select a display mode
     *
     * @param gHandler a handler to a grammar
     * @param displayMode the display mode value
     * @param col number of char by line
     * @exception Exception
     */
    public synchronized void XipSetDisplayMode(int gHandler,
            long displayMode, int col) throws Exception {
        //System.out.println("XipSetDisplayMode:" + displayMode);
        XipSetDisplayModeImplementation(gHandler, displayMode, col);
    }
    //------------------------------------------------------------------------

    private native void XipAddFlagDisplayImplementation(int gHandler,
            long displayFlag, int colonne) throws Exception;

    /**
     * add display flag
     *
     * @param gHandler a handler to a grammar
     * @param displayFlag the display flag
     * @param col number of char by line
     * @exception Exception
     */
    public synchronized void XipAddFlagDisplay(int gHandler,
            long displayFlag, int col) throws Exception {
        //System.out.println("XipAddFlagDisplay:" + displayFlag);
        XipAddFlagDisplayImplementation(gHandler, displayFlag, col);
    }
    //----------------------------------------------------------------------------------------------------

    private native void XipRemoveFlagDisplayImplementation(int gHandler,
            long displayFlag) throws Exception;

    /**
     * remove a display flag
     *
     * @param gHandler a handler to a grammar
     * @param displayFlag the display flag
     * @exception Exception
     */
    public synchronized void XipRemoveFlagDisplay(int gHandler,
            long displayFlag) throws Exception {
        XipRemoveFlagDisplayImplementation(gHandler, displayFlag);
    }
    //------------------------------------------------------------------------

    private native boolean XipTestFlagDisplayImplementation(int gHandler,
            long displayFlag) throws Exception;

    /**
     * test a display flag
     *
     * @param gHandler a handler to a grammar
     * @param displayFlag the display flag
     * @return true or false
     * @exception Exception
     */
    public synchronized boolean XipTestFlagDisplay(int gHandler,
            long displayFlag) throws Exception {
        return XipTestFlagDisplayImplementation(gHandler, displayFlag);
    }
    //------------------------------------------------------------------------

    private native String getVersionImplementation() throws Exception;

    /**
     * Get version about the current XIP C++ Library used.
     *
     * @return The version value
     * @exception Exception
     */
    public synchronized String GetVersion() throws Exception {
        return getVersionImplementation();
    }

    //    /**
//     * Get version about the current XIP C++ Library used.
//     *
//     * @return                Version number.
//     * @exception  Exception  
//     * @deprecated            : replaced by getVersion
//     */
//        @Deprecated
//    public synchronized String getSystemInfo() throws Exception {
//        return getVersionImplementation();
//    }
    //------------------------------------------------------------------------
    private native int XipLicenseImplementation(int gHandler) throws Exception;

    /**
     * returns the number of days before the grammar licence expires
     *
     * @param gHandler a handler to a grammar
     * @return number of days
     * @exception Exception
     */
    public synchronized int XipLicense(int gHandler) throws Exception {
        return XipLicenseImplementation(gHandler);
    }

    //    /**
//     *  returns the number of days before the grammar licence expires
//     *
//     * @param  gHandler       a handler to a grammar
//     * @return                number of days
//     * @exception  Exception  
//     * @deprecated            replaced by XipLicense
//     */
//        @Deprecated
//    public synchronized int getLicenseCountDown(int gHandler) throws Exception {
//        return XipLicenseImplementation(gHandler);
//    }
    //------------------------------------------------------------------------
    private native String XipParseStringOSImplementation(int gHandler,
            String textInput, char xml, char apply_grammar) throws Exception;

    /**
     * parse one string
     *
     * @param gHandler a handler to a grammar
     * @param textInput string to parse
     * @param xml XML_NONE=0, XML_OUTPUT=1,XML_INSERT=2
     * @param apply_grammar
     * @return
     * @exception Exception
     */
    public synchronized String parseString(int gHandler, String textInput,
            char xml, char apply_grammar) throws Exception {
        //System.out.println("JXip::parseString() xml = " + xml + " apply = " + apply_grammar);
        //System.out.println("JXip::parseString() textInput = " + textInput);
        String res = XipParseStringOSImplementation(gHandler, textInput, xml, apply_grammar);
        //System.out.println("JXip::parseString() res = " + res);
        return res;
    }

    private native String XipParseStringStructureImplementation(int gHandler,
            String textInput, char xml, char apply_grammar) throws Exception;

    /**
     * parse a string that contains a tree structure
     *
     * @param gHandler a handler to a grammar
     * @param textInput structure string to parse
     * @param xml XML_NONE=0, XML_OUTPUT=1,XML_INSERT=2
     * @param apply_grammar
     * @return
     * @exception Exception
     */
    public synchronized String parseStringStructure(int gHandler, String textInput,
            char xml, char apply_grammar) throws Exception {
        //System.out.println("JXip::parseString() xml = " + xml + " apply = " + apply_grammar);
        //System.out.println("JXip::parseString() textInput = " + textInput);
        String res = XipParseStringStructureImplementation(gHandler, textInput, xml, apply_grammar);
        //System.out.println("JXip::parseString() res = " + res);
        return res;
    }

    /**
     * Parse one String.
     *
     * @param gHandler a handler to a grammar
     * @param textInput String to parse.
     * @return
     * @exception Exception
     */
    public synchronized String parseStringXml(int gHandler, String textInput) throws Exception {
        String res = XipParseStringOSImplementation(gHandler,
                textInput, Const.XML_OUTPUT, (char) 1);
        //System.out.println("JXip::parseStringXml() res = " + res);
        return res;
    }
    //------------------------------------------------------------------------

    private native String XipParseBArrayOSImplementation(int gHandler,
            byte[] bArray, char xml, char apply_grammar) throws Exception;

    /**
     * parse the string: Encodes this String into a sequence of bytes using the
     * named charset storing the result into a new byte array and parse this
     * array.
     *
     * @param gHandler a handler to a grammar
     * @param textInput string to parse
     * @param xml XML_NONE=0, XML_OUTPUT=1,XML_INSERT=2
     * @param apply_grammar
     * @param charSet iso-8859-1, utf-8, ...
     * @return
     * @exception Exception
     */
    public synchronized String parseStringCharSet(int gHandler,
            String textInput, String charSet, char xml, char apply_grammar) throws Exception {
        byte[] bArray = null;
        String res = "";
        bArray = textInput.getBytes(charSet);
        if (bArray != null) {
            res = XipParseBArrayOSImplementation(gHandler, bArray, xml, apply_grammar);
        }
        //System.out.println("JXip::parseString() res = " + res);
        return res;
    }
    //------------------------------------------------------------------------

    private native String parseFileXmlOSImplementation(int gHandler,
            String inputFile, int depth, char xml, char apply_grammar) throws Exception;

    /**
     *
     *
     * @param gHandler a handler to a grammar
     * @param inputFile
     * @param depth
     * @param xml
     * @param apply
     * @return
     * @exception Exception
     */
    public synchronized String XipParseFileXmlOS(int gHandler,
            String inputFile, int depth, char xml, char apply) throws Exception {
        String result = parseFileXmlOSImplementation(gHandler, inputFile, depth, xml, apply);
        return result;
    }
    //------------------------------------------------------------------------

    private native String parseStringXmlOSImplementation(int gHandler,
            String textInput, int depth, char xml, char apply_grammar) throws Exception;

    /**
     * Parse one String.
     *
     * @param gHandler a handler to a grammar a handler to a grammar
     * @param input text to parse
     * @param depth
     * @param xml XML_NONE=0, XML_OUTPUT=1,XML_INSERT=2
     * @param apply
     * @return
     * @exception Exception
     */
    public synchronized String parseStringXmlOS(int gHandler,
            String input, int depth, char xml, char apply) throws Exception {
        String result = parseStringXmlOSImplementation(gHandler, input, depth, xml, apply);
        return result;
    }

    /**
     * Parse one String. depth is set to 1, xml is set to XML_INSERT, and apply
     * is set to APPLY_GRAMMAR
     *
     * @param gHandler a handler to a grammar
     * @param input text to parse
     * @return String A XML result as a String.
     * @exception Exception
     */
    public synchronized String parseStringXmlOS(int gHandler, String input)
            throws Exception {
        char XML_INSERT = 2;
        char APPLY_GRAMMAR = 1;
        int DEPTH = 0;
        String result = parseStringXmlOSImplementation(gHandler, input,
                DEPTH, XML_INSERT, APPLY_GRAMMAR);
        return result;
    }
    //------------------------------------------------------------------------

    private native void XipSetDependencyExtractionImplementation(int gHandler, char ext)
            throws Exception;

    /**
     *
     *
     * @param gHandler a handler to a grammar
     * @param extraction
     * @exception Exception
     */
    public synchronized void XipSetDependencyExtraction(int gHandler, boolean extraction)
            throws Exception {
        char extr = 0;
        if (extraction) {
            extr = 1;
            //System.out.println("extraction DONE");
        } else {
            extr = 0;
            //System.out.println("extraction NOT DONE");
        }
        XipSetDependencyExtractionImplementation(gHandler, extr);
    }
    //------------------------------------------------------------------------

    private native void xipFreeAllGrammarsImplementation() throws Exception;

    /**
     * free all the grammars and all FST
     *
     * @exception Exception
     */
    public synchronized void XipFreeAllGrammars() throws Exception {
        xipFreeAllGrammarsImplementation();
    }

    //------------------------------------------------------------------------
    private native void xipFreeGrammarImplementation(int gHandler) throws Exception;

    /**
     * free a grammar but FST can't be really deleted
     *
     * @param gHandler a handler to a grammar
     * @exception Exception
     */
    public synchronized void XipFreeGrammar(int gHandler) throws Exception {
        xipFreeGrammarImplementation(gHandler);
    }

    //------------------------------------------------------------------------
    private native int XipParseFileToFileImplementation(int gHandler,
            String input, String output, char xml, char apply) throws Exception;

    /**
     *
     *
     * @param gHandler a handler to a grammar
     * @param output
     * @param xml
     * @param apply
     * @param input
     * @return
     * @exception Exception
     */
    public synchronized int XipParseFileToFile(int gHandler, String input,
            String output, char xml, char apply) throws Exception {
        return XipParseFileToFileImplementation(gHandler, input, output, xml, apply);
    }
    //------------------------------------------------------------------------

    private native int XipParseFileXMLToFileImplementation(int gHandler,
            String input, String output, int depth, char xml, char apply) throws Exception;

    /**
     *
     *
     * @param gHandler a handler to a grammar
     * @param output
     * @param depth
     * @param xml
     * @param apply
     * @param input
     * @return
     * @exception Exception
     */
    public synchronized int XipParseFileXMLToFile(int gHandler,
            String input, String output, int depth, char xml, char apply) throws Exception {
        return XipParseFileXMLToFileImplementation(gHandler, input, output, depth, xml, apply);
    }
    //------------------------------------------------------------------------

    private native void XipGetListCategoriesImplementation(int gHandler)
            throws Exception;
    /**
     * Return a vector of category name (String)
     *
     * @param gHandler a handler to a grammar
     * @return Return a vector of category name (String)
     * @exception Exception
     */
    Vector<String> categories;

    /**
     *
     * @param gHandler
     * @return
     * @throws Exception
     */
    public synchronized Vector<String> XipGetListCategories(int gHandler) throws Exception {
        categories = new Vector<String>();
        XipGetListCategoriesImplementation(gHandler);
        return categories;
    }

    /**
     * This callback method is called by the C++ method
     * JXipImp::XipGetListCategoriesImplementation()
     *
     * @param category The feature to be added to the Category attribute
     */
    private void addCategoryCB(String category) {
        //System.out.println("JXip::addCategory() category = " + category);
        categories.add(category);
    }
    //------------------------------------------------------------------------

    private native void XipGetListDependenciesImplementation(int gHandler) throws Exception;
    /**
     * Return a vector of dependency name (String)
     *
     * @param gHandler a handler to a grammar
     * @return Return a vector of dependency name (String)
     * @exception Exception
     */
    Vector<String> dependencies;

    /**
     *
     * @param gHandler
     * @return
     * @throws Exception
     */
    public synchronized Vector<String> XipGetListDependencies(int gHandler) throws Exception {
        dependencies = new Vector<String>();
        XipGetListDependenciesImplementation(gHandler);
        return dependencies;
    }

    /**
     *
     * This callback method is called by the C++ method
     * JXipImp::XipGetListDependenciesImplementation()
     *
     * @param dependency The feature to be added to the Dependency attribute
     */
    private void addDependencyCB(String dependency) {
        //System.out.println("JXip::addDependency() dependency = " + dependency);
        dependencies.add(dependency);
    }

    //----------------------------------------------------------------------------------------------------
    private native void InitErrorMsgImplementation();

    /**
     * Reset and clear the error message buffer
     */
    public synchronized void InitErrorMsg() {
        InitErrorMsgImplementation();
    }

    //----------------------------------------------------------------------------------------------------
    private native String XipGetSourceCharsetImplementation(int gHandler) throws Exception;

    /**
     * Return the encoding charset of the source (FST)
     *
     * @param gHandler Grammar handler
     * @return the encoding charset: "UTF8_ENCODING" | "LATIN_ENCODING"
     * @exception Exception
     */
    public synchronized String XipGetSourceCharset(int gHandler) throws Exception {
        return XipGetSourceCharsetImplementation(gHandler);
    }

    //----------------------------------------------------------------------------------------------------
    private native String XipGetGrammarCharsetImplementation(int gHandler) throws Exception;

    /**
     * Return the encoding charset of the grammar source files
     *
     * @param gHandler Grammar handler
     * @return the encoding charset: "UTF8_ENCODING" | "LATIN_ENCODING"
     * @exception Exception
     */
    public synchronized String XipGetGrammarCharset(int gHandler) throws Exception {
        return XipGetGrammarCharsetImplementation(gHandler);
    }

    //----------------------------------------------------------------------------------------------------
    private native String getErrorMsgImplementation();

    /**
     * return the error message of the previous xip function call
     *
     * @return The errorMsg value
     */
    public synchronized String getErrorMsg() {
        return getErrorMsgImplementation();
    }
    //------------------------------------------------------------------------

    private native int xipSetIndentFilePathnameImplementation(int gHandler, String path) throws Exception;

    /**
     * @param gHandler a handler to a grammar
     * @param path Indent File Pathname
     * @return true if ok, false if error
     * @exception Exception
     */
    public synchronized boolean xipSetIndentFilePathname(int gHandler, String path) throws Exception {
        if (xipSetIndentFilePathnameImplementation(gHandler, path) == 0) {
            return true;
        } else {
            return false;
        }
    }
    //------------------------------------------------------------------------

    private native int xipSetWorkingFilesPathImplementation(int gHandler, String path) throws Exception;

    /**
     * define the path for any output file
     *
     * @param gHandler a handler to a grammar
     * @param path the Path for any output file
     * @return true if ok, false if error
     * @exception Exception
     */
    public synchronized boolean xipSetWorkingFilesPath(int gHandler, String path) throws Exception {
        if (xipSetWorkingFilesPathImplementation(gHandler, path) == 0) {
            return true;
        } else {
            return false;
        }
    }
    //------------------------------------------------------------------------

    private native void printSysInfoImplementation();

    /**
     * print system info
     *
     */
    public synchronized void printSysInfo() {
        printSysInfoImplementation();
    }
    //------------------------------------------------------------------------

    /**
     *
     * @param strToParse
     * @param gHandler a handler to a grammar
     * @param jxip
     * @exception Exception
     */
    private void test1(JXip jxip, String strToParse, int gHandler) throws Exception {
        long displayMode = Const.DISPLAY_TREE;
        int col = 80;
        /*
         * jxip.XipSetDisplayMode(gHandler, displayMode, col); if
         * (jxip.XipTestFlagDisplay(gHandler, displayMode)) {
         * System.out.println("the flag is set"); } else {
         * System.out.println("the flag is NOT set"); }
         * jxip.XipRemoveFlagDisplay(gHandler, displayMode); if
         * (jxip.XipTestFlagDisplay(gHandler, displayMode)) {
         * System.out.println("the flag is set"); } else {
         * System.out.println("the flag is NOT set"); }
         * jxip.XipAddFlagDisplay(gHandler, displayMode, col); if
         * (jxip.XipTestFlagDisplay(gHandler, displayMode)) {
         * System.out.println("the flag is set"); } else {
         * System.out.println("the flag is NOT set"); }
         */
        //jxip.XipRemoveFlagDisplay(gHandler, Const.DISPLAY_WORDNUM);
        jxip.XipAddFlagDisplay(gHandler, Const.DISPLAY_CHUNK_TREE, col);
        //String res = jxip.parseString(gHandler, strToParse, Const.XML_NONE, (char) 1);
        String res = jxip.parseStringCharSet(gHandler, strToParse, "utf-8", Const.XML_NONE, (char) 1);
        System.out.println("\nresultat after parsing = " + res);
    }

    /**
     *
     * @param gHandler a handler to a grammar
     * @param jxip
     * @exception Exception
     */
    private void test2(JXip jxip, int gHandler) throws Exception {
        //String input = "\\\\smbhost\\grondeau\\projets\\parSem\\xip\\dev\\GRAMMARS\\SPANISH\\TEST\\basic\\Frio\\x.txt";
        String input = "/home/grondeau/projets/parSem/xip/dev/GRAMMARS/SPANISH/TEST/basic/Frio/x.txt";
        jxip.XipSetDisplayMode(gHandler, Const.UTF8_INPUT, 80);
        int err1 = jxip.XipParseFileToFile(gHandler, input, "output", Const.XML_NONE, (char) 1);
        if (err1 != 0) {
            System.out.println("XipParseFileToFile Error");
        }
    }

    /**
     *
     * @param gHandler a handler to a grammar
     * @param jxip
     * @exception Exception
     */
    private void test3(JXip jxip, int gHandler) throws Exception {
        int err2 = jxip.XipParseFileXMLToFile(gHandler,
                "../GRAMMARS/ENGLISH/TEST/entity/testXml/input.txt", "output", 0, Const.XML_NONE, (char) 1);
        if (err2 != 0) {
            System.out.println("XipParseFileToFile Error");
        }
    }

    /**
     *
     * @param gHandler a handler to a grammar
     * @param jxip
     * @exception Exception
     */
    private void test4(JXip jxip, int gHandler) throws Exception {
        Vector<String> dep = jxip.XipGetListDependencies(gHandler);
        System.out.println("++++++++++ dependencies:  " + dep.toString());
        Vector<String> cat = jxip.XipGetListCategories(gHandler);
        System.out.println("++++++++++ categories:  " + cat.toString());
    }

    private static JXip test_init(boolean netbeans, boolean python) throws Exception {
        JXip jxip;
        if (netbeans) {
            //JXip jxip = new JXip(null); // to be used with netbeans
            String libs[];
            if (DynLoaderFromJar.isWindows()) {
                if (python) {
                    libs = new String[2];
                    String rootLib = "Z:\\users\\grondeau\\parSem\\xip\\dev\\java\\build\\lib\\jXipPy\\windows\\sharedLib";
                    libs[0] = rootLib + "\\jXipPy.dll";
                    libs[1] = rootLib + "\\python25.dll";
                } else {
                    libs = new String[1];
                    String rootLib = "Z:\\users\\grondeau\\parSem\\xip\\dev\\java\\build\\lib\\jXip\\windows\\sharedLib";
                    libs[0] = rootLib + "\\jXip.dll";
                }
            } else if (python) {
                libs = new String[2];
                String rootLib = "/opt/ParSem/users/grondeau/parSem/xip/dev/java/build/lib/jXipPy/linux/sharedLib";
                libs[0] = rootLib + "/libjXipPy.so";
                libs[1] = rootLib + "/libpython.so";
            } else {
                libs = new String[1];
                String rootLib = "/opt/ParSem/users/grondeau/parSem/xip/dev/java/build/lib/jXip/linux/sharedLib";
                libs[0] = rootLib + "/libjXip.so";
            }
            jxip = new JXip(libs, false);
        } else {
            jxip = new JXip();
        }
        System.out.println("\ngetVersion = " + jxip.GetVersion() + "\n");
        return jxip;
    }

    private int test_loadGram(String[] args) throws Exception {
        String gram;
        int gHandler;
        if (args.length != 2) {
            if (DynLoaderFromJar.isWindows()) {
                gram = "Z:\\users\\grondeau\\parSem\\xip\\dev\\GRAMMARS\\ENGLISH\\GRMFILES\\GRAM_GEN\\gram_gen_entit.grm";
            } else {
                gram = "/opt/ParSem/users/grondeau/parSem/xip/dev/GRAMMARS/ENGLISH/GRMFILES/GRAM_GEN/gram_gen_entit.grm";
            }
        } else {
            gram = args[0];
        }
        System.out.println("Loading grammar file: " + gram);
        gHandler = XipGrmFile(gram, 0, true);
        if (gHandler == -1) {
            System.out.println("XipGrmFile call error");
            System.exit(-1);
        }
        System.out.println("XipGetSourceCharset = " + XipGetSourceCharset(gHandler));
        System.out.println("XipGetGrammarCharset = " + XipGetGrammarCharset(gHandler));
        return gHandler;
    }

    private void test_parse(String[] args, JXip jxip, int gHandler) throws Exception {
        String input;
        if (args.length != 2) {
            input = "This is a test.";
        } else {
            input = args[1];
        }
        System.out.println("parsing the string = " + input);
        test1(jxip, input, gHandler);
        //test2(jxip, gHandler);
        //test3(jxip, gHandler);
        //test4(jxip, gHandler);
        XipFreeGrammar(gHandler);
    }

    /**
     *
     *
     * @param args
     */
    public static void main(String[] args) {
        try {
            boolean netbeans = false;
            boolean python = false;
            JXip jxip = test_init(netbeans, python);
            int gHandler = jxip.test_loadGram(args);
            jxip.test_parse(args, jxip, gHandler);
        } catch (Exception ex) {
            //ex.printStackTrace();
            System.exit(-1);
        }
    }
}
