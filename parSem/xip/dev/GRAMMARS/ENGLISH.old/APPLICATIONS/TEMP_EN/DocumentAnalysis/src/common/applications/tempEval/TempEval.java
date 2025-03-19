/*
 * TempEval.java
 *
 * Created on February 23, 2007, 11:22 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common.applications.tempEval;

import common.Common;
import common.CommonLog;
import common.NotImplementedException;
import common.Options;
import common.TypedPropertyException;
import common.applications.*;
import documentAnalysis.temporality.TemporalElement;
import documentAnalysis.temporality.TemporalGraph;
import documentAnalysis.temporality.TemporalRelation;
import documentAnalysis.events.Event;
import documentAnalysis.temporality.vaguecalendar.date.VagueDate;
import documentAnalysis.temporality.vaguecalendar.duration.Duration;
import java.io.File;
import java.io.IOException;
import java.util.Vector;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.xml.sax.SAXException;

/**
 * Class for TempEval-specific application methods
 * @author xtannier
 */
public class TempEval extends Application {
    
    
//    public static int RANDOM_TASK_A = TemporalRelation.OVERLAPS;
    public static int RANDOM_TASK_A = TemporalRelation.OVERLAPS;
//    public static int RANDOM_TASK_B = TemporalRelation.BEFORE;
    public static int RANDOM_TASK_B = TemporalRelation.BEFORE;
    public static int RANDOM_TASK_C = TemporalRelation.OVERLAPS;
    
