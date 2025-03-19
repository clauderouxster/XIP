/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 ****************************************************************************************
 * Copyright (c) 2005 Xerox Corporation                                                 *
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
package com.xerox.xrce.xipui.ui.corpus;

import com.xerox.xrce.xipui.CorpusSet;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.AbstractStepManager;
import com.xerox.xrce.xipui.ui.Viewer;
import java.io.*;

/**
 *
 * @author Vianney Grassaud
 */
public class AddCorpusStepManager extends AbstractStepManager {

    /**
     * Creates a new instance of AddCorpusStepManager
     * @param properties 
     */
    public AddCorpusStepManager(ModuleContext properties) {
        super(properties, 1);
        this.nextButton.setEnabled(false);
        this.finishButton.setEnabled(true);
        steps[0] = new AddCorpusStep(getModuleContext());
        setTitle("Add a new Corpus");
        initDescription(0, "label.corpus.new.name", steps[0].getDescription());
        stepPanel.add(stepsLabel[0]);
        entryPanel.add(steps[0]);
        loadMode();
    }

    /**
     * Ajoute le corpusSet au projet,
     * par defaut on aura toujours un element pour le corpus nomme input.txt
     */
    protected boolean finish() {
        try {
            AddCorpusStep addcorpus = (AddCorpusStep) steps[0];
            //CorpusSet cs = new CorpusSet(getProject().getFullName(),addcorpus.getCorpusSetName());
            CorpusSet cs = new CorpusSet(getProject().getCorpusDirectory(), addcorpus.getCorpusSetName());
            File f = cs.getDirectory();
            f.mkdir();
            getProject().addCorpusSet(cs);
            getProject().save();
            CorpusManager cm = (CorpusManager) getProperty(module_corpus);
            Viewer viewer = cm.getViewer();
            if (viewer != null) {
                viewer.loadTree();
            }
            return true;
        } catch (Exception e) {
            severe("ERROR finish manager", e);
            return false;
        }
    }

    public void loadMode() {
        //nothing
    }
}
