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

import com.xerox.xrce.xipui.modules.IModule;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.editor.DefaultEditor;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import org.apache.log4j.*;

/**
 *
 * @author vgrassau
 */
public class FileMenu extends JMenu {

    /** noms des items poru le menu File */
    static public final String FILE_ = XIPUIUtilities.getApplicationResource("menu.file");
    static public final String SAVE_ = XIPUIUtilities.getApplicationResource("item.save");
    static public final String CLOSE_ = XIPUIUtilities.getApplicationResource("item.close");
    static public final String SAVE_AS_ = XIPUIUtilities.getApplicationResource("item.save.as");
    /**
     *Menu save
     */
    protected JMenuItem save;
    /**
     *Menu close
     */
    protected JMenuItem close;
    /**
     *Menu save as
     */
    protected JMenuItem saveAs;
    private JComponent object;
    protected Logger log = Logger.getLogger("tracer");
    private Action saveAction;
    private Action saveAsAction;
    private Action closeAction;
    private ModuleContext properties;

    /** Creates a new instance of FileMenu
     * @param properties
     * @param object 
     */
    public FileMenu(ModuleContext properties, JComponent object) {
        super(FILE_);
        this.properties = properties;
        initComponents();
        initAction();
        initMenu(object);
        setFont(UIConstants.DEFAULT_FONT);
    }

    private void initComponents() {
        save = XIPUIUtilities.createJMenuItem(SAVE_, SAVE_, XIPUIUtilities.getIcon(UIConstants.SAVE));
        saveAs = XIPUIUtilities.createJMenuItem(SAVE_AS_, SAVE_AS_, XIPUIUtilities.getIcon(UIConstants.SAVE_ALL));
        close = XIPUIUtilities.createJMenuItem(CLOSE_, CLOSE_, XIPUIUtilities.getIcon(UIConstants.CLOSE));
        add(new NewFileAction(properties, FileStepManager.ALL));
        addSeparator();
        add(new OpenFileAction(properties));
        add(save);
        add(saveAs);
        addSeparator();
        add(close);
        addSeparator();
        add(new ProjectPropertiesAction(properties));
    }

    public void initMenu(JComponent object) {
        this.object = object;
        if (object == null) {
            setClosable(false);
            setSavable(false);
            saveAsAction.setEnabled(false);
        } else {
            setClosable(true);
            if (object instanceof DefaultEditor) {
                setSavable(((DefaultEditor) object).wasModified());
                saveAsAction.setEnabled(true);
            } else {
                setSavable(false);
                saveAsAction.setEnabled(false);
            }
        }
        save.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, Event.CTRL_MASK));
        close.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Q, Event.CTRL_MASK));
    //  openFile.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, Event.CTRL_MASK));
    }

    public void setClosable(boolean b) {
        this.closeAction.setEnabled(b);
    }

    private void initAction() {
        saveAction = new AbstractAction(SAVE_, XIPUIUtilities.getIcon(UIConstants.SAVE)) {

            public void actionPerformed(java.awt.event.ActionEvent evt) {
                saveItemActionPerformed(evt);
            }
        };

        closeAction = new AbstractAction(CLOSE_, XIPUIUtilities.getIcon(UIConstants.CLOSE)) {

            public void actionPerformed(java.awt.event.ActionEvent evt) {
                closeItemActionPerformed(evt);
            }
        };

        saveAsAction = new AbstractAction(SAVE_AS_, XIPUIUtilities.getIcon(UIConstants.SAVE_ALL)) {

            public void actionPerformed(java.awt.event.ActionEvent evt) {
                saveAs();
            }
        };
        save.setAction(saveAction);
        close.setAction(closeAction);
        saveAs.setAction(saveAsAction);
    }

    public void setSavable(boolean b) {
        // changement d'icone 
        changeIcon(b);
        saveAction.setEnabled(b);
    }

    private void changeIcon(boolean b) {
        if (object instanceof DefaultEditor) {
            DefaultEditor editor = (DefaultEditor) object;
            JTabbedPane center = (JTabbedPane) properties.get(IModule.CENTER_TABBED);
            int index = center.indexOfComponent(editor);
            Icon icon;
            if (b) {
                icon = XIPUIUtilities.getIcon(UIConstants.AMBIGOUS);
            } else {
                icon = editor.getIcon();
            }
            if (index != -1) {
                center.setIconAt(index, icon);
            }
        }
    }

    public void save() {
        if (object instanceof DefaultEditor) {
            DefaultEditor editor = (DefaultEditor) object;
            try {
                log.info("Save file : " + editor.getFile().getAbsolutePath() + " encoding : " + editor.getEncoding());
                editor.save();
            } catch (Exception e) {
                Tracer t = editor.getTracer();
                t.severe("ERROR", e);
            }
        }
        setSavable(false);
    }

    /**
     *Sauvegarde le fichier edite.
     *@param evt evenement lors du clique sur l'item
     */
    private void saveItemActionPerformed(ActionEvent evt) {
        save();
    }

    public void close() {
        boolean toClose = true;
        if (object instanceof IModule) {
            IModule module = (IModule) object;
            module.close();
            toClose = module.isClosedModule();
            if (toClose) {
                JComponent root = module.getRootComponent();
                root.remove(object);
            }
        }
        setClosable(!toClose);
    }

    /**
     *Retire l'objet de son conteneur.
     *@param evt evenement lors du clique sur l'item
     */
    private void closeItemActionPerformed(ActionEvent evt) {
        close();
    }

    private void saveAs() {
        if (object instanceof DefaultEditor) {
            DefaultEditor editor = (DefaultEditor) object;
            editor.saveAs();
        }
    }

    public Action getSaveAction() {
        return saveAction;
    }

    public Action getCloseAction() {
        return closeAction;
    }
}
