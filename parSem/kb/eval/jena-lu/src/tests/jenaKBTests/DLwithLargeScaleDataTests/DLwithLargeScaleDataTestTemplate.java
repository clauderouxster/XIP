package tests.jenaKBTests.DLwithLargeScaleDataTests;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import org.apache.log4j.Logger;
import basics.Basics;
import tests.TestUtilities;

/**
 * This test sets are designed for the large scale data with the example out.owl.
 * @author tlu
 *
 */
public class DLwithLargeScaleDataTestTemplate {

    public static String ontologyURL = "http://www.owl-ontologies.com/OntologyPersonXeroxDraft1.owl";
    public static String schemaFileName = "personXeroxFOL_test.owl";
    //public final static String dataFileName = "Corpus_Iran_norm-ac.owl";
    public final static String dataFileName = "out_firstPart.owl";
    protected String schemaUrl;
    protected List<String> dataLocations;
    protected static Logger log = Logger.getLogger(DLwithLargeScaleDataTestTemplate.class);
    private long stopwatch = 0;

    public DLwithLargeScaleDataTestTemplate() {
        try {
            // start logging
            Basics.useLog("jenaKB-Test-DL-with-large-Scale-Data");
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
            dataLocations.add(new File(path, dataFileName).getAbsolutePath());
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
