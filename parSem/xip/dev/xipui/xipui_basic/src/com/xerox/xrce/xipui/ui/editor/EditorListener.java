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

import com.xerox.xrce.xipui.ui.FileMenu;
import javax.swing.event.*;

/**
 *
 * @author vgrassau
 */
public class EditorListener implements DocumentListener {

    private FileMenu menu;
    private boolean isModified = false;

    public EditorListener(FileMenu menu) {
        this.menu = menu;

    }

    public void insertUpdate(DocumentEvent e) {
        displayEditInfo(e);
    }

    public void removeUpdate(DocumentEvent e) {
        displayEditInfo(e);
    }

    public void changedUpdate(DocumentEvent e) {
        displayEditInfo(e);
    }

    private void displayEditInfo(DocumentEvent e) {
        if (e.getType() == DocumentEvent.EventType.INSERT || e.getType() == DocumentEvent.EventType.REMOVE) {
            this.isModified = true;
            if (menu != null) {
                menu.setSavable(this.isModified);
            }
        }
    }

    public boolean isModified() {
        return this.isModified;
    }

    public void setModified(boolean b) {
        this.isModified = b;
        if (menu != null) {
            menu.setSavable(b);
        }
    }
}
