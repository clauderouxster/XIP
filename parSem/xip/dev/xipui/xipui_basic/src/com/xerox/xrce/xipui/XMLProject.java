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

import com.xerox.xrce.xipui.util.XML;

import java.util.*;
import org.apache.log4j.*;
import java.io.*;
import org.dom4j.*;

/**
 *
 *
 * @author vgrassau
 */
public class XMLProject {

    //the XML document
    private XML xmlProject; 
    private Logger log;

    /** Creates a new instance of XMLProject
     * @param documentProject 
     */
    public XMLProject(XML documentProject) {
        log = Logger.getLogger("project");
        xmlProject = documentProject;
    }

    /**
     *Construct the squeleton of the project xml file
     *@return true if the squeleton is ok, false if error
     */
    public boolean construct() {
        boolean success = false;
        try {
            xmlProject.setRoot(ROOT_);
            xmlProject.moveCurrent(XPATH_ROOT);
            xmlProject.addNode(CONTEXT_);
            xmlProject.addNode(RUN_CONTEXT_);
            xmlProject.add(DISPLAY_);
            xmlProject.addNode(NAME_);
            xmlProject.addNode(FOLDER_);
            xmlProject.addNode(AUTHOR_);
            xmlProject.addNode(VERSION_);
            xmlProject.addNode(COMMENT_);
            xmlProject.addNode(GRAMMAR_);
            xmlProject.addAttribute(CONF_FILE_, "null");
            xmlProject.addAttribute(GRM_, "null");
            xmlProject.addAttribute(ENCODING_, "null");
            success = true;
        } catch (Exception e) {
            log.warn(e);
            success = false;
        }
        return success;
    }

    /**
     * Initialise une valeur pour le document XML decrivant le projet
     * @param xpath noeud a initialise
     * @param value nouvelle valuer
     * @return true si l'initialisation s'est bien passee false sinon
     * les noeuds ne sont pas creer s'ils n'existent pas deja.
     */
    public boolean setValueOf(String xpath, String value) {
        boolean success = false;
        try {
            xmlProject.moveCurrent(xpath);
            xmlProject.setText(value);
            success = true;
        } catch (Exception e) {
            log.error("ERROR, can't set a value for " + xpath, e);
            success = false;
        }
        return success;
    }

    /***
     *Rajoute ou met a jour un noeud dans le document XML, le noeud possedant un attribut
     *specifique et une valeur specifique.
     *@param xpath xpath du noeud parent du noeud a modifier ou a creer
     *@param node noeud a rajouter ou a creer
     *@param nodeValue valeur du noeud a rajouter ou a creer
     *@param valAttribute valeur de l'attribut
     *@param attribute attribut du noeud a rajouter ou a modifier
     *@return true si la modification ou l'ajout se passe correctement false sinon.
     *
     */
    public boolean setValueAndAttribute(String xpath, String node, String nodeValue, String valAttribute, String attribute) {
        boolean success = false;
        try {
            String xpath_ = xpath + "/" + node + "[@" + attribute + "='" + valAttribute + "']";
            xmlProject.xpath(xpath_);
            if (xmlProject.getNode() == null) {
                //le nooeud XML n'existe pas on le cree
                xmlProject.moveCurrent(xpath);
                if (nodeValue == null || nodeValue.equals("")) {
                    xmlProject.add(node);
                } else {
                    xmlProject.add(node, nodeValue);
                    xmlProject.addAttribute(attribute, valAttribute);
                }
            } else {
                //le noeud xml existe on le met a jour avec la nouvelle valeur attribut
                xmlProject.moveCurrent(xpath_);
                log.trace("Attribute " + attribute + " = " + valAttribute + " replace by  " + nodeValue);
                xmlProject.setText(nodeValue);
            }
            success = true;
        } catch (Exception e) {
            log.fatal("Can't set value or attribute", e);
            success = false;
        }
        return success;
    }

