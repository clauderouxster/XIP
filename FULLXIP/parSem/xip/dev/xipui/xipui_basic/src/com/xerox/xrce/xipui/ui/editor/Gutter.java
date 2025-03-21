/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 *
 * Created on 1 ao�t 2005, 17:45
 *
 *  Copyright (C) 2005 Yves Zoundi
 *  MOdified by Jason Davis
 *  
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation; either version 2.1 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
package com.xerox.xrce.xipui.ui.editor;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.FontMetrics;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.RenderingHints;
import java.awt.event.AdjustmentEvent;
import java.awt.event.AdjustmentListener;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import javax.swing.JComponent;
import javax.swing.JScrollPane;
import javax.swing.text.JTextComponent;

/**
 * A class which adds line numbering to a editor
 * 
 * @author Yves Zoundi
 */
public final class Gutter extends JComponent {

    private JTextComponent edit;
    private int rhWidth = 40;

    /**
     * 
     * @param edit
     *          the editor which has to display line numbers
     * @param pane 
     */
    public Gutter(JTextComponent edit, JScrollPane pane) {
        this.edit = edit;
        //listen for font change
        edit.addPropertyChangeListener(new PropertyChangeListener() {

            public void propertyChange(PropertyChangeEvent evt) {
                if (evt.getPropertyName().equals("font")) {
                    revalidate();
                    repaint();
                }
            }
        });
        setBackground(Color.GRAY);
        setForeground(Color.GRAY.darker());
        pane.getVerticalScrollBar().addAdjustmentListener(new AdjustmentListener() {

            public void adjustmentValueChanged(AdjustmentEvent e) {
                revalidate();
                repaint();
            }
        });
    }

    /**
     * 
     * @return The preferred size of the line numbering column
     */
    public Dimension getPreferredSize() {
        FontMetrics fm = edit.getFontMetrics(edit.getFont());
        int h = fm.getHeight();
        int w = fm.stringWidth(String.valueOf(edit.getHeight() / h)) + 6;
        rhWidth = w;
        int hi = (int) edit.getPreferredSize().getHeight();
        return new Dimension(w, hi);
    }

    /**
     * 
     * @param g
     *          A graphic component
     */
    public void paintComponent(java.awt.Graphics g) {
        super.paintComponent(g);
        Graphics2D g2d = (Graphics2D) g;
        g2d.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_OFF);
        g2d.setFont(edit.getFont());
        FontMetrics fm = g2d.getFontMetrics();
        Rectangle rec = edit.getVisibleRect();
        int h = fm.getHeight(); //the height of a line

        int row = rec.y / h;
        int i = h - fm.getDescent();
        i += h * row;
        int max = row + (rec.height / h) + 2;
        // AbstractDocument doc = (AbstractDocument)edit.getDocument();
        // max = doc.getDefaultRootElement().getElementIndex(doc.getLength());
        while (row < max) {
            String s = Integer.toString(row) + "  ";
            g2d.drawString(s, (rhWidth + 9) - fm.stringWidth(s), i);
            //g2d.drawString(s, (rhWidth+2 ) - fm.stringWidth(s), i  );
            i += h;
            row++;
        }
        g2d.setColor(Color.GRAY.darker());
        g2d.drawLine(getWidth() - 3, rec.y, getWidth() - 3, rec.height + rec.y);
    }
}
