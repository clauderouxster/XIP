/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 *
 * GoldminerParseTask.java
 *
 * Created on November 10, 2006, 10:50 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer.task;

import com.xerox.jxip.JXip;
import com.xerox.xrce.xipui.CorpusElement;
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.RunContext;
import com.xerox.xrce.xipui.Xip;
import com.xerox.xrce.xipui.goldminer.GoldminerDataWriter;
import com.xerox.xrce.xipui.goldminer.GoldminerEngine;
import com.xerox.xrce.xipui.goldminer.ui.GoldminerOverview;
import com.xerox.xrce.xipui.goldminer.ui.GoldminerParseAction;
import com.xerox.xrce.xipui.goldminer.ui.GoldminerRequest;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.task.AbstractParsingTask;
import com.xerox.xrce.xipui.ui.RunManager;
import java.io.File;
import java.util.Collection;
import java.util.Iterator;
import jcoref.CorefResolver;

/**
 *
 * @author vgrassau
 */
public class GoldminerParseTask extends AbstractParsingTask<Boolean> {

    private final char apply_rule = (char) 1;
    private final boolean xml_output = false;
    private char xml_mode;
    private boolean wasIndentFile;
    private boolean wasHMMOnly;
    // private long parserStartTime;

    /** Creates a new instance of GoldminerParseTask
     * @param context 
     */
    public GoldminerParseTask(ModuleContext context) {
        super(context);
        Project project = getProject();
        RunContext ctx = project.getRunContext();
        idResult = (Integer) ctx.getProperty(RunContext.ID_RESULT);
        //XML mode
        xml_mode = ctx.getXMLMode();
        wasIndentFile = ctx.isIndentFile();
        wasHMMOnly = ctx.isHmmOnly();
        try {
            if (wasIndentFile) {
                ctx.setIndentFile(false);
            }
            if (wasHMMOnly) {
                ctx.setHmmOnly(false);
            }
        } catch (Exception e) {
            error("error, could not initialize context for running correctly ");
        }
    }

    protected void parsingDone(Boolean result) {
        if (result) {
            StringBuffer msg = new StringBuffer("Database was completed correctly");
            //  msg.append(" (in ");
            //  msg.append((System.currentTimeMillis() - parserStartTime) / 1000);
            //  msg.append(" seconds)");
            warning(msg.toString());
        } else {
            warning("Database was not completed !!");
        }
        GoldminerEngine engine = (GoldminerEngine) getProperty("goldminerEngine");
        GoldminerRequest request = (GoldminerRequest) getProperty("goldminerRequest");
        engine.load();
        request.reLoad();
        RunManager runner = (RunManager) getProperty(module_run);
        runner.setEnabledAction(GoldminerParseAction.NAME_, true);
        try {
            runner.addResult(new GoldminerOverview(getModuleContext()));
        } catch (Exception e) {
            log.error(e);
        }
    }

    @Override
    public Boolean doParsing() {
        Boolean success = false;
        Project project = getProject();
        Xip xip = project.getXip();
        setMessage("Parsing and storing...");
        // parserStartTime = System.currentTimeMillis();
        try {
            xip.setRuleLimit(project.getGhandler(), -1);
        } catch (Exception e) {
            warning("Error : " + e);
        }
        setProgress(10);
        GoldminerEngine engine = (GoldminerEngine) getProperty("goldminerEngine");
        try {
            xip.setDisplayMode(project.getGhandler(), 0);
            store(project, corpus);
            //  engine.parseAndStore(project,corpus);
            success = true;
        } catch (Exception e) {
            publish(e);
            success = false;
        }
        setProgress(95);
        setMessage("re-init the runcontext");
        try {
            RunContext ctx = project.getRunContext();
            ctx.setIndentFile(wasIndentFile);
            ctx.setHmmOnly(wasHMMOnly);
            ctx.loadOptions();
        } catch (Exception e) {
            error("error, could not initialize context for running correctly ");
        }
        setProgress(100);
        return success;
    }

    /**
     * 
     * @param interims
     */
    protected void processInterim(java.util.List interims) {
        Object interim = interims.get(0);
        if (interim instanceof Throwable) {
            error("ERROR" + interim);
        }
        if (interim instanceof File) {
            Object storeTime = interims.get(1);
            Object elapsedTime = interims.get(2);
            info(interim + " was parsed and stored in " + storeTime + " ms");
            info("Elapsed Time : " + elapsedTime + " ms\n");
        }
    }

    /**
     * 
     * @param project
     * @param corpus
     */
    public void store(Project project, Collection<CorpusElement> corpus) {
        GoldminerEngine engine = (GoldminerEngine) getProperty("goldminerEngine");
        if (engine.isConnected()) {
            GoldminerDataWriter writer = engine.getGoldminerDataWriter(project);
            CorefResolver resolver = null;
            JXip jxip = project.getXip().getJXip();
            int gHandler = project.getGhandler();
            resolver = new CorefResolver(jxip, gHandler);
            resolver.setTrace(false);
            Iterator<CorpusElement> iteCorpus = corpus.iterator();
            long startFull = System.currentTimeMillis();
            while (iteCorpus.hasNext() && !isCancelled()) {
                CorpusElement ce = iteCorpus.next();
                if (!ce.exists() || ce.isInput() || ce.isPartOfInput() || ce.isXML()) {
                    publish(new Exception(ce + " is not a valid file for goldminer, process interrupted."));
                } else {
                    //try to parse and store the corpus element
                    try {
                        Collection<File> files = ce.getFiles();
                        for (Iterator<File> it = files.iterator(); it.hasNext();) {
                            long start = System.currentTimeMillis();
                            File file = it.next();
                            writer.write(file, resolver);
                            long end = System.currentTimeMillis();
                            publish(file, (end - start), (System.currentTimeMillis() - startFull));
                        //log.info("Stored in " + (end-start) + " ms");
                        //log.info("Elpased time :  "  + (System.currentTimeMillis()-startFull) + "  ms \n");
                        }
                    } catch (Exception e) {
                        publish(e);
                    }
                }
            }
        }
    }
}
