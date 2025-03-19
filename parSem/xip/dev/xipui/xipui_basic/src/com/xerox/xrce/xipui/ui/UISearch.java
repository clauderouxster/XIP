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

import com.xerox.xrce.xipui.Grammar;
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.ProjectContext;
import com.xerox.xrce.xipui.RuleMap;
import com.xerox.xrce.xipui.Search;
import com.xerox.xrce.xipui.ui.editor.EditFileAction;
import com.xerox.xrce.xipui.ui.editor.DefaultEditor;
import com.xerox.xrce.xipui.ui.editor.Editor;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.modules.PanelModule;
import com.xerox.xrce.xipui.util.FileTools;
import java.awt.*;
import javax.swing.*;
import java.awt.event.*;

import javax.swing.tree.*;
import java.util.*;
import java.util.regex.*;
import java.io.*;

/**
 *
 * @author  vgrassau
 */
public class UISearch extends PanelModule {

    // Table fichier offsets pour la recherche dans un repertoire et la grammaire
    private Hashtable fileMap;
    // evenement souris sur la liste d'une recherche
    private MouseListener ml;
    // objet pour effectuer la recherche
    private Search search;
    // Editeur ou effectuer une recherche
    private DefaultEditor editor;
    // Vecteur d'offset contenant le resultat de la recherche sur un editeur
    private Vector enumResearch;
    // Flag pour la recherche
    private int flag = Pattern.LITERAL + Pattern.CASE_INSENSITIVE;
    // Repertoire ou chercher
    private File directory;
    private EditFileAction editFileAction;
    // Curseur pour le vecteur d'offset de la recherche
    private int cursor = 0;
    // Text a chercher
    private String text;

    /** Creates new form UISearch
     * @param properties 
     */
    public UISearch(ModuleContext properties) {
        super(properties);
        editFileAction = new EditFileAction(properties);
        text = null;
        fileMap = new Hashtable();
        Project p = getProject();
        ProjectContext pc = p.getProjectContext();
        File f = new File((String) pc.getProperty(ProjectContext.default_directory));
        this.directory = f;
        initComponents();
        this.find.setIcon(XIPUIUtilities.getIcon(UIConstants.SEARCH));
        this.searchText.requestFocusInWindow();
        this.useGrammar.setToolTipText("Find in " + p.getName() + " grammar project");
        this.directoryText.setText(directory.getAbsolutePath());
        this.directoryText.setToolTipText(directory.getAbsolutePath());
        this.directoryText.setCaretPosition(0);
        tree = new JTree();
        this.search = new Search();
    }

    /**
     *Fait un next sur le resultat d'une recherche pour un editeur
     */
    public void next() {
        this.nextActionPerformed(null);
    }

    /**
     *Fait un search pour un editeur
     */
    public void search() {
        if (editor.getFile() != null) {
            search.setSearch(text, editor.getText(), flag, editor.getFile().getName());
        } else {
            search.setSearch(text, editor.getText(), flag, null);
        }
        search.find();
        enumResearch = search.getOffsets();
    }

    /**
     *Fait un precedent sur le resultat d'une recherche.
     */
    public void prev() {
        this.prevActionPerformed(null);
    }

    public void close() {
    }

    /**
     *Initialise l'editeur dans lequel on effectuera la recherche.
     *@param editor_ editeur, ne pas mettre un editor null...
     */
    public void setEditor(DefaultEditor editor_) {
        this.editor = editor_;
        String selected = editor.getSelected();
        if (selected != null) {
            searchText.setText(selected);
        }
    }

