/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * DebugTask.java
 *
 * Created on November 9, 2006, 5:45 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.ide.task;

import com.xerox.xrce.xipui.ide.ui.*;
import com.xerox.xrce.xipui.task.AbstractParsingTask;
import com.xerox.xrce.xipui.ui.RunManager;
import com.xerox.xrce.xipui.CorpusElement;
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.RunContext;
import com.xerox.xrce.xipui.Xip;
import com.xerox.xrce.xipui.ide.DebugXip;
import com.xerox.xrce.xipui.ide.grammar.GrammarEdit;
import com.xerox.xrce.xipui.modules.ModuleContext;
import java.io.File;
import java.util.*;

/**
 *
 * @author vgrassau
 */
public class DebugTask extends AbstractParsingTask<Collection<DebugElement>> {

    /** Creates a new instance of DebugTask
     * @param context 
     */
    public DebugTask(ModuleContext context) {
        super(context);

    }

    @Override
    protected void parsingDone(Collection<DebugElement> elements) {
        RunManager runner = (RunManager) getProperty(module_run);
        runner.setEnabledAction(DebugAction.NAME_, true);
        if (elements != null && !elements.isEmpty()) {
            DebugViewer debug = new DebugViewer(getModuleContext(), idResult);
            debug.setListCorpusElement(corpus); // construction de l'arbre.

            debug.init(elements);
            runner.addResult(debug);
            debug.requestFocusInWindow();
        } else {
            warning("No sentences for the debuger");
        }
    }

    @Override
    protected Collection<DebugElement> doParsing() {
        Collection<DebugElement> elements = new Vector();
        Project project = getProject();
        Xip xip = project.getXip();
        DebugXip debug = new DebugXip(xip);
        if (hadtoBeReloaded()) {
            publish(true); //reload the grammar edit module

        }
        try {
            //xip.setDisplayMode(project.getGhandler(),0);
            xip.addFlag(project.getGhandler(), Xip.DISPLAY_SENTENCE_NUMBER);
            xip.addFlag(project.getGhandler(), Xip.DISPLAY_DEPENDENCY_NUMBER);
            //counter of sentences
            int cpt = 0;
            int numCorpus = 1;
            Iterator<CorpusElement> ite = corpus.iterator();
            while (ite.hasNext() && !isCancelled()) {
                CorpusElement ce = ite.next();
                //if ce is a directory the parsing is refused.
                if (!ce.isDirectory()) {
                    ce = ce.createSubCorpusElement();
                    debug.prepareDebug(project.getGhandler(), ce);
                    if (ce.isInput() || ce.isPartOfInput()) {
                        setMessage("parsing of input...");
                    } else {
                        setMessage("Parsing of " + ce.toString() + "...");
                    }
                    publish(ce.getFile());
                }
                setProgress(((numCorpus * 100) / corpus.size()) / 2);
                numCorpus++;
            }
            RunContext rContext = getProject().getRunContext();
            rContext.loadOptions();
        } catch (Exception e) {
            log.error("ERROR", e);
            warning("can't create debug elements");
        }
        return debug.getElements();
    }

    @Override
    protected void processInterim(java.util.List list) {
        Object o = list.get(0);
        if (o instanceof Boolean) {
            GrammarEdit gEdit = (GrammarEdit) getProperty(module_grammar);
            if (gEdit != null) {
                gEdit.reload();
            }
        }
        if (o instanceof File) {
            File f = (File) o;
            info(f + " was parsed");
        }
    }
}
