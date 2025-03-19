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
import com.xerox.xrce.xipui.Grammar;
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;

import java.util.*;
import javax.swing.*;
import javax.swing.tree.*;
import java.awt.event.*;

/**
 *
 * @author vgrassau
 */
public class ViewerVariables extends Viewer {

    /** Creates a new instance of ViewerVariables
     * @param properties 
     */
    public ViewerVariables(ModuleContext properties) {
        super(properties);
    }

    public ViewerVariables(ModuleContext properties, String name, Icon icon) {
        super(properties, name, icon);
    }

    protected void initTreeRenderer() {
        treeRenderer = new ViewerRenderer();
        ((ViewerRenderer) treeRenderer).setLeafIcon(XIPUIUtilities.getIcon(UIConstants.VARIABLE));
        ((ViewerRenderer) treeRenderer).setExpandedIcon(XIPUIUtilities.getIcon(UIConstants.VARIABLE));
        ((ViewerRenderer) treeRenderer).setNodeIcon(XIPUIUtilities.getIcon(UIConstants.VARIABLE));
    }

    protected void constructTree() {
        Project p = getProject();
        Grammar gram = p.getGrammar();
        Vector v = gram.getListVariables();
        if (v != null) {
            tree = new JTree(v);
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
            GramVariable gf = null;
            Object nodeInfo = node.getUserObject();
            if (nodeInfo instanceof GramVariable) {
            }
        } catch (Exception ex) {
            severe("ERROR", ex);
        }
    }

    public void showPopup(MouseEvent evt, int selRow, TreePath selPath) {
    }
}
