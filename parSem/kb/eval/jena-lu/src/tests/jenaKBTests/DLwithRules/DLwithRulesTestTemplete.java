package tests.jenaKBTests.DLwithRules;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import org.apache.log4j.Logger;
import basics.Basics;
import tests.TestUtilities;

public class DLwithRulesTestTemplete {

    public static String ontologyURL = "http://www.owl-ontologies.com/OntologyPersonXeroxDraft1.owl";
    public static String schemaFileName = "personParSemFOLwithRules.owl";
    // public final static String dataFileName = "out.owl";
    public static String dataFileName1 = "personXeroxFOLArnoldExample.rdf";
    // public final static String alltogether =
    // "personXeroxFOLInstancesDraft4.owl";
    protected String schemaUrl;
    protected List<String> dataLocations;
    protected static Logger log = Logger.getLogger(DLwithRulesTestTemplete.class);

    public DLwithRulesTestTemplete() {
        try {
            // start logging
            Basics.useLog("jenaKBTestDL-withRules");
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
            dataLocations.add(new File(path, dataFileName1).getAbsolutePath());
        } catch (Exception e) {
            log.error("Problem catched by application: " + e);
        }
    }

    /**
     * Here you need to override to implement how the test should be
     */
    protected void run() {
    }
}
