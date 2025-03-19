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
package com.xerox.xrce.xipui.ui;

import com.xerox.xrce.xipui.CorpusElement;
import com.xerox.xrce.xipui.XipUIContext;
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.ui.editor.InputEditor;
import com.xerox.xrce.xipui.modules.InternalFrameModule;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.task.LoadProjectTask;
import com.xerox.xrce.xipui.task.TaskManager;
import com.xerox.xrce.xipui.ui.corpus.CorpusManager;
import javax.swing.*;

/**
 *
 * @author  vgrassau
 */
public abstract class AbstractProfile extends InternalFrameModule {

    // longueur de la frame par defaut
    public final short WIDTH_IDEP = 800;
    // hauteur  de la frame par defaut
    public final short HEIGHT_IDEP = 600;
    // input editor 
    protected InputEditor inputEditor;
    // the corpusMananer 
    protected CorpusManager corpusManager;
    // the run manager
    protected RunManager runManager;

    /**
     * Create a template for a project.
     *@param context the context for some initialisations
     * @param project 
     *
     */
    public AbstractProfile(ModuleContext context, Project project) {
        super(context, project);
        //NB : the order of the instruction is important
        try {
            initComponents();
            setSize(WIDTH_IDEP, HEIGHT_IDEP);
            menuBar = new JMenuBar();
            setJMenuBar(menuBar);
            //add the tracer as an output
            XIPUIUtilities.addComponentOnTab(outputTabbed, getTracer(), XIPUIUtilities.getIcon(UIConstants.OUTPUT), "Output", "Output");
            //insert components into the module context :
            setProperty(LEFT_SPLIT, leftSplit);
            setProperty(MENUBAR, menuBar);
            setProperty(OUTPUT_TABBED, outputTabbed);
            setProperty(LEFT_TABBED, leftTabbed);
            setProperty(CENTER_TABBED, centerTabbed);
            setProperty(OPTION_TABBED, optionTabbed);
            setProperty(module_tracer, getTracer());
            //we add the file menu
            //we add the edit menu
            menuBar.add(new FileMenu(getModuleContext(), null));
            menuBar.add(new EditMenu(getModuleContext(), null, null, null));
            //we call the abstract method initMenubar;
            initMenuBar();
            setName(project.getName());
            setTitle(project.getName());
            //setup input editor :
            createInputModule();
            //setup corpusManager
            corpusManager = new CorpusManager(getModuleContext());
            XIPUIUtilities.addComponentOnTab(leftTabbed, corpusManager, XIPUIUtilities.getIcon(UIConstants.CORPUS), CorpusManager.TITLE, "Corpus edition");
            //set the run manager
            runManager = new RunManager(getModuleContext());
            XIPUIUtilities.addComponentOnTab(centerTabbed, runManager, XIPUIUtilities.getIcon(UIConstants.RUN), RunManager.TITLE, "Results....");
            setProperty(module_run, runManager);
            initRunManager();
            setRunnerEnabled(false);
            optionTabbed.setVisible(false);
            leftSplit.setDividerSize(0);
            //this.mainSplit.setResizeWeight(1.0);
            // todo fix bug... the output size is not good
            System.out.println(this.leftTabbed.getBounds());
            mainSplit.setDividerLocation(0.50);
        //mainSplit.setDividerSize(3);
        } catch (Exception e) {
            log.error("ERROR", e);
        }
    }

    public void setRunnerEnabled(boolean b) {
        this.runManager.setEnabled(b);
    }

    public boolean isLoaded() {
        return getProject().isGrammarLoaded();
    }

    abstract public void reloadCustom();

    public void reload() {
        if (this.corpusManager != null) {
            corpusManager.reload();
        }
        reloadCustom();
    }

