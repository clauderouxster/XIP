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

import com.xerox.xrce.xipui.ui.editor.EditFileAction;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.modules.ScrollPaneModule;
import javax.swing.*;
import javax.swing.tree.*;
import java.awt.event.*;

/**
 * 
 * @author vgrassau
 */
public abstract class Viewer extends ScrollPaneModule {
    //arbre pour visualiser la structure

    protected JTree tree;
    // ecoute des evenements souris
    protected MouseListener ml;
    protected JPopupMenu popup;
    protected EditFileAction editFileAction;
    protected Icon icon;
    protected TreeCellRenderer treeRenderer;

    /**
     *Create a Viewer with the given ModuleContext
     *@param properties the ModuleContext for the viewer
     */
    public Viewer(ModuleContext properties) {
        super(properties);
        this.popup = new JPopupMenu();
        initTreeRenderer();
        addComponentListener(new java.awt.event.ComponentAdapter() {

            public void componentResized(java.awt.event.ComponentEvent evt) {
                formComponentResized(evt);
            }
        });
        editFileAction = new EditFileAction(getModuleContext());
        this.setBorder(null);
        //initialize the popup
        initPopup();
    }

    /**
     *Event 
     *@param evt the event 
     */
    private void formComponentResized(ComponentEvent evt) {
        if (tree != null) {
            tree.setSize(this.getSize());
            tree.repaint();
        }
        this.repaint();
    }

    /**
     *Create a Viewer with the given ModuleContext and the given name and icon
     *@param properties the ModuleContext for the viewer
     *@param name the name for the viewer 
     *@param icon the icon to represent the viewer
     */
    public Viewer(ModuleContext properties, String name, Icon icon) {
        this(properties);
        setIcon(icon);
        setName(name);
    }

    /**
     *Init the renderer for the tree
     */
    protected abstract void initTreeRenderer();

    public TreeCellRenderer getTreeRenderer() {
        return this.treeRenderer;
    }

    /**
     *Create the tree and set the view of the tree
     * this method calls the constructTree() and setViewTree() methods
     */
    public void loadTree() {
        constructTree();
        setViewTree();
    }

    /**
     *Set the view tree for the viewer
     *add the tooltip function and the mouselistener
     */
    protected void setViewTree() {
        if (tree != null) {
            tree.setRowHeight(20);
            try {
                DefaultTreeCellRenderer render = (DefaultTreeCellRenderer) tree.getCellRenderer();
                render.setIconTextGap(5);
            } catch (ClassCastException e) {
                //on ne fait rien
            }
            initMouseListener();
            ToolTipManager.sharedInstance().registerComponent(tree);
            this.add(tree);
            this.setViewportView(tree);
            this.setBorder(null);
        }
    }

    /**
     * Initialisation des evenements souris sur l'arbre (JTree)
     * Double clique avec le bouton gauche de la souris sur un noeud de l'arbre invoque la methode editView.
     * Un clique avec le bouton droit invoque la methode showPopup
     * Si l'arbdre est null, alors les evenement souris ne sont pas geres.
     *
     * @see com.xerox.xrce.lingdexip.ui.grammar.Viewer#editView(int, TreePath)
     * @see com.xerox.xrce.lingdexip.ui.grammar.Viewer#showPopup(MouseEvent)
     */
    protected void initMouseListener() {
        if (tree != null) {
            ml = new MouseAdapter() {

                public void mouseClicked(MouseEvent e) {
                    int selRow = tree.getRowForLocation(e.getX(), e.getY());
                    TreePath selPath = tree.getPathForLocation(e.getX(), e.getY());
                    if (selRow != -1) {
                        if (e.getClickCount() == 2) {
                            editView(selRow, selPath);
                        }
                    }
                    if (MouseEvent.BUTTON3 == e.getButton()) {
                        // if we selected more than 1 row and click on right button 
                        tree.getSelectionPaths();
                        showPopup(e, selRow, selPath);
                    }
                }
            };
            tree.addMouseListener(ml);
        }
    }

    /**
     *refresh the tree; this method calls the loadTree() method
     */
    public void refresh() {
        loadTree();
    }

    /**
     *Initialize the Popup menu for the viewer
     */
    protected abstract void initPopup();

    /**
     *Method to edit the element at the selected row 
     *@param selRow 
     * @param selPath path of the row selected on the tree
     */
    public abstract void editView(int selRow, TreePath selPath);

    /**
     *Method to show a popupmenu when an user use the right mouse button on the tree
     *@param evt mouse event
     *@param selRow the row selected in the tree
     *@param selPath the paths of the element selected in the tree
     */
    public abstract void showPopup(MouseEvent evt, int selRow, TreePath selPath);

    /**
     *Method to construct the tree
     *this method is called by loadTree() 
     *
     */
    protected abstract void constructTree();

    /**
     *Return a descritpion of this viwer
     *this method calls the getName() method.
     *@return the name of the viewer if it was set
     *
     */
    public String toString() {
        return this.getName();
    }

    /***
     *Set the icon for the viewer 
     *@param icon icon to use
     */
    public void setIcon(Icon icon) {
        this.icon = icon;
    }

    /**
     *Return the Icon of this viewer
     *@return the icon 
     */
    public Icon getIcon() {
        return this.icon;
    }
}
