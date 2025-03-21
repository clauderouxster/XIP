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
import com.xerox.xrce.xipui.util.XML;
import java.util.*;
import org.apache.log4j.*;
import java.io.*;

/**
 * Classe permettant de construire un projet.
 *
 * @author vgrassau
 */
public class ProjectFactory {

    public static final String FILE_SEPARATOR = System.getProperty("file.separator");
    public static final String NAME = "NAME";
    public static final String FOLDER = "FOLDER";
    public static final String AUTHOR = "AUTHOR";
    public static final String COMMENT = "COMMENT";
    public static final String VERSION = "VERSION";
    public static final String GRAMMAR = "GRAMMAR";
    public static final String GRM = "GRM";
    public static final String project_context = "project_context";
    private Logger log = Logger.getLogger("project");
    private Map<String, Object> properties;
    private Map<String, CorpusSet> tableCorpusSet;
    private File setupFile = null;
    private ProjectContext projectContext;
    private Xip xip;

    /**
     *  Creates a new instance of ProjectFactory
     *  use the static method getInstance instead
     *
     * @param setupFile the config file to read
     * @param xip the xip parser
     */
    private ProjectFactory(File xmlFile, Xip xip) {
        properties = new Hashtable();
        this.setupFile = xmlFile;
        this.xip = xip;
        projectContext = new ProjectContext(xmlFile);
        projectContext.initContext();
    }

    /**
     *  Creates a new instance of ProjectFactory
     *  use the static method getInstance instead
     *  @param props a table of properties to create a project
     *  @param xip the xip parser
     */
    private ProjectFactory(Map<String, Object> props, Xip xip) {
        properties = props;
        projectContext = (ProjectContext) props.get(project_context);
        this.xip = xip;
    }

    /**
     *Return an instance of a ProjectFactory
     *@param props
     *@param xip
     * @return 
     */
    static public ProjectFactory getInstance(Hashtable props, Xip xip) {
        return new ProjectFactory(props, xip);
    }

    /**
     *Return an instance of a ProjectFactory
     *@param xmlFile 
     * @param xip
     * @return 
     */
    static public ProjectFactory getInstance(File xmlFile, Xip xip) {
        return new ProjectFactory(xmlFile, xip);
    }

    /**
     *This method checks if the project will be created from an xml file or a table of properties
     *@return true is the project will be created from an xml file, false otherwise
     *
     */
    public boolean isXMLDocument() {
        return (setupFile != null);
    }

    /**
     * Return the path of setupFile
     * @return the absolute path of the xml document or null if it's no used
     */
    public String getSetupFilePath() {
        String path = null;
        if (setupFile != null) {
            path = setupFile.getAbsolutePath();
        }
        return path;
    }

    /**
     *Return the setup file
     *@return 
     * @reurn a file, null if no setup file was speficied
     */
    public File getSetupFile() {
        return setupFile;
    }

    /**
     *Creates the project.
     *@return the project created or null if it's impossible to create it corretly
     *if it's the case then the error is writting on the log file
     */
    public Project createProject() {
        Project project = null;
        try {
            project = new DefaultProject(xip, projectContext);
            tableCorpusSet = new Hashtable();
            //set the project, read the XML document and create directories for project
            //if they don't exit
            setupProject(project);
            createDefaultCorpus(project.getCorpusDirectory());
            RunContext rContext = new RunContext(xip, new File(project.getXmlStore()));
            rContext.initContext();
            project.setRunContext(rContext);
            project.save();

        } catch (Exception e) {
            log.fatal("FATAL ERROR", e);
            project = null;
        }
        return project;
    }

    /**
     *Set the project given in argument.
     *uses the properties attribute of the factory.
     *In this method we set the GrmFile and the Grammar objects
     *@param project the project to set
     *@throws ProjectException if error
     */
    private void setupProject(Project project) throws ProjectException, Exception {

        if (isXMLDocument()) {
            read();
        }
        writeProjectDirectories();
        project.setAuthor((String) properties.get(AUTHOR));
        project.setName((String) properties.get(NAME));
        project.setDirectory((String) properties.get(FOLDER));
        project.setComment((String) properties.get(COMMENT));
        project.setVersion((String) properties.get(VERSION));
        project.setGrmFile((Grm) properties.get(GRM));
        project.setGrammar((Grammar) properties.get(GRAMMAR));
        //add the table of corpus
        project.setTableCorpusSet(tableCorpusSet);
    }

