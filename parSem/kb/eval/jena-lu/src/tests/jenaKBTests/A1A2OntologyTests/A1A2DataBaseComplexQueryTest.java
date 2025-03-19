package tests.jenaKBTests.A1A2OntologyTests;

import knowledgeBases.JenaKBManager;
import knowledgeBases.KBManagerFactory;

/**
 * Set up the KB with database combination and test the KB with complex query.
 * 
 * @author tlu
 *
 */
public class A1A2DataBaseComplexQueryTest extends A1A2TestTemplate {

    protected void run() {
        JenaKBManager kbm = (JenaKBManager) KBManagerFactory.createKBManager(JenaKBManager.JENA_PELLET_PELLET_DB);
        if (kbm.buildKB(ontologyURL, schemaUrl, dataLocations, "pellet")) {
            complexQueryTest1A1A2SPARQL(kbm);
            // kbm.showClassTree(schemaUrl);
            // kbm.renderKnowledge();
            kbm.clean();
        }
    }

    public static void main(String[] args) {
        System.out.println("A1A2DataBaseComplexQueryTest");
        A1A2DataBaseComplexQueryTest test = new A1A2DataBaseComplexQueryTest();
        test.run();
    }
}
