/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 ****************************************************************************************
 * Copyright (c) 2006 Xerox Corporation                                                 *
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
package com.xerox.xrce.xipui.ide.generator;

import com.xerox.jxip.*;
import com.xerox.xrce.xipui.Xip;
import com.xerox.xrce.xipui.XipUtilities;
import com.xerox.xrce.xipui.ui.graph.*;

import com.xerox.xrce.xipui.modules.ModuleContext;
import java.util.*;

/**
 *
 * @author csaillet
 */
public class CommonMethod {

    /** Creates a new instance of CommonMethod */
    public CommonMethod() {
    }

//    /** 
//     * concatenation de deux Vector vec1 et vec2 dans vec1 
//     * // remark by Vianney Grassaud 
//     * // this method can be replace by a addAll method 
//     */
//    public static Vector concVector(Vector vec1, Vector vec2){
//        int size_vec1 = vec1.size();
//        int tot= vec1.size()+vec2.size();
//        int k =0;
//        for (int i= size_vec1;i<tot;i++){
//            vec1.addElement(vec2.get(k));
//            k++;
//        }
//        return vec1;
//    }
    /** soustraction de deux Vector vec1 et vec2
     * @param vec1
     * @param vec2
     * @return 
     */
    public Vector subVector(Vector vec1, Vector vec2) {

        Iterator it = vec2.iterator();
        while (it.hasNext()) {
            SelectedObject select = (SelectedObject) it.next();
            Iterator ite = vec1.iterator();
            while (ite.hasNext()) {
                SelectedObject selObj = (SelectedObject) ite.next();
                if (select.getObject() instanceof XipNode && selObj.getObject() instanceof XipNode) {
                    if (((XipNode) select.getObject()).getIndex() == ((XipNode) selObj.getObject()).getIndex()) {
                        ite.remove();
                    }
                } else if (select.getObject() instanceof XipDependency && selObj.getObject() instanceof XipNode) {
                    if (((XipDependency) select.getObject()).getIndex() == ((XipNode) selObj.getObject()).getIndex()) {
                        ite.remove();
                    }
                } else if (select.getObject() instanceof XipNode && selObj.getObject() instanceof XipDependency) {
                    if (((XipNode) select.getObject()).getIndex() == ((XipDependency) selObj.getObject()).getIndex()) {
                        ite.remove();
                    }
                } else if (select.getObject() instanceof XipDependency && selObj.getObject() instanceof XipDependency) {
                    if (((XipDependency) select.getObject()).getIndex() == ((XipDependency) selObj.getObject()).getIndex()) {
                        ite.remove();
                    }
                }
            }
        }
        return vec1;
    }

    //recupere l index le plus petit des elements du vecteur de selectedObject
    public long findMinSelObject(Vector vec) {
        long res = 9000;
        Iterator it = vec.iterator();
        while (it.hasNext()) {
            SelectedObject selObj = (SelectedObject) it.next();
            if (selObj.getObject() instanceof XipNode) {
                long ind = ((XipNode) selObj.getObject()).getIndex();
                if (ind < res) {
                    res = ind;
                }
            } else {
                long ind = ((XipDependency) selObj.getObject()).getIndex();
                if (ind < res) {
                    res = ind;
                }
            }
        }
        return res;
    }

    //recupere l index le plus grand des elements du vecteur de selectedObject
    public long findMaxSelObject(Vector vec) {
        long res = 0;
        Iterator it = vec.iterator();
        while (it.hasNext()) {
            SelectedObject selObj = (SelectedObject) it.next();
            if (selObj.getObject() instanceof XipNode) {
                long ind = ((XipNode) selObj.getObject()).getIndex();
                if (ind > res) {
                    res = ind;
                }
            } else {
                long ind = ((XipDependency) selObj.getObject()).getIndex();
            }

        }
        return res;
    }

