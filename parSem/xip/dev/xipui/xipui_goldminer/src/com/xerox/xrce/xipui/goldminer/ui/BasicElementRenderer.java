/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * BasicElementRenderer.java
 *
 * Created on March 14, 2007, 2:21 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer.ui;

import com.xerox.xrce.xipui.goldminer.BasicElement;
import com.xerox.xrce.xipui.goldminer.GoldminerEngine;
import java.awt.Color;
import java.awt.Component;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.ListCellRenderer;

/**
 *Class to make a renderer for the list of applied rules.
 *a rule is rendered by a label
 *rule icon, rule number for tje label and the rule type for the tooltip text
 *
 */
public class BasicElementRenderer implements ListCellRenderer {

    private GoldminerEngine engine;

    public BasicElementRenderer(GoldminerEngine engine) {
        super();
        this.engine = engine;
    }

    /**
     *Return the render for an applied rule
     *@param value must be a Integer, this the index in the vector of applied rules.
     *@return a JLabel, if the value parameter is not a Integer then we return an empty label
     *
     */
    public Component getListCellRendererComponent(JList list, Object value, int index, boolean isSelected, boolean cellHasFocus) {
        JLabel label = new JLabel();
        if (value != null) {
            label.setBackground(isSelected ? Color.lightGray : Color.white);
            label.setForeground(isSelected ? Color.red : Color.black);
            label.setOpaque(true);
            if (value != null) {
                BasicElement element = engine.getBasicElement((Integer) value);
                if (element != null) {
                    label.setText(element.getLabel());
                    StringBuilder tip = new StringBuilder("<html><body><p>");
                    tip.append(element.getLabel());
                    tip.append("</p>");
                    if (element.getDescription() != null) {
                        tip.append("<p>");
                        tip.append(element.getDescription());
                        tip.append("</p>");
                    }
                    tip.append("</body></html>");
                    label.setToolTipText(tip.toString());
                } else {
                    label.setText("Any");
                }
            }
            return label;
        } else {
            return new JLabel();
        }
    }
}
