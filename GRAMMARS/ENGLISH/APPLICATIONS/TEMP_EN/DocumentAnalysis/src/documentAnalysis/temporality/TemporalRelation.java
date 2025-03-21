/*
 * TemporalRelation.java
 *
 * Created on December 11, 2006, 2:37 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality;

import common.NotImplementedException;
import documentAnalysis.events.Event;
import documentAnalysis.temporality.vaguecalendar.BadFormatException;
import documentAnalysis.temporality.vaguecalendar.date.VagueDate;
import documentAnalysis.temporality.vaguecalendar.date.VagueMultipleDate;
import documentAnalysis.temporality.vaguecalendar.duration.Duration;
import java.util.Vector;

/**
 * Class representation a temporal relation between two TemporalElements
 * @author xtannier
 * @see TemporalElement
 */
public class TemporalRelation implements java.lang.Cloneable{
    
    /* *******************
     * temporal relations
     ********************/
    /** undefined */
    public static int UNDEFINED = -100;
    /** vague */
    public static int VAGUE = -99;
    /** equals, same events */
    public static int EQUALS = 1;
    /** after AND starts_with*/
    public static int AFTER = 2;
//    public static int JUST_AFTER = 2;
//    public static int STARTS_WITH = 2;
    /** before AND ends_with*/
    public static int BEFORE = 4;
//    public static int ENDS_WITH = 4;
    /** overlaps */
    public static int OVERLAPS = 8;
    /** is overlaped */
    public static int OVERLAPPED_BY = 16;
    /** during */
    public static int DURING = 32;
    /** includes */
    public static int INCLUDES = 64;
    
    /* ****************
     * Priority values.
     * The lower the number, the higher the priority
     ******************/
    /** Certain relation */
    public static int CERTAIN = -100;
    /** When the relation comes from a comparison between dates */
    public static int CALENDAR = -15;
    /** When the relation comes from a temporal expression linking */
    public static int TEMPORAL_EXPRESSION = -10;
    /** When the relation comes from an ordering decided by the grammar */
    public static int GRAMMAR1 = -8;
    /** When the relation comes from an ordering decided by the grammar */
    public static int GRAMMAR2 = -7;
    /** When the relation comes from a verb tense */
    public static int TENSE = -5;
    /** When the relation comes from relation composition */
    public static int COMPOSITION = -1;
    /** When the relation comes from a default last-moment desperate choice */
    public static int DEFAULT_CHOICE = 8;
    /** When the relation comes from random */
    public static int RANDOM = 10;
    
    
    private TemporalElement e1;
    private TemporalElement e2;
    private int relation;
    private int priority;
    private String comment;
    private Duration interval;
    private boolean intervalEnabled;
    private boolean enabled;
//    private TemporalGraph temporalGraph;
    
    private static int test = 0;
    
    /**
     * Creates a new instance of TemporalRelation
     * @param e1 the first parameter of the relation
     * @param e2 the second parameter of the relation
     * @param relation the relation holding between both elements
     */
    public TemporalRelation(TemporalElement e1, TemporalElement e2, int relation) {
        if (e1 == null || e2 == null)
            throw new NullPointerException();
        if (relation == 0)
            throw new NullPointerException();
        this.e1 = e1;
        this.e2 = e2;
        this.setRelation(relation);
        this.setComment("");
        this.setInterval(Duration.getNewDuration());
        this.setPriority(0);
        this.intervalEnabled = true;
        this.enabled = true;
//        this.temporalGraph = null;
    }
    
    
    /**
     * Creates a new instance of TemporalRelation
     * @param e1 the first parameter of the relation
     * @param e2 the second parameter of the relation
     * @param relation the relation holding between both elements
     * @param priority the priority level of the relation
     */
    public TemporalRelation(TemporalElement e1, TemporalElement e2, int relation,
            int priority) {
        this(e1, e2, relation);
        this.setPriority(priority);
    }
    
    /**
     * Creates a new instance of TemporalRelation
     * @param e1 the first parameter of the relation
     * @param e2 the second parameter of the relation
     * @param relation the relation holding between both elements
     * @param duration the interval of time holding between both elements
     */
    public TemporalRelation(TemporalElement e1, TemporalElement e2, int relation,
            Duration duration) {
        this(e1, e2, relation);
        this.setInterval(duration);
    }
    
    
    /**
     * Creates a new instance of TemporalRelation
     * @param e1 the first parameter of the relation
     * @param e2 the second parameter of the relation
     * @param relation the relation holding between both elements
     * @param priority the priority level of the relation
     * @param duration the interval of time holding between both elements
     */
    public TemporalRelation(TemporalElement e1, TemporalElement e2, int relation,
            Duration duration, int priority) {
        this(e1, e2, relation, priority);
        this.setInterval(duration);
    }
    
    
    /**
     * Creates a new instance of TemporalRelation
     * @param e1 the first parameter of the relation
     * @param e2 the second parameter of the relation
     * @param relation the relation holding between both elements
     * @param priority the priority level of the relation
     * @param comment  how this relation was found
     */
    public TemporalRelation(TemporalElement e1, TemporalElement e2, int relation,
            int priority, String comment) {
        this(e1, e2, relation, priority);
        this.setComment(comment);
    }
    
    
    /**
     * Creates a new instance of TemporalRelation
     * @param e1  the first parameter of the relation
     * @param e2  the second parameter of the relation
     * @param relation  the relation holding between both elements
     * @param comment   how this relation was found
     */
    public TemporalRelation(TemporalElement e1, TemporalElement e2, int relation,
            String comment) {
        this(e1, e2, relation);
        this.setComment(comment);
    }
    
    /**
     * Creates a new instance of TemporalRelation
     * @param e1  the first parameter of the relation
     * @param e2  the second parameter of the relation
     * @param relation  the relation holding between both elements
     * @param duration  the interval of time holding between both elements
     * @param comment  how this relation was found
     */
    public TemporalRelation(TemporalElement e1, TemporalElement e2, int relation,
            Duration duration, String comment) {
        this(e1, e2, relation, duration);
        this.setComment(comment);
    }
    
    
    /**
     * Creates a new instance of TemporalRelation
     * @param e1  the first parameter of the relation
     * @param e2  the second parameter of the relation
     * @param relation  the relation holding between both elements
     * @param priority the priority level of the relation
     * @param duration  the interval of time holding between both elements
     * @param comment  how this relation was found
     */
    public TemporalRelation(TemporalElement e1, TemporalElement e2, int relation,
            Duration duration, int priority, String comment) {
        this(e1, e2, relation, duration, priority);
        this.setComment(comment);
    }
    
    
    /**
     * Gets a clone
     * @return a clone.
     */
    public Object clone() throws CloneNotSupportedException{
        TemporalRelation result = new TemporalRelation(this.getTemporalElement1(),
                this.getTemporalElement2(), this.getRelation(), this.getInterval(),
                this.getPriority(), this.getComment());
        result.intervalEnabled = this.intervalEnabled;
        return result;
    }
    
    
    /**
     * Returns the first parameter of the relation
     * @return the first parameter of the relation
     */
    public TemporalElement getTemporalElement1() {
        return this.e1;
    }
    
