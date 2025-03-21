/*
 * TemporalGraph.java
 *
 * Created on December 11, 2006, 3:10 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality;

import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipUnit;
import common.CommonLog;
import common.NotImplementedException;
import documentAnalysis.events.Event;
import documentAnalysis.temporality.vaguecalendar.BadFormatException;
import documentAnalysis.temporality.vaguecalendar.date.IntervalException;
import documentAnalysis.temporality.vaguecalendar.date.VagueDate;
import documentAnalysis.temporality.vaguecalendar.date.VagueInterval;
import documentAnalysis.temporality.vaguecalendar.date.VagueSimpleDate;
import documentAnalysis.temporality.vaguecalendar.duration.Duration;
import java.util.HashMap;
import java.util.Map;
import java.util.Random;
import java.util.Stack;
import java.util.Vector;

/**
 * Class representing a temporal graph. The nodes are TemporalElements, the arcs are TemporalRelations.
 * A graph represents a document.
 * @author xtannier
 */
public class TemporalGraph {
    
    private Vector<Event> events;
    private Vector<VagueDate> dates;
    private Vector<Duration> durations;
    private Vector<TemporalRelation> relations;
//    private Vector<Pair<VagueDate, VagueDate> > dateReferences;
    
    private Event speechTime;
    private VagueDate dct;
    private Stack<Event> referenceTimes;
    
    private static int test=0;
    
    
    /** Creates a new instance of an empty TemporalGraph */
    public TemporalGraph() {
        this.events = new Vector<Event>();
        this.relations = new Vector<TemporalRelation>();
        this.dates = new Vector<VagueDate>();
        this.durations = new Vector<Duration>();
//        this.dateReferences = new Vector<Pair<VagueDate, VagueDate> >();
        this.referenceTimes = new Stack<Event>();
        Event speechTime = new Event(true);
        this.setSpeechTime(speechTime);
        TempFunctions.clearTempDict();
    }
    
    /**
     * Creates a new instance of a TemporalGraph with the specified speech time
     * @param speechTime - the speech time event
     */
    public TemporalGraph(Event speechTime) {
        this();
        this.setSpeechTime(speechTime);
    }
    
    
    /**
     * Clears temporal graph.
     */
    public void clear(){
        this.events = new Vector<Event>();
        this.relations = new Vector<TemporalRelation>();
        this.dates = new Vector<VagueDate>();
        this.durations = new Vector<Duration>();
        this.referenceTimes = new Stack<Event>();
//        this.dateReferences = new Vector<Pair<VagueDate, VagueDate> >();
        Event speechTime = new Event(true);
        this.setSpeechTime(speechTime);
        this.dct = null;
        TempFunctions.clearTempDict();
    }
    
