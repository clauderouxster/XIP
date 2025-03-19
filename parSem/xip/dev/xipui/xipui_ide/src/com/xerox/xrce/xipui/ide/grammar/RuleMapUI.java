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
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.RuleMap;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;
import java.util.*;
import javax.swing.*;
import javax.swing.tree.*;
import java.awt.event.*;
import java.awt.*;

/**
 * Represent le rule map de la regle sous forme d'arbre.
 * @author Vianney Grassaud
 */
public class RuleMapUI extends Viewer {

    public final static String TITLE = "Rule Map";
    public final static int VIEW_TYPE = 0;
    public final static int VIEW_REL_LAYER = 3;
    public final static int VIEW_ABS_LAYER = 4;
    public final static int VIEW_NUM = 2;
    public final static int VIEW_FILE = 1;
    private int type = VIEW_FILE;
    private JToolBar toolbar;
    private Action[] actions;

    /** Creates the viewer for the RuleMap
     *@param properties the ModuleContext to use
     *
     */
    public RuleMapUI(ModuleContext properties) {
        super(properties);
        toolbar = initToolbar();
        this.type = VIEW_FILE;
        editFileAction.setType(EditFileAction.GRAMMAR_EDITOR);
    }

    /**
     *Create the viewer for the rule map
     *@param properties the moduleContext to use
     *@param name for this viewer
     *@param icon the icon to use
     */
    public RuleMapUI(ModuleContext properties, String name, Icon icon) {
        this(properties);
        setName(name);
        setIcon(icon);
    }

    /**
     *Init the rendere for the JTree
     */
    protected void initTreeRenderer() {
        treeRenderer = new RuleMapRenderer();
        ((ViewerRenderer) treeRenderer).setExpandedIcon(XIPUIUtilities.getIcon(UIConstants.RULE_CLASS));
        ((ViewerRenderer) treeRenderer).setRootIcon(XIPUIUtilities.getIcon(UIConstants.CODE));
        ((ViewerRenderer) treeRenderer).setNodeIcon(XIPUIUtilities.getIcon(UIConstants.RULE_CLASS_CLOSED));
        ((ViewerRenderer) treeRenderer).setLeafIcon(XIPUIUtilities.getIcon(UIConstants.RULE));
    }

    /**
     *Init the availables actions
     */
    private void initActions() {
        actions = new Action[5];
        actions[VIEW_TYPE] = createChangeViewAction("Type", "Rule map by type", VIEW_TYPE, XIPUIUtilities.getIcon(UIConstants.RULE_MAP_TYPE));
        actions[VIEW_FILE] = createChangeViewAction("File", "Rule map by file", VIEW_FILE, XIPUIUtilities.getIcon(UIConstants.CODE));
        actions[VIEW_NUM] = createChangeViewAction("Number", "Rule map by number", VIEW_NUM, XIPUIUtilities.getIcon(UIConstants.RULE_MAP_NUM));
        actions[VIEW_REL_LAYER] = createChangeViewAction("Rel layer", "Rule map by relative layer", VIEW_REL_LAYER, XIPUIUtilities.getIcon(UIConstants.RULE_MAP_REL));
        actions[VIEW_ABS_LAYER] = createChangeViewAction("Abs layer", "Rule map by absolute layer", VIEW_ABS_LAYER, XIPUIUtilities.getIcon(UIConstants.RULE_MAP_ABS));
    }

    /**
     *Create an action for change the view of the JTree, ie the rule map by file, by type...
     *@param label the label for the action
     *@param tooltip the description for the action
     *@param int the type of the view
     *@param icon the icon to use for the action
     *@return the Action to use for change a view
     */
    private Action createChangeViewAction(String label, String toolTip, final int view, final Icon icon) {
        Action action = new AbstractAction(label) {

            public void actionPerformed(ActionEvent evt) {
                changeView(evt, view, icon);
            }
        };
        action.putValue(Action.SHORT_DESCRIPTION, toolTip);
        action.putValue(Action.SMALL_ICON, icon);
        return action;
    }

