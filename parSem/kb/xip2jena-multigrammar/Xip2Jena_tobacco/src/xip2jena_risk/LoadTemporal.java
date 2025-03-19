/*
 * Copyright Xerox 2009
 */

package xip2jena_risk;

import com.hp.hpl.jena.ontology.OntModel;
import com.hp.hpl.jena.rdf.model.Model;
import com.hp.hpl.jena.rdf.model.ModelFactory;
import com.xerox.xrce.xip2jena.DataStore;
import xip2jena_risk.StorageConfigTobacco;
import com.xerox.xrce.xip2jena.ModelMerger;
import com.xerox.xrce.xip2jena.ModelMerger.EntityResolutionException;
import com.xerox.xrce.xip2jena.ParsemGenericOntology;
import com.xerox.xrce.xip2jena.Util;
import com.xerox.xrce.xip2jena.temporal.EventTemporal;
import java.io.File;
import java.net.MalformedURLException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import static com.xerox.xrce.xip2jena.Util.registerOntology;

public class LoadTemporal {
    public static void main(String[] args) throws Exception {
        List<File> infiles = new ArrayList<File>();
        infiles.addAll(Util.listFiles(new File("temporal-output")));
        Collections.sort(infiles);

        Model tempModel = ModelFactory.createDefaultModel();

        DataStore db = StorageConfigTobacco.connect();
        OntModel ontology = ModelFactory.createOntologyModel();
        registerOntology(ParsemGenericOntology.class);
        ontology.read(EventTemporal.class.getResource("EventTemporal.owl").toString());
        //ModelMerger merger = new ModelMerger(db.getDefaultModel(), ontology);
        //doit(infiles, tempModel, merger);
    }

    // just separated this out in order to have a profiling root.
    private static void doit(List<File> infiles, Model tempModel, ModelMerger merger) throws MalformedURLException, EntityResolutionException {
        for (File f : infiles) {
            System.out.println(f);
            tempModel.read(f.toURI().toURL().toString(), "N3");
            merger.merge(tempModel);
            tempModel.removeAll();
        }
        //db.dump(System.out);
    }
}
