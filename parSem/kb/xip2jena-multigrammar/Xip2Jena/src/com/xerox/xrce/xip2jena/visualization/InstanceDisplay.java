/*
 * Copyright Xerox 2009
 */

package com.xerox.xrce.xip2jena.visualization;

import com.hp.hpl.jena.ontology.Individual;
import com.hp.hpl.jena.ontology.OntClass;
import com.hp.hpl.jena.rdf.model.StmtIterator;
import java.util.NoSuchElementException;
import prefuse.data.Graph;
import com.hp.hpl.jena.ontology.OntModel;
import com.hp.hpl.jena.rdf.model.RDFNode;
import com.hp.hpl.jena.rdf.model.Statement;
import com.hp.hpl.jena.util.iterator.ExtendedIterator;
import java.awt.event.MouseEvent;
import java.awt.geom.Rectangle2D;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import prefuse.Display;
import prefuse.Visualization;
import prefuse.action.ActionList;
import prefuse.action.RepaintAction;
import prefuse.action.assignment.ColorAction;
import prefuse.action.layout.Layout;
import prefuse.action.layout.graph.ForceDirectedLayout;
import prefuse.controls.ControlAdapter;
import prefuse.controls.DragControl;
import prefuse.controls.PanControl;
import prefuse.controls.ZoomControl;
import prefuse.controls.ZoomToFitControl;
import prefuse.data.Edge;
import prefuse.data.Node;
import prefuse.data.Schema;
import prefuse.render.DefaultRendererFactory;
import prefuse.render.EdgeRenderer;
import prefuse.render.LabelRenderer;
import prefuse.render.Renderer;
import prefuse.util.ColorLib;
import prefuse.util.PrefuseLib;
import prefuse.util.force.DragForce;
import prefuse.util.force.ForceSimulator;
import prefuse.util.force.NBodyForce;
import prefuse.util.force.SpringForce;
import prefuse.visual.DecoratorItem;
import prefuse.visual.VisualItem;
import prefuse.visual.expression.InGroupPredicate;


/**
 *
 * @author akaplan
 */
public class InstanceDisplay extends Display {

    // prefuse columns
    private static final String C_LABEL = "label";
    public static final String C_INDIV = "indiv";
    
    // prefuse groups
    private static final String G_GRAPH = "graph";
    private static final String G_NODES = G_GRAPH + ".nodes";
    private static final String G_EDGES = G_GRAPH + ".edges";
    public static final String NODE_DECORATORS = "nodeDeco";
    public static final String EDGE_DECORATORS = "edgeDeco";

    // prefuse actions
    private static final String A_COLOR = "color";
    private static final String A_LAYOUT = "layout";
    private static final String A_EDGE_LABELS = "edgeLabels";

    // I tried defining a union class, but it seems that union classes
    // aren't supported by the RDFS reasoner.  So here's a procedural definition.
    private boolean interestingClass(Individual i, OntClass[] excludeClasses) {
        for (OntClass oc : excludeClasses) {
            if (i.hasOntClass(oc)) {
                return false;
            }
        }
        return true;
    }

    // make this externally accessible in order to be able to control it with
    // a JForcePanel.
    private ForceSimulator forceSimulator;
    public ForceSimulator getForceSimulator() { return forceSimulator; }

    private Node getNode(Individual i, Map<Individual,Node> map, Graph g) {
        Node n = map.get(i);
        if (n == null) {
            n = makeNode(g, i);
            map.put(i, n);
        }
        return n;
    }
    private Node makeNode(Graph g, Individual i) {
        Node n = g.addNode();
        n.set(C_INDIV, i);
        return n;
    }

    private static final Schema DECORATOR_SCHEMA = PrefuseLib.getVisualItemSchema();
    static {
    	DECORATOR_SCHEMA.setDefault(VisualItem.INTERACTIVE, false);
    	DECORATOR_SCHEMA.setDefault(VisualItem.TEXTCOLOR, ColorLib.gray(0));
    }

    private SelectCallback selectCallback;

