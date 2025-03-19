package tests.jenaKBTests.DLwithLargeScaleDataTests;

import com.hp.hpl.jena.query.ResultSet;
import tests.TestUtilities;
import knowledgeBases.JenaKBManager;
import knowledgeBases.KBManagerFactory;

public class QueryTest
    extends DLwithLargeScaleDataTestTemplate
{
    @Override
    protected void run() throws Exception
    {
        log.info("Run the QueryTest.");

        JenaKBManager kbm = (JenaKBManager) KBManagerFactory
                .createKBManager(JenaKBManager.JENA_PELLET_DB);

        if (kbm.buildKB(ontologyURL, schemaUrl, dataLocations, "pellet")) {
            //TestUtilities.validationTest(kbm);
            //TestUtilities.tellMeAllAbout(kbm, "http://xrce.xerox.com/parsem/PrimitiveData-9183");
            
      //each following methods can return the corresponding ResultSet of the query
        	//TestUtilities.showClassTree(schemaUrl);
 //           TestUtilities.getEntityTypeOccurrences(kbm, "Country");
 //           TestUtilities.getOccurrences(kbm,"Hassan Rowhani","Person");
     //       TestUtilities.getOccurrences(kbm,"Hassan Rowhani","Entity");
            TestUtilities.getOccurrences(kbm,"Iran","Entity");
            //TestUtilities.getOccurrences(kbm,"Russia","Country");
//            TestUtilities.getOccurrences(kbm,"Iran","Person");
            //TestUtilities.getEvent(kbm,"Iran","Entity");
 //           TestUtilities.getEvent(kbm,"Russia","Country");
//            TestUtilities.getEventWithCore(kbm, "Russia", "Country");
           // TestUtilities.getParticipantOfEvent(kbm,"ElBaradei","Person");
            
            
            
           //kbm.clean();
        }

    }


    public static void main(String[] args) throws Exception
    {
        QueryTest test = new QueryTest();
        test.run();
    }
}
