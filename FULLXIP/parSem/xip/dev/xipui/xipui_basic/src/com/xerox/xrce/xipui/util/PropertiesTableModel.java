/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * PropertiesTableModel.java
 *
 * Created on May 18, 2006, 2:21 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.util;

import javax.swing.table.*;

/**
 *
 * @author vgrassau
 */
public class PropertiesTableModel extends DefaultTableModel {

    private Class[] types = {java.lang.String.class, java.lang.String.class};
    public static final String[] columnNames = {"Name", "Value"};
    private Object[][] data;
    private boolean[][] editable;

    public PropertiesTableModel(Object[][] data, boolean[][] editable) {
        super(data, columnNames);
        this.editable = editable;
    }

    public Class getColumnClass(int columnIndex) {
        return types[columnIndex];
    }

    public boolean isCellEditable(int rowIndex, int columnIndex) {
        return editable[rowIndex][columnIndex];
    }
}
