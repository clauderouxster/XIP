/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 ****************************************************************************************
 * Copyright (c) 2006 Xerox Corporation                                                 *
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
package com.xerox.xrce.xipui.util;

import java.util.*;
import org.apache.log4j.*;

/**
 *
 * @author vgrassau
 */
public abstract class Context extends Hashtable {
    //logger for this class

    protected Logger log = Logger.getLogger("context");
    // the parent context, can be null
    private Context contextParent;

    /**
     *Instanciate a new context
     */
    public Context() {
        super();
    }

    /**
     *instanciate a context with a parent context.
     *@param context a context 
     */
    public Context(Context context) {
        super(context);
        contextParent = context;
    }

    /**
     *Set a property for this context.
     *@param name the name for the property
     *@param value the object to insert like a property
     *
     *NB : this class obliged user to have a string as key
     */
    public void setProperty(String name, Object value) {
        //log.info("add property : (" + name + ", " + value +")");
        if (contextParent != null && contextParent.getProperty(name) != null) {
            contextParent.setProperty(name, value);
        }
        put(name, value);
    }

    /**
     *Return the property giving the name
     *@param name the name of the property
     *@return the property if found otherwise null
     */
    public Object getProperty(String name) {
        Object o = get(name);
        if (o == null && contextParent != null) {
            return contextParent.getProperty(name);
        } else {
            return o;
        }
    }

    /**
     *Must be implemented.
     *Initialize the context
     *@return true if the setup was corectly done false otherwise
     */
    public abstract boolean initContext();

    /**
     *Return the context parent
     *@return a context or null if the context has no parent
     */
    public Context getContextParent() {
        return this.contextParent;
    }
}
