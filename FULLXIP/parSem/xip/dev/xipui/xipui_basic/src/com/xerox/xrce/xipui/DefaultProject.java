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

import com.xerox.xrce.xipui.util.XML;
import com.xerox.xrce.xipui.exception.*;
import java.util.*;
import org.apache.log4j.*;
import java.io.File;

/**
 * This class implements the interface project
 * This implementation it's a first version of that a Porject can be.
 * @version 1.1 : modification from the new version of the interface Project
 *
 * @author vgrassau
 */
public class DefaultProject implements Project {

    /**
     *The grammar of the project
     */
    protected Grammar grammar;
    /**
     *The GrmFile describing the grammar
     */
    protected Grm grm;
    /**
     *The table of CorpusSet
     *the entries are the names of the CorpusSet, the values are the CorpusSet
     */
    protected Map<String, CorpusSet> tableCorpusSet;
    /**
     * The Xip parser
     */
    protected Xip xip;
    /**
     *The runcontext for the project
     */
    protected RunContext runContext;
    /**
     *The project context
     */
    protected ProjectContext projectContext;
    /**
     *The ruleMap of the grammar
     *@todo : move the rule map to the Grammar object
     */
    protected RuleMap ruleMap;
    protected String nameProject;
    protected File folderProject;
    protected String versionProject;
    protected String commentProject;
    protected String authorProject;
    //protected String typeProject;
    private Logger log;

    /**
     * This class should be used internally by a ProjectFactory.
     * Creates a Default Project
     * @param xip The xip Parser
     * @param context  
     * @throws ProjectException
     */
    public DefaultProject(Xip xip, ProjectContext context) throws ProjectException {
        log = Logger.getLogger("project");
        setXip(xip);
        setProjectContext(context);
        tableCorpusSet = new Hashtable<String, CorpusSet>();
    //   setUp();
    }

    /**
     *Creates a GrmFile Object and a Grammar Object with Xip and ProjectContext
     */
    private void setUp() throws ProjectException {
        setGrmFile(new Grm(getXip(), getProjectContext()));
        setGrammar(new Grammar(getXip()));
    }

    /**
     *Set the runContext
     *
     * @param r
     * @throws com.xerox.xrce.xipui.exception.ProjectException 
     */
    public void setRunContext(RunContext r) throws ProjectException {
        if (r == null) {
            ProjectException exception = new ProjectException("Runcontext is invalid", new NullPointerException());
            exception.setPropertyFailed(com.xerox.xrce.xipui.RunContext.class);
            throw exception;
        } else {
            runContext = r;
        }
    }

    /**
     *@see Project#getRunContext()
     */
    public RunContext getRunContext() {
        return runContext;
    }

    /**
     *@param pc 
     * @throws com.xerox.xrce.xipui.exception.ProjectException 
     * @see Project#setProjectContext(ProjectContext)
     *if pc is null a ProjectException is thrown
     *
     */
    public void setProjectContext(ProjectContext pc) throws ProjectException {
        if (pc == null) {
            ProjectException exception = new ProjectException("Project context is invalid", new NullPointerException());
            exception.setPropertyFailed(com.xerox.xrce.xipui.ProjectContext.class);
            throw exception;
        } else {
            projectContext = pc;
        }
    }

    /**
     *@see Project#getProjectContext()
     */
    public ProjectContext getProjectContext() {
        return projectContext;
    }

    /**
     *Renvoie l'objet permettant la manipulation du fichier grm
     *@return l'objet GrmFile contenant la description du fichier grm et les variables d'envirronement.
     */
    public Grm getGrmFile() {
        //        if ( grm.getFile().lastModified() == grm.lastModified())
//            return this.grm;
//        else {
//            grm.reset();
//            grm.read();
//            return grm;
//        }
//
        return grm;
    }