    /**
     * Returns a clone of the temporal graph
     */
    public TemporalGraph clone() {
        try {
            TemporalGraph result = new TemporalGraph(this.speechTime);
            // events
            for (Event event : this.events) {
                result.addEvent(event);
            }
            // dates
            for (VagueDate date : this.dates) {
                
                result.addDate((VagueDate)date.clone());
            }
            // durations
            for (Duration duration : this.durations) {
                result.addDuration((Duration)duration.clone());
            }
            // reference times
            for (Event event : this.referenceTimes) {
                result.addReferenceTime(event);
            }
            // relations
            for (TemporalRelation relation : this.relations) {
                result.addRelation((TemporalRelation)relation.clone());
            }
            return result;
        } catch (CloneNotSupportedException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        } catch (TemporalConsistencyException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
    }
    
    
    /**
     * Tests for equality of graphs
     *
     * @param object another graph
     * @return true if both graphs contain the same elements and relations
     *  and so on, false otherwise.
     */
    public boolean equals(Object object) {
        if (!(object instanceof TemporalGraph)) {
            return false;
        }
        TemporalGraph graph = (TemporalGraph) object;
        // events
        if (this.events.size() != graph.events.size())
            return false;
        
        if (!graph.events.containsAll(this.events))
            return false;
        
        // dates
        if (this.dates.size() != graph.dates.size())
            return false;
        
        if (!graph.dates.containsAll(this.dates))
            return false;
        
        // durations
        if (this.durations.size() != graph.durations.size())
            return false;
        
        if (!graph.durations.containsAll(this.durations))
            return false;
        
        // relations
        if (this.relations.size() != graph.relations.size())
            return false;
        
        if (!graph.relations.containsAll(this.relations))
            return false;
        
        // speechTime
        if (this.speechTime == null && graph.speechTime != null)
            return false;
        if (this.speechTime != null && graph.speechTime == null)
            return false;
        if (this.speechTime != null && graph.speechTime != null && !this.speechTime.equals(graph.speechTime))
            return false;
        
        // dct
        if (this.dct == null && graph.dct != null)
            return false;
        if (this.dct != null && graph.dct == null)
            return false;
        if (this.dct != null && graph.dct != null){
            if (this.dct.isDefined() && !graph.dct.isDefined())
                return false;
            if (!this.dct.isDefined() && graph.dct.isDefined())
                return false;
            if (this.dct.isDefined() && graph.dct.isDefined() && !this.dct.equals(graph.dct))
                return false;
        }
        
        // reference times
        if (this.referenceTimes.size() != graph.referenceTimes.size())
            return false;
        
        if (!graph.referenceTimes.containsAll(this.referenceTimes))
            return false;
        
        return true;
    }
    
    
    /**
     * Returns the number of elements (nodes) of the graph
     *
     * @return the number of elements of the graph
     */
    public int getNodeNumber() {
        return this.events.size() + this.dates.size() + this.durations.size() +
                this.referenceTimes.size();
    }
    
    
    /**
     * Returns the number of relations (arcs) of the graph
     *
     * @return the number of relations of the graph
     */
    public int getRelationNumber() {
        return this.relations.size();
    }
    
    
    /**
     * Returns the speech time event of the graph
     * @return the speech time event
     */
    public Event getSpeechTime() {
        return speechTime;
    }
    
    /**
     * Sets the speech time event of the graph
     * @param speechTime the speech time event
     */
    public void setSpeechTime(Event speechTime) {
        if (this.speechTime == null || !this.speechTime.equals(speechTime) || this.referenceTimes.isEmpty())
            this.addReferenceTime(speechTime);
        this.speechTime = speechTime;
    }
    
    
    /**
     * Returns the DCT (Document Creation Time) of the graph
     * @return the DCT
     */
    public VagueDate getDCT(){
        return this.dct;
    }
    
    /**
     * Sets the DCT (Document Creation Time) of the graph
     * @param date the DCT
     */
    public void setDCT(VagueDate date) throws TemporalConsistencyException {
        this.dct = date;
        this.addDate(date);
    }
    
    
    /**
     * Returns a Vector containing all the relations of the graph
     * @return a Vector containing all the relations of the graph
     */
    public Vector<TemporalRelation> getRelations() {
        return this.relations;
    }
    
    /**
     * Returns a Vector containing all the events of the graph. Events are special TemporalElements.
     * @return a Vector containing all the events of the graph
     */
    public Vector<Event> getEvents() {
        return this.events;
    }
    
    
    /**
     * Returns a Vector containing all the dates of the graph. Dates are special TemporalElements.
     * @return a Vector containing all the dates of the graph
     */
    public Vector<VagueDate> getDates() {
        return this.dates;
    }
    
    
    /**
     * Returns a Vector containing all the durations of the graph. Durations are special TemporalElements.
     * @return a Vector containing all the durations of the graph
     */
    public Vector<Duration> getDurations() {
        return this.durations;
    }
    
    
    /**
     * Returns a Vector containing all the temporal elements of the graph.
     * @return a Vector containing all the temporal elements of the graph.
     */
    public Vector<TemporalElement> getElements(){
        Vector<TemporalElement> result = new Vector<TemporalElement>();
        result.addAll(this.events);
        result.addAll(this.dates);
        result.addAll(this.durations);
        return result;
    }
    
    /**
     * Returns a Vector containing all the events of the graph corresponding to the specified sentence.
     * Events are special TemporalElements.
     * @return a Vector containing all the events of the graph corresponding to the specified sentence
     */
    public Vector<Event> getEvents(long sentenceNumber) {
        Vector<Event> result = new Vector<Event>();
        for (Event event : this.events){
            if (event.getSentenceNumber() == sentenceNumber)
                result.add(event);
        }
        return result;
    }
    
    /**
     * Returns a Vector containing all the dates of the graph corresponding to the specified sentence.
     * Dates are special TemporalElements.
     * @return a Vector containing all the dates of the graph corresponding to the specified sentence
     */
    public Vector<VagueDate> getDates(long sentenceNumber) {
        Vector<VagueDate> result = new Vector<VagueDate>();
        for (VagueDate date : this.dates){
            if (date.getSentenceNumber() == sentenceNumber)
                result.add(date);
        }
        return result;
    }
    
    
    /**
     * Returns a Vector containing all the durations of the graph corresponding to the specified sentence.
     * Durations are special TemporalElements.
     * @return a Vector containing all the durations of the graph corresponding to the specified sentence
     */
    public Vector<Duration> getDurations(long sentenceNumber) {
        Vector<Duration> result = new Vector<Duration>();
        for (Duration duration : this.durations){
            if (duration.getSentenceNumber() == sentenceNumber)
                result.add(duration);
        }
        return result;
    }
    
    
    /**
     * Returns a Vector containing all the temporal elements of the graph corresponding to the specified sentence.
     * @return a Vector containing all the temporal elements of the graph corresponding to the specified sentence.
     */
    public Vector<TemporalElement> getElements(long sentenceNumber){
        Vector<TemporalElement> result = new Vector<TemporalElement>();
        result.addAll(this.getEvents(sentenceNumber));
        result.addAll(this.getDates(sentenceNumber));
        result.addAll(this.getDurations(sentenceNumber));
        return result;
    }
    
    
    /**
     * Gets the Event corresponding the the specified XipNode in the Graph.
     *
     * @param unit the XipUnit where the event has been found
     * @param node the XipNode associated to the event
     * @param addIfNotPresent if true and if the Event is not present in the Graph,
     * creates it and adds it in the Graph
     * @return the corresponding event.
     */
    public Event getEvent(XipUnit unit, XipNode node, boolean addIfNotPresent){
        for (Event e : this.events){
            if (e.getXipNode() != null && e.getXipNode().equals(node)){
                return e;
            }
        }
        // not found
        Event result = null;
        if (addIfNotPresent){
            result = new Event(unit, node);
            this.addEvent(result);
        }
        return result;
    }
    
    
    /**
     * Gets the VagueDates corresponding the the specified XipNode in the Graph.
     *
     * @param unit the XipUnit where the date has been found
     * @param node the XipNode associated to the date
     * @return a Vector a corresponding dates.
     */
    public Vector<VagueDate> getDates(XipUnit unit, XipNode node){
        Vector<VagueDate> result = new Vector<VagueDate>();
        for (VagueDate d : this.dates){
            if (d.getXipNode() != null && d.getXipNode().equals(node)){
                result.add(d);
            }
        }
        return result;
    }
    
    
    /**
     * Gets the Duration corresponding the the specified XipNode in the Graph.
     *
     * @param unit the XipUnit where the duration has been found
     * @param node the XipNode associated to the duration
     * @return the corresponding duration, null if not found
     */
    public Duration getDuration(XipUnit unit, XipNode node){
        for (Duration d : this.durations){
            if (d.getXipNode() != null && d.getXipNode().equals(node)){
                return d;
            }
        }
        return null;
    }
    
    /**
     * Adds an Event into the graph.
     * @param e the Event to add
     */
    private void addEvent(Event e){
        int index = this.events.indexOf(e);
        if (index == -1){
            this.events.add(e);
            if (e.isSpeechTime())
                this.setSpeechTime(e);
        } else {
            Event e2 = this.events.get(index);
            @SuppressWarnings("unchecked")
            Vector<TemporalRelation> relations = (Vector<TemporalRelation>)e.getTemporalRelations().clone();
            for (TemporalRelation relation : relations){
                e2.addTemporalRelation(relation);
            }
            if (e.hasDuration() && !e2.hasDuration())
                e2.setDuration(e.getDuration());
            if (e2.isSpeechTime())
                this.setSpeechTime(e2);
            
        }
    }
    
    /**
     * Adds a VagueDate into the graph
     * @param date the date to add.
     */
    public void addDate(VagueDate date) throws TemporalConsistencyException{
        VagueDate oldDate = null;
        XipNode node = date.getXipNode();
        if (node != null){
            Vector<VagueDate> existingDates = this.getDates(date.getXipUnit(), node);
            for (VagueDate existingDate : existingDates){
                if (existingDate.getClass().equals(date.getClass())){
                    oldDate = existingDate;
                    break;
                }
            }
        } else {
            int index = this.dates.indexOf(date);
            if (index != -1)
                oldDate = this.dates.get(index);
        }
        
        
        if (oldDate == null){
            dates.add(date);
            VagueDate date2;
            int c;
            for (int i = 0; i < dates.size(); ++i) {
                date2 = dates.get(i);
                c = date.compareTo(date2);
                if (!date.equals(date2) && c != TemporalRelation.UNDEFINED) {
                    TemporalRelation relation = new TemporalRelation(date, date2, c,
                            TemporalRelation.CALENDAR, "Date comparison");
//                    test++;
//                    System.err.println("Test: " + test);
//                    if (test == 400)
//                        System.err.println("Test: " + test);
                    this.addRelation(relation);
                }
            }
        } else {
//                    ++test;
//                    System.err.println("test = " + test );
//                    if (test == 44) {
//                        System.err.println("");
//                    }            
            @SuppressWarnings("unchecked")
            Vector<TemporalRelation> relations = (Vector<TemporalRelation>)date.getTemporalRelations().clone();
            for (TemporalRelation relation : relations){
                if (oldDate.isDCT()) {

                    ((VagueDate)relation.getTemporalElement1()).setDCT();
                }
                oldDate.addTemporalRelation(relation);
            }
        }
    }
    
    /**
     * Adds a Duration into the graph
     * @param duration the duration to add.
     */
    public void addDuration(Duration duration) {
        Duration oldDuration = null;
        XipNode node = duration.getXipNode();
        if (node != null){
            oldDuration = this.getDuration(duration.getXipUnit(), node);
        }
        
        if (oldDuration == null){
            durations.add(duration);
        } else {
            @SuppressWarnings("unchecked")
            Vector<TemporalRelation> relations = (Vector<TemporalRelation>)duration.getTemporalRelations().clone();
            for (TemporalRelation relation : relations){
                oldDuration.addTemporalRelation(relation);
            }
        }
    }
    
    
    /**
     * Adds a TemporalElement into the graph. Adds it either in the Event list or in the Date list.
     * @param element - the element to add
     */
    public void addTemporalElement(TemporalElement element) throws TemporalConsistencyException{
        if (element instanceof Event)
            this.addEvent((Event) element);
        else if (element instanceof VagueDate)
            this.addDate((VagueDate) element);
        else if (element instanceof Duration)
            this.addDuration((Duration) element);
        else
            throw new NotImplementedException();
    }
    
    
    /**
     * Replaces a VagueDate by another into the graph. Updates relations
     * @param oldDate the date to remove.
     * @param newDate the date to add
     */
    public void replaceDate(VagueDate oldDate, VagueDate newDate) throws TemporalConsistencyException{
        int i = dates.indexOf(oldDate);
        if (i == -1)
            this.addDate(newDate);
        else {
            VagueDate date = dates.get(i);
            Vector<TemporalRelation> oldRelations = date.getTemporalRelations();
            Vector<TemporalRelation> relationsToAdd = new Vector<TemporalRelation>();
            for (TemporalRelation oldRelation : oldRelations) {
                if (oldRelation.getOtherElement(date) instanceof Event){
                    TemporalRelation newRelation = oldRelation.replaceElement(date, newDate);
                    newDate.addTemporalRelation(newRelation);
                    relationsToAdd.add(newRelation);
                } //else
                this.relations.remove(oldRelation);
            }
            for (TemporalRelation relationToAdd : relationsToAdd){
                this.addRelation(relationToAdd);
            }
            dates.remove(i);
            this.addDate(newDate);
        }
    }
    
    
    /**
     * Removes a TemporalRelation from the graph.
     *
     * @param relation the TemporalRelation to remove
     */
    public void removeRelation(TemporalRelation relation){
        this.relations.remove(relation);
    }
    
    /**
     * Returns the number of enabled relations in the graph
     * @return the number of enabled relations in the graph
     */
    public int countEnabledRelations(){
        int result = 0;
        for (TemporalRelation relation : this.relations){
            if (relation.isEnabled())
                ++result;
        }
        return result;
    }
    
    /**
     * Tests whether two TemporalElements are linked by a relation in the graph
     * @param e1 the first element
     * @param e2 the second element
     * @return true whether two TemporalElements are linked by a relation in the graph, false otherwise
     */
    public boolean hasRelation(TemporalElement e1, TemporalElement e2){
        for (TemporalRelation relation : this.relations){
            if ((relation.getTemporalElement1().equals(e1) && relation.getTemporalElement2().equals(e2)) ||
                    (relation.getTemporalElement1().equals(e2) && relation.getTemporalElement1().equals(e2)))
                return true;
        }
        return false;
    }
    
    
    /**
     * Returns the TemporalRelation holding between the two specified TemporalElements
     * @param e1 the first element
     * @param e2 the second element
     * @return the TemporalRelation holding between the two TemporalElements, null if no relation.
     */
    public TemporalRelation getRelation(TemporalElement e1, TemporalElement e2){
        TemporalElement rele1, rele2;
        for (TemporalRelation relation : this.relations){
            rele1 = relation.getTemporalElement1();
            rele2 = relation.getTemporalElement2();
            if (rele1.equals(e1) && rele2.equals(e2))
                return relation;
            else if (rele1.equals(e2) && rele2.equals(e1))
                return relation.inverse();
//            if (rele1.equals(e1) && rele2.equals(e2) || (rele1.equals(e2) && rele2.equals(e1)))
//                return relation;
            
        }
        return null;
    }
    
    
    /**
     * Replaces a relation by another in the graph
     * @param oldRelation the old relation
     * @param newRelation the new relation
     * @throws documentanalysis.temporality.TemporalConsistencyException if a consistency problem
     *  occurs in the graph with the new relation
     */
    public void replaceRelation(TemporalRelation oldRelation, TemporalRelation newRelation) throws TemporalConsistencyException{
        if (oldRelation == null || newRelation == null)
            throw new NullPointerException();
        TemporalRelation relation2;
        for (int i = 0; i < relations.size(); i++) {
            relation2 = relations.get(i);
            if (oldRelation.same(relation2)){
                relations.remove(i);
                relations.add(newRelation);
                oldRelation.getTemporalElement1().replaceTemporalRelation(oldRelation, newRelation);
                oldRelation.getTemporalElement2().replaceTemporalRelation(oldRelation, newRelation);
                return;
            }
        }
        this.addRelation(newRelation);
    }
    
    
    
    /**
     * Adds a TemporalRelation into the graph.
     * @param relation the relation to add.
     * @return 1 if the relation has been actually added, 0 otherwise
     * @throws documentanalysis.temporality.TemporalConsistencyException if a consistency problem
     *  occurs in the graph with the new relation
     */
    public int addRelation(TemporalRelation relation) throws TemporalConsistencyException{
        TemporalElement element1 = relation.getTemporalElement1();
        TemporalElement element2 = relation.getTemporalElement2();
        
        if (relation == null || element1 == null || element2 == null)
            throw new NullPointerException();
        
        if (element1.equals(element2))
            return 0;
        
        TemporalRelation oldRelation = this.getRelation(element1,
                element2);
        
        if (oldRelation != null){
            TemporalRelation newRelation = null;
            try {
                
                newRelation = TemporalRelation.checkRelation(oldRelation, relation);
                if (!oldRelation.equals(newRelation)) {
                    this.replaceRelation(oldRelation, newRelation);
                    return 1;
                } else {
                    return 0;
                }
            } catch (TemporalConsistencyException e) {
                newRelation = TemporalRelation.getHighestPriorityRelation(oldRelation, relation);
                if (newRelation == null)
                    throw e;
                else{
                    int done = 0;
                    if (!oldRelation.equals(newRelation)) {
                        this.replaceRelation(oldRelation, newRelation);
                        done = 1;
                    }
                    CommonLog.warningMessage("Relations \"" + oldRelation.toString() + "\" and \"" +
                            relation.toString() + "\" present a temporal conflict. Kept: \"" +
                            newRelation.toString() + "\" (Priority = " + newRelation.getPriority() + ").");
                    return done;
                }
            }
        } else {
            relation.getTemporalElement1().addTemporalRelation(relation);
            relation.getTemporalElement2().addTemporalRelation(relation);
            relations.add(relation);
            TemporalElement event1 = relation.getTemporalElement1();
            TemporalElement event2 = relation.getTemporalElement2();
            event1.addTemporalRelation(relation);
            event2.addTemporalRelation(relation);
            this.addTemporalElement(event1);
            this.addTemporalElement(event2);
            return 1;
        }
    }
    
    
    /**
     * Get transitive closure of the graph by composing pairs of relations
     *
     * @see documentAnalysis.temporality.TemporalRelation#compose(TemporalRelation relation)
     * @see #getTemporalClosure(boolean withInference)
     * @see #getTemporalClosure(boolean withInference, int compositionLevel,
            boolean throwConsistencyException)
     * @throws documentanalysis.temporality.TemporalConsistencyException if a consistency problem
     *  occurs in the graph with the new relations. Do not use any inference technique.
     */
    public void getTemporalClosure() throws TemporalConsistencyException{
        getTemporalClosure(false);
    }
    
    /**
     * Get transitive closure of the graph by composing pairs of relations
     *
     * @param withInference a boolean specifying if inference calculus should be
     * ran (true) or not (false)
     * @see #getTemporalClosure(boolean withInference, int compositionLevel, boolean throwConsistencyException)
     * @throws documentanalysis.temporality.TemporalConsistencyException if a consistency problem
     *  occurs in the graph with the new relations
     */
    public void getTemporalClosure(boolean withInference) throws TemporalConsistencyException{
        getTemporalClosure(withInference, 0, true);
    }
    
    
    /**
     * Get transitive closure of the graph by composing pairs of relations
     *
     * @param compositionLevel the maximum number of sentences for which the
     *   composition of temporal relations is calculated (0 = infinite)
     * @see #getTemporalClosure(boolean withInference, int compositionLevel,
            boolean throwConsistencyException)
     * @throws documentanalysis.temporality.TemporalConsistencyException if a consistency problem
     *  occurs in the graph with the new relations
     */
    public void getTemporalClosure(int compositionLevel) throws TemporalConsistencyException{
        getTemporalClosure(false, compositionLevel, true);
    }
    
    
    /**
     * Get transitive closure of the graph by composing pairs of relations
     * @param withInference a boolean specifying if inference calculus should be
     * ran (true) or not (false)
     * @param compositionLevel the maximum number of sentences for which the
     *   composition of temporal relations is calculated (0 = infinite)
     * @param throwConsistencyException true whether the method should try
     *  to solve a consistency problem before throwing the exception, false otherwise
     * @throws documentAnalysis.temporality.TemporalConsistencyException if a consistency problem
     *  occurs in the graph with the new relations
     * @see documentAnalysis.temporality.TemporalRelation#compose(TemporalRelation relation)
     */
    public void getTemporalClosure(boolean withInference,
            int compositionLevel,
            boolean throwConsistencyException) throws TemporalConsistencyException{
        TemporalRelation relation1 = null;
        TemporalRelation relation2 = null;
        TemporalRelation newRel = null;
        int count = 0;
        Vector<TemporalRelation> newRelations = new Vector<TemporalRelation>();
        Vector<TemporalRelation> reinjectedRelations =  new Vector<TemporalRelation>();
        reinjectedRelations.addAll(this.relations);
        int relationNumber = relations.size();
        int reinjectedRelationNumber = relationNumber;
        
        while (!reinjectedRelations.isEmpty()){
            count++;
            for (int i = 0; i < reinjectedRelationNumber ; ++i) {
                relation1 = reinjectedRelations.get(i);
                if (relation1.isEnabled()){
                    
//                    ++test;
//                    System.err.println("test = " + test );
//                    if (test == 18)
//                        System.err.println("");
//
                    if (withInference) {
                        TemporalRelation auxRelation = relation1.refine();
                        if (!auxRelation.equals(relation1)){
                            newRelations.add(auxRelation);
                            relation1 = auxRelation;
                        }
                    }
                    
                    for (int j = 0 ; j < relationNumber ; ++j) {
                        relation2 = relations.get(j);
                        if (relation1.equals(relation2))
                            continue;
                        if (relation2.isEnabled()){
                            /**********************
                             * Simple composition
                             **********************/
//                            ++test;
//                            System.err.println("test = " + test );
//                            if (test == 991){
//                                System.err.println("");
//                            }
//                            if (relation1.toString().equals("DURING(are, night)") &&
//                                    relation2.toString().equals("DURING(are, (Friday) and (Sunday))")){
//                                System.err.println("");
//                                System.err.println(relation2);
//                            }
                            
                            try {
                                newRel = relation1.compose(relation2, withInference, compositionLevel);
                                if (newRel != null){
//                                    System.err.println("newRel = " + newRel );
//                ++test;
//                System.err.println("test = " + test );
//                if (test == 16)
//                System.err.println("test = " + test );
//                if (newRel.toString().equals("DURING(VERB, 2007 April 19 (Thursday) and 2007 April 19 (Thursday))")){
//                    System.err.println("");
//                System.err.println("relation1 = " + relation1 );
//                System.err.println("relation2 = " + relation2 );
//                                }
//                                    if (newRel.equals("EQUALS((Friday) night and (Friday) night and (Sunday) night and (Sunday) night, (Friday) night and (Sunday) night)")){
//                                                        System.err.println("relation1 = " + relation1 );
//                System.err.println("relation2 = " + relation2 );
//
//                                    }
                                    
                                }
                            } catch (TemporalConsistencyException ex) {
                                if (throwConsistencyException)
                                    throw ex;
                                else
                                    CommonLog.warningMessage(ex.getMessage());
                            }
                            
                            if (newRel != null && newRel.getRelation() != TemporalRelation.UNDEFINED){
                                if (withInference)
                                    newRel = newRel.refine();
                                
                                
//
//                                    if (newRel.toString().equals("EQUALS((Friday) night and (Sunday) night, night)")){
//                                        System.err.println(newRel);
//                                        System.err.println(relation1);
//                                        System.err.println(relation2);
//                                    }
                                
                                // if the found relation concern the ST and a vague date
                                //   we don't care.
                                if ((newRel.getTemporalElement1().equals(this.getSpeechTime()) &&
                                        !(newRel.getTemporalElement2() instanceof Event)) ||
                                        (newRel.getTemporalElement2().equals(this.getSpeechTime()) &&
                                        !(newRel.getTemporalElement1() instanceof Event))) {
                                    
                                } else
                                    newRelations.add(newRel);
                            }
                        }
//                        relations.set(j, relation2);
                    }
                }
//                relations.set(i, relation1);
            }
            
            
            reinjectedRelations.clear();
            for (TemporalRelation newRelation : newRelations){
                try {
                    
//                                ++test;
//                                System.err.println("test = " + test );
//                                if (test == 9) {
////
//                                System.err.println("newRelation: " + newRelation);
////                                System.err.println("relation2: " + relation2);
//                                }
                    
//                    this.addRelation(newRelation);
                    
                    // CONVENTION
                    // EQUALS between two dates that have the same XipNode
                    //  this means that the first date must replace the second date
                    //   (inferenceComposition)
                    if (newRelation.getRelation() == TemporalRelation.EQUALS &&
                            newRelation.getTemporalElement1() instanceof VagueDate &&
                            newRelation.getTemporalElement2() instanceof VagueDate &&
                            newRelation.getTemporalElement1().getXipNode() != null &&
                            newRelation.getTemporalElement2().getXipNode() != null &&
                            newRelation.getTemporalElement1().getXipNode().equals(
                            newRelation.getTemporalElement2().getXipNode()))
                        this.replaceDate((VagueDate)newRelation.getTemporalElement2(),
                                (VagueDate)newRelation.getTemporalElement1());
                    else
                        if (this.addRelation(newRelation) == 1)
                            reinjectedRelations.add(newRelation);
                } catch (TemporalConsistencyException ex) {
                    if (throwConsistencyException){
//                        System.err.println(newRelation.toString());
                        throw ex;
                    } else {
                        CommonLog.warningMessage(ex.getMessage());
                    }
                }
            }
            newRelations.clear();
            relationNumber = this.relations.size();
            reinjectedRelationNumber = reinjectedRelations.size();
            CommonLog.infoMessage("Passage number " + count + ": relations added = " + reinjectedRelationNumber);
            
        }
        CommonLog.infoMessage("End of graph closure: new count = " + relationNumber);
    }
    
    /**
     * log in base 2
     */
    private static double log2(double d) {
        return Math.log(d) / Math.log(2);
    }
    
    
    /**
     * Return a vector containing all minimal graphs of the current one.
     *
     * @return a Vector containing all minimal graphs of the current one.
     */
    public Vector<TemporalGraph> getMinimalGraphs() throws TemporalConsistencyException {
        this.getTemporalClosure();
        
        Vector<TemporalGraph> result = new Vector<TemporalGraph>();
        
        
        // builds derivation map
        HashMap<TemporalRelation, Vector<TemporalRelation>> derivationMap =
                new HashMap<TemporalRelation, Vector<TemporalRelation>>();
        
        TemporalElement element1;
        TemporalElement element2;
        
        for (TemporalRelation relation : this.getRelations()) {
            element1 = relation.getTemporalElement1();
            element2 = relation.getTemporalElement2();
            // builds derivation list for this relation
            // example of derivation list of a relation e1-e2 :
            //    composition(e1-e3, e3-e2) inter composition(e1-e4, e4-e2) inter composition(e1-e5, e5-e2), ...
            Vector<TemporalRelation> element1Relations = element1.getTemporalRelations();
            Vector<TemporalRelation> element2Relations = element2.getTemporalRelations();
            Vector<TemporalRelation> composedRelations = new Vector<TemporalRelation>();
            for (TemporalRelation relation1 : element1Relations) {
                if (!relation1.equals(relation)) {
                    for (TemporalRelation relation2 : element2Relations) {
                        if (!relation2.equals(relation)){
                            relation2 = relation2.inverse();
                            // link
                            if (relation1.getTemporalElement2().equals(relation2.getTemporalElement1())) {
                                TemporalRelation composedRelation = new TemporalRelation(
                                        element1, element2,
                                        TemporalRelation.getComposedRelation(relation1.getRelation(),
                                            relation2.getRelation()));
//                                if (!composedRelations.contains(composedRelation))
                                composedRelations.add(composedRelation);
                            }
                        }
                    }
                }
                derivationMap.put(relation, composedRelations);
            }
        }
        
        // find relations that will appear in every minimal graph
        Vector<TemporalRelation> majorRelations = new Vector<TemporalRelation>();
        // they are the ones that are not found by the derivation table
        for (Map.Entry<TemporalRelation, Vector<TemporalRelation>> derivation : derivationMap.entrySet()) {
            Vector<TemporalRelation> derivatedRelations = derivation.getValue();
            Vector<Integer> integerRelations = new Vector<Integer>();
            for (TemporalRelation derivatedRelation : derivatedRelations) {
                integerRelations.add(new Integer(derivatedRelation.getRelation()));
            }
            int relationFromDerived = TemporalRelation.getIntersection(integerRelations);
            if (relationFromDerived != derivation.getKey().getRelation()) {
                majorRelations.add(derivation.getKey());
            }
        }
        
        // build a temporal graph with only the major relations
        TemporalGraph witnessGraph = new TemporalGraph();
        for (TemporalRelation majorRelation : majorRelations) {
            witnessGraph.addRelation(majorRelation);
        }
        
        // check that the witness is not enough to build the temporal closure
        TemporalGraph testGraph = witnessGraph.clone();
        testGraph.getTemporalClosure();
        if (this.equals(testGraph)) {
            // if yes, this is the unique minimal graph
            result.add(witnessGraph);
        } else {
            
            // get all combinations of minor relations
            Vector<Vector<TemporalRelation>> combinations = new Vector<Vector<TemporalRelation>>();
            
            for (TemporalRelation relation : this.getRelations()) {
                if (!majorRelations.contains(relation)) {
                    if (relation.getRelation() != TemporalRelation.EQUALS &&
                            !(relation.getTemporalElement1() instanceof VagueDate ||
                            relation.getTemporalElement2() instanceof VagueDate)) {
                        // for all combinations, create a clone where this relation is added
                        Vector<Vector<TemporalRelation>> newCombinations = new Vector<Vector<TemporalRelation>>();
                        for (Vector<TemporalRelation> combination : combinations) {
                            @SuppressWarnings("unchecked")
                            Vector<TemporalRelation> newCombination = (Vector<TemporalRelation>)combination.clone();
                            newCombination.add(relation);
                            newCombinations.add(newCombination);
                        }
                        // also add this relation alone
                        Vector<TemporalRelation> relations1 = new Vector<TemporalRelation>();
                        relations1.add(relation);
                        combinations.add(relations1);
                        combinations.addAll(newCombinations);
                    }
                }
            }
            
            Vector<Vector<TemporalRelation>> workingCombinations = new Vector<Vector<TemporalRelation>>();
            boolean keepGoing;
            
            // check all combinations
            for (Vector<TemporalRelation> combination : combinations) {
                // check that a working combination does not include this one
                keepGoing = true;
                for (Vector<TemporalRelation> workingCombination : workingCombinations) {
                    if (combination.containsAll(workingCombination))
                        keepGoing = false;
                }
                // if a working combination does not include this one
                // add it to the graph, close it and check that we obtain a temporal closure
                if (keepGoing) {
                    TemporalGraph newGraph = witnessGraph.clone();
                    for (TemporalRelation relation : combination) {
                        newGraph.addRelation(relation);
                    }
                    newGraph.getTemporalClosure();
                    if (this.equals(newGraph)) {
                        workingCombinations.add(combination);
                    }
                }
            }
            
            // build all graphs
            for (Vector<TemporalRelation> workingCombination : workingCombinations) {
                TemporalGraph graph = witnessGraph.clone();
                for (TemporalRelation relation : workingCombination) {
                    graph.addRelation(relation);
                }
                result.add(graph);
            }
        }
        
        return result;
    }
    
    
    /**
     * Returns an HTML representation of the graph
     * @param verbose true for more information
     * @return the HTML representation of the graph
     */
    public String toHTML(boolean verbose){
        String result = "";
        String uid1;
        String uid2;
        result += "<TABLE>\n";
        int count = 0;
        boolean disj;
        for (TemporalRelation rel : this.getRelations()){
            if (rel.isEnabled()){
                TemporalElement e1 = rel.getTemporalElement1();
                TemporalElement e2 = rel.getTemporalElement2();
                
                if (rel.isADisjunction())
                    disj = true;
                else
                    disj = false;
                
                // if not verbose, skip relations between two dates
                if (!verbose && e1 instanceof VagueDate && e2 instanceof VagueDate)
                    continue;
                
                ++count;
                if (count % 2 == 1)
                    result += "  <TR><TD>\n";
                else
                    result += "  <TD>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</TD><TD>\n";
                uid1 = e1.getUid();
                uid2 = e2.getUid();
                if (uid1 != null)
                    uid1 = uid1.replaceAll("/", "_").replaceAll("\\.", "").replaceAll("-", "_");
                else
                    uid1 = "speechtime";
                if (uid2 != null)
                    uid2 = uid2.replaceAll("/", "_").replaceAll("\\.", "").replaceAll("-", "_");
                else
                    uid2 = "speechtime";
                
                if (disj)
                    result += "<font color=\"gray\">";
                else
                    result += "<font color=\"red\">";
                result += "<span onclick=\"selec(" + uid1 + ", 'blue', 1);selec(" + uid2 + ", 'red', 0)\">";
                result += TemporalRelation.relationTypeToString(rel.getRelation());
                result += "</span>";
                result += "(";
                result += "<span onclick=\"selec(" +
                        uid1 + ", 'blue', 1)\">" + e1 + "</span>";
                result += ", ";
                result += "<span onclick=\"selec(" +
                        uid2 + ", 'red', 1)\">" + e2 + "</span>";
                result += ")";
                
                result += "</font>";
                result += "  </TD>\n";
                
                // INTERVAL
                result += "<TD>";
                if (rel.getInterval().isDefined() && !rel.getInterval().isNull())
                    result += "interval: " + rel.getInterval().toString();
                result += "</TD>";
                
                if (!rel.getComment().equals("")){
                    result += "<TD><font color=\"gray\"><i>(" + rel.getComment() + ")</i></font></TD>";
                }
                if (count % 2 == 0)
                    result += "<TR/>\n";
                else
                    result += "\n";
            }
        }
        result += "</TABLE>\n";
        return result;
    }
    
    /**
     * Returns a String representation of the graph
     * @return a String representation of the graph
     */
    public String toString(){
        String result = "";
        for (TemporalRelation rel : this.getRelations()){
            result += rel.toString() + ", ";
        }
        return result;
    }
    
//    public Vector<Pair<VagueDate, VagueDate> > getDateReferences(){
//        return this.dateReferences;
//    }
//
//    public void addDateReference(Pair<VagueDate, VagueDate> reference) {
//        this.dateReferences.add(reference);
//    }
    
    /**
     * Returns the Stack containing all candidates for being reference time.
     * @return the Stack of all reference times.
     */
    public Stack<Event> getReferenceTimes() {
        return referenceTimes;
    }
    
    
    
    
    
    /**
     * Adds a candidate for being reference time
     * @param referenceTime the event to add
     */
    public void addReferenceTime(Event referenceTime) {
        if (this.referenceTimes == null)
            this.referenceTimes = new Stack<Event>();
        if (!this.events.contains(referenceTime))
            this.addEvent(referenceTime);
        if (this.referenceTimes.empty() || !this.referenceTimes.peek().equals(referenceTime))
            this.referenceTimes.push(referenceTime);
    }
    
    private static int fibo(int n) {
        if (n <= 0)
            return 0;
        else
            return n + fibo(n-1);
    }
    
    
    private static TemporalGraph getRandomFullGraph(int eventNumber, boolean equalsAllowed) {
        TemporalGraph graph = new TemporalGraph();
        Random random = new Random();
        
        Vector<VagueInterval> intervalList = new Vector<VagueInterval>();
        boolean go;
        
//        Vector<Event> events = new Vector<Event>();
        for (int i = 1 ; i <= eventNumber ; ++i) {
            Event e = new Event("e" + (new Integer(i)).toString());
            int i1;
            int i2;
            
            try {
                VagueInterval date;
                do {
                    do {
                        i1 = random.nextInt(10);
                        i2 = random.nextInt(10);
                    } while (i2 <= i1);
                    
                    VagueSimpleDate date1 = new VagueSimpleDate("+20000101T12000" + i1 + "-O---W---");
                    VagueSimpleDate date2 = new VagueSimpleDate("+20000101T12000" + i2 + "-O---W---");
                    date = new VagueInterval(date1, date2);
                    go = true;
                    if (!equalsAllowed) {
                        if (intervalList.contains(date))
                            go = false;
                        else
                            intervalList.add(date);
                    }
                } while (!go);
                
                TemporalRelation relation = new TemporalRelation(e, date, TemporalRelation.EQUALS);
                graph.addRelation(relation);
                
//                events.add(e);
            } catch (BadFormatException ex) {
                ex.printStackTrace();
                
            } catch (IntervalException ex) {
                ex.printStackTrace();
            } catch (TemporalConsistencyException ex) {
                ex.printStackTrace();
            }
            
            
        }
        try {
            graph.getTemporalClosure();
            
            // recompose a new graph with only interesting relations
            TemporalGraph newGraph = new TemporalGraph();
            Vector<Event> events = new Vector<Event>();
            for (int i = 1 ; i <= eventNumber ; ++i) {
                Event e = new Event("e" + (new Integer(i)).toString());
                events.add(e);
            }
            
            for (TemporalRelation relation : graph.getRelations()) {
                if (relation.getTemporalElement1() instanceof Event &&
                        relation.getTemporalElement2() instanceof Event) {
                    
                    Event event1 = (Event)relation.getTemporalElement1();
                    Event event2 = (Event)relation.getTemporalElement2();
                    
                    for (Event e : events) {
                        if (e.getComment().equals(event1.getComment())) {
                            event1 = e;
                        }
                        if (e.getComment().equals(event2.getComment())) {
                            event2 = e;
                        }
                    }
                    TemporalRelation newRelation = new TemporalRelation(event1, event2,
                            relation.getRelation());
                    newGraph.addRelation(newRelation);
                }
            }
            
            
            return newGraph;
        } catch (TemporalConsistencyException ex) {
            ex.printStackTrace();
            return null;
        }
        
    }
    
    
    private static TemporalGraph getRandomGraph(int eventNumber, int relationNumber) {
        TemporalGraph graph = new TemporalGraph();
        
        Vector<Integer> possibleRelations = new Vector<Integer>();
        possibleRelations.add(new Integer(TemporalRelation.BEFORE));
        possibleRelations.add(new Integer(TemporalRelation.AFTER));
        possibleRelations.add(new Integer(TemporalRelation.OVERLAPS));
        possibleRelations.add(new Integer(TemporalRelation.OVERLAPPED_BY));
        possibleRelations.add(new Integer(TemporalRelation.DURING));
        possibleRelations.add(new Integer(TemporalRelation.INCLUDES));
        possibleRelations.add(new Integer(TemporalRelation.EQUALS));
        possibleRelations.add(new Integer(TemporalRelation.BEFORE | TemporalRelation.OVERLAPS));
        possibleRelations.add(new Integer(TemporalRelation.BEFORE | TemporalRelation.OVERLAPS | TemporalRelation.DURING));
        possibleRelations.add(new Integer(TemporalRelation.BEFORE | TemporalRelation.OVERLAPS | TemporalRelation.INCLUDES));
        possibleRelations.add(new Integer(TemporalRelation.BEFORE | TemporalRelation.OVERLAPS | TemporalRelation.DURING | TemporalRelation.EQUALS));
        possibleRelations.add(new Integer(TemporalRelation.BEFORE | TemporalRelation.OVERLAPS | TemporalRelation.INCLUDES | TemporalRelation.EQUALS));
        possibleRelations.add(new Integer(TemporalRelation.AFTER | TemporalRelation.OVERLAPPED_BY));
        possibleRelations.add(new Integer(TemporalRelation.AFTER | TemporalRelation.OVERLAPPED_BY | TemporalRelation.DURING));
        possibleRelations.add(new Integer(TemporalRelation.AFTER | TemporalRelation.OVERLAPPED_BY | TemporalRelation.INCLUDES));
        possibleRelations.add(new Integer(TemporalRelation.AFTER | TemporalRelation.OVERLAPPED_BY | TemporalRelation.DURING | TemporalRelation.EQUALS));
        possibleRelations.add(new Integer(TemporalRelation.AFTER | TemporalRelation.OVERLAPPED_BY | TemporalRelation.INCLUDES | TemporalRelation.EQUALS));
        possibleRelations.add(new Integer(TemporalRelation.OVERLAPS | TemporalRelation.DURING));
        possibleRelations.add(new Integer(TemporalRelation.OVERLAPS | TemporalRelation.INCLUDES));
        possibleRelations.add(new Integer(TemporalRelation.OVERLAPS | TemporalRelation.DURING | TemporalRelation.EQUALS));
        possibleRelations.add(new Integer(TemporalRelation.OVERLAPS | TemporalRelation.INCLUDES | TemporalRelation.EQUALS));
        possibleRelations.add(new Integer(TemporalRelation.OVERLAPPED_BY | TemporalRelation.DURING));
        possibleRelations.add(new Integer(TemporalRelation.OVERLAPPED_BY | TemporalRelation.INCLUDES));
        possibleRelations.add(new Integer(TemporalRelation.OVERLAPPED_BY | TemporalRelation.DURING | TemporalRelation.EQUALS));
        possibleRelations.add(new Integer(TemporalRelation.OVERLAPPED_BY | TemporalRelation.INCLUDES | TemporalRelation.EQUALS));
        
        Random random = new Random();
        
        Vector<Event> events = new Vector<Event>();
        for (int i = 1 ; i <= eventNumber ; ++i) {
            Event e = new Event("e" + (new Integer(i)).toString());
            events.add(e);
        }
        
        
        do {
            Event event1;
            Event event2;
            // get events
            try{
                // random events
                if (relationNumber != fibo(eventNumber-1)) {
                    do {
                        event1 = events.get(random.nextInt(events.size()));
                        event2 = events.get(random.nextInt(events.size()));
                    } while (event1.equals(event2));
                    // get relation
                    if (!graph.hasRelation(event1, event2)) {
                        int rel = possibleRelations.get(random.nextInt(possibleRelations.size())).intValue();
                        TemporalRelation relation = new TemporalRelation(event1, event2, rel);
//                        System.err.println("Relation found");
                        graph.addRelation(relation);
                    } else {
                        System.err.println("Look for another relation");
                    }
                } else {
                    return getRandomFullGraph(eventNumber, false);
                }
            } catch(documentAnalysis.temporality.TemporalConsistencyException e) {
//                e.printStackTrace();
                graph.relations.clear();
            }
        } while (graph.relations.size() < relationNumber);
        
        return graph;
    }
    
    
    private static TemporalGraph getManualGraph() throws TemporalConsistencyException {
        TemporalGraph graph = new TemporalGraph();
        
        Event e1 = new Event("e1");
        Event e2 = new Event("e2");
        Event e3 = new Event("e3");
        Event e4 = new Event("e4");
        Event e5 = new Event("e5");
        
        TemporalRelation relation12 = new TemporalRelation(e1, e2, TemporalRelation.AFTER, "r12");
        TemporalRelation relation23 = new TemporalRelation(e2, e3, TemporalRelation.BEFORE | TemporalRelation.OVERLAPS | TemporalRelation.DURING | TemporalRelation.INCLUDES | TemporalRelation.EQUALS, "r23");
        TemporalRelation relation34 = new TemporalRelation(e3, e4, TemporalRelation.BEFORE | TemporalRelation.OVERLAPS | TemporalRelation.DURING, "r34");
        TemporalRelation relation45 = new TemporalRelation(e4, e5, TemporalRelation.OVERLAPPED_BY, "r45");
        
        TemporalRelation relation25 = new TemporalRelation(e2, e5, TemporalRelation.BEFORE | TemporalRelation.OVERLAPS, "r25");        
        TemporalRelation relation14 = new TemporalRelation(e1, e4, TemporalRelation.INCLUDES, "r14");                
        
        graph.addRelation(relation12);
        graph.addRelation(relation23);
        graph.addRelation(relation34);
        graph.addRelation(relation45);        
        
        graph.addRelation(relation25);                
        graph.addRelation(relation14);                        
//        // major relations
//        TemporalRelation relation13 = new TemporalRelation(e1, e3, TemporalRelation.EQUALS, "r13");
//        TemporalRelation relation25 = new TemporalRelation(e2, e5, TemporalRelation.OVERLAPS, "r25");
////        TemporalRelation relation45 = new TemporalRelation(e4, e5, TemporalRelation.OVERLAPPED_BY, "r45");
//        TemporalRelation relation45 = new TemporalRelation(e4, e5, TemporalRelation.AFTER | TemporalRelation.OVERLAPPED_BY, "r45");
//        graph.addRelation(relation13);
//        graph.addRelation(relation25);
//        graph.addRelation(relation45);
//
//        // end of minimal graph
//        TemporalRelation relation12 = new TemporalRelation(e1, e2, TemporalRelation.AFTER, "r12");
//        TemporalRelation relation14 = new TemporalRelation(e1, e4, TemporalRelation.INCLUDES, "r14");
//        graph.addRelation(relation12);
//        graph.addRelation(relation14);
//
//        // more relations
//        TemporalRelation relation23 = new TemporalRelation(e2, e3, TemporalRelation.BEFORE | TemporalRelation.OVERLAPS, "r23");
//        graph.addRelation(relation23);
        return graph;
    }
    
    
    public static void main(String args[]) throws BadFormatException, TemporalConsistencyException {
        
        Vector<TemporalGraph> minimalGraphs = new Vector<TemporalGraph>();
        TemporalGraph graph = new TemporalGraph();
        int testNumber = 0;
//        do {
            try{
                System.err.println("Looking for a graph...");
//                graph = getRandomFullGraph(5, true);
                graph = getManualGraph();
                System.err.println("Graph Found...");
                System.err.println("graph = " + graph );
                
                minimalGraphs.clear();
                
                ++testNumber;
                System.err.println("testNumber = " + testNumber );
                minimalGraphs = graph.getMinimalGraphs();
            } catch(documentAnalysis.temporality.TemporalConsistencyException e) {
                e.printStackTrace();
                minimalGraphs.add(null);
            }
//        } while (minimalGraphs.size() <= 1);
        
        System.err.println("minimalGraphs = " + minimalGraphs.size() );
        System.err.println("original graph = " + graph.toString() );
        System.err.println("minimalGraphs = " + minimalGraphs );
        
//        graph.getTemporalClosure();
        
//        System.out.println(graph.toString());
        
//        System.err.println("Entropy: " + graph.getEntropy());
    }
}
