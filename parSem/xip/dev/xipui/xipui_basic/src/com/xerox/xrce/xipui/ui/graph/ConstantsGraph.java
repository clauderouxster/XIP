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

import java.awt.*;

/**
 *
 * @author vgrassau
 */
public interface ConstantsGraph {

    /**
     *Couleur des aretes d'un graphe
     */
    public final Color EDGE_COLOR = Color.BLUE;
    /**
     *Couleur d'highlight
     */
    public final Color HILGHLIGHT_COLOR = new Color(50, 50, 255);
    /**
     *Couleur de focus
     */
    //  public  final  Color FOCUS_COLOR = new Color(253, 218, 217);
    /**
     *Couleur des feuilles
     */
    public final Color LEAF_COLOR = new Color(255, 255, 255);
    /**
     *Couleur de fond pour les noeuds
     */
    public final Color NODE_FILL = new Color(213, 240, 253);
    /**
     *Couleur d'un noeud avec l'attribut check_ a true_
     */
    public final Color CHECKED_FILL = Color.RED;
    /**
     *couleur du texte pour un elemenet selectionne
     */
    public final Color CHECKED_COLOR = Color.WHITE;
    /**
     *Couleur pour le texte d'un Noeud
     */
    public final Color NODE_COLOR = new Color(33, 150, 60);
    /**
     *Couleur de fond pour uen dependances
     */
    public final Color DEPENDENCY_FILL = new Color(202, 255, 176); //couleur de fond d'une dependance

    /**
     *Couleur du texte pour une dependance
     */
    public final Color DEPENDENCY_COLOR = Color.RED;
}
