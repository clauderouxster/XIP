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
 * This class permits to create task with graphical component in XIPUI
 * This class is abstract see the SwingWorker to create your own task
 * This task should be manipulate after a instanciation by the TaskManager.
 *
 * @param T 
 * @param V 
 * @author vgrassau
 */
public abstract class TaskModule<T, V> extends SwingWorker<T, V> implements IModule, Serializable {

    private boolean indeterminate = true;
    //tracer to use for this module
    private Tracer tracer;
    //the root component
    protected JComponent root;
    private String message = null;
    private ModuleContext context;
    protected Logger log = Logger.getLogger("module");

    /**
     * Create a task module with an empty context
     * Creates a new instance of AbstractModule
     */
    public TaskModule() {
        this.context = new ModuleContext();
        setTracer(null);//creates an empty Tracer
    }

    /**
     *Indicates if the task knows its progession
     *@param b if true the task will be indeterminate
     */
    protected void setIndeterminate(boolean b) {
        this.indeterminate = b;
    }

    /**
     *Checks if the task is indetermintade or not
     *@return true if it is
     */
    public boolean isIndeterminate() {
        return this.indeterminate;
    }

    public Object removeProperty(String name) {
        return getModuleContext().remove(name);
    }

    /**
     *Returns a message from the task
     *a descrption of a special instruction for example.
     *@return a message can be null
     */
    public String getMessage() {
        return this.message;
    }

    /**
     *Sets a message for the task
     *@param message
     */
    public void setMessage(String message) {
        this.message = message;
    }

    /**
     * Writes an error or an exception in the log file and the tracer module. We show an error dialog.
     * @param msg the message to write
     * @param e the exception or the error
     */
    public void severe(String msg, Throwable e) {
        log.fatal(msg, e);
        tracer.severe(msg, e);
    }

    /**
     * Writes an info message in the log file and in the tracer module
     * @param msg the message to write
     */
    public void info(String msg) {
        log.info(msg);
        tracer.message(msg);
    }

    /**
     * Writes a warning message on the log file and tracer module and show a warning dialog.
     * @param warning The message to write
     */
    public void warning(String warning) {
        log.warn(warning);
        tracer.warning(warning);
    }

    /**
     * Writes a message in the tracer with a end of line at the end
     * @param msg the message to write
     */
    public void write(String msg) {
        tracer.writeln(msg);
    }

    /**
     * Writes an error message in the tracer.
     * @param msg , the message to write
     */
    public void error(String msg) {
        tracer.error(msg);
    }

    /**
     * Creates a task module with a given context.
     * @param context
     * @throws java.lang.NullPointerException
     */
    public TaskModule(ModuleContext context) throws NullPointerException {
        this();
        setModuleContext(context);
        setTracer((Tracer) context.getProperty(module_tracer));
        setRootComponent((JComponent) context.getProperty(module_root));
    }

    /**
     * Sets the module context for this task. The previous context will be lost for this task
     * @param context the new context,
     * @throws java.lang.NullPointerException if the context is null, the previous context will be kept
     */
    public void setModuleContext(ModuleContext context) throws NullPointerException {
        if (context != null) {
            this.context = context;
        } else {
            throw new NullPointerException();
        }
    }

    /**
     * Sets a property for this task
     * @param name the name of the property to set
     * @param value  the value of the property
     */
    public void setProperty(String name, Object value) {
        getModuleContext().setProperty(name, value);
    }

    /**
     * Returns the property value of the given name
     * @param name the name of the property
     * @return the value or null if the property is not found
     */
    public Object getProperty(String name) {
        return getModuleContext().getProperty(name);
    }

    /**
     * Returns the module context of the task
     * @return a ModuleContext can't be null
     */
    public ModuleContext getModuleContext() {
        return context;
    }

    /**
     * Return the project used by this module
     * @return a project (can be null)
     */
    public Project getProject() {
        return this.context.getProject();
    }

    /**
     * sets the project to use for this module
     * @param project
     */
    public void setProject(Project project) {
        this.context.setProject(project);
    }

    /**
     * Sets the tracer of this module
     * @param t the new tracer, if null a empty will be created
     */
    public void setTracer(Tracer t) {
        tracer = t;
        if (tracer == null) {
            tracer = new Tracer();
        }
    }

    /**
     * Sets the root component of this module
     * @param root a component can be null
     */
    public void setRootComponent(JComponent root) {
        this.root = root;
    }

    /**
     * Returns the Tracer of this module
     * @return a tracer can't be null
     */
    public Tracer getTracer() {
        return this.tracer;
    }

    /**
     * Returns the root component for this module
     * @return a component can be null
     */
    public JComponent getRootComponent() {
        return this.root;
    }

    /**
     * Checks if the task is done or the task was canceled.
     * @return true if the task was done or canceld false otherwise
     */
    public boolean isClosedModule() {
        return this.isDone() || this.isCancelled();
    }

    /**
     *Close the task...
     */
    public void close() {
        this.cancel(true);
    }
}// end of class
