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
package com.xerox.xrce.xipui.ui.corpus;

import com.xerox.xrce.xipui.ui.editor.EditFileAction;
import com.xerox.xrce.xipui.ui.RunManager;
import com.xerox.xrce.xipui.ui.Viewer;
import com.xerox.xrce.xipui.CorpusElement;
import com.xerox.xrce.xipui.CorpusSet;
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.ProjectContext;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;
import javax.swing.*;

import java.io.*;
import javax.swing.tree.*;
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;

/**
 * Affiche le jeux de corpus sources du projet.
 * @author Vianney Grassaud
 */
public class CorpusViewer extends Viewer {

    private boolean[] expandedRow;
    public static final String TITLE = "Corpus Manager";
    private CorpusSet corpusSet2Edit;
    private CorpusElement ce2Edit;
    /** some actions */
    /** add a corpusElement */
    private Action addCorpusElement;
    /** remove a corpusElement */
    private Action removeCorpusElement;
    /** set Properties for a corpusElement */
    private Action setProperties;

    public CorpusViewer(ModuleContext properties) {
        super(properties);
        editFileAction.putValue(EditFileAction.WRAPPING, true);
        setName(TITLE);
        setProperty("CORPUS_SOURCE", this);
        //we add actions to the Corpus Menu
        JMenuBar menu = (JMenuBar) getProperty(MENUBAR);
        boolean stop = false;
        for (int i = 0; i < menu.getMenuCount() && !stop; i++) {
            JMenu subMenu = menu.getMenu(i);
            if (subMenu.getText().equals("Corpus")) {
                subMenu.add(addCorpusElement);
                stop = true;
            }
        }
        addFocusListener(new java.awt.event.FocusAdapter() {

            public void focusGained(java.awt.event.FocusEvent evt) {
                formFocusGained(evt);
            }
        });
        corpusSet2Edit = getProject().getCorpusSet(Project.DEFAULT_CORPUSSET);
    }

    private void formFocusGained(java.awt.event.FocusEvent evt) {
        //we must select the corpus option
        RunManager runner = (RunManager) getProperty(module_run);
        if (runner != null) {
            runner.setFromInput(false);
        }
    }

    public void close() {
    }

    /***
     *Create the tree to represent the table of CorpusSet in a project.
     */
    protected void constructTree() {
        try {
            DefaultMutableTreeNode top = new DefaultMutableTreeNode("Corpus list");
            DefaultMutableTreeNode node = null;
            Vector sorted = new Vector(getProject().getAllCorpusSetName());
            Collections.sort(sorted);
            Iterator ite = sorted.iterator();
            int cptRow = 1;
            while (ite.hasNext()) {
                String key = (String) ite.next();
                cptRow = cptRow + 1;
                CorpusSet cs = getProject().getCorpusSet(key);
                node = new DefaultMutableTreeNode(cs);
                Vector vCe = cs.getAllCorpusElement();
                if (vCe != null && vCe.size() != 0) {
                    Collections.sort(vCe);
                    for (int i = 0; i < vCe.size(); i++) {
                        cptRow = cptRow + 1;
                        DefaultMutableTreeNode nodeCe = new DefaultMutableTreeNode(vCe.get(i));
                        node.add(nodeCe);
                    }
                }
                top.add(node);
            }
            expandedRow = getExpandedRow(cptRow);
            //Creation de l'arbre
            tree = new JTree(top);
            tree.addFocusListener(new java.awt.event.FocusAdapter() {

                public void focusGained(java.awt.event.FocusEvent evt) {
                    formFocusGained(evt);
                }
            });
            tree.getSelectionModel().setSelectionMode(TreeSelectionModel.DISCONTIGUOUS_TREE_SELECTION);
            tree.setCellRenderer(treeRenderer);
            tree.addTreeSelectionListener(new TreeSelectionListener() {

                public void valueChanged(TreeSelectionEvent e) {
                    makeSelection(e);
                }
            });
            tree.setVisibleRowCount(10);
            keepLastView();
        } catch (Exception ex) {
            log.fatal("error", ex);
        }
    }

    protected void initTreeRenderer() {
        treeRenderer = new TreeCellsRenderer();
    }

