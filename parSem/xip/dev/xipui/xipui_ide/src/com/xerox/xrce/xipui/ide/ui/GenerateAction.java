/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * GenerateAction.java
 *
 * Created on November 9, 2006, 1:36 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.ide.ui;

import com.xerox.xrce.xipui.ide.task.GenerateTask;
import com.xerox.xrce.xipui.modules.ActionModule;
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
public class GenerateAction extends ActionModule {

    public static final String NAME_ = "Generator";

    /** Creates a new instance of GenerateAction
     * @param context 
     */
    public GenerateAction(ModuleContext context) {
        super(context, NAME_, XIPUIUtilities.getIcon(UIConstants.GENERATOR));
        putValue(ACCELERATOR_KEY, KeyStroke.getKeyStroke(KeyEvent.VK_F6, Event.ALT_MASK + Event.SHIFT_MASK));
        setToolTipText("Generate (Alt+Shift+F6)");
    }

    public void actionPerformed(ActionEvent event) {
        try {
            RunManager runner = (RunManager) getProperty(module_run);
            runner.prepareForRunning();
        } catch (Exception e) {
            log.error("ERROR", e);
        }
        //invoker la parsing
        TaskManager manager = TaskManager.getInstance();
        manager.invoke(new GenerateTask(getModuleContext()));
        setEnabled(false);
    }
}