    /**
     *Create the default corpus
     *@param tableCorpusSet the table of corpus set
     *@param corpusDirectory the directory for the corpus
     *@throws Exception if error
     *
     */
    private void createDefaultCorpus(File corpusDirectory) throws Exception {
        log.info("Creation of default corpus");
        CorpusSet cs = tableCorpusSet.get(Project.DEFAULT_CORPUSSET);
        if (cs == null) {
            cs = new CorpusSet(corpusDirectory, Project.DEFAULT_CORPUSSET);
        }
        cs.setDefault(true);
        //create the input file
        File input = new File(cs.getPath(), CorpusElement.DEFAULT);
        //try to create a new file if the input doesn't exist
        input.createNewFile();
        CorpusElement ce = new CorpusElement(input);
        ce.setDescription("input text");
        ce.setInput(true);
        cs.addCorpusElement(ce);
        tableCorpusSet.put(Project.DEFAULT_CORPUSSET, cs);
    }

    /**
     *Contruct the directories for a project if
     *the directories doesn't exist
     *@throws Exception if  error, SecurityException if the creation of the directories failed
     */
    private void writeProjectDirectories() throws Exception, SecurityException {
        try {
            /**  creation du repertoire du projet */
            String name_ = (String) properties.get(NAME);
            String folder_ = (String) properties.get(FOLDER);
            File directory = new File(folder_, name_);
            log.info("Creation of the directory : " + directory);
            directory.mkdirs();
            // Creation du repertoire de la grammaire :
            File grmDirectory = new File(directory, Project.GRAMMAR_DIRECTORY);
            grmDirectory.mkdirs();
            //Creation du corpus par defaut :
            File corpusDirectory = new File(directory, Project.CORPUS_DIRECTORY);
            corpusDirectory.mkdirs();
        } catch (SecurityException e) {
            throw e;
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     *Read the xml document, call this method only if the setupFile is not null.
     *Use the isDocumentXMl to check if this method can be well called
     *
     *This method check also if the absolute path fof the xml document is different than the
     *path store in the XML document denoted by FOLDER et NAME tags...
     *if it's the case, these properties are updated.
     *This method create also the Grammar object and the GRM object.
     *@throws Exception if error
     */
    private void read() throws Exception {
        File grmLocation = null;

        log.info("Reading file configuration : " + setupFile.getAbsolutePath());
        try {
            XMLProject xmlP = new XMLProject(new XML(setupFile));
            properties.put(NAME, xmlP.getValueOf(XMLProject.XPATH_NAME));
            properties.put(VERSION, xmlP.getValueOf(XMLProject.XPATH_VERSION));
            properties.put(FOLDER, xmlP.getValueOf(XMLProject.XPATH_FOLDER));
            properties.put(COMMENT, xmlP.getValueOf(XMLProject.XPATH_COMMENT));
            properties.put(AUTHOR, xmlP.getValueOf(XMLProject.XPATH_AUTHOR));
            String directory = (String) properties.get(FOLDER);
            String name = (String) properties.get(NAME);
            File projectDirectory = new File(directory, name);
            //File  parent =  setupFile.getParentFile();
//            //String folder_ = properties.get(FOLDER)  + "/" + properties.get(NAME) ;
//            //folder_ = folder_.replace('\\','/');
//
//
//            boolean hadChanged = false;
//            if ( !parent.equals(projectFullNameInXML)) {
//                hadChanged = true;
//                log.info("Folder has been changed : old " + projectFullNameInXML + " new " + parent);
//                //the new directory :
//                this.properties.put(NAME,parent.getName());
//                this.properties.put(FOLDER,parent.getParent());
//            }
            String grmAttributeValue = xmlP.getValueOf(XMLProject.XPATH_GRAMMAR_GRM);
            grmLocation = new File(grmAttributeValue);
            if (!grmLocation.isAbsolute()) {
                grmLocation = new File(projectDirectory, xmlP.getValueOf(XMLProject.XPATH_GRAMMAR_GRM));
            }
            Grm grm = new Grm(xip, projectContext);
            grm.setGrmFile(grmLocation);
            log.info("Reading grm File : " + grmLocation);
            //  grm.read();
            properties.put(GRM, grm);
            log.info("Creating Grammar object");
            Grammar grammar = new Grammar(xip);
            try {
                grammar.preInitWithXMLConf(setupFile);
            } catch (Exception e) {
                log.error("Could not initialize grammar with XML file", e);
            }
            properties.put(GRAMMAR, grammar);
            File corpusDirectory = new File(projectDirectory, Project.CORPUS_DIRECTORY);
            tableCorpusSet = xmlP.initTableCorpusSet(corpusDirectory);
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     *For test only don't use
     * @param args 
     */
    static public void main(String... args) {
        try {
            Vector v = new Vector();
            String s = null;
            v.add(s);
            System.out.println(v.size() + " : " + v);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}//end of class
