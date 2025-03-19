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

import com.xerox.xrce.xipui.exception.ProjectException;
import java.util.Map;
import java.util.Collection;
import java.io.File;

/**
 *@author Grassaud Vianney
 *
 */
public interface Project {

    // the name of the XML document that represent the project 
    public static final String FILE = "project-config.xml";
    // the value for the Corpus directory 
    public final static String CORPUS_DIRECTORY = "Corpus";
    // The value for the Grammar directory
    public final static String GRAMMAR_DIRECTORY = "Grammar";
    // The value for the default corpus 
    public static final String DEFAULT_CORPUSSET = "Default_corpus";

    /**
     *Return the project context
     *@return the context for this project 
     */
    public ProjectContext getProjectContext();

    /**
     *Set the context for this project, used for some properties, like environment variable
     *@param context The project context
     *@throws ProjectException if the context is not a valid argument for the project
     */
    public void setProjectContext(ProjectContext context) throws ProjectException;

    /**
     *set the RunContext for this project, used for the xip options flag and run options
     *@param context the run context
     *@throws ProjectException if the context is not a valid argument for the project
     */
    public void setRunContext(RunContext context) throws ProjectException;

    /**
     *Return the run context of this project
     *@return the runcontext
     */
    public RunContext getRunContext();

    /**
     *Return the GrmFile object fot the project
     *@return the grm file can't be null
     */
    public Grm getGrmFile();

    /**
     *Return the xip trace execution
     *@return the xip trace in a String
     */
    public String getXipTrace();

    /***
     *Set the grm file for this object 
     *@param grm_ a Grmfile 
     *@throws ProjectException if the given param is invalid
     */
    public void setGrmFile(Grm grm_) throws ProjectException;

    /**
     *Set the grammar of the project
     *@param gr the grammar to use
     *@throws ProjectException if the given param is null e.g.
     */
    public void setGrammar(Grammar gr) throws ProjectException;

    /**
     * Return the grammar of this project
     * @return a grammar object can't be null
     */
    public Grammar getGrammar();

    /**
     *Return the name of the project 
     *@return the name 
     */
    public String getName();

    /**
     *Renvoie the folder where this project is stored
     *@return the absolute path of the folder
     */
    public String getDirectory();

    /**
     *Return the author of this project
     *@return a name.
     */
    public String getAuthor();

    /**
     *Return the version of the project
     *@return a version, no format applied on this number
     */
    public String getVersion();

    /**
     *Return the absolute path of the XML document, that describes this project
     *@return an absolute path
     */
    public String getXmlStore();

    /**
     *Return some comments for this project. 
     *@return comments 
     */
    public String getComment();

    /**
     *Return the language file name of the grammar.
     *This file is used with XIP.
     *e.g. english.xip 
     *@return a file name.
     */
    public String getConfigFileName();

    /**
     *Return the name of the grm file.
     *@return a name file 
     */
    public String getGrmName();

    /**
     *Return the corpusElement corresponding to the given file
     *@param file 
     * @return the first CorpusElemen with the given file
     */
    public CorpusElement getCorpusElement(String file);

    /**
     *Set the name of the project
     *@param name the name for the project
     *@throws ProjectException if name is null
     */
    public void setName(String name) throws ProjectException;

    /**
     *set the folder of the project
     *@param folder the folder for the project
     *@throws ProjectException if folder is null
     */
    public void setDirectory(String folder) throws ProjectException;

    /**
     *Set the version of this project
     *@param version the version for this project
     */
    public void setVersion(String version);

    /**
     *Set the author of this project
     *@param author the author for this project
     */
    public void setAuthor(String author);

    /**
     *Set some comment about this project
     *@param comment the comment to add
     *NB : if the comment is written in an XML document, 
     *     comments don't have forbidden characters for XML like <,>,&... .
     */
    public void setComment(String comment);

    /**
     *Set the table of CorpusSet
     *@param table a table of CorpusSet
     */
    public void setTableCorpusSet(Map<String, CorpusSet> table);

    /**
     *Save the project
     *@throws Exception if problem during the saving
     */
    public void save() throws Exception;

    /**
     * Return the CorpusSet with the given name
     * @param name the name of the CorpusSet to get
     * @return a CorpusSet if found or null otherwise
     */
    public CorpusSet getCorpusSet(String name);

    /**
     *Return an enumeration of all the name of the CorpusSet defined in this project
     *@return an enumeration of String, (key for the hashtable of CorpusSet)
     */
    public Collection<String> getAllCorpusSetName();

    /**
     *Return all the CorpusSet stored.
     *@return a Map representing the set of CorpusSet. 
     *        The entries of the map are the names of the CorpusSet
     */
    public Map<String, CorpusSet> getAllCorpusSet();

    /***
     *Creates the rule map of a grammar.
     *This can be done, only if the grammar was loaded, i.e. the project is loaded
     */
    public void initRuleMap();

    /**
     *Return the rule map of the grammar
     *@return the rule map or null, if the grammar was not loaded or the RuleMap was not set
     */
    public RuleMap getRuleMap();

    /**
     *Return the ghandler of the grammar loaded in the xip parser
     *@return the ghandler, or -1 if the grammar was not loaded
     */
    public int getGhandler();

    /**
     *Return the grammar the absolute path of the grammar folder.
     *The grammar folder is the directory where the language.xip file is stored.
     *@return the aboslute path
     *@see getConfigFileName()
     */
    public String getGrammarFolder();

    /**
     *Load the project
     *@throws Exception if the project could not be loaded
     */
    public void load() throws Exception;

    /**
     *Add the given CorupsSet to this project
     *@param cs the CorpusSet to add
     */
    public void addCorpusSet(CorpusSet cs);

    /**
     *Remove the given CorpusSet from this project
     *@param cs the CorpusSet to remove
     */
    public void removeCorpusSet(CorpusSet cs);

    /**
     *Return the absolute path of the grm file that is used.
     *@return the absolute path
     */
    public String getFullpathGrm();

    /**
     *Reload the project
     *@throws Exception if the project could not be reloaded
     */
    public void reload() throws Exception;

    /**
     *Return the grammar encoding. 
     *By default, the grammar encoding is ISO-8859-1
     *@return the grammar encoding
     */
    public String getGrammarEncoding();

    /**
     *Set the xip parser to use for this project
     *@param xip the xip parser to use
     *@throws ProjectException if xip is not a good Argument
     */
    public void setXip(Xip xip) throws ProjectException;

    /**
     *Return the xip parser that is used
     *@return xip parser
     */
    public Xip getXip();

    /**
     *Return the full name of the project, it's mean the absolute path of the directory that indicates this project
     *@return the absoltue path of the project
     */
    public String getFullName();

    /**
     *Return the corpus directory
     *@return the corpus directory
     */
    public File getCorpusDirectory();

    /**
     *Check if the grammar is loaded
     *@return true if the grammar is loaded, false otherwise
     */
    public boolean isGrammarLoaded();
}
