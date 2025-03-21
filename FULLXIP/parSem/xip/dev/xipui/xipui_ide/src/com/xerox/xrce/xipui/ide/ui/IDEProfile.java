/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 ****************************************************************************************
 * Copyright (c) 2005 Xerox Corporation                                                 *
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
package com.xerox.xrce.xipui.ide.ui;

import com.xerox.xrce.xipui.ide.grammar.GrammarEdit;
import com.xerox.xrce.xipui.ui.grammar.IndentFile;
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.ide.task.ReloadGrammarTask;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.task.TaskManager;
import com.xerox.xrce.xipui.ui.*;
import com.xerox.xrce.xipui.util.TreeHelp;
import java.awt.event.KeyEvent;
import java.util.*;
import javax.swing.*;

/**
 *
 * @author vgrassau
 */
public class IDEProfile extends AbstractProfile {

    //gestionnaire de la grammaire
    protected GrammarEdit gEdit;
    //the indent file module
    protected IndentFile indent;

    /**
     * Creates a new instance of IDEProfile
     * @param context
     * @param p 
     */
    public IDEProfile(ModuleContext context, Project p) {
        super(context, p);
        gEdit = new GrammarEdit(getModuleContext());
        XIPUIUtilities.addComponentOnTab(leftTabbed, gEdit, XIPUIUtilities.getIcon(UIConstants.GRAMMAR), "Grammar", "Grammar edition");
        setProperty(module_grammar, gEdit);
        indent = new IndentFile(getModuleContext());
        setProperty("indent_file", indent);
    }

    public void reloadCustom() {
    }

    protected void closeIModule() {
        if (gEdit != null) {
            gEdit.close();
        }
    }

    protected void initRunManager() {
        try {
            ///code special pour Stendhal, on ne veut pas que le module de
            /// generation soit visible
            Action parsing = new ParseAction(getModuleContext());
            Action debug = new DebugAction(getModuleContext());
            Action generate = new GenerateAction(getModuleContext());
            Action display = new XipDisplayAction(getModuleContext());
            runManager.addAction(parsing);
            runManager.addAction(debug);
            runManager.addAction(generate);
            runManager.addAction(display);
            // le run menu
            runMenu.add(parsing);
            runMenu.add(debug);
            runMenu.add(generate);
            runMenu.addSeparator();
            JCheckBoxMenuItem ntmOnly = new JCheckBoxMenuItem();
            ntmOnly.setSelected(getProject().getRunContext().isNtmOnly());
            ntmOnly.setAction(new NTMOnlyAction(getModuleContext()));
            runMenu.add(ntmOnly);
            JCheckBoxMenuItem hmmOnly = new JCheckBoxMenuItem();
            hmmOnly.setSelected(getProject().getRunContext().isHmmOnly());
            hmmOnly.setAction(new HMMOnlyAction(getModuleContext()));
            runMenu.add(hmmOnly);
            JCheckBoxMenuItem single = new JCheckBoxMenuItem();
            single.setSelected(!getProject().getRunContext().isSingleMode());
            single.setAction(new RunMultipleAction(getModuleContext()));
            runMenu.add(single);
            runMenu.addSeparator();
            runMenu.add(display);
        } catch (Exception e) {
            log.trace(e);
            warning("Failure to load runManager module");
        }
    }

    protected void initMenuBar() {
        grammarMenu = XIPUIUtilities.createJMenu("Grammar", "Grammar", null);
        runMenu = XIPUIUtilities.createJMenu("Run", "Run", null);
        grammarMenu.setFont(UIConstants.DEFAULT_FONT);
        menuTools = XIPUIUtilities.createJMenu("Tools", "Tools", null);
        menuTools.setEnabled(false);
        NewFileAction newFileAction = new NewFileAction(getModuleContext(), FileStepManager.GRAMMAR_ONLY);
        newFileAction.setLabelAction(XIPUIUtilities.getApplicationResource("action.new.grammar.file"));
        JMenuItem i = XIPUIUtilities.createJMenuItem("New File", "create a new grammar file", XIPUIUtilities.getIcon(UIConstants.CREATE));
        i.setAction(newFileAction);
        checker = XIPUIUtilities.createJMenuItem("Checker", "Check grammar", null);
        checker.setEnabled(false);
        reloadGrammarItem = XIPUIUtilities.createJMenuItem("Reload", "Reload grammar", XIPUIUtilities.getIcon(UIConstants.REFRESH));
        reloadGrammarItem.addActionListener(new java.awt.event.ActionListener() {

            public void actionPerformed(java.awt.event.ActionEvent evt) {
                reloadGrammarItemActionPerformed(evt);
            }
        });
        reloadGrammarItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_F5, 0, true));
        JMenuItem help = XIPUIUtilities.createJMenuItem("Available Operations", "Available Operations", XIPUIUtilities.getIcon(UIConstants.TUTORIAL));
        help.addActionListener(new java.awt.event.ActionListener() {

            public void actionPerformed(java.awt.event.ActionEvent evt) {
                try {
                    TreeHelp th = new TreeHelp(false);
                    Hashtable table = getProject().getXip().getAvailableOperations();
                    th.setTree(table);
                    th.setHelpText("List of operations which are available in XIP");
                } catch (Exception e) {
                    DEBUG_LOG.fatal("Could not open Help dialog", e);
                }
            }
        });
        grammarMenu.add(i);
        grammarMenu.add(checker);
        grammarMenu.addSeparator();
        grammarMenu.add(reloadGrammarItem);
        grammarMenu.addSeparator();
        grammarMenu.add(help);
        corpusMenu = XIPUIUtilities.createJMenu("Corpus", "Corpus", null);
        menuBar.add(grammarMenu);
        menuBar.add(corpusMenu);
        menuBar.add(runMenu);
        menuBar.add(menuTools);
    }

    protected void initProfile() {
        GrammarEdit gEdit1 = (GrammarEdit) getProperty(module_grammar);
        gEdit1.setSelectedView(GrammarEdit.RULE_MAP_LABEL);
    }

    private void reloadGrammarItemActionPerformed(java.awt.event.ActionEvent evt) {
        try {
            info("Reload Grammar :" + getProject().getGhandler());
            TaskManager manager = TaskManager.getInstance();
            manager.invoke(new ReloadGrammarTask(getModuleContext()));
            this.setRunnerEnabled(false);
        } catch (Exception e) {
            severe("Could not reload grammar", e);
        }
    }

    private void editActionPerformed(java.awt.event.ActionEvent evt) {
        gEdit.reload();
    }
    protected JMenu corpusMenu;
    private JMenuItem checker;
    protected JMenu grammarMenu;
    private JMenu menuTools;
    private JMenuItem operationsHelp;
    private JMenuItem reloadGrammarItem;
    private JMenu runMenu;
}// end of class
