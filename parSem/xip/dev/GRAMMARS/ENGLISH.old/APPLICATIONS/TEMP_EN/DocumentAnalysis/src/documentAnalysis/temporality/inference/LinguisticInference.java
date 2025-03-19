/*
 * LinguisticInference.java
 *
 * Created on December 7, 2006, 2:16 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality.inference;


import common.GrammarErrorException;
import documentAnalysis.temporality.LostGranularityException;
import documentAnalysis.temporality.TemporalConsistencyException;
import documentAnalysis.temporality.TemporalConstituent;
import documentAnalysis.temporality.vaguecalendar.date.IntervalException;
import java.util.*;

import documentAnalysis.temporality.TempFunctions;
import documentAnalysis.temporality.vaguecalendar.date.VagueDate;
import documentAnalysis.temporality.vaguecalendar.BadFormatException;
import com.xerox.jxip.*;
import common.CommonLog;
import common.NotImplementedException;
import common.Options;
import common.Pair;
import common.TypedPropertyException;
import documentAnalysis.temporality.TemporalDictionary;
import documentAnalysis.temporality.TemporalElement;
import documentAnalysis.events.Event;
import documentAnalysis.temporality.TemporalGraph;
import documentAnalysis.temporality.TemporalOptions;
import documentAnalysis.temporality.TemporalRelation;
import documentAnalysis.temporality.vaguecalendar.date.VagueInterval;
import documentAnalysis.temporality.vaguecalendar.date.VagueSimpleDate;
import documentAnalysis.temporality.vaguecalendar.duration.Duration;
import documentAnalysis.temporality.vaguecalendar.duration.DurationUnit;
import documentAnalysis.temporality.vaguecalendar.duration.SimpleDuration;
import java.lang.reflect.Method;


/**
 *
 * @author xtannier
 */
public class LinguisticInference {
    
    private static TemporalDictionary hmTempDict;
    private static short currentCentury = 19;
    private static int newKeyNumber = 0;
    
    private static int test = 0;
    
    
    /** Creates a new instance of LinguisticInference */
    public LinguisticInference() {
    }
    
    
    /**
     * Returns the best candidate for being reference time of a specified node
     * in the temporal graph, according to the current unit.
     * @param unit the current unit
     * @param node the node
     * @param graph the temporal graph
     * @return a pair contaning the Event representing the reference time and the
     *  corresponding level of inference, depending on the way used to find the RT.
     */
    public static Pair<Event, Integer> getBestReferenceTime(XipUnit unit, XipNode node, TemporalGraph graph){
        Pair<Event, Integer> result = null;
        Vector<XipDependency> rtDependencies = node.getDependenciesByName("RT");
        
        for (XipDependency rtDependency : rtDependencies){
            result = new Pair<Event, Integer>(graph.getEvent(unit, rtDependency.getArg(1), true), new Integer(1));
        }
        if (result == null)
            result = new Pair<Event, Integer>(graph.getReferenceTimes().peek(), new Integer(0));;
            return result;
    }
    
    
    /**
     * Returns the best candidate for being reference date in the temporal graph,
     * according to the current unit.
     * @param unit      the current unit
     * @param node      the current node
     * @param reference the reference event is exists
     * @param graph     the temporal graph
     * @param methodName the name of a boolean method to test on candidates (can be null)
     * @param params    the parameters of the previous method
     * @return a VagueDate representing the reference date.
     */
    public static VagueDate getBestReferenceDate(XipUnit unit, XipNode node, Event reference,
            TemporalGraph graph,
            String methodName, Object[] params){
        Method method = null;
        Vector<TemporalElement> candidates = new Stack<TemporalElement>();
        
        if (methodName != null && params != null){
            Class classe = VagueDate.class;
            
            for (Method method2 : classe.getMethods()) {
                if (method2.getName().equals(methodName) &&
                        method2.getReturnType().toString().equals("boolean") &&
                        method2.getParameterTypes().length == params.length){
                    method = method2;
                    break;
                }
            }
            if (method == null){
                throw new NotImplementedException("The method VagueDate." + methodName + " is not implemented.");
            }
        }
        
        // event reference
        if (reference != null) {
            Vector<TemporalRelation> relations = reference.getTemporalRelations(TemporalRelation.DURING, -1);
            for (TemporalRelation relation : relations) {
                candidates.add(relation.getOtherElement(reference));
            }
        }
        
        
        // RT
        Vector<XipDependency> rtDependencies = node.getDependenciesByName("RT", 2);
        
        if (rtDependencies.isEmpty() && candidates.isEmpty()) {
            Stack<Event> referenceTimes = graph.getReferenceTimes();
            
            for (Event referenceTime : referenceTimes)
                candidates.add(0, referenceTime);
//            while (!referenceTimes.empty())
//                candidates.add(referenceTimes.pop());
        } else {
            for (XipDependency rtDependency : rtDependencies) {
                XipNode otherNode = rtDependency.getArg(1);
                if (rtDependency.hasFeature("date", "+")) {
                    Vector<VagueDate> dates = graph.getDates(unit, otherNode);
                    for (VagueDate date : dates)
                        candidates.add(0, date);
                } else
                    candidates.add(0, graph.getEvent(unit, otherNode, true));
            }
        }
        
        
        Vector<TemporalRelation> relations;
        Vector<VagueDate> auxResult = new Vector<VagueDate>();
        TemporalElement element;
        for (TemporalElement candidate : candidates) {
            if (candidate instanceof Event) {
                relations = candidate.getTemporalRelations();
                for (TemporalRelation relation : relations){
                    element = relation.getOtherElement(candidate);
                    if (element != null && element instanceof VagueDate){
                        if (methodName == null)
                            auxResult.add((VagueDate)element);
                        else{
                            try {
                                if (((Boolean) method.invoke((VagueDate)element, params)).booleanValue())
                                    auxResult.add((VagueDate)element);
                            } catch (java.lang.IllegalAccessException e) {
                                e.printStackTrace();
                                System.exit(-1);
                            } catch (java.lang.reflect.InvocationTargetException e) {
                                e.printStackTrace();
                                System.exit(-1);
                            }
                        }
                    }
                }
            } else if (candidate instanceof VagueDate) {
                if (methodName == null)
                    auxResult.add((VagueDate)candidate);
                else{
                    try {
                        if (((Boolean) method.invoke((VagueDate)candidate, params)).booleanValue())
                            auxResult.add((VagueDate)candidate);
                    } catch (java.lang.IllegalAccessException e) {
                        e.printStackTrace();
                        System.exit(-1);
                    } catch (java.lang.reflect.InvocationTargetException e) {
                        e.printStackTrace();
                        System.exit(-1);
                    }
                }
            } else {
                throw new NotImplementedException();
            }
        }
        
        // return the first absolute date candidate
        if (!auxResult.isEmpty()){
            for (VagueDate result : auxResult){
                if (result.isAbsolute())
                    return result;
            }
            return auxResult.get(0);
        }
        
        
        // if we are here, that's because we did'nt find a date
        //  if we had a method, try without
        //  else returned an undefined method
        if (method != null)
            return getBestReferenceDate(unit, node, reference, graph, null, null);
        else
            return graph.getDCT();
    }
    
    
    /**
     * Refines some information from XIP grammar
     *
     * @param unit the parsed sentence XipUnit
     * @param options the current options
     * @throws documentanalysis.temporality.TemporalConsistencyException when
     * a temporal inconsistency is detected.
     */
//    public static void refineGrammarRelations(XipUnit unit, Options options){
//        /******************
//         * COMPARATIVES
//         ******************/
//        // The gain left wages 3.8 percent higher than a year earlier.
//        // here "left" is BEFORE "a year earlier"
//        Vector<XipDependency> comparDependencies = unit.getDependenciesByName("COMPAR");
//        for (XipDependency comparDependency : comparDependencies){
//            // the first argument of COMPAR is always "THAN"
//            // check that the second one is a c_anchor tempexpr (COMPAR(than, a year earlier))
//            XipNode tempexprNode = comparDependency.getArg(2);
//            if (tempexprNode.hasFeature("tempexpr") && tempexprNode.hasFeature("c_anchor")){
//                Vector<XipDependency> modDependencies =
//                        comparDependency.getArg(1).getDependenciesByName("MOD", "comparativ");
//                for (XipDependency modDependency : modDependencies){
//                    // the second argument of modDependency is always THAN
//                    // (MOD[comparativ](higher, than))
//                    XipNode comparNode = modDependency.getArg(1);
//                    // ATTRIB(wages, higher)
//                    XipNode attribNode = comparNode.getDependentAhead("ATTRIB");
//                    if (attribNode != null){
//                        // OBJ-N(left, wages)
//                        XipNode verbNode = attribNode.getDependentAhead("OBJ-N");
//
//                    }
//                }
//            }
//        }
//    }
    
