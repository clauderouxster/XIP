package xip2jena_risk;

import com.hp.hpl.jena.db.DBConnection;
import com.hp.hpl.jena.db.IDBConnection;
import com.hp.hpl.jena.rdf.model.Model;
import com.hp.hpl.jena.rdf.model.ModelFactory;
import com.hp.hpl.jena.rdf.model.ModelMaker;
import com.hp.hpl.jena.util.iterator.ExtendedIterator;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.URL;
import java.sql.SQLException;
import java.util.Properties;
import org.apache.log4j.PropertyConfigurator;
import com.xerox.xrce.xip2jena.Xip2Jena;
import java.io.File;
import java.util.ArrayList;
import java.util.List;

public class Xip2Jena_Risk {

    private static String outFileName = "output1";
    
    // Using an embedded database rather than a database server (e.g. MySQL)
    // for the time being because it's easier to set up.  Currently using
    // Derby simply because it's included in the Sun JDK. The received opinion
    // on the web seems to be that HSQLDB is better, can always try that later.
    // For large databases (over 8GB?) will probably want to switch to
    // a client/server db.
    private static String dbType = "Derby";
    private static String dbClass = "org.apache.derby.jdbc.EmbeddedDriver";
    private static String dbURL = "jdbc:derby:derbydb;create=true";
    private static String dbUser = "";
    private static String dbPw = "";
    
//    private static String dbType = "MySQL";
//    private static String dbClass = "com.mysql.jdbc.Driver";
//    private static String dbURL = "jdbc:mysql://localhost:3306/xip2jenarisk";
//    private static String dbUser = "x2jwriter";
//    private static String dbPw = "";
//

    public Xip2Jena_Risk() {
    }
    
    
    public static void populate() throws Exception {
        
        List<File> infiles = new ArrayList<File>();

//        infiles.addAll(listFiles(new File("/home/jahpine/TEMP/corpus_news/ap/filtr_text")));
//        infiles.addAll(listFiles(new File("/home/jahpine/TEMP/corpus_news/bbc/filtr_text")));
//        infiles.addAll(listFiles(new File("/home/jahpine/TEMP/corpus_news/usatoday/filtr_text")));

        infiles.add(new File("test/test"));
//        infiles.add(new File("test/Corpus_Iran_norm-aa"));
//        infiles.add(new File("test/Corpus_Iran_norm-ab"));
//        infiles.add(new File("test/Corpus_Iran_norm-ac"));
        
        
        URL grmFileUrl = Xip2Jena_Risk.class.getResource("grammar/english_pr_xml_kr.grm");
        if (!grmFileUrl.getProtocol().equals("file")) {
            throw new Exception("Found grammar file at a non-file location?!");
        }
        String grmFileName=grmFileUrl.getPath();
        URL ontFileUrl = Xip2Jena_Risk.class.getResource("RiskOntology.owl");
        String transFileName = "eventTrans";

        Properties logprops = new Properties();
        //logprops.setProperty("log4j.logger.com.xerox.xrce.xip2jena", "ALL");
        logprops.setProperty("log4j.logger.org.mindswap.pellet", "warn");
        PropertyConfigurator.configure(logprops);

        Xip2Jena xip2Jena = new Xip2Jena();
        xip2Jena.init(grmFileName, ontFileUrl); //, transFileName);
      
        // ensure the JDBC driver class is loaded
        Class.forName(dbClass);

        ModelMaker maker = getModelMaker(true);
        
        for (File infile : infiles) {
            Model model = maker.createModel(infile.toURI().toURL().toString());
            xip2Jena.processFile(infile, model);
        }
        
        System.out.println("Ignored dependencies:");
        for (String depname : xip2Jena.getIgnoredDeps()) {
            System.out.println(depname);
        }
    }
    
    public static void dump() throws FileNotFoundException, IOException, SQLException {
        OutputStream out = new FileOutputStream(outFileName);
        ModelMaker maker = getModelMaker(false);
        
        ExtendedIterator it = maker.listModels();
        while (it.hasNext()) {
            String modelName = (String) it.next();
            Model model = maker.openModel(modelName, true);
            model.write(out, "N3");
        }
        out.close();
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
    
    private static ModelMaker getModelMaker(boolean clean) throws SQLException {
        IDBConnection conn = new DBConnection(dbURL, dbUser, dbPw, dbType);
        if (clean) {
            conn.cleanDB();
        }
        ModelMaker maker = ModelFactory.createModelRDBMaker(conn);
        return maker;
    }


}
