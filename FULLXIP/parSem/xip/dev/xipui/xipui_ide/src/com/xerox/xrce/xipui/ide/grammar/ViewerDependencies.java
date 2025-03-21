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
public class ViewerDependencies extends Viewer {

    /** Creates a new instance of ViewerDependencies
     * @param properties 
     */
    public ViewerDependencies(ModuleContext properties) {
        super(properties);
        editFileAction.setType(EditFileAction.GRAMMAR_EDITOR);
    }

    public ViewerDependencies(ModuleContext properties, String name, Icon icon) {
        super(properties, name, icon);
        editFileAction.setType(EditFileAction.GRAMMAR_EDITOR);
    }

    protected void initTreeRenderer() {
        treeRenderer = new ViewerRenderer();
        ((ViewerRenderer) treeRenderer).setLeafIcon(XIPUIUtilities.getIcon(UIConstants.DEPENDENCY));
        ((ViewerRenderer) treeRenderer).setExpandedIcon(XIPUIUtilities.getIcon(UIConstants.DEPENDENCY));
        ((ViewerRenderer) treeRenderer).setNodeIcon(XIPUIUtilities.getIcon(UIConstants.DEPENDENCY));
    }

    protected void constructTree() {
        Project p = getProject();
        Grammar gram = p.getGrammar();
        Vector v = gram.getListDependencies();
        if (v != null) {
            tree = new JTree(v);
            ViewerRenderer renderer = new ViewerRenderer();
            renderer.setLeafIcon(XIPUIUtilities.getIcon(UIConstants.DEPENDENCY));
            renderer.setExpandedIcon(XIPUIUtilities.getIcon(UIConstants.DEPENDENCY));
            renderer.setNodeIcon(XIPUIUtilities.getIcon(UIConstants.DEPENDENCY));
            // tree.setCellRenderer(treeRenderer);
            tree.setRootVisible(false);
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
            GramDependency gf = null;
            Object nodeInfo = node.getUserObject();
            if (nodeInfo instanceof GramDependency) {
                gf = (GramDependency) nodeInfo;
                if (gf.getFileName() != null) {
                    File f = new File(gf.getFileName());
                    editFileAction.setFile(f);
                    editFileAction.putValue(EditFileAction.CARET, gf.getFileOffset());
                    editFileAction.putValue(EditFileAction.ICON, XIPUIUtilities.getIcon(UIConstants.DEPENDENCIES_LIST));
                    editFileAction.executeWithoutUI();
                } else {
                    warning(gf.getName() + " is not declared in a file");
                }
            }
        } catch (Exception ex) {
            severe("ERROR", ex);
        }
    }

    public void showPopup(MouseEvent evt, int selRow, TreePath selPath) {
    }
}
