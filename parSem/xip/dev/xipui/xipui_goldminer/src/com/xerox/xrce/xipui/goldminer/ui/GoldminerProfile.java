/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * GoldminerProfile.java
 *
 * Created on September 5, 2006, 3:25 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer.ui;

import com.xerox.xrce.xipui.ui.RunMultipleAction;
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.goldminer.GoldminerEngine;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.AbstractProfile;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;

import javax.swing.*;

/**
 *
 * @author vgrassau
 */
public class GoldminerProfile extends AbstractProfile {

    private GoldminerEngine engine;
    private GoldminerRequest goldminerRequest;
    private JMenu menuRun;
    private JMenu menuGoldminer;
    private JMenu menuCorpus;

    /** Creates a new instance of GoldminerProfile
     * @param context
     * @param project 
     */
    public GoldminerProfile(ModuleContext context, Project project) {
        super(context, project);
        leftSplit.setResizeWeight(0.5);
        leftSplit.setDividerSize(3);
        engine = new GoldminerEngine(project.getFullName());
        if (!engine.load()) {
            StringBuilder msg = new StringBuilder();
            msg.append("<html><body>");
            msg.append("<p><b><font color=\"red\">ERROR : </font></b></p>");
            msg.append("<p>could not initialize the context</p>");
            msg.append("<p>Use the <b><font color=\"green\">Setup </font></b> to check connection paramaters </p>");
            msg.append("<p>check status of the database on : </br>");
            msg.append("<u><font color=\"blue\">");
            msg.append(engine.getHost() + ":" + engine.getPort() + "/" + engine.getDatabase());
            msg.append("</font></u><br>");
            msg.append("</p></body></html>");
            warning(msg.toString());
        //TODO allow to change the DB setup if error during the setting
        //TODO GoldminerSetup setup = new GoldminerSetup(context);
        //TODO setup.setVisible(true);
        }
        setProperty("goldminerEngine", engine);
        goldminerRequest = new GoldminerRequest(getModuleContext());
        JTabbedPane tab = (JTabbedPane) getProperty(OUTPUT_TABBED);
        XIPUIUtilities.addComponentOnTab(tab, goldminerRequest, XIPUIUtilities.getIcon(UIConstants.GOLDMINER), "Request", "build your request");
        try {
            this.runManager.addResult(new GoldminerOverview(getModuleContext()));
        } catch (Exception e) {
            log.error(e);
        }
    }

    public void reloadCustom() {
    }

    protected void closeIModule() {
    }

    protected void initMenuBar() {
        menuRun = XIPUIUtilities.createJMenu("Run", "Run", null);
        menuGoldminer = XIPUIUtilities.createJMenu("DB", "DataBase", null);
        menuCorpus = XIPUIUtilities.createJMenu("Corpus", "Corpus", null);
        JMenuItem goldminerReloadItem = XIPUIUtilities.createJMenuItem("Reload", "Reload context", XIPUIUtilities.getIcon(UIConstants.REFRESH));
        goldminerReloadItem.addActionListener(new java.awt.event.ActionListener() {

            public void actionPerformed(java.awt.event.ActionEvent evt) {
                try {
                    engine.load();
                } catch (Exception e) {
                }
            }
        });
        menuGoldminer.add(goldminerReloadItem);
        // add the menu
        menuBar.add(menuGoldminer);
        menuBar.add(menuCorpus);
        menuBar.add(menuRun);
    }

    protected void initRunManager() {
        try {
            Action parsing = new GoldminerParseAction(getModuleContext());
            Action clearData = new GoldminerClearDataBaseAction(getModuleContext());
            Action runSingle = new RunMultipleAction(getModuleContext());
            runManager.addAction(parsing);
            runManager.addAction(clearData);
            runManager.setEnabledInput(false);
            menuRun.add(parsing);
            menuRun.add(clearData);
            menuRun.addSeparator();
            JCheckBoxMenuItem single = new JCheckBoxMenuItem();
            single.setSelected(!getProject().getRunContext().isSingleMode());
            single.setAction(runSingle);
            menuRun.add(single);
        } catch (Exception e) {
            warning("Failure to init the runManager module");
        }
    }

    public void initProfile() {
    }
}
