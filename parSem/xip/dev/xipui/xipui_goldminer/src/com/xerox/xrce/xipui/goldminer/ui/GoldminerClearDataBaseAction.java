/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * GoldminerClearDataBaseAction.java
 *
 * Created on October 19, 2006, 12:49 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer.ui;

import com.xerox.xrce.xipui.goldminer.GoldminerDataBase;
import com.xerox.xrce.xipui.goldminer.GoldminerEngine;
import com.xerox.xrce.xipui.modules.ActionModule;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;
import java.awt.event.*;

/**
 *
 * @author vgrassau
 */
public class GoldminerClearDataBaseAction extends ActionModule {

    public static final String NAME_ = "Clear Database";

    /** Creates a new instance of GoldminerSearchAction
     * @param context 
     */
    public GoldminerClearDataBaseAction(ModuleContext context) {
        super(context, "Clear Database", XIPUIUtilities.getIcon(UIConstants.CLEAR));
        putValue(javax.swing.Action.SHORT_DESCRIPTION, "Clear database");
    }

    public void actionPerformed(ActionEvent evt) {
        try {
            setEnabled(false);
            // we try to get the properties
            GoldminerEngine engine = (GoldminerEngine) getProperty("goldminerEngine");
            GoldminerRequest request = (GoldminerRequest) getProperty("goldminerRequest");
            boolean res = GoldminerDataBase.clearDataBase(engine.getConnection());
            if (res) {
                engine.reload();
                request.reLoad();
                request.repaint();
            } else {
                warning("Can't clear the database");
            }
        } catch (Exception e) {
            error("ERROR " + e);
            DEBUG_LOG.debug("probleme : ", e);
        }
        setEnabled(true);
    }
}
