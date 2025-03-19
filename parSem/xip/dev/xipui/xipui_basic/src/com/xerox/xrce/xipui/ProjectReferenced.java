/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * ProjectOpened.java
 *
 * Created on November 10, 2006, 1:38 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui;

import java.io.*;

/**
 *
 * @author vgrassau
 */
public class ProjectReferenced {

    private File projectPath;
    private String profile;
    private boolean opened = false;
    private boolean deleted = false;

    /** Creates a new instance of ProjectOpened
     * @param projectPath 
     */
    public ProjectReferenced(String projectPath) {
        this.projectPath = new File(projectPath);
    }

    public boolean isOpened() {
        return this.opened;
    }

    public void setOpened(boolean b) {
        this.opened = b;
    }

    public void setDeleted(boolean b) {
        this.deleted = b;
    }

    public boolean isDeleted() {
        return this.deleted;
    }

    public boolean exists() {
        return projectPath.exists();
    }

    public String getProjectPath() {
        return this.projectPath.getAbsolutePath();
    }

    public File getProjectPathFile() {
        return this.projectPath.getAbsoluteFile();
    }

    public void setProfile(String profile) {
        this.profile = profile;
    }

    public String getProfile() {
        return this.profile;
    }

    public String getName() {
        return this.projectPath.getName();
    }

    public boolean equals(Object o) {
        if (o instanceof ProjectReferenced) {
            ProjectReferenced compare = (ProjectReferenced) o;
            return getProjectPath().equals(compare.getProjectPath());
        } else {
            return getProjectPath().equals(o);
        }
    }

    public String toString() {
        return getName();
    }
}//end of class
