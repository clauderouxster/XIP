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
import com.xerox.xrce.xipui.util.FileTools;
import com.xerox.xrce.xipui.util.XML;
import java.util.*;
import java.io.*;
import org.dom4j.*;

/**
 *
 *
 * @author Vianney Grassaud
 */
public class XipUIContext extends Context {

    static private final String _version = "0.2.26";
    /** the year for lingdexip */
    static public final String year = "year";
    /** the user mode for the interface  internal or final */
    static public final String user_mode = "user_mode";
    /** the version */
    static public final String version = "version";
    /** the setup file ie the document XML to initilaze the context */
    static public final String setup_file = "setup_file";
    /** the project directory */
    static public final String project_directory = "project_directory";
    /** the grm directory */
    static public final String grm_directory = "grm_directory";
    /** the xip parser to use */
    static public final String xip_parser = "xip_parser";
    /** the size list for opening project used in GUI*/
    private int SIZE = 6;
    /**Vector of OpenedProject  */
    private Vector<ProjectReferenced> projects;
    private boolean initialized = false;

    /**
     * Create the context for lingdexip
     *
     */
    public XipUIContext() {
        super();

        // we create the setup directory :
        File folderBase = new File(System.getProperty("user.home") + "/.lingdexip");
        // make the directory if he doesn't exist
        folderBase.mkdir();
        //we put the property for the setup file
        setProperty(setup_file, folderBase.getAbsolutePath() + "/lingdexip-config.xml");
        setProperty(year, "2007");
        setProperty(version, _version);
        setProperty(project_directory, System.getProperty("user.home") + "/UIProjects");
        setProperty(grm_directory, System.getProperty("user.home") + "/UIProjects");
        projects = new Vector<ProjectReferenced>();
        ProfileManager profileManager = ProfileManager.getExistedInstance();
    }

    /**
     *return the xip parser
     *@return xip if not found return null
     */
    public Xip getXip() {
        try {
            return (Xip) getProperty(xip_parser);
        } catch (Exception e) {
            return null;
        }
    }

    /**
     *Set the xip parser for the lingdexip
     *@param xip not null !!!
     */
    public void setXip(Xip xip) {
        setProperty(xip_parser, xip);
    }

    /**
     *
     *Read the setup file like a document XML and create the context.
     *If some property aren't found in the setup file, we use the default context
     *after a reading and initialization we save the document XML in a stable format
     */
    public boolean initContext() {
        boolean success = false;
        try {
            File setupFile = new File((String) getProperty(setup_file));
            if (setupFile.exists()) {
                XML xml = new XML(setupFile.getAbsolutePath());
                Element root = xml.getDocument().getRootElement();
                Element context = root.element(XMLProject.CONTEXT_);
                if (context != null) {
                    Iterator ite = context.elementIterator(XMLProject.PROPERTY_);
                    while (ite.hasNext()) {
                        Element property = (Element) ite.next();
                        String name = property.attributeValue(XMLProject.NAME_);
                        String value = property.attributeValue(XMLProject.VALUE_);
                        setProperty(name, value);
                    }
                }
                Element listProject = root.element("list-projectOpened");
                String size = listProject.attributeValue("size");
                if (size != null) {
                    SIZE = Integer.parseInt(size);
                }
                Iterator iteP = listProject.elementIterator("project");
                ProfileManager profileManager = ProfileManager.getExistedInstance();
                while (iteP.hasNext()) {
                    Element project = (Element) iteP.next();
                    String path = project.attributeValue("name");
                    String profile = project.attributeValue("profile");
                    ProjectReferenced ref = new ProjectReferenced(path);
                    if (ref.exists() && !projects.contains(ref)) {
                        projects.add(ref);
                        ref.setProfile(profile);
                    }
                }
            } else {
                //we must create the file setup with the default context !!!
                setupFile.createNewFile();
                save();
            }
            setProperty(year, "2006");
            setProperty(version, _version);
            Xip xip = new Xip();
            xip.initErrorMsg();
            setXip(xip);
            this.initialized = true;
            success = true;
        } catch (Error e) {
            log.fatal("FATAL ERROR", e);
            this.initialized = false;
            success = false;
        } catch (Exception e) {
            log.fatal("FATAL ERROR", e);
            this.initialized = false;
            success = false;
        }
        return success;
    }