    /** Creates a new instance of TempEval */
    public TempEval() {
        super();
        this.name = "TEMPEVAL";
    }
    
    
    /** Creates a new instance of TempEval
     *
     * @param options the parer options
     */
    public TempEval(Options options) {
        this();
    }
    
    
    /**
     * Returns an output file name from the input file.
     *
     * @param inputFileName the name of the input file
     * @return the name of the output file
     */
    public String getOutputFileName(String inputFileName){
        if (inputFileName.endsWith(".tml")){
            return inputFileName;
        } else
            return super.getOutputFileName(inputFileName);
    }
    
    
    /**
     * Returns an output file name from the input file. The new file name
     * will be located in the specified directory.
     *
     * @param inputFileName the name of the input file
     * @return the name of the output file
     */
    public String getOutputFileName(String inputFileName, String dirName){
        if (dirName == null)
            return this.getOutputFileName(inputFileName);
        
        String abstractName = new File(inputFileName).getName();
        File dir = new File(dirName);
        if (!dir.exists())
            dir.mkdir();
        
        if (abstractName.endsWith(".tml")){
            return dir.getAbsolutePath() + File.separator + abstractName;
        } else
            return super.getOutputFileName(inputFileName, dirName);
    }
    
    
    /**
     * Open the output file if necessary and perform any appropriate operation
     * in the output.
     *
     * @param options the parser options.
     */
    public void closeOutput(Options options){
        String taskType = options.getProperty("CORPUS_TASK");
        String dctId = options.getProperty("CORPUS_DCTID");
        String inputFileName = options.getInputFileName();
        String fileName = options.getOutputFileName();
        if (fileName == null){
            fileName = options.getApplication().getOutputFileName(inputFileName, options.getOutputDirName());
        }
        int intLinkId = 1;
        
        if (fileName != null){
            try {
                //Create a factory of DOM's builder
                DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
                DocumentBuilder builder;
                
                builder = factory.newDocumentBuilder();
                
                
                //Reading XML file with DOM
                Document document;
                document = builder.parse(inputFileName);
                Node root = document.getDocumentElement();
                
                // get relations to add at the end of the document
                TemporalGraph temporalGraph = options.getTemporalGraph();
//                Vector<TemporalRelation> relations = temporalGraph.getRelations();
                Vector<Event> events = temporalGraph.getEvents();
                for (Event event : events){
                    if (event instanceof TempEvalEvent){
                        intLinkId = getRelationElementsForEvent((TempEvalEvent)event,
                                document, taskType, dctId, intLinkId, temporalGraph, options);
                    }
                }
//                for (TemporalRelation relation : relations){
//                    ++intLinkId;
//                    Element tlink = getRelationElement(relation,
//                            document, taskType, intLinkId);
//                    if (tlink != null)
//                        root.appendChild(tlink);
//                }
                
                
                Common.printXMLDocument(document, fileName);
                CommonLog.infoMessage("Output XML file has been written into " + fileName);
                
            } catch (ParserConfigurationException ex) {
                CommonLog.errorMessage(ex.getMessage());
                System.exit(-1);
            } catch (IOException ex) {
                CommonLog.errorMessage(ex.getMessage());
                System.exit(-1);
            } catch (SAXException ex) {
                CommonLog.errorMessage(ex.getMessage());
                System.exit(-1);
            } catch (TypedPropertyException ex) {
                CommonLog.errorMessage(ex.getMessage());
                System.exit(-1);
            }
        }
        options.getTemporalGraph().clear();
    }
    
    
    private int getRelationElementsForEvent(TempEvalEvent event,
            Document document,
            String taskType,
            String dctId,
            int intLinkId,
            TemporalGraph temporalGraph, Options options) throws TypedPropertyException{
        
        Node root = document.getDocumentElement();
        
//        try{
        // task AB
        if (taskType.equals("AB")){
            /************
             * task A
             ***********/
            //   need a relation with all TIMEX3 of the same sentence
            // DATEs
            Vector<VagueDate> dates = temporalGraph.getDates(event.getSentenceNumber());
            for (VagueDate date : dates){
                if (date instanceof TempEvalDate){
                    if (options.getBooleanProperty("CORPUS_ALL_RANDOM_RELATIONS")) {
                        TemporalRelation relation = new TemporalRelation(event, date, RANDOM_TASK_A);
                        root.appendChild(getRelationElement(relation, document, "A", dctId, intLinkId++, options));
                        root.appendChild(document.createTextNode("\n"));
                    } else {
                        TemporalRelation relation = temporalGraph.getRelation(event, date);
                        if (relation != null){
                            root.appendChild(getRelationElement(relation, document, "A", dctId, intLinkId++, options));
                            root.appendChild(document.createTextNode("\n"));
                        }
                    }
                }
            }
            // DURATIONs
            Vector<Duration> durations = temporalGraph.getDurations(event.getSentenceNumber());
            for (Duration duration : durations){
                if (duration instanceof TempEvalDuration){
                    if (options.getBooleanProperty("CORPUS_ALL_RANDOM_RELATIONS")) {
                        TemporalRelation relation = new TemporalRelation(event, duration, RANDOM_TASK_A);
                        root.appendChild(getRelationElement(relation, document, "A", dctId, intLinkId++, options));
                        root.appendChild(document.createTextNode("\n"));
                    } else {
                        TemporalRelation relation = temporalGraph.getRelation(event, duration);
                        if (relation != null) {
                            root.appendChild(getRelationElement(relation, document, "A", dctId, intLinkId++, options));
                            root.appendChild(document.createTextNode("\n"));
                        }
                    }
                }
            }
            
            /************
             * task B
             ***********/
            // need a relation between the event and the DCT
            VagueDate dct = temporalGraph.getDCT();
//                Event speechTime = temporalGraph.getSpeechTime();
            
            if (options.getBooleanProperty("CORPUS_ALL_RANDOM_RELATIONS")) {
                TemporalRelation relation = new TemporalRelation(event, dct, RANDOM_TASK_B);
                root.appendChild(getRelationElement(relation, document, "B", dctId, intLinkId++, options));
                root.appendChild(document.createTextNode("\n"));
            } else {
                
                TemporalRelation relationDCT = temporalGraph.getRelation(event, dct);
//                TemporalRelation relationST = null;
//                TemporalRelation relation;
//            Event speechTime = temporalGraph.getSpeechTime();
//            relationDCT = temporalGraph.getRelation(event, dct);
//                relationST = temporalGraph.getRelation(event, speechTime);
//                boolean random = false;
                
//                ++test;
//                if (test == 4)
//                    System.err.println(test);
//
                if (relationDCT != null) {
                    root.appendChild(getRelationElement(relationDCT, document, "B", dctId, intLinkId++, options));
                    root.appendChild(document.createTextNode("\n"));
                }
            }
            
//                if (options.getBooleanProperty("CORPUS_ALL_RANDOM_RELATIONS")){
//                    relationDCT = new TemporalRelation(event, dct, RANDOM_TASK_B,
//                            TemporalRelation.RANDOM, "random");
//                    root.appendChild(getRelationElement(relationDCT, document, "B", intLinkId++));
//                    root.appendChild(document.createTextNode("\n"));
//                } else {
//                    if (relationDCT != null){
//                        if (relationST != null) {
//                            if ((relationDCT.getRelation() ==
//                                    (TemporalRelation.BEFORE | TemporalRelation.OVERLAPS | TemporalRelation.DURING))
//                                    && relationST.getRelation() == TemporalRelation.BEFORE)
//                                relationDCT.setRelation(TemporalRelation.BEFORE);
//                        }
//                    } else {
//                        if (relationST == null && options.getBooleanProperty("CORPUS_PRIORITY_TO_RECALL")){
//                            relationDCT = new TemporalRelation(event, dct, RANDOM_TASK_B,
//                                    TemporalRelation.RANDOM, "random");
//                            random = true;
//                        } else if (relationST == null && !options.getBooleanProperty("CORPUS_PRIORITY_TO_RECALL")) {
//                            relationDCT = new TemporalRelation(event, dct, TemporalRelation.UNDEFINED,
//                                    TemporalRelation.RANDOM, "random");
//                            random = true;
//                        } else
//                            relationDCT = new TemporalRelation(event, dct, relationST.getRelation());
//                    }
//                    if (!random || options.getBooleanProperty("CORPUS_PRIORITY_TO_RECALL")){
//                        root.appendChild(getRelationElement(relationDCT, document, "B", intLinkId++));
//                        root.appendChild(document.createTextNode("\n"));
//                    }
//                }
        } else if (taskType.equals("C")) {
            /************
             * task C
             ***********/
            //   need a relation with the event of previous sentence
            if (event.getSentenceNumber() > 1){
                Vector<Event> events = temporalGraph.getEvents(event.getSentenceNumber() - 1);
                Vector<Event> tempEvalEvents = new Vector<Event>();
                for (Event test : events) {
                    if (test instanceof TempEvalEvent)
                        tempEvalEvents.add(test);
                }
                if (tempEvalEvents == null || tempEvalEvents.size() > 1)
                    throw new NotImplementedException();
                else if (!tempEvalEvents.isEmpty()) {
                    Event previousEvent = tempEvalEvents.get(0);
                    if (options.getBooleanProperty("CORPUS_ALL_RANDOM_RELATIONS")) {
                        TemporalRelation relation = new TemporalRelation(previousEvent, event, RANDOM_TASK_C);
                        root.appendChild(getRelationElement(relation, document, "C", dctId, intLinkId++, options));
                        root.appendChild(document.createTextNode("\n"));
                    } else {
                        TemporalRelation relation = temporalGraph.getRelation(previousEvent, event);
                        if (relation == null && options.getBooleanProperty("CORPUS_PRIORITY_TO_RECALL")){
                            TemporalRelation randomRelation = new TemporalRelation(previousEvent, event, RANDOM_TASK_C);
                            root.appendChild(getRelationElement(randomRelation, document, "C", dctId, intLinkId++, options));
                            root.appendChild(document.createTextNode("\n"));
                        } else if (relation != null) {
                            root.appendChild(getRelationElement(relation, document, "C", dctId, intLinkId++, options));
                            root.appendChild(document.createTextNode("\n"));
                        }
                    }
                }
            }
        }
        
        return intLinkId;
//        } catch (common.TypedPropertyException ex) {
//            CommonLog.errorMessage(ex.getMessage());
//            return -1;
//        }
    }
    
    
    