    /**
     * Gets a new VagueDate according to the value.
     *
     * @param value         the String formated representation of the date obtained by XIP
     * @return a VagueDate.
     * @see documentAnalysis.temporality.vaguecalendar.date.VagueDate#getNewDate(String value)
     */
    public static VagueDate getNewDate(String value) throws BadFormatException{
        
        /* DATE directly as collected by XIP */
        VagueDate date = VagueDate.getNewDate(value);
        
        /*************************
         * CHANGES
         *************************/
        
        
        
        // intervals
        // missing elements (upper granularities) must
        // be completed from a date to another
        // eg: from February to May 1988
        //  => from February 1988 to May 1988
        if (date instanceof VagueInterval) {
            VagueInterval idate = (VagueInterval)date;
            VagueSimpleDate beginDate = idate.getLowerBound();
            VagueSimpleDate endDate = idate.getUpperBound();
            beginDate = beginDate.completeDate(endDate).getLowerBound();
            endDate = endDate.completeDate(beginDate).getUpperBound();
            try{
                date = new VagueInterval(beginDate, endDate);
            } catch(IntervalException e) {
                
            }
        }
        
        // decade only: add century
        if (date.isCenturyMissing()){
            date = date.completeCentury(currentCentury);
        }
                
        return date;
    }
    
    
    /**
     * Completes dates when possible and not done
     *
     * @param options the current options
     */
    public static void completeDates(XipUnit unit, Options options) {
        TemporalGraph temporalGraph = options.getTemporalGraph();
        // get all EVENTs
        Vector<Event> events = temporalGraph.getEvents(unit.getSentenceNb());
        
        for (Event event : events) {
            // get all DURING relations
            Vector<TemporalRelation> relations = event.getTemporalRelations(TemporalRelation.DURING, -1);
            // store corresponding dates
            Vector<VagueDate> dates = new Vector<VagueDate>();
            for (TemporalRelation relation : relations) {
                TemporalElement otherElement = relation.getOtherElement(event);
                if (otherElement instanceof VagueDate) {
                    dates.add((VagueDate)otherElement);
                }
            }
            
            // there we have a number of dates having relation DURING
            // with the event
//            if
        }
    }
    
