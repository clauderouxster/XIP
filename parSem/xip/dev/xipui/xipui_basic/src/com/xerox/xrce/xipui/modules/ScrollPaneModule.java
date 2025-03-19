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

import java.io.*;
import org.apache.log4j.*;

/**
 *
 * @author vgrassau
 */
public abstract class ScrollPaneModule extends JScrollPane implements Serializable, IModule {

    private Tracer tracer;
    protected JComponent root;
    protected boolean isClosed = false;
    private ModuleContext context;
    protected Logger log = Logger.getLogger("module");

    /**
     * Creates a new instance of ScrollPanelModule
     */
    public ScrollPaneModule() {
        this.context = new ModuleContext();
        setTracer(null);
    }

    /**
     * Creates a new instances of a ScrollPanelModule with a given context
     * @param context
     * @throws java.lang.NullPointerException
     */
    public ScrollPaneModule(ModuleContext context) throws NullPointerException {
        this();
        setModuleContext(context);
        setTracer((Tracer) getProperty(module_tracer));
        setRootComponent((JComponent) getProperty(module_root));
    }

    /**
     * @param e 
     * @see IModule#severe(String,Throwable)
     */
    public void severe(String msg, Throwable e) {
        log.fatal(msg, e);
        tracer.severe(msg, e);
    }

    public Object removeProperty(String name) {
        return getModuleContext().remove(name);
    }

    /**
     *
     *@param msg 
     * @see IModule#info(String)
     */
    public void info(String msg) {
        log.info(msg);
        tracer.message(msg);
    }

    /**
     *
     *@see IModule#warning(String)
     */
    public void warning(String warning) {
        log.warn(warning);
        tracer.warning(warning);
    }

    /**
     *
     *@see IModule#write(String)
     */
    public void write(String msg) {
        tracer.writeln(msg);
    }

    /**
     *@see IModule#error(String)
     */
    public void error(String msg) {
        tracer.error(msg);
    }

    /**
     *Returns a String representation of this object
     *@return the name of this object
     */
    public String toString() {
        return this.getName();
    }

    /**
     * Sets the module context for this object
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

    /**
     *
     * @return
     */
    public ModuleContext getModuleContext() {
        return context;
    }

    /**
     *
     *@see IModule#getProject()
     */
    public Project getProject() {
        return this.context.getProject();
    }

    /**
     *
     * @see IModule#setProject(Project)
     */
    public void setProject(Project project) {
        this.context.setProject(project);
    }

    /**
     * @see IModule#setTracer(Tracer)
     */
    public void setTracer(Tracer t) {
        tracer = t;
        if (tracer == null) {
            tracer = new Tracer();
        }
    }

    /**
     *@see IModule#setRootComponent(JComponent)
     */
    public void setRootComponent(JComponent root) {
        this.root = root;
    }

    /**
     *@see IModule#getTracer()
     * 
     */
    public Tracer getTracer() {
        return this.tracer;
    }

    /**
     *
     *@see IModule#getRootComponent()
     */
    public JComponent getRootComponent() {
        return this.root;
    }

    /**
     * @see IModule#isClosedModule()
     */
    public boolean isClosedModule() {
        return this.isClosed;
    }
}
