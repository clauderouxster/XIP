/*
 * TempEval.java
 *
 * Created on February 7, 2007, 8:27 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common.corpora;

import com.xerox.jxip.XipDependency;
import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipUnit;
import common.Common;
import common.CommonLog;
import common.NotImplementedException;
import common.Options;
import common.Pair;
import common.TypedPropertyException;
import common.applications.tempEval.TempEvalDate;
import common.applications.tempEval.TempEvalDuration;
import common.applications.tempEval.TempEvalEvent;
import documentAnalysis.temporality.TemporalConsistencyException;
import documentAnalysis.temporality.TemporalConstituent;
import documentAnalysis.temporality.TemporalElement;
import documentAnalysis.temporality.TemporalGraph;
import documentAnalysis.temporality.TemporalRelation;
import documentAnalysis.events.Event;
import documentAnalysis.temporality.vaguecalendar.BadFormatException;
import documentAnalysis.temporality.vaguecalendar.date.VagueDate;
import documentAnalysis.temporality.vaguecalendar.date.VagueInterval;
import documentAnalysis.temporality.vaguecalendar.duration.Duration;
import documentAnalysis.temporality.vaguecalendar.duration.Hours;
import documentAnalysis.temporality.vaguecalendar.duration.SimpleDuration;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Vector;

/**
 * This class contains methods for TempEval corpus
 * @author xtannier
 */
public class TempEval extends Corpus{
    
    
    private HashMap<String, String> eventsMapping;
    private HashMap<String, Pair<String, String>> tempExprMapping;
    private Integer eventNumber;
    private Integer timexNumber;
    private Vector<String> eventList;
    private String taskType;
    private int sentenceNumber;
    
    private static int test = 0;
    
    
    private static Vector<String> tempEvalArguments;
    
    static{
        tempEvalArguments = new Vector<String>();
        tempEvalArguments.add("CORPUS_EVENT_LIST");
        tempEvalArguments.add("CORPUS_TASK");
        tempEvalArguments.add("CORPUS_USE_TIMEX_VALUES");
        tempEvalArguments.add("CORPUS_PRIORITY_TO_RECALL");
        tempEvalArguments.add("CORPUS_ALL_RANDOM_RELATIONS");
    }
    
    
    /** Creates a new instance of TempEval */
    public TempEval() {
        super();
        this.name = "TEMPEVAL";
        this.eventsMapping = new HashMap<String, String>();
        this.tempExprMapping = new HashMap<String, Pair<String, String>>();
        this.eventList = new Vector<String>();
        this.eventNumber = 0;
        this.sentenceNumber = 1;
        this.timexNumber = 0;
        this.taskType = "AB";
//        this.dctId = "";
    }
    
    
    /** Creates a new instance of TempEval
     * @param options the parser options
     */
    public TempEval(Options options) {
        this();
    }
    
    
    
    /**
     * Method called at the end of each sentence
     */
    public void clear(){
        this.eventsMapping.clear();
        this.tempExprMapping.clear();
        this.eventNumber = 0;
        this.sentenceNumber = 1;
        this.timexNumber = 0;
//        this.dctId = "";
    }
    
    /**
     * Treats corpus-specific properties.
     *
     * @param name the name of the property
     * @param value the value of the property
     * @return -1 if the treatment failed (basically if the property is unknown), 0 otherwise.
     */
    public int setProperty(String name, String value){
        name = name.toUpperCase();
        if (name.equals("CORPUS_EVENT_LIST")){
            try {
                eventList = Common.readToVector(value);
                return 0;
            } catch (IOException ex) {
                CommonLog.errorMessage(ex.getMessage());
                return -1;
            }
        } else if (name.equals("CORPUS_TASK")){
            if (value.toUpperCase().equals("AB") || value.toUpperCase().equals("C")) {
                this.taskType = value.toUpperCase();
                return 0;
            } else {
                CommonLog.errorMessage("TempEval task " + value + " specified by " + name + " is unknown");
                return -1;
            }
        } else if (tempEvalArguments.contains(name))
            return 0;
        else
            return -1;
    }
    
    
    private int getSentenceNumber(String text, int position){
        text = text.substring(0, position);
        int index = text.indexOf("<s>");
        int number = 0;
        while (index != -1){
            ++number;
            index = text.indexOf("<s>", index + 3);
        }
        return number;
    }
    
