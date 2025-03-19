/*
 * Event.java
 *
 * Created on December 11, 2006, 3:17 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.events;

import com.xerox.jxip.*;
import documentAnalysis.Element;
import documentAnalysis.temporality.TemporalConstituent;
import documentAnalysis.temporality.TemporalElement;
import documentAnalysis.temporality.TemporalRelation;
import documentAnalysis.temporality.vaguecalendar.duration.Duration;
import java.util.Vector;

/**
 * Class representing an Event.
 * Implements documentAnalysis.temporality.TemporalElement because it can be in a temporal graph.
 * @author xtannier
 */
public class Event implements TemporalElement {
    
    private EventCore core;
    private Vector<TemporalRelation> temporalRelations;
    private boolean speechTime;
    private boolean iterative;
    private Duration duration;
    private String comment;
    
    
    /**
     * Builds an undefined event
     */
    public Event(){
        super();
        this.core = new EventCore();
        this.speechTime = false;
        this.duration = null;
        this.iterative = false;
        this.temporalRelations = new Vector<TemporalRelation>();
    }
    
    /**
     * Builds an undefined Event with a comment
     */
    public Event(String comment) {
        this();
        this.core = new EventCore();
        this.comment = comment;
    }
    
    
    /**
     * Builds an undefined event
     * @param speechTime - true if this is the speech time of the document, false otherwise
     */
    public Event(boolean speechTime){
        this();
        this.core = new EventCore();
        this.speechTime = speechTime;
    }
    
    /**
     * Builds an event based on the specified XipNode
     * @param unit the associated XipUnit
     * @param node the associated XipNode
     */
    public Event(XipUnit unit, XipNode node){
        super();
        this.core = new EventCore(unit, node);
        this.duration = null;
        this.iterative = this.getXipNode() != null && this.getXipNode().hasFeature("e_iteration", "+");
        this.temporalRelations = new Vector<TemporalRelation>();
    }
    
    
    /**
     * Gets the category of the associated XipNode.
     * @return the category of the associated XipNode, null if no XipNode associated.
     */
    public String getCategory() {
        return this.core.getCategory();
    }
    
    
    /**
     * Returns the universal id of the current element (XipNode UID).
     * @return the uid the XipNode associated with the current element, null if no XipNode.
     */
    public String getUid() {
        return this.core.getUid();
    }    
    
    
   /**
     * Gets the number of the sentence where the element has been found.
     * @return the number of the sentence where the element has been found.
     */
    public long getSentenceNumber(){
        return this.core.getSentenceNumber();
    }    
    
    
    /**
     * Associates a XipNode to the current element.
     * @param unit the XipUnit in which the element is found.
     * @param node the XipNode to associate to the current element.
     */
    public void setXipNode(XipUnit unit, XipNode node){
        this.core.setXipNode(unit, node);
    }    
    
    /**
     * Returns the XipUnit associated with the current element, null if no XipUnit.
     * @return the XipUnit associated with the current element, null if no XipUnit.
     */
    public XipUnit getXipUnit(){
        return this.core.getXipUnit();
    }
    
    /**
     * Returns the XipNode associated with the current element, null if no XipNode.
     * @return the XipNode associated with the current element, null if no XipNode.
     */
    public XipNode getXipNode(){
        return this.core.getXipNode();
    }
    
    
    /**
     * Tests whether the current event is the speech time of the document
     * @return true if the event is the speech time, false otherwise
     */
    public boolean isSpeechTime() {
        return speechTime;
    }
    
    
    /**
     * Tests whether the current event is a iteration event
     * @return true if the event is iterative, false otherwise.
     */
    public boolean isIterative() {
        return this.iterative;
    }
    
    /**
     * Tests whether the element is really defined.
     * An event, if built, is necessary defined.
     *
     * @return true if the element is really defined, false otherwise.
     */
    public boolean isDefined(){
        return true;
    }
    
    
    /**
     * Returns a Vector containing temporal relations coming to or leaving the node.
     * @return a Vector containing temporal relations coming to or leaving the node.
     */
    public Vector<TemporalRelation> getTemporalRelations() {
        return this.temporalRelations;
    }
    
    
    /**
     * Returns a Vector containing temporal relations leaving the node
     * add having the specified relation identifier and with or without interval.
     * @param relation the int identifier of the excepted relation, or
     *  TemporalRelation.UNDEFINED if any relation identier is possible.
     * @param withInterval 1 if a non-null interval is expected,
     *  -1 if an interval is rejected, 0 if any relation is wanted.
     * @return a Vector containing temporal relations leaving the node and having the correct
     * attributes.
     * @see TemporalConstituent#getTemporalRelations(TemporalElement temporalElement,int relation,int withInterval)
     */
    public Vector<TemporalRelation> getTemporalRelations(int relation, int withInterval) {
        return TemporalConstituent.getTemporalRelations(this, relation, withInterval);
    }
    
    
    
    /**
     * Adds a temporal relation to the temporal constituent.
     * @param relation the temporal relation to add to the graph
     * @see TemporalConstituent#addTemporalRelation(TemporalRelation relation)
     */
    public void addTemporalRelation(TemporalRelation relation){
        TemporalConstituent.addTemporalRelation(this, relation);
    }
    
    
    /**
     * Replaces a relation by another in the element
     *
     * @param oldRelation the old relation
     * @param newRelation the new relation
     * @see TemporalConstituent#replaceTemporalRelation(TemporalElement temporalElement,
     * TemporalRelation oldRelation,
     * TemporalRelation newRelation)
     */
    public void replaceTemporalRelation(TemporalRelation oldRelation,
            TemporalRelation newRelation) {
        TemporalConstituent.replaceTemporalRelation(this, oldRelation, newRelation);
    }
    
    
    
    
    /**
     * Tests equality
     * @param o the object to compoare
     * @return true whether the object equals the current event, false otherwise
     */
    public boolean equals(Object o){
        if (o instanceof Event){
            Event e = (Event)o;
            return (this.speechTime && e.speechTime) ||
                    (this.core.equals(e.core) && super.equals(e));
        } else {
            return false;
        }
    }
    
    /**
     * Returns a String representation of the event.
     * @return a String representation of the event.
     */
    public String toString(){
        String result = "";
        if (this.getXipNode() != null)
            result = this.getXipNode().getSurface();
        else if (this.speechTime)
            result = "ST";
        else if (this.getComment() != null)
            result = this.getComment();
        else
            result = "SOME EVENT";
        return result;
    }
    
    /**
     * Returns the Duration of the event
     * @return the Duration of the event
     */
    public Duration getDuration() {
        return duration;
    }
    
    /**
     * Sets the Duration of the event
     * @param duration the Duration of the event
     */
    public void setDuration(Duration duration) {
        this.duration = duration;
    }
    
    /**
     * Tests if the event has a defined Duration.
     * @return true whether the event has a defined Duration, false otherwise
     */
    public boolean hasDuration(){
        return this.duration != null && this.duration.isDefined();
    }
    
    /**
     * Returns the comment
     * @return the comment
     */
    public String getComment() {
        return comment;
    }
}
