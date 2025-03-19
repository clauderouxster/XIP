package tests.jenaKBTests.DLwithLUBM;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.IOException;
import java.util.Scanner;
import tests.TestUtilities;
import knowledgeBases.JenaKBManager;
import knowledgeBases.KBManagerFactory;

public class QueryTestLUBM
    extends DLwithLUBMTemplate
{
    
    private static BufferedReader buff;
    private static String s;
    
    @Override
    protected void run() throws Exception
    {
        JenaKBManager kbm = (JenaKBManager) KBManagerFactory
               .createKBManager(JenaKBManager.JENA_DL_DB);
        //kbm.clean();
        Scanner reader = new Scanner(System.in);
        int in = 0;
        
        System.out.println("Please select the reasoner to use");
        System.out.println("Options: 1. Native OWL reasoner");
        System.out.println("2. Pellet OWL DL reasoner");
        System.out.println("3. SPARQL query to database\n");
        
        in = reader.nextInt();
        switch (in){
                case 1: kbm = buildKB(kbm, "internal");break;
                case 2: kbm = buildKB(kbm, "externalpellet");break;
                case 3: kbm = buildKB(kbm, "");break;
                default:System.out.println("The option you entered is incorrect - No reasoner is attached");kbm = buildKB(kbm, "");break;
            }
        
         runQuery (kbm);
                
    }
    
    private static JenaKBManager buildKB(JenaKBManager kbm, String reasoner){
        
        if (kbm.buildKB(ontologyURL, schemaUrl, dataLocations, reasoner)) {     
        }
        return kbm;
    }
  
    
    private static void runQuery (JenaKBManager kbm){
        log.info("Run the QueryTest.");
  
        double time = System.currentTimeMillis();
            //TestUtilities.getOccurrences(kbm, "takesCourse", "http://www.Department0.University0.edu/GraduateCourse0", "GraduateStudent");
            //TestUtilities.getEntityTypeOccurrences(kbm, "Chair");
        
        TestUtilities.selectNamedGraphs(kbm, "he");

        TestUtilities.TestNamedGraphsWithReasoner(kbm, "Person", "hasParticipant", "Event");
           
            //TestUtilities.getNames(kbm, "hasTime", "PAST", "Event");
            
            //TestUtilities.getCooccuringPeopleGraphs(kbm);
           
        log.info("The query time is: "+(Double.toString(System.currentTimeMillis()-time)));
            //kbm.clean();
        
    }

    
    public static void main(String[] args) throws Exception
    {
        
        QueryTestLUBM test = new QueryTestLUBM();
        test.run();
        
    }
}