    // vec est un vector de nodeGraph, index est un index de nodeGraph
    // retrouve le nodeGraph d index index et le supprime du vector
    public void removeNodeGraphFromIndex(Vector vec, long index) {
        boolean stop = false;
        Iterator it = vec.iterator();
        while (it.hasNext() && stop == false) {
            SelectedObject selObj = (SelectedObject) it.next();
            long indNode = ((XipNode) selObj.getObject()).getIndex();
            if (indNode == index) {
                it.remove();
                stop = true;
            }
        }
    }

    // deselectionne dans l arbre de chunk tous les elements d un vecteur de selectedObject
    public void unselectXipNodeFromVector(Vector vect, SessionGraph session) {
        Iterator it = vect.iterator();
        while (it.hasNext()) {
            SelectedObject obj = (SelectedObject) it.next();
            if (obj.getObject() instanceof XipDependency) {
                long index = ((XipDependency) obj.getObject()).getIndex();
                session.unSelectXipDependency(index);
            } else if (obj.getObject() instanceof XipNode) {
                long index = ((XipNode) obj.getObject()).getIndex();
                session.unSelectXipNode(index);
            }
        }
    }

    // deselectionne dans l arbre de chunk tous les elements (XipDependency) d un vecteur de selectedObject
    public void unselectXipDependencyFromVector(Vector vect, SessionGraph session) {
        Iterator it = vect.iterator();
        while (it.hasNext()) {
            SelectedObject obj = (SelectedObject) it.next();
            long index = ((XipDependency) obj.getObject()).getIndex();
            session.unSelectXipDependency(index);
        }
    }

    // this method is defined in the Object XipUtilities with the same signature
//    public static boolean containsFeature(Vector features, XipFeature feat) {
//        //Vector features = new Vector();
//        Iterator ite = features.iterator();
//        boolean stop = false;
//        String toCompare = feat.getName() + ":" + feat.getValue();
//        while ( ite.hasNext() && !stop) {
//            XipFeature curFeat = (XipFeature)ite.next();
//            String value = curFeat.getName() + ":" + curFeat.getValue();
//            if ( value.equals(toCompare) ) {
//                stop = true;
//            }
//        }
//        return stop;
//    }
    /** XipNode xipNod est un noeud
     * verifie si ce noeud a un fils
     * et si le fils est une feuille
     * @param xipNod 
     * @return vrai si le fils est une feuille faux sinon
     */
    public static boolean herDaughterIsLeaf(XipNode xipNod) {
        boolean res = false;
        Vector vec = xipNod.getDaughters();
        Iterator it = vec.iterator();
        while (it.hasNext() && !res) {
            XipNode xNod = (XipNode) it.next();
            if (xNod.isLeaf()) {
                res = true;
            }
        }
        return res;
    }

    /** recupere les feuilles du xipNode et les mets dans le vector vec
     * @param xipNode
     * @param vec
     * @param xip
     * @param ghandler
     * @throws java.lang.Exception 
     */
    public static void readXipNode(XipNode xipNode, Vector vec, Xip xip, int ghandler) throws Exception {
        boolean bool;
        Vector features = xipNode.getFeatures();
        int size = features.size();
        bool = herDaughterIsLeaf(xipNode);
        if (bool) {   //si xipNode a un fils qui est une feuille

            XipLeaf leaf = (XipLeaf) xipNode.getDaughters().get(0);
            if (ghandler == -1) {
                //DEBUG_LOG.trace("ghandler = -1");
                System.out.println("ghandler = -1 !!!!!!!!");
            }
            XipFeature surfaceFeature = new XipFeature(xip.getLabelsSurface(ghandler), leaf.getSurface(), size);
            XipFeature lemmaFeature = new XipFeature(xip.getLabelsLemma(ghandler), leaf.getLemma(), size + 1);
            if (!XipUtilities.containsFeature(features, surfaceFeature)) {
                features.add(surfaceFeature);
            }
            if (!XipUtilities.containsFeature(features, lemmaFeature)) {
                features.add(lemmaFeature);
            }
//            System.out.println(xipNode.getCategory());
        }
        if (xipNode.isLeaf()) {
            vec.add(xipNode);
        } else {
            vec.add(xipNode);
            Vector vOfChild = xipNode.getDaughters();
            for (int i = 0; i < vOfChild.size(); i++) {
                XipNode node = (XipNode) vOfChild.get(i);
                readXipNode(node, vec, xip, ghandler);
            }
        }
    }

