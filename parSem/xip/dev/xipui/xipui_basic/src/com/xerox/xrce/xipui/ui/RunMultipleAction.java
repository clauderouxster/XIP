/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * RunMultipleAction.java
 *
 * Created on November 9, 2006, 4:47 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.ui;

import com.xerox.xrce.xipui.RunContext;
import com.xerox.xrce.xipui.modules.ActionModule;
import com.xerox.xrce.xipui.modules.ModuleContext;

import java.awt.event.*;

/**
 *
 * @author vgrassau
 */
public class RunMultipleAction extends ActionModule {

    public static final String NAME_ = "Run Multiple";

    /** Creates a new instance of RunMultipleAction
     * @param context 
     */
    public RunMultipleAction(ModuleContext context) {
        super(context, NAME_, XIPUIUtilities.getIcon(UIConstants.RUN_MULTIPLE));
    }

    public void actionPerformed(ActionEvent event) {
        RunContext runContext = getProject().getRunContext();
        info("run in single mode : " + !runContext.isSingleMode());
        runContext.setSingleMode(!runContext.isSingleMode());
    }
}
