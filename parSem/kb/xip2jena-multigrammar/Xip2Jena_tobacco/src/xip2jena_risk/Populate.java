package xip2jena_risk;

import com.hp.hpl.jena.ontology.OntModel;
import com.hp.hpl.jena.rdf.model.Model;
import com.hp.hpl.jena.rdf.model.ModelFactory;
import com.xerox.jxip.JXip;
import com.xerox.xrce.xip2jena.DataStore;
import com.xerox.xrce.xip2jena.DependencyMapper;
import com.xerox.xrce.xip2jena.MentionsMerger;
import com.xerox.xrce.xip2jena.ParsemGenericOntology;
import com.xerox.xrce.xip2jena.PersonsEntityMerger;
import com.xerox.xrce.xip2jena.Util;
import com.xerox.xrce.xip2jena.Xip2Jena;
import com.xerox.xrce.xip2jena.Xip2JenaException;
import com.xerox.xrce.xip2jena.coref.Coref2Jena;
import com.xerox.xrce.xip2jena.corpus.Corpus;
import com.xerox.xrce.xip2jena.corpus.Document;
import com.xerox.xrce.xip2jena.temporal.EventTemporal;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.FilenameFilter;
import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import java.util.Properties;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import static com.xerox.xrce.xip2jena.Util.registerOntology;

public class Populate {
    public static void main(String[] args) throws Exception {
        Logger log = Logger.getLogger(Populate.class);
        List<File> temporalInfiles = new ArrayList<File>();


        //--Tobacco clean subset for cross-document coref scalability check
        //infiles.addAll(Util.listFiles(new File("/opt/WPT/wptparsem/ToolsLitigation/Knowledge_base/Corpora/Tobacco_clean_strict")));

        //--Iran corpus for cross-document coref check
        //infiles.addAll(Util.listFiles(new File("/opt/WPT/wptparsem/ToolsLitigation/Knowledge_base/Corpora/iran-corpus")));
        //temporalInfiles.addAll(Util.listFiles(new File("/home/nlagos/code/parSem/kb/xip2jena-multigrammar/Xip2Jena_tobacco/temporal-output-iran-new")));

        //--Tobacco small subset with temporal information--
        //File inputDir = new File("/opt/WPT/wptparsem/ToolsLitigation/Knowledge_base/Corpora/Tobacco_clean_strict");
        //infiles.addAll(Arrays.asList(inputDir.listFiles(new TxtFilter())));
        //temporalInfiles.addAll(Util.listFiles(new File("/home/nlagos/code/parSem/kb/xip2jena-multigrammar/Xip2Jena_tobacco/temporal-output-tobacco-clean-0")));
        //-------------

        //--Test coref invocation exception
        //infiles.add(new File("/opt/WPT/wptparsem/ToolsLitigation/Knowledge_base/Corpora/Tobacco_clean_strict/0/xib66d00.txt"));
        //infiles.add(new File("/opt/WPT/wptparsem/ToolsLitigation/Knowledge_base/Corpora/Tobacco_clean_strict/0/kmf43a00.txt"));
        

        //---Check cross-document coref and cataphora
        Corpus corpus = new IranXdocTestCorpus();

        Properties logprops = new Properties();
        //logprops.setProperty("log4j.logger.com.xerox.xrce.xip2jena", "ALL");
        //logprops.setProperty("log4j.logger.com.hp.hpl.jena.util.FileManager", "ALL");
        logprops.setProperty("log4j.logger.org.mindswap.pellet", "warn");
        PropertyConfigurator.configure(logprops);

            //System.out.println("The time is:" + System.currentTimeMillis());

        DataStore db = StorageConfigTobacco.connect();
        JXip jxip = new JXip();
        
        registerOntology(TobaccoOntology.class);
        registerOntology(EventTemporal.class);
        registerOntology(ParsemGenericOntology.class);

        URL ontUrl = Populate.class.getResource("TobaccoOntology.owl");

        db.clean();

        String riskGrmUri = Xip2Jena.grammarBase + "polrisk";
        String riskGrmRelFile = "grammar/english_pr_kr.grm";
        File riskGrmFile = Util.findGrammarFile(Populate.class, riskGrmRelFile);
        Model riskModel = ModelFactory.createDefaultModel();

                

        Model documentModel = ModelFactory.createDefaultModel();
        Model  tempModel = ModelFactory.createDefaultModel();

        Coref2Jena coref2Jena = new Coref2Jena(jxip, documentModel);

        OntModel ontology = ModelFactory.createOntologyModel();
        ontology.read(ontUrl.toString());

        MentionsMerger mentionsMerger = new MentionsMerger(documentModel,ontology);

        DependencyMapper depmapper = new DependencyMapper(ontUrl, riskGrmUri, riskGrmFile, jxip, riskModel);

        PersonsEntityMerger personsMerger = new PersonsEntityMerger(db.getDefaultModel(),ontology);

        for (Document d : corpus) {
            File f = d.getFile();

            System.out.println("File: " + f.getAbsolutePath());
            try {
                coref2Jena.populate(f);
                depmapper.populate(f);
                mentionsMerger.merge(riskModel);
                personsMerger.merge(documentModel);
            //tempModel.read("file:/home/nlagos/code/parSem/kb/xip2jena-multigrammar/Xip2Jena_tobacco/temporal-output-tobacco-clean-0/"+ f.getName()+".ttl", "N3");
            //mentionsMerger.merge(tempModel);
            //personsMerger.merge(corefModel);
            } catch (Xip2JenaException e) {
                log.error("Coref failed to process " + f.getPath(), e);
            }

            documentModel.setNsPrefix("risk", TobaccoOntology.NS);
            documentModel.setNsPrefix("time", EventTemporal.NS);

            riskModel.removeAll();
            tempModel.removeAll();
            documentModel.removeAll();

        }
           
        //System.out.println("The last time is:" + System.currentTimeMillis());
        try{
            createFiles("Risk/DocumentLucene", db.getDefaultModel());
        } catch (FileNotFoundException fe){
            fe.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    private static void createFiles (String filename, Model model) throws FileNotFoundException, IOException{
        String outFileName =  filename + ".owl";
        System.out.println("Writing output file " + outFileName);
        BufferedWriter out = new BufferedWriter(new FileWriter(outFileName));
        model.write(out);
        out.close();
    }
   
}

class TxtFilter implements FilenameFilter {
    public boolean accept(File dir, String name) {
        return (name.endsWith(".txt"));
    }
}
