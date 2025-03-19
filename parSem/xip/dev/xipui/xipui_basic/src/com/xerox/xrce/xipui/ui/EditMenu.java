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

import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.editor.*;
import com.xerox.xrce.xipui.ui.editor.DefaultEditor;
import javax.swing.*;
import javax.swing.text.*;
import java.awt.*;
import java.awt.event.*;

import java.util.*;

/**
 *
 * @author vgrassau
 */
public class EditMenu extends JMenu {

    /** noms des items pour le menu EDIT */
    static public final String UNDO_ = XIPUIUtilities.getApplicationResource("item.undo");
    static public final String REDO_ = XIPUIUtilities.getApplicationResource("item.redo");
    static public final String COPY_ = XIPUIUtilities.getApplicationResource("item.copy");
    static public final String CUT_ = XIPUIUtilities.getApplicationResource("item.cut");
    static public final String PASTE_ = XIPUIUtilities.getApplicationResource("item.paste");
    static public final String SELECT_ALL_ = XIPUIUtilities.getApplicationResource("item.select.all");
    static public final String EDIT_ = XIPUIUtilities.getApplicationResource("menu.edit");
    static public final String FIND_ = XIPUIUtilities.getApplicationResource("item.find");
    private JMenuItem copy;
    private JMenuItem cut;
    private JMenuItem paste;
    private JMenuItem all;
    private JMenuItem undo;
    private JMenuItem redo;
    private JMenuItem find;
    /** pour l'edition */
    private ModuleContext properties;
    private UndoAction undoAction;
    private RedoAction redoAction;
    private HashMap actions;
    private UISearch findUI;
    private JComponent object;

    /** Creates a new instance of EditMenu
     * @param properties
     * @param object
     * @param undoAction
     * @param redoAction 
     */
    public EditMenu(ModuleContext properties, JComponent object, UndoAction undoAction, RedoAction redoAction) {
        super(EDIT_);
        actions = new HashMap();
        this.properties = properties;
        findUI = new UISearch(properties);
        initMenu(object, undoAction, redoAction);
        setFont(UIConstants.DEFAULT_FONT);
    }

    public void initMenu(JComponent object, UndoAction undoAction, RedoAction redoAction) {
        this.object = object;
        this.undoAction = undoAction;
        this.redoAction = redoAction;
        initMenu();
    }

    private void initMenu() {
        this.removeAll();
        cut = new JMenuItem();
        paste = new JMenuItem();
        copy = new JMenuItem();
        all = new JMenuItem();
        undo = new JMenuItem();
        redo = new JMenuItem();
        find = new JMenuItem();
        createActionTable();
        add(undo);
        add(redo);
        addSeparator();
        add(cut);
        add(copy);
        add(paste);
        addSeparator();
        add(all);
        addSeparator();
        add(find);
        undo.setAction(undoAction);
        undo.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Z, Event.CTRL_MASK));
        undo.setText(UNDO_);
        undo.setIcon(XIPUIUtilities.getIcon(UIConstants.UNDO));
        undo.setFont(UIConstants.DEFAULT_FONT);
        redo.setAction(redoAction);
        redo.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Y, Event.CTRL_MASK));
        redo.setText(REDO_);
        redo.setIcon(XIPUIUtilities.getIcon(UIConstants.REDO));
        redo.setFont(UIConstants.DEFAULT_FONT);
        cut.setAction(getActionByName(DefaultEditorKit.cutAction));
        cut.setText(CUT_);
        cut.setIcon(XIPUIUtilities.getIcon(UIConstants.CUT));
        cut.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_X, Event.CTRL_MASK));
        cut.setFont(UIConstants.DEFAULT_FONT);
        copy.setAction(getActionByName(DefaultEditorKit.copyAction));
        copy.setText(COPY_);
        copy.setIcon(XIPUIUtilities.getIcon(UIConstants.COPY));
        copy.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_C, Event.CTRL_MASK));
        copy.setFont(UIConstants.DEFAULT_FONT);
        paste.setAction(getActionByName(DefaultEditorKit.pasteAction));
        paste.setText(PASTE_);
        paste.setIcon(XIPUIUtilities.getIcon(UIConstants.PASTE));
        paste.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_V, Event.CTRL_MASK));
        paste.setFont(UIConstants.DEFAULT_FONT);
        all.setAction(getActionByName(DefaultEditorKit.selectAllAction));
        all.setText(SELECT_ALL_);
        all.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_A, Event.CTRL_MASK));
        all.setFont(UIConstants.DEFAULT_FONT);
        find.setAction(setFindAction());
        find.setText(FIND_);
        find.setIcon(XIPUIUtilities.getIcon(UIConstants.SEARCH));
        find.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_F, Event.CTRL_MASK));
        find.setFont(UIConstants.DEFAULT_FONT);
        if (object == null) {
            cut.setEnabled(false);
            copy.setEnabled(false);
            paste.setEnabled(false);
            undo.setEnabled(false);
            redo.setEnabled(false);
            all.setEnabled(false);
        }
        if (object instanceof DefaultEditor) {
            DefaultEditor edit = (DefaultEditor) object;
            findUI.setEditor(edit);
            findUI.search();
        }
    }

    public Action getUndoAction() {
        return this.undoAction;
    }

    public Action getRedoAction() {
        return this.redoAction;
    }

    public Action getCutAction() {
        return this.cut.getAction();
    }

    public Action getCopyAction() {
        return this.copy.getAction();
    }

    public Action getPasteAction() {
        return this.paste.getAction();
    }

    public Action getFindAction() {
        return this.find.getAction();
    }

    public void findNext() {
        if (object instanceof DefaultEditor) {
            findUI.next();
        }
    }

    public void findPrev() {
        if (object instanceof DefaultEditor) {
            findUI.prev();
        }
    }

    private Action setFindAction() {

        AbstractAction action = new AbstractAction(XIPUIUtilities.getApplicationResource("action.find"),
                XIPUIUtilities.getIcon(UIConstants.FIND_SELECTION)) {

            public void actionPerformed(java.awt.event.ActionEvent evt) {
                findItemActionPerformed(evt);
            }
        };
        action.putValue(AbstractAction.LONG_DESCRIPTION, "Find");
        return action;

    }

    /***
     *Action lors de la recherche de texte dans le document 
     *@param evt evenement lors d'un clique sur l'item
     */
    public void findItemActionPerformed(ActionEvent evt) {
        if (object instanceof DefaultEditor && findUI != null) {
            JDialog dialog = new JDialog();
            dialog.setContentPane(findUI);
            dialog.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);
            dialog.setSize(500, 180);
            dialog.setLocationRelativeTo(object);
            dialog.setVisible(true);
            dialog.setAlwaysOnTop(true);
        }
    }

    /**
     *
     */
    private void createActionTable() {
        if (object instanceof DefaultEditor) {
            DefaultEditor editor = (DefaultEditor) object;
            Action[] actionsArray = editor.getTextComponent().getActions();
            for (int i = 0; i < actionsArray.length; i++) {
                Action a = actionsArray[i];
                actions.put(a.getValue(Action.NAME), a);
            }
        }
    }

    /**
     *Renvoie l'action dont le nom est donne, contenue dans la table des actions
    @param name nom de l'action a recuperer
     *@return action si l'action existe, null sinon
     */
    private Action getActionByName(String name) {
        return (Action) (actions.get(name));
    }
}