    /**
     * Returns the second parameter of the relation
     * @return the second parameter of the relation
     */
    public TemporalElement getTemporalElement2() {
        return this.e2;
    }
    
    /**
     * Returns the paramater that is linked to the specified temporal element
     * in the TemporalRelation. Returns null if the specified element is not
     * part of the relation.
     *
     * @param e     the temporal element
     * @return      the paramater that is linked to the specified temporal element.
     */
    public TemporalElement getOtherElement(TemporalElement e) {
        if (e1.equals(e))
            return this.e2;
        else if (e2.equals(e))
            return this.e1;
        else
            return null;
    }
    
    /**
     * Replaces an element by another in the temporal relation.
     *
     * @param oldElement the element to by replaced
     * @param newElement the new element.
     * @return  the new TemporalRelation.
     */
    public TemporalRelation replaceElement(TemporalElement oldElement, 
            TemporalElement newElement){
        try {
            TemporalRelation result =(TemporalRelation) this.clone();
            if (this.getTemporalElement1().equals(oldElement))
                result.e1 = newElement;
            else if (this.getTemporalElement2().equals(oldElement))
                result.e2 = newElement;
            return result;
        } catch (CloneNotSupportedException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
    }
    
    /**
     * Tests if the relation can be used for inference and composition or not.
     * @return true whether the relation is enabled, false otherwise
     */
    public boolean isEnabled() {
        return enabled;
    }
    
    
    /**
     * Decides that the relation cannot be used for inference and composition or not.
     */
    public void disable(){
        this.enabled = false;
    }
    
    /**
     * Decides if the relation can be used for inference and composition or not.
     * @param enabled true whether the relation is enabled, false otherwise
     */
    public void setEnabled(boolean enabled) {
        this.enabled = enabled;
    }
    
    
    /**
     * Tests if the interval can be used for inference or not.
     * @return true whether the interval is enabled, false otherwise
     */
    public boolean isIntervalEnabled() {
        return intervalEnabled;
    }
    
    /**
     * Tests if the interval is existing and defined.
     * @return true whether the interval is existing and defined, false otherwise
     */
    public boolean hasInterval(){
        return (this.interval != null && this.interval.isDefined());
    }
    
    /**
     * Disables the interval for inference uses.
     */
    public void disableInterval() {
        this.intervalEnabled = false;
    }
    
    
    /**
     * Returns the relation holding between both elements
     * @return the relation holding between both elements
     */
    public int getRelation() {
        return relation;
    }
    
    /**
     * Sets the relation holding between both elements
     * @param relation the relation holding between both elements
     */
    public void setRelation(int relation) {
        if (relation == 0)
            throw new NullPointerException();
        this.relation = relation;
    }
    
    /**
     * Returns the relation priority level.
     * @return the relation priority level.
     */
    public int getPriority() {
        return priority;
    }
    
    /**
     * Sets the relation priority level.
     * @param priority the relation priority level.
     */
    public void setPriority(int priority) {
        this.priority = priority;
    }
    
    
    /**
     * Returns the highest priority level among the two specified relations
     * @param relation1 the first relation
     * @param relation2 the second relation
     * @return the highest priority level among the two specified relations
     */
    static int getHighestPriority(TemporalRelation relation1, TemporalRelation relation2){
        return java.lang.Math.min(relation1.getPriority(), relation2.getPriority());
    }
    
    /**
     * Returns the relation having the highest priority level among the two specified relations
     * @param relation1 the first relation
     * @param relation2 the second relation
     * @return the relation having the highest priority level among the two specified relations
     */
    public static TemporalRelation getHighestPriorityRelation(TemporalRelation relation1, TemporalRelation relation2){
        if (relation1.getPriority() < relation2.getPriority())
            return relation1;
        else if (relation1.getPriority() > relation2.getPriority())
            return relation2;
        else
            return null;
    }
    
//
//    /**
//     * Gets the temporal graph associated with this relation
//     * @return the temporal graph associated with this relation, null if none.
//     */
//    public TemporalGraph getTemporalGraph() {
//        return temporalGraph;
//    }
//
//    /**
//     * Associates a temporal graph with the relation
//     * @param temporalGraph the graph to associate to the relation
//     */
//    public void setTemporalGraph(TemporalGraph temporalGraph) {
//        this.temporalGraph = temporalGraph;
//    }
    
    
    /**
     * Returns the String value of the specified int relation
     * @param relation the int value of the temporal relation
     * @return the String value of the specified int relation
     */
    public static String relationTypeToString(int relation){
        String result = "";
        if (relation == UNDEFINED){
            result += "UNDEFINED";
        } else {
            if ((relation & EQUALS) != 0)
                result += " EQUALS";
            if ((relation & AFTER) != 0)
                result += " AFTER";
            if ((relation & BEFORE) != 0)
                result += " BEFORE";
            if ((relation & OVERLAPS) != 0)
                result += " OVERLAPS";
            if ((relation & OVERLAPPED_BY) != 0)
                result += " IS_OVERLAPED";
            if ((relation & DURING) != 0)
                result += " DURING";
            if ((relation & INCLUDES) != 0)
                result += " INCLUDES";
        }
        if (result.equals(""))
            throw new NotImplementedException("Relation " + relation + " is unknown.");
        else
            return result.trim().replaceAll(" ", " OR ");
    }
    
    /**
     * Returns the int value of the specified String relation
     * @param relation  the int value of the temporal relation
     * @return the int value of the specified String relation
     */
    public static int stringToRelationType(String relation){
        if (relation.equalsIgnoreCase("UNDEFINED")){
            return UNDEFINED;
        } else if (relation.equalsIgnoreCase("VAGUE")){
            return VAGUE;
        } else if (relation.equalsIgnoreCase("EQUALS")){
            return EQUALS;
        } else if (relation.equalsIgnoreCase("AFTER")){
            return AFTER;
        } else if (relation.equalsIgnoreCase("JUST_AFTER")){
            return AFTER;
        } else if (relation.equalsIgnoreCase("STARTS_WITH")){
            return AFTER | OVERLAPS;
        } else if (relation.equalsIgnoreCase("STARTS")){
            return BEFORE | OVERLAPPED_BY;
        } else if (relation.equalsIgnoreCase("BEFORE")){
            return BEFORE;
        } else if (relation.equalsIgnoreCase("JUST_BEFORE")){
            return BEFORE;
        } else if (relation.equalsIgnoreCase("ENDS_WITH")){
            return BEFORE | OVERLAPPED_BY;
        } else if (relation.equalsIgnoreCase("ENDS")){
            return AFTER | OVERLAPS;
        } else if (relation.equalsIgnoreCase("OVERLAPS")){
            return OVERLAPS;
        } else if (relation.equalsIgnoreCase("IS_OVERLAPED")){
            return OVERLAPPED_BY;
        } else if (relation.equalsIgnoreCase("DURING")){
            return DURING;
        } else if (relation.equalsIgnoreCase("PRESENT")){
            return DURING;
        } else if (relation.equalsIgnoreCase("CONCOMITANT")){
            return DURING | INCLUDES | OVERLAPS | OVERLAPPED_BY;
        } else if (relation.equalsIgnoreCase("INCLUDES")){
            return INCLUDES;
        } else if (relation.equalsIgnoreCase("BEFORE_OR_DURING")){
            return BEFORE | DURING | OVERLAPPED_BY;
        } else if (relation.equalsIgnoreCase("AFTER_OR_DURING")){
            return AFTER | DURING | OVERLAPS;
        } else if (relation.equalsIgnoreCase("BEFORE_OR_OVERLAPS")){
            return BEFORE | OVERLAPPED_BY;
        } else if (relation.equalsIgnoreCase("AFTER_OR_OVERLAPS")){
            return AFTER | OVERLAPS;
        } else if (relation.equalsIgnoreCase("BEFORE_AFTER")){
            return BEFORE | AFTER;
        } else
            throw new NotImplementedException("Relation " + relation + " is unknown.");
    }
    
    
    
    /**
     * Returns the inverse of the current relation (eg: BEFORE(A, B) => AFTER(A, B))
     * @return the inverse of the current relation
     */
    public TemporalRelation inverse(){
        TemporalRelation inverse = new TemporalRelation(this.getTemporalElement2(),
                this.getTemporalElement1(), getInverseRelation(this.getRelation()),
                this.getInterval(), this.getPriority(), this.getComment());
        if (!this.isIntervalEnabled())
            inverse.disableInterval();
        inverse.addComment("+ inversed");
        return inverse;
    }
    
    /**
     * Returns the value of the inversed of the specified relation
     * @param relation the relation
     * @return the value of the inversed relation
     */
    public static int getInverseRelation(int relation){
        int result = 0;
        if (relation == UNDEFINED)
            return UNDEFINED;
        if (relation == VAGUE)
            return VAGUE;
        if (containsRelation(relation, EQUALS))
            result = addRelation(result, EQUALS);
        if (containsRelation(relation, AFTER))
            result = addRelation(result, BEFORE);
        if (containsRelation(relation, BEFORE))
            result = addRelation(result, AFTER);
        if (containsRelation(relation, OVERLAPS))
            result = addRelation(result, OVERLAPPED_BY);
        if (containsRelation(relation, OVERLAPPED_BY))
            result = addRelation(result, OVERLAPS);
        if (containsRelation(relation, DURING))
            result = addRelation(result, INCLUDES);
        if (containsRelation(relation, INCLUDES))
            result = addRelation(result, DURING);
        return result;
    }
    
    
    /**
     * Returns the String representation of the inverse of the specified relation
     * @param relation the relation
     * @return the String representation of the inversed relation
     */
    public static String getInverseRelation(String relation){
        if (relation.equalsIgnoreCase("UNDEFINED"))
            return "UNDEFINED";
        else if (relation.equalsIgnoreCase("VAGUE")){
            return "VAGUE";
        } else if (relation.equalsIgnoreCase("EQUALS")){
            return "EQUALS";
        } else if (relation.equalsIgnoreCase("AFTER")){
            return "BEFORE";
        } else if (relation.equalsIgnoreCase("JUST_AFTER")){
            return "JUST_BEFORE";
        } else if (relation.equalsIgnoreCase("STARTS_WITH")){
            return "STARTS";
        } else if (relation.equalsIgnoreCase("STARTS")){
            return "STARTS_WITH";
        } else if (relation.equalsIgnoreCase("BEFORE")){
            return "AFTER";
        } else if (relation.equalsIgnoreCase("JUST_BEFORE")){
            return "JUST_AFTER";
        } else if (relation.equalsIgnoreCase("ENDS_WITH")){
            return "ENDS";
        } else if (relation.equalsIgnoreCase("ENDS")){
            return "ENDS_WITH";
        } else if (relation.equalsIgnoreCase("OVERLAPS")){
            return "IS_OVERLAPED";
        } else if (relation.equalsIgnoreCase("IS_OVERLAPED")){
            return "OVERLAPS";
        } else if (relation.equalsIgnoreCase("DURING")){
            return "INCLUDES";
        } else if (relation.equalsIgnoreCase("PRESENT")){
            return "PRESENT";
        } else if (relation.equalsIgnoreCase("CONCOMITANT")){
            return "CONCOMITANT";
        } else if (relation.equalsIgnoreCase("INCLUDES")){
            return "DURING";
        } else if (relation.equalsIgnoreCase("BEFORE_OR_DURING")){
            return "AFTER_OR_DURING";
        } else if (relation.equalsIgnoreCase("AFTER_OR_DURING")){
            return "BEFORE_OR_DURING";
        } else if (relation.equalsIgnoreCase("BEFORE_OR_OVERLAPS")){
            return "AFTER_OR_OVERLAPS";
        } else if (relation.equalsIgnoreCase("AFTER_OR_OVERLAPS")){
            return "BEFORE_OR_OVERLAPS";
        } else if (relation.equalsIgnoreCase("BEFORE_AFTER")){
            return "BEFORE_AFTER";
        } else
            throw new NotImplementedException("Relation " + relation + " is unknown.");
    }
    
    /**
     * Returns a String representation of the current relation
     * @return a String representation of the current relation
     */
    public String toString() {
        String result = "";
        TemporalElement element1 = this.getTemporalElement1();
        TemporalElement element2 = this.getTemporalElement2();        
        result += TemporalRelation.relationTypeToString(this.getRelation());
        result += "(";        
        if (element1.isDefined() || element1.getXipNode() == null)
            result += element1.toString();
        else {
            result += "\"" + element1.getXipNode().getSurface() + "\"";
        }
        result += ", ";
        if (element2.isDefined() || element2.getXipNode() == null)
            result += element2.toString();
        else {
            result += "\"" + element2.getXipNode().getSurface() + "\"";
        }

        result += ")";
        if (this.getInterval().isDefined() && !this.getInterval().isNull())
            result += " (interval: " + this.getInterval().toString() + ")";
        return result;
    }
    
    /**
     * Returns a String explaining how this relation was found.
     * @return the comment
     */
    public String getComment() {
        return comment;
    }
    
    /**
     * Sets a String explaining how this relation was found.
     * @param comment a String explaining how this relation was found.
     */
    public void setComment(String comment) {
        this.comment = comment;
    }
    
    /**
     * Adds to the comment a String explaining how this relation was found.
     * @param comment a String explaining how this relation was found.
     */
    public void addComment(String comment) {
        this.comment += " " + comment;
    }
    
    /**
     * Returns the interval holding between the two events
     * @return the interval holding between the two events
     */
    public Duration getInterval() {
        return interval;
    }
    
    /**
     * Sets the interval holding between the two events
     * @param interval the interval holding between the two events
     */
    public void setInterval(Duration interval) {
        this.interval = interval;
    }
    
    
    /**
     * Checks if two relations have the same value
     * @param relation the other relation
     * @return true whether both relations have the same value
     */
    public boolean same(TemporalRelation relation){
        return (this.e1.equals(relation.e1) &&
                this.e2.equals(relation.e2)) ||
                (this.e1.equals(relation.e2) &&
                this.e2.equals(relation.e1));
    }
    
    
    /**
     * Test equality.
     * @param o the object ot compare
     * @return true whether the object equals the current relation, false otherwise
     */
    public boolean equals(Object o){
        if (o instanceof TemporalRelation){
            TemporalRelation relation = (TemporalRelation)o;
            return (this.e1.equals(relation.e1) &&
                    this.e2.equals(relation.e2) &&
                    this.relation == relation.relation) ||
                    (this.e1.equals(relation.e2) &&
                    this.e2.equals(relation.e1) &&
                    this.relation == getInverseRelation(relation.relation));
        } else {
            return false;
        }
    }
    
    
    /**
     * Bit-wise comparison between current relation value and an atomic specified one.
     * 
     * @param relation    the relation to test
     * @return true if the relation contains the specified atomic relation, false otherwise.
     */
    public boolean containsRelation(int relation){
        return containsRelation(this.getRelation(), relation);
    }
    
    
    /**
     * Bit-wise comparison between the disjunctive relation and an atomic one.
     * 
     * @param relationTested   the disjunctive relation to test
     * @param relationSearched         the atomic relation to test
     * @return true if the disjunctive relation contains the atomic one, false otherwise
     */
    public static boolean containsRelation(int relationTested, int relationSearched){
        return ((relationTested & relationSearched) != 0);
    }
    
    /**
     * Adds the flag of the atomic relation to the disjunctive one
     */
    private static int addRelation(int disjunctiveRelation, int atomicRelation){
        if (!containsRelation(disjunctiveRelation, atomicRelation))
            return disjunctiveRelation + atomicRelation;
        else
            return disjunctiveRelation;
    }
    
    
    /**
     * Removes a relation from the specified one
     */
    private static int removeRelationFrom(int relationToRemove, int originalRelation){
        return originalRelation - relationToRemove;
    }
    
    
    /**
     * Returns the number of atomic relations that compose the relation
     *
     * @return the number of atomic relations that compose the relation
     */
    public int getAtomicRelationNumber() {
        int rel = this.getRelation();
        int result = 0;
        if (rel == UNDEFINED || rel == VAGUE)
            return 0;
        else {
            if (containsRelation(rel, EQUALS)) {
                ++result;
            }
            if (containsRelation(rel, AFTER)){
                ++result;                
            }
            if (containsRelation(rel, BEFORE)){
                ++result;            }
            if (containsRelation(rel, OVERLAPS)){
                ++result;            }
            if (containsRelation(rel, OVERLAPPED_BY)){
                ++result;            }
            if (containsRelation(rel, DURING)){
                ++result;            }
            if (containsRelation(rel, INCLUDES)){
                ++result;        
            }
        }
        return result;        
    }
    
    
    /**
     * Tests if the relation is a disjunction of atomic relations
     * @return true whether the relation is a disjunction of atomic relations, false otherwise
     */
    public boolean isADisjunction(){
        int rel = this.getRelation();
        boolean found = false;
        if (rel == UNDEFINED || rel == VAGUE)
            return false;
        else {
            if (containsRelation(rel, EQUALS))
                found = true;
            if (containsRelation(rel, AFTER)){
                if (found)
                    return true;
                else
                    found = true;
            }
            if (containsRelation(rel, BEFORE)){
                if (found)
                    return true;
                else
                    found = true;
            }
            if (containsRelation(rel, OVERLAPS)){
                if (found)
                    return true;
                else
                    found = true;
            }
            if (containsRelation(rel, OVERLAPPED_BY)){
                if (found)
                    return true;
                else
                    found = true;
            }
            if (containsRelation(rel, DURING)){
                if (found)
                    return true;
                else
                    found = true;
            }
            if (containsRelation(rel, INCLUDES)){
                if (found)
                    return true;
                else
                    found = true;
            }
        }
        return false;
    }
    
    /**
     * Returns the value of the relation obtained by merging the two specified
     * values.
     * @param relation1 the first relation
     * @param relation2 the second relation
     * @return the value of the relation obtained by conjunction
     */
    public static int mergeRelations(int relation1, int relation2){
        return relation1 & relation2;
    }
    
    /**
     * Returns the TemporalRelation obtained by merging the current one and the specified one.
     * @param relation the relation to merge with the current one
     * @return the TemporalRelation obtained by conjunction
     */
    public TemporalRelation merge(TemporalRelation relation){
        TemporalElement r1e1 = this.e1;
        TemporalElement r1e2 = this.e2;
        TemporalElement r2e1 = relation.e1;
        TemporalElement r2e2 = relation.e2;
        
        try {
            TemporalRelation result = (TemporalRelation) this.clone();
            
            // same leaving and coming nodes
            if (r1e1.equals(r2e1) && r1e2.equals(r2e2)){
                int newRelation;
                String newComment;
                newRelation = mergeRelations(this.getRelation(), relation.getRelation());
                if (!this.getComment().equals(relation.getComment()))
                    newComment = this.getComment() + " + " + relation.getComment();
                else
                    newComment = this.getComment();
                
                if (newRelation == 0)
                    result = null;
                else {
                    result = new TemporalRelation(r1e1,
                            r1e2,
                            newRelation,
                            getHighestPriority(this, relation),
                            newComment);
                    
                    if (relation.hasInterval())
                        result.setInterval(relation.getInterval());
                    if (this.hasInterval())
                        result.setInterval(this.getInterval());
                    if (this.hasInterval() && relation.hasInterval() &&
                            !this.getInterval().equals(relation.getInterval()))
                        throw new NotImplementedException("Two relations to be merged have different interval values!");
                }
            } else if (r1e1.equals(r2e2) && r1e2.equals(r2e1)){
                result = this.merge(relation.inverse());
            }
            return result;
        } catch (CloneNotSupportedException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
        
    }
    
    /**
     * Return the intersection between two relations.
     *
     * @param relation1 the first relation
     * @param relation2 the second relation
     * @return the intersection between the two relations.
     */
    public static int getIntersection(int relation1, int relation2){
        if (relation1 == UNDEFINED)
            return relation2;
        else if (relation2 == UNDEFINED)
            return relation1;
        else
            return relation1 & relation2;
    }
    

    /**
     * Return the intersection of several relations.
     *
     * @param relations a Vector containing relations to intersect
     * @return the intersection of the relations.
     */
    public static int getIntersection(Vector<Integer> relations){
        int result = getAllRelations();
        for (Integer relation : relations) {
            result = TemporalRelation.getIntersection(result, relation.intValue());
        }
        return result;
    }    
    

    /**
     * Return the union of two relations.
     *
     * @param relation1 the first relation
     * @param relation2 the second relation
     * @return the union of the two relations.
     */
    public static int getUnion(int relation1, int relation2){
        if (relation1 == UNDEFINED || relation2 == UNDEFINED)
            return UNDEFINED;
        else
            return relation1 | relation2;
    }
    
    /**
     * Return the union of several relations.
     *
     * @param relations a Vector containing relations to unify
     * @return the union of the relations.
     */
    public static int getUnion(Vector<Integer> relations){
        int result = 0;
        for (Integer relation : relations) {
            result = TemporalRelation.getUnion(result, relation.intValue());
        }
        return result;
    }
    
    
    /**
     * Returns the value of the relation obtained by the composition of the two specified
     * values.
     * @param relation1 the first relation
     * @param relation2 the second relation
     * @return the value of the relation obtained by composition
     */
    public static int getComposedRelation(int relation1, int relation2){
        int result = 0;
        // UNDEFINED
        if (relation1 == UNDEFINED || relation2 == UNDEFINED)
            return UNDEFINED;
        else if (relation1 == VAGUE || relation2 == VAGUE)
            return UNDEFINED;
        // EQUALS + REL = REL
        else if (relation1 == EQUALS)
            return relation2;
        // REL + EQUALS = REL
        else if (relation2 == EQUALS)
            return relation1;
        // OTHERs (possible disjunctions)
        else {
            /********************************/            
            // e1 OVERLAPS e2:
            //      e1 --------
            //             ---------  e2
            //
            // e1 OVERLAPPED_BY e2:
            //          e1 ---------
            //        --------- e2
            /********************************/
            // AFTER + AFTER = AFTER
            if (containsRelation(relation1, AFTER) && containsRelation(relation2, AFTER)){
                result = addRelation(result, AFTER);
            }
            // AFTER + BEFORE => ??
            if (containsRelation(relation1, AFTER) && containsRelation(relation2, BEFORE)){
                return UNDEFINED;
            }
            // AFTER + OVERLAPS => AFTER + OVERLAPPED_BY + DURING
            if (containsRelation(relation1, AFTER) && containsRelation(relation2, OVERLAPS)){
                result = addRelation(result, AFTER);
                result = addRelation(result, OVERLAPPED_BY);
                result = addRelation(result, DURING);                
            }
            // AFTER + OVERLAPPED_BY => AFTER
            if (containsRelation(relation1, AFTER) && containsRelation(relation2, OVERLAPPED_BY)){
                result = addRelation(result, AFTER);
            }
            // AFTER + DURING => all but BEFORE and INCLUDES
            if (containsRelation(relation1, AFTER) && containsRelation(relation2, DURING)){
                result = addRelation(result, AFTER);
                result = addRelation(result, OVERLAPPED_BY);
                result = addRelation(result, DURING);
            }
            // AFTER + INCLUDES = AFTER
            if (containsRelation(relation1, AFTER) && containsRelation(relation2, INCLUDES)){
                result = addRelation(result, AFTER);
            }
            /////////////
            // BEFORE + AFTER => ??
            if (containsRelation(relation1, BEFORE) && containsRelation(relation2, AFTER)){
                return UNDEFINED;
            }
            // BEFORE + BEFORE => BEFORE
            if (containsRelation(relation1, BEFORE) && containsRelation(relation2, BEFORE)){
                result = addRelation(result, BEFORE);
            }
            // BEFORE + OVERLAPS => BEFORE
            if (containsRelation(relation1, BEFORE) && containsRelation(relation2, OVERLAPS)){
                result = addRelation(result, BEFORE);
            }
            // BEFORE + OVERLAPPED_BY => BEFORE + OVERLAPS + DURING
            if (containsRelation(relation1, BEFORE) && containsRelation(relation2, OVERLAPPED_BY)){
                result = addRelation(result, BEFORE);
                result = addRelation(result, OVERLAPS);
                result = addRelation(result, DURING);
            }
            // BEFORE + DURING => BEFORE + OVERLAPS + DURING
            if (containsRelation(relation1, BEFORE) && containsRelation(relation2, DURING)){
                result = addRelation(result, BEFORE);
                result = addRelation(result, OVERLAPS);
                result = addRelation(result, DURING);
            }
            // BEFORE + INCLUDES => BEFORE
            if (containsRelation(relation1, BEFORE) && containsRelation(relation2, INCLUDES)){
                result = addRelation(result, BEFORE);
            }
            /////////////
            // OVERLAPS + AFTER = AFTER + OVERLAPPED_BY + INCLUDES
            if (containsRelation(relation1, OVERLAPS) && containsRelation(relation2, AFTER)){
                result = addRelation(result, AFTER);
                result = addRelation(result, OVERLAPPED_BY);
                result = addRelation(result, INCLUDES);                
            }
            // OVERLAPS + BEFORE = BEFORE
            if (containsRelation(relation1, OVERLAPS) && containsRelation(relation2, BEFORE)){
                result = addRelation(result, BEFORE);
            }
            // OVERLAPS + OVERLAPS = OVERLAPS + BEFORE
            if (containsRelation(relation1, OVERLAPS) && containsRelation(relation2, OVERLAPS)){
                result = addRelation(result, OVERLAPS);
                result = addRelation(result, BEFORE);
            }
            // OVERLAPS + DURING = OVERLAPS + DURING
            if (containsRelation(relation1, OVERLAPS) && containsRelation(relation2, DURING)){
                result = addRelation(result, OVERLAPS);
                result = addRelation(result, DURING);
            }
            // OVERLAPS + INCLUDES = BEFORE + OVERLAPS + INCLUDES
            if (containsRelation(relation1, OVERLAPS) && containsRelation(relation2, INCLUDES)){
                result = addRelation(result, BEFORE);
                result = addRelation(result, OVERLAPS);
                result = addRelation(result, INCLUDES);
            }
            // OVERLAPS + OVERLAPPED_BY = DURING + OVERLAPS + INCLUDES + OVERLAPPED_BY + EQUALS
            if (containsRelation(relation1, OVERLAPS) && containsRelation(relation2, OVERLAPPED_BY)){
                result = addRelation(result, DURING);
                result = addRelation(result, OVERLAPS);
                result = addRelation(result, INCLUDES);
                result = addRelation(result, OVERLAPPED_BY);
                result = addRelation(result, EQUALS);
            }
            /////////////
            // OVERLAPPED_BY + AFTER = AFTER
            if (containsRelation(relation1, OVERLAPPED_BY) && containsRelation(relation2, AFTER)){
                result = addRelation(result, AFTER);
            }
            // OVERLAPPED_BY + BEFORE = INCLUDES + OVERLAPS + BEFORE
            if (containsRelation(relation1, OVERLAPPED_BY) && containsRelation(relation2, BEFORE)){
                result = addRelation(result, BEFORE);
                result = addRelation(result, OVERLAPS);
                result = addRelation(result, INCLUDES);                
            }
            // OVERLAPPED_BY + OVERLAPPED_BY = AFTER + OVERLAPPED_BY
            if (containsRelation(relation1, OVERLAPPED_BY) && containsRelation(relation2, OVERLAPPED_BY)){
                result = addRelation(result, AFTER);
                result = addRelation(result, OVERLAPPED_BY);
            }
            // OVERLAPPED_BY + DURING = OVERLAPPED_BY + DURING
            if (containsRelation(relation1, OVERLAPPED_BY) && containsRelation(relation2, DURING)){
                result = addRelation(result, OVERLAPPED_BY);
                result = addRelation(result, DURING);
            }
            // OVERLAPPED_BY + INCLUDES = AFTER + OVERLAPPED_BY + INCLUDES
            if (containsRelation(relation1, OVERLAPPED_BY) && containsRelation(relation2, INCLUDES)){
                result = addRelation(result, AFTER);
                result = addRelation(result, OVERLAPPED_BY);
                result = addRelation(result, INCLUDES);
            }
            // OVERLAPPED_BY + OVERLAPS = ALL BUT AFTER / BEFORE
            if (containsRelation(relation1, OVERLAPPED_BY) && containsRelation(relation2, OVERLAPS)){
                result = addRelation(result, OVERLAPS);
                result = addRelation(result, DURING);
                result = addRelation(result, EQUALS);
                result = addRelation(result, INCLUDES);
                result = addRelation(result, OVERLAPPED_BY);
            }
            //////////
            // DURING + AFTER = AFTER
            if (containsRelation(relation1, DURING) && containsRelation(relation2, AFTER)){
                result = addRelation(result, AFTER);
            }
            // DURING + BEFORE = BEFORE
            if (containsRelation(relation1, DURING) && containsRelation(relation2, BEFORE)){
                result = addRelation(result, BEFORE);
            }
            // DURING + OVERLAPS = AFTER + OVERLAPS + DURING
            if (containsRelation(relation1, DURING) && containsRelation(relation2, OVERLAPS)){
                result = addRelation(result, BEFORE);
                result = addRelation(result, OVERLAPS);
                result = addRelation(result, DURING);
            }
            // DURING + OVERLAPPED_BY = BEFORE + OVERLAPPED_BY + DURING
            if (containsRelation(relation1, DURING) && containsRelation(relation2, OVERLAPPED_BY)){
                result = addRelation(result, AFTER);
                result = addRelation(result, OVERLAPPED_BY);
                result = addRelation(result, DURING);
            }
            // DURING + DURING = DURING
            if (containsRelation(relation1, DURING) && containsRelation(relation2, DURING)){
                result = addRelation(result, DURING);
            }
            // DURING + INCLUDES = ??
            if (containsRelation(relation1, DURING) && containsRelation(relation2, INCLUDES)){
                return UNDEFINED;
            }
            ////////////
            // INCLUDES + AFTER = AFTER + OVERLAPPED_BY + INCLUDES
            if (containsRelation(relation1, INCLUDES) && containsRelation(relation2, AFTER)){
                result = addRelation(result, AFTER);
                result = addRelation(result, OVERLAPPED_BY);
                result = addRelation(result, INCLUDES);
            }
            // INCLUDES + BEFORE = BEFORE + OVERLAPS + INCLUDES
            if (containsRelation(relation1, INCLUDES) && containsRelation(relation2, BEFORE)){
                result = addRelation(result, BEFORE);
                result = addRelation(result, OVERLAPS);
                result = addRelation(result, INCLUDES);
            }
            // INCLUDES + OVERLAPS = OVERLAPS + INCLUDES
            if (containsRelation(relation1, INCLUDES) && containsRelation(relation2, OVERLAPS)){
                result = addRelation(result, OVERLAPS);
                result = addRelation(result, INCLUDES);
            }
            // INCLUDES + OVERLAPPED_BY = OVERLAPPED_BY + INCLUDES
            if (containsRelation(relation1, INCLUDES) && containsRelation(relation2, OVERLAPPED_BY)){
                result = addRelation(result, OVERLAPPED_BY);
                result = addRelation(result, INCLUDES);
            }
            // INCLUDES + DURING = OVERLAPS + OVERLAPPED_BY + DURING + INCLUDES + EQUALS
            if (containsRelation(relation1, INCLUDES) && containsRelation(relation2, DURING)){
                result = addRelation(result, OVERLAPS);
                result = addRelation(result, OVERLAPPED_BY);
                result = addRelation(result, DURING);
                result = addRelation(result, INCLUDES);
                result = addRelation(result, EQUALS);
            }
            // INCLUDES + INCLUDES = INCLUDES
            if (containsRelation(relation1, INCLUDES) && containsRelation(relation2, INCLUDES)){
                result = addRelation(result, INCLUDES);
            }
            if (result == 0)
                throw new NotImplementedException("Relations not found (" + relation1 + ", " + relation2 + ")");
            else
                return result;
        }
    }
    
    
    /**
     * Returns the TemporalRelation obtained by the composition of the current one and the specified one.
     * Do not use any inference technique
     * @param relation the relation to compose with the current one
     * @return the TemporalRelation obtained by composition
     * @see #compose(TemporalRelation relation, boolean withInference,
     *       int compositionLevel)
     */
    public TemporalRelation compose(TemporalRelation relation) throws TemporalConsistencyException{
        return this.compose(relation, false, 0);
    }
    
    
    /**
     * Returns the TemporalRelation obtained by the composition of the current one and the specified one.
     * @param relation the relation to compose with the current one
     * @param withInference a boolean specifying if inference calculus should be
     * ran (true) or not (false)
     * @return the TemporalRelation obtained by composition
     */
    public TemporalRelation compose(TemporalRelation relation,
            boolean withInference,
            int compositionLevel) throws TemporalConsistencyException{
        TemporalElement r1e1 = this.e1;
        TemporalElement r1e2 = this.e2;
        TemporalElement r2e1 = relation.e1;
        TemporalElement r2e2 = relation.e2;
        int r;
        TemporalRelation result = null;
        TemporalRelation resultInference = null;
        
        
//        if (r1e1.toString().equals("expected"))
//            System.err.println("");
        
        /**************
         * Inference
         **************/
        
        if (withInference)
            resultInference = inferenceComposition(this, relation);
        
//        if (resultInference != null)
//               System.err.println("");
        
        
        if (r1e1.equals(r2e1) && r1e2.equals(r2e2)){
            return null;
        } else if (r1e2.equals(r2e1)) {
            // do not compose a relation between two dates or two durations
            if (!((r1e1 instanceof VagueDate && r2e2 instanceof VagueDate) ||
                    (r1e1 instanceof Duration && r2e2 instanceof Duration))){
                if (compositionLevel == 0 || r1e1.getSentenceNumber() <= 0 || r2e2.getSentenceNumber() <= 0 ||
                        java.lang.Math.abs(r1e1.getSentenceNumber() - r2e2.getSentenceNumber()) <= compositionLevel){
                    /***********************
                     * Simple composition
                     ***********************/
//                    test++;
//                    System.err.println(test);
//                    if (test == 5)
//                        System.err.println("");
                    r = getComposedRelation(this.relation, relation.relation);
                    // Priority
                    // if the composition concerns a date, then the maximum priority
                    // is kept
                    // e.g. : if we have DURING(A, D1) and AFTER(D1, D2) ==> AFTER(A, D2)
                    //                                          with high priority
                    // else we keep COMPOSITION priority
                    int priority;
                    priority = java.lang.Math.max(this.getPriority(), relation.getPriority());
                    if (!r1e1.equals(r2e2))
                        result = new TemporalRelation(r1e1, r2e2, r, priority, "Composition");
                }
            }
        } else if (r1e1.equals(r2e1)) {
            TemporalRelation inverseRelation = this.inverse();
            result = inverseRelation.compose(relation, withInference, compositionLevel);
        } else if (r1e2.equals(r2e2)) {
            TemporalRelation inverseRelation = relation.inverse();
            result = this.compose(inverseRelation, withInference, compositionLevel);
        } else if (r1e1.equals(r2e2)) {
            result = relation.compose(this, withInference, compositionLevel);
        }
        // merge both relations
        if (result != null && resultInference != null)
            result = result.merge(resultInference);
        if (result == null && resultInference != null)
            result = resultInference;
        return result;
    }
    
    /**
     * Refines a temporal relation with some basic strategies.
     * Returns the new refined relation, do not modify the current one.
     * @return a refined relation
     */
    public TemporalRelation refine(){
        TemporalElement element1 = this.getTemporalElement1();
        TemporalElement element2 = this.getTemporalElement2();
        try {
            TemporalRelation result = (TemporalRelation)this.clone();
            
            // relation between two dates with an interval
            if (element1 instanceof VagueDate && element2 instanceof VagueDate &&
                    this.hasInterval()) {
                VagueDate date1 = (VagueDate)element1;
                VagueDate date2 = (VagueDate)element2;
                Duration interval = this.getInterval();
                if (!date1.isDefined() && date2.isDefined()){
                    VagueDate newDate;
                    if (this.getRelation() == BEFORE){
                        newDate = date2.substract(interval);
                    } else if (this.getRelation() == AFTER){
                        newDate = date2.add(interval);
                    } else if (this.getRelation() == (AFTER | BEFORE)) {
                        String strDate1 = date1.substract(interval).getValue();
                        String strDate2 = date2.add(interval).getValue();
                        try {
                            newDate = new VagueMultipleDate(strDate1 + "|" + strDate2);
                        } catch (BadFormatException ex) {
                            ex.printStackTrace();
                            System.exit(-1);
                            return null;
                        }
                    } else
                        throw new NotImplementedException("A relation with interval should be only be present with AFTER OR BEFORE !");
                    
                    result = new TemporalRelation(newDate, date1,
                            TemporalRelation.EQUALS, this.getPriority(),
                            "Interval between two dates");
                    this.disableInterval();
                } // same thing but the inverse
                else if (date1.isDefined() && !date2.isDefined()) {
                    result = this.inverse().refine();
                }
            }
            
            
            
            
            // relation between an event and a date
            // if the event has a duration bigger than the date granularity, then
            //  DURING is not possible
            else if (element1 instanceof Event && element2 instanceof VagueDate &&
                    ((Event)element1).hasDuration()){
                Duration duration = ((Event)element1).getDuration();
//                int granularity = ((VagueDate)element2).getGranularity();
                if (((VagueDate)element2).isDefined()){
                    Duration durationGranularity = ((VagueDate)element2).getGranularityDuration();
//                        new SimpleDuration(DurationUnit.getDurationUnit(granularity, 1));
                    if (duration.compareTo(durationGranularity) > 0) {
                        int mergedRelation = mergeRelations(removeRelationFrom(DURING, getAllRelations()),
                                result.getRelation());
                        if (mergedRelation != 0)
                            result.setRelation(mergedRelation);
                    }
                }
            }
            
            // same as previously but inverse relation.
            else if (element2 instanceof Event && element1 instanceof VagueDate &&
                    ((Event)element2).hasDuration()){
                result = this.inverse().refine();
            }
            return result;
        } catch (CloneNotSupportedException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
        
    }
    
    /**
     * Returns a new TemporalRelation obtained from the two specified ones.
     * @param relation1 the first relation
     * @param relation2 the second relation
     * @return a TemporalRelation obtained by inference composition, or null if no such
     * relation has been found.
     */
    private static TemporalRelation inferenceComposition(TemporalRelation relation1,
            TemporalRelation relation2) throws TemporalConsistencyException{
        TemporalRelation result = null;
        TemporalElement r1e1 = relation1.getTemporalElement1();
        TemporalElement r1e2 = relation1.getTemporalElement2();
        TemporalElement r2e1 = relation2.getTemporalElement1();
        TemporalElement r2e2 = relation2.getTemporalElement2();
        // 2 months ago
        //    first relation BEFORE or AFTER with an interval between E1 and E2
        //    second relation between E2 and a date.
        // eg: BEFORE(A, B)[interval] AND DURING(B, DATE)
        if (r1e2.equals(r2e1) &&
                r2e2 instanceof VagueDate &&
                (relation2.getRelation() == INCLUDES || relation2.getRelation() == DURING || relation2.getRelation() == EQUALS)) {
            if (relation1.getInterval().isDefined() && !relation1.getInterval().isNull() &&
                    relation1.isIntervalEnabled()) { // &&
                
                VagueDate newDate;
                // BEFORE => we substract the duration (interval)
                if (relation1.getRelation() == BEFORE){
                    newDate = ((VagueDate)r2e2).substract(relation1.getInterval());
                    // AFTER => we add the duration (interval)
                } else if (relation1.getRelation() == AFTER){
                    newDate = ((VagueDate)r2e2).add(relation1.getInterval());
                    // BEFORE OR AFTER (some expressions in English are ambiguous and only
                    //                        mean that an interval exists)
                    //  Create a OR multiple date with substraction and addition of the duration
                } else if (relation1.getRelation() == (AFTER | BEFORE)) {
                    String date1 = ((VagueDate)r2e2).substract(relation1.getInterval()).getValue();
                    String date2 = ((VagueDate)r2e2).add(relation1.getInterval()).getValue();
                    try {
                        newDate = new VagueMultipleDate(date1 + "|" + date2);
                    } catch (BadFormatException ex) {
                        ex.printStackTrace();
                        System.exit(-1);
                        return null;
                    }
                    // any other relation should not exist (DURING with an interval is not possible)
                } else
                    throw new NotImplementedException("A relation with interval should be only present with AFTER OR BEFORE !");
                
                // if the first element of the relation
                // is a event, this event is DURING the new calculated date
                if (r1e1 instanceof Event)
                    result = new TemporalRelation(r1e1, newDate, DURING,
                            getHighestPriority(relation1, relation2),
                            "ANCHOR with interval");
                // if the first element of the relation
                // is a date (eg: DATE1 BEFORE DATE2, interval 2 months)
                // then this date DATE1 is exactly (EQUALS) the new calculated date
                else if (r1e1 instanceof VagueDate){
//                r1e1 = VagueDate.copyDateInfos((VagueDate)r1e1, newDate);
                    result = new TemporalRelation(r1e1, newDate, EQUALS,
                            getHighestPriority(relation1, relation2),
                            "ANCHOR with interval");
                    relation1.disable();
                }
                // the interval is disabled so that the calculus is not made several times
                relation1.disableInterval();
            } // same thing but the inverse
            else if (relation2.getInterval().isDefined() && !relation2.getInterval().isNull() &&
                    relation2.isIntervalEnabled()) {
                return inferenceComposition(relation2.inverse(), relation1.inverse());
            }
        }
        // two relations from a same event to different dates
        else if (r1e1.equals(r2e1) &&
                r1e1 instanceof Event &&
                r1e2 instanceof VagueDate &&
                r2e2 instanceof VagueDate){
            
            // check relations and disable useless one
//            int rel = ((VagueDate)r1e2).compareTo((VagueDate)r2e2);
//            if (relation1.getRelation() == DURING && relation2.getRelation() == DURING){
//                if (rel == INCLUDES)
//                    relation1.disable();
//                else if (rel == DURING)
//                    relation2.disable();
//            } else if (relation1.getRelation() == DURING && relation2.getRelation() == AFTER){
//                if (rel == AFTER)
//                    relation2.disable();
//            } else if (relation1.getRelation() == DURING && relation2.getRelation() == BEFORE){
//                if (rel == BEFORE)
//                    relation2.disable();
//            } else if (relation1.getRelation() == AFTER && relation2.getRelation() == DURING){
//                if (rel == BEFORE)
//                    relation1.disable();
//            } else if (relation1.getRelation() == BEFORE && relation2.getRelation() == DURING){
//                if (rel == AFTER)
//                    relation1.disable();
//            } else if (relation1.getRelation() == AFTER && relation2.getRelation() == AFTER){
//                if (rel == AFTER)
//                    relation2.disable();
//                else if (rel == BEFORE)
//                    relation1.disable();
//            } else if (relation1.getRelation() == BEFORE && relation2.getRelation() == BEFORE){
//                if (rel == BEFORE)
//                    relation1.disable();
//                else if (rel == AFTER)
//                    relation2.disable();
//            }
            
            
            // "at 3 o'oclock" (=> 3 AM or 3 PM) and "in the afternoon"
            // "at 3:20" and "on 2nd of February"
            //   linked to the same event
            //   => merge
            if (relation1.getRelation() == DURING && relation2.getRelation() == DURING){
                VagueDate date1 = (VagueDate)r1e2;
                VagueDate date2 = (VagueDate)r2e2;
                // "at 3:20" and "on 2nd of February"
                // => at 3:20, 2nd of February
                if (date1.isDefined() && date1.getGranularity() < date2.getGranularity()){
                    VagueDate newDate1;
                    VagueDate newDate2;
                    
                    newDate1 = date2.completeDate(date1);
                    result = new TemporalRelation(newDate1, date2, EQUALS,
                            getHighestPriority(relation1, relation2), "Merge two dates");
                    newDate2 = date1;
                } else if (date2.isDefined() && date1.getGranularity() > date2.getGranularity()){
                    VagueDate newDate1;
                    VagueDate newDate2;
                    
                    newDate1 = date1.completeDate(date2);
                    result = new TemporalRelation(newDate1, date1, EQUALS,
                            getHighestPriority(relation1, relation2), "Merge two dates");
                    newDate2 = date2;
                } else {
                    
                    VagueDate newDate = date1.merge(date2);
//                if (date1.getXipNode() != null && date1.getXipNode().equals(newDate.getXipNode()))
//                    result = new TemporalRelation(r1e2, newDate)
//                if (newDate instanceof VagueInterval)
//                    System.err.println("");
                    if (newDate != null && newDate.isDefined()){
                        result =
                                new TemporalRelation(r1e1, newDate, DURING,
                                getHighestPriority(relation1, relation2), "Merge two dates");
                        if (newDate.equals(date1))
                            relation2.disable();
                        else if (newDate.equals(date2))
                            relation1.disable();
                    }
                }
            }
        }
        // two relations with dates :
        //  DURING(DATE1, DATE2) AND EQUALS(DATE2, DATE3)
        // complete DATE1 with DATE3 if needed
        else if (r1e2.equals(r2e1) &
                relation1.getRelation() == TemporalRelation.DURING &&
                relation2.getRelation() == TemporalRelation.EQUALS &&
                r1e1 instanceof VagueDate && r1e2 instanceof VagueDate && r2e2 instanceof VagueDate) {
            VagueDate newDate = ((VagueDate)r1e1).completeDate((VagueDate)r2e2);
            result = new TemporalRelation(newDate, r1e1, TemporalRelation.EQUALS,
                    TemporalRelation.getHighestPriority(relation1, relation2), "Complete date");
//            relation1.s
        }
        return result;
    }
    
    
    /**
     * Gets the relation number corresponding to the disjunction of all
     * possible relations.
     *
     * @return the relation number corresponding to the disjunction of all
     * possible relations.
     */
    public static int getAllRelations(){
        return BEFORE | AFTER | OVERLAPS | OVERLAPPED_BY | INCLUDES | DURING | EQUALS;
    }
    
    
    
    /**
     * Checks whether two relations are compatible or not. eg:
     * <ul><li>BEFORE(A, B) and BEFORE(B, A) are not compatible.
     *     <li>AFTER(A, B) and AFTER(A, B) are compatible.
     *     <li>UNDEFINED is compatible with everything, the other being returned
     * </ul>
     * @param relation1 the first relation
     * @param relation2 the second relation
     * @throws documentanalysis.temporality.TemporalConsistencyException if they are not compatible
     * @return the relation to keep between both
     */
    public static TemporalRelation checkRelation(TemporalRelation relation1,
            TemporalRelation relation2) throws TemporalConsistencyException{
        // same relation => OK
        if (relation1.equals(relation2))
            return relation2;
        // (E1, E2) and (E2, E1) => rerun with (E1, E2) and (E1, E2)
        else if (relation1.getTemporalElement1().equals(relation2.getTemporalElement2()) &&
                relation1.getTemporalElement2().equals(relation2.getTemporalElement1())){
            TemporalRelation auxRelation = relation2.inverse();
            return checkRelation(relation1, auxRelation);
            // (E1, E2) and (E1, E2) => check consistency
        } else if (relation1.getTemporalElement1().equals(relation2.getTemporalElement1()) &&
                relation1.getTemporalElement2().equals(relation2.getTemporalElement2())){
            int rel1 = relation1.getRelation();
            int rel2 = relation2.getRelation();
            // if both relations are the same, OK
            if (rel1 == rel2)
                return relation2;
            // one UNDEFINED, return the other
            else if (rel1 == UNDEFINED || rel1 == VAGUE)
                return relation2;
            else if (rel2 == UNDEFINED || rel2 == VAGUE)
                return relation1;
            // ELSE bit-wise mix of two relations
            else {
                int rel = mergeRelations(rel1, rel2);
                if (rel == 0)
                    throw new TemporalConsistencyException("Temporal consistency exception when merging \"" +
                            relation1.toString() + "\" and \"" + relation2.toString() + "\".");
                else {
                    try {
                        TemporalRelation newRelation = (TemporalRelation) relation1.clone();
                        newRelation.setRelation(rel);
                        return newRelation;
                    } catch (CloneNotSupportedException ex) {
                        ex.printStackTrace();
                        System.exit(-1);
                        return null;
                    }
                }
            }
        } else
            // not the same nodes
            return null;
    }
    
    
    /**
     * Sorts temporal relations from the most important ones to the least important.
     * The most important are DURING/INCLUDES, followed by other atomic relations, then
     * disjunctive relations, and finally relations between two dates
     */
    public static Vector<TemporalRelation> sortRelations(Vector<TemporalRelation> relations) {
        Vector<TemporalRelation> group1 = new Vector<TemporalRelation>();
        Vector<TemporalRelation> group2 = new Vector<TemporalRelation>();
        Vector<TemporalRelation> group3 = new Vector<TemporalRelation>();
        Vector<TemporalRelation> group4 = new Vector<TemporalRelation>();
        
        for (TemporalRelation relation : relations) {
            TemporalElement element1 = relation.getTemporalElement1();
            TemporalElement element2 = relation.getTemporalElement2();
            if (element1 instanceof VagueDate && element2 instanceof VagueDate) {
                group4.add(relation);
            } else {
                if (relation.isADisjunction()) {
                    group3.add(relation);
                } else {
                    int r = relation.getRelation();
                    if (r == INCLUDES || r == DURING) {
                        group1.add(relation);
                    } else {
                        group2.add(relation);
                    }
                }
            }
        }
        group3.addAll(group4);
        group2.addAll(group3);
        group1.addAll(group2);                
        return group1;
    }
    
}