    public InstanceDisplay(OntModel m, OntClass[] excludeClasses,
            SelectCallback selectCallback) {
        this.selectCallback = selectCallback;
        
        Graph g = jena2prefuse(m, excludeClasses);
        Visualization vis = new Visualization();
        vis.add(G_GRAPH, g);

        Renderer nodeLabelR = new IndividualRenderer();
        Renderer edgeR = new EdgeRenderer();
        Renderer edgeLabelR = new LabelRenderer();
        DefaultRendererFactory drf = new DefaultRendererFactory(nodeLabelR, edgeR);
        drf.add(new InGroupPredicate(EDGE_DECORATORS), edgeLabelR);
        vis.setRendererFactory(drf);
        vis.addDecorators(EDGE_DECORATORS, G_EDGES, DECORATOR_SCHEMA);

        ActionList color = new ActionList();
        color.add(new ColorAction(G_NODES, VisualItem.FILLCOLOR, ColorLib.rgba(0,0,255,20)));
        color.add(new ColorAction(G_NODES, VisualItem.STROKECOLOR, ColorLib.gray(0)));
        color.add(new ColorAction(G_NODES, VisualItem.TEXTCOLOR, ColorLib.gray(0)));
        color.add(new ColorAction(G_EDGES, VisualItem.FILLCOLOR, ColorLib.gray(0)));
        color.add(new ColorAction(G_EDGES, VisualItem.STROKECOLOR, ColorLib.gray(0)));
        color.add(new ColorAction(G_EDGES, VisualItem.TEXTCOLOR, ColorLib.gray(0)));
        vis.putAction(A_COLOR, color);

        ActionList layout = new ActionList();
        forceSimulator = new ForceSimulator();
        // gravconstant (-1.0f), min dist (-1f), theta (0.9f)
        forceSimulator.addForce(new NBodyForce(-1f, -1f, 0.9f));
        // default spring coef (1e-4f), default length (50)
        forceSimulator.addForce(new SpringForce(1E-5f, 100));
        // drag coef (.01)
        forceSimulator.addForce(new DragForce(.01f));
        ForceDirectedLayout fdl = new ForceDirectedLayout(G_GRAPH, forceSimulator, false, true);
        fdl.setIterations(1000); // default 100
        layout.add(fdl);
        layout.add(new RepaintAction());
        vis.putAction(A_LAYOUT, layout);

        ActionList layoutEdgeLabels = new ActionList();
        layoutEdgeLabels.add(new LabelLayout(EDGE_DECORATORS));
        layoutEdgeLabels.add(new RepaintAction());
        vis.putAction(A_EDGE_LABELS, layoutEdgeLabels);

        setVisualization(vis);
        setSize(600, 600);
        addControlListener(new DragControl(A_EDGE_LABELS)); // drag items around
        addControlListener(new PanControl());  // pan with background left-drag
        addControlListener(new ZoomControl()); // zoom with vertical right-drag
        addControlListener(new ZoomToFitControl());
        addControlListener(new SelectControl());

        vis.run(A_COLOR);
        vis.run(A_LAYOUT);
        vis.run(A_EDGE_LABELS);

    }

// For the tree layouts, prefuse can't handle a graph that's not fully connected, so
    // we'd need to identify connected components.  Here's a start--not quite right.
    // And I haven't figured out how to display multiple trees in the same frame.
//    private Collection<Graph> getConnectedGraphs(OntModel m) {
//        Collection<Graph> result = new ArrayList<Graph>();
//        ExtendedIterator indivIt = m.listIndividuals();
//        LinkedList<Individual> q = new LinkedList<Individual>();
//        Map<Individual,Node> indiv2Node = new HashMap<Individual,Node>();
//        while (indivIt.hasNext()) {
//            Individual rooti = (Individual) indivIt.next();
//            if (indiv2Node.containsKey(rooti)) continue;
//            Graph g = makeGraph();
//            result.add(g);
//            Node rootn = makeNode(g, rooti);
//            indiv2Node.put(rooti,rootn);
//            q.add(rooti);
//            while (!q.isEmpty()) {
//                Individual si = q.removeFirst();
//                Node sn = indiv2Node.get(si);
//                StmtIterator stmts = si.listProperties();
//                while (stmts.hasNext()) {
//                    Statement stmt = stmts.nextStatement();
//                    RDFNode or = stmt.getObject();
//                    if (or.canAs(Individual.class)) {
//                        Individual oi = (Individual) or.as(Individual.class);
//                        Node on = indiv2Node.get(oi);
//                        if (on == null) {
//                            on = makeNode(g, oi);
//                            indiv2Node.put(oi, on);
//                            q.add(oi);
//                        }
//                        if (on.getGraph() == g) {
//                            g.addEdge(sn, on);
//                        }
//                    }
//                }
//            }
//        }
//        return result;
//    }

