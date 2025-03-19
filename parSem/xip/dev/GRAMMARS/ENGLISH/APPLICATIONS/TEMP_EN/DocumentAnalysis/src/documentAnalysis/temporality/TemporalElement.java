/*
 * TemporalElement.java
 *
 * Created on July 25, 2007, 10:37 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality;

import documentAnalysis.XipLink;
import java.util.Vector;

/**
 * Interface representing all temporal constituents that can
 *  be a node in the temporal graph
 * @author xtannier
 * @see documentAnalysis.temporality.TemporalGraph
 */
public interface TemporalElement extends XipLink {               
    
    
    /**
     * Returns a Vector containing temporal relations coming to or leaving the node.
     * @return a Vector containing temporal relations coming to or leaving the node.
     */
    public Vector<TemporalRelation> getTemporalRelations();
    
    /**
     * Returns a Vector containing temporal relations leaving the node
     * add having the specified relation identifier and with or without interval.
     * @param relation the int identifier of the excepted relation, or
     *  TemporalRelation.UNDEFINED if any relation identier is possible.
     * @param withInterval 1 if a non-null interval is expected,
     *  -1 if an interval is rejected, 0 if any relation is wanted.
     * @return a Vector containing temporal relations leaving the node and having the correct
     * attributes.
     */
    public Vector<TemporalRelation> getTemporalRelations(int relation, int withInterval); 
     
    
    
    /**
     * Adds a temporal relation to the graph.
     * @param relation the temporal relation to add to the graph
     */
    public void addTemporalRelation(TemporalRelation relation);
    
    
    /**
     * Replaces a relation by another in the element
     *
     * @param oldRelation the old relation
     * @param newRelation the new relation
     */
    public void replaceTemporalRelation(TemporalRelation oldRelation, TemporalRelation newRelation);

    
    
    /**
     * Tests whether the element is really defined.
     *
     * @return true if the element is really defined, false otherwise.
     */
    public boolean isDefined();    
    
    
}
