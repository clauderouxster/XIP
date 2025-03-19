package tests.jenaKBTests.A1A2OntologyTests;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import knowledgeBases.JenaKBManager;
import org.apache.log4j.Logger;
import tests.TestUtilities;
import basics.Basics;

/**
 * This class provide a Template for testing the A1A2 ontology. The A1 and A2 ontology treats relations as
 * entitys (Objects) and hence provide the ability to encode the reification and uncertainty rating on the 
 * very top level. 
 * 
 * You can write your specific test class for particular query or other properties easily. All the other tests related A1A2 setting should extends this class. 
 * 
 * For more information on A1A2 setting, please refer to the scientific report of ParSemKB in deliverable folder 
 * under documentations.
 * 
 * @author tlu
 *
 */
public class A1A2TestTemplate {

    public static String ontologyURL = "http://www.owl-ontologies.com/personXeroxDraftHOL2.owl";
    public static String schemaFileName = "personParSemA1A2.owl";
    protected static Logger log = Logger.getLogger(A1A2TestTemplate.class);
    public static String dataFileName1 = "documentExampleBush.rdf";
    public static String dataFileName2 = "schwarzneggerExample.rdf";
    public static String dataFileName3 = "correferenceExample.rdf";
    protected String schemaUrl;
    protected List<String> dataLocations;

    public A1A2TestTemplate() {
        try {
            // start logging
            Basics.useLog("jenaKBTestA1A2");
            // find the current path, corresponding to different OS system
            File path = new File(System.getProperty("user.dir"), "inputs");
            // get the valid URL for TBox, including the check of existence of
            // the file
            schemaUrl = TestUtilities.getValidSchemaURL(path, schemaFileName);
            if (schemaUrl.equals("")) {
                log.error("Can't find the schema file. Exit.");
                return;
            }
            // get the data locations
            dataLocations = new ArrayList<String>();
            // dataLocations.add(path + ruleFileName); // add rules separately
            // causes some internal problems of pellet.
            dataLocations.add(new File(path, dataFileName1).getAbsolutePath());
            dataLocations.add(new File(path, dataFileName2).getAbsolutePath());
            dataLocations.add(new File(path, dataFileName3).getAbsolutePath());
        } catch (Exception e) {
            log.error("Problem catched by application: " + e);
        }
    }

    /**
     * Here you need to implement how the test should be
     */
    protected void run() {
    }

    protected static boolean basicQueryTestA1A2SPARQL(JenaKBManager kbm) {
        String restriction1 = "   _:hasP rdf:type ns:Has_participant .\r\n" + "   ?entity rdf:type ns:Entity .\r\n" + "   _:hasP ns:a2 ?x .\r\n";
        String restriction2 = "   ?hasP rdf:type ns:Has_participant .\r\n" + "   ?entity rdf:type ns:Entity .\r\n" + "   ?hasP ns:a2 ?x .\r\n";
        kbm.querySingleItem(restriction1);
        kbm.querySingleItem(restriction2);
        return true;
    }

    /**
     * Test "what does Arnold do?" in schwarzenegger example.
     * 
     * @param kbm
     * @return
     */
    protected static boolean complexQueryTest1A1A2SPARQL(JenaKBManager kbm) {
        String restriction = "  _:person1   rdf:type    ns:Person .\r\n" + " _:hasName1      rdf:type      ns:Has_person_name .\r\n" + "_:hasName1       ns:a1         _:person1 .\r\n" + "_:hasName1       ns:a2         _:pn1 .\r\n" + " _:pn1      rdf:type      ns:PersonName .\r\n" + " _:ham      rdf:type      ns:Has_person_atom_name .\r\n" + "_:ham       ns:a1         _:pn1 .\r\n" + "_:ham       ns:a2         _:Namestring .\r\n" + "_:Namestring ns:form     \"Arnold\" .\r\n" + "_:hasFun     rdf:type    ns:Has_function .\r\n" + "_:hasFun     ns:a1       _:person1 .\r\n" + "_:hasFun     ns:a2       ?x.\r\n";
        // + "_:hasFun ns:a2 _:Fun.\r\n"
        // + "_:Fun rdf:type ns:Function .\r\n"
        // + "_:Fun ns:form ?x";
        kbm.querySingleItem(restriction);
        return true;
    }

    /**
     * :TODO Test "Is Zini arrested in 2003?"
     * 
     * @param kbm
     * @return
     */
    protected static boolean complexQueryTest2SPARQL(JenaKBManager kbm) {
        String restriction = "  _:hasP1 rdf:type ns:Has_participant .\r\n" + "_:hasP1  ns:a1 ?y .\r\n" + "_:hasP1  ns:a2 ?x .\r\n" + "_:hasP2  rdf:type ns:Has_participant .\r\n" + "_:hasP2   ns:a1 ?y .\r\n" + "_:hasP2  ns:a2 ?z .\r\n" + "?z       rdf:type ns:Person .\r\n" + "?z       ns:form \"Giampaolo Zini\"";
        kbm.querySingleItem(restriction);
        return true;
    }
}