    /**
     *Sets the toolbar for the ruleMap
     *add the actions
     *@return the toolbar
     */
    private JToolBar initToolbar() {
        toolbar = new JToolBar();
        toolbar.setFloatable(false);
        toolbar.add(XIPUIUtilities.createJLabel("Filters :", null, "Choose a filter"));
        for (int i = 0; i < actions.length; i++) {
            toolbar.add(XIPUIUtilities.createJButton("", actions[i], false));
        }
        toolbar.setBorderPainted(false);
        toolbar.setRollover(true);
        toolbar.setMaximumSize(new Dimension(307, 23));
        toolbar.setMinimumSize(new Dimension(307, 23));
        toolbar.setPreferredSize(new Dimension(100, 23));
        return toolbar;
    }

    /**
     *Return the toolbar of this viewer
     *@return toolabr, null if not initialized
     */
    public JToolBar getToolBar() {
        return this.toolbar;
    }

    /**
     *Init the popup menu, this metho call the iniAction method
     */
    protected void initPopup() {
        initActions();
        for (int i = 0; i < actions.length; i++) {
            popup.add(actions[i]);
        }
    }

    /**
     *Change the view of the JTree
     *@param evt the event
     *@param type the new type for the view
     *@param icon the icon to use for the root element in the Jtree
     *
     *calls the loadTree() method
     */
    private void changeView(ActionEvent evt, int type, Icon icon) {
        this.type = type;
        if (treeRenderer != null) {
            ((ViewerRenderer) treeRenderer).setRootIcon(icon);
        }
        loadTree();
    }

    private void disabledAction(int index) {
        if (actions != null) {
            for (int i = 0; i < actions.length; i++) {
                if (i == index) {
                    actions[i].setEnabled(false);
                } else {
                    actions[i].setEnabled(true);
                }
            }
        }
    }

    /**
     *Construct the JTree this method is called by the loadTree() method
     *the JTree is built in function of the specific type.
     */
    protected void constructTree() {
        Project project = getProject();
        RuleMap ruleMap = project.getRuleMap();
        if (ruleMap != null) {
            switch (type) {
                default: {
                    createSorted(ruleMap.getFileTable(), "Files", VIEW_FILE);
                    break;
                }
                case VIEW_TYPE: {
                    createSorted(ruleMap.getTypeTable(), "Types", RuleMapUI.VIEW_TYPE);
                    break;
                }
                case VIEW_REL_LAYER: {
                    createSorted(ruleMap.getLayerRelTable(), "Relative Layers", RuleMapUI.VIEW_REL_LAYER);
                    break;
                }
                case VIEW_ABS_LAYER: {
                    createSorted(ruleMap.getLayerAbsTable(), "Absolute Layers", RuleMapUI.VIEW_ABS_LAYER);
                    break;
                }
                case VIEW_NUM: {
                    createSorted(ruleMap.getNumberTable(), "Number", RuleMapUI.VIEW_NUM);
                    break;
                }
                case VIEW_FILE: {
                    createSorted(ruleMap.getFileTable(), "Files", RuleMapUI.VIEW_FILE);
                    break;
                }
            }
            disabledAction(type);
        } else {
            tree = new JTree(new DefaultMutableTreeNode("No RuleMap"));
            tree.setToolTipText("Maybe the grammar is not loaded  correctly.");
        }
        if (tree != null) {
            tree.setCellRenderer(treeRenderer);
        }
        repaint();
    }

    public void close() {
    }

    /** This method permits to know if the type of the rule map will be
     * printed by the rule numbers, layers number or not
     * @param type the type to use
     * @return true if the rule map will be printed by  "numbers"
     */
    private boolean isRuleOrLayerType(int typeNum) {
        return ((typeNum != VIEW_TYPE) && (typeNum != VIEW_FILE));
    }

