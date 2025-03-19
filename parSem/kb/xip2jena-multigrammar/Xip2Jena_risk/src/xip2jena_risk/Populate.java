package xip2jena_risk;

import com.hp.hpl.jena.ontology.OntModel;
import com.hp.hpl.jena.rdf.model.Model;
import com.hp.hpl.jena.rdf.model.ModelFactory;
import com.xerox.jxip.JXip;
import com.xerox.xrce.xip2jena.corpus.Corpus;
import com.xerox.xrce.xip2jena.DataStore;
import com.xerox.xrce.xip2jena.ModelMerger;
import com.xerox.xrce.xip2jena.DependencyMapper;
import com.xerox.xrce.xip2jena.corpus.Document;
import com.xerox.xrce.xip2jena.MentionsMerger;
import com.xerox.xrce.xip2jena.ParsemGenericOntology;
import com.xerox.xrce.xip2jena.Util;
import com.xerox.xrce.xip2jena.Xip2Jena;
import com.xerox.xrce.xip2jena.Xip2JenaException;
import com.xerox.xrce.xip2jena.coref.Coref2Jena;
import com.xerox.xrce.xip2jena.temporal.EventTemporal;
import java.io.File;
import java.net.URL;
import java.util.Properties;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import static com.xerox.xrce.xip2jena.Util.registerOntology;

public class Populate {
    public static void main(String[] args) throws Exception {
        Logger log = Logger.getLogger(Populate.class);

        Corpus corpus = new com.xerox.xrce.xip2jena.corpora.IranCorpus();
        //Corpus corpus = new com.xerox.xrce.xip2jena.corpora.JulienCorpus();
        //Corpus corpus = new com.xerox.xrce.xip2jena.corpora.JulienCorpusSmallSubset();
        //Corpus corpus = new com.xerox.xrce.xip2jena.corpora.TobaccoCorpus();
        //Corpus corpus = new com.xerox.xrce.xip2jena.corpora.TestCorpus();



        Properties logprops = new Properties();
        //logprops.setProperty("log4j.logger.com.xerox.xrce.xip2jena", "ALL");
        //logprops.setProperty("log4j.logger.com.xerox.xrce.xip2jena.coref", "ALL");
        //logprops.setProperty("log4j.logger.com.hp.hpl.jena.util.FileManager", "ALL");
        logprops.setProperty("log4j.logger.org.mindswap.pellet", "warn");
        //logprops.setProperty("log4j.logger.com.xerox.xrce.xip2jena.DependencyMapper", "debug");
        PropertyConfigurator.configure(logprops);

        DataStore store = StorageConfigRisk.connect();
        JXip jxip = new JXip();

        URL ontUrl = RiskOntology.class.getResource("RiskOntology.owl");

        store.clean();

        Coref2Jena coref2Jena = new Coref2Jena(jxip, store.getDefaultModel());
        // Since this is the first one in the chain, it can populate the
        // db directly, rather than populating a temporary model and then using
        // the merger to merge the contents into the db.
        for (Document d : corpus) {
            File f = d.getFile();
            try {
                coref2Jena.populate(f);
            } catch (Xip2JenaException e) {
                log.error("Coref failed to process " + f.getPath(), e);
            }
        }

        String riskGrmUri = Xip2Jena.grammarBase + "polrisk";
        String riskGrmRelFile = "grammar/english_pr_kr.grm";
        File riskGrmFile = Util.findGrammarFile(Populate.class, riskGrmRelFile);
        Model riskModel = ModelFactory.createDefaultModel();
        registerOntology(EventTemporal.class);
        registerOntology(ParsemGenericOntology.class);
        OntModel ontology = ModelFactory.createOntologyModel();
        ontology.read(ontUrl.toString());
        ModelMerger merger = new MentionsMerger(store.getDefaultModel(),ontology);
        DependencyMapper depmapper = new DependencyMapper(ontUrl, riskGrmUri, riskGrmFile, jxip, riskModel);
        for (Document d : corpus) {
            File f = d.getFile();
            try {
                depmapper.populate(f);
            } catch (Exception e) {
                log.error("depmapper failed on " + f.getPath(), e);
            }
            try {
                merger.merge(riskModel);
            } catch (Exception e) {
                log.error("merger failed on " + f.getPath(), e);
            }
            riskModel.removeAll();
        }

        store.getDefaultModel().setNsPrefix("risk", RiskOntology.NS);
        store.getDefaultModel().setNsPrefix("time", EventTemporal.NS);
        store.dump(System.out, "N3");
        //store.dump(new FileOutputStream("dumpfile"), "N3");
   }
    
}
