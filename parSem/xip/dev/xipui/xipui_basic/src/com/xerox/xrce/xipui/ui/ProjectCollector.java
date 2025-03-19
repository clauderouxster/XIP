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

import com.xerox.xrce.xipui.FileFilterXip;
import com.xerox.xrce.xipui.XipUIContext;
import com.xerox.xrce.xipui.ProfileManager;
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.ProjectFactory;
import com.xerox.xrce.xipui.ProjectReferenced;
import com.xerox.xrce.xipui.Xip;
import com.xerox.xrce.xipui.modules.DialogModule;
import com.xerox.xrce.xipui.modules.IModule;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.task.TaskManager;
import com.xerox.xrce.xipui.util.PropertiesTableModel;
import com.xerox.xrce.xipui.util.PropertyCellRenderer;
import com.xerox.xrce.xipui.util.XML;
import org.dom4j.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.table.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;

/**
 *
 * @author  vgrassau
 */
public class ProjectCollector extends DialogModule {

    private XipUIContext context;
    private OpenProjectAction openAction;
    private ProjectReferenced projectSelected;
    private JMenuItem[] shortCutMenu;

    /** Creates new form BeanForm
     * @param context 
     */
    public ProjectCollector(XipUIContext context) {
        super();
        this.context = context;
        openAction = new OpenProjectAction();
        initComponents();
        setTitle("Project Manager");
        setSize(640, 480);
        setModal(true);
        tablePane.add(tableAbout.getTableHeader(), BorderLayout.PAGE_START);
        aboutLabel.setIcon(XIPUIUtilities.getIcon(UIConstants.PROPERTIES));
        profileBox.removeAllItems();
        profileBox.setToolTipText("Click on cell to change the profile");
        //we initialize the profile combobox
        ProfileManager profiler = ProfileManager.getExistedInstance();
        Collection<String> profiles = profiler.getProfiles();
        Iterator ite = profiles.iterator();
        while (ite.hasNext()) {
            profileBox.addItem(ite.next());
        }
        projectTable.setModel(initAvailableProjects());
        ListSelectionModel rowSM = projectTable.getSelectionModel();
        rowSM.addListSelectionListener(new ListSelectionListener() {

            public void valueChanged(ListSelectionEvent e) {
                selectionOnProjectTable(e);
            }
        });
        TableColumn profileColumn = projectTable.getColumnModel().getColumn(1);
        profileColumn.setCellEditor(new DefaultCellEditor(profileBox));
        TableColumn projectColumn = projectTable.getColumnModel().getColumn(0);
        projectColumn.setCellRenderer(new TableOpenedProjectRenderer());
        setCreateAction();
        importP.setIcon(XIPUIUtilities.getIcon(UIConstants.OPEN));
        delete.setIcon(XIPUIUtilities.getIcon(UIConstants.CLEAR));
        open.setIcon(XIPUIUtilities.getIcon(UIConstants.PROJECT));
    }

    public void setCreateAction() {
        AbstractAction action = new AbstractAction("New", XIPUIUtilities.getIcon(UIConstants.CREATE)) {

            public void actionPerformed(ActionEvent evt) {
                createProject();
            }
        };
        newP.setAction(action);
    }

    public void refreshTable() {
        projectTable.setModel(initAvailableProjects());
        TableColumn profileColumn = projectTable.getColumnModel().getColumn(1);
        profileColumn.setCellEditor(new DefaultCellEditor(profileBox));
        TableColumn projectColumn = projectTable.getColumnModel().getColumn(0);
        projectColumn.setCellRenderer(new TableOpenedProjectRenderer());
        projectTable.repaint();
    }

    private DefaultTableModel initAvailableProjects() {
        DefaultTableModel model = new DefaultTableModel();
        Collection<ProjectReferenced> refs = context.getAllProjects();
        Iterator<ProjectReferenced> ite = refs.iterator();
        Object[][] data = new Object[refs.size()][2];
        String[] columns = {"Project", "Profile"};
        int i = 0;
        while (ite.hasNext()) {
            ProjectReferenced ref = ite.next();
            data[i][0] = ref;
            data[i][1] = ref.getProfile();
            i++;
        }
        model.setDataVector(data, columns);
        return model;
    }

