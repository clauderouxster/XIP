/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * CorpusElementProperties.java
 *
 * Created on February 22, 2006, 3:55 PM
 */
package com.xerox.xrce.xipui.ui.corpus;

import com.xerox.xrce.xipui.CorpusElement;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;
import com.xerox.xrce.xipui.util.FileTools;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

/**
 *
 * @author  vgrassau
 */
public class CorpusElementProperties extends javax.swing.JPanel {

    private CorpusElement[] elements;
    private ActionListener actionEncoding;
    private JTree tree;

    /** Creates new form CorpusElementProperties
     * @param corpusElement
     * @param tree_ 
     */
    public CorpusElementProperties(CorpusElement corpusElement, JTree tree_) {
        elements = new CorpusElement[1];
        elements[0] = corpusElement;
        initPanel(tree_);
    }

    public CorpusElementProperties(CorpusElement[] elements, JTree tree_) {
        this.elements = elements;
        initPanel(tree_);
    }

    private void initPanel(JTree tree) {
        this.tree = tree;
        initComponents();
        int min = -1;
        int step = 1;
        SpinnerNumberModel model1 = new SpinnerNumberModel(min, min, null, step);
        SpinnerNumberModel model2 = new SpinnerNumberModel(min, min, null, step);
        startParse.setModel(model1);
        endParse.setModel(model2);
        encoding.setRenderer(new EncodingRenderer());
        actionEncoding = new ActionListener() {

            public void actionPerformed(ActionEvent evt) {
                chooseEncoding();
            }
        };
        initAvailableEncoding();
        this.iconInput.setIcon(XIPUIUtilities.getIcon(UIConstants.INPUT));
        this.iconXml.setIcon(XIPUIUtilities.getIcon(UIConstants.XML));
        this.iconParsed.setIcon(XIPUIUtilities.getIcon(UIConstants.TOPARSE));
        initWithCorpusElement();
    }

