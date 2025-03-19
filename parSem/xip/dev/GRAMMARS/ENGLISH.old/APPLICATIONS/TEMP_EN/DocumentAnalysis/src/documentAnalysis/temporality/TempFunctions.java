/*
 * TempFunctions.java
 *
 * Created on November 29, 2006, 9:32 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package documentAnalysis.temporality;


import common.CommonLog;
import common.TypedPropertyException;
import common.corpora.UnknownCorpusException;
import java.sql.*;

import java.util.*;
import documentAnalysis.temporality.vaguecalendar.date.VagueDate;
import documentAnalysis.temporality.vaguecalendar.BadFormatException;
import com.xerox.jxip.*;
import common.Options;
import documentAnalysis.temporality.inference.LinguisticInference;

/**
 * Class for all <b>static</b> temporal functions.
 * @author xtannier
 */
public class TempFunctions {
    
    private static TemporalDictionary hmTempDict;
    private static long sentenceNumberTempDict = 0;
    private static int intAbsoluteDates = 0;

    /** Property for a default temporal relation
     *  Example of use : in AutoCalendar, meetings are generally set AFTER DCT
     */
    public static String DEFAULT_TEMPORAL_RELATION = "DEFAULT_TEMPORAL_RELATION";
    
    
    
    /**
     * Delete current temporal dictionary
     */
    public static void clearTempDict(){
        hmTempDict = null;
    }
    
    /**
     * Returns the XIP temporal dictionary for the given sentence in the given file.
     * @param fileName the name of the input file
     * @param sentenceNumber the number of the analysed sentence
     * @param options the parser options
     * @return the temporal dictionary for the given sentence in the given file
     */
    public static TemporalDictionary getTempDict(String fileName, long sentenceNumber, Options options){
        if (hmTempDict != null && sentenceNumberTempDict == sentenceNumber)
            return hmTempDict;
        
        if (hmTempDict == null)
            hmTempDict = new TemporalDictionary();
        else
            hmTempDict.clear();
        
        try{
            JXip2 jxip2 = new JXip2();
            Hashtable<String,String> tempDict = new Hashtable<String,String>();
            try {
                jxip2.xipGetDictionaryVariable(options.getGrammarHandler(), "temp_dict", tempDict);
            } catch (Exception ex) {
                ex.printStackTrace();
                System.exit(-1);
                return null;
            }
            
            String key;
            String value;
            String[] fields;
            String[] pair;
            boolean inSentence = false;
            for (Map.Entry<String, String> entry : tempDict.entrySet()) {
                HashMap<String, String> tempDictEntry = new HashMap<String, String>();
                key = entry.getKey();
                value = entry.getValue();
                fields = value.trim().split(";");
                for (int i = 0 ; i < fields.length ; ++i){
                    pair = fields[i].split(":");
                    if (pair.length != 2)
                        throw new BadFormatException("The temporal dictionary has a bad format (" + pair.toString() + ").");
                    // header
                    if (i == 0 && pair[0].equals("file")){
                        // same file
                        if (pair[1].equals(fileName)){
                            // look directly for sentence number
                            pair = fields[i+1].split(":");
                            if (pair.length != 2 || !pair[0].equals("sentence"))
                                throw new BadFormatException("The temporal dictionary has a bad format (" + pair.toString() + ").");
                            // same sentence number
//                            if (pair[1].equals(String.valueOf(sentenceNumber)))
                                inSentence = true;
                        }
                    } else {
                        // line => skip if not in the specified sentence
                        if (inSentence) {
                            tempDictEntry.put(pair[0], pair[1]);
                            if (pair[0].equals("key"))
                                key = pair[1];
                        } else {
                            break;
                        }
                    }
                }
                if (!tempDictEntry.isEmpty()){
                    hmTempDict.put(key, tempDictEntry);
                    inSentence = false;
                }
            }

            sentenceNumberTempDict = sentenceNumber;
            if (options.getBooleanProperty(TemporalOptions.TEMPORAL_PRINT_DICT))
                System.out.println(hmTempDict);

        } catch(BadFormatException se) {
            CommonLog.errorMessage(se.getMessage());
            return hmTempDict;
        } catch (TypedPropertyException ex) {
            ex.printStackTrace();
            System.exit(-1);
            return null;
        }
        return hmTempDict;
    }
    