    public DefaultEditor getEditor() {
        return this.editor;
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc=" Generated Code ">//GEN-BEGIN:initComponents
    private void initComponents() {
        java.awt.GridBagConstraints gridBagConstraints;

        directoryResult = new javax.swing.JPanel();
        labelResult = new javax.swing.JLabel();
        scroller = new javax.swing.JScrollPane();
        tree = new javax.swing.JTree();
        browseDialog = new javax.swing.JDialog();
        fileChooser = new javax.swing.JFileChooser();
        buttonGroup1 = new javax.swing.ButtonGroup();
        labelSearch = new javax.swing.JLabel();
        searchText = new javax.swing.JTextField();
        useDirectory = new javax.swing.JCheckBox();
        caseSensitive = new javax.swing.JCheckBox();
        labelDirectory = new javax.swing.JLabel();
        browse = new javax.swing.JButton();
        directoryText = new javax.swing.JTextField();
        find = new javax.swing.JButton();
        next = new javax.swing.JButton();
        result = new javax.swing.JLabel();
        position = new javax.swing.JLabel();
        prev = new javax.swing.JButton();
        useGrammar = new javax.swing.JCheckBox();
        useFile = new javax.swing.JCheckBox();

        directoryResult.setLayout(new java.awt.GridBagLayout());

        labelResult.setBackground(new java.awt.Color(153, 255, 153));
        labelResult.setFont(new java.awt.Font("Arial", 0, 11));
        labelResult.setText("jLabel1");
        labelResult.setBorder(javax.swing.BorderFactory.createMatteBorder(0, 0, 2, 0, new java.awt.Color(255, 102, 51)));
        labelResult.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        directoryResult.add(labelResult, gridBagConstraints);

        scroller.setViewportView(tree);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        directoryResult.add(scroller, gridBagConstraints);

        fileChooser.setFileSelectionMode(javax.swing.JFileChooser.DIRECTORIES_ONLY);
        browseDialog.getContentPane().add(fileChooser, java.awt.BorderLayout.CENTER);

        setLayout(new java.awt.GridBagLayout());

        labelSearch.setBackground(new java.awt.Color(153, 255, 153));
        labelSearch.setFont(new java.awt.Font("Arial", 0, 11));
        labelSearch.setText("Find What");
        labelSearch.setBorder(javax.swing.BorderFactory.createMatteBorder(0, 0, 2, 0, new java.awt.Color(255, 102, 51)));
        labelSearch.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        add(labelSearch, gridBagConstraints);

        searchText.setFont(new java.awt.Font("Arial", 0, 11));
        searchText.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyReleased(java.awt.event.KeyEvent evt) {
                searchTextKeyReleased(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridwidth = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        add(searchText, gridBagConstraints);

        buttonGroup1.add(useDirectory);
        useDirectory.setFont(new java.awt.Font("Arial", 0, 11));
        useDirectory.setText("Directory");
        useDirectory.setBorder(javax.swing.BorderFactory.createEmptyBorder(0, 0, 0, 0));
        useDirectory.setMargin(new java.awt.Insets(0, 0, 0, 0));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        add(useDirectory, gridBagConstraints);

        caseSensitive.setFont(new java.awt.Font("Arial", 0, 11));
        caseSensitive.setText("case sensitive");
        caseSensitive.setBorder(javax.swing.BorderFactory.createEmptyBorder(0, 0, 0, 0));
        caseSensitive.setMargin(new java.awt.Insets(0, 0, 0, 0));
        caseSensitive.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                caseSensitiveActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        add(caseSensitive, gridBagConstraints);

        labelDirectory.setBackground(new java.awt.Color(153, 255, 153));
        labelDirectory.setFont(new java.awt.Font("Arial", 0, 11));
        labelDirectory.setText("Directory");
        labelDirectory.setBorder(javax.swing.BorderFactory.createMatteBorder(0, 0, 2, 0, new java.awt.Color(255, 102, 51)));
        labelDirectory.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.gridwidth = 4;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        add(labelDirectory, gridBagConstraints);

        browse.setFont(new java.awt.Font("Arial", 0, 11));
        browse.setText("Browse");
        browse.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                browseActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.gridheight = 2;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weighty = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 6, 2, 2);
        add(browse, gridBagConstraints);

        directoryText.setFont(new java.awt.Font("Arial", 0, 11));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.gridwidth = 3;
        gridBagConstraints.gridheight = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        add(directoryText, gridBagConstraints);

        find.setFont(new java.awt.Font("Arial", 0, 11));
        find.setText("Find");
        find.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                findActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 4;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(2, 6, 2, 2);
        add(find, gridBagConstraints);

        next.setFont(new java.awt.Font("Arial", 0, 11));
        next.setText("Next");
        next.setEnabled(false);
        next.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                nextActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 4;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(0, 5, 0, 0);
        add(next, gridBagConstraints);

        result.setFont(new java.awt.Font("Arial", 0, 11));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 4;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(4, 6, 0, 0);
        add(result, gridBagConstraints);

        position.setFont(new java.awt.Font("Arial", 0, 11));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 4;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weighty = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(4, 6, 0, 0);
        add(position, gridBagConstraints);

        prev.setFont(new java.awt.Font("Arial", 0, 11));
        prev.setText("Prev");
        prev.setEnabled(false);
        prev.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                prevActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 4;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(2, 6, 2, 2);
        add(prev, gridBagConstraints);

        buttonGroup1.add(useGrammar);
        useGrammar.setFont(new java.awt.Font("Arial", 0, 11));
        useGrammar.setText("In Grammar");
        useGrammar.setBorder(javax.swing.BorderFactory.createEmptyBorder(0, 0, 0, 0));
        useGrammar.setMargin(new java.awt.Insets(0, 0, 0, 0));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        add(useGrammar, gridBagConstraints);

        buttonGroup1.add(useFile);
        useFile.setSelected(true);
        useFile.setText("In file");
        useFile.setBorder(javax.swing.BorderFactory.createEmptyBorder(0, 0, 0, 0));
        useFile.setMargin(new java.awt.Insets(0, 0, 0, 0));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(2, 6, 2, 2);
        add(useFile, gridBagConstraints);

    }// </editor-fold>//GEN-END:initComponents

    /**
     *Affiche, selectionne un offset trouve.
     *@param cursor index dans le veteur d'offset
     */
    private void showOffset(int cursor) {
        if (cursor >= 0 && cursor < enumResearch.size()) {
            Search.Offset o = (Search.Offset) enumResearch.get(cursor);
            int begin = o.getStart();
            int end = o.getEnd();
            position.setText(begin + ":" + end);
            position.setToolTipText(begin + ":" + end);
            editor.grabFocus();
            editor.requestFocusInWindow();
            editor.setCaretPosition(end);
            editor.setSelectedText(begin, end);
        //editor.repaint();
        }
    }

    private void prevActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_prevActionPerformed
        if (cursor > 0) {
            cursor = cursor - 1;
            showOffset(cursor);
            if (cursor < search.count()) {
                next.setEnabled(true);
            }
            if (cursor == 0) {
                prev.setEnabled(false);
            }
        }

    }//GEN-LAST:event_prevActionPerformed

    private void caseSensitiveActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_caseSensitiveActionPerformed
        if (this.caseSensitive.isSelected()) {
            this.flag = flag - Pattern.CASE_INSENSITIVE;
        } else {
            this.flag = flag + Pattern.CASE_INSENSITIVE;
        }
    }//GEN-LAST:event_caseSensitiveActionPerformed