    /**
     *@param file 
     * @see Project#getCorpusElement(String)
     */
    public CorpusElement getCorpusElement(String file) {
        Collection<String> listOfNames = getAllCorpusSetName();
        CorpusElement toFind = null;
        for (Iterator<String> it = listOfNames.iterator(); it.hasNext() && (toFind == null);) {
            CorpusSet cs = getCorpusSet(it.next());
            CorpusElement ce = cs.getCorpusElement(file);
            if (ce != null) {
                toFind = ce;
            }
        }
        return toFind;
    }

    /**
     *@see Project#getGrammarEncoding()
     */
    public String getGrammarEncoding() {
        return grammar.getRealEncoding();
    }

    /***
     *@throws com.xerox.xrce.xipui.exception.ProjectException 
     * @see Project#setGrmFile(GrmFile)
     *if grm_ is null a ProjectException is thrown
     */
    public void setGrmFile(Grm grm_) throws ProjectException {
        if (grm_ == null) {
            ProjectException exception = new ProjectException("GrmFile is invalid", new NullPointerException());
            exception.setPropertyFailed(com.xerox.xrce.xipui.Grm.class);
            throw exception;
        } else {
            grm = grm_;
        }
    }

    /**
     *@throws com.xerox.xrce.xipui.exception.ProjectException 
     * @see Project#setGrammar(Grammar)
     *If gr is null, a ProjectException is thrown
     */
    public void setGrammar(Grammar gr) throws ProjectException {
        if (gr == null) {
            ProjectException exception = new ProjectException("Grammar is invalid", new NullPointerException());
            exception.setPropertyFailed(com.xerox.xrce.xipui.Grammar.class);
            throw exception;
        } else {
            grammar = gr;
        }
    }

    /**
     *@see Project#getGrammar()
     */
    public Grammar getGrammar() {
        return this.grammar;
    }

    /**
     *@see Project#getName()
     */
    public String getName() {
        return this.nameProject;
    }

    /**
     *
     *
     * @see Project#getDirectory()
     */
    public String getDirectory() {
        return this.folderProject.getAbsolutePath();
    }

    /**
     *Renvoie le nom de l'auteur decrit dans le document XML.
     *@return renvoie le nom de l'auteur ou null si probleme
     */
    public String getAuthor() {
        return this.authorProject;
    }

    /**
     *Renvoie la version du projet
     *@return renvoie la version du projet ou null si probleme
     */
    public String getVersion() {
        return this.versionProject;
    }

    /**
     *Renvoie le lien vers le document XML
     *@return le path du fichier XML
     */
    public String getXmlStore() {
        return getFullName() + "/" + DefaultProject.FILE;
    }

    /**
     *Renvoie les commentaires du projet
     *@return renvoie les commentaires du projets ou vide si probleme ou non present
     */
    public String getComment() {
        return this.commentProject;
    }

    /**Renvoie le nom du fichier de configuration de la grammaire. On ne renvoie pas l'extension du fichier.
     *@return renvoie la langue de la grammaire ou vide si probleme ou non present
     */
    public String getConfigFileName() {
        File f = this.grammar.getGrammarFile();
        if (f != null) {
            return f.getName();
        } else {
            return null;
        }
    }

    /**
     *Renvoie le nom du fichier grm du projet
     *@return renvoie le nom du fichier grm
     *@see Grm#getName()
     */
    public String getGrmName() {
        return grm.getName();
    }

    /**
     *@throws com.xerox.xrce.xipui.exception.ProjectException 
     * @see Project#setName(String)
     */
    public void setName(String name) throws ProjectException {
        if (name != null) {
            this.nameProject = name;
        } else {
            ProjectException exception = new ProjectException("The name is invalid", new NullPointerException());
            exception.setPropertyFailed(java.lang.String.class);
            throw exception;
        }
    }

    /**
     *@param directory 
     * @throws com.xerox.xrce.xipui.exception.ProjectException 
     * @see Project#setFolder(String)
     *if directory is null of a File cannot be instanciated with the given String, a ProjectException is thrown
     */
    public void setDirectory(String directory) throws ProjectException {
        Throwable error = null;
        File f_temp = null;
        if (directory == null) {
            error = new NullPointerException();
        } else {
            try {
                f_temp = new File(directory.replace('\\', '/'));
                f_temp.exists();
            } catch (Exception e) {
                error = e;
            }
        }
        if (error == null && f_temp != null) {
            folderProject = f_temp;
        } else {
            ProjectException exception = new ProjectException("The directory is invalid", error);
            exception.setPropertyFailed(java.io.File.class);
            throw exception;
        }
    }

