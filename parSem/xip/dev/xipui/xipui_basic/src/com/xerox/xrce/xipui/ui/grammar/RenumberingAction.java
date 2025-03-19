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
package com.xerox.xrce.xipui.ui.grammar;

import com.xerox.jxip.XipException;

import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.Xip;
import com.xerox.xrce.xipui.modules.ActionModule;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.editor.Editor;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;
import com.xerox.xrce.xipui.ui.editor.EditFileAction;
import com.xerox.xrce.xipui.ui.RunManager;
import javax.swing.*;
import java.awt.event.*;
import java.awt.Cursor;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.io.File;
import java.util.concurrent.ExecutionException;

/**
 *
 * @author vgrassau
 */
public class RenumberingAction extends ActionModule {

    public static String EDITOR = "EDITOR";
    public static String RULE_NUMBER = "RULE_NUMBER";

    /** Creates a new instance of RenumberingAction
     * @param context 
     */
    public RenumberingAction(ModuleContext context) {
        super(context, XIPUIUtilities.getApplicationResource("action.grammar.renumbering"),
                XIPUIUtilities.getIcon(UIConstants.EDIT));
    }

    public void setToolTipText(int rule) {
        putValue(RenumberingAction.SHORT_DESCRIPTION, XIPUIUtilities.getApplicationResource("action.grammar.renumbering") + " : " + rule);
    }

    public void actionPerformed(ActionEvent evt) {
        //d'abord demander si on veut reellement faire une renumerotation
        Editor editor = (Editor) getValue(EDITOR);
        Number number = (Number) getValue(RULE_NUMBER);
        int index = -1;
        if ((editor != null) && (number != null)) {
            StringBuffer message = new StringBuffer("<html><body><p>Renumber  file ");
            message.append(editor.getFile() + "from rule " + number.intValue() + " ? </p>");
            message.append("<p>Your file will be saved before the renumbering</p></body></html>");
            int res = JOptionPane.showConfirmDialog(null, message.toString(), "", JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE);
            if (res == JOptionPane.YES_OPTION) {
                try {
                    editor.save();
                    RenumberTask task = new RenumberTask(number, editor);
                    task.execute();
                } catch (Exception e) {
                    severe("Error : " + e, e);
                }
            } else if (res == JOptionPane.CLOSED_OPTION) {
            } else if (res == JOptionPane.NO_OPTION) {
            }
        }
    }

    private class RenumberTask extends SwingWorker<Boolean, XipException> implements PropertyChangeListener {

        private Number number;
        private Editor editor;

        public RenumberTask(Number number, Editor editor) {
            this.editor = editor;
            this.number = number;
            this.addPropertyChangeListener(this);
        }

        @Override
        public Boolean doInBackground() {
            Boolean success = false;
            Project project = getProject();
            try {
                if (project.isGrammarLoaded()) {
                    project.reload();
                    success = true;
                } else {
                    project.load();
                    success = true;
                }
            } catch (XipException e) {
                publish(e);
                success = false;
            } catch (Exception e) {
                success = false;
            }
            if (success) {
                Project p = getProject();
                Xip xip = p.getXip();
                xip.renumberLayer(p.getGhandler(), number.intValue());
            }
            return success;
        }

        @Override
        public void done() {
            Boolean b;
            try {
                b = get();
            } catch (ExecutionException ex) {
                log.error("ERROR", ex);
                b = false;
            } catch (InterruptedException ex) {
                log.error("ERROR", ex);
                b = false;
            }
            RunManager runner = (RunManager) getProperty(module_run);
            runner.setEnabled(b);
            if (!b) {
                error("The grammar was not reloaded, correct error(s) first");
            } else {
                int oldPosition = editor.getCaretPosition();
                editor.editFile();
                editor.setCaretPosition(oldPosition);
                warning("To undo renumbering : click two times on undo and save the file");
            }
        }

        public void process(java.util.List<XipException> errors) {
            XipException error = errors.get(0);
            EditFileAction editFileAction = new EditFileAction(getModuleContext());
            File file = new File(error.getFileName().replace('\\', '/'));
            editFileAction.setFile(file);
            editFileAction.putValue(EditFileAction.ICON, XIPUIUtilities.getIcon(UIConstants.CODE_ERROR));
            editFileAction.putValue(EditFileAction.CARET, error.getPosition());
            editFileAction.setType(EditFileAction.GRAMMAR_EDITOR);
            editFileAction.executeWithoutUI();
        }

        /**
         * Invoked when task's state property changes.
         */
        public void propertyChange(PropertyChangeEvent evt) {
            if (evt.getPropertyName().equals("state")) {
                SwingWorker.StateValue stateValue = getState();
                switch (stateValue) {
                    case STARTED: {
                        if (editor instanceof JComponent) {
                            ((JComponent) editor).setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
                        }
                        break;
                    }
                    case DONE: {
                        ((JComponent) editor).setCursor(null);
                        break;
                    }
                }
            }
        }
    }
}

