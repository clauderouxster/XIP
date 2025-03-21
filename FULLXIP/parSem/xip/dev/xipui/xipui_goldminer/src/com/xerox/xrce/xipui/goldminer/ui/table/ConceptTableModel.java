/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * ConceptTableModel.java
 *
 * Created on March 14, 2007, 11:31 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer.ui.table;

import javax.swing.table.*;
import java.util.*;

/**
 * 
 * @author grondeau
 */
public class ConceptTableModel extends AbstractTableModel {

    private String[] columnNames = {"lemma", "occurence"};
    private Object[][] data;

    /**
     * Creates a new Model for the concepts
     * @param data_ the map containing the lemmas and their occurency
     */
    public ConceptTableModel(Map<String, Integer> data_) {
        super();
        data = new Object[data_.size()][2];
        Set<String> set = data_.keySet();
        int i = 0;
        for (Iterator<String> it = set.iterator(); it.hasNext(); i++) {
            String lemma = it.next();
            data[i][0] = lemma;
            data[i][1] = data_.get(lemma);
        }
    }

    /**
     *@return le nombre de colonnes.
     */
    public int getColumnCount() {
        return columnNames.length;
    }

    /**
     *@return le nombre de lignes
     */
    public int getRowCount() {
        return data.length;
    }

    /**
     *@param col 
     * @return le nom de la colonne numero i
     */
    public String getColumnName(int col) {
        return columnNames[col];
    }

    /**
     *Renvoie l'objet contenue dans la cellule a la ligne row et a la colonne col
     * @param row
     * @param col 
     */
    public Object getValueAt(int row, int col) {
        return data[row][col];
    }

    /*
     * JTable uses this method to determine the default renderer/
     * editor for each cell.  If we didn't implement this method,
     * then the last column would contain text ("true"/"false"),
     * rather than a check box.
     */
    public Class getColumnClass(int c) {
        return getValueAt(0, c).getClass();
    }

    public boolean isCellEditable(int row, int col) {
        //Note that the data/cell address is constant,
        //no matter where the cell appears onscreen.
        return false;
    }

    public void setValueAt(Object value, int row, int col) {
        data[row][col] = value;
        fireTableCellUpdated(row, col);
    }
}
    
