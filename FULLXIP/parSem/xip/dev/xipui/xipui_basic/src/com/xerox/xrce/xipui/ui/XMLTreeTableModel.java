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
package com.xerox.xrce.xipui.ui;

import com.xerox.xrce.xipui.util.DOMUtil;
import com.xerox.xrce.xipui.util.TreeTableModel;
import javax.swing.event.*;
import javax.swing.tree.*;

import org.w3c.dom.*;

import org.xml.sax.*;

/**
 *
 * @author vgrassau
 */
public class XMLTreeTableModel implements TreeTableModel {

    private Node node;

    public XMLTreeTableModel(InputSource is) throws Exception {
        this(DOMUtil.createDocument(is).getDocumentElement());
    }

    public XMLTreeTableModel(Node node) {
        this.node = node;
    }

    public Object getRoot() {
        return node;
    }

    public Object getChild(Object parent, int index) {
        Node node1 = (Node) parent;
        NamedNodeMap attrs = node1.getAttributes();
        int attrCount = attrs != null ? attrs.getLength() : 0;
        if (index < attrCount) {
            return attrs.item(index);
        }
        NodeList children = node1.getChildNodes();
        return children.item(index - attrCount);
    }

    public int getChildCount(Object parent) {
        Node node1 = (Node) parent;
        NamedNodeMap attrs = node1.getAttributes();
        int attrCount = attrs != null ? attrs.getLength() : 0;
        NodeList children = node1.getChildNodes();
        int childCount = children != null ? children.getLength() : 0;
        if (childCount == 1 && children.item(0).getNodeType() == Node.TEXT_NODE) {
            return attrCount;
        } else {
            return attrCount + childCount;
        }
    }

    public boolean isLeaf(Object node) {
        return getChildCount(node) == 0;
    }

    public int getIndexOfChild(Object parent, Object child) {
        Node node1 = (Node) parent;
        Node childNode = (Node) child;
        NamedNodeMap attrs = node1.getAttributes();
        int attrCount = attrs != null ? attrs.getLength() : 0;
        if (childNode.getNodeType() == Node.ATTRIBUTE_NODE) {
            for (int i = 0; i < attrCount; i++) {
                if (attrs.item(i) == child) {
                    return i;
                }
            }
        } else {
            NodeList children = node1.getChildNodes();
            int childCount = children != null ? children.getLength() : 0;
            for (int i = 0; i < childCount; i++) {
                if (children.item(i) == child) {
                    return attrCount + i;
                }
            }
        }
        throw new RuntimeException("this should never happen!");
    }

    public void addTreeModelListener(TreeModelListener listener) {
        // not editable 
    }

    public void removeTreeModelListener(TreeModelListener listener) {
        // not editable 
    }

    public void valueForPathChanged(TreePath path, Object newValue) {
        // not editable 
    }

    public int getColumnCount() {
        return 2;
    }

    public String getColumnName(int column) {
        return column == 0 ? "Node" : "Value";
    }

    public Class getColumnClass(int column) {
        return column == 0 ? TreeTableModel.class : String.class;
    }

    public Object getValueAt(Object node, int column) {
        if (column == 0) {
            return node;
        } else {
            Node n = (Node) node;
            if (n.getNodeType() == Node.ELEMENT_NODE) {
                NodeList children = n.getChildNodes();
                int childCount = children != null ? children.getLength() : 0;
                if (childCount == 1 && children.item(0).getNodeType() == Node.TEXT_NODE) {
                    return children.item(0).getNodeValue();
                }
            }
            return n.getNodeValue();
        }
    }

    public boolean isCellEditable(Object node, int column) {
        return false;
    }

    public void setValueAt(Object aValue, Object node, int column) {
        // not editable 
    }
}