    public void showInDialog(boolean modal) {
        JDialog dialog = new JDialog();
        if (elements.length > 1) {
            dialog.setTitle("Properties for : " + elements.length + " files");
        } else {
            dialog.setTitle("Properties for : " + elements[0].toString());
        }
        dialog.setSize(500, 400);
        dialog.setContentPane(this);
        dialog.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);
        dialog.setLocationByPlatform(true);
        dialog.setModal(modal);
        dialog.setVisible(true);
    }

    private void initAvailableEncoding() {
        encoding.addItem(FileTools.ASCII);
        encoding.addItem(FileTools.ISO_8859_1);
        encoding.addItem(FileTools.CP1252);
        encoding.addItem(FileTools.UTF_8);
        encoding.addItem(FileTools.UTF_16);
        encoding.addActionListener(actionEncoding);
    }

    private void initWithCorpusElement() {
        //nom du fichier
        if (elements.length > 1) {
            name.setText("several files selected");
            name.setToolTipText("");
            absolutePath.setText("several files selected");
            absolutePath.setToolTipText("");
            date.setText("several files selected");
            //isXML ?
            xml.setSelected(false);
            //input
            input.setSelected(false);
            input.setEnabled(false);
            //description
            description.setText("");
            //parent
            parent.setText("several files selected");
            toParse.setSelected(false);
            toParse.setEnabled(false);
            startParse.setValue(-1);
            endParse.setValue(-1);
        } else {
            name.setText(elements[0].getFile().getName());
            name.setToolTipText(elements[0].getFile().getName());
            name.setCaretPosition(0);
            //path absolu du fichier
            absolutePath.setText(elements[0].getFile().getAbsolutePath());
            absolutePath.setToolTipText(elements[0].getFile().getAbsolutePath());
            absolutePath.setCaretPosition(0);
            date.setText(elements[0].getDate());
            //isXML ?
            xml.setSelected(elements[0].isXML());
            //input
            input.setSelected(elements[0].isInput());
            //description
            description.setText(elements[0].getDescription());
            //parent
            parent.setText(elements[0].getParent());
            toParse.setSelected(elements[0].isToParse());
            startParse.setValue(elements[0].getStartLine());
            endParse.setValue(elements[0].getEndLine());
            if (elements[0].isInput()) {
                this.encoding.setEnabled(false);
                this.input.setEnabled(false);
            }
        }
        //encodage
        if (FileTools.isUnknownEncoding(elements[0].getEncoding())) {
            this.encoding.setSelectedItem(FileTools.ENCODING_DEFAULT);
        } else {
            this.encoding.setSelectedItem(elements[0].getEncoding());
        }
        if (elements.length == 1 && elements[0].isDirectory()) {
            labelName.setIcon(XIPUIUtilities.getIcon(UIConstants.OPEN));
            labelName.setToolTipText("This corpus is a directory");
        }
        this.size.setText("wait...");
        SizeTask task = new SizeTask();
        task.execute();
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc=" Generated Code ">//GEN-BEGIN:initComponents
    private void initComponents() {
        java.awt.GridBagConstraints gridBagConstraints;

        labelName = new javax.swing.JLabel();
        name = new javax.swing.JTextField();
        labelPath = new javax.swing.JLabel();
        absolutePath = new javax.swing.JTextField();
        labelEncoding = new javax.swing.JLabel();
        encoding = new javax.swing.JComboBox();
        xml = new javax.swing.JCheckBox();
        labelDescription = new javax.swing.JLabel();
        scroller = new javax.swing.JScrollPane();
        description = new javax.swing.JTextArea();
        labelParent = new javax.swing.JLabel();
        size = new javax.swing.JTextField();
        labelSize = new javax.swing.JLabel();
        parent = new javax.swing.JTextField();
        labelDate = new javax.swing.JLabel();
        date = new javax.swing.JTextField();
        input = new javax.swing.JCheckBox();
        toParse = new javax.swing.JCheckBox();
        iconXml = new javax.swing.JLabel();
        iconInput = new javax.swing.JLabel();
        iconParsed = new javax.swing.JLabel();
        lab1_ = new javax.swing.JLabel();
        endParse = new javax.swing.JSpinner();
        lab_2 = new javax.swing.JLabel();
        startParse = new javax.swing.JSpinner();
        keepValue = new javax.swing.JCheckBox();

        setLayout(new java.awt.GridBagLayout());

        setBackground(new java.awt.Color(255, 255, 255));
        labelName.setBackground(new java.awt.Color(153, 255, 153));
        labelName.setText("Name");
        labelName.setBorder(javax.swing.BorderFactory.createMatteBorder(0, 0, 2, 0, new java.awt.Color(255, 102, 51)));
        labelName.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(10, 10, 2, 4);
        add(labelName, gridBagConstraints);

        name.setEditable(false);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridwidth = 6;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(10, 4, 2, 10);
        add(name, gridBagConstraints);

        labelPath.setBackground(new java.awt.Color(153, 255, 153));
        labelPath.setText("FullName");
        labelPath.setBorder(javax.swing.BorderFactory.createMatteBorder(0, 0, 2, 0, new java.awt.Color(255, 102, 51)));
        labelPath.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(2, 10, 2, 4);
        add(labelPath, gridBagConstraints);

        absolutePath.setEditable(false);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.gridwidth = 6;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 4, 2, 10);
        add(absolutePath, gridBagConstraints);

        labelEncoding.setBackground(new java.awt.Color(153, 255, 153));
        labelEncoding.setText("Encoding");
        labelEncoding.setBorder(javax.swing.BorderFactory.createMatteBorder(0, 0, 2, 0, new java.awt.Color(255, 102, 51)));
        labelEncoding.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(2, 10, 2, 4);
        add(labelEncoding, gridBagConstraints);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.gridwidth = 6;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 4, 2, 10);
        add(encoding, gridBagConstraints);

        xml.setText("is XML");
        xml.setBorder(javax.swing.BorderFactory.createEmptyBorder(0, 0, 0, 0));
        xml.setMargin(new java.awt.Insets(0, 0, 0, 0));
        xml.setOpaque(false);
        xml.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                xmlActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 4);
        add(xml, gridBagConstraints);

        labelDescription.setBackground(new java.awt.Color(153, 255, 153));
        labelDescription.setText("Description");
        labelDescription.setBorder(javax.swing.BorderFactory.createMatteBorder(0, 0, 2, 0, new java.awt.Color(255, 102, 51)));
        labelDescription.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 8;
        gridBagConstraints.gridwidth = 7;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 10, 2, 10);
        add(labelDescription, gridBagConstraints);

        description.setColumns(20);
        description.setLineWrap(true);
        description.setRows(5);
        description.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyReleased(java.awt.event.KeyEvent evt) {
                descriptionKeyReleased(evt);
            }
        });

        scroller.setViewportView(description);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 9;
        gridBagConstraints.gridwidth = 7;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 10, 10, 10);
        add(scroller, gridBagConstraints);

        labelParent.setBackground(new java.awt.Color(153, 255, 153));
        labelParent.setText("Parent");
        labelParent.setBorder(javax.swing.BorderFactory.createMatteBorder(0, 0, 2, 0, new java.awt.Color(255, 102, 51)));
        labelParent.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 6;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(2, 10, 2, 4);
        add(labelParent, gridBagConstraints);

        size.setEditable(false);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 7;
        gridBagConstraints.gridwidth = 6;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 4, 2, 10);
        add(size, gridBagConstraints);

        labelSize.setBackground(new java.awt.Color(153, 255, 153));
        labelSize.setText("size");
        labelSize.setBorder(javax.swing.BorderFactory.createMatteBorder(0, 0, 2, 0, new java.awt.Color(255, 102, 51)));
        labelSize.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 7;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(2, 10, 2, 4);
        add(labelSize, gridBagConstraints);

        parent.setEditable(false);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 6;
        gridBagConstraints.gridwidth = 6;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 4, 2, 10);
        add(parent, gridBagConstraints);

        labelDate.setBackground(new java.awt.Color(153, 255, 153));
        labelDate.setText("Date");
        labelDate.setBorder(javax.swing.BorderFactory.createMatteBorder(0, 0, 2, 0, new java.awt.Color(255, 102, 51)));
        labelDate.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(2, 10, 2, 4);
        add(labelDate, gridBagConstraints);

        date.setEditable(false);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.gridwidth = 6;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 4, 2, 10);
        add(date, gridBagConstraints);

        input.setText("is Input");
        input.setBorder(javax.swing.BorderFactory.createEmptyBorder(0, 0, 0, 0));
        input.setEnabled(false);
        input.setMargin(new java.awt.Insets(0, 0, 0, 0));
        input.setOpaque(false);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(2, 4, 2, 2);
        add(input, gridBagConstraints);

        toParse.setText("is prepared to be parsed");
        toParse.setBorder(javax.swing.BorderFactory.createEmptyBorder(0, 0, 0, 0));
        toParse.setEnabled(false);
        toParse.setMargin(new java.awt.Insets(0, 0, 0, 0));
        toParse.setOpaque(false);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 5;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 10);
        add(toParse, gridBagConstraints);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHEAST;
        gridBagConstraints.insets = new java.awt.Insets(2, 10, 2, 2);
        add(iconXml, gridBagConstraints);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHEAST;
        gridBagConstraints.insets = new java.awt.Insets(2, 6, 2, 2);
        add(iconInput, gridBagConstraints);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 4;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHEAST;
        gridBagConstraints.insets = new java.awt.Insets(2, 6, 2, 2);
        add(iconParsed, gridBagConstraints);

        lab1_.setFont(new java.awt.Font("Arial", 0, 11));
        lab1_.setText("Parse from");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 5;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        add(lab1_, gridBagConstraints);

        endParse.setToolTipText("-1 for the end of file");
        endParse.setMaximumSize(new java.awt.Dimension(60, 18));
        endParse.setMinimumSize(new java.awt.Dimension(60, 18));
        endParse.setPreferredSize(new java.awt.Dimension(60, 18));
        endParse.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                endParseStateChanged(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 5;
        gridBagConstraints.gridy = 5;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        add(endParse, gridBagConstraints);

        lab_2.setFont(new java.awt.Font("Arial", 0, 11));
        lab_2.setText(" to ");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 4;
        gridBagConstraints.gridy = 5;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        add(lab_2, gridBagConstraints);

        startParse.setToolTipText("-1 or 0 for the first line");
        startParse.setMaximumSize(new java.awt.Dimension(60, 18));
        startParse.setMinimumSize(new java.awt.Dimension(60, 18));
        startParse.setPreferredSize(new java.awt.Dimension(60, 18));
        startParse.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                startParseStateChanged(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 5;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        add(startParse, gridBagConstraints);

        keepValue.setFont(new java.awt.Font("Arial", 0, 11));
        keepValue.setText("keep Values");
        keepValue.setBorder(javax.swing.BorderFactory.createEmptyBorder(0, 0, 0, 0));
        keepValue.setMargin(new java.awt.Insets(0, 0, 0, 0));
        keepValue.setOpaque(false);
        keepValue.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                keepValueActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 6;
        gridBagConstraints.gridy = 5;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        add(keepValue, gridBagConstraints);

    }// </editor-fold>//GEN-END:initComponents
    private void keepValueActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_keepValueActionPerformed
        for (int i = 0; i < elements.length; i++) {
            elements[i].setKeepValueLine(keepValue.isSelected());
        }
    }//GEN-LAST:event_keepValueActionPerformed

    private void endParseStateChanged(javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_endParseStateChanged
        Object source = evt.getSource();
        if (source instanceof JSpinner) {
            Integer numSent = (Integer) endParse.getValue();
            for (int i = 0; i < elements.length; i++) {
                elements[i].setEndLine(numSent.intValue());
            }
        }
    }//GEN-LAST:event_endParseStateChanged

    private void startParseStateChanged(javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_startParseStateChanged
        Object source = evt.getSource();
        if (source instanceof JSpinner) {
            Integer numSent = (Integer) startParse.getValue();
            for (int i = 0; i < elements.length; i++) {
                elements[i].setStartLine(numSent.intValue());
            }
        }
    }//GEN-LAST:event_startParseStateChanged

    private void xmlActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_xmlActionPerformed
        for (int i = 0; i < elements.length; i++) {
            elements[i].setXML(this.xml.isSelected());
        }
        if (tree != null) {
            tree.repaint();
        }
    }//GEN-LAST:event_xmlActionPerformed

    private void chooseEncoding() {
        Object o = encoding.getSelectedItem();
        if (o != null) {
            for (int i = 0; i < elements.length; i++) {
                if (!elements[i].isInput()) {
                    elements[i].setEncoding(o.toString());
                }
            }
        }
        if (tree != null) {
            tree.repaint();
        }
    }

    private void descriptionKeyReleased(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_descriptionKeyReleased
        if (evt.getID() == KeyEvent.KEY_RELEASED) {
            if (evt.getKeyCode() == KeyEvent.VK_ENTER) {
                for (int i = 0; i < elements.length; i++) {
                    elements[i].setDescription(description.getText());
                }
            }
        }
    }//GEN-LAST:event_descriptionKeyReleased
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JTextField absolutePath;
    private javax.swing.JTextField date;
    private javax.swing.JTextArea description;
    private javax.swing.JComboBox encoding;
    private javax.swing.JSpinner endParse;
    private javax.swing.JLabel iconInput;
    private javax.swing.JLabel iconParsed;
    private javax.swing.JLabel iconXml;
    private javax.swing.JCheckBox input;
    private javax.swing.JCheckBox keepValue;
    private javax.swing.JLabel lab1_;
    private javax.swing.JLabel lab_2;
    private javax.swing.JLabel labelDate;
    private javax.swing.JLabel labelDescription;
    private javax.swing.JLabel labelEncoding;
    private javax.swing.JLabel labelName;
    private javax.swing.JLabel labelParent;
    private javax.swing.JLabel labelPath;
    private javax.swing.JLabel labelSize;
    private javax.swing.JTextField name;
    private javax.swing.JTextField parent;
    private javax.swing.JScrollPane scroller;
    private javax.swing.JTextField size;
    private javax.swing.JSpinner startParse;
    private javax.swing.JCheckBox toParse;
    private javax.swing.JCheckBox xml;
    // End of variables declaration//GEN-END:variables

    private class EncodingRenderer implements ListCellRenderer {

        public EncodingRenderer() {
        }

        public Component getListCellRendererComponent(JList list, Object value, int index, boolean isSelected, boolean cellHasFocus) {
            if (value != null && value instanceof String) {
                JLabel label = new JLabel();
                label.setFont(UIConstants.DEFAULT_FONT);
                label.setOpaque(true);
                label.setBackground(isSelected ? Color.lightGray : Color.white);
                label.setForeground(isSelected ? Color.red : Color.black);
                String encoding_ = (String) value;
                if (encoding_.equalsIgnoreCase(FileTools.ENCODING_DEFAULT)) {
                    label.setText(encoding_ + " (default Plateform)");
                } else {
                    label.setText(encoding_);
                }
                return label;
            } else {
                return new JLabel();
            }
        }
    }

    private class SizeTask extends SwingWorker<Void, Number> {

        public SizeTask() {
        }

        @Override
        protected Void doInBackground() {
            try {
                long cpt = 0;
                for (int i = 0; i < elements.length; i++) {
                    cpt = cpt + elements[i].size();
                }
                long humanSize = (cpt / 1024);
                long rest = cpt % 1024;
                if (rest > 0) {
                    humanSize += 1;
                }
                Number s = humanSize;
                publish(s);
                if (elements.length == 1 && elements[0].isDirectory()) {
                    Number nFiles = elements[0].getCountFiles();
                    publish(nFiles);
                }
            } catch (Exception e) {
            }
            return null;
        }

        public void process(java.util.List<Number> sizes) {
            Number humanSize = sizes.get(0);
            String nbFiles = " [" + size + " file(s)]";
            if (sizes.size() > 1) {
                size.setText(humanSize + " Kbytes [" + sizes.get(1) + " file(s)]");
            } else {
                size.setText(humanSize + " Kbytes");
            }
        }
    }
}
