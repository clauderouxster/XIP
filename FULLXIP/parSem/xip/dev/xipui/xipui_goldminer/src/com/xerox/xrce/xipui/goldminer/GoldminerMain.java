/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 *
 * GoldminerMain.java
 *
 * Created on February 2, 2007, 3:30 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer;

import com.xerox.xrce.xipui.task.TaskManager;
import javax.swing.SwingUtilities;

/**
 *
 * @author vgrassau
 */
public class GoldminerMain {

    /** Creates a new instance of DevMain */
    public GoldminerMain() {
    }

    static public void main(String[] arg) {
        SwingUtilities.invokeLater(new Runnable() {

            public void run() {
                TaskManager manager = TaskManager.getInstance();
                manager.loadXipUI(new GoldminerProfileListing());
            }
        });
    }
}
