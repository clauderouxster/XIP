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
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;
import java.util.*;


import edu.berkeley.guir.prefuse.*;
import edu.berkeley.guir.prefuse.action.*;
import edu.berkeley.guir.prefuse.activity.*;
import edu.berkeley.guir.prefuse.graph.*;

/**
 * 
 * 
 */
public class SubChunkGraph extends ChunkGraph {

    /**
     * Represente l'arbre des chunk retourne par XIP. 
     * cette representation est interactive, c a d que son graphe est cliquable avec une souris
     *
    
     * @param properties
     * @param id 
     */
    public SubChunkGraph(ModuleContext properties, int id) {
        super(properties, id);

    }

    public void initSubGraph(XipNode node, Vector xipNodeDaughter) {
        this.optimalWidth = 0;
        SessionGraph session = context.getSessionGraph(false, id);
        graph = new DefaultGraph();
        registry = new ItemRegistry(graph);
        tableNode = new Hashtable();
        initRender();
        createActivity();

        ActionList update = new ActionList(registry);
        update.add(new ChunkGraph.DemoColorFunction(ChunkGraph.DEPTH));
        update.add(new RepaintAction());
        nodeControl.setActionList(update);

        initDisplay();

        Node ni = (Node) session.getVisualNode(node.getIndex());
        DefaultTreeNode root = new DefaultTreeNode();
        root.setAttribute(label_, node.getCategory() + "#" + node.getIndex());
        root.setAttribute(index_, String.valueOf(node.getIndex()));
        root.setAttribute(type_, NODE);
        root.setAttribute(check_, false_);

        graph.addNode(root);

        Iterator ite = xipNodeDaughter.iterator();
        while (ite.hasNext()) {
            Long index = (Long) ite.next();
            XipNode n = session.getXipNode(index.longValue());
            DefaultEdge edge = new DefaultEdge(root, constructFull(n));
            root.addChild(edge);
            graph.addEdge(edge);

        }
        //setSize();
        this.run();

    }

    private DefaultTreeNode constructFull(XipNode root) {
        if (!root.isLeaf()) {
            DefaultTreeNode node = new DefaultTreeNode();
            node.setAttribute(label_, root.toString() + "#" + root.getIndex());
            node.setAttribute(index_, String.valueOf(root.getIndex()));
            node.setAttribute(type_, NODE);
            node.setAttribute(check_, false_);
            tableNode.put(root.getIndex(), root);
            if (root.getLeft() == -100 && root.getRight() == -10) { //le noeud est fictif...
                node.setAttribute("icon", XIPUIUtilities.getIconString(UIConstants.FICTIF));
                node.setAttribute("fictif", true_);
            } else {
                node.setAttribute("fictif", false_);
            }
            graph.addNode(node);
            for (int i = 0; i < root.getDaughters().size(); i++) {
                XipNode dght = root.getDaughters().get(i);
                DefaultNode node2 = constructFull(dght);
                DefaultEdge edge = new DefaultEdge(node, node2);
                node.addChild(edge);
                graph.addEdge(edge);
            }
            return node;
        } else {
            DefaultTreeNode node = new DefaultTreeNode();
            node.setAttribute(index_, String.valueOf(root.getIndex()));
            node.setAttribute(label_, root.toString());
            node.setAttribute(type_, LEAF);
            node.setAttribute(check_, false_);
            this.optimalWidth += (12 * root.toString().length());
            tableNode.put(root.getIndex(), root);
            graph.addNode(node);
            return node;
        }
    }

    public void setSize() {
    }
}

