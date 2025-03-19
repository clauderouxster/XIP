/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * ViewerSelectorGraph.java
 *
 * Created on March 7, 2006, 10:51 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.ide.graph;

import com.xerox.jxip.*;

import com.xerox.xrce.xipui.ide.generator.CommonMethod;
import com.xerox.xrce.xipui.ide.generator.ui.GeneralPanel;
import com.xerox.xrce.xipui.ui.graph.*;
import com.xerox.xrce.xipui.ui.Viewer;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;

import java.util.*;
import java.awt.event.*;
import java.awt.*;
import javax.swing.*;
import javax.swing.tree.*;

/**
 *
 * @author vgrassau
 */
public class ViewerSelectorGraph extends Viewer {

    Vector vecfocus;
    Vector veckeep;
    CommonMethod cmethod;
    private int id;

    /**
     * Creates a new instance of ViewerSelectorGraph
     * @param properties
     * @param id 
     */
    public ViewerSelectorGraph(ModuleContext properties, int id) {
        super(properties);
        this.id = id;
    }

    protected void constructTree() {
        SessionGraph session = getModuleContext().getSessionGraph(false, id);
        Vector v = session.getNodeGraphSelected();
        if (v != null) {
            Iterator ite = v.iterator();
            DefaultMutableTreeNode top = new DefaultMutableTreeNode("Selection");
            while (ite.hasNext()) {
                NodeGraph ng = (NodeGraph) ite.next();
                DefaultMutableTreeNode node = new DefaultMutableTreeNode(ng);
                Vector selectedFeatures = ng.getSelectedFeatures();
                Iterator ite2 = selectedFeatures.iterator();
                while (ite2.hasNext()) {
                    node.add(new DefaultMutableTreeNode(ite2.next()));
                }
                top.add(node);
            }
            tree = new JTree(top);
            tree.setCellRenderer(new TreeCellsRenderer());
            tree.expandRow(0);
        }

    }

    public void refresh() {
        super.refresh();
//        GenerateRulePanel gRulePan = new GenerateRulePanel(getModuleContext(),root,id);
        String typePanel = (String) this.getProperty("panel");
        GeneralPanel b = (GeneralPanel) getProperty("panel_");
        if (b != null) {
            b.generateRule();
        }
    }

    protected void initTreeRenderer() {
    }

    public void showPopup(MouseEvent evt, int selRow, TreePath selPath) {
        popup.removeAll();
        final TreePath treep = selPath;
        final int treer = selRow;
        if (selRow == -1 || selPath.equals(null)) {
            selRow = tree.getRowForLocation(evt.getX(), evt.getY());
            selPath = tree.getPathForLocation(evt.getX(), evt.getY());
        }
        vecfocus = (Vector) this.getProperty("vectFocusSelect");
        veckeep = (Vector) this.getProperty("vectKeepNodeGraph");
        cmethod = new CommonMethod();
        JMenuItem item1 = new JMenuItem("Add a focus");
        item1.addActionListener(new ActionListener() {

            public void actionPerformed(ActionEvent evt) {
                System.out.println("selPath : " + treep.toString());
                System.out.println("treer (selRow) : " + treer);
                TreePath[] paths = tree.getSelectionPaths();
                if (paths == null) {
                    tree.setSelectionPath(treep);
                    paths = tree.getSelectionPaths();
                }
                if (paths != null) {
                    for (int i = 0; i < paths.length; i++) {
                        DefaultMutableTreeNode node = (DefaultMutableTreeNode) paths[i].getLastPathComponent();
                        if (node != null) {
                            NodeGraph nodegraph = (NodeGraph) node.getUserObject();
                            long indexNode = nodegraph.getIndex();
                            if (cmethod.isFocus(nodegraph, vecfocus)) {
                                warning("choose an another node");
                            } else {
                                vecfocus.add(indexNode);
                                System.out.println("index NodeGraph = " + indexNode);
                                DEBUG_LOG.trace("  .... ");
                                repaint();
                                refresh();
                            }
                        }
                    }
                }
            }
        });
        JMenuItem item2 = new JMenuItem("Delete focus");
        item2.addActionListener(new ActionListener() {

            public void actionPerformed(ActionEvent evt) {
                TreePath[] paths = tree.getSelectionPaths();
                if (paths == null) {
                    tree.setSelectionPath(treep);
                    paths = tree.getSelectionPaths();
                }
                if (paths != null) {
                    for (int i = 0; i < paths.length; i++) {
                        DefaultMutableTreeNode node = (DefaultMutableTreeNode) paths[i].getLastPathComponent();
                        if (node != null) {
                            NodeGraph nodegraph = (NodeGraph) node.getUserObject();
                            long indexNode = nodegraph.getIndex();
                            vecfocus.remove(indexNode);
                            System.out.println("index NodeGraph = " + indexNode);
                            repaint();
                            refresh();
                        }
                    }
                }
            }
        });
        this.setProperty("vectFocusSelect", vecfocus);
        JMenuItem item3 = new JMenuItem("Add keep node");
        item3.addActionListener(new ActionListener() {

            public void actionPerformed(ActionEvent evt) {
                TreePath[] paths = tree.getSelectionPaths();
                if (paths == null) {
                    tree.setSelectionPath(treep);
                    paths = tree.getSelectionPaths();
                }
                if (paths != null) {
                    for (int i = 0; i < paths.length; i++) {
                        DefaultMutableTreeNode node = (DefaultMutableTreeNode) paths[i].getLastPathComponent();
                        if (node != null) {
                            NodeGraph nodegraph = (NodeGraph) node.getUserObject();
                            long indexNode = nodegraph.getIndex();
                            if (cmethod.isKeep(indexNode, veckeep)) {
                                warning("choose an another node");
                            } else {
                                veckeep.add(nodegraph);
                                System.out.println("index NodeGraph = " + indexNode);
                                repaint();
                                refresh();
                            }
                        }
                    }
                }
            }
        });
        JMenuItem item4 = new JMenuItem("Delete keep node");
        item4.addActionListener(new ActionListener() {

            public void actionPerformed(ActionEvent evt) {
                TreePath[] paths = tree.getSelectionPaths();
                if (paths == null) {
                    tree.setSelectionPath(treep);
                    paths = tree.getSelectionPaths();
                }
                if (paths != null) {
                    for (int i = 0; i < paths.length; i++) {
                        DefaultMutableTreeNode node = (DefaultMutableTreeNode) paths[i].getLastPathComponent();
                        if (node != null) {
                            NodeGraph nodegraph = (NodeGraph) node.getUserObject();
                            long indexNode = nodegraph.getIndex();
                            veckeep.remove(nodegraph);
                            System.out.println("index NodeGraph = " + indexNode);
                            repaint();
                            refresh();
                        }
                    }
                }
            }
        });
        this.setProperty("vectKeepNodeGraph", veckeep);
        String typePanel = (String) this.getProperty("panel");
        if (typePanel.equals("focus")) {
            popup.add(item1);
            popup.add(item2);
        } else if (typePanel.equals("keep")) {
            popup.add(item3);
            popup.add(item4);
        }
        popup.setVisible(true);
        popup.show(this, evt.getX(), evt.getY());

    }

