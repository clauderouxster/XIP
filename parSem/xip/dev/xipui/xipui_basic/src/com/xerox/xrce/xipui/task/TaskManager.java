/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * TaskManager.java
 *
 * Created on November 7, 2006, 10:25 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.task;

import com.xerox.xrce.xipui.*;
import com.xerox.xrce.xipui.modules.*;
import java.io.*;
import org.apache.log4j.*;
import javax.swing.*;

/**
 *
 * @author vgrassau
 */
public class TaskManager {

    public static enum Command {

        LOAD_XIPUI, LOAD_PROFILE, SWITCH_PROFILE, LOAD_PROJECT, LOAD_DOC
    };
    private TaskProgressBar progressBar;
    private Logger log;

    /** Creates a new instance of TaskManager */
    private TaskManager() {
        // set the look and feel
        if (System.getProperty("os.name").startsWith("Windows")) {
            try {
                String classLook = UIManager.getSystemLookAndFeelClassName();
                UIManager.setLookAndFeel(classLook);
            } catch (Exception e) {
                e.printStackTrace();
                System.err.println("Error, can't load look and feel manager !!!!");
            }
        }
        System.gc();
        progressBar = new TaskProgressBar();
    }

    public TaskProgressBar getTaskProgressBar() {
        return this.progressBar;
    }

    /**
     *Invocation d'une tache particuliere :
     *@param c la command a effectuer
     *@param context le context de module transmis a la tache a effectuer
     */
    public void invoke(Command c, ModuleContext context) {
        TaskModule task = createTask(c, context);
        progressBar.startProcess(task);
    }

    public void invokeWithDialog(Command c, ModuleContext context, boolean wait) {
        TaskModule task = createTask(c, context);
        TaskDialog dialog = new TaskDialog(task, c == Command.LOAD_XIPUI, wait);
        dialog.start();
    }

    public void invokeWithDialog(Command c, boolean wait) {
        invokeWithDialog(c, new ModuleContext(), wait);
    }

    public void invokeWithDialog(Command c, ModuleContext context) {
        invokeWithDialog(c, context, true);
    }

    public void invoke(Command c) {
        invoke(c, new ModuleContext());
    }

    public void invoke(Command c, IModule module) {
        invoke(c, module.getModuleContext());
    }

    public void invoke(TaskModule task) {
        progressBar.startProcess(task);
    }

    public void invokeWithDialog(TaskModule task, boolean modal) {
        TaskDialog dialog = new TaskDialog(task, false, modal);
        dialog.start();
    }

    public void loadXipUI(ProfileListing profiles) {
        loadXipUI(profiles, false);
    }

    /**
     *Load the xipui GUI 
     *@param profiles the profiles to use with XipUI
     *@param console determine if the log will be written on the console or not
     *
     */
    public void loadXipUI(ProfileListing profiles, boolean console) {
        String userName = System.getProperty("user.name");
        String loggingFile = "/com/xerox/xrce/xipui/logging.properties";
        if (userName.equalsIgnoreCase("grondeau") || console) {
            System.out.println("console and log mode");
            loggingFile = "/com/xerox/xrce/xipui/loggingDev.properties";
        }
        InputStream stream = getClass().getResourceAsStream(loggingFile);
        PropertyConfigurator logConfig = new PropertyConfigurator();
        logConfig.doConfigure(stream, LogManager.getLoggerRepository());
        log = Logger.getLogger("module");
        ProfileManager profileManager = ProfileManager.newInstance(profiles);
        invokeWithDialog(TaskManager.Command.LOAD_XIPUI, true);
    }

    private TaskModule createTask(Command c, ModuleContext context) {
        TaskModule task = null;
        switch (c) {
            case LOAD_XIPUI: {
                task = new LoadApplicationTask();
                break;
            }
            case LOAD_PROJECT: {
                task = new LoadProjectTask(context);
                break;
            }
            case LOAD_DOC: {
                task = new LoadDocumentationTask(context);
                break;
            }
            case LOAD_PROFILE: {
                task = new LoadProfileTask(context);
                break;
            }
            case SWITCH_PROFILE: {
                task = new SwitchProfileTask(context);
                break;
            }
        }
        return task;
    }

    private static class SingletonHolder {

        private static TaskManager instance = new TaskManager();
    }

    public static TaskManager getInstance() {
        return SingletonHolder.instance;
    }
}
