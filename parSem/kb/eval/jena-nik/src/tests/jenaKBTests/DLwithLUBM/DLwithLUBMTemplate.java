/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package tests.jenaKBTests.DLwithLUBM;


import java.io.File;
import java.util.ArrayList;
import java.util.List;
import org.apache.log4j.Logger;
import basics.Basics;
import tests.TestUtilities;
/**
 *
 * @author nlagos
 */
public class DLwithLUBMTemplate {
    /*public static String ontologyURL = "http://www.owl-ontologies.com/OntologyPersonXeroxDraft1.owl";
    public static String schemaFileName = "personXeroxFOL_test.owl";*/
    public static String ontologyURL = "http://www.xrce.xerox.com/parsem/RiskOntology.owl";
    public static String schemaFileName = "RiskOntology.owl";
    /*public static String ontologyURL = "http://www.lehigh.edu/~zhp2/2004/0401/univ-bench.owl";
    public static String schemaFileName = "univ-bench.owl";*/
    //public final static String dataFileName = "Corpus_Iran_norm-ac.owl";
    
    private String count;
    protected static String schemaUrl;
    protected static List<String> dataLocations;
    protected static Logger log = Logger.getLogger(DLwithLUBMTemplate.class);
    private long stopwatch = 0;

    public DLwithLUBMTemplate() {
        
        
        try {
            // start logging
            Basics.useLog("jenaKB-Test-DL-with-ULBM");
            // find the current path, corresponding to different OS system
            File path = new File(System.getProperty("user.dir"), "inputs");
            // get the valid URL for TBox, including the check of existence of
            // the file
            schemaUrl = TestUtilities.getValidSchemaURL(path, schemaFileName);
            if (schemaUrl.equals("")) {
                log.error("Can't find the schema file. Exit.");
                return;
            }

            /*public void setMatchingFlights(List<? extends Flight> matchingFlights) {
                this.matchingFlights.addAll(matchingFlights);
            }*/

            List <String> dataFileName = new ArrayList <String> (returnFiles(path.toString()));
            
            
            // get the data locations
            dataLocations = new ArrayList<String>();
            /*for (int i=0; i<dataFileName.size(); i++){
                
                if (dataFileName.get(i).toString().startsWith("uba\\University0")){
                    dataLocations.add(new File(path, dataFileName.get(i).toString()).getAbsolutePath());
                }
            }*/
            //dataLocations.add(new File(path, "Test.owl").getAbsolutePath());
            //dataLocations.add(new File(path, "example_mappings.owl").getAbsolutePath());
            //System.out.println(dataLocations.toString());
        } catch (Exception e) {
            log.error("Problem catched by application: " + e);
        }
    }
    
    protected List<String> returnFiles (String directoryName){
        File dir = new File(directoryName);
        List <String> filename = new ArrayList <String> ();
    
        String[] children = dir.list();
        if (children == null) {
        // Either dir does not exist or is not a directory
        } else {
            for (int i=0; i<children.length; i++) {
            // Get filename of file or directory
                filename.add(children[i]);
                
            }
        }
        
        return filename;
    }

    /**
     * Here you need to override to implement how the test should be
     */
    protected void run() throws Exception {
    }
}