    /**
     * Tests whether the specified node is in the temporal dictionary (with Xip node info).
     *
     * @return true whether the specified node has an entry in the temporal dictionary,
     *    false otherwise.
     * @param options
     * @param fileName the input file name
     * @param sentenceNumber the number of the current sentence
     * @param nodeNumber the number of the current node
     */
    public static boolean isTempNode(String fileName, long sentenceNumber, long nodeNumber, Options options){
        String strKey;
        if (hmTempDict == null || sentenceNumberTempDict != sentenceNumber)
            getTempDict(fileName, sentenceNumber, options);
        for (Map.Entry<String, HashMap<String, String>> entry : hmTempDict.entrySet()) {
            strKey = entry.getKey();
            if (strKey.matches("[a-z_A-Z]+" + String.valueOf(nodeNumber) + "[a-z\\+]?"))
                return true;
        }
        return false;
    }
    
    /**
     * Tests whether the specified node is in the temporal dictionary (with Python keys).
     * @param fileName the input file name
     * @param sentenceNumber the number of the current sentence
     * @param strKey the node key
     * @return true whether the specified node has an entry in the temporal dictionary,
     *    false otherwise.
     */
    public static boolean isTempNode(String fileName, long sentenceNumber, String strKey, Options options){
        if (hmTempDict == null || sentenceNumberTempDict != sentenceNumber)
            getTempDict(fileName, sentenceNumber, options);
        return hmTempDict.containsKey(strKey);
    }
    
    /**
     * Tests whether the specified node is in the temporal dictionary with ANCHOR type.
     *
     * @param fileName the input file name
     * @param sentenceNumber the number of the current sentence
     * @param key the node key
     * @return true whether the specified node has an ANCHOR entry in the temporal dictionary,
     *    false otherwise.
     */
    public static boolean isAnchor(String fileName, long sentenceNumber, String key, Options options){
        if (hmTempDict == null || sentenceNumberTempDict != sentenceNumber)
            getTempDict(fileName, sentenceNumber, options);
        return (hmTempDict.containsKey(key) && hmTempDict.get(key).get("type").toUpperCase().equals("ANCHOR"));
    }
    
    /**
     * Tests whether the specified node is in the temporal dictionary with DATE type.
     * @param fileName the input file name
     * @param sentenceNumber the number of the current sentence
     * @param strKey the node key
     * @return true whether the specified node has an DATE entry in the temporal dictionary,
     *    false otherwise.
     */
    public static boolean isDate(String fileName, long sentenceNumber, String strKey, Options options){
        if (hmTempDict == null || sentenceNumberTempDict != sentenceNumber)
            getTempDict(fileName, sentenceNumber, options);
        return (hmTempDict.containsKey(strKey) && hmTempDict.get(strKey).get("type").toUpperCase().equals("DATE"));
    }
    
    /**
     * Tests whether the specified node is in the temporal dictionary with DURATION type.
     * @param fileName the input file name
     * @param sentenceNumber the number of the current sentence
     * @param strKey the node key
     * @return true whether the specified node has an DURATION entry in the temporal dictionary,
     *    false otherwise.
     */
    public static boolean isDuration(String fileName, long sentenceNumber, String strKey, Options options){
        if (hmTempDict == null || sentenceNumberTempDict != sentenceNumber)
            getTempDict(fileName, sentenceNumber, options);
        return (hmTempDict.containsKey(strKey) && hmTempDict.get(strKey).get("type").toUpperCase().equals("DURATION"));
    }
    
    /**
     * Tests whether the specified node is in the temporal dictionary with SET type.
     * @param fileName the input file name
     * @param sentenceNumber the number of the current sentence
     * @param strKey the node key
     * @return true whether the specified node has an SET entry in the temporal dictionary,
     *    false otherwise.
     */
    public static boolean isSet(String fileName, long sentenceNumber, String strKey, Options options){
        if (hmTempDict == null || sentenceNumberTempDict != sentenceNumber)
            getTempDict(fileName, sentenceNumber, options);
        return (hmTempDict.containsKey(strKey) && hmTempDict.get(strKey).get("type").toUpperCase().equals("SET"));
    }
    
    
    /**
     * Gets temporal information for the specified node (with Xip node info).
     * @param fileName the input file name
     * @param sentenceNumber the number of the current sentence
     * @param type the type of the expected entry (ANCHOR, DATE, SET, DURATION)
     * @param nodeNumber the number of the current node
     * @return the temporal information about the specified node.
     */
    public static HashMap<String, String> getTempNodeInfo(String fileName, long sentenceNumber,
            String type, long nodeNumber, Options options){
        if (hmTempDict == null || sentenceNumberTempDict != sentenceNumber)
            getTempDict(fileName, sentenceNumber, options);
        String key;
        String value;
        for (Map.Entry<String, HashMap<String, String>> entry : hmTempDict.entrySet()) {
            key = entry.getKey();
            HashMap<String, String> hmEntry = entry.getValue();
            
            if (hmEntry.get("type").equals(type) && 
                    hmEntry.get("nodenumber").equals(String.valueOf(nodeNumber))) {
                hmEntry.put("key", key);
                return hmEntry;
            }
            
        }
        return null;
    }
    
    
    /**
     * Gets temporal information for the specified node (with Python key).
     *
     * @param fileName the input file name
     * @param sentenceNumber the number of the current sentence
     * @param key the node key
     * @return the temporal information about the specified node.
     */
    public static HashMap<String, String> getTempNodeInfo(String fileName, long sentenceNumber,
            String key, Options options){
        if (hmTempDict == null || sentenceNumberTempDict != sentenceNumber)
            getTempDict(fileName, sentenceNumber, options);
        return hmTempDict.get(key);
    }
    
