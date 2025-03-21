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

import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.modules.PanelModule;
import java.util.*;

/**
 *
 * @author vgrassau
 */
public abstract class AbstractStep extends PanelModule {

    protected boolean couldFinish = false;
    private String description = "...";
    protected Hashtable managerProperties;

    public AbstractStep() {
        super();
    }

    public boolean couldFinish() {
        return this.couldFinish;
    }

    public AbstractStep(ModuleContext context) {
        super(context);
    }

    public abstract void initStepManager();

    public void setManagerProperties(Hashtable prop) {
        this.managerProperties = prop;
    }

    public Hashtable getManagerProperties() {
        return this.managerProperties;
    }

    /**
     *Cette fonction desactive ou active certains composant en fonction d'un mode.
     *Un utilisateur final, n'a pas acces a toutes les options.
     *@param mode : le mode a initialiser. 
     **/
    public abstract void loadModePolicy(short mode);

    /**
     *Initialise la description du panneau
     *@param desc la description
     */
    public void setDescription(String desc) {
        this.description = desc;
    }

    /**
     *@return renvoie la description d'un panneau
     */
    public String getDescription() {
        return this.description;
    }

    public abstract boolean isValide();
}
