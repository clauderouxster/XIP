package com.xerox.xrce.xip2jena.storageconfigs;

import com.xerox.xrce.xip2jena.*;
import com.hp.hpl.jena.query.Dataset;
import com.hp.hpl.jena.rdf.model.Model;
import java.io.OutputStream;
import java.io.PrintStream;
import com.hp.hpl.jena.tdb.TDBFactory;
import org.apache.log4j.Logger;

public class DataStoreTdb extends DbImpl {

    private String directory;
    private Dataset dataset;
    //private Model model;
    private static Logger log = Logger.getLogger(DataStoreTdb.class);

    public DataStoreTdb(String aDirectory) throws ClassNotFoundException {
        directory = aDirectory;
        init();
    }

    @Override
    public void close() {
        dataset.close();
    }

    @Override
    public Model getDefaultModel() { return dataset.getDefaultModel(); }

    //Doesn't work with current combination of jena/tdb libraries
    @Override
    public void dump(OutputStream out, String format) {
        PrintStream psout = new PrintStream(out);
        dataset.getDefaultModel().write(psout, format);
    }

    @Override
    public void clean(){
        dataset.getDefaultModel().removeAll();
    }

    private void init() {
        dataset = TDBFactory.createDataset(directory);
        //model = TDBFactory.createModel(directory);
        // Strangely, the SDB API doesn't provide a way to check if the tables
        // have been set up.  Setting them up again if they exist already 
        // has the effect of deleting all the previously-stored information, which
        // is not what we want.
        dataset.getDefaultModel().setNsPrefix("indiv", Xip2Jena.indivBase);
        dataset.getDefaultModel().setNsPrefix("parsem", ParsemGenericOntology.NS);
    }

}
