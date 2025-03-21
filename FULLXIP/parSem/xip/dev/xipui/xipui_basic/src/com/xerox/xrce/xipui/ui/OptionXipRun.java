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
package com.xerox.xrce.xipui.ui;

import com.xerox.xrce.xipui.RunContext;
import com.xerox.xrce.xipui.Xip;
import com.xerox.xrce.xipui.ui.grammar.IndentFile;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.modules.PanelModule;
import javax.swing.*;

/**
 * Objet permettant de faire les options sur le display de XIP
 * @author  Vianney Grassaud
 */
public class OptionXipRun extends PanelModule {

    /** Creates new form OptionXipRun
     * @param properties 
     */
    public OptionXipRun(ModuleContext properties) {
        super(properties);
        initComponents();
        RunContext rCxt = getProject().getRunContext();
        lemma.setSelected(rCxt.getOption(new Long(Xip.DISPLAY_LEMMA)));
        surface.setSelected(rCxt.getOption(new Long(Xip.DISPLAY_SURFACE)));
        categories.setSelected(rCxt.getOption(new Long(Xip.DISPLAY_CATEGORY)));
        reduced.setSelected(rCxt.getOption(new Long(Xip.DISPLAY_REDUCED)));
        full.setSelected(rCxt.getOption(new Long(Xip.DISPLAY_FULL)));
        offset.setSelected(rCxt.getOption(new Long(Xip.DISPLAY_OFFSET)));
        wordNumber.setSelected(rCxt.getOption(new Long(Xip.DISPLAY_WORDNUM)));
        sentence.setSelected(rCxt.getOption(new Long(Xip.DISPLAY_SENTENCE)));
        byName.setSelected(rCxt.getOption(new Long(Xip.DISPLAY_DEPENDENCY_BY_NAME)));
        byCreation.setSelected(rCxt.getOption(new Long(Xip.DISPLAY_DEPENDENCY_BY_CREATION)));
        byNode.setSelected(rCxt.getOption(new Long(Xip.DISPLAY_DEPENDENCY_BY_NODE)));
        tree.setSelected(rCxt.getOption(new Long(Xip.DISPLAY_TREE)));
        treeColumn.setSelected(rCxt.getOption(new Long(Xip.DISPLAY_TREE_COLUMN)));
        math.setSelected(rCxt.getOption(new Long(Xip.DISPLAY_MATHEMATICS)));
        dependence.setSelected(rCxt.getOption(new Long(Xip.DISPLAY_DEPENDENCY_NUMBER)));
        ruleNumber.setSelected(rCxt.getOption(new Long(Xip.DISPLAY_SENTENCE_NUMBER)));
        chunkTree.setSelected(rCxt.getOption(new Long(Xip.DISPLAY_CHUNK_TREE)));
        this.indent.setSelected(rCxt.isIndentFile());
        XMLOutput.setSelected(rCxt.isXmlOutput());
        SpinnerNumberModel model = new SpinnerNumberModel(rCxt.getColumn(), Xip.COL, null, 1);
        this.column.setModel(model);
        changeDisplayMode();
    }

    public void close() {
        removeAll();
        this.isClosed = true;
    }