    //we want to keep the last view so we need to re-expand all the row which were expanded
    private void keepLastView() {
        if (tree != null) {
            for (int i = 0; i < expandedRow.length; i++) {
                if (expandedRow[i]) {
                    tree.expandRow(i);
                }
            }
        }
    }

    private boolean[] getExpandedRow(int length) {
        if (expandedRow == null) {
            return new boolean[length];
        } else {
            boolean[] aux = new boolean[length];
            for (int i = 0; i < expandedRow.length && i < aux.length; i++) {
                aux[i] = expandedRow[i];
            }
            return aux;
        }
    }

    /***
     *Make selection on the JTree
     *@param e event
     */
    private void makeSelection(TreeSelectionEvent e) {
        TreePath selPath = e.getPath();
        DefaultMutableTreeNode node = (DefaultMutableTreeNode) selPath.getLastPathComponent();
        if (node != null) {
            if (node.isLeaf()) {
                Object o = node.getUserObject();
                if (o instanceof CorpusElement) {
                    ce2Edit = (CorpusElement) o;
                    corpusSet2Edit = getProject().getCorpusSet(ce2Edit.getParent());
                } else {
                    corpusSet2Edit = (CorpusSet) o;
                }
            } else {
                if (!node.isRoot()) {
                    corpusSet2Edit = (CorpusSet) node.getUserObject();
                }
            }
        }
    }

    public void editView(int rowSel, TreePath selPath) {
        DefaultMutableTreeNode node = (DefaultMutableTreeNode) selPath.getLastPathComponent();
        JTabbedPane target = (JTabbedPane) getProperty(CENTER_TABBED);
        if (node == null) {
            return;
        }
        if (node.isLeaf()) {
            Object object = node.getUserObject();
            if (object instanceof CorpusElement) {
                CorpusElement ce = (CorpusElement) object;
                if (ce.isInput()) {
                    JTabbedPane tab = (JTabbedPane) getProperty(OUTPUT_TABBED);
                    tab.setSelectedIndex(1);
                } else {
                    info("Edition of the corpus element " + ce.toString());
                    editFileAction.setFile(ce.getFile(), ce.getEncoding());
                    if (ce.isXML()) {
                        editFileAction.putValue(EditFileAction.ICON, XIPUIUtilities.getIcon(UIConstants.XML));
                    } else {
                        editFileAction.putValue(EditFileAction.ICON, XIPUIUtilities.getIcon(UIConstants.TEXT));
                    }
                    editFileAction.executeWithoutUI();
                }
            }
        }
    }

    public void showPopup(MouseEvent evt, int selRow, TreePath selPath) {
        if (selPath != null && selRow != -1) {
            // if ( tree.getSelectionPath() == null) {
            if (!tree.isPathSelected(selPath)) {
                tree.setSelectionPath(selPath);
            }
            //  }

            DefaultMutableTreeNode node = (DefaultMutableTreeNode) selPath.getLastPathComponent();
            initPopup();
            if (node == null) {
                return;
            }
            Object object = node.getUserObject();
            if (object instanceof CorpusElement) {
                ce2Edit = (CorpusElement) object;
                corpusSet2Edit = getProject().getCorpusSet(ce2Edit.getParent());
                removeCorpusElement.setEnabled(!ce2Edit.isInput());
                popup.addSeparator();
                popup.add(this.createCheckToParseCorpusElement(ce2Edit.isToParse()));
                this.setProperties.setEnabled(true);
            } else if (object instanceof CorpusSet) {
                corpusSet2Edit = (CorpusSet) object;
                popup.addSeparator();
                popup.add(this.createCheckToParseCorpusSet(corpusSet2Edit.isToParse()));
                removeCorpusElement.setEnabled(!corpusSet2Edit.isDefault());
                this.setProperties.setEnabled(false);
            }
            popup.setVisible(true);
            popup.show(this.tree, evt.getX(), evt.getY());
        }
    }

    private JCheckBoxMenuItem createCheckToParseCorpusElement(boolean b) {
        JCheckBoxMenuItem item = new JCheckBoxMenuItem("to parse", XIPUIUtilities.getIcon(UIConstants.RUN));
        item.setSelected(b);
        item.addActionListener(new ActionListener() {

            public void actionPerformed(ActionEvent evt) {
                toParseCe(evt);
            }
        });
        return item;
    }

