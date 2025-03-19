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
package com.xerox.xrce.xipui.ui;

import com.xerox.xrce.xipui.modules.IModule;
import javax.swing.*;
import java.util.*;

/**
 * This interface describe the method for a result from xip and Lingdexip
 * A result is also a IModule 
 * @author vgrassau
 */
public interface Result extends IModule {

    /**
     *Set the name of the result 
     *the name must be created by the implementation, this is the reason 
     *that there are no parameters
     */
    public void setResultName();

    /**
     *Return the name of the result
     *@return a name
     *@see setResultName()
     */
    public String getResultName();

    /**
     *Set the list of corpusElment which are permitted this result
     *@param corpus a Vector of CorpusElement
     */
    public void setListCorpusElement(Collection corpus);

    /**
     *Return the list of corpusElement from this result
     *@return a Vector of CorpusElement
     */
    public Vector getListCorpusElement();

    /**
     *Set the icon for the result
     *usefull to determinate easily a type of result 
     * @param icon 
     */
    public void setIcon(Icon icon);

    /**
     *Return icon
     *@return an icon
     */
    public Icon getIcon();

    /**
     *This methode should be implemented for memory reasons
     *In this methode you should free the result and remove it from 
     *a graphical container. After the call, the result won't be used
     */
    public void closeModule();

    /**
     *Return the id of a result.
     *When you implement a result you need to create or associate an id 
     *for it. This is usefull when you have several results in a container or something 
     *like that
     *@return the id of this result
     */
    public int getId();
}