    /**
     *@see Project#setVersion(String)
     */
    public void setVersion(String version) {
        this.versionProject = version;
    }

    /**
     *@see Project#setAuthor(String)
     */
    public void setAuthor(String author) {
        this.authorProject = author;
    }

    /**
     *@see Project#setComment(String)
     */
    public void setComment(String comment) {
        this.commentProject = comment;
    }

    /**
     *@see Project#setTableCorpusSet(Map<String,CorpusSet>)
     */
    public void setTableCorpusSet(Map<String, CorpusSet> table) {
        this.tableCorpusSet = table;
    }

    /**
     *@see Project#getGrammarFolder()
     *@see Grammar#getGrammarFile()
     */
    public String getGrammarFolder() {
        return grammar.getGrammarFile().getParent();
    }

    /**
     *Save a project, store information about it like an XML document
     *@throws java.lang.Exception 
     * @see Project#save()
     */
    public void save() throws Exception {
        try {
            XMLProject document = new XMLProject(new XML());
            if (document.construct()) {
                document.setValueOf(XMLProject.XPATH_NAME, getName());
                document.setValueOf(XMLProject.XPATH_FOLDER, getDirectory());
                document.setValueOf(XMLProject.XPATH_VERSION, getVersion());
                document.setValueOf(XMLProject.XPATH_AUTHOR, getAuthor());
                document.setValueOf(XMLProject.XPATH_COMMENT, getComment());
                document.setAttribute(XMLProject.XPATH_GRAMMAR, XMLProject.GRM_, getFullpathGrm());
                document.setAttribute(XMLProject.XPATH_GRAMMAR, XMLProject.CONF_FILE_, grammar.getGrammarFile().getAbsolutePath());
                document.setAttribute(XMLProject.XPATH_GRAMMAR, XMLProject.ENCODING_, grammar.getRealEncoding());
                document.setAttribute(XMLProject.XPATH_GRAMMAR, XMLProject.CRYPTED_, Boolean.toString(grammar.isCrypted()));
                //ajout des specialisations de la grammaire.
                document.setSpecialisations(XMLProject.DECLARATION_, grammar.getDeclarations());
                document.setSpecialisations(XMLProject.LEXICON_, grammar.getLexicons());
                document.setSpecialisations(XMLProject.RULE_, grammar.getRules());
                document.setSpecialisations(XMLProject.PARAMETER_, grammar.getParameters());
                document.setSpecialisations(XMLProject.SCRIPT_, grammar.getScripts());
                //add the CorpusSet
                Collection<String> listOfNames = getAllCorpusSetName();
                for (Iterator<String> it = listOfNames.iterator(); it.hasNext();) {
                    CorpusSet cs = getCorpusSet(it.next());
                    document.setCorpusSet(cs);
                }
                document.setContext(getProjectContext());
                document.setRunContext(getRunContext());
                document.save(getFullName(), DefaultProject.FILE);
            }
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     * @see Project#getCorpusSet(String)
     */
    public CorpusSet getCorpusSet(String name) {
        return tableCorpusSet.get(name);
    }

    /**
     *@see Project#getAllCorpusSet()
     */
    public Map<String, CorpusSet> getAllCorpusSet() {
        return tableCorpusSet;
    }

    /**
     *@see Project#getAllCorpusSetName()
     */
    public Collection<String> getAllCorpusSetName() {
        Collection listOfNames = null;
        if (tableCorpusSet != null) {
            listOfNames = tableCorpusSet.keySet();
        }
        return listOfNames;
    }

    /**
     *@see Project#getXipTrace()
     */
    public String getXipTrace() {
        return xip.getErrorMsg();
    }

    /**
     *@see Project#isGrammarLoaded()
     *@see Grammar#isLoaded()
     */
    public boolean isGrammarLoaded() {
        return grammar.isLoaded();
    }

    /**
     *@see Project#initRuleMap()
     *@see RuleMap#init(int)
     */
    public void initRuleMap() {
        if (isGrammarLoaded()) {
            ruleMap = new RuleMap();
            ruleMap.init(getGhandler());
        }
    }

    /**
     *@see Project#getRuleMap()
     */
    public RuleMap getRuleMap() {
        return this.ruleMap;
    }

    /**
     *@see Project#getGhandler()
     *@see Grammar#getGhandler()
     */
    public int getGhandler() {
        return grammar.getGhandler();
    }

    /**
     *set the working directory for this project
     *
     */
    private void setWorkingDirectory() {
        VarEnv path = projectContext.getVariable(ProjectContext.working_directory);
        if (isGrammarLoaded() && path != null) {
            xip.setWorkingPath(getGhandler(), path.getValue());
        }
    }

    /**
     *@see Project#load()
     *@throws java.lang.Exception  
    @see Grammar#loadGrammar(String)
     */
    public void load() throws Exception {
        grammar.loadGrammar(getFullpathGrm());
        doAfterGrammarLoaded();
    }

    /**
     *do some needed instrucion after that the grammar was loaded
     *@throws Exception if error
     */
    private void doAfterGrammarLoaded() throws Exception {
        runContext.setGHandler(getGhandler());
        if (isGrammarLoaded()) {
            if (ruleMap != null) {
                this.ruleMap.init(getGhandler());
            }
            setWorkingDirectory();
            save();
        }
    }

    /**
     *@see Project#addCorpusSet(CorpusSet)
     */
    public void addCorpusSet(CorpusSet cs) {
        log.info("Add a new corpus named " + cs.getName());
        tableCorpusSet.put(cs.getName(), cs);
    }

    /**
     *@see Project#removeCorpusSet(CorpusSet)
     *
     */
    public void removeCorpusSet(CorpusSet cs) {
        tableCorpusSet.remove(cs.getName());
    }

    /**
     *
     *
     * @see Project#getFullPathGrm()
     * @see Grm#getAbsolutePath()
     */
    public String getFullpathGrm() {
        return grm.getAbsolutePath();
    }

    /**
     *@see Project#reload()
     *@see Project#load()
     *@throws java.lang.Exception 
     * @see Grammar#reload()
     */
    public void reload() throws Exception {
        grammar.reload();
        doAfterGrammarLoaded();
    }

    /**
     *@throws com.xerox.xrce.xipui.exception.ProjectException  
    @see Project#setXip(Xip)
     *
     *if xip is null a ProjectException is thrown
     */
    public void setXip(Xip xip) throws ProjectException {
        if (xip == null) {
            ProjectException exception = new ProjectException("Invalid Xip Parser", new NullPointerException());
            exception.setPropertyFailed(com.xerox.xrce.xipui.Xip.class);
            throw (exception);
        } else {
            this.xip = xip;
        }
    }

    /**
     *
     *@return xip
     */
    public Xip getXip() {
        return xip;
    }

    /**
     *
     *@return the full path of the project
     */
    public String getFullName() {
        File f = new File(getDirectory(), getName());
        return f.getAbsolutePath();
    }

    /**
     *A project is equal to another object if the object
     *is equal to the getFullName method.
     *@param o  
     *@return true if equal
     */
    public boolean equals(Object o) {
        boolean evaluation = false;
        if (o instanceof Project) {
            Project p = (Project) o;
            evaluation = p.getFullName().equals(getFullName());
        } else {
            evaluation = getFullName().equals(o);
        }
        return evaluation;
    }

    /**
     *@see Project#getCorpusDirectory()
     *@see Project#getFullName()
     */
    public File getCorpusDirectory() {
        File result = null;
        try {
            File dir = new File(getFullName(), CORPUS_DIRECTORY);
            dir.mkdirs();
            result = dir.getCanonicalFile();
        } catch (Exception e) {
            log.error("ERROR", e);
            result = null;
        }
        return result;
    }
}

