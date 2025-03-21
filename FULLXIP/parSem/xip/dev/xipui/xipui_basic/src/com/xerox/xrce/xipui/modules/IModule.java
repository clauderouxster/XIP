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

import com.xerox.xrce.xipui.XipUIContext;
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.ui.Tracer;
import javax.swing.*;
import org.apache.log4j.*;

/**
 * Cette interface donne le squelette des methodes des objets graphiques pour lingdexip
 * Les objets graphiques de lingdexip doivent implementer cette interface.
 * @author vgrassau
 */
public interface IModule {

    /**
     *The log for development, when we deliver the GUI we should put off this log
     */
    static public Logger DEBUG_LOG = Logger.getLogger("debug");
    /**
     *Some keys to store some properties for a module
     */
    static public final String year = XipUIContext.year;
    static public final String user_mode = XipUIContext.user_mode;
    static public final String version = XipUIContext.version;
    static public final String setup_file = XipUIContext.setup_file;
    static public final String project_directory = XipUIContext.project_directory;
    static public final String grm_directory = XipUIContext.grm_directory;
    static public final String xip_parser = XipUIContext.xip_parser;
    static public final String module_tracer = "module_tracer";
    static public final String module_root = "module_root";
    static public final String module_input = "module_input";
    static public final String module_run = "module_run";
    static public final String module_corpus = "module_corpus";
    static public final String module_grammar = "module_grammar";
    static public final String ROOT_MAINPANEL = "ROOT_MAINPANEL";
    static public final String LEFT_SPLIT = "LEFT_SPLIT";
    static public final String OUTPUT_TABBED = "OUTPUT_TABBED";
    static public final String LEFT_TABBED = "LEFT_TABBED";
    static public final String CENTER_TABBED = "CENTER_TABBED";
    static public final String OPTION_TABBED = "OPTION_TABBED";
    static public final String MENUBAR = "MENUBAR";
    //these keys should be removed or updated with the next version of Goldminer
    static public final String LITIGATION_DB = "LITIGATION_DB";
    static public final String REQUEST_PANEL = "REQUEST_PANEL";
    public static final String USER_LITIGATION = "USER_LITIGATION";
    public static final String PASSWORD_LITIGATION = "PASSWORD_LITIGATION";
    public static final String PORT_LITIGATION = "PORT_LITIGATION";
    public static final String HOST_LITIGATION = "HOST_LITIGATION";
    public static final String DATABASE_LITIGATION = "DATABASE_LITIGATION";
    public static final String GOLDMINER_MANAGER = "GOLDMINER_MANAGER";

    /**
     *Initializes the tracer for the module
     *@param t tracer
     */
    public void setTracer(Tracer t);

    /**
     *returns the tracer
     *@return tracer should not be null
     */
    public Tracer getTracer();

    /**
     *Initializes the root component for this module
     *@param root the root Jcomponent
     */
    public void setRootComponent(JComponent root);

    /**
     *returns the root component of this module
     *@return root
     */
    public JComponent getRootComponent();

    /**
     *Closes the module, this methode should be implemented as often as possible. 
     */
    public void close();

    /**
     *Returns the project of this module
     *@return project
     */
    public Project getProject();

    /**
     *Sets the project for this module
     *@param p p can't be null
     */
    public void setProject(Project p);

    /**
     *Sets the module context for this module
     *@param context the context to use. 
     *@throws NullPointerException if context it's null
     *
     */
    public void setModuleContext(ModuleContext context) throws NullPointerException;

    /**
     *Sets a property for the module and insert it in the context.
     *@param name The key to use for the context
     *@param value the objec to insert in the context.
     */
    public void setProperty(String name, Object value);

    /**
     *Returns an object at the specified key in the module context. 
     *@param name the name (key for the context) of the property to get
     *@return an Object or null if not found
     */
    public Object getProperty(String name);

    /**
     *Removes the property from the context
     *@param name the name of the property to remove
     *@return the property denoted by the given name or null, if the property is not found
     */
    public Object removeProperty(String name);

    /**
     *Returns the module context of this module
     *@return a ModuleContext
     */
    public ModuleContext getModuleContext();

    /**
     *Checks if this module is closed.
     *@return true if closed false otherwise
     */
    public boolean isClosedModule();

    /**
     *Writes an info message in the log. The message is also written in the tracer module if 
     *it  exists
     *@param  info the message to write
     */
    public void info(String info);

    /**
     *Write san error or fatal message in the log file and show an error dialog message.
     *The message is also written in the tracer module if it exists.
     *@param msg the message to write
     *@param t the error or the exception caught
     */
    public void severe(String msg, Throwable t);

    /**
     *Writes a warning message in the log file and show a warning dialog message.
     *The message is also written in the tracer module if it exists.
     *@param warning the message to write
     *
     */
    public void warning(String warning);

    /**
     *writes a message in the tracer module if it exists
     *@param msg the message to write
     */
    public void write(String msg);

    /**
     *writes an error message in the tracer module if it exists
     *@param msg the message to write
     */
    public void error(String msg);
}//end of the interface
