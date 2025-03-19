package tests.jenaKBTests.A1A2OntologyTests;

import tests.TestUtilities;
import knowledgeBases.JenaKBManager;
import knowledgeBases.KBManagerFactory;

/**
 * The total test on A1A2 Version of ontology. Besides, we use DB connection here. 
 * 
 *  This is the most complex case we use ParSemKB with A1A2 ontology. If this test
 *  is successfully accomplished, then we can use the ParSemKB for all every setting we want.
 * @author tlu
 *
 */
public class A1A2DBCompleteTest extends A1A2TestTemplate {

    protected void run() throws Exception {
        JenaKBManager kbm = (JenaKBManager) KBManagerFactory.createKBManager(JenaKBManager.JENA_PELLET_DB);
        if (kbm.buildKB(ontologyURL, schemaUrl, dataLocations, "pellet")) {
            kbm.renderKnowledge();
            basicQueryTestA1A2SPARQL(kbm);
            complexQueryTest1A1A2SPARQL(kbm);
            TestUtilities.showClassTree(schemaUrl);
            kbm.clean();
        }
    }

    public static void main(String[] args) throws Exception {
        System.out.println("A1A2DBCompleteTest");
        A1A2DBCompleteTest test = new A1A2DBCompleteTest();
        test.run();
    }
}
