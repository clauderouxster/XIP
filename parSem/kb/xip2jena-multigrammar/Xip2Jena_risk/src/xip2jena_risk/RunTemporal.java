/*
 * Copyright Xerox 2009
 */

package xip2jena_risk;

import com.xerox.xrce.xip2jena.temporal.TemporalParser;
import common.Options;
import com.xerox.xrce.xip2jena.corpora.IranCorpus;
import com.xerox.xrce.xip2jena.corpus.Corpus;
import com.xerox.xrce.xip2jena.corpus.Document;
import documentAnalysis.temporality.TemporalOptions;
import java.io.File;
import java.util.Properties;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import org.mindswap.pellet.PelletOptions;

public class RunTemporal {
    public static void main(String args[]) throws Exception {

        File outDir = new File("temporal-output");
        Corpus corpus = new IranCorpus();

        // documentAnalysis configuration
        Options options = new Options();
        options.setGrmFileName("../../../xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/SPECTEMP/GRMFILES/gram_temporalExp.grm");
        options.setProperty(Options.OUTPUT_DIR, outDir.getPath());
        options.setProperty(TemporalOptions.TEMPORAL_INFERENCE_LEVEL, "1");
        options.setProperty(TemporalOptions.TEMPORAL_COMPOSITION_LEVEL, "1");
        options.setProperty(TemporalOptions.TEMPORAL_ROBUST, "1");
        // options.setProperty(Options.APPLICATION,...) and Options.CORPUS
        // would have no effect if called here.  Use setApplication and setCorpus instead.
        options.setCorpus(corpus.getTemporalCorpus(options));

        // logging configuration
        Properties logprops = new Properties();
        logprops.setProperty("log4j.logger.org.mindswap.pellet", "warn"); // STFU!
        PelletOptions.USE_CLASSIFICATION_MONITOR = PelletOptions.MonitorType.NONE; // No, really, STFU!

        //////////////////////////////////////////////////////////////////////////
        // end configuration

        PropertyConfigurator.configure(logprops);
        Logger log = Logger.getLogger(RunTemporal.class);

        outDir.mkdir();
        if (!outDir.isDirectory() || !outDir.canWrite()) {
            throw new Exception(outDir.getCanonicalPath() + " not a writeable directory");
        }

        Logger.getLogger(RunTemporal.class).info("Output files going to " + outDir.getCanonicalPath());

        TemporalParser parser = new TemporalParser(options);

        for (Document d : corpus) {
            File f = d.getFile();
            try {
                log.info(f);
                parser.parse(f);
            } catch (Exception e) {
                log.warn("parsing failed on " + f, e);
            }
        }

    }

}