    public boolean isInitialized() {
        return this.initialized;
    }

    /**
     *Return the xip version used
     *@return the xip version of null if problem or no xip in the context
     */
    String getXipVersion() {
        Xip xip = getXip();
        if (xip != null) {
            return xip.toString();
        } else {
            return null;
        }
    }

    public ProjectReferenced getProjectReferenced(Project p) {
        String name = p.getDirectory() + "/" + p.getName();
        ProjectReferenced op = new ProjectReferenced(name);
        int index = projects.indexOf(op);
        if (index != -1) {
            return projects.get(index);
        }
        return null;
    }

    public void addProjectReferenced(ProjectReferenced reference) {
        projects.remove(reference);
        projects.add(0, reference);
        try {
            save();
        } catch (Exception e) {
            log.warn("ERROR : could not save the context : ", e);
        }
    }

    public void addProject(Project project) {
        String name = project.getDirectory() + "/" + project.getName();
        ProjectReferenced ref = new ProjectReferenced(name);
        ProfileManager pManager = ProfileManager.getExistedInstance();
        ref.setProfile(pManager.getDefaultProfile());
        projects.add(ref);
        try {
            save();
        } catch (Exception e) {
            log.warn("ERROR : could not save the context : ", e);
        }
    }

    /**
     *Return the list of project opened
     *@return list of OpenedProject
     *
     */
    public List getProjects() {
        int size = projects.size();
        if (projects.isEmpty()) {
            return null;
        } else {
            if (SIZE <= size) {
                return this.projects.subList(0, SIZE);
            } else {
                return this.projects.subList(0, size);
            }
        }
    }

    /** Return the Openedproject available
     *@return Vector of OpenedProject
     */
    public Collection<ProjectReferenced> getAllProjects() {
        return projects;
    }

    /**
     *delete a project from an OpenedProject, the project it's also remove on the disk
     *param project project to delete
     *@param project 
     * @return true if it was deleted correctly, false otherwise
     */
    public boolean removeProject(ProjectReferenced project) {
        projects.remove(project);
        return FileTools.deleteFiles(project.getProjectPathFile());
    }

    //    /**
//     *Delete files and directories recursively on the disk
//     *@param projectDirectory the file (or directory) to delete
//     *@return true it was correctly deleted
//     */
//    private boolean  deleteFiles(File projectDirecory){
//        if (projectDirecory.isFile() || projectDirecory.list().length == 0) {
//            projectDirecory.delete();
//            return true;
//        } else {
//            File[] fs = projectDirecory.listFiles();
//            boolean res = false;
//            for(int i=0; i < fs.length; i++) {
//                res = deleteFiles(fs[i]);
//            }
//            projectDirecory.delete();
//            
//            return res;
//        }
//    }
    /**
     *Save the context
     *
     * @throws java.lang.Exception 
     */
    public void save() throws Exception {
        try {
            XML xml = new XML();
            xml.setRoot(XMLProject.ROOT_API_);
            xml.moveCurrent(XMLProject.ROOT_API_);
            xml.addAttribute(XMLProject.VERSION_, getVersion());
            xml.addAttribute(year, (String) getProperty(year));
            xml.addNode(XMLProject.CONTEXT_);
            Enumeration eProperties = keys();
            while (eProperties.hasMoreElements()) {
                String key = (String) eProperties.nextElement();
                xml.add(XMLProject.PROPERTY_);
                xml.addAttribute(XMLProject.NAME_, key);
                xml.addAttribute(XMLProject.VALUE_, getProperty(key).toString());
                xml.parent();
            }
            xml.addNode("list-projectOpened");
            xml.addAttribute("size", Integer.toString(this.SIZE));
            Iterator<ProjectReferenced> ite = projects.iterator();
            while (ite.hasNext()) {
                ProjectReferenced ref = ite.next();
                xml.add("project");
                xml.addAttribute("name", ref.getProjectPath());
                xml.addAttribute("profile", ref.getProfile().toString());
                xml.parent();
            }
            xml.save((String) getProperty(setup_file));
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     *Return the version of lingdexip
     *@return the current version
     */
    public String getVersion() {
        return (String) getProperty(version);
    }

    /**
     *Describe the context, we print only version and xip version
     *@return a short description
     */
    @Override
    public String toString() {
        return getVersion() + " - " + getXipVersion();
    }
}