    //set the runcontext from the uiPanel
    private void initTableOfFlags() {
        RunContext rCxt = getProject().getRunContext();
        rCxt.setOption(new Long(Xip.DISPLAY_LEMMA), new Boolean(lemma.isSelected()));
        rCxt.setOption(new Long(Xip.DISPLAY_SURFACE), new Boolean(surface.isSelected()));
        //      rCxt.setOption(new Long(Xip.DISPLAY_MARKUP), new Boolean(false));
        //     rCxt.setOption(new Long(Xip.DISPLAY_ENTREE), new Boolean(false));
        rCxt.setOption(new Long(Xip.DISPLAY_CATEGORY), new Boolean(categories.isSelected()));
        rCxt.setOption(new Long(Xip.DISPLAY_REDUCED), new Boolean(reduced.isSelected()));
        rCxt.setOption(new Long(Xip.DISPLAY_FULL), new Boolean(full.isSelected()));
        rCxt.setOption(new Long(Xip.DISPLAY_OFFSET), new Boolean(offset.isSelected()));
        rCxt.setOption(new Long(Xip.DISPLAY_WORDNUM), new Boolean(wordNumber.isSelected()));
        rCxt.setOption(new Long(Xip.DISPLAY_SENTENCE), new Boolean(sentence.isSelected()));
        //    rCxt.setOption(new Long(Xip.DISPLAY_NONE), new Boolean(false));
        rCxt.setOption(new Long(Xip.DISPLAY_DEPENDENCY_BY_NAME), new Boolean(byName.isSelected()));
        rCxt.setOption(new Long(Xip.DISPLAY_DEPENDENCY_BY_CREATION), new Boolean(byCreation.isSelected()));
        rCxt.setOption(new Long(Xip.DISPLAY_DEPENDENCY_BY_NODE), new Boolean(byNode.isSelected()));
        rCxt.setOption(new Long(Xip.DISPLAY_TREE), new Boolean(tree.isSelected()));
        //        rCxt.setOption(new Long(Xip.UTF8_INPUT),new Boolean(inputUtf8.isSelected()));
        //        rCxt.setOption(new Long(Xip.DISPLAY_TREE_PHRASE), new Boolean(false));
        rCxt.setOption(new Long(Xip.DISPLAY_TREE_COLUMN), new Boolean(treeColumn.isSelected()));
        //        rCxt.setOption(new Long(Xip.DISPLAY_MERGE_XML_SUBTREE), new Boolean(false));
        //        rCxt.setOption(new Long(Xip.DISPLAY_CONVERSION_UTF8),new Boolean(utf8.isSelected()));
        //        rCxt.setOption(new Long(Xip.DISPLAY_EXECUTION_ERROR), new Boolean(false));
        rCxt.setOption(new Long(Xip.DISPLAY_MATHEMATICS), new Boolean(math.isSelected()));
        rCxt.setOption(new Long(Xip.DISPLAY_DEPENDENCY_NUMBER), new Boolean(dependence.isSelected()));
        //      rCxt.setOption(new Long(Xip.DISPLAY_EXECUTE_TOKEN), new Boolean(false));
        rCxt.setOption(new Long(Xip.DISPLAY_SENTENCE_NUMBER), new Boolean(ruleNumber.isSelected()));
        rCxt.setOption(new Long(Xip.DISPLAY_CHUNK_TREE), new Boolean(chunkTree.isSelected()));
    //   rCxt.setColumn(column.getText());
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc=" Generated Code ">//GEN-BEGIN:initComponents
    private void initComponents() {
        java.awt.GridBagConstraints gridBagConstraints;

        buttonGroup1 = new javax.swing.ButtonGroup();
        buttonGroup2 = new javax.swing.ButtonGroup();
        buttonGroup3 = new javax.swing.ButtonGroup();
        buttonGroup4 = new javax.swing.ButtonGroup();
        buttonGroup5 = new javax.swing.ButtonGroup();
        display = new javax.swing.JPanel();
        jPanel1 = new javax.swing.JPanel();
        lemma = new javax.swing.JRadioButton();
        reduced = new javax.swing.JRadioButton();
        surface = new javax.swing.JRadioButton();
        full = new javax.swing.JRadioButton();
        offset = new javax.swing.JCheckBox();
        wordNumber = new javax.swing.JCheckBox();
        labelDisplay = new javax.swing.JLabel();
        indent = new javax.swing.JCheckBox();
        sentence = new javax.swing.JCheckBox();
        orderDepPane = new javax.swing.JPanel();
        jPanel2 = new javax.swing.JPanel();
        byName = new javax.swing.JRadioButton();
        byNode = new javax.swing.JRadioButton();
        byCreation = new javax.swing.JRadioButton();
        labelOrder = new javax.swing.JLabel();
        miscPane = new javax.swing.JPanel();
        labelMisc = new javax.swing.JLabel();
        jPanel3 = new javax.swing.JPanel();
        panelColumn = new javax.swing.JPanel();
        columnLabel = new javax.swing.JLabel();
        column = new javax.swing.JSpinner();
        tree = new javax.swing.JCheckBox();
        treeColumn = new javax.swing.JCheckBox();
        chunkTree = new javax.swing.JCheckBox();
        ruleNumber = new javax.swing.JCheckBox();
        categories = new javax.swing.JCheckBox();
        dependence = new javax.swing.JCheckBox();
        math = new javax.swing.JCheckBox();
        XMLOutput = new javax.swing.JCheckBox();

        setLayout(new java.awt.GridBagLayout());

        setBackground(new java.awt.Color(255, 255, 255));
        setMaximumSize(new java.awt.Dimension(500, 2147483647));
        setMinimumSize(new java.awt.Dimension(300, 250));
        setName("Options\n");
        setPreferredSize(new java.awt.Dimension(300, 250));
        display.setLayout(new java.awt.GridBagLayout());

        display.setOpaque(false);
        jPanel1.setLayout(new java.awt.GridBagLayout());

        jPanel1.setBorder(javax.swing.BorderFactory.createEtchedBorder());
        jPanel1.setOpaque(false);
        buttonGroup1.add(lemma);
        lemma.setFont(new java.awt.Font("Arial", 0, 11));
        lemma.setSelected(true);
        lemma.setText("Lemma\n");
        lemma.setMargin(new java.awt.Insets(0, 0, 0, 0));
        lemma.setOpaque(false);
        lemma.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                lemmaActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        jPanel1.add(lemma, gridBagConstraints);

        buttonGroup4.add(reduced);
        reduced.setFont(new java.awt.Font("Arial", 0, 11));
        reduced.setSelected(true);
        reduced.setText("Reduced");
        reduced.setMargin(new java.awt.Insets(0, 0, 0, 0));
        reduced.setOpaque(false);
        reduced.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                reducedActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        jPanel1.add(reduced, gridBagConstraints);

        buttonGroup1.add(surface);
        surface.setFont(new java.awt.Font("Arial", 0, 11));
        surface.setText("Surface");
        surface.setMargin(new java.awt.Insets(0, 0, 0, 0));
        surface.setOpaque(false);
        surface.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                surfaceActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weighty = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        jPanel1.add(surface, gridBagConstraints);

        buttonGroup4.add(full);
        full.setFont(new java.awt.Font("Arial", 0, 11));
        full.setText("Full");
        full.setMargin(new java.awt.Insets(0, 0, 0, 0));
        full.setOpaque(false);
        full.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                fullActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        jPanel1.add(full, gridBagConstraints);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
        display.add(jPanel1, gridBagConstraints);

        offset.setFont(new java.awt.Font("Arial", 0, 11));
        offset.setText("Offset");
        offset.setMargin(new java.awt.Insets(0, 0, 0, 0));
        offset.setOpaque(false);
        offset.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                offsetActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        display.add(offset, gridBagConstraints);

        wordNumber.setFont(new java.awt.Font("Arial", 0, 11));
        wordNumber.setText("Word number");
        wordNumber.setMargin(new java.awt.Insets(0, 0, 0, 0));
        wordNumber.setOpaque(false);
        wordNumber.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                wordNumberActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        display.add(wordNumber, gridBagConstraints);

        labelDisplay.setBackground(new java.awt.Color(153, 255, 153));
        labelDisplay.setFont(new java.awt.Font("Arial", 0, 11));
        labelDisplay.setText("Display\n");
        labelDisplay.setBorder(javax.swing.BorderFactory.createMatteBorder(0, 0, 2, 0, new java.awt.Color(255, 102, 0)));
        labelDisplay.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
        display.add(labelDisplay, gridBagConstraints);

        indent.setFont(new java.awt.Font("Arial", 0, 11));
        indent.setSelected(true);
        indent.setText("Indent");
        indent.setBorder(javax.swing.BorderFactory.createEmptyBorder(0, 0, 0, 0));
        indent.setMargin(new java.awt.Insets(0, 0, 0, 0));
        indent.setOpaque(false);
        indent.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                indentActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        display.add(indent, gridBagConstraints);

        sentence.setFont(new java.awt.Font("Arial", 0, 11));
        sentence.setText("Sentence");
        sentence.setBorder(javax.swing.BorderFactory.createEmptyBorder(0, 0, 0, 0));
        sentence.setMargin(new java.awt.Insets(0, 0, 0, 0));
        sentence.setOpaque(false);
        sentence.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                sentenceActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        display.add(sentence, gridBagConstraints);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
        add(display, gridBagConstraints);

        orderDepPane.setLayout(new java.awt.GridBagLayout());

        orderDepPane.setOpaque(false);
        jPanel2.setLayout(new java.awt.GridBagLayout());

        jPanel2.setBorder(javax.swing.BorderFactory.createEtchedBorder());
        jPanel2.setOpaque(false);
        buttonGroup2.add(byName);
        byName.setFont(new java.awt.Font("Arial", 0, 11));
        byName.setText("Name");
        byName.setMargin(new java.awt.Insets(0, 0, 0, 0));
        byName.setOpaque(false);
        byName.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                byNameActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        jPanel2.add(byName, gridBagConstraints);

        buttonGroup2.add(byNode);
        byNode.setFont(new java.awt.Font("Arial", 0, 11));
        byNode.setText("Node");
        byNode.setMargin(new java.awt.Insets(0, 0, 0, 0));
        byNode.setOpaque(false);
        byNode.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                byNodeActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        jPanel2.add(byNode, gridBagConstraints);

        buttonGroup2.add(byCreation);
        byCreation.setFont(new java.awt.Font("Arial", 0, 11));
        byCreation.setSelected(true);
        byCreation.setText("Creation");
        byCreation.setMargin(new java.awt.Insets(0, 0, 0, 0));
        byCreation.setOpaque(false);
        byCreation.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                byCreationActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        jPanel2.add(byCreation, gridBagConstraints);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
        orderDepPane.add(jPanel2, gridBagConstraints);

        labelOrder.setBackground(new java.awt.Color(153, 255, 153));
        labelOrder.setFont(new java.awt.Font("Arial", 0, 11));
        labelOrder.setText("Order dependency by");
        labelOrder.setBorder(javax.swing.BorderFactory.createMatteBorder(0, 0, 2, 0, new java.awt.Color(255, 102, 51)));
        labelOrder.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        orderDepPane.add(labelOrder, gridBagConstraints);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
        add(orderDepPane, gridBagConstraints);

        miscPane.setLayout(new java.awt.GridBagLayout());

        miscPane.setOpaque(false);
        labelMisc.setBackground(new java.awt.Color(153, 255, 153));
        labelMisc.setFont(new java.awt.Font("Arial", 0, 11));
        labelMisc.setText("Other");
        labelMisc.setBorder(javax.swing.BorderFactory.createMatteBorder(0, 0, 2, 0, new java.awt.Color(255, 102, 51)));
        labelMisc.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        miscPane.add(labelMisc, gridBagConstraints);

        jPanel3.setLayout(new java.awt.GridBagLayout());

        jPanel3.setBorder(javax.swing.BorderFactory.createEtchedBorder());
        jPanel3.setOpaque(false);
        panelColumn.setLayout(new java.awt.GridBagLayout());

        panelColumn.setOpaque(false);
        columnLabel.setFont(new java.awt.Font("Arial", 0, 11));
        columnLabel.setText("Column");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
        panelColumn.add(columnLabel, gridBagConstraints);

        column.setFont(new java.awt.Font("Arial", 0, 11));
        column.setToolTipText("column size");
        column.setMaximumSize(new java.awt.Dimension(50, 20));
        column.setMinimumSize(new java.awt.Dimension(50, 20));
        column.setOpaque(false);
        column.setPreferredSize(new java.awt.Dimension(50, 20));
        column.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                columnStateChanged(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        panelColumn.add(column, gridBagConstraints);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
        jPanel3.add(panelColumn, gridBagConstraints);

        tree.setFont(new java.awt.Font("Arial", 0, 11));
        tree.setSelected(true);
        tree.setText("Tree");
        tree.setBorder(javax.swing.BorderFactory.createEmptyBorder(0, 0, 0, 0));
        tree.setMargin(new java.awt.Insets(0, 0, 0, 0));
        tree.setOpaque(false);
        tree.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                treeActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        jPanel3.add(tree, gridBagConstraints);

        treeColumn.setFont(new java.awt.Font("Arial", 0, 11));
        treeColumn.setText("Tree column");
        treeColumn.setBorder(javax.swing.BorderFactory.createEmptyBorder(0, 0, 0, 0));
        treeColumn.setMargin(new java.awt.Insets(0, 0, 0, 0));
        treeColumn.setOpaque(false);
        treeColumn.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                treeColumnActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.ipadx = 1;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 4, 2, 2);
        jPanel3.add(treeColumn, gridBagConstraints);

        chunkTree.setFont(new java.awt.Font("Arial", 0, 11));
        chunkTree.setSelected(true);
        chunkTree.setText("Chunk tree");
        chunkTree.setMargin(new java.awt.Insets(0, 0, 0, 0));
        chunkTree.setOpaque(false);
        chunkTree.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                chunkTreeActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        jPanel3.add(chunkTree, gridBagConstraints);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.gridheight = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
        miscPane.add(jPanel3, gridBagConstraints);

        ruleNumber.setFont(new java.awt.Font("Arial", 0, 11));
        ruleNumber.setText("Rule number");
        ruleNumber.setMargin(new java.awt.Insets(0, 0, 0, 0));
        ruleNumber.setOpaque(false);
        ruleNumber.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                ruleNumberActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.gridheight = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
        miscPane.add(ruleNumber, gridBagConstraints);

        categories.setFont(new java.awt.Font("Arial", 0, 11));
        categories.setText("Categories");
        categories.setBorder(javax.swing.BorderFactory.createEmptyBorder(0, 0, 0, 0));
        categories.setMargin(new java.awt.Insets(0, 0, 0, 0));
        categories.setOpaque(false);
        categories.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                categoriesActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
        miscPane.add(categories, gridBagConstraints);

        dependence.setFont(new java.awt.Font("Arial", 0, 11));
        dependence.setSelected(true);
        dependence.setText("Dependencies");
        dependence.setActionCommand("Dependence");
        dependence.setBorder(javax.swing.BorderFactory.createEmptyBorder(0, 0, 0, 0));
        dependence.setMargin(new java.awt.Insets(0, 0, 0, 0));
        dependence.setOpaque(false);
        dependence.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                dependenceActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
        miscPane.add(dependence, gridBagConstraints);

        math.setFont(new java.awt.Font("Arial", 0, 11));
        math.setText("Mathematics");
        math.setBorder(javax.swing.BorderFactory.createEmptyBorder(0, 0, 0, 0));
        math.setMargin(new java.awt.Insets(0, 0, 0, 0));
        math.setOpaque(false);
        math.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                mathActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
        miscPane.add(math, gridBagConstraints);

        XMLOutput.setFont(new java.awt.Font("Arial", 0, 11));
        XMLOutput.setText("XML output");
        XMLOutput.setBorder(javax.swing.BorderFactory.createEmptyBorder(0, 0, 0, 0));
        XMLOutput.setMargin(new java.awt.Insets(0, 0, 0, 0));
        XMLOutput.setOpaque(false);
        XMLOutput.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                XMLOutputActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
        miscPane.add(XMLOutput, gridBagConstraints);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
        add(miscPane, gridBagConstraints);

    }// </editor-fold>//GEN-END:initComponents

    private void columnStateChanged(javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_columnStateChanged
        RunContext rCxt = getProject().getRunContext();
        Integer col = (Integer) column.getValue();
        rCxt.setColumn(col.intValue());
        changeDisplayMode();
    }//GEN-LAST:event_columnStateChanged

    private void XMLOutputActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_XMLOutputActionPerformed
        RunContext rCxt = getProject().getRunContext();
        if (XMLOutput.isSelected()) {
            rCxt.setXMLMode(Xip.XML_OUTPUT);
        } else {
            rCxt.setXMLMode(Xip.XML_NONE);
        }
    }//GEN-LAST:event_XMLOutputActionPerformed

