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

import com.xerox.xrce.xipui.util.Context;
import com.xerox.xrce.xipui.ui.*;
import com.xerox.xrce.xipui.modules.*;
import java.util.*;

import com.xerox.jxip.*;
import edu.berkeley.guir.prefuse.activity.*;
import edu.berkeley.guir.prefuse.*;
import edu.berkeley.guir.prefuse.graph.*;
import javax.swing.*;

/**
 *
 * @author vgrassau
 */
public class SessionGraph extends Context {

    public final String CHUNK_REGISTRY = "CHUNK_REGISTRY";          //acces au registre de l'arbre de chunk

    public final String DEP_REGISTRY = "DEP_REGISTRY";            //acces aux dependences

    public final String TABLE = "TABLE";                   //acces a la table graphique representant la table des dependances

    public final String NAME = "NAME";                    //nom de l'analyse graphique

    public final String DEPENDENCIES = "DEPENDENCIES";            //table des dependances

    public final String TABLE_NODE = "TABLE_NODE";             // table des xipnodes

    public final String FOCUS = "FOCUS";                  // focus des noeuds selectionnes

    public final String VECTOR_NODEGRAPH = "VECTOR_NODEGRAPH"; // focus des noeuds slectionnes

    public final String VIEWER_GRAPH = "VIEWER_GRAPH"; // objet permettant de visualiser les objets selectionnes.

    public final String ACTIONS_CHUNK = "ACTIONS_CHUNK";
    private int idSession;
    private boolean generate = true;
    private ModuleContext context;
    private boolean isLeafControl = true;

    /** Creates a new instance of SessionGraph
     * @param properties
     * @param idSession 
     */
    public SessionGraph(ModuleContext properties, int idSession) {
        super(properties);
        context = properties;
        this.idSession = idSession;
        initContext();
    }

    /**
     *Enabled or not the control on the leaves of the chunk graph
     *@param b true for enabled false otherwise
     */
    public void setLeafControl(boolean b) {
        this.isLeafControl = b;
    }

    /**
     *Check if the control on the leaves of the chunk graph is enabled or not
     *@return true if enabled false otherwise
     */
    public boolean isLeafControl() {
        return this.isLeafControl;
    }

    public void invalidate() {
        clear();
        context.remove(ModuleContext.SESSION);
        context = null;
    }

    public boolean initContext() {
        //this.clear();
        //we put some parameters, these are esstentials
        // the dependencies registry it's a vector 
        setProperty(DEP_REGISTRY, new Vector());
        //the table of dependency it's a JTable 
        setProperty(TABLE, new JTable());
        //the vector of NodeGraph, ie the nodes or dependencies selected           
        setProperty(VECTOR_NODEGRAPH, new Vector());
        //the graph for the chunk graph
        DefaultGraph gChunk = new DefaultGraph();
        setProperty(CHUNK_REGISTRY, new ItemRegistry(gChunk));
        return true;
    }

    public long getIdSession() {
        return this.idSession;
    }

    public void setViewer(Viewer viewer) {
        setProperty(VIEWER_GRAPH, viewer);
        JTabbedPane tab = (JTabbedPane) getProperty(IModule.OPTION_TABBED);
        viewer.setRootComponent(tab);
        XIPUIUtilities.addComponentOnTab(tab, viewer, null, "SelectedObject", "");
    }

    public Viewer getViewer() {
        return (Viewer) getProperty(VIEWER_GRAPH);
    }

    public void refreshViewer() {
        Viewer v = getViewer();
        if (v != null) {
            v.refresh();
            v.repaint();
        }
    }

    public boolean isSelectable() {
        Viewer v = getViewer();
        if (v == null) {
            return false;
        } else {
            return true;
        }
    }

    public void repaintViewer() {
        Viewer v = getViewer();
        if (v != null) {
            v.repaint();
        }
    }

    public void setDependencies(Hashtable dependencies) {
        setProperty(DEPENDENCIES, dependencies);
    }

    public Hashtable getDependencies() {
        return (Hashtable) getProperty(this.DEPENDENCIES);
    }

    public XipDependency getXipDependency(long index) {
        Hashtable table = (Hashtable) getProperty(DEPENDENCIES);
        if (table != null) {
            return (XipDependency) table.get(index);
        } else {
            return null;
        }
    }

    public void setTableNode(Hashtable table) {
        setProperty(TABLE_NODE, table);
    }

    public Hashtable getTableNode() {
        return (Hashtable) getProperty(TABLE_NODE);
    }

    public Vector getNodeGraphSelected() {
        return (Vector) getProperty(VECTOR_NODEGRAPH);
    }

    public XipNode getXipNode(long index) {
        Hashtable table = (Hashtable) this.getProperty(TABLE_NODE);
        if (table != null) {
            return (XipNode) table.get(index);
        } else {
            return null;
        }
    }

    public ItemRegistry getChunkRegistry() {
        return (ItemRegistry) this.getProperty(this.CHUNK_REGISTRY);
    }

