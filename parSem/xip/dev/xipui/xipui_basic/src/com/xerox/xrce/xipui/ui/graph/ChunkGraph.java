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
import javax.swing.*;
import java.awt.*;
import java.util.*;
import org.apache.log4j.*;
import edu.berkeley.guir.prefuse.*;
import edu.berkeley.guir.prefuse.action.*;
import edu.berkeley.guir.prefuse.action.assignment.*;
import edu.berkeley.guir.prefuse.action.filter.*;
import edu.berkeley.guir.prefuse.activity.*;
import edu.berkeley.guir.prefuse.graph.*;
import edu.berkeley.guir.prefuse.render.*;
import edu.berkeley.guir.prefuse.util.*;
import edu.berkeley.guir.prefusex.controls.*;
import edu.berkeley.guir.prefusex.layout.*;

/**
 * representation de l'arbre de chunk,
 * l'arbre est represente selon un VerticalTreeLayout
 * les noeuds du graphe possede des attributs, ces attributs sont accessibles via des cles
 * @author     Vianney Grassaud
 *
 */
public class ChunkGraph {

    private Logger log = Logger.getLogger("debug");
    protected int optimalWidth = 0;
    // registre des items de l'arbre de chunk
    protected ItemRegistry registry;
    //graph
    protected DefaultGraph graph;
    //composant contenant le graphe
    protected Display display;
    // table des xipnodes
    protected Hashtable tableNode;
    // table des ambiguites
    protected Hashtable tableAmbiguous;
    //liste d'action sur le display
    protected ActionList actions;
    //liste d'action sur le display
    protected ActionList filter;
    // representation du graphe sous forme d'arbre
    protected VerticalTreeLayout lay;
    protected NodeControl2 nodeControl;
    protected ZoomControl zoomControl;
    protected PanControl panControl;
    protected SubtreeDragControl subControl;
    protected ModuleContext context;
    protected int id;
    //------------ definition des constantes -----------
    /**
     *label pour un noeud
     */
    public static final String label_ = "label";
    /**
     *type d'un noeud
     */
    public static final String type_ = "type";
    /**
     *index d'un noeud
     */
    public static final String index_ = "index";
    /**
     *flag permettant de checker (selectionner) un noeud, valeur false ou true
     */
    public static final String check_ = "check";
    /**
     *valeur false
     */
    public static final String false_ = "false";
    /**
     *true
     */
    public static final String true_ = "true";
    /**
     *Type NODE
     */
    public static final String NODE = "NODE";
    /**
     *Type LEAF
     */
    public static final String LEAF = "LEAF";
    /**
     *Depth of the tree in order to change color
     */
    public static final int DEPTH = 4;

    /**
     * Represente l'arbre des chunk retourne par XIP.
     * cette representation est interactive, c a d que son graphe est cliquable avec une souris
     * @param properties
     * @param id 
     */
    public ChunkGraph(ModuleContext properties, int id) {
        this.id = id;
        context = properties;
        //layout to use
        lay = new VerticalTreeLayout();
        // global controls on the display
        zoomControl = new ZoomControl(true);
        panControl = new PanControl(true);
        subControl = new SubtreeDragControl(true);
        nodeControl = new NodeControl2(context, id);
    }

    /**
     *This method is static and return a subchunkgraph of a XipNode
     *@param context_ 
     *@param index
     *@param id 
     *@return the display of the SubChunkGraph
     *@see com.xerox.xrce.lingdexip.run.graph.SubChunkGraph#initSubGraph(XipNode,Vector)
     */
    public static JComponent getSubChunkGraph(ModuleContext context_, long index, int id) {
        SessionGraph session = context_.getSessionGraph(id);
        SubChunkGraph subChunk = new SubChunkGraph(context_, id);
        XipNode node = session.getXipNode(index);
        if (node != null) {
            subChunk.initSubGraph(node, session.getVectorAmbiguous(index));
            return subChunk.getGraph();
        }
        return null;
    }

    /**
     *This method create the activities in order to layout the
     *graph.
     */
    protected void createActivity() {
        actions = new ActionList(registry);
        filter = new ActionList(registry);
        filter.add(new TreeFilter());
        filter.add(lay);
        actions.add(new DemoColorFunction(DEPTH));
        actions.add(new RepaintAction());
    }

