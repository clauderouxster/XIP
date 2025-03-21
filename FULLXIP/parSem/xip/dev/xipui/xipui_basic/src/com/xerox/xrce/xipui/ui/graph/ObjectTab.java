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
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

import com.xerox.jxip.*;

/**
 *Cette classe est un objet permettant d'effectuer un rendu sur les differents element 
 * de la table des dependances. Un ObjectTab ne fait que representer une structure de XIP
 * a l'aide d'un JLabel. Cette objet graphique est interactif. C'est a dire que l'on peut interagir
 * avec l'arbre de chunk a partir d'un ObjetTab en utilisant la souris.
 *Ceci se fait a l'aide d'une session contenant des parametres a utiliser.
 */
public class ObjectTab extends JLabel implements MouseListener {

    private ModuleContext context;
    private int id;
    private boolean isChecked; //boolean indiquant si l'objet est selectione ou non

    private long index;        //index de l'objet

    private int row;           //ligne ou se trouve objet dans la table

    private int col;           //colonne ou se trouve l'objet dans la table

    private Object obj;        //objet a representer 

    /**
     *Couleur de fond d'un noeud
     */
    public static final Color nodeFillColor = ConstantsGraph.NODE_FILL;
    /**
     *Couleur du texte d'un noeud
     */
    public static final Color nodeColor = ConstantsGraph.NODE_COLOR;
    /**
     *Couleur de fond pour uen dependances
     */
    public static final Color depFillColor = ConstantsGraph.DEPENDENCY_FILL;
    /**
     *Couleur du texte pour une dependance
     */
    public static final Color depColor = ConstantsGraph.DEPENDENCY_COLOR;
    ;
    /**
     *Couleur pour un element selectionne
     */
    public static final Color checkFillColor = ConstantsGraph.CHECKED_FILL;
    ;
    /**
     *couleur du texte pour un elemenet selectionne
     */
    public static final Color checkColor = ConstantsGraph.CHECKED_COLOR;

    /**
     *Constructeur minimaliste, 
    
     * 
     *
     * @param properties
     * @param id 
     */
    public ObjectTab(ModuleContext properties, int id) {
        this.id = id;
        this.context = properties;
        setOpaque(true);
        setFont(UIConstants.DEFAULT_FONT);
        this.obj = new Object();
        isChecked = false;
    }

    /**
     *Constructeur d'un ObjectTab
     *@param properties 
     * @param id 
     * @param obj_ objet a representer
     *@see com.xerox.xrce.lingdexip.ui.run.graph.ObjectTab#(SessionGraph)
     *@see com.xerox.xrce.lingdexip.ui.run.graph.ObjectTab#setObject(Object)
     */
    public ObjectTab(ModuleContext properties, int id, Object obj_) {
        this(properties, id);
        setObject(obj_);

    }

    /**
     *Initialise la position de l'objet par rapport a la table
     *@param row numero de la ligne
     *@param col numero de la colonne 
     */
    public void setPosition(int row, int col) {
        this.row = row;
        this.col = col;
    }

    /**
     *Renvoie le numero de la colonne ou se trouve l'objet dans la table
     *@return numero de la colonne
     *
     **@see com.xerox.xrce.lingdexip.ui.run.graph.ObjectTab#getRow()
     */
    public int getCol() {
        return this.col;
    }

    /**
     *Renvoie le numero de la ligne ou se trouve l'objet dans la table
     *@return numero de ligne
     *
     *@see com.xerox.xrce.lingdexip.ui.run.graph.ObjectTab#getCol()
     */
    public int getRow() {
        return this.row;
    }

    /**
     *Selectionne ou non l'objet, on lui change les parametres de visualisation.
     *@param b true selectionne l'objet , false le deselectionne
     *
     *@see com.xerox.xrce.lingdexip.ui.run.graph.ObjectTab#reset()
     */
    public void setChecked(boolean b) {
        isChecked = b;
        if (isChecked) {
            setBackground(checkFillColor);
            setForeground(checkColor);
        } else {
            reset();
        }
    }

    /**
     *Verifie si l'objet est selectionne ou non
     *@return true si oui false sinon
     */
    public boolean isChecked() {
        return this.isChecked;
    }

    /**
     *Verifie si l'objet contenu est une dependance
     *@return true si oui, false sinon
     *
     *@see com.xerox.jxip.XipDependency
     */
    public boolean isDependency() {
        return obj instanceof XipDependency;
    }