    private JCheckBoxMenuItem createCheckToParseCorpusSet(boolean b) {
        JCheckBoxMenuItem item = new JCheckBoxMenuItem("to parse", XIPUIUtilities.getIcon(UIConstants.RUN));
        item.setSelected(b);
        item.addActionListener(new ActionListener() {

            public void actionPerformed(ActionEvent evt) {
                toParseCorpusSet(evt);
            }
        });
        return item;
    }

    private void initActions() {
        addCorpusElement = new AbstractAction("Add file/Directory", XIPUIUtilities.getIcon(UIConstants.ADD)) {

            public void actionPerformed(ActionEvent evt) {
                addButtonActionPerformed(evt);
            }
        };



        removeCorpusElement = new AbstractAction("Remove", XIPUIUtilities.getIcon(UIConstants.DELETE)) {

            public void actionPerformed(ActionEvent evt) {
                delButtonActionPerformed(evt);
            }
        };

        setProperties = new AbstractAction("Properties", XIPUIUtilities.getIcon(UIConstants.PROPERTIES)) {

            public void actionPerformed(ActionEvent evt) {
                showProperties();
            }
        };
    }

    private void toParseCe(ActionEvent evt) {
        Object o = evt.getSource();
        if (o instanceof JCheckBoxMenuItem) {
            JCheckBoxMenuItem item = (JCheckBoxMenuItem) o;
            if (ce2Edit.exists()) {
                ce2Edit.setToParse(item.isSelected());
            } else {
                ce2Edit.setToParse(false);
            }
            toParseSelection(item.isSelected());
            repaint();
        }
    }

    /** set enabled or disabled the field to parse for a corpusSet */
    private void toParseCorpusSet(ActionEvent evt) {
        Object o = evt.getSource();
        if (o instanceof JCheckBoxMenuItem) {
            JCheckBoxMenuItem item = (JCheckBoxMenuItem) o;
            boolean toParse = item.isSelected();
            corpusSet2Edit.setToParse(toParse);
            toParseSelection(toParse);
            this.repaint();
        }
    }

    /**
     *Try to setParse or not a selection in the JTree
     *@param b true setParse enabled false otherwise
     */
    private void toParseSelection(boolean b) {
        TreePath[] paths = tree.getSelectionPaths();
        if (paths != null) {
            for (int i = 0; i < paths.length; i++) {
                DefaultMutableTreeNode node = (DefaultMutableTreeNode) paths[i].getLastPathComponent();
                if (node != null) {
                    Object object = node.getUserObject();
                    if (object instanceof CorpusElement) {
                        CorpusElement ce = (CorpusElement) object;
                        ce.setToParse(b);
                    } else if (object instanceof CorpusSet) {
                        CorpusSet cs = (CorpusSet) object;
                        cs.setToParse(b);
                    }
                }
            }
            repaint();
            try {
                info("save project");
                getProject().save();
            } catch (Exception e) {
            }
        }
    }

    protected void initPopup() {
        popup.removeAll();
        initActions();
        Action addCorpusSet = (Action) getProperty(CorpusManager.addCorpusSet_action);
        if (addCorpusSet != null) {
            popup.add(addCorpusSet);
        }
        Action newCorpusFile = (Action) getProperty(CorpusManager.newCorpusFile_action);
        if (newCorpusFile != null) {
            popup.add(newCorpusFile);
        }
        popup.addSeparator();
        popup.add(addCorpusElement);
        popup.add(removeCorpusElement);
        popup.addSeparator();
        popup.add(setProperties);
    }

    /*** add a new(s) CorpsuElement to the project
     */
    private void addButtonActionPerformed(java.awt.event.ActionEvent evt) {
        JFileChooser openChooser = new JFileChooser();
        openChooser.setFileSelectionMode(JFileChooser.FILES_AND_DIRECTORIES);
        openChooser.setMultiSelectionEnabled(true);
        JDialog openDialog = new JDialog();
        ProjectContext pc = getProject().getProjectContext();
        String corpusDirectory = (String) pc.getProperty(ProjectContext.corpusElement_directory);
        try {
            openChooser.setCurrentDirectory(new File(corpusDirectory));
            int returnVal = openChooser.showOpenDialog(openDialog);
            if (returnVal == JFileChooser.APPROVE_OPTION) {
                File[] file = openChooser.getSelectedFiles(); // selection du fichier

                CorpusElement[] elements = new CorpusElement[file.length];
                for (int i = 0; i < file.length; i++) {
                    elements[i] = new CorpusElement(file[i]);
                    corpusSet2Edit.addCorpusElement(elements[i]);
                    //we uptdate the default corpus directory in the project context
                    pc.setProperty(ProjectContext.corpusElement_directory, file[i].getParent());
                    info("Add  the corpus element" + file[i].getName());
                }
                CorpusElementProperties ceProp = new CorpusElementProperties(elements, tree);
                ceProp.showInDialog(true);
                loadTree();
                getProject().save();
                info("project saved");
            } else {
            }
        } catch (Exception e) {
            severe("ERROR adding new corpus", e);
        }
    }

