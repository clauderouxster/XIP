/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * InputEditor.java
 *
 * Created on May 23, 2006, 9:40 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.ui.editor;

import com.xerox.xrce.xipui.CorpusElement;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.RunManager;

/**
 *
 * @author vgrassau
 */
public class InputEditor extends DefaultEditor {

    /** Creates a new instance of InputEditor
     * @param context
     * @param ce 
     */
    public InputEditor(ModuleContext context, CorpusElement ce) {
        super(context, ce.getEncoding(), false, false, true);
        setFile(ce.getFile());
        editFile();
        setEditable(true);
        setToolBarVissible(true);
        setClosable(false);
        setSavable(false);
        addFocusListener(new java.awt.event.FocusAdapter() {

            public void focusGained(java.awt.event.FocusEvent evt) {
                formFocusGained(evt);
            }
        });
        text.addFocusListener(new java.awt.event.FocusAdapter() {

            public void focusGained(java.awt.event.FocusEvent evt) {
                formFocusGained(evt);
            }
        });
    }

    private void formFocusGained(java.awt.event.FocusEvent evt) {
        reloadMenu();
        RunManager runner = (RunManager) getProperty(module_run);
        if (runner != null) {
            runner.setFromInput(true);
        }
    }
}
