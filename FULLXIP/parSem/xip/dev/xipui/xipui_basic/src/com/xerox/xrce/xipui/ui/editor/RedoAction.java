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
package com.xerox.xrce.xipui.ui.editor;

import java.awt.event.*;
import javax.swing.*;
import javax.swing.undo.*;

/**
 *
 * @author vgrassau
 */
public class RedoAction extends AbstractAction {

    private UndoManager undoM;
    private UndoAction undoAction;

    public RedoAction(UndoManager undoM, UndoAction undoAction) {
        super("Redo");
        this.undoM = undoM;
        this.undoAction = undoAction;
        setEnabled(false);
    }

    public void actionPerformed(ActionEvent e) {
        try {
            undoM.redo();
        } catch (CannotRedoException ex) {
            System.out.println("Unable to redo: " + ex);
            ex.printStackTrace();
        }
        updateRedoState();
        undoAction.updateUndoState();
    }

    protected void updateRedoState() {
        if (undoM.canRedo()) {
            setEnabled(true);
        //  putValue(Action.NAME,undoM.getRedoPresentationName());
        } else {
            setEnabled(false);
        // putValue(Action.NAME, "Redo");
        }
    }
}

