/*
 * TimeForEntities.java
 *
 * Created on July 5, 2007, 3:04 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package common.applications.timeForEntities;

import com.xerox.jxip.XipDependency;
import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipUnit;
import common.CommonLog;
import common.Options;
import common.Pair;
import common.TypedPropertyException;
import common.applications.*;
import common.corpora.Corpus;
import common.database.DataBaseConnection;
import documentAnalysis.temporality.TemporalElement;
import documentAnalysis.temporality.TemporalRelation;
import documentAnalysis.events.Event;
import documentAnalysis.temporality.vaguecalendar.date.VagueDate;
import java.util.Vector;



/**
 * This class contains methods for the use of Temporal Expressions for
 * Named Entity Disambiguation
 * @author xtannier
 */
public class TimeForEntities extends Application{
    
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
    private Vector<Pair<XipUnit, Vector<XipDependency>>> neDependencies;
    /** add the connection to DB or not */
    private boolean dbConnection;
    
    
    private static String APP_TASK = "APP_TASK";
    private static String APP_TEMP_EN_TABLE = "APP_TEMP_EN_TABLE";
    private static String APP_TEMP_EN_BASE = "APP_TEMP_EN_BASE";
    private static String APP_TEMP_EN_LOGIN = "APP_TEMP_EN_LOGIN";
    private static String APP_TEMP_EN_PASSWORD = "APP_TEMP_EN_PASSWD";
    private static String APP_DBCONNECTION = "APP_DBCONNECTION";
    private static Vector<String> wsjArguments;
    private static Vector<String> entityTypes;
    
    
    static{
        wsjArguments = new Vector<String>();
        wsjArguments.add(APP_TASK);
        wsjArguments.add(APP_TEMP_EN_TABLE);
        wsjArguments.add(APP_TEMP_EN_BASE);
        wsjArguments.add(APP_TEMP_EN_LOGIN);
        wsjArguments.add(APP_TEMP_EN_PASSWORD);
        wsjArguments.add(APP_DBCONNECTION);
        
        entityTypes = new Vector<String>();
        entityTypes.add("ORGANISATION");
        entityTypes.add("LOCATION");
        entityTypes.add("LOCORG");
        entityTypes.add("PERSON");
        entityTypes.add("LIXO");
    }
    
    
    
    /** Creates a new instance of TimeForEntities */
    public TimeForEntities() {
        super();
        this.name = "TimeForEntities";
        this.taskType = "";
        this.connection = null;
        this.neDependencies = new Vector<Pair<XipUnit, Vector<XipDependency>>>();
        this.dbConnection = true;
    }
    
    
    
