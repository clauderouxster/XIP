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
public class UndoAction extends AbstractAction {

    private UndoManager undoM;
    private RedoAction redoAction;

    public UndoAction(UndoManager undoM, RedoAction redoAction) {
        super("Undo");
        this.undoM = undoM;
        this.redoAction = redoAction;
        setEnabled(false);
    }

    public void setRedoAction(RedoAction redoAction_) {
        this.redoAction = redoAction_;
    }

    public void actionPerformed(ActionEvent e) {
        try {
            //  undoM.   
            undoM.undo();
        } catch (CannotUndoException ex) {
            System.out.println("Unable to undo: " + ex);
            ex.printStackTrace();
        }
        updateUndoState();
        redoAction.updateRedoState();
    }

    protected void updateUndoState() {
        if (undoM.canUndo()) {
            setEnabled(true);
        //  putValue(Action.NAME, undoM.getUndoPresentationName());
        } else {
            setEnabled(false);
        // putValue(Action.NAME, "Undo");
        }
    }
}
