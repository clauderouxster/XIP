package tests.jenaKBTests.A1A2OntologyTests;

import tests.TestUtilities;
import knowledgeBases.JenaKBManager;
import knowledgeBases.KBManagerFactory;

/**
 * The most basic test for A1A2 version of the ontology and parSem setting. This is
 * the first test, which need to be successful, in order to do more on our ParSemKB
 * @author tlu
 *
 */
public class A1A2BasicTest extends A1A2TestTemplate {

    protected void run() {  
        JenaKBManager kbm = (JenaKBManager) KBManagerFactory.createKBManager(JenaKBManager.JENA_PELLET_PELLET_MEM);
        if (kbm.buildKB(ontologyURL, schemaUrl, dataLocations, "pellet")) {
            TestUtilities.showClassTree(schemaUrl);
            kbm.renderKnowledge();
            kbm.clean();
        }
    }

    public static void main(String[] args) {
        System.out.println("A1A2BasicTest");
        A1A2BasicTest test = new A1A2BasicTest();
        test.run();
    }
}
