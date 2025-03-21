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
public abstract class DialogModule extends JDialog implements Serializable, IModule {

    private Tracer tracer;
    protected JComponent root;
    protected boolean isClosed = false;
    private ModuleContext context;
    protected Logger log = Logger.getLogger("module");

    /**
     * Creates a new instance of AbstractModule
     */
    public DialogModule() {
        this.context = new ModuleContext();
        setTracer(null);
    }

    public Object removeProperty(String name) {
        return getModuleContext().remove(name);
    }

    public void severe(String msg, Throwable e) {
        log.fatal(msg, e);
        tracer.severe(msg, e);
    }

    public void info(String msg) {
        log.info(msg);
        tracer.message(msg);
    }

    public void warning(String warning) {
        log.warn(warning);
        tracer.warning(warning);
    }

    public void write(String msg) {
        tracer.writeln(msg);
    }

    public void error(String msg) {
        tracer.error(msg);
    }

    public DialogModule(ModuleContext context) throws NullPointerException {
        this();
        setModuleContext(context);
        setTracer((Tracer) getProperty(module_tracer));
        setRootComponent((JComponent) getProperty(module_root));
    }

    public String toString() {
        return this.getName();
    }

    public void setModuleContext(ModuleContext context) throws NullPointerException {
        if (context != null) {
            this.context = context;
        } else {
            throw new NullPointerException();
        }
    }

    public void setProperty(String name, Object value) {
        getModuleContext().setProperty(name, value);
    }

    public Object getProperty(String name) {
        return getModuleContext().getProperty(name);
    }

    public ModuleContext getModuleContext() {
        return context;
    }

    public Project getProject() {
        return this.context.getProject();
    }

    public void setProject(Project project) {
        this.context.setProject(project);
    }

    public void setTracer(Tracer t) {
        tracer = t;
        if (tracer == null) {
            tracer = new Tracer();
        }
    }

    public void setRootComponent(JComponent root) {
        this.root = root;
    }

    public Tracer getTracer() {
        return this.tracer;
    }

    public JComponent getRootComponent() {
        return this.root;
    }

    public boolean isClosedModule() {
        return this.isClosed;
    }
}