    /**
     *Add a attribute from a Xpath expression
     *@param xpath expression to execute in XML document
     *@param attribute the attribute name to add
     *@param value the value to use for the attribute
     * @return 
     */
    public boolean setAttribute(String xpath, String attribute, String value) {
        boolean success = false;
        try {
            xmlProject.moveCurrent(xpath);
            if (xmlProject.getCurrent() != null) {
                xmlProject.addAttribute(attribute, value);
                success = true;
            }
        } catch (Exception e) {
            log.fatal("Can't set attribute : " + attribute, e);

        }
        return success;
    }

    /**
     *add a specialistions node to the XML project document
     *@param typeSpecialisation 
     * @param spe the set of Specialisation to add
     *@return true if succeed false otherwise
     */
    public boolean setSpecialisations(String typeSpecialisation, Specialisations spe) {
        boolean success = false;
        try {
            Enumeration enumSpe = spe.getElements();
            while (enumSpe.hasMoreElements()) {
                Specialisation s = (Specialisation) enumSpe.nextElement();
                if (xmlProject.getNode(XPATH_GRAMMAR + "/" + typeSpecialisation + "[@file ='" + s.getAbsolutePath() + "']") == null) {
                    xmlProject.moveCurrent(XPATH_GRAMMAR);
                    xmlProject.add(typeSpecialisation);
                    xmlProject.addAttribute(FILE_, s.getAbsolutePath());
                    xmlProject.addAttribute(CRYPTED_, Boolean.toString(s.isCrypted()));
                    xmlProject.addAttribute(PARAMETER_, Boolean.toString(s.isParameter()));
                }
            }
            success = true;
        } catch (Exception ex) {
            log.error("Can't add the set of specialisation : " + typeSpecialisation, ex);
            success = false;
        }
        return success;
    }

    /**
     *Add the CorpusElement which are contained in the given  corpuSet.
     *@param cs the Corpusset containing the CorpusElement, cs should not be null
     *@return true if succeed false otherwise
     */
    public boolean setCorpusSet(CorpusSet cs) {
        boolean success = false;
        try {
            Iterator<CorpusElement> ite = cs.getElements();
            while (ite.hasNext()) {
                CorpusElement c = ite.next();
                xmlProject.addNode(CORPUS_);
                if (c.isInput()) {
                    xmlProject.addAttribute(FILE_, c.getFile().getName());//modif pour le pointage
                } else {
                    xmlProject.addAttribute(FILE_, c.getFile().getAbsolutePath());//modif pour le pointage
                }
                xmlProject.addAttribute(NAME_, cs.getName());
                xmlProject.addAttribute(ENCODING_, c.getEncoding());
                // the offset for a corpus element
                int start = -1;
                int end = -1;
                if (c.isKeepValueLine()) {
                    start = c.getStartLine();
                    end = c.getEndLine();
                }
                // the first line to read
                xmlProject.addAttribute(START_, String.valueOf(start));
                // the last line to read
                xmlProject.addAttribute(END_, String.valueOf(end));
                // if this CorpusElement have to be parsed the next time
                xmlProject.addAttribute(TOPARSE_, Boolean.toString(c.isToParse()));
                // if this CorpusElement is an XML document
                xmlProject.addAttribute(IS_XML_, Boolean.toString(c.isXML()));
            }
            success = true;
        } catch (Exception e) {
            log.error("Can't add the CorpusSet : " + cs, e);
            success = false;
        }
        return success;
    }

    /**
     *Add the Project Context to the XML document
     * @param context 
     */
    public void setContext(ProjectContext context) {
        try {
            //add properties to the xml document
            Enumeration eProperties = context.keys();
            while (eProperties.hasMoreElements()) {
                String key = (String) eProperties.nextElement();
                if (!key.equals(ProjectContext.variables)) {
                    xmlProject.moveCurrent(XPATH_CONTEXT);
                    xmlProject.add(PROPERTY_);
                    xmlProject.addAttribute(NAME_, key);
                    xmlProject.addAttribute(VALUE_, context.getProperty(key).toString());
                }
            }
            //add variables to the xml document
            Vector v = context.getVariables();
            Iterator ite = v.iterator();
            while (ite.hasNext()) {
                VarEnv var = (VarEnv) ite.next();
                xmlProject.moveCurrent(XPATH_CONTEXT);
                xmlProject.add(VARIABLES_);
                xmlProject.addAttribute(NAME_, var.getName());
                xmlProject.addAttribute(VALUE_, var.getValue());
            }
        } catch (Exception e) {
            log.error("Can't add the ProjectContext " + context.getSetupFile(), e);
        }
    }

