/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * IDEMain.java
 *
 * Created on February 5, 2007, 3:00 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.ide;

import com.xerox.xrce.xipui.task.TaskManager;
import javax.swing.*;

/**
 *
 * @author vgrassau
 */
public class IDEMain {

    /** Creates a new instance of IDEMain */
    public IDEMain() {
    }

    public static void main(String args[]) {
        SwingUtilities.invokeLater(new Runnable() {

            public void run() {
                TaskManager manager = TaskManager.getInstance();
                manager.loadXipUI(new DeliveryProfileListing());
            }
        });
    //System.out.println("****** Copyright (c) 2007 Xerox Corporation  ******");
    //System.out.println("***** Xerox Research Centre Europe - Grenoble *****");
    //System.out.println("******************* XipUI *************************");  
    }
}
