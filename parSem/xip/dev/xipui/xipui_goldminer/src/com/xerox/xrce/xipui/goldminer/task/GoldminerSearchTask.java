/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 *
 * GoldminerSearchTask.java
 *
 * Created on January 25, 2007, 3:21 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer.task;

import com.xerox.xrce.xipui.goldminer.GoldminerEngine;
import com.xerox.xrce.xipui.goldminer.ui.GoldminerResultViewer;
import com.xerox.xrce.xipui.goldminer.ui.GoldminerSearchAction;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.modules.TaskModule;
import java.util.Iterator;
import java.util.concurrent.ExecutionException;
import javax.swing.Action;

/**
 *
 * @author vgrassau
 */
public class GoldminerSearchTask extends TaskModule<Boolean, Exception> {

    /** Creates a new instance of LoadApplicationTask
     * @param context 
     */
    public GoldminerSearchTask(ModuleContext context) {
        super(context);
    }

    protected Boolean doInBackground() {
        boolean success = false;
        setMessage("Searching and creating answer");
        try {
            GoldminerEngine engine = (GoldminerEngine) getProperty("goldminerEngine");
            setProgress(1);
            success = engine.sendRequest();
        } catch (Exception e) {
            publish(e);
            success = false;
        }
        setProgress(100);
        return success;
    }

    protected void process(java.util.List<Exception> errors) {
        log.error("ERROR in parsing");
        Iterator<Exception> ite = errors.iterator();
        while (ite.hasNext()) {
            Exception error = ite.next();
            severe("ERROR", error);
        }
    }

    protected void done() {
        boolean succeed = false;
        try {
            succeed = get();
        } catch (InterruptedException ex) {
            log.error("ERROR", ex);
            succeed = false;
        } catch (ExecutionException ex) {
            log.error("ERROR", ex);
            succeed = false;
        } catch (Exception ex) {
            log.error("error", ex);
            succeed = false;
        }
        if (!succeed) {
            warning("An error was occured");
        }
        Action action = (Action) getProperty(GoldminerSearchAction.ACTION_);
        action.setEnabled(true);
        GoldminerResultViewer result = (GoldminerResultViewer) getProperty(GoldminerSearchAction.RESULT_VIEWER);
        result.refresh();
    }
}
