package tests.jenaKBTests.DLOntologyTests;

import knowledgeBases.JenaKBManager;
import knowledgeBases.KBManagerFactory;

/**
 * Test the KB with DB connection and meanwhile use all the query to test the reasoning.
 * @author tlu
 *
 */
public class DLDBQueryTest extends DLTestTemplete {

    @Override
    protected void run() throws Exception {
        JenaKBManager kbm = (JenaKBManager) KBManagerFactory.createKBManager(JenaKBManager.JENA_PELLET_DB);
        if (kbm.buildKB(ontologyURL, schemaUrl, dataLocations, "pellet")) {
            simpleBasicTestKB(kbm);
            simpleQueryTestSPARQL(kbm);
            simpleQueryTestDataTypePropertyMatchingSPARQL(kbm);
            complexQueryTestSPARQL(kbm);
            kbm.clean();
        }
    }

    public static void main(String[] args) throws Exception {
        DLDBQueryTest test = new DLDBQueryTest();
        test.run();
    }
}