    /**
     * Gets different information from the input file (DCT for example).
     *
     * @param options the parser options.
     */
    public void getInputInfo(Options options){
        // (re)initialize event and timex numbers
        this.eventNumber = 0;
        this.timexNumber = 0;
        String inputFileName = options.getInputFileName();
        TemporalGraph temporalGraph = options.getTemporalGraph();
        // for TempEval a document is in XML
        // with <EVENT>s and <TIMEX3>s.
        // an EVENT has an attribute eid
        // an TIMEX3 has an attribute tid
        // that's what we want to get and to associate with corresponding text offsets
        try {
            String text = Common.read(inputFileName);
            int indexElement = 0;
            int indexIdBegin = 0;
            int indexIdEnd = 0;
            int indexInfoBegin = -1;
            int indexInfoEnd = -1;
            int indexEndOfOpeningTag = 0;
            int indexRightOffset = 0;
            int elementNumber = 1;
            int currentSentenceNumber = 0;
            String id;
            String info;
            
            // EVENTs
            while (indexElement != -1){
                indexElement = text.indexOf("<EVENT", indexRightOffset);
                if (indexElement != -1){
                    // find id
                    indexIdBegin = text.indexOf("eid=", indexElement);
                    if (indexIdBegin != -1){
                        indexIdEnd = text.indexOf("\"", indexIdBegin + 5);
                        if (indexIdEnd != -1){
                            id = text.substring(indexIdBegin + 5, indexIdEnd);
                            // find stem or mainevent info according to task
                            if (taskType.equals("AB"))
                                indexInfoBegin = text.indexOf("stem=", indexElement);
                            else if (taskType.equals("C"))
                                indexInfoBegin = text.indexOf("mainevent=", indexElement);
                            
                            if (indexInfoBegin != -1){
                                int infoLength = 0;
                                if (taskType.equals("AB"))
                                    infoLength = 6;
                                else if (taskType.equals("C"))
                                    infoLength = 11;
                                
                                indexInfoEnd = text.indexOf("\"", indexInfoBegin + infoLength);
                                
                                if (indexInfoEnd != -1){
                                    info = text.substring(indexInfoBegin + infoLength, indexInfoEnd);
                                    // insert in Hashmap according to the task
                                    indexEndOfOpeningTag = text.indexOf(">", indexIdEnd);
                                    indexRightOffset = text.indexOf("</EVENT", indexEndOfOpeningTag);
                                    int sentenceNumber = getSentenceNumber(text, indexElement);
                                    if (sentenceNumber != currentSentenceNumber){
                                        elementNumber = 1;
                                        currentSentenceNumber = sentenceNumber;
                                    }
                                    String keyValue = sentenceNumber + "." + elementNumber;
                                    if (indexEndOfOpeningTag != -1 && indexRightOffset != -1){
                                        if (this.taskType.equals("AB") && this.eventList.contains(info))
                                            this.eventsMapping.put(keyValue, id);
                                        else if (this.taskType.equals("C") && info.toUpperCase().equals("YES"))
                                            this.eventsMapping.put(keyValue, id);
//                                        else
//                                            this.eventsMapping.put(keyValue, "");
                                        ++elementNumber;
                                    } else
                                        break;
                                } else
                                    break;
                            } else
                                break;
                        } else
                            break;
                    } else
                        break;
                } else
                    break;
            }
            // TIMEX3s
            indexElement = 0;
            indexRightOffset = 0;
            elementNumber = 1;
            while (indexElement != -1){
                indexElement = text.indexOf("<TIMEX3", indexRightOffset);
                if (indexElement != -1){
                    indexIdBegin = text.indexOf("tid=", indexElement);
                    if (indexIdBegin != -1){
                        indexIdEnd = text.indexOf("\"", indexIdBegin + 5);
                        if (indexIdEnd != -1){
                            id = text.substring(indexIdBegin + 5, indexIdEnd);
                            indexEndOfOpeningTag = text.indexOf(">", indexIdEnd);
                            indexRightOffset = text.indexOf("</TIMEX3", indexEndOfOpeningTag);
                            // get the attributes
                            String attributes = text.substring(indexElement, indexEndOfOpeningTag);
                            // if duration do not get the value
                            boolean isDate = true;
                            if (attributes.indexOf("type=\"DURATION\"") != -1)
                                isDate = false;
//                            if (attributes.indexOf("type=\"SET\"") != -1)
//                                isDate = false;
                            // get the value
                            int valueAttribute = attributes.indexOf("value=");
                            if (valueAttribute != -1) {
                                int valueRight = attributes.indexOf("\"", valueAttribute + 7);
                                if (valueRight != -1){
                                    String value = attributes.substring(valueAttribute + 7, valueRight);
                                    try {
                                        if (!isDate)
                                            value = "";
                                        
                                        /****************
                                         * Find DCT
                                         ****************/
                                        // DCT ? yes if functionInDocument="CREATION_TIME" for this element
                                        if (attributes.indexOf("functionInDocument=\"CREATION_TIME\"") != -1){
                                            VagueDate dct = VagueDate.getDateFromISO(value);
                                            // decade only: add century
                                            if (dct.isCenturyMissing()){
                                                dct = dct.completeCentury(19);
                                            }
//                                            try {
//                                                dct = dct.changeGranularity(TemporalElement.DAY);
//                                            } catch (LostGranularityException ex) {
//                                                ex.printStackTrace();
//                                            }
                                            options.setDct(dct);
                                            options.setProperty("CORPUS_DCTID", id);
                                        } else {
                                            // if not DCT put the date in the dictionary
                                            if (indexEndOfOpeningTag != -1 && indexRightOffset != -1){
                                                int sentenceNumber = getSentenceNumber(text, indexElement);
                                                if (sentenceNumber != currentSentenceNumber){
                                                    elementNumber = 1;
                                                    currentSentenceNumber = sentenceNumber;
                                                }
                                                String keyValue = sentenceNumber + "." + elementNumber;
                                                this.tempExprMapping.put(keyValue, new Pair<String, String>(id, value));
                                                ++elementNumber;
                                            }
                                        }
                                    } catch (TemporalConsistencyException ex) {
                                        CommonLog.warningMessage(ex.toString());
                                    } catch (BadFormatException ex) {
                                        CommonLog.errorMessage(ex.toString());
                                    }
                                }
                            }
                            
//                                // first get the tid
//                                int idLeft = attributes.indexOf("tid=");
//                                if (idLeft != -1){
//                                    int idRight = attributes.indexOf("\"", idLeft + 5);
//                                    if (idRight != -1){
//                                        this.dctId = attributes.substring(idLeft + 5, idRight);
//                                    }
//                                }
                            // then get the value
                            
//                                if (valueAttribute != -1) {
//                                    int valueRight = attributes.indexOf("\"", valueAttribute + 7);
//                                    if (valueRight != -1){
//                                        String value = attributes.substring(valueAttribute + 7, valueRight);
//                                        VagueDate dct;
//                                        try {
//                                            dct = VagueDate.getDateFromISO(value);
//                                            options.setDct(dct);
//                                        } catch (TemporalConsistencyException ex) {
//                                            CommonLog.warningMessage(ex.toString());
//                                        } catch (BadFormatException ex) {
//                                            CommonLog.warningMessage(ex.toString());
//                                        }
//                                    }
//                                }
//                            } else {
//                                // if not DCT put the date in the dictionary
//
                        }
                    } else
                        break;
                } else
                    break;
            }
        } catch (FileNotFoundException ex) {
            ex.printStackTrace();
        } catch (IOException ex) {
            ex.printStackTrace();
        }
//        TemporalElement speechTime = temporalGraph.getSpeechTime();
//        TemporalElement dct = temporalGraph.getDCT();
//        try {
//            temporalGraph.addRelation(new TemporalRelation(speechTime, dct,
//                        TemporalRelation.EQUALS, TemporalRelation.CERTAIN));
//        } catch (TemporalConsistencyException ex) {
//            ex.printStackTrace();
//            System.exit(-1);
//        }
//        System.err.println("");
    }
    
    
    /**
     * Updates the temporal graph with an ISO date value
     */
    private void getDateFromISO(String value, TempEvalDate date, TemporalGraph temporalGraph) throws BadFormatException, TemporalConsistencyException{
        TemporalRelation relation;
        if (value.equals("PRESENT_REF")){
            relation = new TemporalRelation(temporalGraph.getDCT(), date,
                    TemporalRelation.EQUALS, TemporalRelation.CERTAIN, "Tempeval TIMEX");
            temporalGraph.addRelation(relation);
        } else if (value.equals("FUTURE_REF")){
            relation = new TemporalRelation(temporalGraph.getDCT(), date,
                    TemporalRelation.BEFORE, TemporalRelation.CERTAIN, "Tempeval TIMEX");
            temporalGraph.addRelation(relation);
        } else if (value.equals("PAST_REF")){
            relation = new TemporalRelation(temporalGraph.getDCT(), date,
                    TemporalRelation.AFTER, TemporalRelation.CERTAIN, "Tempeval TIMEX");
            temporalGraph.addRelation(relation);
        } else if (value.equals("")) {
            
        } else {
            date.copy(VagueDate.getDateFromISO(value));
//            relation = new TemporalRelation(newDate, date,
//                    TemporalRelation.EQUALS, TemporalRelation.CERTAIN, "Tempeval TIMEX");
//            temporalGraph.addRelation(relation);
        }
        
    }
    