    /**
     * Returns all keys correspong the the specified sentence in the temporal dictionary.
     * @param fileName the input file name
     * @param sentenceNumber the number of the current sentence
     * @return all keys correspong the the specified sentence in the temporal dictionary.
     */
    public static Set<String> getTempNodeKeys(String fileName, long sentenceNumber, Options options){
        if (hmTempDict == null || sentenceNumberTempDict != sentenceNumber)
            getTempDict(fileName, sentenceNumber, options);
        return hmTempDict.keySet();
    }
    
    
    /**
     * Get transitive closure of the temporal graph by composing pairs of relations
     * 
     * @param options the parser options
     * @param throwConsistencyException true whether the method should try
     *  to solve a consistency problem before throwing the exception, false otherwise
     * @see documentAnalysis.temporality.TemporalRelation#compose(TemporalRelation relation)
     * @see documentAnalysis.temporality.TemporalGraph#getTemporalClosure(boolean withInference, int compositionLevel, boolean throwConsistencyException)
     * @throws documentanalysis.temporality.TemporalConsistencyException if a consistency problem
     *  occurs in the graph with the new relations. Do not use any inference technique.
     */
    public static void getGraphTemporalClosure(Options options,
            boolean throwConsistencyException) throws TemporalConsistencyException{
        String inferenceLevel = options.getProperty(TemporalOptions.TEMPORAL_INFERENCE_LEVEL);
        if (inferenceLevel == null || !inferenceLevel.equals("0")){
            String strCompositionLevel = options.getProperty(TemporalOptions.TEMPORAL_COMPOSITION_LEVEL);
            int intCompositionLevel;
            if (strCompositionLevel == null)
                intCompositionLevel = 0;
            else
                try{
                    intCompositionLevel = Integer.parseInt(strCompositionLevel);
                    options.getTemporalGraph().getTemporalClosure(true, intCompositionLevel,
                            throwConsistencyException);
                } catch (NumberFormatException e) {
                    CommonLog.warningMessage("Property " + TemporalOptions.TEMPORAL_COMPOSITION_LEVEL +
                            " must be a number.");
                }
            
        }
        options.getCorpus().refineTemporalGraphAfterClosure(options.getTemporalGraph(), options);
    }
    
      
    
    /**
     * Returns all temporal expression nodes contained by the specified node.
     * @param root the root node
     * @return all XipNodes having the feature "tempexpr"
     */
    public static Vector<XipNode> getTemporalNodes(XipNode root){
        return root.getDescendantsByFeature("tempexpr", "+");
    }
    
    /**
     * Returns the number of absolute dates contained by the specified sentence.
     * @param fileName the input file name
     * @param sentenceNumber the sentence number
     * @return the number of absolute dates in this sentence
     */
    public static int countAbsoluteDates(String fileName, int sentenceNumber, Options options){
        intAbsoluteDates = 0;
        if (hmTempDict == null || sentenceNumberTempDict != sentenceNumber)
            getTempDict(fileName, sentenceNumber, options);
        
        String strKey;
        String strValue;
        
        for (Map.Entry<String, HashMap<String, String>> entry : hmTempDict.entrySet()) {
            strKey = entry.getKey();
            HashMap<String, String> hmEntry = entry.getValue();
            
            if (hmEntry.get("type").equals("DATE")) {
                strValue = hmEntry.get("value");
                try{
                    if ((VagueDate.getNewDate(strValue)).isAbsolute()) {
                        ++intAbsoluteDates;
                    }
                } catch(BadFormatException e){
                    continue;
                }
                
                
            }
        }
        
        return intAbsoluteDates;
    }
    
    /**
     * Tests whether the specified XipNode is a date.
     * @param node the XipNode
     * @return true whether the node has the feature "date", false otherwise
     */
    public static boolean isDate(XipNode node){
        return node.hasFeature("date", "+");
    }
    