    public JMenuItem[] createShortcutMenu() {
        java.util.List list = context.getProjects();
        if (list != null) {
            Iterator ite = list.iterator();
            shortCutMenu = new JMenuItem[list.size()];
            for (int j = 0; j < list.size(); j++) {
                final ProjectReferenced op = (ProjectReferenced) ite.next();
                JMenuItem jMenu = XIPUIUtilities.createJMenuItem(op.getName(), null, XIPUIUtilities.getIcon(UIConstants.PROJECT));
                OpenProjectAction action = new OpenProjectAction();
                action.setProject(op);
                action.putValue(Action.NAME, op.getName());
                action.putValue(Action.SMALL_ICON, XIPUIUtilities.getIcon(UIConstants.PROJECT));
                action.putValue(Action.SHORT_DESCRIPTION, op.getProjectPath() + " (" + op.getProfile() + ")");
                jMenu.setAction(action);
                shortCutMenu[j] = jMenu;
            }
            return shortCutMenu;
        }
        return null;
    }

    private void selectionOnProjectTable(ListSelectionEvent e) {
        //Ignore extra messages.
        if (e.getValueIsAdjusting()) {
            return;
        }
        ListSelectionModel lsm = (ListSelectionModel) e.getSource();
        if (!lsm.isSelectionEmpty()) {
            int selectedRow = lsm.getMinSelectionIndex();
            ProjectReferenced project = (ProjectReferenced) projectTable.getValueAt(selectedRow, 0);
            projectSelected = project;
            openAction.setProject(project);
            initAboutTable(project);
        }
    }

