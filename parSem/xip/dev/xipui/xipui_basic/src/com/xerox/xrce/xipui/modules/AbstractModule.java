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
package com.xerox.xrce.xipui.modules;

import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.ui.Tracer;
import javax.swing.*;

import org.apache.log4j.*;
import java.io.*;

/**
 *
 * @author vgrassau
 */
public abstract class AbstractModule implements IModule, Serializable {

    private Tracer tracer;
    protected JComponent root;
    protected boolean isClosed = false;
    private ModuleContext context;
    protected Logger log = Logger.getLogger("module");

    /**
     * Creates a new instance of AbstractModule
     */
    public AbstractModule() {
        this.context = new ModuleContext();
        setTracer(null);
    }

    /**
     * Write an error or an exception in the log file and the tracer module. We show an error dialog.
     * @param msg the message to write
     * @param e the exception or the error
     */
    public void severe(String msg, Throwable e) {
        log.fatal(msg, e);
        tracer.severe(msg, e);
    }

    /**
     * Write an info message in the log file and in the tracer module
     * @param msg the message to write
     */
    public void info(String msg) {
        log.info(msg);
        tracer.message(msg);
    }

    /**
     * Write a warning message on the log file and tracer module and show a warning dialog.
     * @param warning The message to write
     */
    public void warning(String warning) {
        log.warn(warning);
        tracer.warning(warning);
    }

    /**
     * 
     * @param msg 
     */
    public void write(String msg) {
        tracer.writeln(msg);
    }

    /**
     * 
     * @param msg 
     */
    public void error(String msg) {
        tracer.error(msg);
    }

    /**
     * 
     * @param context 
     * @throws java.lang.NullPointerException 
     */
    public AbstractModule(ModuleContext context) throws NullPointerException {
        this();
        setModuleContext(context);
        setTracer((Tracer) context.getProperty(module_tracer));
        setRootComponent((JComponent) context.getProperty(module_root));
    }

    /**
     * 
     * @param context 
     * @throws java.lang.NullPointerException 
     */
    public void setModuleContext(ModuleContext context) throws NullPointerException {
        if (context != null) {
            this.context = context;
        } else {
            throw new NullPointerException();
        }
    }

    /**
     * 
     * @param name 
     * @param value 
     */
    public void setProperty(String name, Object value) {
        getModuleContext().setProperty(name, value);

    }

    /**
     * 
     * @param name 
     * @return 
     */
    public Object getProperty(String name) {
        return getModuleContext().getProperty(name);

    }

    public Object removeProperty(String name) {
        return getModuleContext().remove(name);
    }

    /**
     * 
     * @return 
     */
    public ModuleContext getModuleContext() {
        return context;

    }

    /**
     * 
     * @return 
     */
    public Project getProject() {
        return this.context.getProject();
    }

    /**
     * 
     * @param project 
     */
    public void setProject(Project project) {
        this.context.setProject(project);
    }

    /**
     * 
     * @param t 
     */
    public void setTracer(Tracer t) {
        tracer = t;
        if (tracer == null) {
            tracer = new Tracer();
        }
    }

    /**
     * 
     * @param root 
     */
    public void setRootComponent(JComponent root) {
        this.root = root;
    }

    /**
     * 
     * @return 
     */
    public Tracer getTracer() {
        return this.tracer;
    }

    /**
     * 
     * @return 
     */
    public JComponent getRootComponent() {
        return this.root;
    }

    /**
     * 
     * @return 
     */
    public boolean isClosedModule() {
        return this.isClosed;
    }
}//end of the class
