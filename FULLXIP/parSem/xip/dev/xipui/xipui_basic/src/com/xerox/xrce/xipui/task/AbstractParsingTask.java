/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 ****************************************************************************************
 * Copyright (c) 2007 Xerox Corporation                                                 *
 * Xerox Research Centre Europe - Grenoble                                              *
 * All Rights Reserved                                                                  *
 * Copyright protection claimed includes all forms and matters of copyrightable         *
 * material and information now allowed by statutory or judicial law or                 *
 * hereinafter granted, including without limitation, material generated from           *
 * the software programs which are displayed on the screen such as icons,               *
 * screen display looks, etc. All rights reserved. No part of this document             *
 * may be reproduced, stored in a retrieval system, or transmitted, in any              *
 * form or by any means, electronic, mechanical, photocopied, recorded, or              *
 * otherwise, without prior written consent from XEROX corporation.                     *
 ****************************************************************************************
 */
package com.xerox.xrce.xipui.task;

import com.xerox.jxip.*;

import com.xerox.xrce.xipui.ui.editor.EditFileAction;
import com.xerox.xrce.xipui.CorpusElement;
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.RunContext;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.modules.TaskModule;
import com.xerox.xrce.xipui.ui.RunManager;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;

import java.io.File;
import java.util.*;
import java.util.concurrent.CancellationException;
import java.util.concurrent.ExecutionException;

/**
 *
 * @param T 
 * @author vgrassau
 */
public abstract class AbstractParsingTask<T> extends TaskModule {

    protected Collection<CorpusElement> corpus;
    protected int idResult;
    // if the grammar had been reloaded before the parsing
    private boolean hadReload = false;

    /** Creates a new instance of AbstractParsingTask
     * @param context 
     */
    public AbstractParsingTask(ModuleContext context) {
        super(context);
        Project project = getProject();
        RunContext ctx = project.getRunContext();
        //get the corpus
        corpus = (Collection) ctx.getProperty(RunContext.CORPUS);
        if (corpus == null) {
            corpus = new Vector();
        }
        idResult = (Integer) ctx.getProperty(RunContext.ID_RESULT);
        setIndeterminate(false);
    }

    /**
     *Calls when the task is finished, all in this method is executed in the EDT
     *@see parsingDone(T);
     */
    @Override
    protected void done() {
        T result = null;
        try {
            result = (T) get();
        } catch (ExecutionException ex) {
            log.error("error", ex);
        } catch (InterruptedException ex) {
            log.error("error", ex);
        } catch (CancellationException ex) {
            log.info("error", ex);
        } catch (Exception ex) {
            log.error("error", ex);
        }
        parsingDone(result);
    }

    /**
     *Checks if the grammar had to be reloaded before the parsing
     *@return true if reloading
     */
    protected boolean hadtoBeReloaded() {
        return hadReload;
    }

    /**
     *Construct a graphical result when parsing is finished
     *This method is called by done()
     * @param result 
     */
    protected abstract void parsingDone(T result);

    /**
     *Do a specific parsing to implement in the doInBackground() method.
     *You should not to use graphical object in this method.
     *@return a result for the parsing
     */
    protected abstract T doParsing();

    /**
     *Does the parsing in background thread. 
     *Before to launch the parsing, checks if the grammar needs to be reloaded.
     *The parsing is done by the method <code>doParsing()</code>.
     *@return T the result for the parsing
     *@see doParsing()
     *@see process(java.util.List)
     *
     */
    @Override
    protected T doInBackground() {
        setMessage("Prepare parsing for " + corpus.size() + " element(s)...");
        setProgress(0);
        T result = null;
        Project project = getProject();
        boolean reload = false;
        try {
            if (project.getGrammar().isModified()) {
                if (project.isGrammarLoaded()) {
                    project.reload();
                    hadReload = true;
                    reload = true;
                } else {
                    project.load();
                    hadReload = true;
                    reload = true;
                }
            } else {
                reload = true;
            }
        } catch (XipException e) {
            publish(e);
            reload = false;

        } catch (Exception e) {
            reload = false;
        }
        if (reload) {
            result = doParsing();
        }
        return result;
    }

    protected void editBadFile(String name, long position) {
        EditFileAction editFileAction = new EditFileAction(getModuleContext());
        File file = new File(name.replace('\\', '/'));
        editFileAction.setFile(file);
        editFileAction.putValue(EditFileAction.ICON, XIPUIUtilities.getIcon(UIConstants.CODE_ERROR));
        editFileAction.putValue(EditFileAction.CARET, position);
        editFileAction.setType(EditFileAction.GRAMMAR_EDITOR);
        editFileAction.executeWithoutUI();
    }

    protected void processInterim(java.util.List interim) {
        //do nothing...
        //Override this method if you need to do something with an interime result in the EDT
    }

    protected void process(java.util.List interims) {
        Object interim = interims.get(0);
        if (interim instanceof XipException) {
            XipException error = (XipException) interim;
            severe("ERROR", error);
            RunManager runner = (RunManager) getProperty(module_run);
            if (runner != null) {
                runner.setEnabled(false);
            }
            editBadFile(error.getFileName(), error.getPosition());
        } else {
            processInterim(interims);
        }
    }
}