    /**
     * Gets different information from the XML tag currently parsed.
     * Called in the XML callback.
     *
     * @param xmlTagInfos the info about XML tag name and attributes.
     * @param unit the last XipUnit parsed.
     * @param options the parser options.
     */
    public void getXMLTagInfo(Hashtable<String,String> xmlTagInfos,
            XipUnit unit,
            Options options){
        // get tag name
        String tagName = xmlTagInfos.get("tagName");
        if (tagName.equals("s")){
            ++this.sentenceNumber;
            this.eventNumber = 0;
            this.timexNumber = 0;
        }
    }
    
    
    /**
     * Add new corpus-specific temporal stRelations in the temporal graph.
     *
     * @param temporalGraph the graph to refine
     * @param options the parser options
     */
    public void refineTemporalGraphAfterClosure(TemporalGraph temporalGraph,
            Options options) throws TemporalConsistencyException{
        try {
            
            // if we have BEFORE(event, ST) and BEFORE-OR-OVERLAPPED_BY(event, DCT)
            // and no other information saying that event can be really be overlaped by DCT
            // convert into BEFORE(event, DCT)
            Event speechTime = temporalGraph.getSpeechTime();
            VagueDate dct = temporalGraph.getDCT();
            Vector<TemporalRelation> stRelations = speechTime.getTemporalRelations();
            for (TemporalRelation relation : stRelations){
                // all temporal stRelations returned by ST.getRelations
                //  should have ST as first argument
                if (!relation.getTemporalElement1().equals(speechTime))
                    throw new NotImplementedException();
                else {
                    TemporalElement element = relation.getTemporalElement2();
                    if (element instanceof Event){
                        Event event = (Event)element;
                        int rel = relation.getRelation();
                        if (rel == TemporalRelation.AFTER){
                            TemporalRelation relation2 = temporalGraph.getRelation(element, dct);
                            if (relation2 != null && relation2.getRelation() ==
                                    (TemporalRelation.BEFORE | TemporalRelation.OVERLAPPED_BY | TemporalRelation.DURING)) {
                                relation2.setRelation(TemporalRelation.BEFORE);
                                temporalGraph.addRelation(relation2);
                            }
                        }
                        if (rel == TemporalRelation.BEFORE){
                            TemporalRelation relation2 = temporalGraph.getRelation(element, dct);
                            if (relation2 != null && relation2.getRelation() ==
                                    (TemporalRelation.AFTER | TemporalRelation.OVERLAPS | TemporalRelation.DURING)) {
                                relation2.setRelation(TemporalRelation.AFTER);
                                temporalGraph.addRelation(relation2);
                            }
                        }
                        // AFTER-OR-OVERLAP found by composition
                        // is turned into AFTER, same for BEFORE
                        if (rel != TemporalRelation.BEFORE && rel != TemporalRelation.UNDEFINED &&
                                !TemporalRelation.containsRelation(rel, TemporalRelation.AFTER) &&
                                TemporalRelation.containsRelation(rel, TemporalRelation.BEFORE)) {
                            if (relation.getPriority() == TemporalRelation.COMPOSITION) {
                                relation.setRelation(TemporalRelation.BEFORE);
                            }
                        }
                        if (rel != TemporalRelation.AFTER && rel != TemporalRelation.UNDEFINED &&
                                !TemporalRelation.containsRelation(rel, TemporalRelation.BEFORE) &&
                                TemporalRelation.containsRelation(rel, TemporalRelation.AFTER)) {
                            if (relation.getPriority() == TemporalRelation.COMPOSITION) {
                                relation.setRelation(TemporalRelation.AFTER);
                            }
                        }
                        // if DCT's granularity is beyond the hour
                        // REL(ST) => REL(DCT)
                        if (dct.hasGranularity(VagueDate.HOUR)) {
                            TemporalRelation newRelation = new TemporalRelation(
                                    dct, element, rel, relation.getPriority());
                            try{
                                temporalGraph.addRelation(newRelation);
                            } catch (documentAnalysis.temporality.TemporalConsistencyException e) {
                                CommonLog.warningMessage(e.getMessage());
                            }
                        }
                        // if the MAIN is PAST but without date, let's say it's just before ST
                        // (sayone hour)
                        if (options.getBooleanProperty("CORPUS_PRIORITY_TO_RECALL")){
                            XipNode node = event.getXipNode();
                            if (node != null && rel == TemporalRelation.AFTER && node.hasDependency("MAIN") &&
                                    node.hasFeature("indspeech")) {
                                Vector<TemporalRelation> mainRelations = event.getTemporalRelations(TemporalRelation.DURING, -1);
                                boolean found = false;
                                for (TemporalRelation mainRelation : mainRelations){
                                    if (mainRelation.getOtherElement(event) instanceof VagueDate)
                                        found = true;
                                }
                                if (!found) {
                                    VagueDate newDate = dct.substract(new SimpleDuration(new Hours(1)));
                                    TemporalRelation newRelation = new TemporalRelation(
                                            event, newDate, TemporalRelation.DURING, TemporalRelation.RANDOM);
                                    temporalGraph.addRelation(newRelation);
                                }
                            }
                        }
                        
                        //  if the event OVERLAPS the ST, we give it
                        //   an interval around ST
                        if (options.getBooleanProperty("CORPUS_PRIORITY_TO_RECALL") &&
                                event.getXipNode() != null && event.getXipNode().hasDependency("ST-ORDER", "temp_rel:concomitant")){
//                        if (!TemporalRelation.containsRelation(TemporalRelation.BEFORE, rel) &&
//                                !TemporalRelation.containsRelation(TemporalRelation.AFTER, rel)) {
                            VagueDate newDate = VagueInterval.getAroundDate(dct,
                                    Duration.getNewDuration(new Hours(12)));
                            TemporalRelation newRelation = new TemporalRelation(
                                    event, newDate, TemporalRelation.DURING, TemporalRelation.RANDOM);
                            temporalGraph.addRelation(newRelation);
                        }
                    }
                }
            }
            
            
            // if an event is strictly AFTER an "indspeech",
            // then it's also after the ST
//            for (TemporalElement element : temporalGraph.getEvents()){
//                XipNode node = element.getXipNode();
//                if (node != null && node.hasFeature("indspeech")) {
//                    Vector<TemporalRelation> indSpeechRelations = element.getRelations();
//                    for (TemporalRelation indSpeechRelation : indSpeechRelations){
//                        if (indSpeechRelation.getRelation() == TemporalRelation.BEFORE) {
//                            TemporalElement secondElement = indSpeechRelation.getOtherElement(element);
//                            if (secondElement instanceof Event){
//                                TemporalRelation relation = temporalGraph.getRelation(secondElement, speechTime);
//                                if (relation == null || relation.getRelation() == TemporalRelation.UNDEFINED) {
//                                    TemporalRelation newRelation =
//                                            new TemporalRelation(secondElement, speechTime,
//                                            TemporalRelation.AFTER, TemporalRelation.DEFAULT_CHOICE);
//                                    temporalGraph.addRelation(newRelation);
//                                }
//                            }
//                        }
//                    }
//                }
//            }
            
            
            // find relations between TempEvalEvents and TempEvalDates/Durations
            //   that have not been found yet
            Vector<Event> events = temporalGraph.getEvents();
            for (Event event : events){
                if (event instanceof TempEvalEvent){
                    
                    /************
                     * task A
                     ***********/
                    //   need a relation with all TIMEX3 of the same sentence
                    // DATEs
                    Vector<VagueDate> dates = temporalGraph.getDates(event.getSentenceNumber());
                    for (VagueDate date : dates){
                        if (date instanceof TempEvalDate){
                            TemporalRelation relation = temporalGraph.getRelation(event, date);
                            if ((relation == null && options.getBooleanProperty("CORPUS_PRIORITY_TO_RECALL")) ||
                                    options.getBooleanProperty("CORPUS_ALL_RANDOM_RELATIONS")){
                                int c = date.compareTo(dct);
                                if (c == TemporalRelation.AFTER || c == TemporalRelation.DURING ||
                                        c == TemporalRelation.EQUALS || c == TemporalRelation.OVERLAPS)
                                    relation = relation = new TemporalRelation(event, date, TemporalRelation.BEFORE,
                                            TemporalRelation.RANDOM, "random");
                                else
                                    relation = new TemporalRelation(event, date,
                                            common.applications.tempEval.TempEval.RANDOM_TASK_A,
                                            TemporalRelation.RANDOM, "random");
                                temporalGraph.addRelation(relation);
                            } else if (relation == null) {
//                            relation = new TemporalRelation(event, date, TemporalRelation.UNDEFINED,
//                                    TemporalRelation.RANDOM, "random");
                            } else if (relation != null) {
                                temporalGraph.addRelation(relation);
                            }
                        }
                    }
                    // DURATIONs
                    Vector<Duration> durations = temporalGraph.getDurations(event.getSentenceNumber());
                    for (Duration duration : durations){
                        if (duration instanceof TempEvalDuration){
                            TemporalRelation relation = temporalGraph.getRelation(event, duration);
                            if ((relation == null && options.getBooleanProperty("CORPUS_PRIORITY_TO_RECALL")) ||
                                    options.getBooleanProperty("CORPUS_ALL_RANDOM_RELATIONS")){
                                relation = new TemporalRelation(event, duration,
                                        common.applications.tempEval.TempEval.RANDOM_TASK_A,
                                        TemporalRelation.RANDOM, "random");
                                temporalGraph.addRelation(relation);
                            } else if (relation == null) {
//                            relation = new TemporalRelation(event, duration, TemporalRelation.UNDEFINED,
//                                    TemporalRelation.RANDOM, "random");
                            } else if (relation != null) {
                                temporalGraph.addRelation(relation);
                            }
                        }
                    }
                    
                    /************
                     * task B
                     ***********/
                    // need a relation between the event and the DCT
                    TemporalRelation relationDCT = null;
                    TemporalRelation relationST = null;
                    TemporalRelation relation;
                    relationDCT = temporalGraph.getRelation(event, dct);
                    relationST = temporalGraph.getRelation(event, speechTime);
                    boolean random = false;
                    
                    if (options.getBooleanProperty("CORPUS_ALL_RANDOM_RELATIONS")){
                        relationDCT = new TemporalRelation(event, dct,
                                common.applications.tempEval.TempEval.RANDOM_TASK_B,
                                TemporalRelation.RANDOM, "random");
                        temporalGraph.addRelation(relationDCT);
                    } else {
                        if (relationDCT != null){
                            if (relationST != null) {
                                if ((relationDCT.getRelation() ==
                                        (TemporalRelation.BEFORE | TemporalRelation.OVERLAPS | TemporalRelation.DURING))
                                        && relationST.getRelation() == TemporalRelation.BEFORE)
                                    relationDCT.setRelation(TemporalRelation.BEFORE);
                            }
                        } else {
                            if (relationST == null && options.getBooleanProperty("CORPUS_PRIORITY_TO_RECALL")){
////                                if (event.getXipNode() != null && event.getXipNode().getCategory().equalsIgnoreCase("NOUN"))
////                                    relationDCT = new TemporalRelation(event, dct, TemporalRelation.OVERLAPS,
////                                        TemporalRelation.RANDOM, "random");
////                                else
                                relationDCT = new TemporalRelation(event, dct,
                                        common.applications.tempEval.TempEval.RANDOM_TASK_B,
                                        TemporalRelation.RANDOM, "random");
                                random = true;
                            } else if (relationST == null && !options.getBooleanProperty("CORPUS_PRIORITY_TO_RECALL")) {
                                relationDCT = new TemporalRelation(event, dct, TemporalRelation.UNDEFINED,
                                        TemporalRelation.RANDOM, "random");
                                random = true;
                            } else
                                relationDCT = new TemporalRelation(event, dct, relationST.getRelation());
                        }
                        if (!random || options.getBooleanProperty("CORPUS_PRIORITY_TO_RECALL")){
                            temporalGraph.addRelation(relationDCT);
                        }
                    }
                }
            }
            
//            if (options.getBooleanProperty("CORPUS_PRIORITY_TO_RECALL")){
            temporalGraph.getTemporalClosure(true, 1,false);
//            }
        } catch (TypedPropertyException ex) {
            ex.printStackTrace();
            System.exit(-1);
        }
    }
    
    
    /**
     * Apply any appropriate corpus-specific inference with a XipUnit.
     *
     * @param unit the current XipUnit treated
     * @param options the parser options.
     */
    public void corpusSpecificInference(XipUnit unit, Options options) throws TemporalConsistencyException{
        TemporalGraph temporalGraph = options.getTemporalGraph();
        
        
        
        /**************************
         *    First,
         * add temporal relations
         *     with DURATIONs
         **************************/
        Vector<TemporalRelation> relationsToAdd = new Vector<TemporalRelation>();
        // This is not done in general temporal module (because senseless)
        // but TempEval requires that
        // INTERVALs
        for (TemporalRelation relation : temporalGraph.getRelations()){
            // DELTA between two events
            if (relation.hasInterval() &&
                    relation.getInterval().getSentenceNumber() == unit.getSentenceNb()){
                Duration interval = relation.getInterval();
//                temporalGraph.addDuration(interval);
                // for tempeval, the duration is between both events
                // get corresponding duration
                Vector<XipNode> durationNodes = new Vector<XipNode>();
                if (interval.getXipNode().hasFeature("c_dur"))
                    durationNodes.add(interval.getXipNode());
                else
                    durationNodes = interval.getXipNode().getFirstDescendantsByFeature("c_dur");
                for (XipNode durationNode : durationNodes){
                    Duration duration = Duration.getNewDuration(interval.getXipUnit(),
                            durationNode);
                    temporalGraph.addDuration(duration);
                    // add relations
                    if (relation.getTemporalElement1() instanceof Event)
                        relationsToAdd.add(new TemporalRelation(relation.getTemporalElement1(),
                                duration,
                                relation.getRelation(),
                                TemporalRelation.GRAMMAR2,
                                "Special TEMPEVAL"));
                    if (relation.getTemporalElement2() instanceof Event)
                        relationsToAdd.add(new TemporalRelation(relation.getTemporalElement2(),
                                duration,
                                TemporalRelation.getInverseRelation(relation.getRelation()),
                                TemporalRelation.GRAMMAR2,
                                "Special TEMPEVAL"));
                    
                }
                // if both temporal elements are events, link the first one
                //  to the full temporal expression (if not a duration).
                //  (at this point, the order of elements in temporal elements is still the
                //  same as when creating the relation)
                if (relation.getTemporalElement1() instanceof Event &&
                        relation.getTemporalElement2() instanceof Event &&
                        interval.getXipNode().hasFeature("c_anchor")){
                    VagueDate date = VagueDate.getNewDate(interval.getXipUnit(), interval.getXipNode());
                    relationsToAdd.add(new TemporalRelation(relation.getTemporalElement1(),
                            date,
                            TemporalRelation.DURING,
                            TemporalRelation.GRAMMAR2,
                            "Special TEMPEVAL"));
                    relationsToAdd.add(new TemporalRelation(relation.getTemporalElement2(),
                            date,
                            TemporalRelation.getInverseRelation(relation.getRelation()),
                            TemporalRelation.GRAMMAR2,
                            "Special TEMPEVAL"));
                }
            }
        }
        
        // EVENT DURATIONs
        for (Event event : temporalGraph.getEvents()){
            if (event.getSentenceNumber() == unit.getSentenceNb() &&
                    event.hasDuration()){
                relationsToAdd.add(new TemporalRelation(event, event.getDuration(),
                        TemporalRelation.EQUALS,
                        TemporalRelation.GRAMMAR2,
                        "Special TEMPEVAL"));
            }
        }
        
        // Some relations TEMP do not exactly correspond to DATEs,
        //  but to relations
        //  eg: it's now, it's at any time, ...
        //  in these cases we add a relation between the event and the expression
        // TEMP dependencies
//        Vector<XipDependency> dependencies = unit.getDependenciesByName("TEMP");
//        for (XipDependency dependency : dependencies){
//            Event e1 = temporalGraph.getEvent(unit, dependency.getArg(1), false);
//            XipNode n2 = dependency.getArg(2);
//            Vector<TemporalRelation> relations = e1.getRelations();
//            boolean found = false;
//            for (TemporalRelation relation : relations){
//                TemporalElement e2 = relation.getOtherElement(e1);
//                if (e2.getXipNode() != null && e2.getXipNode().equals(n2)){
//                    found = true;
//                    break;
//                }
//            }
//            if (!found){
//                VagueDate fakeDate = VagueDate.getNewDate(unit, n2);
//                TemporalRelation relation = new TemporalRelation(e1, fakeDate,
//                        TemporalRelation.stringToRelationType("CONCOMITANT"),
//                        TemporalRelation.GRAMMAR, "Special TEMPEVAL");
//                relationsToAdd.add(relation);
//            }
//        }
        
        
        
        // add found temporal relations
        for (TemporalRelation relation : relationsToAdd){
//            ++test;
//            System.err.println("test = " + test );
//            if (test == 3)
//                System.err.println("");
            try{
                temporalGraph.addRelation(relation);
            } catch (documentAnalysis.temporality.TemporalConsistencyException ex) {
                CommonLog.warningMessage(ex.getMessage());
            }
        }
        
        
        
        
        
        
        
        
        
        /****************************
         * Associated XIP events and
         *  temporal expressions to
         * TempEval ones when needed
         *  (ad hoc and experimental)
         ****************************/
        // EVENTs
        Vector<XipNode> tempEvalEventNodes = unit.getNodesByFeature("te_event", "+");
        String id;
        XipNode lastMother = null;
        for (XipNode tempEvalEventNode : tempEvalEventNodes){
            // two elements are tagged te_event for each events :
            //   the leaf (the lemma) and the upper level : VERB for example
            // sometimes upper nodes are also tagged (why ?)
            //   we want the VERB level, because it is the one that is linked to the rest
            //    of the sentence...
            // that's why we take the mother
            if (tempEvalEventNode.isLeaf()){
                XipNode mother = tempEvalEventNode.getMother();
                if (lastMother != null && mother.equals(lastMother))
                    continue;
                lastMother = mother;
                ++this.eventNumber;
                id = this.eventsMapping.get(this.sentenceNumber + "." + this.eventNumber);
                
                Event event = temporalGraph.getEvent(unit, mother, false);
                
                if (id != null){
                    TempEvalEvent tempEvalEvent = new TempEvalEvent(unit, mother, id);
                    if (event != null){
                        temporalGraph.addRelation(new TemporalRelation(event,
                                tempEvalEvent,
                                TemporalRelation.EQUALS, TemporalRelation.CERTAIN,
                                "Special TEMPEVAL, te_event"));
                    } else {
                        event = getClosestEvent(unit, mother, temporalGraph,
                                new Integer(-1), new Vector<XipNode>());
                        if (event == null)
                            event = temporalGraph.getEvent(unit, mother, true);
                        temporalGraph.addRelation(new TemporalRelation(event,
                                tempEvalEvent,
                                TemporalRelation.EQUALS, TemporalRelation.CERTAIN,
                                "Special TEMPEVAL, te_event"));
                    }
                }
            }
        }
        
        // TIMEX3s
        Vector<XipNode> tempEvalTemporalNodes = unit.getNodesByFeature("te_tempexprfirst", "+");
        long left;
        long right;
        for (XipNode tempEvalTemporalNode : tempEvalTemporalNodes){
            // two elements are tagged te_tempexpr for each tempexpr :
            //   the leaf (the lemma) and the upper level : NOUN for example
            //   we want the later, because it is the one that is linked to the rest
            //    of the sentence...
            // that's why we take the mother
//            XipNode tempEvalTemporalNode = tempEvalTemporalNodes.get(i);
            if (tempEvalTemporalNode.isLeaf()){
                XipNode mother = tempEvalTemporalNode.getMother();
                if (lastMother != null && mother.equals(lastMother))
                    continue;
                lastMother = mother;
                left = mother.getLeftTokOffset();
                right = left;
                // find right boundary of the te_tempexpr
                XipNode followingNode = unit.getLeafNodeByTokOffset(right + 1);
                while(followingNode != null && followingNode.hasFeature("te_tempexpr", "+")){
                    right++;
                    followingNode = unit.getLeafNodeByTokOffset(right + 1);
                }
                
                ++this.timexNumber;
                // build the special TemporalElement
                Pair<String, String> timex = this.tempExprMapping.get(this.sentenceNumber + "." + this.timexNumber);
                id = timex.getFirst();
                
//            TempEvalEvent tempEvalEvent = new TempEvalEvent(unit, tempEvalEventNode, id);
                
                // get corresponding TemporalElement in the graph
                TemporalConstituent tempNode = getCorrespondingDateOrDuration(temporalGraph, unit, left, right);
                // Equality between both
                if (tempNode != null){
                    if (tempNode instanceof VagueDate){
                        TempEvalDate tempEvalDate = new TempEvalDate(unit, mother, id);
                        try {
                            if (options.getBooleanProperty("CORPUS_USE_TIMEX_VALUES")){
                                String value = timex.getSecond();
                                getDateFromISO(value, tempEvalDate, temporalGraph);
                                
                                
                                tempEvalDate.setXipNode(tempNode.getXipUnit(), tempNode.getXipNode());
//                                tempNode = tempEvalDate
                                if (tempNode instanceof VagueDate)
                                    temporalGraph.replaceDate((VagueDate)tempNode, tempEvalDate);
//                                temporalGraph.addRelation(new TemporalRelation(tempNode, tempEvalDate,
//                                        TemporalRelation.EQUALS, TemporalRelation.CERTAIN,
//                                        "Special TEMPEVAL, te_tempexpr"));
                            } else if (options.getProperty("CORPUS_TASK").equals("AB")) {
                                temporalGraph.addRelation(new TemporalRelation(tempNode, tempEvalDate,
                                        TemporalRelation.EQUALS, TemporalRelation.CERTAIN,
                                        "Special TEMPEVAL, te_tempexpr"));
                            }
                        } catch (BadFormatException ex) {
                            CommonLog.errorMessage(ex.getMessage());
                        } catch (TypedPropertyException ex) {
                            CommonLog.errorMessage(ex.getMessage());
                        }
                        
                    } else if (tempNode instanceof Duration){
                        Duration tempEvalDuration = new TempEvalDuration(unit, mother, id);
                        temporalGraph.addRelation(new TemporalRelation(tempNode, tempEvalDuration,
                                TemporalRelation.EQUALS, TemporalRelation.CERTAIN,
                                "Special TEMPEVAL, te_tempexpr"));
                    }
                } else {
//                    if (id == null)
//                        System.err.println("");
                    VagueDate tempEvalDate = new TempEvalDate(unit, mother, id);
                    temporalGraph.addDate(tempEvalDate);
                }
            }
        }
        
//        for (XipNode tempEvalEventNode : tempEvalEventNodes){
//            // two elements are tagged te_event for each events :
//            //   the leaf (the lemma) and the upper level : VERB for example
//            //   we want the later, because it is the one that is linked to the rest
//            //    of the sentence...
//            if (!tempEvalEventNode.isLeaf()){
//                ++this.eventNumber;
//                Event event = temporalGraph.getEvent(unit, tempEvalEventNode, false);
//                eid = this.eventsMapping.get(this.eventNumber);
//                TempEvalEvent tempEvalEvent = new TempEvalEvent(unit, tempEvalEventNode, eid);
//                if (event != null){
//                    temporalGraph.addRelation(new TemporalRelation(event,
//                            tempEvalEvent,
//                            TemporalRelation.EQUALS,
//                            "Special TEMPEVAL, te_event"));
//                } else {
//                    event = getClosestEvent(unit, tempEvalEventNode, temporalGraph,
//                            new Integer(-1), new Vector<XipNode>());
//                    if (event != null) {
//                        temporalGraph.addRelation(new TemporalRelation(event,
//                                tempEvalEvent,
//                                TemporalRelation.EQUALS,
//                                "Special TEMPEVAL, te_event"));
//                    }
//                }
//            }
//        }
        
    }
    
