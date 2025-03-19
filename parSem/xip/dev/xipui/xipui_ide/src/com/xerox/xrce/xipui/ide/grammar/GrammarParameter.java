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

import com.xerox.xrce.xipui.ui.editor.EditFileAction;
import com.xerox.xrce.xipui.ui.Viewer;

import com.xerox.xrce.xipui.Grammar;
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.Specialisation;
import com.xerox.xrce.xipui.Specialisations;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;
import java.util.*;
import javax.swing.*;
import javax.swing.tree.*;
import java.awt.event.*;

/**
 * Cette classe represente la grammaire selon l'arborescence physique des fichier qui la composent.
 *
 * @author  Vianney Grassaud
 */
public class GrammarParameter extends Viewer {

    public GrammarParameter(ModuleContext properties) {
        super(properties);
        editFileAction.setType(EditFileAction.GRAMMAR_EDITOR);
    }

    public void close() {
    }

    public GrammarParameter(ModuleContext properties, String name, Icon icon) {
        this(properties);
        setName(name);
        setIcon(icon);
    }

    protected void initTreeRenderer() {
        treeRenderer = new ViewerRenderer();
        ((ViewerRenderer) treeRenderer).setLeafIcon(XIPUIUtilities.getIcon(UIConstants.CODE));
        ((ViewerRenderer) treeRenderer).setExpandedIcon(XIPUIUtilities.getIcon(UIConstants.CODE));
        ((ViewerRenderer) treeRenderer).setNodeIcon(XIPUIUtilities.getIcon(UIConstants.CODE));
    }

    /***
     *Creer l'arbre affichant les scripts du projet
     */
    protected void constructTree() {
        Project project = getProject();
        Grammar grammar = project.getGrammar();
        Specialisations spec = grammar.getParameters();
        DefaultMutableTreeNode top = new DefaultMutableTreeNode("Parameters");
        DefaultMutableTreeNode node = null;
        Iterator<Specialisation> ite = spec.iterator();
        while (ite.hasNext()) {
            node = new DefaultMutableTreeNode(ite.next());
            top.add(node);
        }
        //Creation de l'arbre
        tree = new JTree(top);
        tree.setCellRenderer(treeRenderer);
        tree.setRootVisible(false);

    }

    protected void initPopup() {
    }

    public void editView(int rowSel, TreePath selPath) {
        DefaultMutableTreeNode node = (DefaultMutableTreeNode) selPath.getLastPathComponent();
        if (node == null) {
            return;
        }
        Object nodeInfo = node.getUserObject();
        if (node.isLeaf()) {
            Specialisation s = (Specialisation) node.getUserObject();
            editFileAction.setFile(s.getFile());
            editFileAction.putValue(EditFileAction.ICON, XIPUIUtilities.getIcon(UIConstants.CODE));
            editFileAction.executeWithoutUI();
        }
    }

    public void showPopup(MouseEvent evt, int selRow, TreePath selPath) {
    }
}
