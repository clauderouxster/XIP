/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * IDEInternProfile.java
 *
 * Created on March 26, 2007, 2:38 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.ide.ui;

import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.*;
import javax.swing.*;

/**
 * The ide intern profile
 * Add some intern action as Copy a Grammar, crypte a grammar
 * @author vgrassau
 */
public class IDEInternProfile extends IDEProfile {

    private Action copyEncrypte;
    private Action decrypte;

    /** Creates a new instance of IDEInternProfile
     * @param context
     * @param p 
     */
    public IDEInternProfile(ModuleContext context, Project p) {
        super(context, p);
    }

    @Override
    protected void initMenuBar() {
        super.initMenuBar();
        initActions();
        JMenuItem itemCopy = XIPUIUtilities.createJMenuItem("Copy/Encrypte", "Copy, encrypte the grammar", null);
        itemCopy.setAction(copyEncrypte);
        JMenuItem itemDecrypte = XIPUIUtilities.createJMenuItem("Decrypte", "Decrypte the grammar", null);
        itemDecrypte.setAction(decrypte);
        this.grammarMenu.add(itemCopy, 0);
        this.grammarMenu.add(itemDecrypte, 1);
        this.grammarMenu.add(new JSeparator(), 2);
    }

    private void initActions() {
        copyEncrypte = new CopyEncrypteAction(getModuleContext());
        decrypte = new DecrypteAction(getModuleContext());
    }
}
