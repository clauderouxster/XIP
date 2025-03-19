/*
 * Copyright Xerox 2009
 */

package xip2jena_risk;

import com.hp.hpl.jena.rdf.model.Model;
import com.hp.hpl.jena.rdf.model.ModelFactory;
import com.xerox.xrce.xip2jena.RdfDiff;
import com.xerox.xrce.xip2jena.corpora.IranCorpus;
import com.xerox.xrce.xip2jena.temporal.TemporalParser;
import common.Options;
import documentAnalysis.temporality.TemporalOptions;
import java.io.File;
import java.io.InputStream;
import java.util.Properties;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;
import org.mindswap.pellet.PelletOptions;
import static org.junit.Assert.*;


public class RunTemporalTest {

    public RunTemporalTest() {
    }

    @BeforeClass
    public static void setUpClass() throws Exception {
    }

    @AfterClass
    public static void tearDownClass() throws Exception {
    }


    @Test
    public void testMain() throws Exception {
        File[] infiles = {
            new File("/home/akaplan/parSem/kb/xip2jena-multigrammar/Xip2Jena_tobacco/test/xip2jena_risk/RunTemporalTest.input0_20090908"),
            new File("/home/akaplan/parSem/kb/xip2jena-multigrammar/Xip2Jena_tobacco/test/xip2jena_risk/RunTemporalTest.input1_20050105"),
        };

        // documentAnalysis configuration
        Options options = new Options();
        options.setGrmFileName("../../../xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/SPECTEMP/GRMFILES/gram_temporalExp.grm");
        options.setProperty(TemporalOptions.TEMPORAL_INFERENCE_LEVEL, "1");
        options.setProperty(TemporalOptions.TEMPORAL_COMPOSITION_LEVEL, "1");
        options.setProperty(TemporalOptions.TEMPORAL_ROBUST, "1");
        // options.setProperty(Options.CORPUS,...)
        // would have no effect if called here.  Use setCorpus instead.
        options.setCorpus(new IranCorpus().getTemporalCorpus(options));

        // logging configuration
        Properties logprops = new Properties();
        logprops.setProperty("log4j.logger.org.mindswap.pellet", "warn"); // STFU!
        PelletOptions.USE_CLASSIFICATION_MONITOR = PelletOptions.MonitorType.NONE; // No, really, STFU!

        Logger.getLogger(RdfDiff.class).setLevel(Level.DEBUG);

        //////////////////////////////////////////////////////////////////////////
        // end configuration

        PropertyConfigurator.configure(logprops);
        TemporalParser parser = new TemporalParser(options);

        for (File f : infiles) {

            Model outputModel = ModelFactory.createDefaultModel();
            parser.parse(f, outputModel);

            // compare against saved result from before
            Model referenceModel = ModelFactory.createDefaultModel();
            InputStream s = RunTemporalTest.class.getResourceAsStream(f.getName()+".ReferenceModel.ttl");
            referenceModel.read(s, null, "N3");

            boolean same = RdfDiff.compareModels(referenceModel, outputModel).isomorphic();
            if (!same) {
                referenceModel.write(System.out, "N3");
                System.out.println("--------------------------------------------------");
                outputModel.write(System.out, "N3");
                fail();
            }
        }
    }
}


