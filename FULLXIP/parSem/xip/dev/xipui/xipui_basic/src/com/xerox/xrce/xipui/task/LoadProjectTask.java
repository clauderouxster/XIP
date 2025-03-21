/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * LoadProjectTask.java
 *
 * Created on November 7, 2006, 4:37 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.task;

import com.xerox.jxip.XipException;
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.ui.editor.EditFileAction;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.modules.TaskModule;
import com.xerox.xrce.xipui.ui.RunManager;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.UIVariable;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;
import java.io.*;
import java.util.concurrent.ExecutionException;
import java.util.*;
import javax.swing.*;

/**
 *
 * @author vgrassau
 */
public class LoadProjectTask extends TaskModule<Boolean, XipException> {

    /** Creates a new instance of LoadProjectTask
     * @param context 
     */
    public LoadProjectTask(ModuleContext context) {
        super(context);
        this.setIndeterminate(false);
    }

    private void editBadFile(String badFile, long badPosition) {
        EditFileAction editFileAction = new EditFileAction(getModuleContext());
        File file = new File(badFile);
        editFileAction.setFile(file);
        editFileAction.putValue(EditFileAction.ICON, XIPUIUtilities.getIcon(UIConstants.CODE_ERROR));
        editFileAction.putValue(EditFileAction.CARET, badPosition);
        editFileAction.setType(EditFileAction.GRAMMAR_EDITOR);
        editFileAction.executeWithoutUI();
    }

    @Override
    public void done() {
        Boolean b;
        try {
            b = get();
        } catch (ExecutionException ex) {
            log.error("ERROR", ex);
            b = false;
        } catch (InterruptedException ex) {
            log.error("ERROR", ex);
            b = false;
        }
        RunManager runner = (RunManager) getProperty(module_run);
        runner.setEnabled(b);
    }

    private Collection<String> checkGrmFile() {
        try {
            log.info("chek grm");
            Collection<String> badVariables = getProject().getGrmFile().resolve();
            log.info("chek fin");
            return badVariables;
        } catch (Exception e) {
            log.error("error", e);
            return new Vector();
        }
    }

    @Override
    public Boolean doInBackground() {
        Boolean success = false;
        try {
            Project project = getProject();
            setMessage("Checking grm file...");
            Collection<String> undefinedVariables = checkGrmFile();
            while (undefinedVariables != null) {
                for (Iterator<String> it = undefinedVariables.iterator(); it.hasNext();) {
                    final String variable = it.next();
                    SwingUtilities.invokeAndWait(new Runnable() {
                        public void run() {
                            UIVariable uiVar = new UIVariable(getModuleContext(), variable);
                            uiVar.setVisible(true);
                        }
                    });
                }
                undefinedVariables = checkGrmFile();
            }
            setProgress(25);
            if (project.isGrammarLoaded()) {
                setMessage("reloading grammar...");
                project.reload();
            } else {
                setMessage("loading project " + project.getName() + "...");
                project.load();
            }
            setProgress(90);
            info(project.getXipTrace());
            if (project.isGrammarLoaded()) {
                setMessage("rule map initialization...");
                project.initRuleMap();
            }
            setProgress(100);
            success = true;
        } catch (XipException e) {
            severe("ERROR", e);
            publish(e);
            success = false;
        } catch (Exception e) {
            log.error("ERROR", e);
            success = false;
        }
        return success;
    }

    public void process(java.util.List<XipException> errors) {
        XipException error = errors.get(0);
        editBadFile(error.getFileName().replace('\\', '/'), error.getPosition());
    }
}