    /** ecrit (dans la console) les XipNode compris dans le vector vec
     * @param vec 
     */
    public static void printXipNode(Vector vec) {
        Iterator it = vec.iterator();
        while (it.hasNext()) {
            XipNode xNode = (XipNode) it.next();
            System.out.println(xNode.getIndex() + "#" + xNode.toString(true, "\t", false));
        }
    }

    /** recupere les xipNodes contenus dans un vector de NodeGraph
     * les mets dans un vecteur de xipNode
     * @param context
     * @param id
     * @param vNodeGraph
     * @return 
     */
    public Vector vectNodeGraphToSelectedObject(ModuleContext context, int id, Vector vNodeGraph) {
        long index;
        Vector vSelectObj = new Vector();
        Vector vXipNode = new Vector();
        Hashtable table = context.getSessionGraph(false, id).getTableNode();
        Iterator it = vNodeGraph.iterator();
        while (it.hasNext()) {
            NodeGraph node = (NodeGraph) it.next();
            index = node.getIndex();
            if (table.get(index) != null) {
                SelectedObject sObj = node.getSelectedObject();
                vSelectObj.add(sObj);
            }
        }
        return vSelectObj;
    }

    /** recupere l index des noeuds selectionnes
     * return lindex-1
     * @param index
     * @return 
     */
    public static int[] indexChoice(int[] index) {
        int[] res = new int[index.length];
        if (index.length != 0) {
            for (int i = 0; i < index.length; i++) {
                res[i] = index[i] - 1;
                System.out.println("res[" + i + "]=" + res[i]);
            }
            return res;
        } else {
            int res2[] = {};
            return res2;
        }
    }

    /** Vector vNode  vecteur des neouds selectionnes
     * return un tableau des indices -1 de ces noeuds
     * @param vNode
     * @return 
     */
    public int[] indexSelectedNodes(Vector vNode) {
        int[] res = new int[vNode.size()];
        int[] result;
        for (int i = 0; i < res.length; i++) {
            res[i] = (int) ((NodeGraph) vNode.get(i)).getIndex();
        }
        result = indexChoice(res);
        return result;
    }

    /* recupere le lemme du premier element du body
     */
    public static Vector constructVectorFromArray(Vector ve, int[] indexbody) {
        Vector vec = new Vector();
        for (int i = 0; i < indexbody.length; i++) {
            XipNode leaf = (XipNode) ve.get(indexbody[i]);
            vec.add(leaf);
        }
        return vec;
    }

    public static XipNode xipNodeFromVector(Vector vec, long min) {
        XipNode node = null;
        boolean stop = false;
        Iterator it = vec.iterator();
        while (it.hasNext() && stop == false) {
            SelectedObject selObj = (SelectedObject) it.next();
            long index = ((XipNode) selObj.getObject()).getIndex();
            if (index == min) {
                node = (XipNode) selObj.getObject();
                stop = true;
            }
        }
        return node;
    }

    /** deselectionne sur l arbre de chunk les noeuds dont les index sont dans int[] select
     *
     * @param vec
     * @param select
     * @param session 
     */
    public void unSelectNodeFromArray(Vector vec, int[] select, SessionGraph session) {
        XipNode node = null;
        XipDependency dependency = null;
        boolean stop = false;
        long indexnode = -1;
        for (int j = 0; j < select.length; j++) {
            select[j] = select[j] + 1;
        }
        for (int i = 0; i < select.length; i++) {
            long index = (long) select[i];

            Iterator it = vec.iterator();
            stop = false;
            while (it.hasNext() && stop == false) {
                SelectedObject selobj = (SelectedObject) it.next();
                Object xnode = selobj.getObject();
                if (xnode instanceof XipNode) {
                    node = (XipNode) xnode;
                    indexnode = node.getIndex();
                    if (indexnode == index) {
                        session.unSelectXipNode(index);
                        stop = true;
                    }
                } else if (xnode instanceof XipDependency) {
                    dependency = (XipDependency) xnode;
                    indexnode = dependency.getIndex();
                    if (indexnode == index) {
                        session.unSelectXipDependency(index);
                        stop = true;
                    }
                }
            }

        }
    }