    /**
     * Gets the temporal graph date or duration corresponding to TempEval TIMEX3 in the text.
     */
    private static TemporalConstituent getCorrespondingDateOrDuration(TemporalGraph temporalGraph,
            XipUnit unit, long left, long right){
        Vector<TemporalElement> temporalElements = temporalGraph.getElements(unit.getSentenceNb());
        Vector<TemporalConstituent> candidates = new Vector<TemporalConstituent>();
//        Vector<TemporalElement> testcandidates = new Vector<TemporalElement>();
        TemporalConstituent bestCandidate = null;
        long lowerBound = 300;
        long upperBound = 0;
        // parse all elements in graph corresponding to this sentence.
        for (TemporalElement element : temporalElements){
            if (element instanceof TemporalConstituent){
                XipNode tempNode = element.getXipNode();
                if (tempNode != null){
                    long i = tempNode.getLeftTokOffset();
                    long j = tempNode.getRightTokOffset();
//                    if (i <= right && j >= left)
//                        testcandidates.add(element);
                    // if same or overlaping offsets, return the corresponding element
                    if (tempNode.getLeftTokOffset() == left && tempNode.getRightTokOffset() == right)
                        return (TemporalConstituent)element;
                    else if (i <= right && j >= left &&
                            (lowerBound >= right || upperBound <= left)){
                        candidates.add((TemporalConstituent)element);
                        lowerBound = java.lang.Math.min(lowerBound, left);
                        upperBound = java.lang.Math.max(upperBound, right);
                    }
                }
            }
        }
//        if (testcandidates.size() > 1)
//            throw new NotImplementedException();
        // if nothing is found, return null;
        if (candidates.isEmpty())
            return null;
        else if (candidates.size() == 1)
            return candidates.get(0);
        else
            throw new NotImplementedException();
    }
    
