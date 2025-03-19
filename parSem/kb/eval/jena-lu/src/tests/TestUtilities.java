package tests;

import java.io.File;
import java.net.URI;
import java.net.URL;
import java.util.Iterator;

import javax.swing.JFrame;
import javax.swing.JScrollPane;
import javax.swing.WindowConstants;

import knowledgeBases.JenaKBManager;


import com.hp.hpl.jena.rdf.model.InfModel;
import com.hp.hpl.jena.rdf.model.Resource;
import com.hp.hpl.jena.reasoner.ValidityReport;
import com.hp.hpl.jena.vocabulary.RDF;
import com.hp.hpl.jena.query.ResultSet;
import org.apache.log4j.Logger;

/**
 * This class provide the basic reusable methods and ease the way of writing test codes.
 *
 */
public class TestUtilities {

    public final static String schemaFileName = "personXeroxFOL.owl";
    public final static String dataFileName = "out.owl";
    //public static Category log = Category.getInstance(TestUtilities.class.getName());
    static public Logger log = Logger.getLogger(TestUtilities.class);

    /**
     * Combine the given parameters and check the result as URL, return the string form 
     * of the URL.
     * 
     * In order to use an URL, we need to first check if it's valid. 
     * This method Help to find the valide URL and return the String form of it back. 
     * 
     * @param path
     * @param filename
     * @return the String form of the URL 
     */
    public static String getValidSchemaURL(File path, String filename) {
        try {
            File schemaFile = new File(path, filename);
            if (schemaFile.exists()) {
                URI schemaURI = schemaFile.toURI();
                URL schemalURL = schemaURI.toURL();
                return schemalURL.toString();
            } else {
                return "";
            }
        } catch (Exception e) {
            log.error("invalid file name or system can't generate URL from it: " + filename);
            log.error("Original Exception Message: \n" + e);
            return "";
        }
    }

    /**
     * Input (Document data) independent test: Validate the ontology schema. 
     * @param kbm
     * @return true, if the ontology is valid;  otherwise false
     */
    public static boolean validationTest(JenaKBManager kbm) {
        InfModel kb = kbm.getKb();
        // test case 3:we can check for inconsistencies within the data
        // by using the validation interface:
        log.info("test case 3: check for inconsistencies within the data by using the validation interface.");
        ValidityReport validity = kb.validate();
        if (validity.isValid()) {
            log.info("OK");
            return true;
        } else {
            log.info("Conflicts");
            for (Iterator i = validity.getReports(); i.hasNext();) {
                ValidityReport.Report report = (ValidityReport.Report) i.next();
                log.info(" - " + report);
            }
            return false;
        }
    }

    /**
     * Testing if an individual is an instance of a class expression.
     * @param kbm
     * @param individualId
     * @param classId
     * @return true if we proved that the given object belongs to the given class
     */
    public static boolean checkClass(JenaKBManager kbm, String individualId, String classId) {
        InfModel kb = kbm.getKb();
        log.info("test case 2: Testing if an individual is an instance of a class expression.");
        Resource personName1 = kb.getResource(kbm.getXmlBase() + individualId);
        Resource personName = kb.getResource(kbm.getXmlBase() + classId);
        if (kb.contains(personName1, RDF.type, personName)) {
            log.info(individualId + " recognized as " + classId);
            return true;
        } else {
            log.info("Failed to recognize " + individualId + " correctly");
            return false;
        }
    }

    /**
     * Find out all we know about a specific instance This can be done using:
     * 
     * The query itself seems to be stupid. But with this query, we want to show that 
     * one individual could be classified to different classes.
     * 
     * @param kbm
     * @param individualAbout 
     */
    public static void tellMeAllAbout(JenaKBManager kbm, String individualAbout) {
        InfModel kb = kbm.getKb();
        log.info("test case 1: find out all we know about the " + individualAbout);
        Resource unknownTypeEntity = kb.getResource(individualAbout);
        log.info("unknownTypeEntity *:" + unknownTypeEntity);
        kbm.printStatements(kb, unknownTypeEntity, null, null);
    }

    /**
     * Find out all occurrences of an expression with a specified type  
     * 
     * 
     * @param kbm
     * @param entityName
     * @param entityType
     * @return the ResultSet rs 
     */
    public static ResultSet getOccurrences(JenaKBManager kbm, String entityName, String entityType)
    {
        String restriction = "?x1             rdf:type   ns:"+entityType+"  .\r\n"
                           + "?x1             ns:hasName ?primitiveData1    .\r\n"
                           + "?primitiveData1 ns:form    \""+entityName+"\" .\r\n"
                           + "?primitiveData1 ns:form    ?x2                .\r\n";
        
        log.info("\n\nAsked Entity :" + entityName+" ("+entityType+")");
        ResultSet rs = kbm.queryXItem(restriction,2);
        String title = "Occurrences of the Named Entity "+entityName+"("+entityType+")";
        JenaKBManager.displayResultSet(rs,title);
        return rs;
    }
 
