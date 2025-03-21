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

import java.awt.*;
import javax.swing.*;
import javax.swing.tree.*;
import org.w3c.dom.*;

/**
 *
 * @author vgrassau
 */
// @author Santhosh Kumar T - santhosh@in.fiorano.com
public class XMLTreeTableCellRenderer extends DefaultTreeCellRenderer {

    Color elementColor = new Color(0, 0, 128);
    Color attributeColor = new Color(0, 128, 0);

    public XMLTreeTableCellRenderer() {
        setOpenIcon(null);
        setClosedIcon(null);
        setLeafIcon(null);
    }

    public Component getTreeCellRendererComponent(JTree tree, Object value, boolean sel, boolean expanded, boolean leaf, int row, boolean hasFocus) {
        Node node = (Node) value;
        switch (node.getNodeType()) {
            case Node.ELEMENT_NODE:
                value = '<' + node.getNodeName() + '>';
                break;
            case Node.ATTRIBUTE_NODE:
                value = '@' + node.getNodeName();
                break;
            case Node.TEXT_NODE:
                value = "# text";
                break;
            case Node.COMMENT_NODE:
                value = "# comment";
                break;
            case Node.DOCUMENT_TYPE_NODE:
                DocumentType dtype = (DocumentType) node;
                value = "# doctype";
                break;
            default:
                value = node.getNodeName();
        }
        super.getTreeCellRendererComponent(tree, value, sel, expanded, leaf, row, hasFocus);
        if (!selected) {
            switch (node.getNodeType()) {
                case Node.ELEMENT_NODE:
                    setForeground(elementColor);
                    break;
                case Node.ATTRIBUTE_NODE:
                    setForeground(attributeColor);
                    break;
            }
        }
        return this;
    }
}