    public void close() {
        removeAll();
        getRootComponent().remove(this);
    }

    public void editView(int selRow, TreePath selPath) {
    }

    public void initPopup() {
    }

    public class TreeCellsRenderer implements TreeCellRenderer {

        public TreeCellsRenderer() {
        }

        public Component getTreeCellRendererComponent(JTree tree, Object value, boolean selected,
                boolean expanded, boolean leaf, int row, boolean hasFocus) {
            Object object = null;
            JLabel label = new JLabel();
            label.setFont(UIConstants.DEFAULT_FONT);
            label.setPreferredSize(getSize());
            label.setOpaque(true);
            if (selected) {
                label.setBackground(Color.lightGray);
                label.setForeground(Color.red);
            } else {
                label.setBackground(Color.white);
                label.setForeground(Color.black);
            }
            if (value instanceof DefaultMutableTreeNode) {
                DefaultMutableTreeNode node = (DefaultMutableTreeNode) value;
                object = node.getUserObject();
                if (node.isRoot()) {
                    label.setText(object.toString());
                    label.setIcon(XIPUIUtilities.getIcon(UIConstants.RULE_CLASS_LIST));
                    return label;
                }
            }
            if (object instanceof NodeGraph) {
                NodeGraph ng = (NodeGraph) object;
                label.setText(object.toString());
                // si ng appartient a la liste des focus ou keep -> label.setFont(...);
                CommonMethod cmethod = new CommonMethod();
                Vector vecfocus = (Vector) getModuleContext().getProperty("vectFocusSelect");
                Vector veckeep = (Vector) getModuleContext().getProperty("vectKeepNodeGraph");
                long indNG = ng.getIndex();
                if (vecfocus != null && (cmethod.isFocus(ng, vecfocus) || cmethod.isKeep(indNG, veckeep))) {
                    label.setFont(new Font("Times new Roman", Font.ITALIC, 11));
                } else if (ng.isXipNode()) {
                    label.setIcon(XIPUIUtilities.getIcon(UIConstants.CATEGORY));
                } else {
                    label.setIcon(XIPUIUtilities.getIcon(UIConstants.DEPENDENCY));
                }
                return label;
            } else if (object instanceof XipFeature) {
                XipFeature feat = (XipFeature) object;
                label.setText(feat.getName() + ":" + feat.getValue());
                label.setIcon(XIPUIUtilities.getIcon(UIConstants.SELECTED_FEATURE));
                return label;
            }
            if (value != null) {
                return new JLabel(value.toString());
            }
            return new JLabel();
        }
    }
}
