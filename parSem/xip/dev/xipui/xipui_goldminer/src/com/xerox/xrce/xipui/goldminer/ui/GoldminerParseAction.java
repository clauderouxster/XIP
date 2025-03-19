/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * GoldminerParseAction.java
 *
 * Created on November 10, 2006, 11:07 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer.ui;

import com.xerox.xrce.xipui.goldminer.task.GoldminerParseTask;
import com.xerox.xrce.xipui.task.TaskManager;
import com.xerox.xrce.xipui.modules.ActionModule;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.RunManager;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;

import java.awt.event.*;

/**
 *
 * @author vgrassau
 */
public class GoldminerParseAction extends ActionModule {

    public static final String NAME_ = "Parse and store in Database";

    /** Creates a new instance of GoldminerParseAction
     * @param context 
     */
    public GoldminerParseAction(ModuleContext context) {
        super(context, NAME_, XIPUIUtilities.getIcon(UIConstants.DATA));
    }

    public void actionPerformed(ActionEvent evt) {
        //creer la liste des CorpusElement a parser
        try {
            RunManager runner = (RunManager) getProperty(module_run);
            runner.prepareForRunning();
        } catch (Exception e) {
            log.error(e);
        }
        //invoker la parsing
        TaskManager manager = TaskManager.getInstance();
        manager.invoke(new GoldminerParseTask(getModuleContext()));
        setEnabled(false);
    }
}
