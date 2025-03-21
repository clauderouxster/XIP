/*
 * TimeForEvents.java
 *
 * Created on July 5, 2007, 3:04 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common.applications.timeForEvents;

import com.xerox.jxip.XipDependency;
import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipUnit;
import common.CommonLog;
import common.Options;
import common.TypedPropertyException;
import common.applications.*;
import common.corpora.Corpus;
import common.database.DataBaseConnection;
import documentAnalysis.temporality.TemporalElement;
import documentAnalysis.temporality.TemporalGraph;
import documentAnalysis.temporality.TemporalRelation;
import documentAnalysis.events.Event;
import documentAnalysis.temporality.vaguecalendar.date.VagueDate;
import java.util.Vector;



/**
 * This class contains methods for the use of Temporal Expressions for
 * Named Entity Disambiguation
 * @author xtannier
 */
public class TimeForEvents extends Application{
    
    private DataBaseConnection connection;
    /**
     * Task type.
     * Can be:
     * <ul>
     *   <li>BUILD_TEMP_EN_BASE: use the corpus to build a base for work on Named Entities
     *              with temporal expressions
     *   <li>ANALYSE_TEMP_EN: use the corpus to desambiguate Named Entities with temporal expressions
     */
    private String taskType;
    /** List of SQL commands to be run at the end of the document */
//    private Vector<String> commands;
    /** List of NE dependencies */
//    private Vector<Pair<XipUnit, Vector<XipDependency>>> neDependencies;
    /** add the connection to DB or not */
    private boolean dbConnection;
    
    
    private static String APP_TASK = "APP_TASK";
    private static String APP_TEMP_EN_TABLE = "APP_TEMP_EN_TABLE";
    private static String APP_TEMP_EN_BASE = "APP_TEMP_EN_BASE";
    private static String APP_TEMP_EN_LOGIN = "APP_TEMP_EN_LOGIN";
    private static String APP_TEMP_EN_PASSWORD = "APP_TEMP_EN_PASSWD";
    private static String APP_DBCONNECTION = "APP_DBCONNECTION";
    private static Vector<String> timeForEventsArguments;
    
    
    static{
        timeForEventsArguments = new Vector<String>();
        timeForEventsArguments.add(APP_TASK);
        timeForEventsArguments.add(APP_TEMP_EN_TABLE);
        timeForEventsArguments.add(APP_TEMP_EN_BASE);
        timeForEventsArguments.add(APP_TEMP_EN_LOGIN);
        timeForEventsArguments.add(APP_TEMP_EN_PASSWORD);
        timeForEventsArguments.add(APP_DBCONNECTION);
        
    }
    
    
    
    /**
     * Creates a new instance of TimeForEvents
     */
    public TimeForEvents() {
        super();
        this.name = "TimeForEvents";
        this.taskType = "";
        this.connection = null;
//        this.neDependencies = new Vector<Pair<XipUnit, Vector<XipDependency>>>();
        this.dbConnection = true;
    }
    
    
    
