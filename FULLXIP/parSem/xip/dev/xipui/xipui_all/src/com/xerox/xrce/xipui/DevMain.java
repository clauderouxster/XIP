/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 *
 * DevMain.java
 *
 * Created on February 2, 2007, 3:23 PM
 *
 */
package com.xerox.xrce.xipui;

import com.xerox.xrce.xipui.task.TaskManager;
import javax.swing.SwingUtilities;

/**
 *
 * @author vgrassau
 */
public class DevMain {

    /** Creates a new instance of DevMain */
    public DevMain() {
    }

    public static void main(String[] arg) {
        SwingUtilities.invokeLater(new Runnable() {

            public void run() {
                TaskManager manager = TaskManager.getInstance();
                manager.loadXipUI(new DevProfileListing());
            }
        });
    }
}
