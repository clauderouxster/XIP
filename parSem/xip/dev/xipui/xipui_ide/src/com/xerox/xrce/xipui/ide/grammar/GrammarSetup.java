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
package com.xerox.xrce.xipui.ide.grammar;

import com.xerox.xrce.xipui.ui.Viewer;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;

import com.xerox.xrce.xipui.ui.editor.EditFileAction;
import com.xerox.xrce.xipui.Grammar;
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.Specialisation;
import com.xerox.xrce.xipui.Specialisations;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.UIConstants;

import java.util.*;
import javax.swing.*;
import javax.swing.tree.*;
import java.awt.event.*;
import java.io.*;

/**
 * Cette classe represente la grammaire selon l'arborescence physique des fichier qui la composent.
 *
 * @author  Vianney Grassaud
 */
public class GrammarSetup extends Viewer {

    public final String TITLE = "Setup";

    public GrammarSetup(ModuleContext properties) {
        super(properties);
    }

    public GrammarSetup(ModuleContext properties, String name, Icon icon) {
        super(properties, name, icon);
    }

    public void close() {
    }

    protected void initTreeRenderer() {
        treeRenderer = new ViewerRenderer();
        ((ViewerRenderer) treeRenderer).setLeafIcon(XIPUIUtilities.getIcon(UIConstants.CONF_FILE));
        ((ViewerRenderer) treeRenderer).setExpandedIcon(XIPUIUtilities.getIcon(UIConstants.CONF_FILE));
        ((ViewerRenderer) treeRenderer).setNodeIcon(XIPUIUtilities.getIcon(UIConstants.CONF_FILE));
    }

    protected void constructTree() {
        try {
            DefaultMutableTreeNode node = null;
            node = new DefaultMutableTreeNode(this.TITLE);
            //Ajout du fichier grm :
            Grammar gr = getProject().getGrammar();
            String grmPath = getProject().getFullpathGrm();
            node.add(new DefaultMutableTreeNode(new File(grmPath)));
            //Ajout du fichier de configuration de la grammaire
            node.add(new DefaultMutableTreeNode(gr.getGrammarFile()));
            Specialisations v = gr.getScripts();
            Enumeration enu = v.getElements();
            while (enu.hasMoreElements()) {
                node.add(new DefaultMutableTreeNode(enu.nextElement()));
            }
            tree = new JTree(node);
            tree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
            tree.setCellRenderer(treeRenderer);
            tree.setRootVisible(false);

        } catch (Exception e) {
            log.error("error", e);
            warning("Error could not display the setup files");
        }
    }

    public void editView(int selRow, TreePath selPath) {
        try {
            Project project = getProject();
            DefaultMutableTreeNode node = (DefaultMutableTreeNode) selPath.getLastPathComponent();
            if (node == null) {
                return;
            }
            Object nodeInfo = node.getUserObject();
            if (node.isLeaf()) {
                Object nodeObject = node.getUserObject();
                File f = null;
                if (nodeObject instanceof File) {
                    f = (File) nodeObject;
                } else if (nodeObject instanceof Specialisation) {
                    Specialisation s = (Specialisation) nodeObject;
                    f = s.getFile();
                }
                if (f != null && f.isFile()) {
//                    if (f.getName().endsWith("grm")) {
//                        GrmEditor grme = new GrmEditor(getModuleContext(),project.getGrammarEncoding());                      
//                        JTabbedPane target = (JTabbedPane)getProperty(CENTER_TABBED);
//                        XIPUIUtilities.addComponentOnTab(target,grme,XIPUIUtilities.getIcon(UIConstants.SETUP),f.getName(),f.getAbsolutePath());                   
//                    } else {
                    editFileAction.setFile(f, project.getGrammarEncoding());
                    editFileAction.putValue(EditFileAction.ICON, XIPUIUtilities.getIcon(UIConstants.SETUP));
                    editFileAction.executeWithoutUI();
//                    }
                }
            }
        } catch (Exception ex) {
            severe("ERROR", ex);
        }
    }

    public void showPopup(MouseEvent evt, int selRow, TreePath selPath) {
    }

    protected void initPopup() {
    }
}
