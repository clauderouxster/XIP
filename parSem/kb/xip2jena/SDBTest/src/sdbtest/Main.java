 /*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package sdbtest;

import com.hp.hpl.jena.ontology.OntModel;
import com.hp.hpl.jena.ontology.OntModelSpec;
import com.hp.hpl.jena.rdf.model.Model;
import com.hp.hpl.jena.rdf.model.ModelFactory;
import com.hp.hpl.jena.sdb.SDBFactory;
import com.hp.hpl.jena.sdb.Store;
import com.hp.hpl.jena.sdb.StoreDesc;
import com.hp.hpl.jena.sdb.sql.SDBConnection;
import com.hp.hpl.jena.sdb.store.DatabaseType;
import com.hp.hpl.jena.sdb.store.LayoutType;
import com.xerox.xrce.xip2jena.Xip2Jena;
import java.net.URL;
import java.util.Properties;
import org.apache.log4j.PropertyConfigurator;

import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;

/**
 *
 * @author nlagos
 */
public class Main {
    
//    private static DatabaseType DBTYPE = DatabaseType.MySQL;
//    private static String DBCLASS = "com.mysql.jdbc.Driver";
//    private static String DBURL = "jdbc:mysql://etretat:3306/sdbNamedGraphs";
//    private static String DBUSER = "root";
//    private static String DBPW = "root";
    
    private static DatabaseType DBTYPE = DatabaseType.Derby;
    private static String DBCLASS = "org.apache.derby.jdbc.EmbeddedDriver";
    private static String DBURL = "jdbc:derby:derbydb-sdbNamedGraphs;create=true";
    private static String DBUSER = "";
    private static String DBPW = "";

    public Main() {
    }
    
    public static void populate() throws Exception {
        
        
       
        Class.forName(DBCLASS);
        StoreDesc storeDesc= new StoreDesc(LayoutType.LayoutTripleNodesHash,
                                       DBTYPE) ;
        SDBConnection conn = new SDBConnection(DBURL, DBUSER, DBPW);
        Store store = SDBFactory.connectStore (conn, storeDesc);
        store.getTableFormatter().create();
        Model data = SDBFactory.connectDefaultModel(store);
        
        
        String outputDir = "output";
        
        URL grmFileUrl = Main.class.getResource("grammar/english_pr_xml_kr.grm");
        if (!grmFileUrl.getProtocol().equals("file")) {
            throw new Exception("Found grammar file at a non-file location?!");
        }
        String grmFileName=grmFileUrl.getPath();
        URL ontFileUrl = Main.class.getResource("RiskOntology.owl");
        String transFileName = "eventTrans";

        Properties logprops = new Properties();
        //logprops.setProperty("log4j.logger.com.xerox.xrce.xip2jena", "ALL");
        logprops.setProperty("log4j.logger.org.mindswap.pellet", "warn");
        PropertyConfigurator.configure(logprops);

        Xip2Jena xip2Jena = new Xip2Jena();
        xip2Jena.init(grmFileName, ontFileUrl); //, transFileName);
      
        List<File> infiles = new ArrayList<File>();

        //infiles.addAll(listFiles(new File("/home/jahpine/TEMP/bbc_corpus_20080514/filtr_text")));
        //infiles.addAll(listFiles(new File("/home/nlagos/test_data")));
        //infiles.add(new File("/home/nlagos/Test.txt"));
        
        //infiles.add(new File("test/Corpus_Iran_norm-selected.txt"));
        infiles.add(new File("test/Corpus_Iran_norm.txt"));
        
        
        //Test data
        int n=0;
       
        
        for (File infile : infiles) {
            String path = infile.toURI().toURL().toString();
            System.out.println("The name of the graph is:------------------------------  "+path);
            //OntModel model = ModelFactory.createOntologyModel(OntModelSpec.OWL_MEM_RULE_INF, data); 
            xip2Jena.processFile(infile, data, store);
            //System.out.println("The number of sentences is:"+xip2Jena.countingLines());
            //System.out.println("The number of inonsistent sentences is:"+xip2Jena.countingInconsistentLines());
            n++;
            System.out.println("I'm in file number ---"+n);
            /*String outFileName = outputDir + File.separator + infile.getName() + ".owl";
            System.out.println("Writing output file " + outFileName);
            OutputStream out = new FileOutputStream(outFileName);
            data.write(out);
            out.close();*/
            
        }
        
        //System.out.println("The number of sentences is:"+countingLines);
        //System.out.println("The number of inonsistent sentences is:"+numberOfSentencesWithInconsistency);
        //model.close();
        data.close();
        store.close();
        System.out.println("Ignored dependencies:");
        for (String depname : xip2Jena.getIgnoredDeps()) {
            System.out.println(depname);
        }
        
    }
    
    private static List<File> listFiles(File root) {
        File[] children = root.listFiles();
        ArrayList<File> results = new ArrayList<File>();
        for (File f : children) {
            if (f.isDirectory()) {
                results.addAll(listFiles(f));
            } else {
                results.add(f);
            }
        }
        return results;
    }
     
}