    /**
     * Tests whether the specified XipNode is a duration.
     * @param node the XipNode
     * @return true whether the node has the feature "c_dur", false otherwise
     */
    public static boolean isDuration(XipNode node){
        return node.hasFeature("c_dur", "+");
    }
    
    /**
     * Tests whether the specified XipNode is a set.
     * @param node the XipNode
     * @return true whether the node has the feature "c_set", false otherwise
     */
    public static boolean isSet(XipNode node){
        return node.hasFeature("c_set", "+");
    }
    
    /**
     * Tests whether the specified XipNode is an anchor.
     * @param node the XipNode
     * @return true whether the node has the feature "c_anchor", false otherwise
     */
    public static boolean isAnchor(XipNode node){
        return node.hasFeature("c_anchor", "+");
    }
    
    /**
     * Tests whether the specified XipNode is an interval.
     * @param node the XipNode
     * @return true whether the node has the feature "interval", false otherwise
     */
    public static boolean isInterval(XipNode node){
        return node.hasFeature("interval", "+");
    }
    
    /**
     * Tests whether the specified XipNode is a temporal expression.
     * @param node the XipNode
     * @return true whether the node has the feature "tempexpr", false otherwise
     */
    public static boolean isTempNode(XipNode node){
        return node.hasFeature("tempexpr", "+");
    }
    
    /**
     * Returns an HTML representation of the specified temporal graph.
     * @param temporalGraph the temporal graph
     * @param verbose true to get more information
     * @return the HTML representation of the temporal graph.
     */
    public static String temporalGraphToHTML(TemporalGraph temporalGraph, boolean verbose){
        return temporalGraph.toHTML(verbose);
    }
    
    /**
     * Runs all basic inference algorithms for temporal relations.
     * @param unit the current XipUnit
     * @param options the parser options
     * @throws documentanalysis.temporality.TemporalConsistencyException raised when the DCT
     *       failed to be found according to the specified corpus
     * @throws common.UnknownCorpusException raised when the specified corpus is unknown
     */
    public static void basicInference(XipUnit unit,
            Options options) throws TemporalConsistencyException, UnknownCorpusException{

        LinguisticInference.storeEvents(unit, options);
        String temporalInference = options.getProperty(TemporalOptions.TEMPORAL_INFERENCE_LEVEL);
        if (temporalInference != null && !temporalInference.equals("0")){
            LinguisticInference.storeDates(unit, options);
        }

        options.getCorpus().corpusSpecificInference(unit, options);
    }
    
    /**
     * Links all absolute dates contained in the specified sentence to their corresponding events.
     * @param fileName the input file name
     * @param xipRes the XipResult
     * @param sentenceNumber the number of the sentence
     * @deprecated now these links are created elsewhere.
     */
    @Deprecated public static void linkAbsoluteDatesWithEvents(String fileName, XipResult xipRes,
    long sentenceNumber, Options options){
        if (hmTempDict == null || sentenceNumberTempDict != sentenceNumber)
            getTempDict(fileName, sentenceNumber, options);
        
        Vector<XipUnit> xipUnits = xipRes.getXipUnits();
        
        XipUnit xipUnitSentence = xipUnits.get(0);
        
        // TEMP dependencies
//        Vector<XipDependency> TempInterpDependencies = xipUnitSentence.getDependenciesByName("TEMP_INTERP_ANCHOR");
//
//        for (XipDependency tempInterpDep : TempInterpDependencies){
//            XipNode tempNode = tempInterpDep.getArg(2);
//            System.err.println(tempNode.getIndex() + " " + tempNode.getId());
//            if (isTempNode(sentenceNumber, tempNode.getIndex()))
//                System.err.println(tempNode);
//        }
//
//
        
        Connection conn;
        Statement stmt;
        ResultSet rs;
        String pilote = "com.mysql.jdbc.Driver";
        String url = new String("jdbc:mysql://etretat/test_xavier");
        
        try {
            Class.forName(pilote);
            
            conn = DriverManager.getConnection(url,"root","root");
            
            stmt = conn.createStatement();
            
            rs = stmt.executeQuery("select * from tempexpr where NoSentence = " + String.valueOf(sentenceNumber) + ";");
            
            rs.close();
            stmt.close();
            conn.close();
            
        }
        
        catch ( SQLException e) {
            e.printStackTrace();
            System.out.println("SQLException: " + e.getMessage());
            System.out.println("SQLState: " + e.getSQLState());
            System.out.println("VendorError: " + e.getErrorCode());
            System.exit(-1);
        } catch ( ClassNotFoundException e) {
            e.printStackTrace();
            System.exit(-1);
        }
        
    }
    
}
