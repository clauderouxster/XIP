/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 ****************************************************************************************
 * Copyright (c) 2007 Xerox Corporation                                                 *
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

import com.xerox.xrce.xipui.exception.BadGrammarHandlerException;
import com.xerox.xrce.xipui.util.FileTools;
import com.xerox.xrce.xipui.util.XML;
import java.util.*;
import org.apache.log4j.*;
import java.io.*;

/**
 *
 * @author vgrassau
 */
public class Grammar {
    // keys to access to the grammar information

    public static final String RULES = "RULES";
    public static final String LEXICONS = "LEXICONS";
    public static final String PARAMETERS = "PARAMETERS";
    public static final String DECLARATIONS = "DECLARATIONS";
    public static final String SCRIPTS = "SCRIPTS";
    public static final String DIVERS = "DIVERS";
    public static final String KEY = "clef";
    public static final String LICENSE = "licence";
    public static final String VERSION = "version";
    public static final String LANGUAGE = "language";
    public static final String GRAMMAR_LANGUAGE = "grammarlanguage";
    public static final String LOCALE = "thelocale";
    public static final String CHARSET = "charset";
    public static final String INDENT = "indentpathname";
    public static final String TRACE = "tracerpathname";
    public static final String MAX_WORDS = "max_words";
    public static final String MAX_SIZE_WORD = "max_size_word";
    public static final String FEATURE_NUMBER = "featurenumber";
    public static final String HMM = "hmm";
    public static final String REENTRANCE = "reentrance";
    public static final String ENCODING_LEX = "encodage_lexique";
    /** boolean to check if grammar is crypted */
    private boolean isCrypted = false;
    /** table for grammar information, like indent pathname....*/
    private Map<String, Object> properties;
    /** the grammar log */
    private Logger log = Logger.getLogger("grammar");
    /** counter of grammar reloading */
    private int cptReload = 0;
    /** the setup file of the grammar */
    private File grammarFile;
    /** the path of the GrmFile */
    private String grmFile;
    /** Numero de la grammaire */
    private int gHandler = -1;
    /** Xip parser */
    private Xip xip;
    /** timestamp for the last loading */
    private long lastLoaded;
    /** boolean to indicate that the grammar was modified */
    private boolean isModified = false;
    /** the grammar encoding stored in a setup xml file */
    private String encodingInXMLConf = null;

    /** Creates a new instance of Grammar
     *@param xip the xip parser in order to use grammar
     */
    public Grammar(Xip xip) {
        this.xip = xip;
        this.grmFile = null;
        this.properties = new Hashtable();
    }

    /**
     * Return if the grammar is crypted. It doesn't mean that all files are crypted.
     * @return true if crypted, false otherwise
     * @see isCryptedFile(File)
     */
    public boolean isCrypted() {
        return this.isCrypted;
    }

    public boolean isLoaded() {
        return (getGhandler() != -1);
    }

