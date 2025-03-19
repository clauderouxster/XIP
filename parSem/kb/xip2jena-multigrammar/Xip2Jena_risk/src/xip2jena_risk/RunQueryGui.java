/*
 * Copyright Xerox 2009
 */

package xip2jena_risk;

import com.hp.hpl.jena.ontology.OntClass;
import com.hp.hpl.jena.ontology.OntModel;
import com.hp.hpl.jena.ontology.OntModelSpec;
import com.hp.hpl.jena.rdf.model.ModelFactory;
import com.xerox.xrce.xip2jena.OntologyLocationException;
import com.xerox.xrce.xip2jena.ParsemGenericOntology;
import com.xerox.xrce.xip2jena.temporal.EventTemporal;
import com.xerox.xrce.xip2jena.visualization.QueryGui;
import java.sql.SQLException;
import static com.xerox.xrce.xip2jena.Util.registerOntology;

public class RunQueryGui {
    public static void main(String[] args) throws OntologyLocationException, ClassNotFoundException, SQLException {
        registerOntology(ParsemGenericOntology.class);
        registerOntology(EventTemporal.class);
        registerOntology(RiskOntology.class);
        OntClass[] excludeClasses = new OntClass[]{ParsemGenericOntology.Mention};
        OntModel ont = ModelFactory.createOntologyModel(OntModelSpec.OWL_MEM);
        ont.read(RiskOntology.class.getResource("RiskOntology.owl").toString());
        QueryGui.queryDB(StorageConfigRisk.connect(), ont, excludeClasses);
    }
}
