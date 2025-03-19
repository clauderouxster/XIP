package tests.jenaKBTests.DLOntologyTests;

import tests.TestUtilities;
import knowledgeBases.JenaKBManager;
import knowledgeBases.KBManagerFactory;

/**
 * Provides the most basic test for our DL-setting. This is
 * the first test, which need to be successful, in order to do more on our ParSemKB
 * 
 * @author tlu
 *
 */
public class DLBasicTest extends DLTestTemplete {

    @Override
    protected void run() {
        JenaKBManager kbm = (JenaKBManager) KBManagerFactory.createKBManager(JenaKBManager.JENA_PELLET_PELLET_MEM);
        if (kbm.buildKB(ontologyURL, schemaUrl, dataLocations, "pellet")) {
            TestUtilities.showClassTree(schemaUrl);
            kbm.renderKnowledge();
            kbm.clean();
        }
    }

    public static void main(String[] args) {
        DLBasicTest test = new DLBasicTest();
        test.run();
    }
}