    /**
     *
     * @param unit the parsed sentence XipUnit
     * @param options the current options
     * @throws documentanalysis.temporality.TemporalConsistencyException when
     *      a temporal inconsistency is detected.
     */
    public static void storeEvents(XipUnit unit,
            Options options) throws TemporalConsistencyException {
        String fileName = options.getInputFileName();
        TemporalGraph graph = options.getTemporalGraph();
        
        boolean robust = false;
        try {
            robust = options.getBooleanProperty(TemporalOptions.TEMPORAL_ROBUST);
        } catch (TypedPropertyException ex) {
            CommonLog.errorMessage(ex.getMessage());
        }
        
        
        // ST-ORDER dependencies (unary dependency)
        //   temporal relation between an event and the Speech Time
        Vector<XipDependency> dependencies = unit.getDependenciesByName("ST-ORDER");
        
        
        for (XipDependency dep : dependencies){
            // get the event
            XipNode link = dep.getArg(1);
            Event e = graph.getEvent(unit, link, true);

            // TEMP_REL FEATURE ?
            String tempRel = dep.getFeature("temp_rel");
            if (tempRel == null)
                tempRel = dep.getFeature("v_temp_rel");
            if (tempRel != null) {
                int priority;
                if (tempRel.equals("CONCOMITANT") && link.hasDependency("TEMP", 1))
                    priority = TemporalRelation.DEFAULT_CHOICE;
                else
                    priority = TemporalRelation.TENSE;
                TemporalRelation relation = new TemporalRelation(e, graph.getSpeechTime(),
                        TemporalRelation.stringToRelationType(tempRel),
                        priority, "ST-ORDER relation");
                try{
                    graph.addRelation(relation);
                } catch(TemporalConsistencyException ex) {
                    if (robust)
                        CommonLog.warningMessage(ex.getMessage());
                    else
                        throw ex;
                }
            }
        }
        
        
        // ORDER dependencies
        dependencies = unit.getDependenciesByName("ORDER");
        
        for (XipDependency dep : dependencies){
            XipNode link = dep.getArg(1);
            XipNode node2 = dep.getArg(2);
            // TEMP_REL FEATURE
            String tempRel = dep.getFeature("temp_rel");
            // BETWEEN TWO EVENTS
            if (!link.hasFeature("tempexpr") && !node2.hasFeature("tempexpr")){
                
                // get the event
                Event e = graph.getEvent(unit, link, true);
                
                if (tempRel == null)
                    tempRel = dep.getFeature("v_temp_rel");
                
                if (tempRel != null) {
                    Event e2 = graph.getEvent(unit, node2, true);
                    // look for a DELTA dependency between these two events
                    Vector<XipDependency> deltaDependencies = link.getDependenciesByName("DELTA");
                    TemporalRelation relation = null;
                    // no delta found, create a temporal relation without interval
                    if (deltaDependencies.isEmpty())
                        relation = new TemporalRelation(e, e2,
                                TemporalRelation.stringToRelationType(tempRel),
                                TemporalRelation.GRAMMAR2,
                                "Relation between two events");
                    else{
                        boolean found = false;
                        // DELTA found with the first event ,
                        //  but not necessarily with the second event
                        for (XipDependency deltaDependency : deltaDependencies){
                            if (deltaDependency.getArg(2).equals(node2)){
                                // delta between the two events found
                                XipNode durationNode = deltaDependency.getArg(3);
                                // analyse the duration
                                HashMap<String, String> durationNodeInfo =
                                        TempFunctions.getTempNodeInfo(fileName, unit.getSentenceNb(),
                                        "DURATION", durationNode.getGramIdNode(), options);
                                if (durationNodeInfo != null){
                                    String value = durationNodeInfo.get("value");
                                    try{
                                        Duration duration = Duration.getNewDuration(value, unit, durationNode);
                                        relation = new TemporalRelation(e, e2,
                                                TemporalRelation.stringToRelationType(tempRel),
                                                duration, TemporalRelation.GRAMMAR2,
                                                "Relation between two events");
                                        found = true;
                                        break;
                                    } catch (BadFormatException ex) {
                                        ex.printStackTrace();
                                        System.exit(-1);
                                    }
                                } else {
                                    if (!robust)
                                        throw new GrammarErrorException("DELTA() 3rd argument should be a valid duration!");
                                    else
                                        CommonLog.warningMessage("DELTA() 3rd argument should be a valid duration! \n" +
                                                "Error occurred in sentence " + unit.getSentenceString());
                                }
                            }
                        }
                        if (!found)
                            relation = new TemporalRelation(e, e2,
                                    TemporalRelation.stringToRelationType(tempRel),
                                    TemporalRelation.GRAMMAR2, "Relation between two events");
                    }
                    if (relation != null){
                        try{
                            graph.addRelation(relation);
                        } catch(TemporalConsistencyException ex) {
                            if (robust)
                                CommonLog.warningMessage(ex.getMessage());
                            else
                                throw ex;
                        }
                    }
                }
            } else if (link.hasFeature("tempexpr") && node2.hasFeature("tempexpr")) {
                throw new NotImplementedException();
            }
            // first node is a tempexpr
            else if (link.hasFeature("tempexpr") && tempRel != null) {
                // ANCHOR DATE ?
                HashMap<String, String> tempNodeInfo = TempFunctions.getTempNodeInfo(fileName,
                        unit.getSentenceNb(), "ANCHOR",
                        link.getGramIdNode(), options);
                if (tempNodeInfo == null){
                    // DATE ?
                    tempNodeInfo = TempFunctions.getTempNodeInfo(fileName,
                            unit.getSentenceNb(), "DATE",
                            link.getGramIdNode(), options);
                }
                if (tempNodeInfo == null){
                    // SET ?
                    tempNodeInfo = TempFunctions.getTempNodeInfo(fileName,
                            unit.getSentenceNb(), "SET",
                            link.getGramIdNode(), options);                    
                }
                if (tempNodeInfo == null){
                    throw new NotImplementedException();
                }
                Event e = graph.getEvent(unit, node2, true);
                HashMap<String, String> anchorNodeInfo = new HashMap<String, String>();
                anchorNodeInfo.put("key", "ANCHOR");
                anchorNodeInfo.put("type", "ANCHOR");
                anchorNodeInfo.put("temp_rel", TemporalRelation.getInverseRelation(tempRel));
                anchorNodeInfo.put("temp_ref", "+");
                anchorNodeInfo.put("value", "P0Y0M0W0DT0H0M0S");
                anchorNodeInfo.put("anchored_to", tempNodeInfo.get("key"));
                try {
                    analyseAnchor(anchorNodeInfo, fileName, unit, e, e, link, false, options);
                } catch (BadFormatException ex) {
                    CommonLog.errorMessage(ex.getMessage());
                }
            } else if (node2.hasFeature("tempexpr") && tempRel != null) {
                // ANCHOR DATE ?
                HashMap<String, String> tempNodeInfo = TempFunctions.getTempNodeInfo(fileName,
                        unit.getSentenceNb(), "ANCHOR",
                        node2.getGramIdNode(), options);
                if (tempNodeInfo == null){
                    tempNodeInfo = TempFunctions.getTempNodeInfo(fileName,
                            unit.getSentenceNb(), "DATE",
                            node2.getGramIdNode(), options);
                }
                if (tempNodeInfo == null){
                    throw new NotImplementedException();
                }
                Event e = graph.getEvent(unit, link, true);
                HashMap<String, String> anchorNodeInfo = new HashMap<String, String>();
                anchorNodeInfo.put("key", "ANCHOR");
                anchorNodeInfo.put("type", "ANCHOR");
                anchorNodeInfo.put("temp_rel", tempRel);
                anchorNodeInfo.put("temp_ref", "+");
                anchorNodeInfo.put("value", "P0Y0M0W0DT0H0M0S");
                anchorNodeInfo.put("anchored_to", tempNodeInfo.get("key"));
                try {
                    analyseAnchor(anchorNodeInfo, fileName, unit, e, e, node2, false, options);
                } catch (BadFormatException ex) {
                    CommonLog.errorMessage(ex.getMessage());
                } catch (TemporalConsistencyException ex) {
                    try {
                        if (options.getBooleanProperty("TEMPORAL_ROBUST"))
                            CommonLog.warningMessage(ex.getMessage());
                        else
                            throw ex;
                    } catch (TypedPropertyException ex2) {
                        ex2.printStackTrace();
                        System.exit(-1);
                    }
                }
            } else
                throw new NotImplementedException();
            
        }
        
        
//
        
        
        /************
         * VERBS
         ************/
        Vector<XipNode> verbs = unit.getNodesByCategory("VERB");
        for (XipNode verb : verbs){
            if (!verb.isLeaf()){
                // get verb as an event
                Event e = graph.getEvent(unit, verb, false);
                
                
                // if the verb is not concerned by any ORDER, ST-ORDER, TEMP, ...
                // then we try to guess!
                if (e == null){
                    e = graph.getEvent(unit, verb, true);
                    
                    // to + inf
                    //   default is after the MAIN
                    // Quantas plans... to boost ...
//                    if (verb.getMother().getCategory().equals("IV")){
//                        Event closestMain = getClosestMain(unit, verb, graph);
//                        if (closestMain == null)
//                            closestMain = graph.getSpeechTime();
//                        else{
//                            try{
//                                graph.addRelation(new TemporalRelation(e, closestMain,
//                                        TemporalRelation.AFTER,
//                                        TemporalRelation.DEFAULT_CHOICE, "to + ... = AFTER"));
//                            } catch(TemporalConsistencyException ex) {
//                                if (robust)
//                                    CommonLog.warningMessage(ex.getMessage());
//                                else
//                                    throw ex;
//                            }
//                        }
//                    }
                }
                
                // if MAIN, add it to the reference times
                if (verb.hasDependency("MAIN"))
                    graph.addReferenceTime(e);
                
            }
        }
        
        
        /**************
         * TIME_SPAN_N
         **************/
        Vector<XipNode> nounEvents = unit.getNodesByCategory("NOUN", "time_span_n");
        for (XipNode nounEvent : nounEvents){
            if (!nounEvent.isLeaf()){
                // get noun as an event
                Event e = graph.getEvent(unit, nounEvent, false);
                
                // if the verb is not concerned by any ORDER, ST-ORDER, TEMP, ...
                // then we try to guess!
                if (e == null){
                    e = graph.getEvent(unit, nounEvent, true);
                    
                }
            }
        }
    }
    
    
    