    /**
     *Get information from the ruleMap, and sort this information for the JTree
     *@param table the table
     *@param type the root element for the JTree
     */
    private void createSorted(Map<Object, Vector<RuleLoaded>> table, String type, int typeNum) {
        RuleMap ruleMap = getProject().getRuleMap();
        if (ruleMap != null) {
            Vector keys = null;
            if (isRuleOrLayerType(typeNum)) {
                keys = ruleMap.getKeysSorted();
            } else {
                keys = new Vector(table.keySet());
            }
            if (table != null && keys != null) {
                DefaultMutableTreeNode top = new DefaultMutableTreeNode(type);
                Iterator ite = keys.iterator();
                while (ite.hasNext()) {
                    Object key = ite.next();
                    DefaultMutableTreeNode node = new DefaultMutableTreeNode(key);
                    Vector rules = table.get(key);
                    Iterator iteRules = rules.iterator();
                    while (iteRules.hasNext()) {
                        node.add(new DefaultMutableTreeNode(iteRules.next()));
                    }
                    top.add(node);
                }
                tree = new JTree(top);

            }
        }
    }

    /**
     *Edit an element of the JTree
     *@param selRow the selected row
     *@param selPath the  path of the selected row
     */
    public void editView(int selRow, TreePath selPath) {
        try {
            Project project = getProject();
            RuleMap ruleMap = project.getRuleMap();
            DefaultMutableTreeNode node = (DefaultMutableTreeNode) selPath.getLastPathComponent();
            JTabbedPane target = (JTabbedPane) getProperty(CENTER_TABBED);
            if (node == null) {
                return;
            }
            if (node.isLeaf()) {
                RuleLoaded rule = null;
                Object nodeInfo = node.getUserObject();
                if (nodeInfo instanceof RuleLoaded) {
                    rule = (RuleLoaded) nodeInfo;
                }
                editFileAction.setFile(rule.getFile());
                editFileAction.putValue(EditFileAction.CARET, (int) rule.getOffset() + 1);
                editFileAction.putValue(EditFileAction.ICON, XIPUIUtilities.getIcon(UIConstants.RULE_CLASS_LIST));
                editFileAction.executeWithoutUI();
            }
        } catch (Exception ex) {
            log.error("error could not edit object for RuleMap...", ex);
        }

    }

    /***
     * Show the popupmenu
     *@param evt the mouse event
     *@param selRow the row selected
     *@param selPath the path of the selected row
     */
    public void showPopup(MouseEvent evt, int selRow, TreePath selPath) {
        if (MouseEvent.BUTTON3 == evt.getButton()) {
            popup.setVisible(true);
            popup.show(this.tree, evt.getX(), evt.getY());
        }
    }

    /**
     *This method try to check if the first child of the given parent is a
     *RuleLoaded object. If it's the case, then we add to the label the file name
     *containing the RuleLoaded.
     *
     */
    private void printTheRuleLoaded(JLabel label, DefaultMutableTreeNode parent) throws Exception {
        if (parent.getChildCount() >= 1) {
            Object nodeObject = parent.getUserObject();
            if (nodeObject instanceof Number) {
                DefaultMutableTreeNode child = (DefaultMutableTreeNode) parent.getFirstChild();
                Object childValue = child.getUserObject();
                if (childValue instanceof RuleLoaded) {
                    RuleLoaded rule = (RuleLoaded) childValue;
                    label.setText(label.getText() + " : " + rule.getFileName());
                    label.setToolTipText(label.getText());
                }
            }
        }
    }

    private class RuleMapRenderer extends ViewerRenderer {

        @Override
        public Component getTreeCellRendererComponent(JTree tree, Object value, boolean selected, boolean expanded, boolean leaf, int row, boolean hasFocus) {
            Component c = super.getTreeCellRendererComponent(tree, value, selected, expanded, leaf, row, hasFocus);
            if ((c instanceof JLabel) && (value instanceof DefaultMutableTreeNode)) {
                try {
                    printTheRuleLoaded((JLabel) c, (DefaultMutableTreeNode) value);
                } catch (Exception e) {
                    // DEBUG_LOG.error(e);
                }
            }
            return c;
        }
    }
}
