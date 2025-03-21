/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * GoldminerSearchAction.java
 *
 * Created on October 19, 2006, 9:57 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer.ui;

import com.xerox.xrce.xipui.goldminer.task.GoldminerSearchTask;
import com.xerox.xrce.xipui.modules.ActionModule;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;
import java.awt.event.*;
import com.xerox.xrce.xipui.task.TaskManager;

/**
 *
 * @author vgrassau
 */
public class GoldminerSearchAction extends ActionModule {

    private GoldminerResultViewer result;
    public static final String ACTION_ = "GoldminerSearchAction";
    public static final String RESULT_VIEWER = "GodlminerResultViewer";

    /** Creates a new instance of GoldminerSearchAction
     * @param context
     * @param result 
     */
    public GoldminerSearchAction(ModuleContext context, GoldminerResultViewer result) {
        super(context, "Search", XIPUIUtilities.getIcon(UIConstants.SEARCH));
        this.result = result;
        setProperty(RESULT_VIEWER, result);
        setProperty(ACTION_, this);
    }

    public void actionPerformed(ActionEvent evt) {
        // we try to get the properties
        result.setSearching();
        TaskManager manager = TaskManager.getInstance();
        manager.invoke(new GoldminerSearchTask(getModuleContext()));
        setEnabled(false);
    }
}