    public void close() {
        try {
            context.save();
        } catch (Exception e) {
        }
        dispose();
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
// <editor-fold defaultstate="collapsed" desc=" Generated Code ">//GEN-BEGIN:initComponents
    private void initComponents() {
        java.awt.GridBagConstraints gridBagConstraints;
        
        profileBox = new javax.swing.JComboBox();
        openDialog = new javax.swing.JDialog();
        openChooser = new javax.swing.JFileChooser();
        availableLabel = new javax.swing.JLabel();
        panel = new javax.swing.JPanel();
        close = new javax.swing.JButton();
        cPane = new javax.swing.JPanel();
        importP = new javax.swing.JButton();
        newP = new javax.swing.JButton();
        delete = new javax.swing.JButton();
        open = new javax.swing.JButton();
        aboutLabel = new javax.swing.JLabel();
        scroll3 = new javax.swing.JScrollPane();
        aboutPanel = new javax.swing.JPanel();
        tablePane = new javax.swing.JPanel();
        tableAbout = new javax.swing.JTable();
        varLabel = new javax.swing.JLabel();
        varTable = new javax.swing.JTable();
        scroll1 = new javax.swing.JScrollPane();
        projectTable = new javax.swing.JTable();
        
        profileBox.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "Item 1", "Item 2", "Item 3", "Item 4" }));
        profileBox.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                profileBoxActionPerformed(evt);
            }
        });
        
        openDialog.setTitle("Import Project");
        openDialog.getContentPane().add(openChooser, java.awt.BorderLayout.CENTER);
        
        
        getContentPane().setLayout(new java.awt.GridBagLayout());
        
        setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);
        setLocationByPlatform(true);
        setModal(true);
        availableLabel.setBackground(new java.awt.Color(153, 255, 153));
        availableLabel.setFont(new java.awt.Font("Arial", 0, 11));
        availableLabel.setText("Available projects");
        availableLabel.setBorder(javax.swing.BorderFactory.createMatteBorder(0, 0, 2, 0, new java.awt.Color(255, 102, 51)));
        availableLabel.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridwidth = java.awt.GridBagConstraints.REMAINDER;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(6, 6, 6, 6);
        getContentPane().add(availableLabel, gridBagConstraints);
        
        panel.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.RIGHT));
        
        close.setFont(new java.awt.Font("Arial", 0, 11));
        close.setText("Close");
        close.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                closeActionPerformed(evt);
            }
        });
        
        panel.add(close);
        
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.gridwidth = java.awt.GridBagConstraints.REMAINDER;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(6, 6, 6, 6);
        getContentPane().add(panel, gridBagConstraints);
        
        cPane.setLayout(new java.awt.GridBagLayout());
        
        importP.setFont(new java.awt.Font("Arial", 0, 11));
        importP.setText("Browse");
        importP.setToolTipText("Browse for import a project");
        importP.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                importPActionPerformed(evt);
            }
        });
        
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTH;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        cPane.add(importP, gridBagConstraints);
        
        newP.setFont(new java.awt.Font("Arial", 0, 11));
        newP.setText("New");
        newP.setToolTipText("Create a new project");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.ipadx = 2;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTH;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        cPane.add(newP, gridBagConstraints);
        
        delete.setFont(new java.awt.Font("Arial", 0, 11));
        delete.setText("delete");
        delete.setToolTipText("Delete project");
        delete.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                deleteActionPerformed(evt);
            }
        });
        
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTH;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        cPane.add(delete, gridBagConstraints);
        
        open.setFont(new java.awt.Font("Arial", 0, 11));
        open.setText("Open");
        open.setToolTipText("Open the selected project");
        open.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                openActionPerformed(evt);
            }
        });
        
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTH;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        cPane.add(open, gridBagConstraints);
        
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.fill = java.awt.GridBagConstraints.VERTICAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTH;
        gridBagConstraints.insets = new java.awt.Insets(0, 0, 7, 7);
        getContentPane().add(cPane, gridBagConstraints);
        
        aboutLabel.setBackground(new java.awt.Color(153, 255, 153));
        aboutLabel.setFont(new java.awt.Font("Arial", 0, 11));
        aboutLabel.setText("Properties");
        aboutLabel.setBorder(javax.swing.BorderFactory.createMatteBorder(0, 0, 2, 0, new java.awt.Color(255, 102, 51)));
        aboutLabel.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.gridwidth = java.awt.GridBagConstraints.REMAINDER;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 8, 1, 9);
        getContentPane().add(aboutLabel, gridBagConstraints);
        
        aboutPanel.setLayout(new java.awt.GridBagLayout());
        
        tablePane.setLayout(new java.awt.BorderLayout());
        
        tablePane.add(tableAbout, java.awt.BorderLayout.CENTER);
        
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        aboutPanel.add(tablePane, gridBagConstraints);
        
        varLabel.setBackground(new java.awt.Color(153, 255, 153));
        varLabel.setFont(new java.awt.Font("Arial", 0, 11));
        varLabel.setText("Variables");
        varLabel.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        aboutPanel.add(varLabel, gridBagConstraints);
        
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        aboutPanel.add(varTable, gridBagConstraints);
        
        scroll3.setViewportView(aboutPanel);
        
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.gridwidth = java.awt.GridBagConstraints.REMAINDER;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(5, 8, 5, 8);
        getContentPane().add(scroll3, gridBagConstraints);
        
        scroll1.setBackground(new java.awt.Color(255, 255, 255));
        scroll1.setMaximumSize(new java.awt.Dimension(160, 150));
        scroll1.setMinimumSize(new java.awt.Dimension(160, 150));
        scroll1.setPreferredSize(new java.awt.Dimension(160, 150));
        projectTable.setAutoResizeMode(javax.swing.JTable.AUTO_RESIZE_NEXT_COLUMN);
        projectTable.setShowHorizontalLines(false);
        projectTable.setShowVerticalLines(false);
        scroll1.setViewportView(projectTable);
        
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(0, 7, 7, 0);
        getContentPane().add(scroll1, gridBagConstraints);
        
    }// </editor-fold>//GEN-END:initComponents

    private void openActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_openActionPerformed
        int row = projectTable.getSelectedRow();
        if (row != -1) {
            ProjectReferenced project = (ProjectReferenced) projectTable.getValueAt(row, 0);
            if (!project.isOpened()) {
                openAction.open();
            }
        }
    }//GEN-LAST:event_openActionPerformed

    private void profileBoxActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_profileBoxActionPerformed
        if (projectSelected != null) {
            String profile = (String) profileBox.getSelectedItem();
            String old_profile = projectSelected.getProfile();
            projectSelected.setProfile(profile);
            //we must change the profile on fly
            //if the projectSelected is opened and if the profile is different
            if (projectSelected.isOpened() && !projectSelected.getProfile().equals(old_profile)) {
                ModuleContext context_ = new ModuleContext();
                context_.setProperty("reference", projectSelected);
                context_.setProperty("old_profile", old_profile);
                TaskManager manager = TaskManager.getInstance();
                manager.invoke(TaskManager.Command.SWITCH_PROFILE, context_);
            // log.info("je change de profile : " + profile);
            }
        }
    }//GEN-LAST:event_profileBoxActionPerformed

    private void deleteActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_deleteActionPerformed
        if (projectSelected != null && !projectSelected.isOpened()) {
            int res = JOptionPane.showConfirmDialog(this, "<html><body><p>Are you sure to delete <b>" + projectSelected + " </b>project ?</p></body></html>");
            if (res == JOptionPane.OK_OPTION) {
                context.removeProject(projectSelected);
                refreshTable();
            }
        }
    }//GEN-LAST:event_deleteActionPerformed

    private void importPActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_importPActionPerformed
        FileFilterXip filter = new FileFilterXip();
        filter.addExtension("xml");
        filter.setFilterName("project-config");
        openChooser.setFileFilter(filter);
        String dir = (String) context.getProperty(XipUIContext.project_directory);
        openChooser.setCurrentDirectory(new File(dir));
        int returnVal = openChooser.showOpenDialog(openDialog);
        if (returnVal == JFileChooser.APPROVE_OPTION) {
            // selection du fichier
            File file = openChooser.getSelectedFile();
            context.setProperty(XipUIContext.project_directory, file.getParentFile().getParent());
            TaskManager manager = TaskManager.getInstance();
            ModuleContext initial = createInitialModuleContext();
            ProjectFactory pf = ProjectFactory.getInstance(file, (Xip) initial.getProperty(xip_parser));
            Project project = null;
            if (pf != null) {
                project = pf.createProject();
            }
            if (project == null) {
                log.debug("Error in createProject()");
                warning("Could not import project : " + file);
            } else {
                context.addProject(project);
            }
        }
        projectTable.setModel(initAvailableProjects());
        projectTable.repaint();
        openDialog.setVisible(false);
    }//GEN-LAST:event_importPActionPerformed

    private void closeActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_closeActionPerformed
        close();
    }//GEN-LAST:event_closeActionPerformed

    private void createProject() {
        // we must create a globalContext here
        ModuleContext InitialContext = createInitialModuleContext();
        //creation du manager
        ProjectStepManager projectManager = new ProjectStepManager(InitialContext);
        //affichage
        projectManager.setVisible(true);
        if (!projectManager.isCanceled()) {
            if (projectManager.getProject() != null) {
                context.addProject(projectManager.getProject());
            }
        }
        refreshTable();
    }
