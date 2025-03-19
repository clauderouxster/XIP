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
package com.xerox.xrce.xipui.ui.graph;

import com.xerox.xrce.xipui.modules.ModuleContext;
import java.awt.event.MouseEvent;
import javax.swing.*;
import edu.berkeley.guir.prefuse.*;
import edu.berkeley.guir.prefuse.event.*;
import edu.berkeley.guir.prefuse.activity.*;

/**
 *<p>Cette classe permet d'effectuer les operations sur les noeuds du ChunkGraph</p>
 * @author Vianney Grassaud
 */
public class NodeControl2 extends ControlAdapter {

    private ModuleContext context;
    private ActionList update2;
    private int id;

    public NodeControl2(ModuleContext properties, int id) {
        super();
        this.id = id;
        context = properties;
    }

    public void setActionList(ActionList update2) {
        this.update2 = update2;
        if (!update2.isRunning()) {
            this.update2.runNow();
        }
    }

    public void itemClicked(VisualItem item, java.awt.event.MouseEvent evt) {
        SessionGraph session = context.getSessionGraph(false, id);
        if (item instanceof NodeItem) {
            // on recupere l'index de l'item 
            String index_ = item.getAttribute("index");
            Long index = new Long(index_);
            //on recupere le type de l'item
            String type = item.getAttribute("type");
            boolean checked = item.getAttribute("check").equals("true");
            boolean leafControl = session.isLeafControl() && type.equals("LEAF");
            if (type.equals("NODE") || leafControl) {
                if (evt.getButton() == MouseEvent.BUTTON3) {
                    //si bouton3 alors on affiche un popup            
                    ZoomGraph zoom = new ZoomGraph(context, index, id);
                    //System.out.println("je fais click droit sur : " + index);
                    zoom.displayNodeZoom();
                    zoom.showInDialog(evt.getX(), evt.getY());
                }
                if (session.isSelectable()) {
                    if (evt.getButton() == MouseEvent.BUTTON1 && evt.getClickCount() == 2) {
                        if (checked) {
                            item.setAttribute("check", "false");
                            session.unSelectXipNode(index);
                        } else {
                            item.setAttribute("check", "true");
                            session.selectXipNode(index);
                        }
                    }
                }
            }
        }
        if (update2 != null && !update2.isRunning()) {
            this.update2.runNow();
        }
        ActionList update = (ActionList) session.getProperty(session.ACTIONS_CHUNK);
        if (update != null && !update.isRunning()) {
            update.runNow();
        }
    }

    public void itemReleased(VisualItem item, java.awt.event.MouseEvent e) {
//        if ( !update.isRunning())   
//           update.runNow();
    }

    public void zoomNode(JPopupMenu pop, long nodeIndex) {
        ZoomGraph zoom = new ZoomGraph(context, nodeIndex, id);
        zoom.displayNodeZoom();
        pop.add(zoom);
    }
}