    /** Creates a new instance of TimeForEntities
     *
     * @param options the parer options
     */
    public TimeForEntities(Options options) {
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
                CommonLog.errorMessage("TimeForEntities task " + value + " specified by " + name + " is unknown");
                return -1;
            }
        } else if (wsjArguments.contains(name)){
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
        String table = options.getProperty(APP_TEMP_EN_TABLE);
        if (connection == null)
            this.openOutput(options);
        String command;
        if (this.taskType.equals("BUILD_TEMP_EN_BASE")){
            Vector<XipDependency> auxDep = new Vector<XipDependency>();
            // for all types of entities
            for (String type : entityTypes){
                // get unambiguous entities
                Vector<XipDependency> dependencies = unit.getDependenciesByName(type);
                if (!dependencies.isEmpty())
                    auxDep.addAll(dependencies);
            }
            
//            // get all proper name that are NOT entities
//            Vector<XipNode> pnNodes = unit.getNodesByFeature("proper", "+");
//            boolean stay = true;
//            for (XipNode pnNode : pnNodes) {
//                // not a NE dependency
//                // not included into a node that has a NE dependency
//                for (String type : entityTypes) {
//                    if (pnNode.hasDependency(type))
//                        stay = false;
//                    if (!pnNode.getDescendantsByDependency(type).isEmpty())
//                        stay = false;
//                }
//                // not included into another PN
//                if (!pnNode.getDescendantsByFeature("proper").isEmpty())
//                    stay = false;
//
//                if (stay) {
//                    // No, EntityValue, EntityType, DateValue, DocNo, SentenceNo, DCT
//                    command = "INSERT INTO " + table + " values(0, '" +
//                            pnNode.getSurface() + "', '" + "NONE" + "', '" +
//                            ((VagueDate)otherElement).getValue() + "', '" +
//                            documentId + "', '" + options.getInputFileName() + "', false);";
//
//                }
//
//            }
            this.neDependencies.add(new Pair<XipUnit, Vector<XipDependency>>(unit, auxDep));
        }
    }
    
    
    
    public void updateOutput(Options options){
        String documentId = options.getProperty(Corpus.CORPUS_DOCUMENT_ID);
        if (dbConnection){
            String table = options.getProperty(APP_TEMP_EN_TABLE);
            VagueDate dct = options.getDct();
            if (connection == null)
                this.openOutput(options);
            boolean done;
            
            if (this.taskType.equals("BUILD_TEMP_EN_BASE")){
                // for all found units containing dependencies
                for (Pair<XipUnit, Vector<XipDependency>> pair : this.neDependencies){
                    XipUnit unit = pair.getFirst();
                    String text = unit.getSentenceString().substring(0, java.lang.Math.min(999, unit.getSentenceString().length())).replaceAll("'", " ");
                    for (XipDependency dependency : pair.getSecond()) {
                        XipNode node = dependency.getArg(1);
                        String lemma = node.getLemma().replaceAll("'", " ");
                        done = false;
                        if (lemma.equalsIgnoreCase("WALL_STREET_JOURNAL") || lemma.equalsIgnoreCase("J"))
                            break;
                        // restrict the length to 200 characters (DB field limit and anyway...)
                        if (lemma.length() >= 200)
                            break;
                        String command;
                        /**************
                         * Direct link
                         **************/
                        Event event = options.getTemporalGraph().getEvent(unit, node, false);
                        if (event != null){
                            Vector<TemporalRelation> relations = event.getTemporalRelations();
                            for (TemporalRelation relation : relations){
                                TemporalElement otherElement = relation.getOtherElement(event);
                                if (otherElement instanceof VagueDate &&
                                        ((VagueDate)otherElement).isAbsolute() &&
//                                    !((VagueDate)otherElement).equals(dct) &&
                                        (!relation.containsRelation(TemporalRelation.AFTER) &&
                                        !relation.containsRelation(TemporalRelation.BEFORE))) {
                                    // check DateValue length
                                    if (((VagueDate)otherElement).getValue().length() > 200) {
                                        CommonLog.warningMessage("Exception, date value is too big, skip it.");
                                    } else {
                                        // No, EntityValue, EntityType, DateValue, DocNo, File, Sentence, DCT
                                        command = "INSERT INTO " + table + " values(0, '" +
                                                lemma + "', '" + dependency.getName() + "', '" +
                                                ((VagueDate)otherElement).getValue() + "', '" +
                                                documentId + "', '" + options.getInputFileName() + "', '" +
                                                text + "', false);";
                                        this.connection.executeUpdate(command);
                                        done = true;
                                        break;
                                    }
                                }
                            }
                        }
                        /****************
                         * Indirect link
                         ****************/
                        Vector<XipDependency> intermediateDependencies = node.getDependencies();
                        if (!done){
                            for (XipDependency intermediateDependency : intermediateDependencies){
                                Vector<XipNode> intermediateNodes = intermediateDependency.getParameters();
                                for (XipNode intermediateNode : intermediateNodes){
                                    event = options.getTemporalGraph().getEvent(unit, intermediateNode, false);
                                    if (event != null){
                                        Vector<TemporalRelation> relations = event.getTemporalRelations();
                                        for (TemporalRelation relation : relations){
                                            TemporalElement otherElement = relation.getOtherElement(event);
                                            if (otherElement instanceof VagueDate &&
                                                    ((VagueDate)otherElement).isAbsolute() &&
//                                            !((VagueDate)otherElement).equals(dct) &&
                                                    (!relation.containsRelation(TemporalRelation.AFTER) &&
                                                    !relation.containsRelation(TemporalRelation.BEFORE))) {
                                                // check DateValue length
                                                if (((VagueDate)otherElement).getValue().length() > 200) {
                                                    CommonLog.warningMessage("Date value is too big, skip it.");
                                                } else {
                                                    // No, EntityValue, EntityType, DateValue, DocNo, File, Sentence, DCT
                                                    command = "INSERT INTO " + table + " values(0, '" +
                                                            lemma + "', '" + dependency.getName() + "', '" +
                                                            ((VagueDate)otherElement).getValue() + "', '" +
                                                            documentId + "', '" + options.getInputFileName() + "', '" +
                                                            text + "', false);";
                                                    this.connection.executeUpdate(command);
                                                    done = true;
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                    if (done)
                                        break;
                                }
                                if (done)
                                    break;
                            }
                        }
                        /**************
                         *   DCT link
                         **************/
                        if (options.getDct() != null){
                            // No, EntityValue, EntityType, DateValue, DocNo, File, Sentence, DCT
                            command = "INSERT INTO " + table + " values(0, '" +
                                    lemma + "', '" + dependency.getName() + "', '" +
                                    dct.getValue() + "', '" +
                                    documentId + "', '" + options.getInputFileName() + "', '" +
                                    text + "', true);";
                            this.connection.executeUpdate(command);
//                        this.commands.add(command);
                        }
                    }
                }
            }
            this.neDependencies.clear();
        }
    }
    
    
    /**
     * Prints a summary of elements that take memory
     */
    public void showElements() {
        System.out.println("neDependencies = " + this.neDependencies.size());
    }
    
    
}
