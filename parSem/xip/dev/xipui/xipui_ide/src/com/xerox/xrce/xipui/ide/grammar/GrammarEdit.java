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
package com.xerox.xrce.xipui.ide.grammar;

import com.xerox.xrce.xipui.ui.Viewer;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.modules.PanelModule;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;
import java.util.*;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

/**
 *
 * @author vgrassau
 */
public class GrammarEdit extends PanelModule {

    public static final String FILES_LABEL = "File view :";
    public static String SETUP_LABEL = "Setup files";
    //structures
    public static String LOGICAL_LABEL = "Structure view :";
    public static String RULE_MAP_LABEL = "Rule Map";
    public static String FEATURE_MAP_LABEL = "Feature Map";
    public static String VARIABLE_LIST_LABEL = "Variables list";
    public static String CATEGORY_LIST_LABEL = "Categories list";
    public static String DEPENDENCY_LIST_LABEL = "Dependencies list";
    // pour les fichiers
    public static String RULE_LABEL = "Rules";
    public static String LEXICON_LABEL = "Lexicons";
    public static String DECLARATION_LABEL = "Declarations";
    public static String SCRIPT_LABEL = "Parameters";
    private JComboBox selector;
    private JPanel panelTree;
    private Hashtable viewers;

    public GrammarEdit(ModuleContext properties) {
        super(properties);
        viewers = new Hashtable();
        // Intanciation des differents viewer pour l'edidtion de la grammaire.
        viewers.put(SETUP_LABEL, new GrammarSetup(properties, SETUP_LABEL, XIPUIUtilities.getIcon(UIConstants.SETUP)));
        viewers.put(RULE_MAP_LABEL, new RuleMapUI(properties, RULE_MAP_LABEL, XIPUIUtilities.getIcon(UIConstants.RULE_CLASS_LIST)));
        viewers.put(LEXICON_LABEL, new GrammarLexicon(properties, LEXICON_LABEL, XIPUIUtilities.getIcon(UIConstants.CODE)));
        viewers.put(DECLARATION_LABEL, new GrammarDeclaration(properties, DECLARATION_LABEL, XIPUIUtilities.getIcon(UIConstants.CODE)));
        viewers.put(SCRIPT_LABEL, new GrammarParameter(properties, SCRIPT_LABEL, XIPUIUtilities.getIcon(UIConstants.CODE)));
        viewers.put(RULE_LABEL, new GrammarRule(properties, RULE_LABEL, XIPUIUtilities.getIcon(UIConstants.CODE)));
        viewers.put(FEATURE_MAP_LABEL, new ViewerFeatures(properties, FEATURE_MAP_LABEL, XIPUIUtilities.getIcon(UIConstants.FEATURES_LIST)));
        viewers.put(VARIABLE_LIST_LABEL, new ViewerVariables(properties, VARIABLE_LIST_LABEL, XIPUIUtilities.getIcon(UIConstants.VARIABLE_LIST)));
        viewers.put(CATEGORY_LIST_LABEL, new ViewerCategories(properties, CATEGORY_LIST_LABEL, XIPUIUtilities.getIcon(UIConstants.CATEGORIES_LIST)));
        viewers.put(DEPENDENCY_LIST_LABEL, new ViewerDependencies(properties, DEPENDENCY_LIST_LABEL, XIPUIUtilities.getIcon(UIConstants.DEPENDENCIES_LIST)));
        initComponents();
        setBorder(null);
        addComponentListener(new java.awt.event.ComponentAdapter() {
            public void componentShown(java.awt.event.ComponentEvent evt) {
                formComponentShown(evt);
            }
        });
        log.info("Grammar edition module was correctly loaded");
    }

    public void setSelectedView(String name) {
        Viewer v = (Viewer) viewers.get(name);
        if (v != null) {
            selector.setSelectedItem(v);
            v.loadTree();
            showViewer(v);
        }
    }

    private void formComponentShown(java.awt.event.ComponentEvent evt) {
        requestFocusInWindow();
    }

