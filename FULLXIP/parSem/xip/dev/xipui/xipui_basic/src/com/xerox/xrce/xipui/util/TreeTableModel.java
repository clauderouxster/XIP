/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * TreeTableModel.java
 *
 */
package com.xerox.xrce.xipui.util;

import javax.swing.tree.TreeModel;

/**
 * TreeTableModel is the model used by a JTreeTable. It extends TreeModel
 * to add methods for getting information about the set of columns each 
 * node in the TreeTableModel may have. Each column, like a column in 
 * a TableModel, has a name and a type associated with it. Each node in 
 * the TreeTableModel can return a value for each of the columns and 
 * set that value if isCellEditable() returns true.
 */
public interface TreeTableModel extends TreeModel {

    /**
     * Returns the number of available columns.
     * @return 
     */
    public int getColumnCount();

    /**
     * Returns the name for column number <code>column</code>.
     * @param column 
     * @return
     */
    public String getColumnName(int column);

    /**
     * Returns the type for column number <code>column</code>.
     * @param column
     * @return 
     */
    public Class getColumnClass(int column);

    /**
     * Returns the value to be displayed for node <code>node</code>, 
     * at column number <code>column</code>.
     * @param node
     * @param column
     * @return 
     */
    public Object getValueAt(Object node, int column);

    /**
     * Indicates whether the the value for node <code>node</code>, 
     * at column number <code>column</code> is editable.
     * @param node
     * @param column
     * @return 
     */
    public boolean isCellEditable(Object node, int column);

    /**
     * Sets the value for node <code>node</code>, 
     * at column number <code>column</code>.
     * @param aValue
     * @param node
     * @param column 
     */
    public void setValueAt(Object aValue, Object node, int column);
}