    /**
     * Initialise the rendered for the graph
     *
     */
    protected void initRender() {
        TextImageItemRenderer nodeRenderer = new TextImageItemRenderer();
        //TextItemRenderer nodeRenderer = new TextItemRenderer();
        //nodeRenderer.setShowImages(true);
        nodeRenderer.setImageAttributeName("icon");
        //nodeRenderer.setRenderType(TextItemRenderer.RENDER_TYPE_DRAW_AND_FILL);
        nodeRenderer.setRenderType(TextItemRenderer.RENDER_TYPE_FILL);
        nodeRenderer.setRoundedCorner(8, 8);
        DefaultEdgeRenderer edgeRenderer = new DefaultEdgeRenderer();
        edgeRenderer.setEdgeType(DefaultEdgeRenderer.EDGE_TYPE_LINE);
        edgeRenderer.setWeightType(DefaultEdgeRenderer.WEIGHT_TYPE_NONE);
        edgeRenderer.setHorizontalAlignment1(DefaultEdgeRenderer.ALIGNMENT_CENTER);
        edgeRenderer.setHorizontalAlignment2(DefaultEdgeRenderer.ALIGNMENT_CENTER);
        edgeRenderer.setVerticalAlignment1(DefaultEdgeRenderer.ALIGNMENT_CENTER);
        edgeRenderer.setVerticalAlignment2(DefaultEdgeRenderer.ALIGNMENT_CENTER);
        registry.setRendererFactory(new DefaultRendererFactory(nodeRenderer, edgeRenderer, null));
    }

    /**
     *Launch the activities to paint the graph
     *NB : sometimes we have problems with the threads.
     *@see edu.berkeley.guir.prefuse.activity.ActionList#runNow()
     */
    public void run() {
        lay.setLayoutBounds(new Rectangle(display.getSize()));
        lay.setLayoutAnchor(new Point(display.getWidth() / 2, 10));
        filter.runNow();
        actions.runNow();
    }

    public void setColor() {
        if (!actions.isRunning()) {
            actions.runNow();
        }
    }

    /**
     *Repaint the registry and the display
     *
     *@see edu.berkeley.guir.prefuse.Display#repaintImmediate()
     */
    public void repaint() {
        //display.validate();
        display.repaintImmediate();
    //registry.repaint();  
    }

    /**
     * Initialize the graph from the xipresult
     * We use only the first XipUnit in the XipResult
     * @param xipResult_ 
     * @return  the root node of the chunk graph
     * @see com.xerox.jxip.XipResult.getSentences()
     */
    public DefaultTreeNode init(XipResult xipResult_) {
        //we get the registry
        SessionGraph session = context.getSessionGraph(false, id);
        this.registry = session.getChunkRegistry();
        this.graph = (DefaultGraph) registry.getGraph();
        this.tableNode = new Hashtable();
        this.tableAmbiguous = new Hashtable();
        initRender();
        createActivity();
        session.setProperty(session.ACTIONS_CHUNK, actions);
        session.setTableNode(tableNode);
        session.setTableAmbiguous(tableAmbiguous);
        initDisplay();
        DefaultTreeNode root = null;
        display.setSize(0, 0);
        if (xipResult_.getXipUnits() != null && !xipResult_.getXipUnits().isEmpty()) {
            this.optimalWidth = 0;
            root = constructFull2((xipResult_.getXipUnits().get(0)).getRoot());
        }
        setSize();
        return root;
    }

    /**
     *Setup the display with the registry and add the control
     *if the registry is null then the display will not be instanciate
     *in the this method
     */
    protected void initDisplay() {
        if (registry != null) {
            display = new Display(registry);
            //display.setHighQuality(true);
            display.setBackground(Color.WHITE);
            display.addControlListener(nodeControl);
            display.addControlListener(subControl);
            display.addControlListener(panControl);
            display.addControlListener(zoomControl);
        }
    }

    /**
     *Return the graph ie the display containing visualItems
     *@return  a Graph like a JComponent
     */
    public JComponent getGraph() {
        return display;
    }