    public void reload() {
        Enumeration eViewer = viewers.elements();
        while (eViewer.hasMoreElements()) {
            Viewer v = (Viewer) eViewer.nextElement();
            v.refresh();
        }
    }

    public void close() {
        removeAll();
        this.isClosed = true;
    }

    private void initComponents() {
        BoxLayout layout = new BoxLayout(this, BoxLayout.Y_AXIS);
        setLayout(layout);
        selector = new JComboBox();
        selector.setFont(UIConstants.DEFAULT_FONT);
        selector.addItem(FILES_LABEL);
        selector.addItem(new JSeparator());
        selector.addItem(viewers.get(SETUP_LABEL));
        selector.addItem(viewers.get(GrammarEdit.DECLARATION_LABEL));
        selector.addItem(viewers.get(GrammarEdit.RULE_LABEL));
        selector.addItem(viewers.get(GrammarEdit.LEXICON_LABEL));
        selector.addItem(viewers.get(GrammarEdit.SCRIPT_LABEL));
        selector.addItem(new JSeparator());
        selector.addItem(LOGICAL_LABEL);
        selector.addItem(new JSeparator());
        selector.addItem(viewers.get(GrammarEdit.RULE_MAP_LABEL));
        selector.addItem(viewers.get(GrammarEdit.FEATURE_MAP_LABEL));
        selector.addItem(viewers.get(GrammarEdit.DEPENDENCY_LIST_LABEL));
        selector.addItem(viewers.get(GrammarEdit.CATEGORY_LIST_LABEL));
        selector.addItem(viewers.get(GrammarEdit.VARIABLE_LIST_LABEL));
        selector.setRenderer(new SelectorRenderer());
        selector.setBackground(Color.WHITE);
        selector.setMaximumRowCount(20);
        panelTree = new JPanel(new BorderLayout());
        panelTree.setBackground(Color.WHITE);
        add(selector);
        add(panelTree);
        setVisible(true);
        selector.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                selectorActionPerformed(evt);
            }
        });
    }

    private void selectorActionPerformed(ActionEvent evt) {
        Object value = selector.getSelectedItem();
        try {
            if (value instanceof Viewer) {
                Viewer viewer = (Viewer) value;
                viewer.loadTree();
                showViewer(viewer);
            }
        } catch (Exception e) {
            severe("ERROR", e);
        }
    }

    private void showViewer(JComponent viewer) {
        panelTree.removeAll();
        panelTree.add(viewer, BorderLayout.CENTER);
        if (viewer instanceof RuleMapUI) {
            RuleMapUI rUI = (RuleMapUI) viewer;
            panelTree.add(rUI.getToolBar(), BorderLayout.PAGE_END);
        }
        this.repaint();
        this.revalidate();
    }

    private class SelectorRenderer extends JLabel implements ListCellRenderer {

        public SelectorRenderer() {
            super();
            setOpaque(true);
        }

        public Component getListCellRendererComponent(JList list, Object value, int index, boolean isSelected, boolean cellHasFocus) {
            if (value != null) {
                setBackground(isSelected ? Color.red : Color.white);
                setForeground(isSelected ? Color.white : Color.black);
                setText(value.toString());
                setHorizontalAlignment(JLabel.LEFT);
                setHorizontalTextPosition(JLabel.RIGHT);
                setFont(UIConstants.DEFAULT_FONT);
                setEnabled(true);
                if (value instanceof JSeparator) {
                    return (JSeparator) value;
                } else if (value instanceof String) {
                    setBackground(isSelected ? Color.white : Color.white);
                    setForeground(isSelected ? Color.black : Color.black);
                    setHorizontalAlignment(JLabel.CENTER);
                    setHorizontalTextPosition(JLabel.RIGHT);
                    setIcon(null);
                    setEnabled(false);
                } else if (value instanceof Viewer) {
                    setIcon(((Viewer) value).getIcon());
                }
                return this;
            } else {
                return new JLabel();
            }
        }
    }
}