    /**
     * This method is usefull, it permits to know some information about the grammar without
     * loading it. But a first loading is mandatory to create this XML document
     * @param xmlConf the document xml that describes grammar
     *
     * @throws java.lang.Exception
     * NB : This method is not called in reload() and loadGrammar(String)
     */
    public void preInitWithXMLConf(File xmlConf) throws Exception {
        try {
            XMLProject xmlP = new XMLProject(new XML(xmlConf));
            String[] typeSpeXML = {XMLProject.DECLARATION_, XMLProject.RULE_, XMLProject.LEXICON_, XMLProject.PARAMETER_, XMLProject.SCRIPT_};
            short[] typeSpe = {Specialisations.DECLARATION, Specialisations.RULE, Specialisations.LEXICON, Specialisations.PARAMETER, Specialisations.NO_TYPE};
            String[] keys = {DECLARATIONS, RULES, LEXICONS, PARAMETERS, SCRIPTS};
            for (int i = 0; i < typeSpeXML.length; i++) {
                Specialisations s = xmlP.readGrammarFiles(typeSpeXML[i]);
                s.setType(typeSpe[i]);
                properties.put(keys[i], s);
            }
            this.grammarFile = new File(xmlP.getValueOf(XMLProject.XPATH_GRAMMAR_CONFIG));
            this.encodingInXMLConf = xmlP.getValueOf(XMLProject.XPATH_GRAMMAR_ENCODING);
            this.isCrypted = Boolean.valueOf(xmlP.getValueOf(XMLProject.XPATH_GRAMMAR_CRYPTED));
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     *Important method. this method inits the grammar table information.
     *This method is called in reload() and loadGrammar(String).
     *the initialisation is done only if the gHandler is different to -1.
     *
     */
    private void initGrammarXip() {
        if (getGhandler() != -1) {
            //the rules
            Vector vr = new Vector();
            // the lexicons
            Vector vl = new Vector();
            //the features (declaration)
            Vector vf = new Vector();
            // the parameters
            Vector vp = new Vector();
            // the scripts
            Vector vs = new Vector();
            // the "divers" files
            Vector vd = new Vector();
            try {
                this.isCrypted = xip.isGrammarCrypted(getGhandler());
                log.trace("Get files path of grammar");
                String file = xip.grammarFiles(getGhandler(), vf, vl, vr, vp, vs, vd);
                properties.put(RULES, new Specialisations(vr.elements(), Specialisations.RULE, isCrypted()));
                properties.put(LEXICONS, new Specialisations(vl.elements(), Specialisations.LEXICON, isCrypted()));
                properties.put(PARAMETERS, new Specialisations(vp.elements(), Specialisations.PARAMETER, false));
                properties.put(DECLARATIONS, new Specialisations(vf.elements(), Specialisations.DECLARATION, isCrypted()));
                properties.put(SCRIPTS, new Specialisations(vs.elements(), Specialisations.SCRIPTS, isCrypted()));
                //properties.put(SCRIPTS,vs);
                properties.put(DIVERS, vd);
                this.grammarFile = new File(file);

                log.trace("Get table of information about the grammar");
                //the licence, language,....
                Hashtable information = xip.grammarInformation(getGhandler());
                properties.putAll(information);
            } catch (Exception e) {
                log.error("can't init the grammar for XipUI", e);
            }
        }
    }

    /**
     *
     * @param grmFile
     * @throws java.lang.Exception
     */
    public void loadGrammar(String grmFile) throws Exception {
        try {
            this.grmFile = grmFile;
            if (getGhandler() == -1) {
                // log.info("load Grammar with : " + grmFile);
                gHandler = xip.loadGrammar(grmFile, 0);
                log.info("load Grammar ok with : " + grmFile + " ghandler : " + gHandler);
                initGrammarXip();
                setLastLoaded();
            } else {
                reload();
            }
        } catch (Exception e) {
            log.error(e.toString());
            throw e;
        }
    }

    /**
     * This method reloads a grammar.
     * if the grammar was not loaded we load it.
     * warning : If grammar failed during reloading then we keep its gHandler
     * 
     * 
     * @see com.xerox.xrce.lingdexip.Xip.reloadGrammar(int,boolean,String)
     * @throws java.lang.Exception if grammar was not correctly written
     * 
     * WARNING dev : this method crashs sometimes with the API
     */
    public void reload() throws Exception {
        try {
            if (getGhandler() == -1) {
                loadGrammar(grmFile);
            } else {
                log.info("reload Grammar with : " + grmFile + "(" + (this.cptReload++) + ")");
                gHandler = xip.reloadGrammar(getGhandler(), true, grmFile);
            }
            initGrammarXip();
            setLastLoaded();
        } catch (BadGrammarHandlerException e) {
            log.error(e.toString());
            throw e;
        } catch (Exception e) {
            log.error(e.toString());
            throw e;
        }
    }

    /**
     * Free a grammar....
     * @throws java.lang.Exception
     * @deprecated
     */
    public void free() throws Exception {
        try {
            if (getGhandler() != -1) {
                xip.freeGrammar(getGhandler());
                gHandler = -1;
            }
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     *return the number of the grammar in the xip environment
     *@return an integer or -1 if grammar was not loaded
     */
    public int getGhandler() {
        return this.gHandler;
    }

    /**
     * Return the grammar encoding. This method is usefull to edit file
     * if the grammar is not loaded... we check in the xml configruation file. If the configuration file
     * doesn't existe or is incorrect we return the ISO-8859-1 encoding.
     * 
     * @return ISO-8859-1(default)  or UTF-8
     * @see com.xerox.xrce.lingdexip.util.FileTools
     * 
     * WARNING for dev : this method crashs sometimes with the API
     * @see com.xerox.xrce.lingdexip.Xip#getGrammarCharset(int)
     */
    public String getRealEncoding() {
        try {
            String encoding = xip.getGrammarCharset(getGhandler());
            if (encoding.equalsIgnoreCase("UTF8_ENCODING")) {
                return FileTools.UTF_8;
            } else if (encoding.equalsIgnoreCase("LATIN_ENCODING")) {
                return FileTools.ISO_8859_1;
            }
            log.warn("Unknown value for grammar encoding : " + encoding);
            //modif par defaut les grammaires sont en ISO
            return FileTools.ISO_8859_1;
        } catch (BadGrammarHandlerException e) {
            // rien
        } catch (Exception e) {
            log.warn("warning ", e);
        }
        if (this.encodingInXMLConf == null) {
            return FileTools.ISO_8859_1;
        } else {
            return encodingInXMLConf;
        }
    }

    /**
     * Return the rules Specialistions
     * @return Specialistions
     */
    public Specialisations getRules() {
        Specialisations s = (Specialisations) this.properties.get(RULES);
        if (s == null) {
            return new Specialisations();
        } else {
            return s;
        }
    }

    /**
     * Return the parameter Specialisations
     * @return Specialisations
     */
    public Specialisations getParameters() {
        Specialisations s = (Specialisations) this.properties.get(PARAMETERS);
        if (s == null) {
            return new Specialisations();
        } else {
            return s;
        }
    }

    /**
     * Checks if the File is crypted or not
     * if grammar is crypted we need ti check if the file is a parameter, because if it is, it could be not crypted.
     * NB : this methode is penalizing if the grammar is crypted.
     * @param f the file to check
     * @return true if crypted false otherwise
     */
    public boolean isCryptedFile(File f) {
        return (isCrypted() && (getParameters().getSpecialisation(f) == null));
    }

    /**
     *
     * @return
     */
    public Specialisations getLexicons() {
        Specialisations s = (Specialisations) this.properties.get(LEXICONS);
        if (s == null) {
            return new Specialisations();
        } else {
            return s;
        }
    }

    /**
     *
     * @return
     */
    public Specialisations getDeclarations() {
        Specialisations s = (Specialisations) this.properties.get(DECLARATIONS);
        if (s == null) {
            return new Specialisations();
        } else {
            return s;
        }
    }

    /**
     *
     * @return return the scripts
     */
    public Specialisations getScripts() {
        Specialisations v = (Specialisations) this.properties.get(SCRIPTS);
        if (v == null) {
            return new Specialisations();
        } else {
            return v;
        }
    }

    /**
     *
     * @return Vector
     */
    public Vector getDivers() {
        Vector v = (Vector) this.properties.get(DIVERS);
        if (v == null) {
            return new Vector();
        } else {
            return v;
        }
    }

    /**
     *Set the last loaded field
     */
    private void setLastLoaded() {
        Date date = new Date();
        this.lastLoaded = date.getTime();
        log.info("Last loaded : " + lastLoaded);
    }

    /**
     *Return the time of the last loaded
     *@return long : 0 if not loaded
     */
    public long getLastLoaded() {
        return this.lastLoaded;
    }

    /**
     *Return the list of declared variables (not environment variable)
     *@return list of GramVariable or an empty Vector if problem
     */
    public Vector getListVariables() {
        try {
            log.info("List of variables gHandler : " + gHandler);
            return xip.getVariables(gHandler);
        } catch (Exception e) {
            log.fatal("error", e);
            return null;
        }
    }

    /** Return the list of declared dependencies
     *@return list of GramDependeny or an empty Vector if problem
     */
    public Vector getListDependencies() {
        try {
            log.info("List of dependencies gHandler : " + gHandler);
            return xip.getDependencies(gHandler);
        } catch (Exception e) {
            log.fatal("error", e);
            return null;
        }
    }

    /**
     *Return the list of declared categories
     *@return return a Vector of GramCategory or an empty Vector if problem
     */
    public Vector getListCategories() {
        try {
            log.info("List of categories gHandler : " + gHandler);
            return xip.getCategories(this.gHandler);
        } catch (Exception e) {
            log.fatal("error", e);
            return null;
        }
    }

    /**
     *Return the feature map of the grammar.
     *@return a Vector or an empty Vector if problem
     */
    public Vector getFeatureMap() {
        try {
            log.info("Get feature Map gHandler : " + gHandler);
            return xip.getFeatureTree(this.gHandler);
        } catch (Exception e) {
            log.fatal("error", e);
            return null;
        }
    }

    /**
     *Return all specialistions of the grammar
     *The files are RULES, PARAMETERS,DECLARATIONS,LEXICONS
     *@return Vector of File and not a Vector of specialisation
     */
    public Vector getAllGrammarFiles() {
        //log.info("List of grammar files gHandler : " + gHandler);
        Vector allFiles = new Vector();
        String[] keys = {RULES, PARAMETERS, DECLARATIONS, LEXICONS};
        for (int i = 0; i < keys.length; i++) {
            Specialisations s = (Specialisations) properties.get(keys[i]);
            Enumeration enumS = s.getElements();
            while (enumS.hasMoreElements()) {
                Specialisation spe = (Specialisation) enumS.nextElement();
                allFiles.add(spe.getFile());
                Date fileDate = new Date(spe.getFile().lastModified());
                Date gDate = new Date(getLastLoaded());
                if (gDate.before(fileDate)) {
                    log.info("Grammar was modified : " + gDate.getTime() + " < " + fileDate.getTime());
                    isModified = true;
                }
            }
        }
        return allFiles;
    }

    /**
     *Check if the grammar was modified
     *We can check only if the grammar was loaded. If a file is more recent than the last grammar loaded then
     *we consider that the grammar was modify.
     *@return true if it is false otherwise
     */
    public boolean isModified() {
        log.info("Check if grammar was modified");
        isModified = false;
        getAllGrammarFiles();
        return isModified;
    }

    /**
     *return the file of the grammar (example : english.xip)
     *@return File
     */
    public File getGrammarFile() {
        return this.grammarFile;
    }

    /**
     *Return the path of the indent file
     *@return null if grammar was not loaded, otherwise we try to get the path from the
     *table of grammar information
     */
    public String getIndentPath() {
        if (getGhandler() != -1) {
            Hashtable h = null;
            try {
                h = xip.grammarInformation(getGhandler());
            } catch (Exception e) {
                return (String) getGrammarInformation(INDENT);
            }
            if (h != null) {
                return (String) h.get(INDENT);
            } else {
                return null;
            }
        }
        return null;
    }

    /**
     *Return an information of the grammar
     *@param name property to knwo
     *@return an object if the property was found null otherwise
     */
    public Object getGrammarInformation(String name) {
        if (getGhandler() != -1) {
            return this.properties.get(name);
        } else {
            return null;
        }
    }
}
