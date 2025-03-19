/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * AddCorpusSetAction.java
 *
 * Created on June 28, 2006, 10:03 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.ui.corpus;

import com.xerox.xrce.xipui.modules.ActionModule;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;
import java.awt.event.*;

/**
 *
 * @author vgrassau
 */
public class AddCorpusSetAction extends ActionModule {

    /** Creates a new instance of AddCorpusSetAction
     * @param context 
     */
    public AddCorpusSetAction(ModuleContext context) {
        super(context, XIPUIUtilities.getApplicationResource("item.new.corpus"),
                XIPUIUtilities.getIcon(UIConstants.CORPUS_OPEN));
    }

    public void actionPerformed(ActionEvent evt) {
        AddCorpusStepManager cm = new AddCorpusStepManager(getModuleContext());
        cm.setVisible(true);
    }
}//end of class