    /**
     *Reads the xipNode and create the tree graph using DefaultTreeNode
     *some attribute are added to the node :
     *type : node, leaf, dependency
     *label : the label printed on the graph
     *check : true or false, if the node is selected or not
     *icon : an image
     *index : the index of the node
     *fictif : true or false if the node is fictif
     *same : if the left and right of a node are equal to another node
     *ambiguous : if two nodes at the same layer have the same surface but not the same category
     * @param root
     * @return 
     */
    protected DefaultTreeNode constructFull2(XipNode root) {
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
            String category = null;
            String surface = null;
            long right = -1;
            long left = -1;
            boolean sameLeaf = false;
            boolean sameNode = false;
            boolean leafCat = false;
            int size = root.getDaughters().size();
            for (int i = 0; i < size; i++) {
                XipNode dght = root.getDaughters().get(i);
                if (i == 0) {
                    category = dght.getCategory();
                    surface = dght.getSurface();
                    right = dght.getRight();
                    left = dght.getLeft();
                }
                String currentCat = dght.getCategory();
                String currentSur = dght.getSurface();
                long currentRight = dght.getRight();
                long currentLeft = dght.getLeft();
                if (i != 0) {
                    if (currentRight == right && currentLeft == left) {
                        node.setAttribute("icon", XIPUIUtilities.getIconString(UIConstants.INFO));
                        node.setAttribute(label_, root.toString() + "#" + root.getIndex());
                        node.setAttribute("same", ChunkGraph.true_);
                        XipNode nodePrev = (root.getDaughters().get(i - 1));
                        addAmbiguous(root.getIndex(), nodePrev.getIndex());
                        addAmbiguous(root.getIndex(), dght.getIndex());
                        sameNode = true;

                    }
                    if (surface.equalsIgnoreCase(currentSur) && !currentCat.equalsIgnoreCase(category)) {
                        node.setAttribute("icon", XIPUIUtilities.getIconString(UIConstants.AMBIGOUS));
                        node.setAttribute(label_, root.toString() + "#" + root.getIndex() + " ?");
                        node.setAttribute("ambiguous", ChunkGraph.true_);
                        sameLeaf = true;
                    }
                }
                DefaultNode node2 = constructFull2(dght);
                if (!sameLeaf && !sameNode && !leafCat) {
                    DefaultEdge edge = new DefaultEdge(node, node2);
                    node.addChild(edge);
                    graph.addEdge(edge);
                }
                category = currentCat;
                surface = currentSur;
                right = currentRight;
                left = currentLeft;
                sameLeaf = false;
                sameNode = false;
            }
            return node;
        } else {
            DefaultTreeNode node = new DefaultTreeNode();
            long indexM = root.getMother().getIndex();
            if (tableAmbiguous.get(indexM) != null) {
                node.setAttribute("ambiguous", "true");
            } else {
                if (root.getLeft() == -100 && root.getRight() == -10) { //le noeud est fictif...

                    node.setAttribute("icon", XIPUIUtilities.getIconString(UIConstants.FICTIF));
                    node.setAttribute("fictif", true_);
                } else {
                    node.setAttribute("fictif", false_);
                    this.optimalWidth += (root.toString().length());
                }
            }
            node.setAttribute(index_, String.valueOf(root.getIndex()));
            node.setAttribute(label_, root.toString());
            node.setAttribute(type_, LEAF);
            node.setAttribute(check_, false_);
            tableNode.put(root.getIndex(), root);
            graph.addNode(node);
            return node;
        }
    }

    /**
     *Add an ambiguous node in a table of XipNode. This table
     *contains only XipNode who are ambiguous.
     *This table is like that :
     * key (indexMother) values : Vector of index
     *So you need to use the table of XipNode in order to use this table
     *@param indexMother the node who has a ambiguity
     *@param index the index of a xipnode who is ambiguous with the mother
     */
    protected void addAmbiguous(long indexMother, long index) {
        Vector w = (Vector) this.tableAmbiguous.get(indexMother);
        if (w == null) {
            Vector v = new Vector();
            v.add(index);
            tableAmbiguous.put(indexMother, v);
        } else {
            if (!w.contains(index)) {
                w.add(index);
            }
        }
    }

    /**
     *Setup the size of the display and repaint the chunk graph.
     *
     */
    public void setSize() {
        //display.setSize(new Dimension( ((optimalWidth * 12)+ tableNode.size()) *2 ,150));
        //  System.out.println("taille graph" + new Dimension((optimalWidth * 15)+ tableAmbiguous.size() *15,150));
        display.setSize(new Dimension((optimalWidth * 17) + tableAmbiguous.size() * 17, 150));
        display.setBounds(new Rectangle(new Dimension((optimalWidth * 17) + tableAmbiguous.size() * 17, 150)));
        repaint();
    }
