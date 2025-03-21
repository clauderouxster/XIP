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

import com.xerox.xrce.xipui.ProjectContext;
import com.xerox.xrce.xipui.ProjectFactory;
import com.xerox.xrce.xipui.modules.ModuleContext;
import javax.swing.*;
import java.io.*;
import java.util.*;

/**
 *
 * @author  grassaud
 */
public class InitProjectStep extends AbstractStep {

    public InitProjectStep(ModuleContext context, Hashtable properties) {
        super(context);
        setManagerProperties(properties);
        setDescription("Choose name and Location");
        initComponents();
    }

    public void initStepManager() {
        coment.setText(managerProperties.get(ProjectFactory.COMMENT).toString());
        nameProject.setText(managerProperties.get(ProjectFactory.NAME).toString());
        // the default folder
        folderView.setText(managerProperties.get(ProjectFactory.FOLDER).toString());
        browseChooser.setCurrentDirectory(new File((String) getProperty(project_directory)));
        information.setValueAt(managerProperties.get(ProjectFactory.AUTHOR), 0, 1);
        information.setValueAt(managerProperties.get(ProjectFactory.VERSION), 1, 1);

        browseButton.setIcon(XIPUIUtilities.getIcon(UIConstants.OPEN));
        browseButton.setText("");
    }

    public boolean isValide() {
        boolean success = false;
        try {
            setProperty(project_directory, getFolderProject());
            managerProperties.put(ProjectFactory.NAME, getProjectName());
            managerProperties.put(ProjectFactory.VERSION, getVersion());
            managerProperties.put(ProjectFactory.FOLDER, getFolderProject());
            managerProperties.put(ProjectFactory.COMMENT, getComment());
            managerProperties.put(ProjectFactory.AUTHOR, getAuthor());
            String fullName = getFolderProject() + "/" + getProjectName();
            File fTest = new File(fullName.replace('\\', '/'));
            if (fTest.exists()) {
                warning("the file : " + fullName + " already exists, <br> Change the name or the folder ");
                success = false;
            } else {
                ProjectContext context = new ProjectContext(getFolderProject(), getProjectName());
                managerProperties.put(ProjectFactory.project_context, context);
                success = true;
            }
        } catch (Exception e) {
            severe("ERROR ", e);
            success = false;
        }
        return success;
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc=" Generated Code ">//GEN-BEGIN:initComponents
    private void initComponents() {
        java.awt.GridBagConstraints gridBagConstraints;

        browseDialog = new javax.swing.JDialog();
        browseChooser = new javax.swing.JFileChooser();
        buttonGroup1 = new javax.swing.ButtonGroup();
        jLabel1 = new javax.swing.JLabel();
        nameProject = new javax.swing.JTextField();
        browseButton = new javax.swing.JButton();
        folderView = new javax.swing.JTextField();
        labelProject = new javax.swing.JLabel();
        information = new javax.swing.JTable();
        infoComent = new javax.swing.JLabel();
        jScrollPane1 = new javax.swing.JScrollPane();
        coment = new javax.swing.JTextArea();
        labelInfo = new javax.swing.JLabel();

        browseDialog.getContentPane().add(browseChooser, java.awt.BorderLayout.CENTER);

        setLayout(new java.awt.GridBagLayout());

        setBackground(new java.awt.Color(255, 255, 255));
        setFont(new java.awt.Font("Arial", 0, 11));
        setMaximumSize(new java.awt.Dimension(400, 280));
        setMinimumSize(new java.awt.Dimension(400, 280));
        setPreferredSize(new java.awt.Dimension(400, 280));
        jLabel1.setBackground(new java.awt.Color(153, 255, 153));
        jLabel1.setFont(new java.awt.Font("Arial", 0, 11));
        jLabel1.setText("Project Name");
        jLabel1.setBorder(javax.swing.BorderFactory.createMatteBorder(0, 0, 2, 0, new java.awt.Color(255, 102, 51)));
        jLabel1.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        add(jLabel1, gridBagConstraints);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.ipadx = 84;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        add(nameProject, gridBagConstraints);

        browseButton.setFont(new java.awt.Font("Arial", 0, 11));
        browseButton.setText("Folder\n");
        browseButton.setToolTipText("Choose your folder");
        browseButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                browseButtonActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.insets = new java.awt.Insets(4, 23, 4, 4);
        add(browseButton, gridBagConstraints);

        folderView.setEditable(false);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(4, 6, 4, 4);
        add(folderView, gridBagConstraints);

        labelProject.setBackground(new java.awt.Color(153, 255, 153));
        labelProject.setFont(new java.awt.Font("Arial", 0, 11));
        labelProject.setText("Project Directory");
        labelProject.setBorder(javax.swing.BorderFactory.createMatteBorder(0, 0, 2, 0, new java.awt.Color(255, 102, 0)));
        labelProject.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.gridwidth = java.awt.GridBagConstraints.REMAINDER;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        add(labelProject, gridBagConstraints);

        information.setBorder(javax.swing.BorderFactory.createEtchedBorder());
        information.setFont(new java.awt.Font("Arial", 0, 11));
        information.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {
                {"Author :", null},
                {"Version :", null}
            },
            new String [] {
                "label", "entry"
            }
        ) {
            Class[] types = new Class [] {
                java.lang.String.class, java.lang.String.class
            };
            boolean[] canEdit = new boolean [] {
                false, true
            };

            public Class getColumnClass(int columnIndex) {
                return types [columnIndex];
            }

            public boolean isCellEditable(int rowIndex, int columnIndex) {
                return canEdit [columnIndex];
            }
        });
        information.setAutoCreateColumnsFromModel(false);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 7;
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(9, 6, 4, 4);
        add(information, gridBagConstraints);

