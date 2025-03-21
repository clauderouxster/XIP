/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * ProjectContext.java
 *
 * Created on May 5, 2006, 3:00 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui;

import com.xerox.xrce.xipui.util.Context;
import com.xerox.xrce.xipui.util.XML;
import java.util.*;
import java.io.*;
import org.dom4j.*;

/**
 *
 * @author vgrassau
 */
public class ProjectContext extends Context {

    private File setupFile;
    // private Xip xip;
    // key for the default directory
    public static final String default_directory = "default_directory";
    //
    public static final String newFile_directory = "newFile_directory";
    //the working directory for the project
    public static final String working_directory = "working_directory";
    // the directory when there is an adding of CorpusElement
    public static final String corpusElement_directory = "corpusElement_directory";
    // the property to put some variables
    public static final String variables = "variables";

    /**
     * Creates a new instance of ProjectContext
     */
    protected ProjectContext() {
        super();
        setProperty(variables, new Vector());
        String python_home = System.getenv("PYTHONHOME");
        String python_path = System.getenv("PYTHONPATH");
        if (python_home == null) {
            addVariable("PYTHONHOME", "null");
        } else {
            addVariable("PYTHONHOME", python_home);
        }
        if (python_path == null) {
            addVariable("PYTHONPATH", "null");
        } else {
            addVariable("PYTHONPATH", python_path);
        }
    }

    private void defaultInitContext(File setup) {
        this.setupFile = setup;
        setProperty(default_directory, setupFile.getParent());
        setProperty(newFile_directory, setupFile.getParent());
        setProperty(corpusElement_directory, setupFile.getParent());
        addVariable(working_directory, setupFile.getParent());
    }

    public ProjectContext(String directory, String name) {
        this();
        File tmp = new File(directory, name);
        defaultInitContext(new File(tmp, Project.FILE));
    }

    public ProjectContext(File setupFile) {
        this();
        defaultInitContext(setupFile);
    }

    public File getSetupFile() {
        return this.setupFile;
    }

    public void addVariable(String name, String value) {
        VarEnv var = new VarEnv(name, value);
        Vector v = getVariables();
        Iterator ite = v.iterator();
        boolean stop = false;
        int i = 0;
        while (ite.hasNext() && !stop) {
            VarEnv var_ = (VarEnv) ite.next();
            if (var_.getName().equals(var.getName())) {
                stop = true;
                log.trace("set the variable :" + var);
                v.set(i, var);
            }
            i++;
        }
        if (!stop) {
            log.trace("add the variable :" + var);
            v.add(var);
        }
//        if ( var.getValue() != null && var.getName() != null) {
//            try {
//                xip.setEnv(var.getName(),var.getValue());
//            } catch (Exception e) {
//                log.error("ERROR with variable : " + var,e);
//            }
//        }
    }

    public VarEnv getVariable(String name) {
        Vector v = getVariables();
        Iterator ite = v.iterator();
        while (ite.hasNext()) {
            VarEnv var = (VarEnv) ite.next();
            if (var.getName().equals(name)) {
                return var;
            }
        }
        return null;
    }

    public Vector<VarEnv> getVariables() {
        return (Vector) getProperty(variables);
    }

    public boolean initContext() {
        boolean success = false;
        try {
            if (setupFile != null && setupFile.exists()) {
                XML xml = new XML(setupFile.getAbsolutePath());
                Document docXML = xml.getDocument();
                Element root = docXML.getRootElement();
                Element context = root.element(XMLProject.CONTEXT_);
                Iterator ite = context.elementIterator(XMLProject.PROPERTY_);
                while (ite.hasNext()) {
                    Element n = (Element) ite.next();
                    String name = n.attributeValue(XMLProject.NAME_);
                    String value = n.attributeValue(XMLProject.VALUE_);
                    if (name != null && value != null) {
                        setProperty(name, value);
                    }
                }
                Iterator ite2 = context.elementIterator(XMLProject.VARIABLES_);
                if (ite2 != null) {
                    while (ite2.hasNext()) {
                        Element n = (Element) ite2.next();
                        String name = n.attributeValue(XMLProject.NAME_);
                        String value = n.attributeValue(XMLProject.VALUE_);
                        if (name != null && value != null) {
                            addVariable(name, value);
                        }
                    }
                }
            }
            success = true;
        } catch (Exception e) {
            log.fatal("ERROR to init the context of the project", e);
            success = false;
        }
        return success;
    }
}
