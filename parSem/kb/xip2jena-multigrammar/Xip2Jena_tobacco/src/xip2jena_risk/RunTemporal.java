/*
 * Copyright Xerox 2009
 */

package xip2jena_risk;

import com.xerox.xrce.xip2jena.temporal.TemporalParser;
import common.Options;
import com.xerox.xrce.xip2jena.temporal.AppToRDF;
import com.xerox.xrce.xip2jena.corpora.TobaccoCorpus;
import documentAnalysis.temporality.TemporalOptions;
import java.io.File;
import java.util.Properties;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;

public class RunTemporal {
/*
    private static File outDir = new File("temporal-output-tobacco-clean");
*/
    public static void main(String args[]) throws Exception {

        File inDir = new File("/opt/WPT/wptparsem/ToolsLitigation/Knowledge_base/Corpora/Tobacco_clean_strict/0");
        //File inDir = new File("/opt/WPT/wptparsem/ToolsLitigation/Knowledge_base/Corpora/iran-corpus");
        //File inDir = new File("/home/nlagos/test_data/Nicolas");
        File outDir = new File("temporal-output-tobacco-clean-0");

        // documentAnalysis configuration
        Options options = new Options();
        options.setGrmFileName("../../../xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/SPECTEMP/GRMFILES/gram_temporalExp.grm");
        options.setProperty(Options.OUTPUT_DIR, outDir.getPath());
        options.setProperty(TemporalOptions.TEMPORAL_INFERENCE_LEVEL, "1");
        options.setProperty(TemporalOptions.TEMPORAL_COMPOSITION_LEVEL, "1");
        options.setProperty(TemporalOptions.TEMPORAL_ROBUST, "1");
        // options.setProperty(Options.CORPUS,...)
        // would have no effect if called here.  Use setCorpus instead.
        options.setCorpus(new TobaccoCorpus().getTemporalCorpus(options));
        //options.setCorpus(new IranCorpus(options));

        // logging configuration
        Properties logprops = new Properties();
        logprops.setProperty("log4j.logger.org.mindswap.pellet", "warn"); // STFU!

        //////////////////////////////////////////////////////////////////////////
        // end configuration

        PropertyConfigurator.configure(logprops);
        Logger log = Logger.getLogger(RunTemporal.class);

        outDir.mkdir();
        if (!outDir.isDirectory() || !outDir.canWrite()) {
            throw new Exception(outDir.getCanonicalPath() + " not a writeable directory");
        }

        Logger.getLogger(RunTemporal.class).info("Output files going to " + outDir.getCanonicalPath());
/*
        documentAnalysis.Main.main(new String[]{
            "-grm", "../../../xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/SPECTEMP/GRMFILES/gram_temporalExp.grm",
            //"-dir", "/opt/WPT/wptparsem/ToolsLitigation/Knowledge_base/Corpora/iran-corpus",
            "-dir", "/opt/WPT/wptparsem/ToolsLitigation/Knowledge_base/Corpora/Tobacco_clean_strict/9",
            //"-text", "/opt/WPT/wptparsem/ToolsLitigation/Knowledge_base/Corpora/iran-corpus/295_20050902",
            //"-text", "/home/nlagos/time/test2.txt",
            //"-text", "/opt/WPT/wptparsem/ToolsLitigation/Knowledge_base/Corpora/Tobacco_clean_strict/2/idt40d00.txt",
            //"-text", "/home/nlagos/time/Frida_Kahlo.txt",
            //"-o", outDir.getPath(),
            "-odir", outDir.getPath(),
            "-temporal_inference_level", "1",
            "-temporal_composition_level", "2",
            "-temporal_robust", "1",
            "-app", "AppToRDF"
        });
*/
        int count=0;
        TemporalParser parser = new TemporalParser(options);
//parser.parse(inDir);
        boolean flag;
        for (File f : inDir.listFiles()) {
            flag=true;
            for (File f2 : outDir.listFiles()){
                if (f2.getName().contains(f.getName())){
                    flag = false;
                }
            }
            if (flag){
                    try {
                        System.out.println("The file is:"+f.getAbsolutePath()+" number "+count++);
                        parser.parse(f);
                    } catch (Exception e) {
                        log.warn("parsing failed on " + f, e);
                    }
            }
            
        }


    }

}
