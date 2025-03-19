package tests.jenaKBTests.DLwithRules;

import tests.TestUtilities;
import knowledgeBases.JenaKBManager;
import knowledgeBases.KBManagerFactory;

public class RulesDLTest extends DLwithRulesTestTemplete {

    @Override
    protected void run() throws Exception {
        log.info("Run the RulesDLTest.");
        JenaKBManager kbm = (JenaKBManager) KBManagerFactory.createKBManager(JenaKBManager.JENA_PELLET_DB);
        if (kbm.buildKB(ontologyURL, schemaUrl, dataLocations, "pellet")) {
            TestUtilities.tellMeAllAbout(kbm, "http://xrce.xerox.com/parsem/Entity-1818");
            kbm.clean();
        }
    }

    public static void main(String[] args) throws Exception {
        RulesDLTest test = new RulesDLTest();
        test.run();
    }
}
