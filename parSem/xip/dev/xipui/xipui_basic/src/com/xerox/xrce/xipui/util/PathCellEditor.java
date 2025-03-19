/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * PathCellEditor.java
 *
 * Created on May 19, 2006, 11:15 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.util;

import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.UIVariable;
import javax.swing.*;
import javax.swing.table.*;
import java.awt.*;

/**
 *
 * @author vgrassau
 */
public class PathCellEditor extends AbstractCellEditor implements TableCellEditor {

    private String pathValue;
    private ModuleContext context;

    public PathCellEditor(ModuleContext context) {
        this.context = context;
    }

    /** 
     *Renvoie le composant graphique permettant l'edition
     *@param col 
     * @return Composant graphique
     */
    public Component getTableCellEditorComponent(JTable table, Object value, boolean isSelected, int row, int col) {
        if (value instanceof String) {
            pathValue = value.toString();
            String pathName = (String) table.getValueAt(row, col - 1);
            UIVariable ui = new UIVariable(context, pathName);
            ui.setValue(pathValue);
            ui.setTitle("Path information : " + pathName);
            ui.setVisible(true);
            if (!ui.wasCanceled()) {
                pathValue = ui.getVariableValue();
                table.setValueAt(pathValue, row, col);
            }
            JLabel lab = new JLabel(pathValue);
            lab.setFont(UIConstants.DEFAULT_FONT);
            return lab;
        } else {
            JLabel label = new JLabel();
            label.setOpaque(true);
            label.setBackground(new Color(204, 204, 204));
            label.setEnabled(false);
            return label;
        }
    }

    /**
     *renvoie l'objet edite
     *@return ObjectTab ou une chaine de caractere vide si l'objet editer est null
     */
    public Object getCellEditorValue() {
        return pathValue;
    }
}
