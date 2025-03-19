/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * ReloadGrammarTask.java
 *
 * Created on November 7, 2006, 4:41 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.ide.task;

import com.xerox.xrce.xipui.task.*;
import com.xerox.xrce.xipui.ide.grammar.GrammarEdit;
import com.xerox.xrce.xipui.modules.ModuleContext;

/**
 *
 * @author vgrassau
 */
public class ReloadGrammarTask extends LoadProjectTask {

    /** Creates a new instance of LoadProjectTask
     * @param context 
     */
    public ReloadGrammarTask(ModuleContext context) {
        super(context);
    }

    @Override
    public void done() {
        super.done();
        GrammarEdit gEdit = (GrammarEdit) getProperty(module_grammar);
        if (gEdit != null) {
            gEdit.reload();
        }
    }
}
