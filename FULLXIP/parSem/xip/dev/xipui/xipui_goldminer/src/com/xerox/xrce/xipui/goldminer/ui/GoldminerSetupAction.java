/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * GoldminerSetupAction.java
 *
 * Created on October 19, 2006, 9:56 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer.ui;

import com.xerox.xrce.xipui.modules.ActionModule;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;

import java.awt.event.*;

/**
 *
 * @author vgrassau
 */
public class GoldminerSetupAction extends ActionModule {

    /** Creates a new instance of GoldminerSetupAction
     * @param context 
     */
    public GoldminerSetupAction(ModuleContext context) {
        super(context, "setup", XIPUIUtilities.getIcon(UIConstants.SETUP));
    }

    public void actionPerformed(ActionEvent evt) {
        // we try to get the properties
        try {
            GoldminerSetup setup = new GoldminerSetup(getModuleContext());
            setup.setVisible(true);
        } catch (Exception e) {
            DEBUG_LOG.debug("probleme : ", e);
        }
    }
}
