/*
 * Copyright Xerox 2009
 */

package xip2jena_risk;

import com.hp.hpl.jena.ontology.OntClass;
import com.xerox.xrce.xip2jena.OntologyLocationException;
import com.xerox.xrce.xip2jena.ParsemGenericOntology;
import com.xerox.xrce.xip2jena.temporal.EventTemporal;
import com.xerox.xrce.xip2jena.visualization.InstanceBrowser;
import java.io.File;
import java.io.FileNotFoundException;
import static com.xerox.xrce.xip2jena.Util.registerOntology;

public class TestInstanceBrowser {

    public static void main(String[] args) throws OntologyLocationException, RuntimeException, FileNotFoundException {
        registerOntology(ParsemGenericOntology.class);
        registerOntology(EventTemporal.class);
        registerOntology(RiskOntology.class);
        File file = new File("test/dumpfile.064.owl");
        Class schemagenClass = RiskOntology.class;
        OntClass[] excludeClasses = {ParsemGenericOntology.Mention};
        InstanceBrowser.visualizeFile(file, schemagenClass, excludeClasses);
    }


}
