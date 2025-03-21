/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * EditFileAction.java
 *
 * Created on July 7, 2006, 10:08 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.ui.editor;

import com.xerox.xrce.xipui.ui.grammar.SpecialisationEditor;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;
import com.xerox.xrce.xipui.Grammar;
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.RuleMap;
import com.xerox.xrce.xipui.modules.ActionModule;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.UIConstants;
import javax.swing.*;
import java.util.*;
import java.io.*;
import java.awt.event.*;
import java.awt.*;

/**
 *
 * @author vgrassau
 */
public class EditFileAction extends ActionModule {

    static public final String EDITABLE = "EDITABLE";
    static public final String WRAPPING = "WRAPPING";
    static public final String EDITOR = "EDITOR";
    static public final String EDIT_MENU = "EDIT_MENU";
    static public final String FILE_MENU = "FILE_MENU";
    static public final String ICON = "ICON";
    static public final String FILE = "FILE";
    static public final String ENCODING = "ENCODING";
    static public final String TARGET = "TARGET";
    static public final String CARET = "CARET";
    static public final String TYPE = "TYPE";
    static public final short BASIC_EDITOR = 0;
    static public final short GRAMMAR_EDITOR = 1;

    /** Creates a new instance of EditFileAction
     * @param context 
     */
    public EditFileAction(ModuleContext context) {
        super(context, XIPUIUtilities.getApplicationResource("action.edit.file"),
                XIPUIUtilities.getIcon(UIConstants.EDIT));
        //initialisation des parametres
        putValue(WRAPPING, false);
        putValue(EDIT_MENU, true);
        putValue(FILE_MENU, true);
        putValue(EDITABLE, true);
        putValue(CARET, (long) 0);
        putValue(TARGET, (JTabbedPane) getProperty(CENTER_TABBED));
        putValue(TYPE, BASIC_EDITOR);
    }

    public void actionPerformed(ActionEvent evt) {
        // we try to get the properties
        try {
            Boolean wrapping = (Boolean) getValue(WRAPPING);
            Boolean editMenu = (Boolean) getValue(EDIT_MENU);
            Boolean fileMenu = (Boolean) getValue(FILE_MENU);
            Boolean editable = (Boolean) getValue(EDITABLE);
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

                switch (getTypeEditor()) {
                    case GRAMMAR_EDITOR: {
                        Project p = getProject();
                        Grammar grammar = p.getGrammar();
                        if (grammar.isCryptedFile(file)) {
                            warning(file + " is crypted");
                        } else {
                            RuleMap ruleMap = p.getRuleMap();
                            Vector rules = new Vector();
                            if (ruleMap != null) {
                                rules = ruleMap.getRulesFromFile(file.getName());
                            }
                            editor = new SpecialisationEditor(getModuleContext(), p.getGrammarEncoding(), rules);
                            break;
                        }
                    }
                    default: {
                        editor = new DefaultEditor(getModuleContext(), encoding, wrapping, fileMenu, editMenu);
                        break;
                    }
                }
                if (editor != null) {
                    editor.setFile(file);
                    editor.editFile();
                    editor.setEditable(editable);
                    editor.setRootComponent(target);
                    editor.setIcon(icon);
                    XIPUIUtilities.addComponentOnTab(target, (Component) editor, icon, file.getName(), file.getAbsolutePath());
                    putValue(EDITOR, editor);
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

    public void setFile(File f, String encoding) {
        putValue(FILE, f);
        putValue(ENCODING, encoding);
    }

    public void setFile(File f) {
        putValue(FILE, f);
    }

    public Editor getEditor() {
        return (Editor) getValue(EDITOR);
    }

    private short getTypeEditor() {
        short type = BASIC_EDITOR;
        Object number = getValue(TYPE);
        if (number instanceof Number) {
            type = ((Number) number).shortValue();
        }
        return type;
    }

    public void setType(short type) {
        putValue(TYPE, type);
    }
}