    /**
     *Add the RunContext to the XML document
     *@param context the RunContext to add
     */
    public void setRunContext(RunContext context) {
        try {
            xmlProject.moveCurrent(XPATH_RUN_CONTEXT);
            xmlProject.add(PROPERTY_);
            xmlProject.addAttribute(NAME_, RunContext.xml_mode);
            xmlProject.addAttribute(VALUE_, (String) context.getProperty(RunContext.xml_mode));
            xmlProject.parent();
            xmlProject.add(PROPERTY_);
            xmlProject.addAttribute(NAME_, RunContext.hmm_mode);
            xmlProject.addAttribute(VALUE_, (String) context.getProperty(RunContext.hmm_mode));
            xmlProject.parent();
            xmlProject.add(PROPERTY_);
            xmlProject.addAttribute(NAME_, RunContext.ntm_only);
            xmlProject.addAttribute(VALUE_, (String) context.getProperty(RunContext.ntm_only));
            xmlProject.parent();
            xmlProject.add(PROPERTY_);
            xmlProject.addAttribute(NAME_, RunContext.indent_file);
            xmlProject.addAttribute(VALUE_, (String) context.getProperty(RunContext.indent_file));
            xmlProject.parent();
            xmlProject.add(PROPERTY_);
            xmlProject.addAttribute(NAME_, RunContext.single_mode);
            xmlProject.addAttribute(VALUE_, (String) context.getProperty(RunContext.single_mode));
            xmlProject.parent();
            xmlProject.add(PROPERTY_);
            xmlProject.addAttribute(NAME_, RunContext.grammar_limit);
            xmlProject.addAttribute(VALUE_, (String) context.getProperty(RunContext.grammar_limit));
            xmlProject.parent();
            xmlProject.add(PROPERTY_);
            xmlProject.addAttribute(NAME_, RunContext.column);
            xmlProject.addAttribute(VALUE_, (String) context.getProperty(RunContext.column));
            xmlProject.parent();
            Hashtable display = context.getDisplayOptions();
            Enumeration eProperties = display.keys();
            xmlProject.moveCurrent(XPATH_DISPLAY);
            while (eProperties.hasMoreElements()) {
                Long key = (Long) eProperties.nextElement();
                xmlProject.add(PROPERTY_);
                xmlProject.addAttribute(NAME_, key.toString());
                xmlProject.addAttribute(VALUE_, display.get(key).toString());
                xmlProject.parent();
            }
        } catch (Exception e) {
            log.error("Can't add the RunContext", e);
        }
    }

    /**
     *Return the value from an xpath expression
     *@param xpath 
     * @return the String value or null if error was cautgh or xpath has not found
     *
     */
    public String getValueOf(String xpath) {
        String value = null;
        try {
            xmlProject.xpath(xpath);
            value = xmlProject.getNode();
        } catch (Exception e) {
            log.warn("Can't get the value for " + xpath, e);

        }
        return value;
    }

