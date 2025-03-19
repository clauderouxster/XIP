package tests.jenaKBTests.DLOntologyTests;

import java.io.File;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import knowledgeBases.JenaKBManager;
import org.apache.log4j.Logger;
import com.hp.hpl.jena.rdf.model.InfModel;
import com.hp.hpl.jena.rdf.model.ReifiedStatement;
import com.hp.hpl.jena.rdf.model.Resource;
import com.hp.hpl.jena.rdf.model.Statement;
import com.hp.hpl.jena.reasoner.ValidityReport;
import com.hp.hpl.jena.util.PrintUtil;
import com.hp.hpl.jena.vocabulary.RDF;
import tests.TestUtilities;
import basics.Basics;

/**
 * DL stands for Description Logic. This class is the template class for testing our DL ontology.
 * 
 * You can write your specific test class for particular query or other properties easily. All the other DL ontology related tests should extend this class.
 * 
 * For more information on DL or DL-ontology, please refer to the scientific report of ParSemKB in deliverable folder 
 * under documentations folder.
 * 
 * @author tlu
 *
 */
public class DLTestTemplete {

    public static String ontologyURL = "http://www.owl-ontologies.com/OntologyPersonXeroxDraft1.owl";
    public static String schemaFileName = "personXeroxFOL.owl";
    // public final static String dataFileName = "out.owl";
    public static String dataFileName1 = "personXeroxFOLArnoldExample.rdf";
    // public final static String alltogether =
    // "personXeroxFOLInstancesDraft4.owl";
    protected String schemaUrl;
    protected List<String> dataLocations;
    protected static Logger log = Logger.getLogger(DLTestTemplete.class);

    public DLTestTemplete() {
        try {
            // start logging
            Basics.useLog("jenaKBTestDL");
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
        // dataLocations.add(new File(path,
        // dataFileName).getAbsolutePath());
        } catch (Exception e) {
            log.error("Problem catched by application: " + e);
        }
    }

    /**
     * Here you need to override to implement how the test should be
     */
    protected void run() throws Exception {
    }

    public static void testReificationStatements(JenaKBManager kbm) {
        // add the reification statements to the kb
        ReifiedStatement rfs1 = kbm.addReifiedStatement("Person_3", "hasFunction", "gouverneur");
        if (rfs1 == null) {
            log.error("Add reification failed.");
            return;
        }
        ReifiedStatement rfs2 = kbm.addReifiedStatement(rfs1.getURI(), "hasTime", "ti_2003-2005");
        if (rfs2 == null) {
            log.error("Add reification failed.");
            return;
        }
        Statement s = kbm.addStatement(rfs2.getURI(), "hasSource", "nyTimes");
        if (s == null) {
            log.error("Add statement failed.");
            return;
        }
        // query the kb to see if it stores the reification
        // print the result out
        log.info("reified statement added: " + rfs1.getStatement());
        log.info("reified statement added: " + rfs2.getStatement());
        log.info("statement added: " + s);
        // The following lines doesn't work, don't know why.
        PrintUtil.print(rfs1.getStatement());
        PrintUtil.print(rfs2.getStatement());
        PrintUtil.print(s);
    }

    /**
     * Test what AtomName does the person "person1" has? We want to see if it
     * can handle the Sub-Property reasoning and rules
     * 
     * @param kbm
     * @return
     */
    protected static boolean simpleQueryTestSPARQL(JenaKBManager kbm) {
        String restriction = "  _:person1   rdf:type    ns:Person .\r\n" + "_:person1        ns:hasName      _:personName1 .\r\n" + "_:personName1    ns:hasPersonAtomName  ?x .\r\n";
        // + "_:personName1 ns:hasPersonAtomName _:namestring .\r\n"
        // + "_:namestring ns:form \"Arnold\" .\r\n"
        // + "_:person1 ns:hasFunction ?x .\r\n";
        kbm.querySingleItem(restriction);
        return true;
    }

    /**
     * Test is there a person with name of form "Arnold". We want to see if it
     * can handle matching on datatype property like String
     * 
     * @param kbm
     * @return
     */
    protected static boolean simpleQueryTestDataTypePropertyMatchingSPARQL(JenaKBManager kbm) {
        String restriction = "  ?x   rdf:type    ns:PersonName .\r\n" + "?x    ns:hasPersonAtomName  _:namestring .\r\n" + "_:namestring ns:form \"Arnold\" .\r\n";
        // + "_:person1 ns:hasFunction ?x .\r\n";
        kbm.querySingleItem(restriction);
        return true;
    }

    /**
     * Test "what does Arnold do?" in schwarzenegger example.Non-A1A2 ontology
     * 
     * @param kbm
     * @return
     */
    protected static boolean complexQueryTestSPARQL(JenaKBManager kbm) {
        String restriction = "  _:person1   rdf:type    ns:Person .\r\n" + "_:person1        ns:hasName      _:personName1 .\r\n" + "_:personName1 ns:hasPersonAtomName _:namestring .\r\n" + "_:namestring ns:form \"Arnold\" .\r\n" + "_:person1 ns:hasFunction ?x .\r\n";
        kbm.querySingleItem(restriction);
        return true;
    }

    public static boolean simpleBasicTestKB(JenaKBManager kbm) {
        InfModel kb = kbm.getKb();
        // following 2 lines are for facilities:
        // Model schema =
        // FileManager.get().loadModel("file:data/owlDemoSchema.owl");
        // Model data =
        // FileManager.get().loadModel("file:data/owlDemoData.rdf");

        // test case 1: A typical example operation on such a model would be to
        // find out all we know about
        // a specific instance This can be done using:
        log.info("test case 1: find out all we know about the Person_3.");
        Resource person = kb.getResource(kbm.getXmlBase() + "#Person_3");
        log.info("Person *:");
        kbm.printStatements(kb, person, null, null);
        // test case 2:A second, typical operation is instance recognition.
        // Testing if an individual is an instance of a class expression.
        log.info("test case 2: Testing if an individual is an instance of a class expression.");
        Resource personName1 = kb.getResource(kbm.getXmlBase() + "#ArnoldSchwazneger");
        Resource personName = kb.getResource(kbm.getXmlBase() + "#PersonName");
        if (kb.contains(personName1, RDF.type, personName)) {
            log.info("personName1 recognized as personName");
        } else {
            log.info("Failed to recognize personName1 correctly");
        }
        // test case 3:we can check for inconsistencies within the data
        // by using the validation interface:
        log.info("test case 3: check for inconsistencies within the data by using the validation interface.");
        ValidityReport validity = kb.validate();
        if (validity.isValid()) {
            log.info("OK");
        } else {
            log.info("Conflicts");
            for (Iterator i = validity.getReports(); i.hasNext();) {
                ValidityReport.Report report = (ValidityReport.Report) i.next();
                log.info(" - " + report);
            }
        }
        // test case 4: we check the subsumptions on the relations
        // log.info("Test if the subsumption on the relation can be considered
        // by reasoning.");
        return true;
    }
}