// Variables declaration - do not modify//GEN-BEGIN:variables
   private javax.swing.JLabel aboutLabel;
   private javax.swing.JPanel aboutPanel;
   private javax.swing.JLabel availableLabel;
   private javax.swing.JPanel cPane;
   private javax.swing.JButton close;
   private javax.swing.JButton delete;
   private javax.swing.JButton importP;
   private javax.swing.JButton newP;
   private javax.swing.JButton open;
   private javax.swing.JFileChooser openChooser;
   private javax.swing.JDialog openDialog;
   private javax.swing.JPanel panel;
   private javax.swing.JComboBox profileBox;
   private javax.swing.JTable projectTable;
   private javax.swing.JScrollPane scroll1;
   private javax.swing.JScrollPane scroll3;
   private javax.swing.JTable tableAbout;
   private javax.swing.JPanel tablePane;
   private javax.swing.JLabel varLabel;
   private javax.swing.JTable varTable;
// End of variables declaration//GEN-END:variables

    private class TableOpenedProjectRenderer implements TableCellRenderer {

        public TableOpenedProjectRenderer() {
            //do nothing
        }

        /**
         *Return the render for an applied rule
         * @param value must be a Integer, this the index in the vector of applied rules.
         * @param cellHasFocus 
         * @param col 
         * @return a JLabel, if the value parameter is not a Integer then we return an empty label
         *
         */
        public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected, boolean cellHasFocus, int row, int col) {
            JLabel label = new JLabel();
            if (value != null) {
                label.setBackground(isSelected ? new Color(240, 240, 240) : Color.white);
                label.setForeground(isSelected ? Color.red : Color.black);
                label.setOpaque(true);
                if (value instanceof ProjectReferenced) {
                    ProjectReferenced p = (ProjectReferenced) value;
                    if (!p.isOpened()) {
                        label.setText(p.getName());
                    } else {
                        label.setText(p.getName() + " (opened)");
                    }
                    label.setToolTipText(p.getName());
                    if (p.isDeleted() || !p.exists()) {
                        label.setIcon(XIPUIUtilities.getIcon(UIConstants.DELETE));
                    } else if (!p.exists()) {
                        label.setIcon(XIPUIUtilities.getIcon(UIConstants.CODE_ERROR));
                    } else {
                        label.setIcon(XIPUIUtilities.getIcon(UIConstants.PROJECT));
                    }
                }
                return label;
            } else {
                return new JLabel();
            }
        }
    }

    private void initAboutTable(ProjectReferenced project) {
        if (project != null && project.exists()) {
            try {
                aboutLabel.setText(project.getName() + " properties");
                aboutLabel.setToolTipText(project.getProjectPath());
                XML confProject = new XML(project.getProjectPath() + "/project-config.xml");
                Document document = confProject.getDocument();
                Element root1 = document.getRootElement();
                Element context1 = root1.element("context");
                int nbCorpus = root1.elements("corpus").size();
                Element grammar = root1.element("grammar");
                java.util.List variables = context1.elements("variable");
                //we initialize the project table :
                Object[][] data = new Object[][]{
                    {"Name", root1.element("name").getText()},
                    {"Folder", root1.element("folder").getText()},
                    {"grm", grammar.attributeValue("grm")},
                    {"Grammar", grammar.attributeValue("configuration_file")},
                    {"Grammar encoding", grammar.attributeValue("encoding")},
                    {"Grammar crypted", grammar.attributeValue("crypted")},
                    {"Grammar files", grammar.elements().size() + " file(s)"},
                    {"Corpus", nbCorpus + " file(s)"},
                    {"Comment", root1.element("comment").getText()},
                    {"Author", root1.element("author").getText()},
                    {"Version", root1.element("version").getText()},
                };
                boolean[][] editable = {{false, false}, {false, false}, {false, false}, {false, false}, {false, false}, {false, false}, {false, false}, {false, false}, {false, false}, {false, false}, {false, false}};
                tableAbout.setModel(new PropertiesTableModel(data, editable));
                tableAbout.setDefaultRenderer(java.lang.String.class, new PropertyCellRenderer());
                tableAbout.setAutoResizeMode(JTable.AUTO_RESIZE_LAST_COLUMN);
                Object[][] data3 = new Object[variables.size()][2];
                boolean[][] editable3 = new boolean[variables.size()][2];
                Iterator ite3 = variables.iterator();
                int i = 0;
                while (ite3.hasNext()) {
                    Element var = (Element) ite3.next();
                    data3[i][0] = var.attributeValue("name");
                    editable3[i][0] = false;
                    data3[i][1] = var.attributeValue("value");
                    editable3[i][1] = false;
                    i = i + 1;
                }
                varTable.setModel(new PropertiesTableModel(data3, editable3));
                varTable.setDefaultRenderer(java.lang.String.class, new PropertyCellRenderer());
            } catch (Exception e) {
            }
        } else {
            aboutLabel.setText("About");
        }
    }

    private ModuleContext createInitialModuleContext() {
        ModuleContext globalModuleContext = new ModuleContext(context);
        globalModuleContext.setProperty(IModule.module_root, getRootComponent());
        return globalModuleContext;
    }

    public class OpenProjectAction extends AbstractAction {

        private ProjectReferenced project;

        public OpenProjectAction() {
            super("Open project", XIPUIUtilities.getIcon(UIConstants.OPEN));
        }

        public void setProject(ProjectReferenced p) {
            this.project = p;
        }

        public void actionPerformed(ActionEvent evt) {
            open();
        }

        public void open() {
            if (project != null) {
                File file = new File(project.getProjectPath());
                context.setProperty(XipUIContext.project_directory, file.getParent());
                ModuleContext taskContext = new ModuleContext();
                taskContext.setProperty("reference", project);
                taskContext.setProperty("initialContext", createInitialModuleContext());
                TaskManager.getInstance().invoke(TaskManager.Command.LOAD_PROFILE, taskContext);
                context.addProjectReferenced(project);
                project.setOpened(true);
            }
        }
    }
}
