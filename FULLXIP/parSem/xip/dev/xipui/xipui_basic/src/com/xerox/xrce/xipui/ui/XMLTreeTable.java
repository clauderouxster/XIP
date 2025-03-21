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

import com.xerox.xrce.xipui.util.FileTools;
import com.xerox.xrce.xipui.util.JTreeTable;
import com.xerox.xrce.xipui.util.TableColumnResizer;
import com.xerox.xrce.xipui.util.TreeTableModel;
import java.io.*;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import org.xml.sax.*;

/**
 *
 * @author vgrassau
 */
public class XMLTreeTable extends JTreeTable {

    public XMLTreeTable(TreeTableModel treeTableModel) {
        super(treeTableModel);
        setShowGrid(true);
        setGridColor(new Color(234, 234, 234));
        setIntercellSpacing(new Dimension(1, 1));
        tree.setCellRenderer(new XMLTreeTableCellRenderer());
        new TableColumnResizer(this);
    }

    public boolean editCellAt(int row, int column, EventObject e) {
        if (e instanceof MouseEvent) {
            MouseEvent me = (MouseEvent) e;
            // If the modifiers are not 0 (or the left mouse button), 
            // tree may try and toggle the selection, and table 
            // will then try and toggle, resulting in the 
            // selection remaining the same. To avoid this, we 
            // only dispatch when the modifiers are 0 (or the left mouse 
            // button). 
            if (me.getModifiers() == 0 ||
                    me.getModifiers() == InputEvent.BUTTON1_MASK) {
                for (int counter = getColumnCount() - 1; counter >= 0;
                        counter--) {
                    if (getColumnClass(counter) == TreeTableModel.class) {
                        MouseEvent newME = new MouseEvent(tree, me.getID(),
                                me.getWhen(), me.getModifiers(),
                                me.getX() - getCellRect(0, counter, true).x,
                                me.getY(), me.getClickCount(),
                                me.isPopupTrigger());
                        tree.dispatchEvent(newME);
                        break;
                    }
                }
            }
            return false;
        }
        return super.editCellAt(row, column, e);
    }

    // overriden to make the height of scroll match viewpost height if smaller 
    public boolean getScrollableTracksViewportHeight() {
        return getPreferredSize().height < getParent().getHeight();
    }

    // mouse press intended for resize shouldn't change row/col/cell celection 
    public void changeSelection(int row, int column, boolean toggle, boolean extend) {
        if (getCursor() == TableColumnResizer.resizeCursor) {
            return;
        }
        super.changeSelection(row, column, toggle, extend);
    }

    public static JComponent getFullXMLTreeTable(File file, String encoding) throws Exception {
        InputSource source = new InputSource(new StringReader(FileTools.read(file, encoding)));
        System.out.println("encodage XML : " + encoding);
        source.setEncoding(encoding);
        XMLTreeTableModel xmlTableModel = new XMLTreeTableModel(source);
        XMLTreeTable table = new XMLTreeTable(xmlTableModel);
        JScrollPane scroll = new JScrollPane(table) {
            public void setColumnHeaderView(Component view) {
            }
        };
        scroll.setBorder(null);
        return scroll;
    }
}