    /**
     *
     * @param unit the parsed sentence XipUnit
     * @param options the current options
     * @throws documentanalysis.temporality.TemporalConsistencyException when
     * a temporal inconsistency is detected.
     */
    public static void storeDates(XipUnit unit,
            Options options) throws TemporalConsistencyException{
        String fileName = options.getInputFileName();
        TemporalGraph graph = options.getTemporalGraph();
        VagueDate dct = options.getDct();
        
        boolean robust = false;
        try {
            robust = options.getBooleanProperty(TemporalOptions.TEMPORAL_ROBUST);
        } catch (TypedPropertyException ex) {
            CommonLog.errorMessage(ex.getMessage());
        }
        
        try{
//                test++;
//                System.err.println("test2 = " + test );
            
            // TEMP dependencies
            Vector<XipDependency> dependencies = unit.getDependenciesByName("TEMP");
            
            dependencies = sortTEMPDependencies(dependencies);
            
            
//                System.err.println("test3 = " + test + " " + dependencies.size());
            
            String mod = null;
            boolean stop;
            boolean done;
            
            for (XipDependency dep : dependencies){
                stop = false;
                done = false;
                
                // get the event
                XipNode link = dep.getArg(1);
                
                Event e = graph.getEvent(unit, link, true);
                String value;
                HashMap<String, String> tempNodeInfo;
                
                XipNode tempNode = dep.getArg(2);
                
                try{
                    // DURATION ?
                    //   duration must be before others because sometimes
                    //    knowing the duration can help to restrict relations
                    tempNodeInfo = TempFunctions.getTempNodeInfo(fileName, unit.getSentenceNb(),
                            "DURATION", tempNode.getGramIdNode(), options);
                    if (tempNodeInfo != null){
                        /**********************
                         * DURATION
                         *  "for 3 weeks"
                         **********************/
                        value = tempNodeInfo.get("value");
                        Duration duration = Duration.getNewDuration(value, unit, tempNode);
                        e.setDuration(duration);
                        graph.addDuration(duration);
                        done = true;
                    }
                    
                    // ANCHOR DATE ?
                    tempNodeInfo = TempFunctions.getTempNodeInfo(fileName, unit.getSentenceNb(), "ANCHOR",
                            tempNode.getGramIdNode(), options);
                    if (tempNodeInfo != null){
                        analyseAnchor(tempNodeInfo, fileName, unit, e, e, tempNode, false, options);
                        stop = true;
                        done = true;
                    } // end ANCHOR
                    
                    
                    
                    // SET ?
                    // for the moment SETs are only considered when they can be used
                    //   to find a DATE reference
                    // if no anchored DATE, just a duration...
                    //   eg : each day of this month ==> this month
                    tempNodeInfo = TempFunctions.getTempNodeInfo(fileName, unit.getSentenceNb(),
                            "SET", tempNode.getGramIdNode(), options);
                    if (tempNodeInfo != null){
                        if (tempNodeInfo.get("temp_rel") != null)
                            analyseAnchor(tempNodeInfo, fileName, unit, e, e, tempNode, true, options);
                        else {
                            value = tempNodeInfo.get("value");
                            Duration duration = Duration.getNewDuration(value, unit, tempNode);
                            graph.addDuration(duration);
                            done = true;
                        }
//                    analyseSet(tempNodeInfo, fileName, unit, e, graph, tempNode, dct);
                        stop = true;
                        done = true;
                    }
                    
                    
                    // DATE ?
                    tempNodeInfo = TempFunctions.getTempNodeInfo(fileName, unit.getSentenceNb(),
                            "DATE", tempNode.getGramIdNode(), options);
                    
                    if (tempNodeInfo != null && !stop){
                        /***********************
                         * DATE
                         *  "Winter"
                         *  "September"
                         *  "2006"
                         ***********************/
                        HashMap<String, String> anchorNodeInfo = new HashMap<String, String>();
                        anchorNodeInfo.put("key", "ANCHOR");
                        anchorNodeInfo.put("type", "ANCHOR");
                        anchorNodeInfo.put("temp_rel", "DURING");
                        anchorNodeInfo.put("temp_ref", "+");
                        anchorNodeInfo.put("value", "P0Y0M0W0DT0H0M0S");
                        anchorNodeInfo.put("anchored_to", tempNodeInfo.get("key"));
                        analyseAnchor(anchorNodeInfo, fileName, unit, e, e, tempNode, false, options);
//                    value = tempNodeInfo.get("value");
//                    mod = tempNodeInfo.get("mod");
//                    VagueDate date = getNewDate(value, tempNode, unit, e, graph);
//                    graph.addDate(date);
//                    TemporalRelation relation = new TemporalRelation(e, date, TemporalRelation.DURING, "DATE");
//                    graph.addRelation(relation);
                        done = true;
                    } // end DATE
                    
                    
//                if (!done) {
//                    VagueDate newDate = VagueDate.getNewDate(unit, tempNode);
//                    graph.addRelation(new TemporalRelation(e, newDate, TemporalRelation.DURING,
//                            TemporalRelation.DEFAULT_CHOICE, "no interpretation found for the date"));
//                }
                    
                    
                } catch(TemporalConsistencyException ex) {
                    if (robust)
                        CommonLog.warningMessage(ex.getMessage());
                    else
                        throw ex;
                }
                
            }
        } catch (BadFormatException e) {
            e.printStackTrace();
            System.exit(-1);
        }
    }
    
    
    /**
     * Analyses information concerning an ANCHOR and updates the temporal graph
     */
    private static TemporalElement analyseSet(HashMap<String, String> tempNodeInfo,
            String fileName, XipUnit unit, Event e, TemporalGraph graph, XipNode tempNode, VagueDate dct,
            Options options)
            throws NotImplementedException, TemporalConsistencyException, BadFormatException {
        TemporalElement result = null;
        String tempRel = tempNodeInfo.get("temp_rel").toUpperCase();
        String tempRef = tempNodeInfo.get("temp_ref").toUpperCase();
        String anchoredTo = tempNodeInfo.get("anchored_to");
//        String value = tempNodeInfo.get("value");
        
        if (anchoredTo != null){
            /***************************************
             * ANCHORed to a reference date
             * "each day OF 2006"
             * "each day OF THIS MONTH"
             * "every Monday"
             ***************************************/
            TemporalElement newElement;
            
            tempNodeInfo = TempFunctions.getTempNodeInfo(fileName, unit.getSentenceNb(),
                    anchoredTo, options);
            newElement = analyseAnchor(tempNodeInfo, fileName, unit, e, null, null, true, options);
            
            if (e != null){
                int rel;
                if (tempRel == null){
                    Duration duration = e.getDuration();
                    if (newElement instanceof VagueDate &&
                            duration != null && !duration.isNull()){
                        int granularity = ((VagueDate)newElement).getGranularity();
                        Duration durationGranularity =
                                new SimpleDuration(DurationUnit.getDurationUnit(granularity, "1"));
                        if (duration.compareTo(durationGranularity) > 0)
                            // eg. these months > July 2006
                            rel = TemporalRelation.OVERLAPS;
                        else
                            // eg. this month and July 2006
                            rel = TemporalRelation.DURING;
                    } else
                        rel = TemporalRelation.DURING;
                } else
                    rel = TemporalRelation.stringToRelationType(tempRel);
                TemporalRelation relation = new TemporalRelation(e, newElement,
                        rel, TemporalRelation.TEMPORAL_EXPRESSION, "DATE");
                graph.addRelation(relation);
            } else
                result = newElement;
        }
        return result;
    }
    
