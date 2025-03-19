/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * ViewGoldminerResultAction.java
 *
 * Created on October 18, 2006, 10:48 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer.ui;

import com.xerox.xrce.xipui.modules.ActionModule;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.editor.Editor;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;
import javax.swing.*;
import java.io.*;
import java.awt.event.*;
import java.awt.*;

/**
 *
 * @author vgrassau
 */
public class ViewGoldminerResultAction extends ActionModule implements Runnable {

    static public final String WRAPPING = "WRAPPING";
    static public final String ICON = "ICON";
    static public final String FILE = "FILE";
    static public final String ENCODING = "ENCODING";
    static public final String TARGET = "TARGET";
    static public final String CARET = "CARET";

    /** Creates a new instance of ViewGoldminerResultAction
     * @param context 
     */
    public ViewGoldminerResultAction(ModuleContext context) {
        super(context, XIPUIUtilities.getApplicationResource("action.edit.file"),
                XIPUIUtilities.getIcon(UIConstants.GOLDMINER));
        //initialisation des parametres
        putValue(WRAPPING, true);
        putValue(CARET, (long) 0);
        putValue(TARGET, (JTabbedPane) getProperty(CENTER_TABBED));
        putValue(ICON, XIPUIUtilities.getIcon(UIConstants.GOLDMINER));
    }

    public void run() {
        // we try to get the properties
        //use invoke later to assure that we are in the EDT
        SwingUtilities.invokeLater(new Runnable() {

            public void run() {
                try {
                    Boolean wrapping = (Boolean) getValue(WRAPPING);
                    long caretPosition = 0;
                    Object number = getValue(CARET);
                    if (number instanceof Number) {
                        caretPosition = ((Number) number).longValue();
                    }
                    File file = (File) getValue(FILE);
                    String encoding = (String) getValue(ENCODING);
                    Icon icon = (Icon) getValue(ICON);
                    JTabbedPane target = (JTabbedPane) getValue(TARGET);
                    Editor editor = null;
                    if (file.exists()) {
                        editor = new GoldminerEditor(getModuleContext(), encoding, wrapping);
                        if (editor != null) {
                            editor.setFile(file);
                            editor.editFile();
                            editor.setEditable(false);
                            editor.setRootComponent(target);
                            editor.setIcon(icon);
                            XIPUIUtilities.addComponentOnTab(target, (Component) editor, icon, "-" + file.getName() + "-", file.getAbsolutePath());
                            editor.requestFocusInWindow();
                            editor.setCaretPosition(caretPosition);
                            //editor.repaint();
                        }
                    } else {
                        warning("The file " + file + " doesn't exist");
                    }
                } catch (Exception e) {
                    DEBUG_LOG.debug("cannot edit file", e);
                }
            }
        });
        //thread.start();
    }

    public void actionPerformed(ActionEvent evt) {
        final Runnable runner = this;
        Thread thread = new Thread() {

            public void run() {
                try {
                    SwingUtilities.invokeAndWait(runner);
                } catch (Exception e) {
                    log.error("ERROR", e);
                }
            }
        };
        thread.start();
    }

    /**
     * 
     * @param f
     * @param encoding
     */
    public void setFile(File f, String encoding) {
        putValue(FILE, f);
        putValue(ENCODING, encoding);
    }
}
