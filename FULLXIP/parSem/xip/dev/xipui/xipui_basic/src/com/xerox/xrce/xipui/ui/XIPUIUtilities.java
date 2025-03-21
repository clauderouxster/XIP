/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 ****************************************************************************************
 * Copyright (c) 2006 Xerox Corporation                                                 *
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

import com.xerox.xrce.xipui.ui.editor.DefaultEditor;
import java.awt.*;
import java.util.*;
import javax.swing.*;

/**
 *
 * @author vgrassau
 */
public final class XIPUIUtilities {

    private static final String name = "com.xerox.xrce.xipui.ui.UIConstants";
    public static final String imagesResource = "/com/xerox/xrce/xipui/ui/images/";

    /** Creates a new instance of XIPUIUtilities */
    public XIPUIUtilities() {
    }

    static final public ImageIcon getIcon(String icon) {
        try {
            return new ImageIcon(Class.forName(name).getResource(imagesResource + icon));
        } catch (Exception e) {
            return null;
        }
    }

    static final public String getIconString(String icon) {
        try {
            return imagesResource + icon;
        } catch (Exception e) {
            return null;
        }
    }

    public final static JMenu createJMenu(String text, String toolTip, Icon icon) {
        JMenu menu = new JMenu(text);
        menu.setFont(UIConstants.DEFAULT_FONT);
        menu.setToolTipText(toolTip);
        menu.setIcon(icon);
        return menu;
    }

    public final static JMenuItem createJMenuItem(String text, String toolTip, Icon icon) {
        JMenuItem item = new JMenuItem(text);
        item.setFont(new Font("Arial", Font.PLAIN, 11));
        item.setToolTipText(toolTip);
        item.setIcon(icon);
        return item;
    }

    /**
     *Reads the resource file application.properties and return the value for the key given for parameter.
     *@param key the key to search
     *@return the value for the key, if the resource file or the key were not found, the key will be return otherwise the value will be return
     */
    public final static String getApplicationResource(String key) {
        return getApplicationResource("com/xerox/xrce/xipui/ui/application", key);
    }

    /**
     *Reads the resource file application.properties and return the value for the key given for parameter.
     *@param resource 
     *@param key the key to search
     *@return the value for the key, if the resource file or the key were not found, the key will be return otherwise the value will be return
     */
    public final static String getApplicationResource(String resource, String key) {
        String result = key;

        try {
            ResourceBundle b = ResourceBundle.getBundle(resource);
            String value = b.getString(key);
            result = value.trim();
        } catch (MissingResourceException e) {
            result = key;
        } catch (NullPointerException e) {
            result = key;
        }
        return result;
    }

    /**
     *Methode permettant d'ajouter un composant graphique dans le conteneur d'onglet
     *@param tab 
     *@param c composant graphique a rajouter
     *@param icon icon a utiliser
     *@param title titre pour l'onglet
     *@param toolTip pour l'onglet
     * @return 
     */
    static final public Component addComponentOnTab(JTabbedPane tab, Component c, Icon icon, String title, String toolTip) {
        Component res = c;

        int index = tab.indexOfTab(title);
        if (index == -1) {
            tab.addTab(title, icon, c, toolTip);
            tab.setSelectedComponent(c);
        } else {
            Component oldComp = tab.getComponentAt(index);
            //si c est un editeur et que le fichier a ete modifie
            // alors on ne fait que selectionner l'onglet au lieu de le recalculer
            if (oldComp instanceof DefaultEditor) {
                DefaultEditor edit = (DefaultEditor) oldComp;
                if (!edit.wasModified()) {
                    tab.setComponentAt(index, c);
                } else {
                    res = oldComp;
                }
            } else {
                tab.setComponentAt(index, c);
            }
            tab.setTitleAt(index, title);
            tab.setIconAt(index, icon);
            tab.setToolTipTextAt(index, toolTip);
            tab.setSelectedIndex(index);
        }
        res.requestFocusInWindow();
        return res;
    }

    /**
     *Creates a JLabel
     *This method calls the createJLabel(String, Icon, String, Color, Color, Font) method
     *@param text the text for the label
     *@param icon the icon to use can be null
     * @param tooltip
     * @return 
     */
    static final public JLabel createJLabel(String text, Icon icon, String tooltip) {
        return createJLabel(text, icon, tooltip, null, null, null);
    }

    /**
     *Creates a JLabel
     *@param text the text for the label
     *@param icon the icon to use can be null
     *@param tooltip the tooltip text to use, can be null
     *@param bg the background color to use, if it is no null then the label will be opaque
     *@param fg the foreground color to use
     *@param font the font to use, if it is null, the defaut font for the XIPUI will be used
     * @return 
     */
    static final public JLabel createJLabel(String text, Icon icon, String tooltip, Color bg, Color fg, Font font) {
        JLabel label = new JLabel(text);
        label.setToolTipText(tooltip);
        label.setIcon(icon);
        if (bg != null) {
            label.setOpaque(true);
            label.setBackground(bg);
        }
        if (fg != null) {
            label.setForeground(fg);
        }
        if (font != null) {
            label.setFont(font);
        } else {
            label.setFont(UIConstants.DEFAULT_FONT);
        }
        return label;
    }

    /**
     *Creates a JButton
     *If the action is no null, we add this to the button before the text, icon and tooltip parameters.
     *So, the text, icon, tooltip parameters are more "prioritaires"
     *@param text the text to write for the button
     *@param icon the icon to use
     *@param tooltip the description to use
     *@param action the action for this button
     * @param font
     * @param border
     * @return 
     *
     */
    static final public JButton createJButton(String text, Icon icon, String tooltip, Action action, Font font, boolean border) {
        JButton button = new JButton();
        if (action != null) {
            button.setAction(action);
        }
        if (text != null) {
            button.setText(text);
        }
        if (tooltip != null) {
            button.setToolTipText(tooltip);
        }
        if (icon != null) {
            button.setIcon(icon);
        }
        if (font != null) {
            button.setFont(font);
        } else {
            button.setFont(UIConstants.DEFAULT_FONT);
        }
        button.setBorderPainted(border);
        return button;
    }

    static final public JButton createJButton(String text, Action action, boolean border) {
        return createJButton(text, null, null, action, null, border);
    }

    public static void main(String[] args) {
        System.out.println(getApplicationResource("menu.file"));
    }
}// end of class
