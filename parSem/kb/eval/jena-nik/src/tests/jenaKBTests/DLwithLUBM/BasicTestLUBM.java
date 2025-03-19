/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package tests.jenaKBTests.DLwithLUBM;

import knowledgeBases.JenaKBManager;
import knowledgeBases.KBManagerFactory;
import tests.TestUtilities;
import tests.jenaKBTests.DLOntologyTests.DLReificationDBTest;
/**
 *
 * @author nlagos
 */
public class BasicTestLUBM 
    extends DLwithLUBMTemplate{
 @Override
    protected void run() throws Exception
    {

        JenaKBManager kbm = (JenaKBManager) KBManagerFactory
                .createKBManager(JenaKBManager.JENA_DL_DB);

        if (kbm.buildKB(ontologyURL, schemaUrl, dataLocations, "pellet")) {// build
            // KB
            TestUtilities.showClassTree(schemaUrl);
            kbm.renderKnowledge();
            //kbm.clean();

        }

    }

    public static void main(String[] args) throws Exception
    {
        BasicTestLUBM test = new BasicTestLUBM();
        test.run();
    }
}
