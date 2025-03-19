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

import com.xerox.jxip.*;
import com.xerox.xrce.xipui.ui.Viewer;
import com.xerox.xrce.xipui.ui.editor.EditFileAction;
import com.xerox.xrce.xipui.Grammar;
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;

import java.util.*;
import javax.swing.*;
import javax.swing.tree.*;
import java.awt.event.*;
import java.io.*;

/**
 *
 * @author vgrassau
 */
public class ViewerFeatures extends Viewer {

    /**
     * Creates a new instance of ViewerFeatures
     * @param properties 
     */
    public ViewerFeatures(ModuleContext properties) {
        super(properties);
        editFileAction.setType(EditFileAction.GRAMMAR_EDITOR);
    }

    public ViewerFeatures(ModuleContext properties, String name, Icon icon) {
        super(properties, name, icon);
        editFileAction.setType(EditFileAction.GRAMMAR_EDITOR);
    }

    protected void initTreeRenderer() {
        treeRenderer = new ViewerRenderer();
        ((ViewerRenderer) treeRenderer).setExpandedIcon(XIPUIUtilities.getIcon(UIConstants.FEATURE));
        ((ViewerRenderer) treeRenderer).setNodeIcon(XIPUIUtilities.getIcon(UIConstants.FEATURES_LIST));
        ((ViewerRenderer) treeRenderer).setLeafIcon(null);
    }

    private DefaultMutableTreeNode readFeatureTree(Vector v, DefaultMutableTreeNode root) {
        if (v != null) {
            Iterator ite = v.iterator();
            while (ite.hasNext()) {
                GramFeature gf = (GramFeature) ite.next();
                DefaultMutableTreeNode node = new DefaultMutableTreeNode(gf);

                root.add(readFeatureTree(gf.getChild(), node));
            }
        }
        return root;
    }

    protected void constructTree() {
        Project p = getProject();
        Grammar gram = p.getGrammar();
        Vector v = gram.getFeatureMap();
        System.out.println("le feature map : " + v);
        if (v != null) {
            tree = new JTree(readFeatureTree(v, new DefaultMutableTreeNode("features")));
            //tree.setCellRenderer(treeRenderer);
            tree.expandRow(0);
        } else {
            tree = new JTree(new DefaultMutableTreeNode("Grammar not loaded"));
            tree.setToolTipText("Maybe the grammar is not loaded  correctly.");
        }
        tree.setCellRenderer(treeRenderer);
    }

    protected void initPopup() {
    }

    public void close() {
    }

    public void editView(int selRow, TreePath selPath) {
        try {
            DefaultMutableTreeNode node = (DefaultMutableTreeNode) selPath.getLastPathComponent();
            if (node == null) {
                return;
            }
            GramFeature gf = null;
            Object nodeInfo = node.getUserObject();
            if (nodeInfo instanceof GramFeature) {
                gf = (GramFeature) nodeInfo;
                if (gf.getFileName() != null) {
                    File f = new File(gf.getFileName());
                    editFileAction.setFile(f);
                    editFileAction.putValue(EditFileAction.CARET, gf.getFileOffset());
                    editFileAction.putValue(EditFileAction.ICON, XIPUIUtilities.getIcon(UIConstants.FEATURES_LIST));
                    editFileAction.executeWithoutUI();
                } else {
                    warning(gf.getName() + " is not declared in a file");
                }
            }

        } catch (Exception ex) {
            severe("error could not edit object for FeatureMap...", ex);
        }
    }

    public void showPopup(MouseEvent evt, int selRow, TreePath selPath) {
    }
}
