/*
 * AppToRDF.java
 *
 * Created on February 25, 2009
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.xerox.xrce.xip2jena.temporal;

import common.applications.*;
import com.hp.hpl.jena.rdf.model.Model;
import common.Options;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import org.apache.log4j.Logger;

/**
 * Class for specific methods for AppToRDF mining.
 */
public class AppToRDF extends Application {

    static Logger log = Logger.getLogger(AppToRDF.class);
    public Model destModel = null;

    /**
     * Creates a new instance of AppToRDF
     */
    public AppToRDF() {
        super();
        this.name = "TORDF";
    }


    /** Creates a new instance of AppToRDF
     *
     * @param options the parser options
     */
    public AppToRDF(Options options) {
        this();
    }

    /**
     * Call ToRDF, to generate the RDF related code after each file is processed.
     * 
     * @param options
     */
    @Override
    public void closeOutput (Options options){
        assert destModel != null;
        ToRDF toRDF = new ToRDF();
        try {
            toRDF.init(destModel);
            toRDF.processDocument(options.getInputFileName(),
                    options.getTemporalGraph());
        } catch (Exception ex) {
            log.error(ex);
        }

    }
    /**
     * I would like add this method in the Application and main for directly passing the model from Xip2Jena_risk
     */
    /*@Override
    public Model getRDFOutput (Options options, Model model){
        ToRDF toRDF = new ToRDF();
        Model model = ModelFactory.createDefaultModel();
        try {
     * change the init if this is used
            toRDF.init(model);
            model=toRDF.processDocument(options);
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return model;
    }*/

}




