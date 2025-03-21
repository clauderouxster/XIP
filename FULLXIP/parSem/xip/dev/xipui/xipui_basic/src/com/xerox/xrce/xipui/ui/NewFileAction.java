/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * NewFileAction.java
 *
 * Created on June 28, 2006, 9:48 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.ui;

import com.xerox.xrce.xipui.modules.ActionModule;
import com.xerox.xrce.xipui.modules.ModuleContext;
import javax.swing.*;
import java.awt.event.*;
import java.awt.Event;

/**
 *
 * @author vgrassau
 */
public class NewFileAction extends ActionModule {

    private short option;

    /** Creates a new instance of NewFileAction
     * @param context
     * @param option 
     */
    public NewFileAction(ModuleContext context, short option) {
        super(context, XIPUIUtilities.getApplicationResource("item.new.file"),
                XIPUIUtilities.getIcon(UIConstants.CREATE));
        this.option = option;
        putValue(Action.ACCELERATOR_KEY, KeyStroke.getKeyStroke(KeyEvent.VK_N, Event.CTRL_MASK));
    }

    public void actionPerformed(ActionEvent evt) {
        FileStepManager manager = new FileStepManager(getModuleContext(), option);
        manager.setVisible(true);
    }
}