//    /**
//     * This method return the optimal size for the graph.
//     * The calcul is made as  :
//     * for each nodes we know the size in characters.
//     * We get all the leaves and add each size.
//     * Then we apply a policy factor.
//     * (total size leaves feuilles  * size_policy) + leaves number
//     *
//     * @return  the "optimal" width
//     */
//    public int widthCalcul() {
//        int cpt = 0;
//        int numNode = 0;
//        Iterator iter = graph.getNodes();
//        while (iter.hasNext()) {
//            numNode++;
//            Node node = (Node) iter.next();
//            String isSame = node.getAttribute("same");
//            String isAmbiguous = node.getAttribute("ambiguous");
//            String type = node.getAttribute(type_);
//            String icon = node.getAttribute("icon");
//
//            if ( type != null && type.equals(LEAF) && isSame==null && isAmbiguous == null) {
//                String dimension = node.getAttribute(dimension_);
//                cpt += Integer.parseInt(dimension);
//            }
//            if ( type.equals(NODE) && icon != null) {
//                cpt += 5;
//              }
//        }
//        //System.out.println("LARGEUR de l'ARBRE : " + (cpt *12 + numNode));
//        return cpt *12 + numNode ;
//    }

    /**
     * This class is usefull to color the graph
     *
     */
    static public class DemoColorFunction extends ColorFunction {

        /**
         *Table de couleur
         */
        private ColorMap colorMap;
        //--------------- definition des constantes pour les couleurs
        /**
         *Couleur des aretes
         */
        private Color graphEdgeColor = ConstantsGraph.EDGE_COLOR;
        /**
         *Couleur d'highlight
         */
        private Color highlightColor1 = ConstantsGraph.HILGHLIGHT_COLOR;
        /**
         *Couleur des feuilles
         */
        private Color leafColor = ConstantsGraph.LEAF_COLOR;
        /**
         *Couleur de fond pour les noeuds
         */
        private Color nodeFillColor = ConstantsGraph.NODE_FILL;
        /**
         *Couleur d'un noeud avec l'attribut check_ a true_
         */
        private Color checkedColor = ConstantsGraph.CHECKED_FILL;

        /**
         *Constructeur
         *@param  thresh  profondeur des couleurs pour les aretes
         *@see edu.berkeley.guir.prefuse.action.assignment.ColorFunction
         */
        public DemoColorFunction(int thresh) {
            super();
            colorMap = new ColorMap(ColorMap.getInterpolatedMap(thresh + 1, new Color(33, 150, 60), Color.RED), 0, thresh);
        }

        /**
         * Return the color to print for an item
         * The method has an effect nonly if the item is a NodeItem or a EdgeItem
         * @param  item  item in the registry
         * @return color for the fill
         *
         * @see edu.berkeley.guir.prefuse.VisualItem
         * @see edu.berkeley.guir.prefuse.action.assignment.ColorFunction
         */
        public Paint getFillColor(VisualItem item) {

            if (item instanceof NodeItem) {
                boolean checked = item.getAttribute(check_).equals(true_);
                boolean isLeaf = item.getAttribute(type_).equals(LEAF);

                if (checked) {
                    return checkedColor;
                }
                if (isLeaf) {
                    return leafColor;
                }
            }
            if (item instanceof EdgeItem) {
                return getColor(item);
            }
            return nodeFillColor;
        }

        /**
         * Return the color of the text of an item
         * @param  item  item in the registry
         * @return  foreground color
         *
         * If the item is a EdgeItem we used the colorMap
         * @see edu.berkeley.guir.prefuse.VisualItem
         * @see edu.berkeley.guir.prefuse.action.assignment.ColorFunction
         */
        public Paint getColor(VisualItem item) {
            if (item.isHighlighted()) {
                return highlightColor1;
            } else if (item instanceof NodeItem) {
                boolean checked = item.getAttribute(check_).equals(true_);
                if (checked) {
                    return Color.WHITE;
                }
                int d = ((NodeItem) item).getDepth();
                return colorMap.getColor(d);
            } else if (item instanceof EdgeItem) {
                EdgeItem e = (EdgeItem) item;
                if (e.isTreeEdge()) {
                    int d;
                    int d1;
                    int d2;
                    d1 = ((NodeItem) e.getFirstNode()).getDepth();
                    d2 = ((NodeItem) e.getSecondNode()).getDepth();
                    d = Math.max(d1, d2);
                    return colorMap.getColor(d);
                } else {
                    return graphEdgeColor;
                }
            } else {
                return Color.BLACK;
            }
        }
    }
}

