/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * OpenFileAction.java
 *
 * Created on July 6, 2006, 11:38 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.ui;

import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.modules.ActionModule;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.editor.DefaultEditor;
import java.awt.Event;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;

/**
 *
 * @author vgrassau
 */
public class OpenFileAction extends ActionModule {

    /** Creates a new instance of OpenFileAction
     * @param context 
     */
    public OpenFileAction(ModuleContext context) {
        super(context, XIPUIUtilities.getApplicationResource("action.open.file"),
                XIPUIUtilities.getIcon(UIConstants.OPEN));
        putValue(Action.ACCELERATOR_KEY, KeyStroke.getKeyStroke(KeyEvent.VK_O, Event.CTRL_MASK));
    }

    public void actionPerformed(ActionEvent event) {
        try {
            JFileChooser openChooser = new JFileChooser();
            JDialog openDialog = new JDialog();
            openDialog.setTitle("File");
            openDialog.setAlwaysOnTop(true);
            openDialog.setModal(true);
            openDialog.getContentPane().add(openChooser, java.awt.BorderLayout.CENTER);
            File file = null;
            int returnVal = openChooser.showOpenDialog(openDialog);
            if (returnVal == JFileChooser.APPROVE_OPTION) {
                // selection du fichier
                file = openChooser.getSelectedFile();
            }
            openDialog.setVisible(false);
            if (file != null) {
                boolean stop = false;
                stop = true;
                JTabbedPane tab = (JTabbedPane) getProperty(CENTER_TABBED);
                Project p = getProject();
                DefaultEditor eFile = new DefaultEditor(getModuleContext(), p.getGrammarEncoding(), false);
                eFile.setFile(file);
                eFile.editFile();
                eFile.setEditable(true);
                eFile.setRootComponent(tab);
                XIPUIUtilities.addComponentOnTab(tab, eFile, XIPUIUtilities.getIcon(UIConstants.EDIT), file.getName(), file.getAbsolutePath());

            }
        } catch (Exception e) {
            severe("Opening Problem", e);
        }
    }
}