    private void indentActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_indentActionPerformed
        try {
            RunContext rCxt = getProject().getRunContext();
            rCxt.setIndentFile(indent.isSelected());
            if (!indent.isSelected()) {
                IndentFile indentF = (IndentFile) getProperty("indent_file");
                if (indentF != null) {
                    indentF.close();
                }
            }
        } catch (Exception e) {
            severe("error", e);
        }
    }//GEN-LAST:event_indentActionPerformed

    private void mathActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_mathActionPerformed
        changeDisplayMode();
    }//GEN-LAST:event_mathActionPerformed

    private void dependenceActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_dependenceActionPerformed
        changeDisplayMode();
    }//GEN-LAST:event_dependenceActionPerformed

    private void categoriesActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_categoriesActionPerformed
        changeDisplayMode();
    }//GEN-LAST:event_categoriesActionPerformed

    private void sentenceActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_sentenceActionPerformed
        changeDisplayMode();
    }//GEN-LAST:event_sentenceActionPerformed

    private void treeColumnActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_treeColumnActionPerformed
        changeDisplayMode();
    }//GEN-LAST:event_treeColumnActionPerformed

    private void treeActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_treeActionPerformed
        changeDisplayMode();
    }//GEN-LAST:event_treeActionPerformed

    private void fullActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_fullActionPerformed
        if (full.isSelected()) {
            lemma.setSelected(false);
            surface.setSelected(true);
        }
        this.revalidate();
        changeDisplayMode();
    }//GEN-LAST:event_fullActionPerformed

    private void surfaceActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_surfaceActionPerformed
        changeDisplayMode();
    }//GEN-LAST:event_surfaceActionPerformed

    private void reducedActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_reducedActionPerformed
        changeDisplayMode();
    }//GEN-LAST:event_reducedActionPerformed

    private void lemmaActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_lemmaActionPerformed
        if (lemma.isSelected()) {
            full.setSelected(false);
            reduced.setSelected(true);
        }
        this.revalidate();
        changeDisplayMode();
    }//GEN-LAST:event_lemmaActionPerformed

    private void byCreationActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_byCreationActionPerformed
        changeDisplayMode();
    }//GEN-LAST:event_byCreationActionPerformed

    private void byNodeActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_byNodeActionPerformed
        changeDisplayMode();
    }//GEN-LAST:event_byNodeActionPerformed

    private void byNameActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_byNameActionPerformed
        changeDisplayMode();
    }//GEN-LAST:event_byNameActionPerformed

    private void ruleNumberActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_ruleNumberActionPerformed
        changeDisplayMode();

    }//GEN-LAST:event_ruleNumberActionPerformed

    private void chunkTreeActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_chunkTreeActionPerformed
        changeDisplayMode();

    }//GEN-LAST:event_chunkTreeActionPerformed

    private void wordNumberActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_wordNumberActionPerformed
        if (wordNumber.isSelected()) {
            offset.setSelected(false);
        }
        this.revalidate();
        changeDisplayMode();

    }//GEN-LAST:event_wordNumberActionPerformed

    private void offsetActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_offsetActionPerformed
        if (offset.isSelected()) {
            wordNumber.setSelected(false);
        }
        this.revalidate();
        changeDisplayMode();

    }//GEN-LAST:event_offsetActionPerformed

    private void changeDisplayMode() {
        RunContext rCxt = getProject().getRunContext();
        initTableOfFlags();
        if (!rCxt.loadOptions()) {
            warning("cannot load display options");
        }
    }
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JCheckBox XMLOutput;
    private javax.swing.ButtonGroup buttonGroup1;
    private javax.swing.ButtonGroup buttonGroup2;
    private javax.swing.ButtonGroup buttonGroup3;
    private javax.swing.ButtonGroup buttonGroup4;
    private javax.swing.ButtonGroup buttonGroup5;
    private javax.swing.JRadioButton byCreation;
    private javax.swing.JRadioButton byName;
    private javax.swing.JRadioButton byNode;
    private javax.swing.JCheckBox categories;
    private javax.swing.JCheckBox chunkTree;
    private javax.swing.JSpinner column;
    private javax.swing.JLabel columnLabel;
    private javax.swing.JCheckBox dependence;
    private javax.swing.JPanel display;
    private javax.swing.JRadioButton full;
    private javax.swing.JCheckBox indent;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JPanel jPanel3;
    private javax.swing.JLabel labelDisplay;
    private javax.swing.JLabel labelMisc;
    private javax.swing.JLabel labelOrder;
    private javax.swing.JRadioButton lemma;
    private javax.swing.JCheckBox math;
    private javax.swing.JPanel miscPane;
    private javax.swing.JCheckBox offset;
    private javax.swing.JPanel orderDepPane;
    private javax.swing.JPanel panelColumn;
    private javax.swing.JRadioButton reduced;
    private javax.swing.JCheckBox ruleNumber;
    private javax.swing.JCheckBox sentence;
    private javax.swing.JRadioButton surface;
    private javax.swing.JCheckBox tree;
    private javax.swing.JCheckBox treeColumn;
    private javax.swing.JCheckBox wordNumber;
    // End of variables declaration//GEN-END:variables
}