    private Graph jena2prefuse(OntModel m, OntClass[] excludeClasses) throws NoSuchElementException {
        Graph g = makeGraph();
        Map<Individual, Node> indiv2node = new HashMap<Individual, Node>();
        ExtendedIterator iIt = m.listIndividuals();
        while (iIt.hasNext()) {
            Individual si = (Individual) iIt.next();
            if (interestingClass(si, excludeClasses)) {
                Node sn = getNode(si, indiv2node, g);
                StmtIterator props = si.listProperties();
                while (props.hasNext()) {
                    Statement stmt = props.nextStatement();
                    RDFNode or = stmt.getObject();
                    if (or.canAs(Individual.class) && !or.canAs(OntClass.class)) {
                        // In Jena 2.5.7 OntClasses return true for
                        // canAs(Individual.class).  Seems like a Jena bug.
                        Individual oi = (Individual) or.as(Individual.class);
                        if (interestingClass(oi, excludeClasses)) {
                            Node on = getNode(oi, indiv2node, g);
                            Edge e = g.getEdge(sn, on);
                            String label = "";
                            if (e == null) {
                                e = g.addEdge(sn, on);
                            } else {
                                label = e.getString(C_LABEL) + ",\n";
                            }
                            e.set(C_LABEL, label + stmt.getPredicate().getLocalName());
                            // todo: eliminate redundant (often inferred) edge labels like
                            // hasName + hasPersonName.  Perhaps instead of building
                            // the label on the fly, build a list of Propertys on
                            // the fly, and then at the end convert the whole
                            // list to a label in one go.
                        }
                    }
                }
            }
        }
        return g;
    }

    private Graph makeGraph() {
        Graph g = new Graph(true);
        g.addColumn(C_LABEL, String.class);
        g.addColumn(C_INDIV, Individual.class);
        return g;
    }

    class SelectControl extends ControlAdapter {

        @Override
        public void itemClicked(VisualItem item, MouseEvent e) {
            if (item.canGet(InstanceDisplay.C_INDIV, Individual.class)) {
                Individual i = (Individual) item.get(InstanceDisplay.C_INDIV);
                selectCallback.select(i);
            }
        }
    }

}


class LabelLayout extends Layout {

    public LabelLayout(String group) {
        super(group);
    }

    public void run(double frac) {
        Iterator iter = m_vis.items(m_group);
        while (iter.hasNext()) {
            DecoratorItem decorator = (DecoratorItem) iter.next();
            VisualItem decoratedItem = decorator.getDecoratedItem();
            Rectangle2D bounds = decoratedItem.getBounds();

            double x = bounds.getCenterX();
            double y = bounds.getCenterY();

            /* modification to move edge labels more to the arrow head
            double x2 = 0, y2 = 0;
            if (decoratedItem instanceof EdgeItem){
            VisualItem dest = ((EdgeItem)decoratedItem).getTargetItem();
            x2 = dest.getX();
            y2 = dest.getY();
            x = (x + x2) / 2;
            y = (y + y2) / 2;
            }
             */

            setX(decorator, null, x);
            setY(decorator, null, y);
        }
    }
}