    /**
     * Gets the temporal graph event corresponding to TempEval Event in the text.
     */
    private static Event getClosestEvent(XipUnit unit, XipNode node, TemporalGraph temporalGraph,
            Integer limit, Vector<XipNode> visited){
        Vector<XipDependency> dependencies = node.getDependencies();
        dependencies = orderDependenciesByImportance(dependencies);
        Event candidateEvent = null;
        Integer newLimit;
        if (limit == 0)
            return null;
        else {
            for (XipDependency dependency : dependencies){
                Vector<XipNode> linkNodes = dependency.getParameters();
                for (XipNode linkNode : linkNodes){
                    if (!linkNode.equals(node) && !visited.contains(linkNode)){
                        // look if this node is an event in the graph
                        Event event = temporalGraph.getEvent(unit, linkNode, false);
                        if (event != null){
                            candidateEvent = event;
                            return candidateEvent;
                        } else {
                            newLimit = limit - 1;
                            visited.add(linkNode);
                            event = getClosestEvent(unit, linkNode, temporalGraph,
                                    newLimit, visited);
                            if (limit < 0 || newLimit < limit - 1)
                                candidateEvent = event;
                        }
                    }
                }
            }
        }
//        return candidateEvent;
        return null;
    }
    
    
    
    
    
    
    
    
    
    /**
     * Orders dependencies by importance, from most important to least important.
     * @param dependencies the dependencies to sort.
     */
    private static Vector<XipDependency> orderDependenciesByImportance(Vector<XipDependency> dependencies){
        Vector<XipDependency> high = new Vector<XipDependency>();
        Vector<XipDependency> medium = new Vector<XipDependency>();
        Vector<XipDependency> low = new Vector<XipDependency>();
        
        for (XipDependency dependency : dependencies){
            if (dependency.getName().equals("SUBJ") ||
                    dependency.getName().equals("SUBJ-N") ||
                    dependency.getName().equals("OBJ") ||
                    dependency.getName().equals("OBJ-N"))
                high.add(dependency);
            else if (dependency.getName().equals("MOD"))
                medium.add(dependency);
            else
                low.add(dependency);
        }
        high.addAll(medium);
        high.addAll(low);
        return high;
    }
    
    
}