        infoComent.setFont(new java.awt.Font("Arial", 0, 11));
        infoComent.setForeground(new java.awt.Color(153, 51, 0));
        infoComent.setText("Project Comment");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 8;
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.ipadx = 38;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(7, 6, 0, 0);
        add(infoComent, gridBagConstraints);

        jScrollPane1.setBorder(null);
        jScrollPane1.setAutoscrolls(true);
        jScrollPane1.setMaximumSize(new java.awt.Dimension(200, 50));
        jScrollPane1.setMinimumSize(new java.awt.Dimension(180, 48));
        coment.setFont(new java.awt.Font("Arial", 0, 10));
        coment.setLineWrap(true);
        coment.setWrapStyleWord(true);
        coment.setBorder(javax.swing.BorderFactory.createEtchedBorder());
        coment.setMaximumSize(new java.awt.Dimension(180, 50));
        coment.setMinimumSize(new java.awt.Dimension(180, 48));
        coment.setPreferredSize(new java.awt.Dimension(180, 48));
        jScrollPane1.setViewportView(coment);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 9;
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(4, 6, 4, 4);
        add(jScrollPane1, gridBagConstraints);

        labelInfo.setBackground(new java.awt.Color(153, 255, 153));
        labelInfo.setFont(new java.awt.Font("Arial", 0, 11));
        labelInfo.setText("Project information");
        labelInfo.setBorder(javax.swing.BorderFactory.createMatteBorder(0, 0, 2, 0, new java.awt.Color(255, 102, 0)));
        labelInfo.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 6;
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        add(labelInfo, gridBagConstraints);

    }// </editor-fold>//GEN-END:initComponents

    /**
     * Importation du dossier ou le projet doit etre creer
     * @param evt ActioEvent "clique sur le bouton browse"
     */
    private void browseButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_browseButtonActionPerformed
        browseChooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
        int returnVal = browseChooser.showOpenDialog(browseDialog);
        if (returnVal == JFileChooser.APPROVE_OPTION) {
            File folderFile = browseChooser.getSelectedFile();
            folderView.setText(folderFile.getAbsolutePath());
        }
    }//GEN-LAST:event_browseButtonActionPerformed

    /**
     *@return renvoie le nom du projet renseigne par l'utilisateur.
     */
    public String getProjectName() {
        return this.nameProject.getText();
    }

    /*
     *@return renvoie le path du projet
     */
    public String getFolderProject() {
        return this.folderView.getText();
    }

    /*
     *Renvoie le nom de l'auteur du projet
     *@return le nom de l'auteur
     */
    public String getAuthor() {
        return (String) information.getValueAt(0, 1);
    }

    /*
     *@return le numero de version du projet
     */
    public String getVersion() {
        return (String) information.getValueAt(1, 1);
    }

    /*
     *@return les commentaires du projet
     */
    public String getComment() {
        return coment.getText();
    }

    public void loadModePolicy(short mode) {
        //TODO
    }

    public void close() {
    }
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton browseButton;
    private javax.swing.JFileChooser browseChooser;
    private javax.swing.JDialog browseDialog;
    private javax.swing.ButtonGroup buttonGroup1;
    private javax.swing.JTextArea coment;
    private javax.swing.JTextField folderView;
    private javax.swing.JLabel infoComent;
    private javax.swing.JTable information;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JLabel labelInfo;
    private javax.swing.JLabel labelProject;
    private javax.swing.JTextField nameProject;
    // End of variables declaration//GEN-END:variables
}
