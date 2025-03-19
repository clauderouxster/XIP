/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * ProjectPropertiesAction.java
 *
 * Created on July 6, 2006, 11:41 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.xerox.xrce.xipui.ui;
import com.xerox.xrce.xipui.modules.ActionModule;
import com.xerox.xrce.xipui.modules.ModuleContext;
import javax.swing.*;
import java.awt.event.*;/**
 *
 * @author vgrassau
 */
public class ProjectPropertiesAction extends ActionModule {
    
    /** Creates a new instance of ProjectPropertiesAction */
    public ProjectPropertiesAction(ModuleContext context) {
       super(context,XIPUIUtilities.getApplicationResource("action.properties.project"),
                     XIPUIUtilities.getIcon(UIConstants.PROPERTIES));
               
    }
    
    
    public void actionPerformed(ActionEvent event) {
        ProjectProperties pp = new ProjectProperties(getModuleContext());
        pp.setVisible(true);
    }
}
