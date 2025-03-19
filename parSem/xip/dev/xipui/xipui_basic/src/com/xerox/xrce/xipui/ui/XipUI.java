/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 ****************************************************************************************
 * Copyright (c) 2006 Xerox Corporation                                                 *
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

import com.xerox.xrce.xipui.modules.*;



import com.xerox.xrce.xipui.*;

import com.xerox.xrce.xipui.task.*;
import javax.swing.*;

/**
 *
 * @author  grassaud
 */
public class XipUI extends javax.swing.JFrame {

    private final int DEFAULT_WIDTH = 1024;
    private final int DEFAULT_HEIGHT = 768;
    // the initial context for the application and some modules 
    private XipUIContext uiContext;
    private ProjectCollector collector;
    public static final String APPLICATION_NAME = "XIP UI";
    private TaskManager taskManager;

    public XipUI(XipUIContext context) {
        taskManager = TaskManager.getInstance();
        //we init the application context
        // if the uiContext initialization failed we have to stop the system.
        uiContext = context;
        if (!context.isInitialized()) {
            Tracer.warning_("context was not initalized !!! can't load XIPUI");
        } else {
            try {
                uiContext.save();
            } catch (Exception e) {
                IModule.DEBUG_LOG.fatal("Error : uiContext was not saved");
            }
            initComponents();
            ImageIcon icon = XIPUIUtilities.getIcon(UIConstants.UI_ICON);
            this.setIconImage(icon.getImage());
            collector = new ProjectCollector(uiContext);
            collector.setRootComponent(mainPanel);
            setTitle(APPLICATION_NAME + " - " + uiContext + " -");
            setName(APPLICATION_NAME);
            initListOpenedProject();
            itemExit.setIcon(XIPUIUtilities.getIcon(UIConstants.EXIT));
            projectManagment.setIcon(XIPUIUtilities.getIcon(UIConstants.SETUP));
            setSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);//taille de la frame par defaut

            menuProject.setFont(UIConstants.DEFAULT_FONT);
            menuHelp.setFont(UIConstants.DEFAULT_FONT);
            userGuideHelp.setIcon(XIPUIUtilities.getIcon(UIConstants.DOCUMENTATION));
            referenceGuideHelp.setIcon(XIPUIUtilities.getIcon(UIConstants.DOCUMENTATION));
            xipUIHelp.setIcon(XIPUIUtilities.getIcon(UIConstants.TUTORIAL));
            TaskManager manager = TaskManager.getInstance();
            statusPanel.add(manager.getTaskProgressBar());
            statusPanel.repaint();
            this.setVisible(true);
        }
    }

    /**
     *Add to the project menu the last opened projects.
     *
     */
    private void initListOpenedProject() {
        try {
            int nbItem = menuProject.getItemCount();
            int end = nbItem - 1;
            int start = 4;
            int length = end - start;
            for (int i = end; i >= start; i--) {
                this.menuProject.remove(i);
            }
            JMenuItem[] items = collector.createShortcutMenu();
            if (items != null) {
                for (int i = 0; i < items.length; i++) {
                    menuProject.add(items[i], start + i);
                }
            }
            uiContext.save();
        } catch (Exception e) {
            IModule.DEBUG_LOG.trace("ERROR", e);
        }
    }

    // <editor-fold defaultstate="collapsed" desc=" Generated Code ">//GEN-BEGIN:initComponents
    private void initComponents() {
        mainPanel = new javax.swing.JDesktopPane();
        statusPanel = new javax.swing.JPanel();
        menuBar = new javax.swing.JMenuBar();
        menuProject = new javax.swing.JMenu();
        projectManagment = new javax.swing.JMenuItem();
        jSeparator1 = new javax.swing.JSeparator();
        itemExit = new javax.swing.JMenuItem();
        jSeparator2 = new javax.swing.JSeparator();
        menuHelp = new javax.swing.JMenu();
        xipUIHelp = new javax.swing.JMenuItem();
        jSeparator3 = new javax.swing.JSeparator();
        userGuideHelp = new javax.swing.JMenuItem();
        referenceGuideHelp = new javax.swing.JMenuItem();
        jSeparator4 = new javax.swing.JSeparator();
        itemAbout = new javax.swing.JMenuItem();

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
        setTitle("LingDeXIP");
        addWindowListener(new java.awt.event.WindowAdapter() {
            public void windowClosed(java.awt.event.WindowEvent evt) {
                formWindowClosed(evt);
            }
            public void windowClosing(java.awt.event.WindowEvent evt) {
                formWindowClosing(evt);
            }
        });

        mainPanel.setAutoscrolls(true);
        mainPanel.setDragMode(javax.swing.JDesktopPane.OUTLINE_DRAG_MODE);
        mainPanel.setName("mainPanel");
        mainPanel.setOpaque(false);
        mainPanel.setPreferredSize(new java.awt.Dimension(1024, 768));
        getContentPane().add(mainPanel, java.awt.BorderLayout.CENTER);
        mainPanel.getAccessibleContext().setAccessibleName("mainPanel");
        mainPanel.getAccessibleContext().setAccessibleParent(menuBar);

        statusPanel.setLayout(new java.awt.BorderLayout());

        statusPanel.setMaximumSize(new java.awt.Dimension(32767, 25));
        statusPanel.setMinimumSize(new java.awt.Dimension(1024, 25));
        statusPanel.setPreferredSize(new java.awt.Dimension(1024, 25));
        getContentPane().add(statusPanel, java.awt.BorderLayout.SOUTH);

        menuProject.setText("Project");
        menuProject.setDoubleBuffered(true);
        projectManagment.setFont(new java.awt.Font("Arial", 0, 11));
        projectManagment.setText("Project Manager");
        projectManagment.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                projectManagmentActionPerformed(evt);
            }
        });

        menuProject.add(projectManagment);

        menuProject.add(jSeparator1);

        itemExit.setFont(new java.awt.Font("Arial", 0, 11));
        itemExit.setText("Exit");
        itemExit.setToolTipText("");
        itemExit.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                itemExitActionPerformed(evt);
            }
        });

        menuProject.add(itemExit);

        menuProject.add(jSeparator2);

        menuBar.add(menuProject);

        menuHelp.setText("Help\n");
        menuHelp.setToolTipText("");
        xipUIHelp.setFont(new java.awt.Font("Arial", 0, 11));
        xipUIHelp.setText("XIP UI Tutorial");
        xipUIHelp.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                xipUIHelpActionPerformed(evt);
            }
        });

        menuHelp.add(xipUIHelp);

        menuHelp.add(jSeparator3);

        userGuideHelp.setFont(new java.awt.Font("Arial", 0, 11));
        userGuideHelp.setText("User Guide");
        userGuideHelp.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                userGuideHelpActionPerformed(evt);
            }
        });

        menuHelp.add(userGuideHelp);

        referenceGuideHelp.setFont(new java.awt.Font("Arial", 0, 11));
        referenceGuideHelp.setText("Reference Guide");
        referenceGuideHelp.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                referenceGuideHelpActionPerformed(evt);
            }
        });

        menuHelp.add(referenceGuideHelp);

        menuHelp.add(jSeparator4);

        itemAbout.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_F1, 0));
        itemAbout.setFont(new java.awt.Font("Arial", 0, 11));
        itemAbout.setText("About");
        itemAbout.setToolTipText("");
        menuHelp.add(itemAbout);

        menuBar.add(menuHelp);

        setJMenuBar(menuBar);
        menuBar.getAccessibleContext().setAccessibleName("menuBar");
        menuBar.getAccessibleContext().setAccessibleParent(this);

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void referenceGuideHelpActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_referenceGuideHelpActionPerformed
        ModuleContext c = new ModuleContext();
        c.setProperty("resource", "/referenceGuide/XIPReferenceGuide.htm");
        taskManager.invoke(TaskManager.Command.LOAD_DOC, c);
    }//GEN-LAST:event_referenceGuideHelpActionPerformed

    private void userGuideHelpActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_userGuideHelpActionPerformed
        ModuleContext c = new ModuleContext();
        c.setProperty("resource", "/userGuide/XIPUserGuide.htm");
        taskManager.invoke(TaskManager.Command.LOAD_DOC, c);
    }//GEN-LAST:event_userGuideHelpActionPerformed

    private void xipUIHelpActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_xipUIHelpActionPerformed
        ModuleContext c = new ModuleContext();
        c.setProperty("resource", "/gui.html");
        taskManager.invoke(TaskManager.Command.LOAD_DOC, c);
    }//GEN-LAST:event_xipUIHelpActionPerformed

    /**
     *Show the project manager and reinit if it's necessary the project menu
     *@param evt the action event
     */
    private void projectManagmentActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_projectManagmentActionPerformed
        collector.setVisible(true);
        initListOpenedProject();
    }//GEN-LAST:event_projectManagmentActionPerformed

    /**
     *When the main frame is closing, we try to close the opened projects
     *and save the context.
     *@param evt the window event
     */
    private void formWindowClosing(java.awt.event.WindowEvent evt) {//GEN-FIRST:event_formWindowClosing
        try {
            JInternalFrame[] iFrame = mainPanel.getAllFrames();
            for (int i = 0; i < iFrame.length; i++) {
                if (iFrame[i] != null && iFrame[i] instanceof AbstractProfile) {
                    AbstractProfile gui = (AbstractProfile) iFrame[i];
                    gui.close();
                }
            }
            uiContext.save();
        } catch (Exception e) {
            Tracer.error_("ERROR", e);
            IModule.DEBUG_LOG.trace("ERROR", e);
        }
    }//GEN-LAST:event_formWindowClosing

    /**
     *When the main frame is closed we try to save the application context.
     *@param evt the window event
     */
    private void formWindowClosed(java.awt.event.WindowEvent evt) {//GEN-FIRST:event_formWindowClosed
        try {
            uiContext.save();
        } catch (Exception e) {
            IModule.DEBUG_LOG.trace("ERROR", e);
        }
    }//GEN-LAST:event_formWindowClosed

    /**
     *Save the context and close the application
     *this method calls the close() method
     *@param evt the Action event
     */
    private void itemExitActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_itemExitActionPerformed
        close();
    }//GEN-LAST:event_itemExitActionPerformed

    /**
     *Save the context and close the application
     *We exit with a code equals to 0
     */
    public void close() {
        try {
            uiContext.save();
        } catch (Exception e) {
            IModule.DEBUG_LOG.trace("ERROR", e);
            Tracer.error_("ERROR", e);
        }
        System.exit(0);
    }

    /**
     * The main methode to load the XipUI application.
     * The arguments are not used. If a problem is occured during the initialization we
     * exit with a code return equals to 1.
     * 
     * 
     * 
     * @param args the command line arguments
     * 
     * NB : it's recommended to execute this method with at least 256 mo of heap memory
     * example : java com.xerox.xrce.lingdexip.XipUI -Xmx256m
     * For linux users don't forget to create a LD_LIBRARY_PATH :
     * example : setenv LD_LIBRARY_PATH /tmp/vgrassau/linux/sharedLib
     * 
     * On a Windows system you can't load 2 instances of an XipUI. Windows put some lock on the
     * DLL libraries used by the JXIP API.
     */
    public static void main(String args[]) {
//        SwingUtilities.invokeLater(new Runnable() {
//            public void run() {
//                TaskManager manager = TaskManager.getInstance();
//                manager.loadXipUI(new DeliveryProfileListing());
//                
//             
//            }
//        });
//        System.out.println("****** Copyright (c) 2007 Xerox Corporation  ******");
//        System.out.println("***** Xerox Research Centre Europe - Grenoble *****");
//        System.out.println("******************* XipUI *************************");
//        
    }
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JMenuItem itemAbout;
    private javax.swing.JMenuItem itemExit;
    private javax.swing.JSeparator jSeparator1;
    private javax.swing.JSeparator jSeparator2;
    private javax.swing.JSeparator jSeparator3;
    private javax.swing.JSeparator jSeparator4;
    private javax.swing.JDesktopPane mainPanel;
    private javax.swing.JMenuBar menuBar;
    private javax.swing.JMenu menuHelp;
    private javax.swing.JMenu menuProject;
    private javax.swing.JMenuItem projectManagment;
    private javax.swing.JMenuItem referenceGuideHelp;
    private javax.swing.JPanel statusPanel;
    private javax.swing.JMenuItem userGuideHelp;
    private javax.swing.JMenuItem xipUIHelp;
    // End of variables declaration//GEN-END:variables
}
