package tests.jenaKBTests.DLwithLargeScaleDataTests;

import knowledgeBases.JenaKBManager;
import knowledgeBases.KBManagerFactory;
import tests.TestUtilities;
import tests.jenaKBTests.DLOntologyTests.DLReificationDBTest;

public class BasicTest
    extends DLwithLargeScaleDataTestTemplate
{
    @Override
    protected void run() throws Exception
    {

        JenaKBManager kbm = (JenaKBManager) KBManagerFactory
                .createKBManager(JenaKBManager.JENA_PELLET_DB);

        if (kbm.buildKB(ontologyURL, schemaUrl, dataLocations, "pellet")) {// build
            // KB
            TestUtilities.showClassTree(schemaUrl);
            kbm.renderKnowledge();
            kbm.clean();

        }

    }

    public static void main(String[] args) throws Exception
    {
        BasicTest test = new BasicTest();
        test.run();
    }
}