    /**
     * Analyses information concerning an ANCHOR and updates the temporal graph
     */
    private static TemporalElement analyseAnchor(HashMap<String, String> tempNodeInfo,
            String fileName, XipUnit unit, Event originalEvent, Event e, XipNode tempNode,
            boolean fromSet,
            Options options)
            throws NotImplementedException, TemporalConsistencyException, BadFormatException {
        TemporalGraph graph = options.getTemporalGraph();
        VagueDate dct = options.getDct();
        TemporalElement result = null;
        // ANCHOR
        String tempRel = tempNodeInfo.get("temp_rel").toUpperCase();
        String tempRef = tempNodeInfo.get("temp_ref").toUpperCase();
        String anchoredTo = tempNodeInfo.get("anchored_to");
        String mod = tempNodeInfo.get("mod");
        String value = tempNodeInfo.get("value");
        Duration duration = Duration.getNewDuration(value, unit, tempNode);
        int inferenceLevel = 0;
        
        boolean robust = false;
        try {
            robust = options.getBooleanProperty(TemporalOptions.TEMPORAL_ROBUST);
        } catch (TypedPropertyException ex) {
            CommonLog.errorMessage(ex.getMessage());
        }
        
        
        // BETWEEN relation !
        if (tempRel.equalsIgnoreCase("BETWEEN")){
            // between must involve an anchored_to1 and an anchored_to2
            String anchoredTo1 = tempNodeInfo.get("anchored_to1");
            String anchoredTo2 = tempNodeInfo.get("anchored_to2");
            HashMap<String, String> anchorNodeInfo1 = new HashMap<String, String>();
            anchorNodeInfo1.put("key", "ANCHOR");
            anchorNodeInfo1.put("type", "ANCHOR");
            anchorNodeInfo1.put("temp_rel", "STARTS_WITH");
            anchorNodeInfo1.put("temp_ref", "+");
            anchorNodeInfo1.put("value", "P0Y0M0W0DT0H0M0S");
            anchorNodeInfo1.put("anchored_to", anchoredTo1);
            analyseAnchor(anchorNodeInfo1, fileName, unit, originalEvent, e, tempNode, fromSet, options);
            HashMap<String, String> anchorNodeInfo2 = new HashMap<String, String>();
            anchorNodeInfo2.put("key", "ANCHOR");
            anchorNodeInfo2.put("type", "ANCHOR");
            anchorNodeInfo2.put("temp_rel", "ENDS_WITH");
            anchorNodeInfo2.put("temp_ref", "+");
            anchorNodeInfo2.put("value", "P0Y0M0W0DT0H0M0S");
            anchorNodeInfo2.put("anchored_to", anchoredTo2);
            analyseAnchor(anchorNodeInfo2, fileName, unit, originalEvent, e, tempNode, fromSet, options);
            return null;
        }
        
        // BRANCH relation !
        // this relation is used when a single temporal expression is linked
        // to two different unmergeable date values (eg: "December of this year")
        if (tempRel.equalsIgnoreCase("BRANCH")){
            // between must involve an anchored_to1 and an anchored_to2
            String anchoredTo1 = tempNodeInfo.get("anchored_to1");
            String anchoredTo2 = tempNodeInfo.get("anchored_to2");
            HashMap<String, String> anchorNodeInfo1 = new HashMap<String, String>();
            anchorNodeInfo1.put("key", "ANCHOR");
            anchorNodeInfo1.put("type", "ANCHOR");
            anchorNodeInfo1.put("temp_rel", "DURING");
            anchorNodeInfo1.put("temp_ref", "+");
            anchorNodeInfo1.put("value", "P0Y0M0W0DT0H0M0S");
            anchorNodeInfo1.put("anchored_to", anchoredTo1);
            analyseAnchor(anchorNodeInfo1, fileName, unit, originalEvent, e, tempNode, fromSet, options);
            HashMap<String, String> anchorNodeInfo2 = new HashMap<String, String>();
            anchorNodeInfo2.put("key", "ANCHOR");
            anchorNodeInfo2.put("type", "ANCHOR");
            anchorNodeInfo2.put("temp_rel", "DURING");
            anchorNodeInfo2.put("temp_ref", "+");
            anchorNodeInfo2.put("value", "P0Y0M0W0DT0H0M0S");
            anchorNodeInfo2.put("anchored_to", anchoredTo2);
            return analyseAnchor(anchorNodeInfo2, fileName, unit, originalEvent, e,
                    tempNode, fromSet, options);
        }
        
        try {
            if (anchoredTo != null){
                /**********************************************
                 * ANCHOR DATE with an explicit anchor
                 **********************************************/
                /**********************************************
                 * with JUST_AFTER or JUST_BEFORE
                 * last October, next Monday...
                 **********************************************/
                if (tempRel.startsWith("JUST") && (!duration.isDefined() || duration.isNull())){
                    int intDirection;
                    if (tempRel.equals("JUST_AFTER"))
                        intDirection = 2;
                    else if (tempRel.equals("JUST_BEFORE"))
                        intDirection = -2;
                    else
                        intDirection = 0;
                    
                    HashMap<String, String> tempNewNodeInfo =
                            TempFunctions.getTempNodeInfo(fileName, unit.getSentenceNb(), anchoredTo, options);
                    
//                TemporalElement temp = analyseAnchor(tempNewNodeInfo,
//                        fileName, unit, originalEvent, e, tempNode, fromSet, options);
                    
                    VagueDate dateToGo;
                    if (tempNewNodeInfo.get("type").equalsIgnoreCase("DATE"))
                        dateToGo = getNewDate(tempNewNodeInfo.get("value"));
                    else if (tempNewNodeInfo.get("type").equalsIgnoreCase("ANCHOR"))
                        dateToGo = (VagueDate)analyseAnchor(tempNewNodeInfo,
                                fileName, unit, originalEvent, null, tempNode, fromSet, options);
                    else
                        throw new NotImplementedException();
                    
                    VagueDate dateToCompare;
                    if (tempRef.equals("S") && dct.isDefined()){
                        dateToCompare = dct;
                        inferenceLevel = TemporalRelation.TEMPORAL_EXPRESSION;
                    } else{
                        Object[] params = {};
                        dateToCompare = getBestReferenceDate(unit, tempNode, e, graph, "isAbsolute", params);
                        
                        if (dateToCompare == null ||
                                options.getIntProperty(TemporalOptions.TEMPORAL_INFERENCE_LEVEL) < 2)
                            dateToCompare = VagueDate.getNewDate();
                        
                        inferenceLevel = 2;
                    }
                    VagueDate newDate;
                    if (dateToGo != null || !robust)
                        newDate = dateToCompare.closest(dateToGo, intDirection);
                    else {
                        newDate = VagueDate.getNewDate();
                        CommonLog.warningMessage("Error/Exception, dateToGo is null in analyseAnchor for sentence \n" +
                                unit.getSentenceString());
                    }
                        
                    newDate.setXipNode(unit, tempNode);
                    if (e != null){
                        TemporalRelation relation = new TemporalRelation(e, newDate,
                                TemporalRelation.DURING,
                                duration, TemporalRelation.TEMPORAL_EXPRESSION, "DATE");
                        graph.addRelation(relation);
                    } else
                        result = newDate;
                    
                } else if (anchoredTo.startsWith("DATE")) {
                    /**********************************************
                     * ANCHOR DATE with an anchored_to DATE
                     *  "after September"
                     *  "in 2006"
                     * also with PRESENT and no duration (co-reference to a relative date)
                     *  "this Winter"
                     **********************************************/
                    tempNodeInfo = TempFunctions.getTempNodeInfo(fileName, unit.getSentenceNb(),
                            anchoredTo, options);
                    // get corresponding DATE
                    String anchorValue = tempNodeInfo.get("value");
                    long nodeNumber = Long.parseLong(tempNodeInfo.get("nodenumber"));
                    VagueDate date = getNewDate(anchorValue);
                    
                    date.setXipNode(unit, unit.getNodeByGramId(nodeNumber));
                    VagueDate referenceDate;
                    // choose reference time according to temp ref.
                    if (tempRef != null && tempRef.equals("S") && dct.isDefined()){
                        referenceDate = dct;
                        inferenceLevel = TemporalRelation.TEMPORAL_EXPRESSION;
                    } else {
                        referenceDate = getBestReferenceDate(unit, tempNode, e, graph, null, null);
                        if (referenceDate == null ||
                                options.getIntProperty(TemporalOptions.TEMPORAL_INFERENCE_LEVEL) < 2)
                            referenceDate = VagueDate.getNewDate();
                        inferenceLevel = 2;
                    }
                    int direction = 0;
                    
                    // RD has the same granularity than new one
                    //  e.g.: RD = "July 2006" and new date ND = "Winter" (gran = MONTH)
                    // If RD is included in ND or equal, complete
                    //  e.g.: RD = "July 2006" and ND = "Summer" => Summer 2006
                    // else, find new date with the help of the tense (if a verb)
                    //  e.g.: RD = "July 2006" and new date ND = "Winter" (gran = MONTH)
                    if (date.isDefined() && !fromSet) {
                        if (referenceDate.hasGranularity(date.getGranularity())) {
                            if (originalEvent != null && referenceDate.equals(dct)) {
                                if (originalEvent.getCategory().equals("VERB")){
                                    if (originalEvent.getXipNode().hasDependency("ST-ORDER", "temp_rel:before") ||
                                            originalEvent.getXipNode().hasDependency("ST-ORDER", "temp_rel:before_or_during"))
                                        direction = -1;
                                    else if (originalEvent.getXipNode().hasDependency("ST-ORDER", "temp_rel:concomitant") ||
                                            originalEvent.getXipNode().hasDependency("ST-ORDER", "temp_rel:after") ||
                                            originalEvent.getXipNode().hasDependency("ST-ORDER", "temp_rel:after_or_during"))
                                        direction = 1;
                                    else {
                                        String defaultRelation = options.getProperty(TempFunctions.DEFAULT_TEMPORAL_RELATION);
                                        if (defaultRelation == null)
                                            direction = 0;
                                        else if (defaultRelation.equalsIgnoreCase("AFTER"))
                                            direction = 1;
                                        else if (defaultRelation.equalsIgnoreCase("BEFORE"))
                                            direction = -1;
                                        else
                                            throw new NotImplementedException(defaultRelation + " is not a known default relation.");
                                    }
                                    // PRESENT => This Winter, etc, keep 1 or -1
                                    // if AFTER, BEFORE, JUST_AFTER... go to to 2 or -2
//                    if (!tempRel.equals("PRESENT"))
//                        direction *= 2;
                                    if (direction == 0)
                                        date = referenceDate.closest(date);
                                    else
                                        date = referenceDate.closest(date, direction);
                                }
//                            else
//                                date = referenceDate.closest(date);
                            } else {
                                date = referenceDate.closest(date);
                            }
                        } else {
                            // RD does not have the same granularity
                            // e.g.: RD = "2006" and new date = "Winter"
                            // just complete the date with RD (=> Winter 2006)
//                        graph.addDateReference(new Pair(date, referenceDate));
                            graph.addRelation(new TemporalRelation(date, referenceDate, TemporalRelation.DURING,
                                    TemporalRelation.TEMPORAL_EXPRESSION, "Co-reference with date"));
                            date = date.completeDate(referenceDate);
                        }
                    }
                    
                    date.setXipNode(unit, unit.getNodeByGramId(nodeNumber));
                    
                    int rel = TemporalRelation.stringToRelationType(tempRel);
                    if (e != null){
                        TemporalRelation relation;
                        // 1/ if AFTER or BEFORE, the "duration" is the interval
                        if (rel == TemporalRelation.AFTER || rel == TemporalRelation.BEFORE){
                            // the MOD correspond to the duration (if it exists)
                            if (mod != null && duration != null)
                                duration.setMod(TemporalConstituent.stringToModType(mod));
                            relation = new TemporalRelation(e, date, rel,
                                    duration, TemporalRelation.TEMPORAL_EXPRESSION, "Co-reference with date");
                        }
                        // 2/ if DURING, the "duration" is a duration
                        //   (a beautiful day of 2006)
                        else if (rel == TemporalRelation.DURING && duration.isDefined() &&
                                !duration.isNull()){
                            e.setDuration(duration);
                            // the MOD correspond to the date
                            //  (the first six months of 2006 => mod "start" is on 2006)
                            if (mod != null)
                                date.setMod(TemporalConstituent.stringToModType(mod));
                            relation = new TemporalRelation(e, date, rel,
                                    TemporalRelation.TEMPORAL_EXPRESSION, "Co-reference with date");
                        } else
                            relation = new TemporalRelation(e, date, rel,
                                    TemporalRelation.TEMPORAL_EXPRESSION, "Co-reference with date");
                        graph.addRelation(relation);
                    } else {
                        if (TemporalRelation.containsRelation(rel, TemporalRelation.AFTER) ||
                                TemporalRelation.containsRelation(rel, TemporalRelation.BEFORE)){
                            result = VagueDate.getNewDate(); //unit, tempNode);
                            TemporalRelation relation;
                            if (duration != null & duration.isDefined() && !duration.isNull()) {
                                // the MOD correspond to the duration (if it exists)
                                if (mod != null && duration != null)
                                    duration.setMod(TemporalConstituent.stringToModType(mod));
                                
                                relation = new TemporalRelation(result, date, rel,
                                        duration, TemporalRelation.TEMPORAL_EXPRESSION, "Co-reference between two dates");
                            } else
                                relation = new TemporalRelation(result, date, rel,
                                        TemporalRelation.TEMPORAL_EXPRESSION, "Co-reference between two dates");
                            graph.addRelation(relation);
                        } else if (rel == TemporalRelation.DURING) {
                            result = date;
                        } else
                            throw new NotImplementedException();
                    }
                } else if (anchoredTo.startsWith("ANCHOR")){
                    /**********************************************
                     * ANCHOR DATE with an anchored_to another ANCHOR
                     *  "two weeks after next Tuesday"
                     *  "two weeks from yesterday"
                     **********************************************/
                    
                    tempNodeInfo = TempFunctions.getTempNodeInfo(fileName, unit.getSentenceNb(),
                            anchoredTo, options);
                    int rel = TemporalRelation.stringToRelationType(tempRel);
                    TemporalElement newElement;
                    String nodeNumber = tempNodeInfo.get("nodenumber");
                    XipNode newTempNode = null;
                    if (nodeNumber != null) {
                        newTempNode = unit.getNodeByGramId(Integer.parseInt(nodeNumber));
                        if (newTempNode == null)
                            newTempNode = tempNode;
                    }
                    if (rel == TemporalRelation.DURING)
                        newElement =
                                analyseAnchor(tempNodeInfo, fileName, unit, originalEvent, e, newTempNode, fromSet, options);
                    else
                        newElement =
                                analyseAnchor(tempNodeInfo, fileName, unit, originalEvent, null, newTempNode, fromSet, options);
                    
                    if (e != null && newElement != null){
                        
                        TemporalRelation relation;
                        
                        // 1/ if AFTER or BEFORE, the "duration" is the interval
                        if (rel == TemporalRelation.AFTER || rel == TemporalRelation.BEFORE){
                            // mod concerns here the duration (exactly 2 years ago)
                            if (mod != null)
                                duration.setMod(TemporalConstituent.stringToModType(mod));
                            relation = new TemporalRelation(e, newElement, rel,
                                    duration, TemporalRelation.TEMPORAL_EXPRESSION, "DATE");
                            graph.addRelation(relation);
                        } else if (rel != TemporalRelation.DURING){
                            relation = new TemporalRelation(e, newElement, rel,
                                    TemporalRelation.TEMPORAL_EXPRESSION, "DATE");
                            graph.addRelation(relation);
                        }
                    } else
                        result = newElement;
                }
                
            } else if (tempRel != null){     // and strAnchoredTo == null
                /**********************************************
                 * ANCHOR DATE without an anchored_to DATE
                 **********************************************/
                
                
                /*********************************************
                 * with temporal relation PRESENT, JUST_AFTER or JUST_BEFORE
                 *  "this year" (PRESENT)
                 *  "last month" (JUST_BEFORE and anchor_type=date)
                 *  "next year" (JUST_AFTER and anchor_type=date)
                 *  "the last year"
                 *  "next 3 months"
                 *  "last 24 hours"
                 *********************************************/
                if ((tempRel.equals("PRESENT") || tempRel.startsWith("JUST")) && !duration.isNull()){
                    VagueDate dateAnchor;
                    // granularity co-reference ("this year")
                    int granularity = duration.getGranularity();
                    if (tempRef.toUpperCase().equals("S")){
                        dateAnchor = graph.getDCT();
                        inferenceLevel = TemporalRelation.TEMPORAL_EXPRESSION;
                    } else {
                        Object[] params = {granularity};
                        dateAnchor = getBestReferenceDate(unit, tempNode, e, graph, "hasGranularity", params);

                       
                        if (dateAnchor == null ||
                                options.getIntProperty(TemporalOptions.TEMPORAL_INFERENCE_LEVEL) < 2)
                            dateAnchor = VagueDate.getNewDate();
                                                                        
                        inferenceLevel = 2;
                    }
                    
                    
                    if (dateAnchor != null && dateAnchor.isDefined()){
                        // mod concerns here the date (the end of this year)
                        VagueDate newDate = null;
                        try {
                            newDate = (VagueDate) dateAnchor.clone();
                        } catch (CloneNotSupportedException ex) {
                            ex.printStackTrace();
                        }
                        if (mod != null)
                            newDate.setMod(TemporalConstituent.stringToModType(mod));
                        
                        String anchorType = tempNodeInfo.get("anchor_type");
                        
                        try {
                            if (newDate.hasGranularity(granularity)){
                                duration.setMod(TemporalConstituent.EXACT);
                                if (tempRel.equals("PRESENT")){
                                    newDate = newDate.changeGranularity(granularity);
                                    
                                    // Change date if not compatible with the tense of the event
                                    if (originalEvent != null && originalEvent.getCategory().equals("VERB") && dct != null && dct.isDefined()){
                                        if ((originalEvent.getXipNode().hasDependency("ST-ORDER", "temp_rel:before") ||
                                                originalEvent.getXipNode().hasDependency("ST-ORDER", "temp_rel:before_or_during")) &&
                                                dct.compareTo(newDate) == TemporalRelation.BEFORE){
                                            Duration durationGranularity =
                                                    new SimpleDuration(DurationUnit.getDurationUnit(granularity, "1"));
                                            newDate = newDate.substract(durationGranularity);
                                        } else if ((originalEvent.getXipNode().hasDependency("ST-ORDER", "temp_rel:after") ||
                                                originalEvent.getXipNode().hasDependency("ST-ORDER", "temp_rel:after_or_during")) &&
                                                dct.compareTo(newDate) == TemporalRelation.AFTER){
                                            Duration durationGranularity =
                                                    new SimpleDuration(DurationUnit.getDurationUnit(granularity, "1"));
                                            newDate = newDate.add(durationGranularity);
                                        }
                                    }
                                } else if (tempRel.equals("JUST_AFTER") &&
                                        anchorType != null && anchorType.toLowerCase().equals("date")){
                                    // case of "last year"
                                    newDate = newDate.changeGranularity(granularity);
                                    newDate = newDate.add(duration);
                                } else if (tempRel.equals("JUST_AFTER")){
                                    // case of "the last year", "the last 3 months"...
                                    VagueDate dateUpperBound = newDate.add(duration);
                                    try{
                                        newDate = new VagueInterval(newDate.getLowerBound().getJustAfter(), dateUpperBound.getUpperBound());
                                    } catch (IntervalException ex) {
                                        ex.printStackTrace();
                                        System.exit(-1);
                                        return null;
                                    }
                                } else if (tempRel.equals("JUST_BEFORE") &&
                                        anchorType != null && anchorType.toLowerCase().equals("date")){
                                    // case of "next year"
                                    newDate = newDate.changeGranularity(granularity);
                                    newDate = newDate.substract(duration);
                                } else if (tempRel.equals("JUST_BEFORE")){
                                    // case of "the next year", "the next 3 months"...
                                    VagueDate dateLowerBound = newDate.substract(duration);
                                    try{
                                        newDate = new VagueInterval(dateLowerBound.getLowerBound(), newDate.getUpperBound().getJustBefore());
                                    } catch (IntervalException ex) {
                                        ex.printStackTrace();
                                        System.exit(-1);
                                        return null;
                                    }
                                } else
                                    throw new NotImplementedException("Relation " + tempRel + " not accepted at this level !");
                            }
                        } catch (LostGranularityException ex) {
                            ex.printStackTrace();
                            System.exit(-1);
                        }
                        newDate.setXipNode(unit, tempNode);
                        graph.addDate(newDate);
                        if (e != null){
                            int rel;
                            // if the event has a duration, we must check that the duration
                            //  fits with the fact that e is DURING the new date
                            //  in terms of granularity
                            //  e.g. if duration is 3 months, date can't be September 9th.
                            if (duration != null && !duration.isNull()){
                                Duration associatedDuration = newDate.getAssociatedDuration();
                                if (duration.compareTo(associatedDuration) > 0)
                                    // if duration and date are not compatible,
                                    //  the event OVERLAPS the date and is not DURING it
                                    rel = TemporalRelation.OVERLAPS;
                                else
                                    // else it's during it
                                    rel = TemporalRelation.DURING;
                            } else{
                                // the event is DURING this new date
                                rel = TemporalRelation.DURING;
                            }
                            TemporalRelation relation = new TemporalRelation(e, newDate,
                                    rel, TemporalRelation.TEMPORAL_EXPRESSION,
                                    "Present co-reference with duration");
                            graph.addRelation(relation);
                        } else
                            result = newDate;
                    } else
                        result = dateAnchor;
                } else {
                    /*********************************************
                     * with other temporal relation
                     *  "after"   (no duration, temp ref = R)
                     *  "earlier" (no duration, temp ref = R)
                     *  "soon" (no duration, temp ref = S)
                     *  "2 years ago" (with duration, temp ref = S)
                     *  "3 months later" (with duration, temp ref = R)
                     *********************************************/
                    TemporalElement eventAnchor;
                    if (tempRef.toUpperCase().equals("S")){
                        eventAnchor = graph.getSpeechTime();
                        inferenceLevel = TemporalRelation.TEMPORAL_EXPRESSION;
                    } else {
                        Pair<Event, Integer> pair = getBestReferenceTime(unit, tempNode, graph);
                        eventAnchor = pair.getFirst();
                        inferenceLevel = pair.getSecond().intValue();
                        if (inferenceLevel == 0)
                            inferenceLevel = 2;
                    }
                    
                    // mod concerns here the duration (exactly 2 years ago)
                    if (mod != null)
                        duration.setMod(TemporalConstituent.stringToModType(mod));
                    
                    if (options.getIntProperty(TemporalOptions.TEMPORAL_INFERENCE_LEVEL) < inferenceLevel){
//                        result = new VagueSimpleDate(unit, tempNode);
//                        return result;
                        eventAnchor = new Event();
                    }
                    
                    TemporalRelation relation;
                    VagueDate newDate = new VagueSimpleDate(unit, tempNode);
                    int intRelation = TemporalRelation.stringToRelationType(tempRel);
                    if (e == null){
                        result = newDate;
                        if (!duration.isNull())
                            relation = new TemporalRelation(newDate, eventAnchor,
                                    intRelation, duration,
                                    inferenceLevel, "Inference " + String.valueOf(inferenceLevel));
                        else
                            relation = new TemporalRelation(newDate, eventAnchor,
                                    intRelation,
                                    inferenceLevel, "Inference " + String.valueOf(inferenceLevel));
                    } else {
                        if (!duration.isNull()) {
                            relation = new TemporalRelation(newDate, eventAnchor,
                                    intRelation, duration,
                                    inferenceLevel, "Inference " + String.valueOf(inferenceLevel));
                            graph.addRelation(new TemporalRelation(e,
                                    newDate,
                                    TemporalRelation.DURING,
                                    TemporalRelation.GRAMMAR2));
                        } else {
                            relation = new TemporalRelation(e, eventAnchor,
                                    intRelation,
                                    inferenceLevel, "Inference " + String.valueOf(inferenceLevel));
                            graph.addRelation(new TemporalRelation(e,
                                    newDate,
                                    TemporalRelation.stringToRelationType("CONCOMITANT"),
                                    TemporalRelation.GRAMMAR2));
                        }
                        if (!duration.isDefined() || duration.isNull()){
//                                if (eventAnchor.equals(graph.getSpeechTime()) && graph.getDCT() != null &&
//                                        graph.getDCT().isDefined())
//                                    eventAnchor = graph.getDCT();
                            // add also the relation between the tempNode and the ST
                            VagueDate otherDate = VagueDate.getNewDate(unit, tempNode);
                            // "now"
                            TemporalRelation otherRelation;
                            if (intRelation == TemporalRelation.DURING)
                                otherRelation = new TemporalRelation(eventAnchor, otherDate,
                                        intRelation,
                                        inferenceLevel, "Inference " + String.valueOf(inferenceLevel));
                            else
                                otherRelation = new TemporalRelation(otherDate, eventAnchor,
                                        intRelation,
                                        inferenceLevel, "Inference " + String.valueOf(inferenceLevel));
                            graph.addRelation(otherRelation);
                        }
//                        else if (intRelation == TemporalRelation.BEFORE || intRelation == TemporalRelation.AFTER){
//                            // yesterday
////                            if (eventAnchor.equals(graph.getSpeechTime()) && graph.getDCT() != null &&
////                                    graph.getDCT().isDefined())
////                                eventAnchor = graph.getDCT();
//                            TemporalRelation otherRelation = new TemporalRelation(newDate, eventAnchor,
//                                    intRelation, duration,
//                                    inferenceLevel, "Inference " + String.valueOf(inferenceLevel));
//                            graph.addRelation(otherRelation);
//                        }
                    }
                    graph.addRelation(relation);
                    
//                   A REMETTRE (2)
//                    } else
//                        result = new VagueSimpleDate(unit, tempNode);
                    
                }
            } else {}
//            result = VagueDate.getNewDate(unit, tempNode);
        } catch (TypedPropertyException ex) {
            CommonLog.errorMessage(ex.getMessage());
        }
        
        return result;
    }
    
    
    /**
     * Stores temporal relations between dates in the specified graph. The input graph
     * shuld already contains some dates, and the relations are calculated and added to the same graph.
     * @param graph the temporal graph
     * @throws documentanalysis.temporality.TemporalConsistencyException when a new relation
     *  is not consistent with already existing ones.
     * @return a Vector containing all relations added to the graph.
     */
    public static Vector<TemporalRelation> storeDateRelations(TemporalGraph graph) throws TemporalConsistencyException{
        Vector<TemporalRelation> result = new Vector<TemporalRelation>();
        
        Vector<VagueDate> dates = graph.getDates();
        VagueDate date1;
        VagueDate date2;
        int c;
        
        for (int i = 0; i < dates.size(); ++i) {
            date1 = dates.get(i);
            for (int j = i+1 ; j < dates.size(); ++j) {
                date2 = dates.get(j);
                c = date1.compareTo(date2);
                TemporalRelation relation = new TemporalRelation(date1, date2, c,
                        TemporalRelation.CALENDAR,
                        "Date comparison");
                graph.addRelation(relation);
                result.add(relation);
            }
        }
        return result;
    }
    
    
    
