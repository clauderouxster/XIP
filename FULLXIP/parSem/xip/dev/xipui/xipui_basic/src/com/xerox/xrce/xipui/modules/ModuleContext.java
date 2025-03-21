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

import com.xerox.xrce.xipui.ui.graph.SessionGraph;
import com.xerox.xrce.xipui.util.Context;

import com.xerox.xrce.xipui.Project;
import java.util.*;

/**
 *
 * @author vgrassau
 */
public class ModuleContext extends Context {

    /** key for the graphical session*/
    public static final String SESSION = "SESSION";
    public static final String PROJECT = "PROJECT";

    /**
     * Creates a new instance of ModuleContext
     */
    public ModuleContext() {
        super();
    }

    /**
     *Creates a new ModuleContext with a context
     *@param context context to use for initialization
     */
    public ModuleContext(Context context) {
        super(context);
    }

    /**
     *Returns a SessionGraph for the ModuleContext
     *@param b to create a new SessionGraph if it doesn't exist.
     *@param id the id for the session
     *@return the  SessionGraph if it already exists, null if it doesn't exist and with false in parameter.
     */
    public SessionGraph getSessionGraph(boolean b, int id) {
        SessionGraph sessionGraph = (SessionGraph) getProperty(SESSION + id);
        if (b && (sessionGraph == null)) {
            try {
                Thread.sleep(5);
            } catch (Exception ex) {
            }
            Date date = new Date();
            //instanciate a new session
            sessionGraph = new SessionGraph(this, id);
            setProperty(SESSION + id, sessionGraph);
            return sessionGraph;
        }
        return sessionGraph;
    }

    /**
     *Returns a SessionGraph and creates it if doesn't exist.
     *@param id the id of the session
     *@return SessionGraph
     */
    public SessionGraph getSessionGraph(int id) {
        return getSessionGraph(true, id);
    }

    /**
     *Sets the project.
     *@param p the project must not be null
     *
     */
    public void setProject(Project p) {
        setProperty(PROJECT, p);
    }

    /***
     *Returns the project
     *@return project null if no project was inserted
     */
    public Project getProject() {
        return (Project) getProperty(PROJECT);
    }

    /**
     *Initializes the context
     *@return always true for this context
     */
    public boolean initContext() {
        return true;
    }
}