    private void delButtonActionPerformed(java.awt.event.ActionEvent evt) {
        try {
            TreePath[] paths = tree.getSelectionPaths();
            if (paths != null) {
                for (int i = 0; i < paths.length; i++) {
                    DefaultMutableTreeNode node = (DefaultMutableTreeNode) paths[i].getLastPathComponent();
                    if (node != null) {
                        Object object = node.getUserObject();
                        if (object instanceof CorpusElement) {
                            CorpusElement ce = (CorpusElement) object;
                            CorpusSet cs = getProject().getCorpusSet(ce.getParent());
                            cs.removeCorpusElement(ce);
                        } else if (object instanceof CorpusSet) {
                            CorpusSet cs = (CorpusSet) object;
                            getProject().removeCorpusSet(cs);
                        }
                    }
                }
                loadTree();
                info("save project");
                getProject().save();
            }
        } catch (Exception e) {
            severe("ERROR del", e);
        }
    }

    public class TreeCellsRenderer implements TreeCellRenderer {

        public TreeCellsRenderer() {
        }

        public Component getTreeCellRendererComponent(JTree tree, Object value, boolean selected, boolean expanded, boolean leaf, int row, boolean hasFocus) {
            Object object = null;
            expandedRow[row] = expanded;
            if (value instanceof DefaultMutableTreeNode) {
                DefaultMutableTreeNode node = (DefaultMutableTreeNode) value;
                object = node.getUserObject();
                if (node.isRoot()) {
                    JLabel label = new JLabel(object.toString());
                    label.setFont(UIConstants.DEFAULT_FONT);
                    if (expanded) {
                        label.setIcon(XIPUIUtilities.getIcon(UIConstants.CORPUS_OPEN));
                    } else {
                        label.setIcon(XIPUIUtilities.getIcon(UIConstants.CORPUS));
                    //label.setPreferredSize(getSize());
                    }
                    return label;
                }
            }
            if (object instanceof CorpusElement) {
                CorpusRenderer cr = new CorpusRenderer((CorpusElement) object, selected);
                cr.setPreferredSize(getSize());
                return cr;
            } else if (object instanceof CorpusSet) {
                CorpusSet cs = (CorpusSet) object;
                CorpusRenderer cr = new CorpusRenderer(cs, selected);
                cr.setExpanded(expanded);
                cr.setPreferredSize(tree.getSize());
                return cr;
            }
            if (value != null) {
                return new JLabel(value.toString());
            }
            return new JLabel();
        }
    }

    private void showProperties() {
        try {
            TreePath[] paths = tree.getSelectionPaths();
            Vector elements = new Vector();
            if (paths != null) {
                for (int i = 0; i < paths.length; i++) {
                    DefaultMutableTreeNode node = (DefaultMutableTreeNode) paths[i].getLastPathComponent();
                    if (node != null) {
                        Object object = node.getUserObject();
                        if (object instanceof CorpusElement) {
                            CorpusElement ce = (CorpusElement) object;
                            elements.add(ce);
                        }
                    }
                }
                if (!elements.isEmpty()) {
                    CorpusElement[] tabElements = new CorpusElement[elements.size()];
                    for (int i = 0; i < tabElements.length; i++) {
                        tabElements[i] = (CorpusElement) elements.get(i);
                    }
                    CorpusElementProperties ceProp = new CorpusElementProperties(tabElements, tree);
                    ceProp.showInDialog(false);
                }
            }
        } catch (Exception e) {
            severe("error to display the properties", e);
        }
    }
}