    /**
     * Returns the XML TLINK element representing temporal relation
     * in TempEval format.
     */
    private Element getRelationElement(TemporalRelation relation,
            Document document,
            String taskType,
            String dctId,
            int intLinkId, Options options) throws TypedPropertyException{
        Element linkNode = null;
        TemporalElement element1 = relation.getTemporalElement1();
        TemporalElement element2 = relation.getTemporalElement2();
        // task A
        if (taskType.equals("A")){
            if (element1 instanceof TempEvalEvent &&
                    (element2 instanceof TempEvalDate || element2 instanceof TempEvalDuration)){
//                linkNode = document.createElement("MY_TLINK");
                linkNode = document.createElement("TLINK");
                // link id
                linkNode.setAttribute("lid", "l" + String.valueOf(intLinkId));
                // relation type
                linkNode.setAttribute("relType",
                        convertRelation(relation,
                        options.getBooleanProperty("CORPUS_PRIORITY_TO_RECALL"), taskType));
                // event ID
                linkNode.setAttribute("eventID", ((TempEvalEvent)element1).getId());
                // relatedToTime
                if (element2 instanceof TempEvalDate)
                    linkNode.setAttribute("relatedToTime", ((TempEvalDate)element2).getId());
                else if (element2 instanceof TempEvalDuration)
                    linkNode.setAttribute("relatedToTime", ((TempEvalDuration)element2).getId());
                // task
                linkNode.setAttribute("task", "A");
            }
            // task A (inverse relation)
            else if (element2 instanceof TempEvalEvent &&
                    (element1 instanceof TempEvalDate || element1 instanceof TempEvalDuration)){
                linkNode = getRelationElement(relation.inverse(), document, taskType, dctId, intLinkId, options);
            }
        }
        // task B
        else if (taskType.equals("B")){
            if (element1 instanceof TempEvalEvent &&
                    (element2 instanceof VagueDate && ((VagueDate)element2).isDCT())){
//                linkNode = document.createElement("MY_TLINK");
                linkNode = document.createElement("TLINK");
                // link id
                linkNode.setAttribute("lid", "l" + String.valueOf(intLinkId));
                // relation type
                linkNode.setAttribute("relType",
                        convertRelation(relation,
                        options.getBooleanProperty("CORPUS_PRIORITY_TO_RECALL"), taskType));
                // event ID
                linkNode.setAttribute("eventID", ((TempEvalEvent)element1).getId());
                // relatedToTime
                // DCT ID
                linkNode.setAttribute("relatedToTime", dctId);
                // task
                linkNode.setAttribute("task", "B");
            }
            // task B (inverse relation)
            else if (element2 instanceof TempEvalEvent &&
                    element1 instanceof Event && ((Event)element1).isSpeechTime()){
                linkNode = getRelationElement(relation.inverse(), document, taskType, dctId, intLinkId, options);
            }
        }
        // task C
        else if (taskType.equals("C")){
            if (element1 instanceof TempEvalEvent &&
                    element2 instanceof TempEvalEvent){
//                linkNode = document.createElement("MY_TLINK");
                linkNode = document.createElement("TLINK");
                // link id
                linkNode.setAttribute("lid", "l" + String.valueOf(intLinkId));
                // relation type
                linkNode.setAttribute("relType",
                        convertRelation(relation,
                        options.getBooleanProperty("CORPUS_PRIORITY_TO_RECALL"), taskType));
                // event1 ID
                linkNode.setAttribute("eventID", ((TempEvalEvent)element1).getId());
                // relatedToEvent
                // event2 ID
                linkNode.setAttribute("relatedToEvent", ((TempEvalEvent)element2).getId());
                // task
                linkNode.setAttribute("task", "C");
            }
        }//        if (relation.getComment().equals("random"))
//            linkNode.setAttribute("random", "yes");
        
        return linkNode;
    }
    
    
    
    
    /**
     * Converts regular relation into TempEval relation.
     * @param relation the integer value of the relation
     * @param recallFirst is true if the priority should be given to the recall
     * @return the String value of the TempEval relation
     */
    private static String convertRelation(TemporalRelation relation, boolean recallFirst, String task){
        int rel = relation.getRelation();
        
        if (rel == TemporalRelation.UNDEFINED)
            return "VAGUE";
        else if (rel == TemporalRelation.AFTER)
            return "AFTER";
        else if (rel == TemporalRelation.BEFORE)
            return "BEFORE";
        else if (TemporalRelation.containsRelation(rel, TemporalRelation.BEFORE) &&
                !TemporalRelation.containsRelation(rel, TemporalRelation.AFTER)){
            
            // OVERLAP-OR-BEFORE relations obtained by composition
            // are turned into BEFORE
//            if (relation.getComment().toUpperCase().contains("COMPOSITION"))
//                return "BEFORE";
//
            
            if (!recallFirst)
                return "BEFORE-OR-OVERLAP";
            else if (task.equals("A"))
                return "OVERLAP";
            else if (task.equals("B"))
                return "BEFORE";
            else if (task.equals("C"))
                return "BEFORE-OR-OVERLAP";
            else
                throw new NotImplementedException();
        } else if (TemporalRelation.containsRelation(rel, TemporalRelation.AFTER) &&
                !TemporalRelation.containsRelation(rel, TemporalRelation.BEFORE)) {
            
            // OVERLAP-OR-AFTER relations obtained by composition
            // are turned into AFTER
//            if (task.equals("B") &&
//                    relation.getPriority() > -8 && relation.getPriority() != TemporalRelation.TENSE)
//                return "AFTER";
//
//            if (!recallFirst)
//                return "OVERLAP-OR-AFTER";
//            else if (task.equals("A"))
//                return "OVERLAP";
//            else if (task.equals("B"))
//                return "OVERLAP";
//            else if (task.equals("C"))
            return "OVERLAP-OR-AFTER";
//            else
//                throw new NotImplementedException();
        } else
            return "OVERLAP";
    }
    
    
    
}
