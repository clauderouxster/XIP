/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * GoldminerResultViewer.java
 *
 * Created on October 17, 2006, 11:58 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer.ui;

import com.xerox.xrce.xipui.ui.Viewer;
import com.xerox.xrce.xipui.goldminer.GoldminerEngine;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;
import com.xerox.xrce.xipui.util.FileTools;

import java.util.*;
import javax.swing.*;
import javax.swing.tree.*;
import java.awt.event.*;
import java.awt.*;
import java.io.*;

/**
 *
 * @author vgrassau
 */
public class GoldminerResultViewer extends Viewer {

    private GoldminerEngine engine;
    private ViewGoldminerResultAction viewAction;
    private boolean searching = false;

    /** Creates a new instance of GoldminerResultViewer
     * @param context 
     */
    public GoldminerResultViewer(ModuleContext context) {
        super(context);
        engine = (GoldminerEngine) getProperty("goldminerEngine");
        viewAction = new ViewGoldminerResultAction(context);
        super.editFileAction = null;
    }

    /**
     *Init the renderer for the tree
     */
    protected void initTreeRenderer() {
        treeRenderer = new TreeCellsRenderer();
    }

    public void setSearching() {
        searching = true;
        DefaultMutableTreeNode top = new DefaultMutableTreeNode("Result");
        DefaultMutableTreeNode node = new DefaultMutableTreeNode("Searching");
        top.add(node);
        //Creation de l'arbre
        tree = new JTree(top);
        tree.setCellRenderer(treeRenderer);
        tree.setRootVisible(true);
        this.setViewTree();
    //tree.repaint();
    //repaint();
    }

    /**
     *Initialize the Popup menu for the viewer
     */
    protected void initPopup() {
    }

    /**
     *Method to edit the element at the selected row
     *@param selRow 
     * @param selPath path of the row selected on the tree
     */
    public void editView(int selRow, TreePath selPath) {
        DefaultMutableTreeNode node = (DefaultMutableTreeNode) selPath.getLastPathComponent();
        JTabbedPane target = (JTabbedPane) getProperty(CENTER_TABBED);
        if (node == null) {
            return;
        }
        if (node.isLeaf()) {
            Object object = node.getUserObject();
            if (object instanceof String) {
                File file = new File(object.toString());
                viewAction.setFile(file, FileTools.ISO_8859_1);
                // viewAction.setFile(file, FileTools.UTF_8);
                viewAction.executeWithoutUI();
            }
        }
    }

    /**
     *Method to show a popupmenu when an user use the right mouse button on the tree
     *@param evt mouse event
     *@param selRow the row selected in the tree
     *@param selPath the paths of the element selected in the tree
     */
    public void showPopup(MouseEvent evt, int selRow, TreePath selPath) {
    }

    /**
     *Method to construct the tree
     *this method is called by loadTree()
     *
     */
    protected void constructTree() {
        Collection<String> c = engine.getFiles();
        DefaultMutableTreeNode top = new DefaultMutableTreeNode("Result");
        DefaultMutableTreeNode node = null;
        if (c != null) {
            searching = false;
            Iterator ite = c.iterator();
            while (ite.hasNext()) {
                node = new DefaultMutableTreeNode(ite.next());
                top.add(node);
            }
        }
        //Creation de l'arbre
        tree = new JTree(top);
        tree.setCellRenderer(treeRenderer);
        tree.setRootVisible(true);
    }

    public void close() {
    }

    public class TreeCellsRenderer implements TreeCellRenderer {

        public TreeCellsRenderer() {
        }

        public Component getTreeCellRendererComponent(JTree tree, Object value, boolean selected, boolean expanded, boolean leaf, int row, boolean hasFocus) {
            Object object = null;
            boolean isEmptyResult = ((engine.getFiles() == null) || (engine.getFiles().size() == 0));
            Component componentRenderer = new JLabel();
            if (value instanceof DefaultMutableTreeNode) {
                DefaultMutableTreeNode node = (DefaultMutableTreeNode) value;
                object = node.getUserObject();
                if (node.isRoot()) {
                    JLabel label = new JLabel();
                    label.setFont(UIConstants.DEFAULT_FONT);
                    label.setIcon(XIPUIUtilities.getIcon(UIConstants.SEARCH));
                    Collection c = engine.getFiles();
                    if (!isEmptyResult && !searching) {
                        label.setText("Result (" + engine.getFiles().size() + " )");
                    } else {
                        label.setText("Result ");
                    }
                    componentRenderer = label;
                } else {
                    if (searching) {
                        JLabel label = new JLabel();
                        label.setForeground(Color.RED);
                        label.setPreferredSize(new Dimension(500, 18));
                        label.setText("Searching...");
                        componentRenderer = label;
                    } else {
                        if (!isEmptyResult) {
                            String fileString = object.toString();
                            componentRenderer = createPanel(fileString, selected);
                        } else {
                            JLabel label = new JLabel();
                            label.setPreferredSize(new Dimension(500, 18));
                            label.setText("no results...");
                            componentRenderer = label;
                        }
                    }
                }
            }
            return componentRenderer;
        }
    }

    private JPanel createPanel(String fileString, boolean selected) {
        File file = new File(fileString);
        JPanel panel = new JPanel();
        panel.setOpaque(false);
        StringBuilder toolTip = new StringBuilder();
        toolTip.append("<html><boby><p>");
        toolTip.append(file.getAbsolutePath());
        toolTip.append("</p>");
        BoxLayout layout = new BoxLayout(panel, BoxLayout.X_AXIS);
        panel.setLayout(layout);
        JLabel labelFile = new JLabel(file.getName());
        labelFile.setFont(UIConstants.DEFAULT_FONT);
        labelFile.setBorder(BorderFactory.createEmptyBorder(0, 4, 0, 4));
        labelFile.setBackground(selected ? Color.lightGray : Color.white);
        labelFile.setOpaque(true);
        labelFile.setForeground(selected ? Color.red : Color.black);
        int[] levels = engine.getNumberOfLevelSentences(fileString);
        if (levels != null) {
            Color[] color = new Color[3];
            color[0] = GoldminerEditor.STRONG;
            color[1] = GoldminerEditor.MEDIUM;
            color[2] = GoldminerEditor.NORMAL;
            toolTip.append("<p>");
            for (int i = 0; i < levels.length; i++) {
                JLabel labelLevel = new JLabel(String.valueOf(levels[i]));
                labelLevel.setToolTipText(String.valueOf(levels[i]));
                toolTip.append("<b>" + i + " :</b>");
                toolTip.append(levels[i]);
                toolTip.append(" &nbsp;&nbsp;");
                labelLevel.setOpaque(true);
                labelLevel.setFont(UIConstants.DEFAULT_FONT);
                labelLevel.setMaximumSize(new Dimension(30, 18));
                labelLevel.setMinimumSize(new Dimension(30, 18));
                labelLevel.setPreferredSize(new Dimension(30, 18));
                labelLevel.setBorder(BorderFactory.createEmptyBorder(0, 4, 0, 4));
                if (i < 3) {
                    labelLevel.setBackground(color[i]);
                }
                panel.add(labelLevel);
            }
        }
        panel.add(labelFile);
        toolTip.append("</p><i><ol>");
        toolTip.append("<li> <b>All</b> research words are in a same relation</li>");
        toolTip.append("<li> <b>At least 2</b> research words are in a same relation</li>");
        toolTip.append("<li> <b>1</b> research words are in a relation</li>");
        toolTip.append("</ol></i></body></html>");
        panel.setToolTipText(toolTip.toString());
        return panel;
    }
}
