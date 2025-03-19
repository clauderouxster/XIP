/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 ****************************************************************************************
 * Copyright (c) 2007 Xerox Corporation                                                 *
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
package com.xerox.xrce.xipui.ide.ui;

import com.xerox.xrce.xipui.ide.task.ParseTask;
import com.xerox.xrce.xipui.modules.ActionModule;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.task.TaskManager;
import com.xerox.xrce.xipui.ui.RunManager;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;

import javax.swing.*;
import java.awt.event.*;

/**
 *
 * @author vgrassau
 */
public class ParseAction extends ActionModule {

    public static final String NAME_ = "Parse";

    /** Creates a new instance of ParseAction
     * @param context 
     */
    public ParseAction(ModuleContext context) {
        super(context, NAME_, XIPUIUtilities.getIcon(UIConstants.PARSE));
        putValue(ACCELERATOR_KEY, KeyStroke.getKeyStroke(KeyEvent.VK_F6, 0, true));
        setToolTipText("Parse (F6)");
    }

    public void actionPerformed(ActionEvent evt) {
        //creer la liste des CorpusElement a parser
        try {
            RunManager runner = (RunManager) getProperty(module_run);
            runner.prepareForRunning();
        } catch (Exception e) {
            log.error("ERROR", e);
        }
        //invoker la parsing
        TaskManager manager = TaskManager.getInstance();
        manager.invoke(new ParseTask(getModuleContext()));
        setEnabled(false);
    }
}
