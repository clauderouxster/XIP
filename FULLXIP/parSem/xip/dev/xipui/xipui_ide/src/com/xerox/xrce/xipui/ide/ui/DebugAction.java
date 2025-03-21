/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * DebugAction.java
 *
 * Created on November 9, 2006, 1:36 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.ide.ui;

import com.xerox.xrce.xipui.ide.task.*;
import com.xerox.xrce.xipui.modules.ActionModule;
import com.xerox.xrce.xipui.modules.IModule;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.task.TaskManager;
import com.xerox.xrce.xipui.ui.RunManager;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;
import java.awt.Event;

import javax.swing.*;
import java.awt.event.*;

/**
 *
 * @author vgrassau
 */
public class DebugAction extends ActionModule {

    public static final String NAME_ = "Debug";

    /** Creates a new instance of DebugAction
     * @param context 
     */
    public DebugAction(ModuleContext context) {
        super(context, NAME_, XIPUIUtilities.getIcon(UIConstants.DEBUG));
        putValue(ACCELERATOR_KEY, KeyStroke.getKeyStroke(KeyEvent.VK_F6, Event.SHIFT_MASK));
        setToolTipText("Debug (Shift+F6)");
    }

    public void actionPerformed(ActionEvent event) {
        try {
            RunManager runner = (RunManager) getProperty(module_run);
            runner.prepareForRunning();
        } catch (Exception e) {
            log.error("error", e);
        }
        //invoker la parsing
        final IModule module = this;
        TaskManager manager = TaskManager.getInstance();
        manager.invoke(new DebugTask(getModuleContext()));
        setEnabled(false);
    }
}
