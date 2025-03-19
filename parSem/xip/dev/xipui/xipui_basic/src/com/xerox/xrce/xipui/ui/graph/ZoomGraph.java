/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 *  Copyright (c) 2005 Xerox Corporation                                                 *
 *  Xerox Research Centre Europe - Grenoble                                              *
 *  All Rights Reserved                                                                  *
 *  Copyright protection claimed includes all forms and matters of copyrightable         *
 *  material and information now allowed by statutory or judicial law or                 *
 *  hereinafter granted, including without limitation, material generated from           *
 *  the software programs which are displayed on the screen such as icons,               *
 *  screen display looks, etc. All rights reserved. No part of this document             *
 *  may be reproduced, stored in a retrieval system, or transmitted, in any              *
 *  form or by any means, electronic, mechanical, photocopied, recorded, or              *
 *  otherwise, without prior written consent from XEROX corporation.                     *
 *
 */
package com.xerox.xrce.xipui.ui.graph;

import com.xerox.jxip.*;

import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.modules.PanelModule;
import javax.swing.*;
import java.util.*;
import edu.berkeley.guir.prefuse.*;

/**
 *
 * @author  vgrassau
 */
public class ZoomGraph extends PanelModule {

    private ZoomDialog dialog; //this object is a intern class of ZoomGraph

    private long zoom; // the index of the dependency or a node

    private int id;  //the index of the result


    /**
     * Creates new form ZoomGraph
     * @param properties the Context to use
     * @param indexToZoom of the <code>XipNode</code> or <code>XipDependency</code> which we want to view some details
     * @param id the id of the result that generated the indexTozoom
     */
    public ZoomGraph(ModuleContext properties, long indexToZoom, int id) {
        super(properties);
        this.id = id;
        this.zoom = indexToZoom;
        this.dialog = new ZoomDialog();
        initComponents();
        dialog.setContentPane(this);
    }

    /**
     *Displays the zoom of the <code>XipNode</code>.
     *the zoom is a dialog containing some panels describing the <code>XipNode</code> (feaures, rules that created the node...)
     *This method creates the panels but not show the dialog window.
     *@see showInDialog(int,int)
     */
    public void displayNodeZoom() {
        SessionGraph session = getModuleContext().getSessionGraph(false, id);
        XipNode node = session.getXipNode(zoom);
        VisualItem item = session.getVisualNode(zoom);
        NodeGraph nodeGraph = session.getNodeGraph(node.getCategory() + "#" + zoom);
        if (nodeGraph == null) {
            nodeGraph = new NodeGraph(getModuleContext(), id, node);
        }
        nodeGraph.setRootComponent(this);
        this.tabNode.addTab(node + "#" + zoom, nodeGraph);
        dialog.setTitle(node + "#" + zoom);
        if (session.isAmbiguous(item)) {
            Iterator ite = node.getDaughters().iterator();
            while (ite.hasNext()) {
                XipNode dght = (XipNode) ite.next();
                String index = dght.getCategory() + "#" + dght.getIndex();
                NodeGraph dghtGraph = session.getNodeGraph(index);
                if (dghtGraph == null) {
                    dghtGraph = new NodeGraph(getModuleContext(), id, dght);
                }
                if (tabNode.indexOfTab(index) == -1) {
                    this.tabNode.addTab(index, dghtGraph);
                }
            }
        }
        if (session.isSame(item)) {
            this.tabNode.addTab("zoom#" + zoom, ChunkGraph.getSubChunkGraph(getModuleContext(), zoom, id));
        }
    }

    /**
     *Displays the zoom of the <code>XipDependency</code>
     *the zoom is a dialog containing some panels describing the <code>XipDependency</code> (feaures, rules that created the dependency...)
     *This method creates the panels but not show the dialog window.
     *@see showInDialog(int,int)
     */
    public void displayDependencyZoom() {
        SessionGraph session = getModuleContext().getSessionGraph(false, id);
        XipDependency dep = session.getXipDependency(zoom);
        NodeGraph nodeGraph = session.getNodeGraph(dep.getName() + "#" + zoom);
        if (nodeGraph == null) {
            nodeGraph = new NodeGraph(getModuleContext(), id, dep);
        }
        nodeGraph.setRootComponent(this);
        this.tabNode.addTab(dep + "#" + zoom, nodeGraph);
    }

    /**
     *Shows the dialog of the zoom, at the specified coordinates. 
     *@param x the X-coordinate.
     *@param y the Y-coordinate.
     *@see displayDependencyZoom()
     *@see displayNodeZoom()
     */
    public void showInDialog(int x, int y) {
        dialog.setLocation(x % 1024, y % 768);
        dialog.setVisible(true);
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc=" Generated Code ">//GEN-BEGIN:initComponents
    private void initComponents() {
        scroller = new javax.swing.JScrollPane();
        tabNode = new javax.swing.JTabbedPane();

        setLayout(new java.awt.BorderLayout());

        setBackground(new java.awt.Color(255, 255, 255));
        setFont(new java.awt.Font("Arial", 0, 11));
        setOpaque(false);
        tabNode.setTabLayoutPolicy(javax.swing.JTabbedPane.SCROLL_TAB_LAYOUT);
        tabNode.setTabPlacement(javax.swing.JTabbedPane.LEFT);
        tabNode.setFont(new java.awt.Font("Arial", 0, 11));
        scroller.setViewportView(tabNode);

        add(scroller, java.awt.BorderLayout.CENTER);

    }// </editor-fold>//GEN-END:initComponents

    /**
     *Close the module
     */
    public void close() {
        this.removeAll();
        this.tabNode.removeAll();
        this.isClosed = true;
        if (dialog != null || dialog.isShowing()) {
            dialog.dispose();
        }
    }
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JScrollPane scroller;
    private javax.swing.JTabbedPane tabNode;
    // End of variables declaration//GEN-END:variables

    /**
     *Class intern for the dialog 
     */
    private class ZoomDialog extends JDialog {

        public ZoomDialog() {
            super();
            setSize(400, 400);
            this.setUndecorated(false);
            this.setAlwaysOnTop(true);
            setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);
            addWindowListener(new java.awt.event.WindowAdapter() {

                public void windowClosed(java.awt.event.WindowEvent evt) {
                    formWindowClosed(evt);
                }
            });
        }

        private void formWindowClosed(java.awt.event.WindowEvent evt) {
            close();
        }
    }
}
