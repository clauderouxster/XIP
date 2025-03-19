/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * LoadProfileTask.java
 *
 * Created on November 10, 2006, 5:25 PM
 *
 * To change this template, choose Tools | Template AbstractStepManager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.task;

import com.xerox.xrce.xipui.ui.AbstractStepManager;
import com.xerox.xrce.xipui.ProfileManager;
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.ProjectFactory;
import com.xerox.xrce.xipui.ProjectReferenced;
import com.xerox.xrce.xipui.Xip;
import com.xerox.xrce.xipui.modules.IModule;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.modules.TaskModule;
import com.xerox.xrce.xipui.ui.AbstractProfile;
import java.io.*;
import java.util.Hashtable;
import java.util.concurrent.ExecutionException;
import javax.swing.*;

/**
 *
 * @author vgrassau
 */
public class LoadProfileTask extends TaskModule<Project, Void> {

    private ProjectReferenced reference;
    private ModuleContext initialContext;
    private JDesktopPane mainPanel = null;

    /** Creates a new instance of LoadProfileTask
     * @param context 
     */
    public LoadProfileTask(ModuleContext context) {
        super(context);
        reference = (ProjectReferenced) getProperty("reference");
        initialContext = (ModuleContext) getProperty("initialContext");
        mainPanel = (JDesktopPane) initialContext.getProperty(IModule.module_root);
        setIndeterminate(false);
    }

    @Override
    public void done() {
        Project project = null;
        try {
            project = get();
        } catch (InterruptedException ex) {
            log.error("ERROR", ex);
        } catch (ExecutionException ex) {
            log.error("ERROR", ex);
        }

        ProfileManager profileManager = ProfileManager.getExistedInstance();
        AbstractProfile gui = profileManager.createProfile(reference.getProfile(), project, initialContext);
        if (gui != null) {
            if (gui.getRootComponent() == null) {
                gui.setRootComponent(mainPanel);
                gui.setProperty(IModule.ROOT_MAINPANEL, mainPanel);
            }
            mainPanel.add(gui, JLayeredPane.DEFAULT_LAYER);
            gui.setActive(true);
            try {
                gui.setMaximum(true);
            } catch (Exception e) {
            }
            gui.load();
        }
    }

    @Override
    public Project doInBackground() {
        //creer le projet
        setMessage("Creating project " + reference.getName() + " with profile : " + reference.getProfile());
        File projectXML = new File(reference.getProjectPath() + "/" + Project.FILE);
        ProjectFactory pf = null;
        Project project = null;
        Xip xip = (Xip) initialContext.getProperty(xip_parser);
        if (projectXML != null) {
            pf = ProjectFactory.getInstance(projectXML, xip);
        } else {
            Hashtable p = (Hashtable) initialContext.getProperty(AbstractStepManager.manager_properties);
            pf = ProjectFactory.getInstance(p, xip);
        }
        setProgress(50);
        if (pf != null) {
            setMessage("Create the project");
            project = pf.createProject();
        }
        setProgress(100);
        return project;
    }
}
