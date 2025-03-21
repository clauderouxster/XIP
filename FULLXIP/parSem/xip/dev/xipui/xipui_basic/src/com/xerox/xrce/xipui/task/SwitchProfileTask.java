/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * LoadProfileTask.java
 *
 * Created on November 10, 2006, 5:25 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.task;

import com.xerox.xrce.xipui.ProfileManager;
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.ProjectReferenced;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.modules.TaskModule;
import com.xerox.xrce.xipui.ui.AbstractProfile;
import javax.swing.*;

/**
 *
 * @author vgrassau
 */
public class SwitchProfileTask extends TaskModule<Void, AbstractProfile> {

    private ProjectReferenced reference;
    private String oldProfile;

    /** Creates a new instance of LoadProfileTask
     * @param context 
     */
    public SwitchProfileTask(ModuleContext context) {
        super(context);
        reference = (ProjectReferenced) getProperty("reference");
        oldProfile = (String) getProperty("old_profile");
    }

    public void process(java.util.List<AbstractProfile> guis) {
        //finir le profile
        AbstractProfile gui = guis.get(0);
        JDesktopPane mainPanel = (JDesktopPane) gui.getRootComponent();
        mainPanel.add(gui, JLayeredPane.DEFAULT_LAYER);
        gui.setActive(true);
        try {
            gui.setMaximum(true);
        } catch (Exception e) {
        }
        //gui.initProfile();
        Project p = gui.getProject();
        gui.setRunnerEnabled((p != null && p.getGhandler() != -1));
    }

    public Void doInBackground() {
        //creer le projet
        SwingUtilities.invokeLater(new Runnable() {

            public void run() {
                setMessage("Profile changing...");
                ProfileManager profileManager = ProfileManager.getExistedInstance();
                AbstractProfile guiOld = profileManager.getProfile(oldProfile, reference.getProjectPath());
                AbstractProfile gui = null;
                if (guiOld != null) {
                    JDesktopPane mainPanel = (JDesktopPane) guiOld.getRootComponent();
                    mainPanel.remove(guiOld);
                    gui = profileManager.createProfile(reference.getProfile(), guiOld.getProject(), guiOld.getModuleContext());
                    publish(gui);
                } else {
                    setMessage("Can't change profile");
                    log.warn("can't change profile");
                }
            }
        });
        return null;
    }
}