    public VisualItem getVisualNode(long index) {
        ItemRegistry registry = (ItemRegistry) this.getProperty(this.CHUNK_REGISTRY);
        Iterator chunkIter = registry.getNodeItems();
        while (chunkIter.hasNext()) {
            NodeItem item = (NodeItem) chunkIter.next();
            String index_ = item.getAttribute("index");
            if (index == new Long(index_).longValue()) {
                return item;
            }
        }
        return null;
    }

    public boolean isAmbiguous(VisualItem item) {
        if (item != null) {
            String attr = item.getAttribute("ambiguous");
            if (attr != null) {
                return attr.equalsIgnoreCase("true");
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    public boolean isSame(VisualItem item) {
        if (item != null) {
            String attr = item.getAttribute("same");
            if (attr != null) {
                return attr.equalsIgnoreCase("true");
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    public void setTableAmbiguous(Hashtable table) {
        setProperty("TABLE_AMBIGUOUS", table);
    }

    public Vector getVectorAmbiguous(long index) {
        Hashtable h = (Hashtable) getProperty("TABLE_AMBIGUOUS");
        if (h != null) {
            return (Vector) h.get(index);
        } else {
            return null;
        }
    }

    public boolean isFictif(VisualItem item) {
        String attr = item.getAttribute("fictif");
        if (attr != null) {
            return attr.equalsIgnoreCase("true");
        } else {
            return false;
        }
    }

    /**
     *Verifie si un noeud de l'arbre a plusieurs lemme
     * @param item
     * @return 
     */
    public boolean isMultipleLemma(VisualItem item) {
        String attr = item.getAttribute("multipleLemma");
        if (attr != null) {
            return attr.equalsIgnoreCase("true");
        } else {
            return false;
        }
    }

    /**
     *Renvoie tous les objets selectionne, cad les SelectedObject des NodeGraph
     *@return vector un vector de SelectedObject, ou vide si pas de NodeGraph
     */
    public Vector getAllSelectedObject() {
        Vector nodeGraphs = this.getNodeGraphSelected();
        Vector res = new Vector();
        if (nodeGraphs != null) {
            Iterator ite = nodeGraphs.iterator();
            while (ite.hasNext()) {
                NodeGraph ng = (NodeGraph) ite.next();
                res.add(ng.getSelectedObject());
            }
        }
        return res;
    }

    /**
     *Renvoie le nodeGraph correspondant au nom passe en parametre : 
     *
     *@param name Le nom et de type : categorie#index
     *@return NodeGraph si trouve null sinon
     */
    public NodeGraph getNodeGraph(String name) {
        Vector v = (Vector) this.getProperty(VECTOR_NODEGRAPH);
        if (v != null) {
            Iterator ite = v.iterator();
            while (ite.hasNext()) {
                NodeGraph ng = (NodeGraph) ite.next();
                if (ng.getName().equals(name)) {
                    return ng;
                }
            }
        }
        return null;
    }

    /**
     *Ajoute un NodeGraph au Vector des NodeGraph de la session
     *@param ng NodeGraph a rajouter
     *
     *la visualisation de l'objet cad XipNode ou XipDependency correspondant au NodeGraph sera 
    modifiee 
     */
    public void addNodeGraph(NodeGraph ng) {
        Vector v = (Vector) this.getProperty(VECTOR_NODEGRAPH);
        if (v != null && getNodeGraph(ng.getName()) == null) {
            v.add(ng);
            if (ng.isXipNode()) {
                checkNode(ng.getIndex(), true);
            } else {
                this.checkDependency(ng.getIndex(), true);
            }
        }
    }

    /**
     *Selectionne un noeud ou le deselectionne , uniquement au niveau visualisation.
     *@param index index du noeud (XipNode, XipDependency) 
     *@param b true : selectionne le noeud false le deselectionne.
     *
     *
     */
    public void checkNode(long index, boolean b) {
        Hashtable tableNode = (Hashtable) this.getProperty(this.TABLE_NODE);
        ItemRegistry chunkRegistry = (ItemRegistry) getProperty(this.CHUNK_REGISTRY);
        Vector depRegistry = (Vector) getProperty(this.DEP_REGISTRY);
        JTable table = (JTable) getProperty(this.TABLE);
        if (tableNode != null && depRegistry != null && chunkRegistry != null && table != null) {
            XipNode node = (XipNode) tableNode.get(index);
            if (node != null) {
                Long indexToCheck = new Long(node.getIndex());
                //les noeuds dans l'arbre de chunk
                Iterator chunkIter = chunkRegistry.getNodeItems();
                while (chunkIter.hasNext()) {
                    NodeItem item = (NodeItem) chunkIter.next();
                    String index_ = item.getAttribute("index");
                    if (indexToCheck.equals(new Long(index_))) {
                        item.setAttribute("check", String.valueOf(b));
                    }
                }
                //les noeuds pour la table des dependances
                Enumeration depIter = depRegistry.elements();
                while (depIter.hasMoreElements()) {
                    ObjectTab item = (ObjectTab) depIter.nextElement();
                    if (!item.isDependency() && indexToCheck.equals(new Long(item.getIndex()))) {
                        item.setChecked(b);
                        table.setValueAt(item, item.getRow(), item.getCol());
                        table.repaint();
                    }
                }
            }
        }
        ActionList update = (ActionList) this.getProperty(this.ACTIONS_CHUNK);
        if (update != null && !update.isRunning()) {
            update.runNow();
        }
    }

    /**
     *Selection d'un XipNode et l'ajoute a la liste des NodeGraph
     *@param index index du noeud a selectionner dans la table
     *
     *@see #getXipNode(int)
     *@see #getNodeGraph(String)
     *@see NodeGraph
     *
     *NB le viewer de NodeGraph est rafraichi
     */
    public void selectXipNode(long index) {
        XipNode node = getXipNode(index);
        NodeGraph nodeGraph = getNodeGraph(node.getCategory() + "#" + index);
        if (nodeGraph == null) {
            nodeGraph = new NodeGraph(context, idSession, node);
        }
        this.addNodeGraph(nodeGraph);
        this.refreshViewer();
    }

    /**
     *Selection d'une dependance, cad d'un XipDependenct
     *@param index index de la dependance
     *
     *@see getXipDependency(int)
     *@see getNodeGraph(String)
     *@see NodeGraph
     *
     *NB le viewer de NodeGraph est rafraichi
     */
    public void selectXipDependency(long index) {
        XipDependency dep = getXipDependency(index);
        NodeGraph nodeGraph = getNodeGraph(dep + "#" + index);
        if (nodeGraph == null) {
            nodeGraph = new NodeGraph(context, idSession, dep);
        }
        this.addNodeGraph(nodeGraph);
        this.refreshViewer();
    }

    /**
     *Deselectionne une dependance cad un XipDependency 
     *@param index index de la dependance a deselectionner
     *
     *Le dependance sera supprimee du vectir de NodeGraph
     *Le rendu de la deselection sera fait par la methode : 
     *checkDependency(int,boolean)
     *
     *NB le viewer de NodeGraph est rafraichi
     */
    public void unSelectXipDependency(long index) {
        Vector v = (Vector) this.getProperty(VECTOR_NODEGRAPH);
        boolean stop = false;
        Iterator ite = v.iterator();
        while (ite.hasNext() && !stop) {
            NodeGraph ng = (NodeGraph) ite.next();
            if (ng.getIndex() == index && ng.isXipDependency()) {
                stop = true;
                ite.remove();
            }
        }
        this.checkDependency(index, false);
        this.refreshViewer();
    }

    /**
     *Deselectionne un noeud  cad un XipNode 
     *@param index index du noeud a deselectionner
     *
     *Le noeud sera supprime du vector de NodeGraph
     *Le rendu de la deselection sera fait par la methode : 
     *checkNode(int,boolean)
     *
     *NB le viewer de NodeGraph est rafraichi
     */
    public void unSelectXipNode(long index) {
        Vector v = (Vector) this.getProperty(VECTOR_NODEGRAPH);
        boolean stop = false;
        Iterator ite = v.iterator();
        while (ite.hasNext() && !stop) {
            NodeGraph ng = (NodeGraph) ite.next();
            if (ng.getIndex() == index && ng.isXipNode()) {
                stop = true;
                ite.remove();
            }
        }
        this.checkNode(index, false);
        this.refreshViewer();
    }

    /**
     *Renvoie un objet du registre de la table des dependances.
     *Ce registre contient des ObjectTab. On ne retourne l'objectTab 
     *que s'il contient une dependance et non un XipNode
     *@param index index de l'objet (XipDependency) a chercher
     *@return ObjectTab correspondant a l'index ou null si index non trouve
     */
    public ObjectTab getObjectTabDependency(long index) {
        Vector v = (Vector) getProperty(DEP_REGISTRY);
        if (v != null) {
            Iterator ite = v.iterator();
            while (ite.hasNext()) {
                ObjectTab ot = (ObjectTab) ite.next();
                if (ot.isDependency() && ot.getIndex() == index) {
                    return ot;
                }
            }
        }
        return null;
    }

    /**
     *Renvoie un objet du registre de la table des dependances.
     *Ce registre contient des ObjectTab. On ne retourne l'objectTab 
     *que s'il contient un XipNode et non un XipDependency
     *@param index index de l'objet (XipNode) a chercher
     *@return ObjectTab correspondant a l'index ou null si index non trouve
     */
    public ObjectTab getObjectTabNode(long index) {
        Vector v = (Vector) getProperty(this.DEP_REGISTRY);
        if (v != null) {
            Iterator ite = v.iterator();
            while (ite.hasNext()) {
                ObjectTab ot = (ObjectTab) ite.next();
                if (!ot.isDependency() && ot.getIndex() == index) {
                    return ot;
                }
            }
        }
        return null;
    }

    /**
     *Selectionne un dependance
     *@param index
     *@param b boolean true pour selectionner, false sinon
     *
     *@see #getObjectTabDependency(int)
     */
    public void checkDependency(long index, boolean b) {
        ObjectTab item = getObjectTabDependency(index);
        item.setChecked(b);
        JTable table = (JTable) getProperty(this.TABLE);
        if (table != null) {
            table.setValueAt(item, item.getRow(), item.getCol());
            table.repaint();
        }
    }
}
