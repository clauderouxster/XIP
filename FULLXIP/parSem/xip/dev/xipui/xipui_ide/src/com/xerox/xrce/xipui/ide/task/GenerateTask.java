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
package com.xerox.xrce.xipui.ide.task;

import com.xerox.xrce.xipui.ide.ui.*;
import com.xerox.xrce.xipui.ui.RunManager;
import com.xerox.xrce.xipui.modules.ModuleContext;

import java.util.*;

/**
 *
 * @author vgrassau
 */
public class GenerateTask extends DebugTask {

    /** Creates a new instance of GenerateTask
     * @param context 
     */
    public GenerateTask(ModuleContext context) {
        super(context);
    }

    @Override
    protected void parsingDone(Collection<DebugElement> elements) {
        RunManager runner = (RunManager) getProperty(module_run);
        if (elements != null && !elements.isEmpty()) {
            Generator generator = new Generator(getModuleContext(), idResult);
            generator.setListCorpusElement(corpus); // construction de l'arbre.
            generator.init(elements);
            runner.addResult(generator);
            generator.requestFocusInWindow();
        } else {
            warning("No sentences for the generator");
        }
        runner.setEnabledAction(GenerateAction.NAME_, true);
    }
}