    /**
     * Find out all occurrences of Named Entities with a specified type  
     * 
     * 
     * @param kbm
     * @param entityType
     * @return the ResultSet rs 
     */
    public static ResultSet getEntityTypeOccurrences(JenaKBManager kbm, String entityType)
    {
        String restriction = "?x1             rdf:type    ns:"+entityType+"  .\r\n"
                           + "?x1             ns:hasName  ?primitiveData1    .\r\n"
                           + "?primitiveData1 ns:form     ?x2 ";
         
        log.info("\n\nAsked typeEntity :" + entityType);
        ResultSet rs = kbm.queryXItem(restriction,2);
        String title = "Occurrences of Entities with the type \""+entityType+"\"";
        JenaKBManager.displayResultSet(rs,title);
        return rs;
    }
 
    /**
     * Find out all occurrences of an expression as participant of an event  
     * 
     * 
     * @param kbm
     * @param entityName
     * @param entityType
     * @return the ResultSet rs 
     */
    public static ResultSet getEvent(JenaKBManager kbm, String entityName, String entityType)
    {
        String restriction = "?x1             rdf:type          ns:"+entityType+"  .\r\n"
                           + "?x2             ns:hasParticipant ?x1                .\r\n"
                           + "?x1             ns:hasName        ?primitiveData1    .\r\n"
                           + "?primitiveData1 ns:form           \""+entityName+"\" .\r\n";

        log.info("\n\nAsked participant :" + entityName+" ("+entityType+")");
        ResultSet rs = kbm.queryXItem(restriction,2);
        String title = "Occurrences (x1) of the Named Entity " + entityName+" ("+entityType+") as participant of an Event (x2)";
        JenaKBManager.displayResultSet(rs,title);
        return rs;
    }

    /**
     * Find out all occurrences of an expression as participant of an event and keep the value of the core
     * 
     * 
     * @param kbm
     * @param entityName
     * @param entityType
     * @return the ResultSet rs 
     */
    public static ResultSet getEventWithCore(JenaKBManager kbm, String entityName, String entityType)
    {
        String restriction = "?x1             rdf:type          ns:"+entityType+"  .\r\n"
                           + "?x2             ns:hasCore        ?x3                .\r\n"
                           + "?x2             ns:hasParticipant ?x1                .\r\n"
                           + "?x1             ns:hasName        ?primitiveData1    .\r\n"
                           + "?primitiveData1 ns:form           \""+entityName+"\" .\r\n";

        log.info("\n\nAsked participant :" + entityName+" ("+entityType+")");
        ResultSet rs = kbm.queryXItem(restriction,3);
        String title = "Occurrences (x1) of the Named Entity " + entityName+" ("+entityType+") as participant of an Event (x2) with the core (x3)";
        JenaKBManager.displayResultSet(rs,title);

        return rs;
    }

    /**
     * Find out all occurrences as participant of an event with entityName as participant
     * 
     * 
     * @param kbm
     * @param entityName
     * @param entityType
     * @return the ResultSet rs 
     */
    public static ResultSet getParticipantOfEvent(JenaKBManager kbm, String entityName, String entityType)
    {
        String restriction = "?entity1        rdf:type          ns:"+entityType+"  .\r\n"
                           + "?event1         ns:hasParticipant ?entity1           .\r\n"
                           + "?event1         ns:hasParticipant ?x1                .\r\n"
                           + "?entity1        ns:hasName        ?primitiveData1    .\r\n"
                           + "?primitiveData1 ns:form           \""+entityName+"\" .\r\n"
                           + "?x1             ns:hasName        ?primitivedata2    .\r\n"
                           + "?primitivedata2 ns:form           ?x2                .\r\n";

        log.info("\n\nAsked participant :" + entityName+" ("+entityType+")");
        ResultSet rs = kbm.queryXItem(restriction,2);
        String title = "Occurrences (x1) of a Named Entity (x2) as participant of an Event with " + entityName+" ("+entityType+")  as participant";
        JenaKBManager.displayResultSet(rs,title);
        return rs;
    }

    /**
     * Find out all surface form of occurrences as participant of an event with entityName as participant
     * 
     * 
     * @param kbm
     * @param entityName
     * @param entityType
     * @return the ResultSet rs 
     */
    public static ResultSet getParticipantOfEventWithSurface(JenaKBManager kbm, String entityName, String entityType)
    {
        String restriction = "?entity1        rdf:type          ns:"+entityType+"  .\r\n"
                           + "?event1         ns:hasParticipant ?entity1           .\r\n"
                           + "?event1         ns:hasParticipant ?x1                .\r\n"
                           + "?x1             ns:hasName        ?primitiveData2    .\r\n"
                           + "?primitiveData2 ns:form           ?x2                .\r\n"
                           + "?entity1        ns:hasName        ?primitiveData1    .\r\n"
                           + "?primitiveData1 ns:form           \""+entityName+"\" .\r\n";

        log.info("\n\nAsked participant :" + entityName+" ("+entityType+")");
        ResultSet rs = kbm.queryXItem(restriction,2);
        String title = "Occurrences (x1) of a Named Entity as participant of an Event (x2) with " + entityName+" ("+entityType+")  as participant";
        JenaKBManager.displayResultSet(rs,title);
        return rs;
    }

    public static void showClassTree(String ontologyURL) {
        try {
            ClassTree tree = new ClassTree(ontologyURL);
            JFrame frame = new JFrame("Ontology Hierarchy");
            frame.getContentPane().add(new JScrollPane(tree.getJTree()));
            frame.setSize(800, 600);
            frame.setVisible(true);
            frame.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        } catch (Exception e) {
            log.error("Something wrong by ClassTree: " + e);
        }
    }
}
