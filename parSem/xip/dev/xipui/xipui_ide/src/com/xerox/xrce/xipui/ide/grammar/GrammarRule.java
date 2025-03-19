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

import com.xerox.jxip.RuleLoaded;
import com.xerox.xrce.xipui.ui.Viewer;
import com.xerox.xrce.xipui.ui.editor.EditFileAction;
import com.xerox.xrce.xipui.Grammar;
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.RuleMap;
import com.xerox.xrce.xipui.RunContext;
import com.xerox.xrce.xipui.Specialisation;
import com.xerox.xrce.xipui.Specialisations;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;
import java.awt.Color;
import java.awt.Component;



import java.util.*;

import javax.swing.*;
import javax.swing.tree.*;

import java.awt.event.*;

/**
 * Cette classe represente la grammaire selon l'arborescence physique des fichiers qui la composent.
 *
 * @author  Vianney Grassaud
 */
public class GrammarRule extends Viewer {

    public static String type = "Rules";
    private JCheckBoxMenuItem itemLimit;
    private Specialisation specSelected;

    public GrammarRule(ModuleContext properties) {
        super(properties);
        editFileAction.setType(EditFileAction.GRAMMAR_EDITOR);
        itemLimit = new JCheckBoxMenuItem("Put a rule limit to this file", XIPUIUtilities.getIcon(UIConstants.GRAMMAR));
        itemLimit.setToolTipText("Fix the last rule of this file as a limit to reach");
        itemLimit.addActionListener(new ActionListener() {

            public void actionPerformed(ActionEvent evt) {
                if (itemLimit.isSelected()) {
                    setRuleLimit(specSelected);
                } else {
                    unsetRuleLimit();
                }
                repaint();
            }
        });
        popup.add(itemLimit);
    }

    public GrammarRule(ModuleContext properties, String name, Icon icon) {
        this(properties);
        setName(name);
        setIcon(icon);
    }

    public void close() {
    }

    protected void initTreeRenderer() {
        treeRenderer = new RuleRenderer();
        ((ViewerRenderer) treeRenderer).setLeafIcon(XIPUIUtilities.getIcon(UIConstants.CODE));
        ((ViewerRenderer) treeRenderer).setExpandedIcon(XIPUIUtilities.getIcon(UIConstants.CODE));
        ((ViewerRenderer) treeRenderer).setNodeIcon(XIPUIUtilities.getIcon(UIConstants.CODE));
    }

    protected void initPopup() {
        //popup.removeAll();
        //popup.add(itemLimit);
    }

    /**
     * Creer l'arbre affichant les regles du projet
     */
    protected void constructTree() {
        Project project = getProject();
        Grammar grammar = project.getGrammar();
        Specialisations spec = grammar.getRules();
        DefaultMutableTreeNode top = new DefaultMutableTreeNode(type);
        DefaultMutableTreeNode node = null;
        Iterator<Specialisation> ite = spec.iterator();
        while (ite.hasNext()) {
            node = new DefaultMutableTreeNode(ite.next());
            top.add(node);
        }
        // Creation de l'arbre
        tree = new JTree(top);
        tree.setCellRenderer(treeRenderer);
        tree.setRootVisible(false);
    }

    /**
     *
     * @param selRow
     * @param selPath
     */
    public void editView(int selRow, TreePath selPath) {
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

    private void setRuleLimit(Specialisation s) {
        if (s != null) {
            Project project = getProject();
            RuleMap ruleMap = project.getRuleMap();
            Vector<RuleLoaded> rules = ruleMap.getRulesFromFile(s.getFile().getName());
            try {
                RuleLoaded rule = rules.firstElement();
                log.info("rule number for limit : " + rule.getRuleNb());
                project.getRunContext().setGrammarLimit(rule.getRuleNb());
            } catch (Exception e) {
                severe("ERROR", e);
            }
        } else {
            log.info("debug : no specialisation file selected");
        }

    }

    private void unsetRuleLimit() {
        try {
            getProject().getRunContext().unsetGrammarLimit();
        } catch (Exception e) {
            error("ERROR : " + e);
        }
    }

    public void showPopup(MouseEvent evt, int selRow, TreePath selPath) {
        if ((selPath != null) && (selRow != -1)) {
            tree.setSelectionPath(selPath);
            DefaultMutableTreeNode node = (DefaultMutableTreeNode) selPath.getLastPathComponent();
            Object value = node.getUserObject();
            if (value instanceof Specialisation) {
                if (!value.equals(specSelected)) {
                    this.itemLimit.setSelected(false);
                }
                this.specSelected = (Specialisation) value;
            }
        }

        popup.setVisible(true);
        popup.show(tree, evt.getX(), evt.getY());
    }

    private class RuleRenderer extends ViewerRenderer {

        @Override
        public Component getTreeCellRendererComponent(JTree tree, Object value, boolean selected, boolean expanded, boolean leaf, int row, boolean hasFocus) {
            Component c = super.getTreeCellRendererComponent(tree, value, selected, expanded, leaf, row, hasFocus);
            if ((c instanceof JLabel) && (value instanceof DefaultMutableTreeNode)) {
                try {
                    Object userObject = ((DefaultMutableTreeNode) value).getUserObject();
                    if (userObject instanceof Specialisation) {
                        RuleMap ruleMap = getProject().getRuleMap();
                        RunContext runContext = getProject().getRunContext();
                        Specialisation s = (Specialisation) userObject;
                        if (ruleMap.isAFileLimit(runContext.getGrammarLimit(), s.getFile())) {
                            ((JLabel) c).setForeground(Color.BLUE);
                            ((JLabel) c).setIcon(XIPUIUtilities.getIcon(UIConstants.GRAMMAR));
                        }
                    }
                } catch (Exception e) {
                    // DEBUG_LOG.error(e);
                }
            }
            return c;
        }
    }
}



