/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * NTMOnlyAction.java
 *
 * Created on November 9, 2006, 4:41 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.ide.ui;

import com.xerox.xrce.xipui.RunContext;
import com.xerox.xrce.xipui.modules.ActionModule;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;
import java.awt.event.*;

/**
 *
 * @author vgrassau
 */
public class NTMOnlyAction extends ActionModule {

    public static final String NAME_ = "NTM only";

    /** Creates a new instance of ParseAction
     * @param context 
     */
    public NTMOnlyAction(ModuleContext context) {
        super(context, NAME_, XIPUIUtilities.getIcon(UIConstants.NTMONLY));
    }

    public void actionPerformed(ActionEvent evt) {
        RunContext runContext = getProject().getRunContext();
        info("run in NTM mode only");
        runContext.setNtmOnly(!runContext.isNtmOnly());
    }
}