    //return true si ng appartient au vector, vec est un vector d index
    public boolean isFocus(NodeGraph ng, Vector vec) {
        boolean res = false;
        if (vec.size() != 0) {
            Iterator it = vec.iterator();
            while (it.hasNext() && res == false) {
                long index = (Long) it.next();
                if (ng.getIndex() == index) {
                    res = true;
                }
            }
        }
        return res;
    }

// return true si ng appartient au vector, vec est un vector de NodeGraph
    // index est l index d un NodeGraph
    public boolean isKeep(long index, Vector vec) {
        boolean res = false;
        if (vec.size() != 0) {
            Iterator it = vec.iterator();
            while (it.hasNext() && res == false) {
                NodeGraph node = (NodeGraph) it.next();
                if (index == node.getIndex()) {
                    res = true;
                }
            }
        }
        return res;
    }

    // reselectionne des noeuds deselectionnes par erreur
    public void selectNodeFromArray(Vector ve, int[] select, SessionGraph session) {
        boolean sel;
        for (int i = 0; i < select.length; i++) {
            sel = false;
            long index = (long) select[i] + 1;
            Vector vectNodeGraph = session.getNodeGraphSelected();
            sel = isKeep(index, vectNodeGraph);
            if (sel == false) {
                session.selectXipNode(index);
            }
        }
    }

    // les vectors allSelect et context contiennent des SelectedObject
    // verifie que tous les SelectedObject sont inclus dans les SelectedObject de allSelect
    //  s il en manque un return son index, sinon -1
    public long verifyNodeSelection(Vector allSelect, Vector context) {
        long res = -1;
        boolean res2 = false;
        Iterator it = context.iterator();
        while (it.hasNext() && res == -1) {
            SelectedObject selectContext = (SelectedObject) it.next();
            res2 = false;
            Iterator ite = allSelect.iterator();
            while (ite.hasNext() && res2 == false) {
                SelectedObject selectAll = (SelectedObject) ite.next();
                if (selectContext.getObject() instanceof XipNode && selectAll.getObject() instanceof XipNode) {
                    if (((XipNode) selectContext.getObject()).getIndex() == ((XipNode) selectAll.getObject()).getIndex()) {
                        res2 = true;
                    }
                } else if (selectContext.getObject() instanceof XipDependency && selectAll.getObject() instanceof XipNode) {
                    if (((XipDependency) selectContext.getObject()).getIndex() == ((XipNode) selectAll.getObject()).getIndex()) {
                        res2 = true;
                    }
                } else if (selectContext.getObject() instanceof XipNode && selectAll.getObject() instanceof XipDependency) {
                    if (((XipNode) selectContext.getObject()).getIndex() == ((XipDependency) selectAll.getObject()).getIndex()) {
                        res2 = true;
                    }
                } else if (selectContext.getObject() instanceof XipDependency && selectAll.getObject() instanceof XipDependency) {
                    if (((XipDependency) selectContext.getObject()).getIndex() == ((XipDependency) selectAll.getObject()).getIndex()) {
                        res2 = true;
                    }
                }
            }
            if (res2 == false && selectContext.getObject() instanceof XipNode) {
                res = ((XipNode) selectContext.getObject()).getIndex();
            } else if (res2 == false && selectContext.getObject() instanceof XipDependency) {
                res = ((XipDependency) selectContext.getObject()).getIndex();
            }
        }
        return res;
    }
}