    /**
     * Returns the default relation used for a verb at PRESENT tense
     */
//    private static int getPresentRelation(){
//        return TemporalRelation.OVERLAPS | TemporalRelation.DURING;
//    }
    
    
    
    
    
    
    
    /**
     * Gets the temporal graph event corresponding to TempEval Event in the text.
     */
    private static Event getClosestMain(XipUnit unit, XipNode node, TemporalGraph graph){
        Vector<XipDependency> mainDependencies = unit.getDependenciesByName("MAIN");
        
        if (mainDependencies.isEmpty())
            return null;
        else if (mainDependencies.size() == 1)
            return graph.getEvent(unit, mainDependencies.get(0).getArg(1), true);
        else {
            XipNode mainNode = mainDependencies.get(0).getArg(1);
            XipNode result = mainNode;
            long bestDistance = java.lang.Math.min(java.lang.Math.abs(node.getLeftTokOffset() - mainNode.getRightTokOffset()),
                    java.lang.Math.abs(node.getRightTokOffset() - mainNode.getLeftTokOffset()));
            long distance;
            
            for (int i = 1 ; i < mainDependencies.size() ; ++i){
                mainNode = mainDependencies.get(i).getArg(1);
                distance = java.lang.Math.min(java.lang.Math.abs(node.getLeftTokOffset() - mainNode.getRightTokOffset()),
                        java.lang.Math.abs(node.getRightTokOffset() - mainNode.getLeftTokOffset()));
                if (distance < bestDistance)
                    result = mainNode;
            }
            return graph.getEvent(unit, result, true);
        }
    }
            
   
    /**
     * sorts TEMP dependencies, so that the dependencies with second elements
     *  having dependency RT are at the end.
     * This is because the RT can be in the same sentence, and we must be sure
     * that it has been already examined
     */
    private static Vector<XipDependency> sortTEMPDependencies(Vector<XipDependency> dependencies) {
        Vector<XipDependency> withoutRT = new Vector<XipDependency>();
        Vector<XipDependency> withRT = new Vector<XipDependency>();
        for (XipDependency dependency : dependencies) {
            if (dependency.getArg(2).hasDependency("RT", 2))
                withRT.add(dependency);
            else
                withoutRT.add(dependency);
        }
        withoutRT.addAll(withRT);
        return withoutRT;
    }
}