    private void nextActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_nextActionPerformed
        if (cursor < search.count()) {
            cursor = cursor + 1;
            showOffset(cursor);
            if (cursor > 0) {
                prev.setEnabled(true);
            }
            if (cursor == search.count() - 1) {
                next.setEnabled(false);
            }
        }
    }//GEN-LAST:event_nextActionPerformed

    private void searchTextKeyReleased(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_searchTextKeyReleased
        if (evt.getKeyCode() == KeyEvent.VK_ENTER) {
            findActionPerformed(null);
        }
    }//GEN-LAST:event_searchTextKeyReleased

    private void findActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_findActionPerformed
        text = searchText.getText();
        cursor = 0;
        if (!useDirectory.isSelected() && !useGrammar.isSelected()) {
            position.setText("-1");
            result.setText("0");
            search.setSearch(text, editor.getText(), flag, editor.getFile().getAbsolutePath());
            search.find();
            int res = search.count();
            this.result.setText(String.valueOf(res));
            if (res > 0) {
                if (cursor < res - 1) {
                    next.setEnabled(true);
                }
                prev.setEnabled(false);
                enumResearch = search.getOffsets();
                showOffset(cursor);
            } else {
                warning(text + " not found");
            }
        } else if (useDirectory.isSelected()) {
            fileMap = searchInDirectory(directory, fileMap);
            constructTreeResult();

        } else if (useGrammar.isSelected()) {
            fileMap = searchInGrammar(fileMap);
            constructTreeResult();
        }
    }//GEN-LAST:event_findActionPerformed

    private void browseActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_browseActionPerformed
        fileChooser.setCurrentDirectory(directory);
        int returnVal = fileChooser.showOpenDialog(browseDialog);

        if (returnVal == JFileChooser.APPROVE_OPTION) {
            directory = fileChooser.getSelectedFile();
            ProjectContext pc = getProject().getProjectContext();
            pc.setProperty(ProjectContext.default_directory, directory.getAbsolutePath());
            this.directoryText.setText(directory.getAbsolutePath());
            this.directoryText.setToolTipText(directory.getAbsolutePath());
            this.directoryText.setCaretPosition(0);
            this.useDirectory.setSelected(true);
        }
    }//GEN-LAST:event_browseActionPerformed

    /***
     *Construit un arbre selon le resultat d'une recherche sur plusieurs fichiers.
     *on utilise la table fileMap pour construire l'arbre.
     */
    private void constructTreeResult() {

        this.labelResult.setText("Searching...");
        // recherche :
        if (fileMap != null) {
            DefaultMutableTreeNode top;
            if (useDirectory.isSelected()) {
                top = new DefaultMutableTreeNode(directory.getName());
            } else {
                top = new DefaultMutableTreeNode("Grammar");
            }
            Enumeration enumKeys = fileMap.keys();
            int cpt = 0;
            while (enumKeys.hasMoreElements()) {
                File key = (File) enumKeys.nextElement();
                Search s = (Search) fileMap.get(key);
                DefaultMutableTreeNode node = new DefaultMutableTreeNode(key);
                Enumeration enumOffset = s.getAllOffsets();
                while (enumOffset.hasMoreElements()) {
                    Search.Offset offset = (Search.Offset) enumOffset.nextElement();
                    node.add(new DefaultMutableTreeNode(offset));
                }
                if (s.count() > 0) {
                    top.add(node);
                    cpt++;
                }
            }
            labelResult.setText("Result finished : " + cpt + "file(s) found ");
            tree = new JTree(top);
            tree.setCellRenderer(new TreeSearchRenderer());
            initMouseListener();
            scroller.setViewportView(tree);
            JDialog resultDialog = new JDialog();
            resultDialog.setContentPane(directoryResult);
            resultDialog.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);
            resultDialog.setAlwaysOnTop(true);
            resultDialog.setLocationRelativeTo(this);
            resultDialog.setSize(200, 500);
            resultDialog.setVisible(true);
            directoryResult.revalidate();
            directoryResult.repaint();
        } else {
            labelResult.setText("Search problem ");
        }
    }

    private Search searchInFile(File f) {
        Search search_ = new Search();
        String text1 = null;
        try {
            text1 = FileTools.read(f, null);
        } catch (Exception e) {
            severe("ERROR", e);
        }
        if (text1 != null) {
            search_.setSearch(searchText.getText(), text1, flag, f.getName());
            search_.find();
        }
        return search_;
    }

    /**
     *Effecture la recherche sur les fichiers de la grammaire.
     *@param fileMap table a remplir
     */
    private Hashtable searchInGrammar(Hashtable fileMap) {
        try {
            Project p = getProject();
            Grammar g = p.getGrammar();
            Vector vFiles = g.getAllGrammarFiles();
            Iterator ite = vFiles.iterator();
            while (ite.hasNext()) {
                File f = (File) ite.next();
                fileMap.put(f, this.searchInFile(f));
            }
        } catch (Exception e) {
            DEBUG_LOG.debug("error", e);
            return null;
        }
        return fileMap;
    }

    /**
     *Recherche dans les fichiers contenus dans le repertoire passe en parametre
     *@param file repertoire racine de la recherche
     *@param fileMap table a construire.
     */
    private Hashtable searchInDirectory(File file, Hashtable fileMap) {
        try {
            if (file.isFile()) {
                //recherche dans le fichier
                fileMap.put(file, this.searchInFile(file));
            } else { //on a un repertoire on doit le parcourir

                File[] filesListed = file.listFiles();
                for (int i = 0; i < filesListed.length; i++) {
                    fileMap = searchInDirectory(filesListed[i], fileMap);
                }
            }
            return fileMap;
        } catch (Exception e) {
            return null;
        }
    }
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton browse;
    private javax.swing.JDialog browseDialog;
    private javax.swing.ButtonGroup buttonGroup1;
    private javax.swing.JCheckBox caseSensitive;
    private javax.swing.JPanel directoryResult;
    private javax.swing.JTextField directoryText;
    private javax.swing.JFileChooser fileChooser;
    private javax.swing.JButton find;
    private javax.swing.JLabel labelDirectory;
    private javax.swing.JLabel labelResult;
    private javax.swing.JLabel labelSearch;
    private javax.swing.JButton next;
    private javax.swing.JLabel position;
    private javax.swing.JButton prev;
    private javax.swing.JLabel result;
    private javax.swing.JScrollPane scroller;
    private javax.swing.JTextField searchText;
    private javax.swing.JTree tree;
    private javax.swing.JCheckBox useDirectory;
    private javax.swing.JCheckBox useFile;
    private javax.swing.JCheckBox useGrammar;
    // End of variables declaration//GEN-END:variables

    private void initMouseListener() {
        ml = new MouseAdapter() {

            public void mouseClicked(MouseEvent e) {
                int selRow = tree.getRowForLocation(e.getX(), e.getY());
                TreePath selPath = tree.getPathForLocation(e.getX(), e.getY());
                if (selRow != -1) {
                    if (e.getClickCount() == 2) {
                        try {
                            DefaultMutableTreeNode node = (DefaultMutableTreeNode) selPath.getLastPathComponent();
                            if (node == null) {
                                return;
                            }
                            Object value = node.getUserObject();
                            if (node.isLeaf()) {
                                // on a un offset
                                TreeNode parent = node.getParent();
                                Search.Offset offset = (Search.Offset) node.getUserObject();
                                if (parent instanceof DefaultMutableTreeNode) {
                                    Object oFile = ((DefaultMutableTreeNode) parent).getUserObject();
                                    File f = (File) oFile;
                                    editFileAction.setFile(f);
                                    editFileAction.putValue(EditFileAction.ICON, XIPUIUtilities.getIcon(UIConstants.CODE));
                                    editFileAction.putValue(EditFileAction.CARET, offset.getStart());
                                    editFileAction.setType(EditFileAction.GRAMMAR_EDITOR);
                                    editFileAction.executeWithoutUI();
                                    Editor editor = editFileAction.getEditor();
                                    if (editor != null) {
                                        editor.setSelectedText(offset.getStart(), offset.getEnd());
                                    }
                                }
                            } else {
                            }
                        } catch (ClassCastException ex) {
                            DEBUG_LOG.debug("error", ex);
                        } catch (Exception ex) {
                            warning("Problem : " + ex);
                        }
                    }
                }
            }
        };
        if (tree != null) {
            tree.addMouseListener(ml);
        }
    }

    public class TreeSearchRenderer implements TreeCellRenderer {

        public Component getTreeCellRendererComponent(JTree tree, Object value, boolean selected, boolean expanded, boolean leaf, int row, boolean hasFocus) {
            if (value instanceof DefaultMutableTreeNode) {
                DefaultMutableTreeNode node = (DefaultMutableTreeNode) value;
                Object value_ = node.getUserObject();
                if (value_ instanceof File) {
                    File f = (File) value_;
                    Search s = (Search) fileMap.get(f);
                    JLabel label = new JLabel(f.getName() + " (" + s.count() + ")");
                    label.setIcon(XIPUIUtilities.getIcon(UIConstants.FILE));
                    label.setToolTipText(f.getAbsolutePath());
                    if (selected) {
                        label.setBackground(Color.DARK_GRAY);
                        label.setForeground(Color.WHITE);
                        label.setOpaque(true);
                    } else {
                        label.setOpaque(false);
                    }
                    return label;
                } else if (value_ instanceof Search.Offset) {
                    Search.Offset off = (Search.Offset) value_;
                    RuleMap rm = getProject().getRuleMap();
                    int ruleNb = -1;
                    if (rm != null) {
                        ruleNb = rm.getRuleNumber(off.getFileName(), (long) off.getStart());
                    }
                    JLabel label = new JLabel();
                    if (ruleNb != -1) {
                        label.setText("rule : " + ruleNb + " at " + off.toString());
                    } else {
                        label.setText(off.toString());
                    }
                    if (selected) {
                        label.setBackground(Color.DARK_GRAY);
                        label.setForeground(Color.WHITE);
                        label.setOpaque(true);
                    } else {
                        label.setOpaque(false);
                    }
                    return label;
                } else if (value_ instanceof Object) {
                    return new JLabel(value_.toString());
                }
            }
            return new JLabel();
        }
    }
}
