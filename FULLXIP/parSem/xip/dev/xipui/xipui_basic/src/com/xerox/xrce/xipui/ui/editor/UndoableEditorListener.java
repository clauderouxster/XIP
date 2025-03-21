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

import javax.swing.event.*;
import javax.swing.undo.*;

/**
 *
 * @author vgrassau
 */
public class UndoableEditorListener implements UndoableEditListener {

    private UndoManager undoM;
    private UndoAction undoAction;
    private RedoAction redoAction;

    public UndoableEditorListener(UndoManager undoM, UndoAction undoAction, RedoAction redoAction) {
        this.undoM = undoM;
        this.undoAction = undoAction;
        this.redoAction = redoAction;
    }

    public void undoableEditHappened(UndoableEditEvent e) {
        //Remember the edit and update the menus.
        undoM.addEdit(e.getEdit());
        undoAction.updateUndoState();
        redoAction.updateRedoState();
    }
}

    

