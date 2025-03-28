/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * TableColumnResizer.java
 *
 * Created on May 3, 2006, 6:06 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.util;

import java.awt.*;

import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.table.*;

/**
 *
 * @author vgrassau
 */
public class TableColumnResizer extends MouseInputAdapter {

    public static Cursor resizeCursor = Cursor.getPredefinedCursor(Cursor.E_RESIZE_CURSOR);
    private int mouseXOffset;
    private Cursor otherCursor = resizeCursor;
    private JTable table;

    public TableColumnResizer(JTable table) {
        this.table = table;
        table.addMouseListener(this);
        table.addMouseMotionListener(this);
    }

    private boolean canResize(TableColumn column) {
        return column != null && table.getTableHeader().getResizingAllowed() && column.getResizable();
    }

    private TableColumn getResizingColumn(Point p) {
        return getResizingColumn(p, table.columnAtPoint(p));
    }

    private TableColumn getResizingColumn(Point p, int column) {
        if (column == -1) {
            return null;
        }
        int row = table.rowAtPoint(p);
        if (row == -1) {
            return null;
        }
        Rectangle r = table.getCellRect(row, column, true);
        r.grow(-3, 0);
        if (r.contains(p)) {
            return null;
        }
        int midPoint = r.x + r.width / 2;
        int columnIndex;
        if (table.getTableHeader().getComponentOrientation().isLeftToRight()) {
            columnIndex = (p.x < midPoint) ? column - 1 : column;
        } else {
            columnIndex = (p.x < midPoint) ? column : column - 1;
        }
        if (columnIndex == -1) {
            return null;
        }
        return table.getTableHeader().getColumnModel().getColumn(columnIndex);
    }

    public void mousePressed(MouseEvent e) {
        table.getTableHeader().setDraggedColumn(null);
        table.getTableHeader().setResizingColumn(null);
        table.getTableHeader().setDraggedDistance(0);
        Point p = e.getPoint();
        // First find which header cell was hit 
        int index = table.columnAtPoint(p);
        if (index == -1) {
            return;
            // The last 3 pixels + 3 pixels of next column are for resizing 
        }
        TableColumn resizingColumn = getResizingColumn(p, index);
        if (!canResize(resizingColumn)) {
            return;
        }
        table.getTableHeader().setResizingColumn(resizingColumn);
        if (table.getTableHeader().getComponentOrientation().isLeftToRight()) {
            mouseXOffset = p.x - resizingColumn.getWidth();
        } else {
            mouseXOffset = p.x + resizingColumn.getWidth();
        }
    }

    private void swapCursor() {
        Cursor tmp = table.getCursor();
        table.setCursor(otherCursor);
        otherCursor = tmp;
    }

    public void mouseMoved(MouseEvent e) {
        if (canResize(getResizingColumn(e.getPoint())) != (table.getCursor() == resizeCursor)) {
            swapCursor();
        }
    }

    public void mouseDragged(MouseEvent e) {
        int mouseX = e.getX();
        TableColumn resizingColumn = table.getTableHeader().getResizingColumn();
        boolean headerLeftToRight = table.getTableHeader().getComponentOrientation().isLeftToRight();
        if (resizingColumn != null) {
            int oldWidth = resizingColumn.getWidth();
            int newWidth;
            if (headerLeftToRight) {
                newWidth = mouseX - mouseXOffset;
            } else {
                newWidth = mouseXOffset - mouseX;
            }
            resizingColumn.setWidth(newWidth);
            Container container;
            if ((table.getTableHeader().getParent() == null) || ((container = table.getTableHeader().getParent().getParent()) == null) || !(container instanceof JScrollPane)) {
                return;
            }
            if (!container.getComponentOrientation().isLeftToRight() && !headerLeftToRight) {
                if (table != null) {
                    JViewport viewport = ((JScrollPane) container).getViewport();
                    int viewportWidth = viewport.getWidth();
                    int diff = newWidth - oldWidth;
                    int newHeaderWidth = table.getWidth() + diff;
                    /* Resize a table */
                    Dimension tableSize = table.getSize();
                    tableSize.width += diff;
                    table.setSize(tableSize);
                    /* 
                     * If this table is in AUTO_RESIZE_OFF mode and has a horizontal 
                     * scrollbar, we need to update a view's position. 
                     */
                    if ((newHeaderWidth >= viewportWidth) && (table.getAutoResizeMode() == JTable.AUTO_RESIZE_OFF)) {
                        Point p = viewport.getViewPosition();
                        p.x = Math.max(0, Math.min(newHeaderWidth - viewportWidth, p.x + diff));
                        viewport.setViewPosition(p);
                        /* Update the original X offset value. */
                        mouseXOffset += diff;
                    }
                }
            }
        }
    }

    public void mouseReleased(MouseEvent e) {
        table.getTableHeader().setResizingColumn(null);
        table.getTableHeader().setDraggedColumn(null);
    }
} 