    /***
     *Permet d'initialiser le runManager selon un profile donne
     *
     *
     */
    protected abstract void initRunManager();

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc=" Generated Code ">//GEN-BEGIN:initComponents
    private void initComponents() {
        mainSplit = new javax.swing.JSplitPane();
        topSplit = new javax.swing.JSplitPane();
        leftSplit = new javax.swing.JSplitPane();
        leftTabbed = new javax.swing.JTabbedPane();
        optionTabbed = new javax.swing.JTabbedPane();
        centerTabbed = new javax.swing.JTabbedPane();
        outputTabbed = new javax.swing.JTabbedPane();
        menuBar = new javax.swing.JMenuBar();

        setClosable(true);
        setIconifiable(true);
        setMaximizable(true);
        setResizable(true);
        setFrameIcon(new javax.swing.ImageIcon(getClass().getResource("/com/xerox/xrce/xipui/ui/images/grammar.gif")));
        setOpaque(true);
        setVisible(true);
        addInternalFrameListener(new javax.swing.event.InternalFrameListener() {
            public void internalFrameActivated(javax.swing.event.InternalFrameEvent evt) {
            }
            public void internalFrameClosed(javax.swing.event.InternalFrameEvent evt) {
                formInternalFrameClosed(evt);
            }
            public void internalFrameClosing(javax.swing.event.InternalFrameEvent evt) {
                formInternalFrameClosing(evt);
            }
            public void internalFrameDeactivated(javax.swing.event.InternalFrameEvent evt) {
            }
            public void internalFrameDeiconified(javax.swing.event.InternalFrameEvent evt) {
            }
            public void internalFrameIconified(javax.swing.event.InternalFrameEvent evt) {
            }
            public void internalFrameOpened(javax.swing.event.InternalFrameEvent evt) {
            }
        });

        mainSplit.setBorder(null);
        mainSplit.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
        mainSplit.setResizeWeight(0.66);
        mainSplit.setFocusCycleRoot(true);
        mainSplit.setOneTouchExpandable(true);
        topSplit.setBorder(null);
        topSplit.setAutoscrolls(true);
        topSplit.setFocusCycleRoot(true);
        topSplit.setOneTouchExpandable(true);
        leftSplit.setBorder(null);
        leftSplit.setDividerSize(3);
        leftSplit.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
        leftSplit.setResizeWeight(1.0);
        leftSplit.setAutoscrolls(true);
        leftSplit.setContinuousLayout(true);
        leftSplit.setFocusCycleRoot(true);
        leftSplit.setMinimumSize(new java.awt.Dimension(0, 0));
        leftTabbed.setTabLayoutPolicy(javax.swing.JTabbedPane.SCROLL_TAB_LAYOUT);
        leftTabbed.setAutoscrolls(true);
        leftTabbed.setFont(new java.awt.Font("Arial", 0, 11));
        leftTabbed.setMinimumSize(new java.awt.Dimension(0, 0));
        leftTabbed.setPreferredSize(new java.awt.Dimension(300, 0));
        leftSplit.setLeftComponent(leftTabbed);

        optionTabbed.setTabLayoutPolicy(javax.swing.JTabbedPane.SCROLL_TAB_LAYOUT);
        optionTabbed.setPreferredSize(new java.awt.Dimension(0, 0));
        leftSplit.setRightComponent(optionTabbed);

        topSplit.setLeftComponent(leftSplit);

        centerTabbed.setTabLayoutPolicy(javax.swing.JTabbedPane.SCROLL_TAB_LAYOUT);
        centerTabbed.setFocusCycleRoot(true);
        centerTabbed.setFont(new java.awt.Font("Arial", 0, 11));
        topSplit.setRightComponent(centerTabbed);

        mainSplit.setLeftComponent(topSplit);

        outputTabbed.setFont(new java.awt.Font("Arial", 0, 11));
        mainSplit.setRightComponent(outputTabbed);

        getContentPane().add(mainSplit, java.awt.BorderLayout.CENTER);

        setJMenuBar(menuBar);

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void formInternalFrameClosed(javax.swing.event.InternalFrameEvent evt) {//GEN-FIRST:event_formInternalFrameClosed
        if (!isClosed()) {
            close();
        }
    }//GEN-LAST:event_formInternalFrameClosed

    private void formInternalFrameClosing(javax.swing.event.InternalFrameEvent evt) {//GEN-FIRST:event_formInternalFrameClosing
        close();
    }//GEN-LAST:event_formInternalFrameClosing

    /**
     * Initialisation de la fenetre de saisie
     *
     **/
    private void createInputModule() {
        try {
            String input = getProject().getCorpusDirectory() + "/" + Project.DEFAULT_CORPUSSET + "/" + "input.txt";
            CorpusElement ce = getProject().getCorpusElement(input);
            ce.setInput(true);
            inputEditor = new InputEditor(getModuleContext(), ce);
            XIPUIUtilities.addComponentOnTab(outputTabbed, inputEditor, XIPUIUtilities.getIcon(UIConstants.INPUT), "Input", "Input");
            setProperty(module_input, inputEditor);
            inputEditor.setCaretPosition(inputEditor.getLenght());
            log.info("Input is loaded");
        } catch (Exception e) {
            warning("Failure to load input file");
        }
    }

    /**
     *Rend active la frame.
     *@param b boolean true rend la frame active, false sinon
     *
     */
    public void setActive(boolean b) {
        try {
            JDesktopPane desktop = this.getDesktopPane();
            JInternalFrame[] iFrames = desktop.getAllFrames();
            for (int i = 0; i < iFrames.length - 1; i++) {
                iFrames[i].setSelected(!b);
            }
            if (!b) {
                iFrames[iFrames.length - 1].setSelected(true);
                desktop.setSelectedFrame(iFrames[iFrames.length - 1]);
            } else {
                setSelected(b);
                desktop.setSelectedFrame(this);
            }
        } catch (Exception e) {
            DEBUG_LOG.trace("ERROR", e);
        }
    }

    /*
     *Set up the menu bar
     *
     */
    protected abstract void initMenuBar();

    protected abstract void initProfile();

    public void load() {
        try {
            if (!isLoaded()) {
                TaskManager manager = TaskManager.getInstance();
                manager.invoke(new LoadTemplateTask(getModuleContext()));
            } else {
                reload();
            }
        } catch (Exception e) {
            severe("ERROR : " + e, e);
        }
    }

    /**
     * Renvoie la representation de l'objet sous forme d'un string
     *
     * @return le nom du AbstractProfile, appel la methode getName()
     */
    public String toString() {
        return this.getName();
    }

    public void close() {
        try {
            log.info("trying to save project : " + getProject().getName());
            getProject().save();
            info("project : " + getProject().getFullName() + " was saved");
        } catch (Exception e) {
            severe("Could not close project correctly", e);
        }
        XipUIContext context_ = (XipUIContext) getModuleContext().getContextParent();
        context_.getProjectReferenced(getProject()).setOpened(false);
        closeIModule();
        if (corpusManager != null) {
            corpusManager.close();
        }
        if (runManager != null) {
            runManager.close();
        }
        getModuleContext().clear();
        runManager = null;
        corpusManager = null;
        root = null;
        inputEditor = null;
        mainSplit = null;
        menuBar = null;
        optionTabbed = null;
        centerTabbed = null;
        topSplit = null;
        leftSplit = null;
        leftTabbed = null;
        optionTabbed = null;
        outputTabbed = null;
        isClosed1 = true;
    }

    protected abstract void closeIModule();
    // Variables declaration - do not modify//GEN-BEGIN:variables
    protected javax.swing.JTabbedPane centerTabbed;
    protected javax.swing.JSplitPane leftSplit;
    protected javax.swing.JTabbedPane leftTabbed;
    private javax.swing.JSplitPane mainSplit;
    protected javax.swing.JMenuBar menuBar;
    protected javax.swing.JTabbedPane optionTabbed;
    protected javax.swing.JTabbedPane outputTabbed;
    private javax.swing.JSplitPane topSplit;
    // End of variables declaration//GEN-END:variables

    private class LoadTemplateTask extends LoadProjectTask {

        public LoadTemplateTask(ModuleContext context) {
            super(context);
        }
        @Override
        public void done() {
            super.done();
            initProfile();
        }
    }
}