    /**
     * Creates a new instance of TimeForEvents
     *
     * @param options the parer options
     */
    public TimeForEvents(Options options) {
        this();
    }
    
    
    /**
     * Treats application-specific properties.
     *
     * @param name the name of the property
     * @param value the value of the property
     * @return -1 if the treatment failed (basically if the property is unknown), 0 otherwise.
     */
    public int setProperty(String name, String value){
        name = name.toUpperCase();
        // Application task
        if (name.equals("APP_TASK")){
            if (value.toUpperCase().equals("BUILD_TEMP_EN_BASE") ||
                    value.toUpperCase().equals("ANALYSE_TEMP_EN")) {
                this.taskType = value.toUpperCase();
                return 0;
            } else {
                CommonLog.errorMessage("TimeForEvents task " + value + " specified by " + name + " is unknown");
                return -1;
            }
        } else if (timeForEventsArguments.contains(name)){
            return 0;
        } else
            return -1;
    }
    
    
    /**
     * Open the output connection
     * in the output.
     *
     * @param options the parser options.
     */
    public void openOutput(Options options){
        String hostname = options.getProperty(Options.MYSQL_HOSTNAME);
        String database = options.getProperty(APP_TEMP_EN_BASE);
        String login = options.getProperty(APP_TEMP_EN_LOGIN);
        String password = options.getProperty(APP_TEMP_EN_PASSWORD);
        String pilote = options.getProperty(Options.MYSQL_PILOTE);
        try {
            if (options.getBooleanProperty(APP_DBCONNECTION, true)) {
                this.connection = new DataBaseConnection(pilote, hostname, database, login, password);
            } else {
                this.dbConnection = false;
            }
        } catch (TypedPropertyException ex) {
            this.connection = new DataBaseConnection(pilote, hostname, database, login, password);
        }
    }
    
    
    /**
     * Update the output according to the new information brought by the current sentence
     *
     * @param unit the currently parsed XipUnit
     * @param options the parser options.
     */
    public void updateOutput(XipUnit unit, Options options){
        String documentId = options.getProperty(Corpus.CORPUS_DOCUMENT_ID);
        
        if (dbConnection){
            String table = options.getProperty(APP_TEMP_EN_TABLE);
            VagueDate dct = options.getDct();
            if (connection == null)
                this.openOutput(options);
            
            if (this.taskType.equals("BUILD_TEMP_EN_BASE")){
                TemporalGraph temporalGraph = options.getTemporalGraph();
                String command;
                String lemma;
                boolean done;
                
                // get verbs
                Vector<XipNode> verbs = unit.getNodesByCategory("verb");
                // FOR ALL VERBS
                for (XipNode verb : verbs) {
                    
                    // get SUBJ-N, OBJ-N, IOBJ-N
                    Vector<XipDependency> subjects = verb.getDependenciesByName("SUBJ-N", 1);
                    if (subjects.isEmpty()) subjects.add(null);
                    Vector<XipDependency> objects = verb.getDependenciesByName("OBJ-N", 1);
                    if (objects.isEmpty()) objects.add(null);
                    Vector<XipDependency> iobjects = verb.getDependenciesByName("IOBJ", 1);
                    if (iobjects.isEmpty()) iobjects.add(null);
                    
                    String strSubject, strObject, strIObject;
                    for (XipDependency subject : subjects) {
                        if (subject != null && !subject.getArg(2).hasFeature("pron"))
                            strSubject = subject.getArg(2).getLemma().replaceAll("'", " ");
                        else
                            strSubject = "";
                        for (XipDependency object : objects) {
                            if (object != null && !object.getArg(2).hasFeature("pron"))
                                strObject = object.getArg(2).getLemma().replaceAll("'", " ");
                            else
                                strObject = "";
                            for (XipDependency iobject : iobjects) {
                                if (iobject != null && !iobject.getArg(2).hasFeature("pron"))
                                    strIObject = iobject.getArg(2).getLemma().replaceAll("'", " ");
                                else
                                    strIObject = "";
                                if (!(strSubject.equals("") &&
                                        strObject.equals("") &&
                                        strIObject.equals("")) && 
                                        !verb.hasFeature("cop") &&
                                        !verb.hasFeature("fictivenode")) {
                                    lemma = verb.getLemma().replaceAll("'", " ");
                                    // DATE from the grammar
                                    // get event
                                    Event event = temporalGraph.getEvent(unit, verb, false);
                                    if (event != null){
                                        // get all temporal relations
                                        Vector<TemporalRelation> relations = event.getTemporalRelations();
                                        for (TemporalRelation relation : relations){
                                            // treat only relations DURING with an absolute date
                                            TemporalElement otherElement = relation.getOtherElement(event);
                                            if (otherElement instanceof VagueDate &&
                                                    ((VagueDate)otherElement).isAbsolute() &&
                                                    (!relation.containsRelation(TemporalRelation.AFTER) &&
                                                    !relation.containsRelation(TemporalRelation.BEFORE))) {
                                                // check DateValue length
                                                if (((VagueDate)otherElement).getValue().length() > 200) {
                                                    CommonLog.warningMessage("Exception, date value is too big, skip it.");
                                                } else {
                                                    
                                                    
                                                    // No, subj, verb lemma, obj, iobj, DateValue, DocNo, doc file, DCT
                                                    command = "INSERT INTO " + table + " values(0, '" +
                                                            strSubject + "', '" +
                                                            lemma + "', '" +
                                                            strObject + "', '" +
                                                            strIObject + "', '" +
                                                            ((VagueDate)otherElement).getValue() + "', '" +
                                                            documentId + "', '" + options.getInputFileName() + "', false);";
//                                                    System.err.println("command = " + command );
                                                    this.connection.executeUpdate(command);
                                                }
                                            }
                                        }
                                    }
                                    /**************
                                     *   DCT link
                                     **************/
                                    if (options.getDct() != null){
                                        command = "INSERT INTO " + table + " values(0, '" +
                                                strSubject + "', '" +
                                                lemma + "', '" +
                                                strObject + "', '" +
                                                strIObject + "', '" +
                                                dct.getValue() + "', '" +
                                                documentId + "', '" + options.getInputFileName() + "', true);";
//                                        System.err.println("command = " + command );
                                        this.connection.executeUpdate(command);
                                    }
                                }
                            }
                        }
                    }
                    done = true;
                    break;
                }
                
                
                
//                // get nouns
//                Vector<XipNode> nouns = unit.getNodesByCategory("noun");
//                // FOR ALL NOUNS
//                for (XipNode noun : nouns) {
//                    // get MOD + PREPOF
//                    Vector<XipDependency> mods = noun.getDependenciesByName("MOD", 1);
//                    if (mods.isEmpty()) mods.add(null);
//
//                    String strMod = "";
//                    for (XipDependency mod : mods) {
//                        if (mod != null) {
//                            XipNode nodeMod = mod.getArg(2);
//                            Vector<XipDependency> prepds = nodeMod.getDependenciesByName("PREPD");
//                            for (XipDependency prepd : prepds) {
//                                if (prepd.getArg(2).hasFeatures("form:fof")) {
//                                    strMod = nodeMod.getLemma();
//                                }
//                            }
//                        }
//                        if (!strMod.equals("")) {
//                            lemma = noun.getLemma().replaceAll("'", " ");
//
//                            // get event
//                            Event event = temporalGraph.getEvent(unit, noun, false);
//                            if (event != null){
//
//                                // get all temporal relations
//                                Vector<TemporalRelation> relations = event.getRelations();
//                                for (TemporalRelation relation : relations){
//                                    // treat only relations DURING with an absolute date
//                                    TemporalElement otherElement = relation.getOtherElement(event);
//                                    if (otherElement instanceof VagueDate &&
//                                            ((VagueDate)otherElement).isAbsolute() &&
//                                            (!relation.containsRelation(TemporalRelation.AFTER) &&
//                                            !relation.containsRelation(TemporalRelation.BEFORE))) {
//                                        // check DateValue length
//                                        if (((VagueDate)otherElement).getValue().length() > 200) {
//                                            CommonLog.warningMessage("Exception, date value is too big, skip it.");
//                                        } else {
//
//
//
//                                            // No, subj, verb lemma, obj, iobj, DateValue, DocNo, doc file, DCT
//                                            command = "INSERT INTO " + table + " values(0, '" +
//                                                    "', '" +
//                                                    lemma + "', '" +
//                                                    strMod + "', '" +
//                                                    "', '" +
//                                                    ((VagueDate)otherElement).getValue() + "', '" +
//                                                    documentId + "', '" + options.getInputFileName() + "', false);";
//                                            this.connection.executeUpdate(command);
//                                        }
//                                    }
//                                }
//                            }
//                            /**************
//                             *   DCT link
//                             **************/
//                            if (options.getDct() != null){
//                                command = "INSERT INTO " + table + " values(0, '" +
//                                        "', '" +
//                                        lemma + "', '" +
//                                        strMod + "', '" +
//                                        "', '" +
//                                        dct.getValue() + "', '" +
//                                        documentId + "', '" + options.getInputFileName() + "', true);";
//                                this.connection.executeUpdate(command);
//                            }
//                        }
//                    }
//                    done = true;
//                    break;
//                }
            }
        }
    }
    
    
//    /**
//     * Prints a summary of elements that take memory
//     */
//    public void showElements() {
//        System.out.println("neDependencies = " + this.neDependencies.size());
//    }
    
    
}
