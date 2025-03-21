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
package com.xerox.xrce.xipui.ui;

import com.xerox.xrce.xipui.Grammar;
import com.xerox.xrce.xipui.Grm;
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.ProjectFactory;
import com.xerox.xrce.xipui.Xip;
import com.xerox.xrce.xipui.modules.ModuleContext;
import java.io.*;

/**
 *
 * @author  vgrassau
 */
public class ProjectStepManager extends AbstractStepManager {

    /**
     *Creates the project manager.
     *@param properties the ModuleContext to create a project
     */
    public ProjectStepManager(ModuleContext properties) {
        super(properties, 2); //construction du manager
        initProperties();//proprietes par defaut pour construire un projet
        steps[0] = new InitProjectStep(properties, managerProperties);
        steps[0].initStepManager();
        steps[1] = new GrmFileStep(properties, managerProperties);
        initComponents(); //composants graphiques pour le manager
        loadMode(); //chargement selon un mode
    }

    // default properties for a project, this properties are usefull for the projectfactory
    private void initProperties() {
        this.managerProperties.put(ProjectFactory.NAME, "NewProject");
        this.managerProperties.put(ProjectFactory.VERSION, "1.0");
        String path = (String) getProperty(project_directory);
        File directory = new File(path);
        if (!directory.exists()) {
            directory.mkdir();
        }
        this.managerProperties.put(ProjectFactory.FOLDER, path);
        this.managerProperties.put(ProjectFactory.COMMENT, "Some comment");
        this.managerProperties.put(ProjectFactory.AUTHOR, System.getProperty("user.name"));
    }

    /**
     *Inits the GUI for this manager
     */
    private void initComponents() {
        setTitle("Project manager");
        entryPanel.add(steps[0]);
        initDescription(0, "label.project.new.description", steps[0].getDescription());
        initDescription(1, "label.project.grm", NEXT_STEP);
    }

    protected boolean finish() {
        boolean success = false;
        // we get the last steps, ist a GrmFileStep
        GrmFileStep stepGrm = (GrmFileStep) steps[steps.length - 1];
        Grm grm = stepGrm.getGrmFile();
        String pathGrm = grm.getAbsolutePath();
        setProperty("GRM_FILE", pathGrm);
        info("Checking grm  : " + pathGrm);
        Xip xip = (Xip) getProperty(xip_parser);
        Grammar grammar = new Grammar(xip);
        try {
            grammar.loadGrammar(pathGrm);
            managerProperties.put(ProjectFactory.GRAMMAR, grammar);
            //we create the project : 
            ProjectFactory pf = ProjectFactory.getInstance(this.managerProperties, (Xip) getProperty(xip_parser));
            Project project = null;
            if (pf != null) {
                project = pf.createProject();
            }
            if (project == null) {
                success = false;
                log.debug("Error in createProject()");
            } else {
                setProject(project);
                success = true;
            }
        } catch (Exception e) {
            severe("Grammar ERROR " + e, e);
            success = false;
        }
        return success;
    }

    public void loadMode() {
    }
}
