/*
 * Copyright Xerox 2009
 */

package xip2jena_risk;

import com.hp.hpl.jena.ontology.OntModel;
import com.hp.hpl.jena.rdf.model.Model;
import com.hp.hpl.jena.rdf.model.ModelFactory;
import com.xerox.jxip.JXip;
import com.xerox.xrce.xip2jena.DataStore;
import com.xerox.xrce.xip2jena.DependencyMapper;
import com.xerox.xrce.xip2jena.MentionsMerger;
import com.xerox.xrce.xip2jena.ModelMerger;
import com.xerox.xrce.xip2jena.ParsemGenericOntology;
import com.xerox.xrce.xip2jena.RdfDiff;
import com.xerox.xrce.xip2jena.Util;
import com.xerox.xrce.xip2jena.Xip2Jena;
import com.xerox.xrce.xip2jena.coref.Coref2Jena;
import com.xerox.xrce.xip2jena.corpus.Corpus;
import com.xerox.xrce.xip2jena.corpus.Document;
import com.xerox.xrce.xip2jena.storageconfigs.DataStoreTdb;
import com.xerox.xrce.xip2jena.temporal.EventTemporal;
import java.io.File;
import java.net.URL;
import java.util.Properties;
import org.apache.log4j.PropertyConfigurator;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static com.xerox.xrce.xip2jena.Util.registerOntology;


public class EndToEndTest {

    public EndToEndTest() {
    }

    @BeforeClass
    public static void setUpClass() throws Exception {
    }

    @AfterClass
    public static void tearDownClass() throws Exception {
    }

    @Before
    public void setUp() {
    }

    @After
    public void tearDown() {
    }

    @Test
    public void EndToEndTest1() throws Exception {
        System.out.println("EndToEndTest1");

        final String expectedFileResource = "EndToEndTest-expected-output1.n3";
        final String storepath = "/local/tdbstore-EndToEndTest1";

        Model expected = ModelFactory.createDefaultModel();
        expected.read(EndToEndTest.class.getResource(expectedFileResource).toString(), "N3");

        Corpus corpus = new com.xerox.xrce.xip2jena.corpora.JulienCorpusSmallSubset();
        Properties logprops = new Properties();
        logprops.setProperty("log4j.logger.org.mindswap.pellet", "warn");
        PropertyConfigurator.configure(logprops);
        DataStore store = new DataStoreTdb(storepath);
        JXip jxip = new JXip();
        store.clean();
        Coref2Jena coref2Jena = new Coref2Jena(jxip, store.getDefaultModel());
        // Since this is the first one in the chain, it can populate the
        // db directly, rather than populating a temporary model and then using
        // the merger to merge the contents into the db.
        System.out.println("coref");
        for (Document d : corpus) {
            File f = d.getFile();
            System.out.println(f);
            coref2Jena.populate(f);
        }

        String riskGrmUri = Xip2Jena.grammarBase + "polrisk";
        String riskGrmRelFile = "grammar/english_pr_kr.grm";
        File riskGrmFile = Util.findGrammarFile(Populate.class, riskGrmRelFile);
        Model riskModel = ModelFactory.createDefaultModel();
        registerOntology(EventTemporal.class);
        registerOntology(ParsemGenericOntology.class);
        OntModel ontology = ModelFactory.createOntologyModel();
        URL ontUrl = RiskOntology.class.getResource("RiskOntology.owl");
        ontology.read(ontUrl.toString());
        ModelMerger merger = new MentionsMerger(store.getDefaultModel(),ontology);
        DependencyMapper depmapper = new DependencyMapper(ontUrl, riskGrmUri, riskGrmFile, jxip, riskModel);
        System.out.println("depmapper");
        for (Document d : corpus) {
            File f = d.getFile();
            depmapper.populate(f);
            merger.merge(riskModel);
            riskModel.removeAll();
        }

        System.out.println("diffing");
        RdfDiff.compareModels(expected, store.getDefaultModel());

        store.close();
        new File(storepath).delete();
        
        // Uncomment to generate new dump.  Then move the generated file to
        // the appropriate place in the class hierarchy.
        //store.getDefaultModel().setNsPrefix("risk", RiskOntology.NS);
        //store.getDefaultModel().setNsPrefix("time", EventTemporal.NS);
        //store.dump(new FileOutputStream(expectedFileResource), "N3");


    }

}