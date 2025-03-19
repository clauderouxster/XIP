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

import javax.swing.text.*;
import java.awt.*;

/**
 *
 * @author vgrassau
 */
public class LineColor extends StyledEditorKit implements ViewFactory {

    public static int HIGHLIGHT_OFFSET = 1;
    private boolean enabled = true;

    public ViewFactory getViewFactory() {
        return this;
    }

    public void setEnabled(boolean b) {
        this.enabled = b;
    }

    public View create(Element elem) {
        String kind = elem.getName();
        if (AbstractDocument.ParagraphElementName.equals(kind) && enabled) {
            return new LineColorParagraphView(elem);
        }
        return super.getViewFactory().create(elem);
    }

    static class LineColorParagraphView extends ParagraphView {
        //public static final Color HIGHLIGHT_COLOR = new Color(255,255,128);

        public static final Color HIGHLIGHT_COLOR = new Color(232, 244, 248);
        private Rectangle tempRect = new Rectangle();

        LineColorParagraphView(Element elem) {
            super(elem);
        }

        public void paint(Graphics g, Shape allocation) {
            Rectangle alloc = (allocation instanceof Rectangle) ? (Rectangle) allocation : allocation.getBounds();
            int n = getViewCount();
            int x = alloc.x + getLeftInset();
            int y = alloc.y + getTopInset();
            Rectangle clip = g.getClipBounds();
            for (int i = 0; i < n; i++) {
                tempRect.x = x + getOffset(X_AXIS, i);
                tempRect.y = y + getOffset(Y_AXIS, i);
                tempRect.width = getSpan(X_AXIS, i);
                tempRect.height = getSpan(Y_AXIS, i);
                if (tempRect.intersects(clip)) {
                    View v = getView(i);
                    // this is where the bg highlight is painted
                    if (v.getStartOffset() < HIGHLIGHT_OFFSET && HIGHLIGHT_OFFSET <= v.getEndOffset()) {
                        g.setColor(HIGHLIGHT_COLOR);
                        g.fillRect(tempRect.x, tempRect.y, alloc.width, tempRect.height);
                    }
                    paintChild(g, tempRect, i);
                }
            }
        }
    }
}
