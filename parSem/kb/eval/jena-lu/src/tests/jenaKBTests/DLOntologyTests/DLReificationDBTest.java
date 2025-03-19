package tests.jenaKBTests.DLOntologyTests;

import knowledgeBases.JenaKBManager;
import knowledgeBases.KBManagerFactory;

/**
 * This class try to write the reification statements into our knowledge base. 
 * 
 * It could be extended with more complex query check, involving the semantic interpretation of the 
 * reification statement. 
 * 
 * @author tlu
 *
 */
public class DLReificationDBTest extends DLTestTemplete {

    @Override
    protected void run() {
        JenaKBManager kbm = (JenaKBManager) KBManagerFactory.createKBManager(JenaKBManager.JENA_PELLET_PELLET_DB);
        if (kbm.buildKB(ontologyURL, schemaUrl, dataLocations, "pellet")) {
            testReificationStatements(kbm);
            complexQueryTestSPARQL(kbm);
            kbm.clean();
        }
    }

    public static void main(String[] args) {
        DLReificationDBTest test = new DLReificationDBTest();
        test.run();
    }
}