    /**
     *Create a Specific Specialisations from the XML document
     *@param type 
     * @return the specific Specialisations
     *@throws Exception si exception capturee
     */
    public Specialisations readGrammarFiles(String type) throws Exception {
        Specialisations spe = new Specialisations();
        try {
            log.info("Reading " + type);
            String xpath = XPATH_GRAMMAR + "/" + type;
            xmlProject.xpathNodes(xpath);
            Vector v = xmlProject.getNodeList();
            Enumeration e = v.elements();
            while (e.hasMoreElements()) {
                Node n = (Node) e.nextElement();
                Specialisation s = new Specialisation(XML.getValueAttribute(n, FILE_));
                s.setParameter(XML.getValueAttribute(n, PARAMETER_));
                //for crypted attribute :
                // Boolean return true only if the string equals ignore case to "true"
                boolean crypted = Boolean.valueOf(XML.getValueAttribute(n, CRYPTED_));
                s.setCrypted(crypted);
                if (!spe.contains(s)) {
                    spe.addSpecialisation(s);
                }
            }
            return spe;
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     *inialise la table des corpusSet a partir du document xml du projet
     *@param corpusDirectory 
     * @return 
     * @throws Exception
     *
     */
    public Map<String, CorpusSet> initTableCorpusSet(File corpusDirectory) throws Exception {
        Map<String, CorpusSet> tableCorpusSet = new Hashtable();
        try {
            String xpath = XPATH_CORPUS;
            xmlProject.xpathNodes(xpath);
            Vector<Node> v = xmlProject.getNodeList();
            for (Iterator<Node> it = v.iterator(); it.hasNext();) {
                Node n = it.next();
                String corpusName = XML.getValueAttribute(n, NAME_);
                if (!tableCorpusSet.containsKey(corpusName)) {
                    CorpusSet cs = createCorpusSet(corpusDirectory, corpusName);
                    tableCorpusSet.put(corpusName, cs);
                }
            }
            return tableCorpusSet;
        } catch (Exception e) {
            throw e;
        }
    }

    public CorpusSet createCorpusSet(File corpusDir, String corpusName) throws Exception {
        try {
            CorpusSet cs = new CorpusSet(corpusDir, corpusName);
            //xpath for xml document
            String xpath = XPATH_CORPUS + "[@name = '" + corpusName + "']";
            xmlProject.xpathNodes(xpath);
            Vector v = xmlProject.getNodeList();
            Iterator ite = v.iterator();
            while (ite.hasNext()) {
                Node n = (Node) ite.next();
                String corpusFile = XML.getValueAttribute(n, FILE_);
                String encoding = XML.getValueAttribute(n, ENCODING_);
                String toParse = XML.getValueAttribute(n, TOPARSE_);
                String isXML = XML.getValueAttribute(n, IS_XML_);
                String start = XML.getValueAttribute(n, START_);
                String end = XML.getValueAttribute(n, END_);
                File corpusText = new File(corpusFile.replace('\\', '/'));
                if (!corpusText.isAbsolute()) {
                    corpusText = new File(cs.getPath() + "/" + corpusFile.replace('\\', '/'));
                }
                CorpusElement ce = new CorpusElement(corpusText);
                ce.setParent(cs.getName());
                if (encoding != null) {
                    ce.setEncoding(encoding);
                }
                if (toParse != null) {
                    // System.out.println("to parse : " + Boolean.getBoolean(toParse));
                    ce.setToParse(Boolean.parseBoolean(toParse));
                }
                if (isXML != null) {
                    ce.setXML(Boolean.parseBoolean(isXML));
                }
                if (start != null) {
                    ce.setStartLine(Integer.valueOf(start));
                }
                if (end != null) {
                    ce.setEndLine(Integer.valueOf(end));
                }
                log.trace("Loading of " + corpusFile + " for " + cs.getName());
                cs.addCorpusElement(ce);
            }
            return cs;
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     *Sauvegarde du document sur le disque.
     * @param where
     * @param file
     * @return 
     */
    public boolean save(String where, String file) {
        try {
            String loc = where + "/" + file;
            xmlProject.save(loc.replace('\\', '/'));
            return true;
        } catch (Exception ex) {
            log.fatal("XMLProject::ERROR", ex);
            return false;
        }
    }
    // A
    public static final String AUTHOR_ = "author";
    // C
    public static final String COMMENT_ = "comment";
    public static final String CONF_FILE_ = "configuration_file";
    public static final String CONTEXT_ = "context";
    public static final String CORPUS_ = "corpus";
    public static final String CRYPTED_ = "crypted";
    // D
    public static final String DECLARATION_ = "declaration";
    public static final String DESCRIPTION_ = "description";
    public static final String DISPLAY_ = "display";
    // E
    public static final String ENCODING_ = "encoding";
    public static final String END_ = "end";
    // F
    public static final String FEATURES_ = "features";
    public static final String FEATURE_ = "feature";
    public static final String FILE_ = "file";
    public static final String FOLDER_ = "folder";
    // G
    public static final String GRAMMAR_ = "grammar";
    public static final String GRM_ = "grm";
    // H
    public static final String HMM_ = "hmm";
    // I
    public static final String ID_ = "id";
    public static final String INDEX_ = "index";
    public static final String IS_XML_ = "isXML";
    // L
    public static final String LEXICON_ = "lexicon";
    // N
    public static final String NAME_ = "name";
    public static final String NTM_ = "ntm";
    // O
    public static final String OPENED_ = "opened";
    // P
    public static final String PARAMETER_ = "parameter";
    public static final String PATH_ = "path";
    public static final String PROPERTY_ = "property";
    // R
    public static final String ROOT_ = "project";
    public static final String ROOT_API_ = "lingdexip";
    public static final String RULE_ = "rule";
    public static final String RUN_CONTEXT_ = "run_context";
    public static final String RUNNING_ = "running";
    // S
    public static final String SCRIPT_ = "script";
    public static final String START_ = "start";
    // T
    public static final String TYPE_ = "type";
    public static final String TOPARSE_ = "toParse";
    // V
    public static final String VALUE_ = "value";
    public static final String VARIABLES_ = "variable";
    public static final String VERSION_ = "version";
    // X
    public static final String XIP_VERSION_ = "xipVersion";
    // les xpath pour acceder aux noeud du document XML representant un projet
    static public final String XPATH_ROOT = "/" + ROOT_;
    static public final String XPATH_PROJECT_TYPE = XPATH_ROOT + "/@" + TYPE_;
    static public final String XPATH_CONTEXT_PROPERTY = XPATH_ROOT + "/" + CONTEXT_ + "/" + PROPERTY_;
    static public final String XPATH_CONTEXT = XPATH_ROOT + "/" + CONTEXT_;
    static public final String XPATH_RUN_CONTEXT = XPATH_ROOT + "/" + RUN_CONTEXT_;
    static public final String XPATH_DISPLAY = XPATH_RUN_CONTEXT + "/" + DISPLAY_;
    static public final String XPATH_GRAMMAR = XPATH_ROOT + "/" + GRAMMAR_;
    static public final String XPATH_GRAMMAR_GRM = XPATH_GRAMMAR + "/@" + GRM_;
    static public final String XPATH_GRAMMAR_CONFIG = XPATH_GRAMMAR + "/@" + CONF_FILE_;
    static public final String XPATH_GRAMMAR_ENCODING = XPATH_GRAMMAR + "/@" + ENCODING_;
    static public final String XPATH_GRAMMAR_CRYPTED = XPATH_GRAMMAR + "/@" + CRYPTED_;
    static public final String XPATH_GRAMMAR_HMM = XPATH_GRAMMAR + "/" + HMM_ + "/@" + FILE_;
    static public final String XPATH_GRAMMAR_NTM = XPATH_GRAMMAR + "/" + NTM_ + "/@" + FILE_;
    static public final String XPATH_GRAMMAR_PATH = XPATH_GRAMMAR + "/" + PATH_;
    static public final String XPATH_GRAMMAR_RULE = XPATH_GRAMMAR + "/" + RULE_;
    static public final String XPATH_GRAMMAR_LEXICON = XPATH_GRAMMAR + "/" + LEXICON_;
    static public final String XPATH_GRAMMAR_DECLARATION = XPATH_GRAMMAR + "/" + DECLARATION_;
    static public final String XPATH_GRAMMAR_SCRIPT = XPATH_GRAMMAR + "/" + SCRIPT_;
    static public final String XPATH_GRAMMAR_PARAMETER = XPATH_GRAMMAR + "/" + PARAMETER_;
    static public final String XPATH_CORPUS = XPATH_ROOT + "/" + CORPUS_;
    static public final String XPATH_FOLDER = XPATH_ROOT + "/" + FOLDER_;
    static public final String XPATH_AUTHOR = XPATH_ROOT + "/" + AUTHOR_;
    static public final String XPATH_NAME = XPATH_ROOT + "/" + NAME_;
    static public final String XPATH_VERSION = XPATH_ROOT + "/" + VERSION_;
    static public final String XPATH_COMMENT = XPATH_ROOT + "/" + COMMENT_;
    static public final String XPATH_FEATURES = "/" + FEATURES_;
    static public final String XPATH_FEATURE = XPATH_FEATURES + "/" + FEATURE_;
}
