/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * XipDisplayAction.java
 *
 * Created on November 9, 2006, 5:11 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.ui;

import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.modules.ActionModule;
import com.xerox.xrce.xipui.modules.ModuleContext;

import javax.swing.*;
import java.awt.event.*;

/**
 *
 * @author vgrassau
 */
public class XipDisplayAction extends ActionModule {

    private OptionXipRun options = null;
    public static final String NAME_ = "Display Options";

    /** Creates a new instance of RunMultipleAction
     * @param context 
     */
    public XipDisplayAction(ModuleContext context) {
        super(context, NAME_, XIPUIUtilities.getIcon(UIConstants.PROPERTIES));
        Project project = getProject();
        if (project != null && project.getGhandler() != -1) {
            options = new OptionXipRun(context);
        }
    }

    public void actionPerformed(ActionEvent event) {
        if (options == null) {
            options = new OptionXipRun(getModuleContext());
        }
        JDialog dialogOptions = new JDialog();
        dialogOptions.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);
        dialogOptions.setLocationByPlatform(true);
        dialogOptions.setContentPane(options);
        dialogOptions.setTitle("Options");
        dialogOptions.setModal(true);
        dialogOptions.setSize(384, 350);
        dialogOptions.setVisible(true);
    }
}
