/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * ViewerRenderer.java
 *
 * Created on May 2, 2006, 2:07 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.ide.grammar;

import com.xerox.xrce.xipui.Specialisation;
import com.xerox.xrce.xipui.ui.UIConstants;
import javax.swing.*;
import javax.swing.tree.*;
import java.awt.*;
import java.io.*;

/**
 *
 * @author vgrassau
 */
public class ViewerRenderer implements TreeCellRenderer {

    protected Icon iconLeaf;
    protected Icon iconExpanded;
    protected Icon iconNode;
    protected Icon iconRoot;

    /** Creates a new instance of ViewerRenderer */
    public ViewerRenderer() {
    }

    /**
     *Set the icon for leaves
     *@param leaf the icon to use for lea
     */
    public void setLeafIcon(Icon leaf) {
        this.iconLeaf = leaf;
    }

    /**
     *Set the icon for the expanded nodes
     *@param expanded the expanded icon to use
     */
    public void setExpandedIcon(Icon expanded) {
        this.iconExpanded = expanded;
    }

    /**
     *Set the icon for the nodes
     *@param node the icon to use for the nodes
     */
    public void setNodeIcon(Icon node) {
        this.iconNode = node;
    }

    /**
     *The the icon for the root node
     *@param root the icon to use for root node
     */
    public void setRootIcon(Icon root) {
        this.iconRoot = root;
    }

    /**
     *The renderer...
     *@param tree 
     * @param value 
     * @param selected 
     * @param expanded 
     * @param leaf 
     * @param row 
     * @param hasFocus 
     * @return a JLabel
     */
    public Component getTreeCellRendererComponent(JTree tree, Object value, boolean selected, boolean expanded, boolean leaf, int row, boolean hasFocus) {
        JLabel label = new JLabel();
        label.setFont(UIConstants.DEFAULT_FONT);
        label.setBackground(selected ? new Color(235, 235, 235) : Color.white);
        label.setForeground(selected ? Color.red : Color.black);
        label.setOpaque(true);
        Object object = null;
        if (value instanceof DefaultMutableTreeNode) {
            DefaultMutableTreeNode node = (DefaultMutableTreeNode) value;
            object = node.getUserObject();
            if (object instanceof File) {
                label.setText(((File) object).getName());
            } else {
                label.setText(object.toString());
            }
            if (object instanceof Specialisation) {
                label.setToolTipText(((Specialisation) object).getAbsolutePath());
            } else {
                label.setToolTipText(object.toString());
            }
            if (node.isRoot()) {
                if (iconRoot == null) {
                    label.setIcon(this.iconNode);
                } else {
                    label.setIcon(this.iconRoot);
                }
            } else if (node.isLeaf()) {
                label.setIcon(this.iconLeaf);
            } else {
                if (expanded) {
                    label.setIcon(iconExpanded);
                } else {
                    label.setIcon(iconNode);
                }
            }
        }
        return label;
    }
}
