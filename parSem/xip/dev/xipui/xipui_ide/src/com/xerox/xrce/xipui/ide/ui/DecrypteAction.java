/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * DecrypteAction.java
 *
 * Created on March 26, 2007, 3:54 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.ide.ui;

import com.xerox.xrce.xipui.Grammar;
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.modules.ActionModule;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;
import java.awt.event.ActionEvent;
import javax.swing.JDialog;

/**
 *
 * @author vgrassau
 */
public class DecrypteAction extends ActionModule {

    /** Creates a new instance of DebugAction
     * @param context 
     */
    public DecrypteAction(ModuleContext context) {
        super(context, "Decrypte", XIPUIUtilities.getIcon(UIConstants.DECRYPTED));
        setToolTipText("Decrypte the grammar");
    }

    public void actionPerformed(ActionEvent event) {
        Project p = getProject();
        Grammar grammar = p.getGrammar();
        if (!grammar.isCrypted()) {
            warning("Your grammar is not crypted");
        } else {
            JDialog dialog = new JDialog();
            dialog.setModal(true);
            setProperty("dialog", dialog);
            dialog.setContentPane(new EncrypteDecryptePanel(getModuleContext(), false));
            dialog.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);
            dialog.setSize(500, 180);
            dialog.setLocationRelativeTo(null);
            dialog.setVisible(true);
        }
    }
}
