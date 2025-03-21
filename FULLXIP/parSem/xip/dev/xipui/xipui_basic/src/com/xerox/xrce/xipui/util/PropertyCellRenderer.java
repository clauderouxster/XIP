/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * PropertyCellRenderer.java
 *
 * Created on May 19, 2006, 3:00 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.util;

import com.xerox.xrce.xipui.ui.UIConstants;
import javax.swing.*;
import javax.swing.table.*;
import java.awt.*;

/**
 *
 * @author vgrassau
 */
public class PropertyCellRenderer implements TableCellRenderer {

    /**
     *Le construceur ne fait rien de particulier
     */
    public PropertyCellRenderer() {
    }

    /**
     *Rennoie le composant graphique pour visionner l'objet de la table.
     *Si l'objet est un ObjectTab, alors on le cast et on le retourne. Un object Tab etant un objet graphique
     *sinon on renvoie un JLabel vide;
     *@param obj_ 
     * @return un composant graphique java
     *@see javax.swing.table.TableCellRenderer#getTableCellRendererComponent(JTable, Object ,boolean,boolean,int , int) 
     */
    public Component getTableCellRendererComponent(JTable table, Object obj_, boolean isSelected, boolean hasFocus, int row, int column) {
        JLabel label = new JLabel();
        label.setOpaque(true);
        label.setBackground(Color.WHITE);
        label.setFont(UIConstants.DEFAULT_FONT);
        if (!table.isCellEditable(row, column)) {
            label.setFont(new Font("Arial", Font.ITALIC, 11));
        }
        String columnName = table.getColumnName(column);
        if (columnName.equals(PropertiesTableModel.columnNames[0])) {
            label.setForeground(Color.RED);
            label.setFont(new Font("Arial", Font.PLAIN, 11));
        }
        if (obj_ != null) {
            label.setText(obj_.toString());
        }
        return label;

    }
}
  