    /**
     *Verifie si l'objet contenu est un XipNode
     *@return true si oui false sinon
     *
     *@see com.xerox.jxip.XipNode
     */
    public boolean isNode() {
        return obj instanceof XipNode;
    }

    /**
     *Reset un Object tab, remet a jour les couleurs et le texte a afficher dans la table
     *selon l'objet contenu.
     *
     *@see com.xerox.xrce.lingdexip.ui.run.graph.ObjectTab#isDependency()
     *@see com.xerox.xrce.lingdexip.ui.run.graph.ObjectTab#isNode()
     */
    public void reset() {
        this.isChecked = false;
        if (isDependency()) {
            XipDependency dep = (XipDependency) obj;
            setText(dep.toString());
            setToolTipText(dep.toString() + "#" + dep.getIndex());
            setBackground(depFillColor);
            setForeground(depColor);
            setIcon(XIPUIUtilities.getIcon(UIConstants.DEPENDENCY));
            index = dep.getIndex();
        } else if (isNode()) {
            XipNode node = (XipNode) obj;
            setToolTipText(node.getCategory());
            setText(node.getSurface() + "#" + node.getIndex());
            setBackground(nodeFillColor);
            setForeground(nodeColor);
            index = node.getIndex();
        }
    }

    /**
     *Initialise l'objet contenu dans cet ObjectTab
     *@param obj objet a representer
     *
     *@see com.xerox.xrce.lingdexip.ui.run.graph.ObjectTab#reset()
     */
    public void setObject(Object obj) {
        this.obj = obj;
        this.addMouseListener(this);
        reset();
    }

    /**
     *Renvoie l'objet contenu dans cet ObjectTab
     *@return objet contenu.
     */
    public Object getObject() {
        return this.obj;
    }

    /**
     *Renvoie l'index de cet ObjetTab
     *@return index de l'objet
     *NB : l'index a du sens uniquement si l'objet que l'on veut representer a un index.
     *l'index est calcule dans la methode reset de l'objet
     */
    public long getIndex() {
        return this.index;
    }

    /**
     *Renvoie la representation d'un ObjectTab sous forme d'une chaine de caracteres
     *@return fait appel a la fonction toString de l'objet. Si l'objet est null alors on renvoie une chaine contenant "null"
     */
    public String toString() {
        if (obj == null) {
            return new String("null");
        }
        return this.obj.toString();
    }

    /**
     *Evenement lors d'un clique sur un ObjectTab
     *On change le rendu de l'objet en fonction du clique, ainsi que son influence sur les objets de la session
     *des object selectionnes.
     *@param evt evenement souris
     *
    
     *
     */
    public void mouseClicked(MouseEvent evt) {
        SessionGraph session = context.getSessionGraph(false, id);
        if (evt.getButton() == MouseEvent.BUTTON3) {
            //si bouton3 alors on affiche un popup
            ZoomGraph zoom = new ZoomGraph(context, getIndex(), id);
            if (isDependency()) {
                zoom.displayDependencyZoom();
            } else {
                zoom.displayNodeZoom();
            }
            zoom.showInDialog(evt.getX(), evt.getY());
        }
        if (session.isSelectable()) {
            if (evt.getButton() == MouseEvent.BUTTON1 && evt.getClickCount() == 2) {
                //verifie si l'objet est deje selectionne
                if (!isChecked()) {
                    if (!isDependency()) {
                        session.selectXipNode(index);
                    } else {
                        session.selectXipDependency(index);
                    }
                } else {
                    if (!isDependency()) {
                        session.unSelectXipNode(index);
                    } else {
                        session.unSelectXipDependency(index);
                    }
                }
            }
        }
    }

    /**
     *evenement lors d'une pression d'un bouton de la souris sur un ObjectTab
     *on ne fait tien
     *@param evt evenement souris
     */
    public void mousePressed(MouseEvent evt) {
    }

    /**
     *evenement lors d'une pression d'un bouton de la souris sur un ObjectTab
     *on ne fait tien 
     *@param evt evenement souris
     */
    public void mouseEntered(MouseEvent evt) {
    }

    /**
     *evenement lors d'une pression d'un bouton de la souris sur un ObjectTab
     *on ne fait tien 
     *@param evt evenement souris
     */
    public void mouseExited(MouseEvent evt) {
    }

    /**
     *evenement lors d'une pression d'un bouton de la souris sur un ObjectTab
     *on ne fait tien 
     *@param evt evenement souris.
     */
    public void mouseReleased(MouseEvent evt) {
    }
}
