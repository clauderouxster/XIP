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
package com.xerox.xrce.xipui.task;

import com.xerox.xrce.xipui.XipUIContext;
import com.xerox.xrce.xipui.modules.TaskModule;
import com.xerox.xrce.xipui.ui.XipUI;
import com.xerox.xrce.xipui.ui.Tracer;
import java.util.concurrent.ExecutionException;

/**
 * Load the XIPUI interface
 * @author vgrassau
 */
public class LoadApplicationTask extends TaskModule<XipUIContext, Void> {

    /** Creates a new instance of LoadApplicationTask */
    public LoadApplicationTask() {
    }

    /**
     *Construct the context for XIPUI 
     *@return the context of XIPUI
     */
    protected XipUIContext doInBackground() {
        setMessage("Loading context of XIPUI");
        XipUIContext context = new XipUIContext();
        if (!context.initContext()) {
            log.error("context for XIPUI failed");
            context = null;
        }
        setProgress(100);
        return context;
    }

    protected void done() {
        try {
            XipUIContext context = get();
            if (context == null) {
                exit();
            } else {
                XipUI xipui = new XipUI(context);
            }
        } catch (InterruptedException ex) {
            log.error("ERROR", ex);
            exit();
        } catch (ExecutionException ex) {
            log.error("ERROR", ex);
            exit();
        }
    }

    private void exit() {
        Tracer.warning_("could not intilized the XIP UI context");
        System.err.println("FATAL ERROR, system exit");
        System.exit(1);
    }
}//end of class
