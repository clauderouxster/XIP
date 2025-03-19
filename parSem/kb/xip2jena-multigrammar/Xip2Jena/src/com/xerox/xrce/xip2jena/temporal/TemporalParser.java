/*
 * Copyright Xerox 2009
 */

package com.xerox.xrce.xip2jena.temporal;

import com.hp.hpl.jena.rdf.model.Model;
import common.Options;
import common.TypedPropertyException;
import documentAnalysis.Main;
import documentAnalysis.temporality.TemporalConsistencyException;
import java.io.File;

/**
 * Cuts Xavier's megalomaniacal document processor down to size: makes it a
 * library that can be used in other applications.
 */
public class TemporalParser {
    private Options options;
    private Main main;
    private AppToRDF appToRDF;
    
    public TemporalParser(Options options) throws Exception {
        this.options = options;
        if (options.getApplication() != null) {
            throw new Exception("misconfigured Options");
        }
        appToRDF = new AppToRDF();
        options.setApplication(appToRDF);
        main = new Main(options);
        appToRDF.openOutput(options);
        appToRDF.preParsing(options);

    }

    public void parse(File file, Model destModel) throws TypedPropertyException, TemporalConsistencyException {
        appToRDF.destModel = destModel;
        options.setInputFileName(file.getPath());
        main.parse(options);
    }
}

// TemporalParser.parse
//    options.setInputFileName
//    Main.parse
//        JXip.xipResultBuilderFromTextfile
//            Main.myCB1
//                options.getCorpus().getSentenceInfo
//                TempFunctions.basicInference(this.currentUnit, this.options);
//                    storeEvents
//                    storeDates (optional)
//                (unnecessary) EntityFunctions.parseEntities(this.currentUnit, this.options);
//                options.getApplication().updateOutput(this.currentUnit, this.options);
//        options.getApplication().closeOutput(options);
