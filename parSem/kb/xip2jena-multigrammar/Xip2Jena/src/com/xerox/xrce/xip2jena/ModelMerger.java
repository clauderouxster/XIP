package com.xerox.xrce.xip2jena;

import com.hp.hpl.jena.ontology.OntModel;
import com.hp.hpl.jena.rdf.model.Model;
import com.hp.hpl.jena.rdf.model.Property;
import com.hp.hpl.jena.rdf.model.RDFNode;
import com.hp.hpl.jena.rdf.model.Resource;
import com.hp.hpl.jena.rdf.model.Statement;
import com.hp.hpl.jena.rdf.model.StmtIterator;
import com.hp.hpl.jena.vocabulary.RDF;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import org.apache.log4j.Logger;


public abstract class ModelMerger {

    private static Logger log = Logger.getLogger(ModelMerger.class);
    private Model destModel;
    private OntModel ontology;

    public ModelMerger(Model aDestModel, OntModel aOntology) {
        destModel = aDestModel;
        ontology = aOntology;
    }

    public Model getDestModel() {
        return destModel;
    }

    public void setDestModel(Model destModel) {
        this.destModel = destModel;
    }

    public OntModel getOntology() {
        return ontology;
    }

    public void setOntology(OntModel ontology) {
        this.ontology = ontology;
    }

    public void merge(Model srcModel) throws EntityResolutionException {
        StmtIterator stmts = srcModel.listStatements();
        /*
        //-----------Making the statements a list and sorting them to avoid non-determinism
        List <Statement> stmtsList = stmts.toList();
        Collections.sort(stmtsList, new StmtSortByObj());
        System.out.println(stmtsList.toString());
        //---------------------------------------------------------------------------------
        for (Statement stmt : stmtsList) {*/
        while (stmts.hasNext()){
            Statement stmt = stmts.nextStatement();
            Resource s = mapResource(stmt.getSubject());
            Property p = stmt.getPredicate();
            RDFNode o = mapNode(stmt.getObject());
            destModel.add(s, p, o);
        }
    }

    private RDFNode mapNode(RDFNode origNode) throws EntityResolutionException {
        if (origNode.isLiteral()) {
            return origNode.inModel(destModel);
        }
        return mapResource((Resource) origNode);
    }


    abstract Resource mapResource(Resource srcNode) throws EntityResolutionException;

    List<Property> getTypes(Resource r) {
        List<?> stmts = r.listProperties(RDF.type).toList();
        List<Property> results = new ArrayList<Property>(stmts.size());
        for (Object s : stmts) {
            results.add((Property) ((Statement) s).getObject().as(Property.class));
        }
        return results;
    }

    public class EntityResolutionException extends Exception {

        public EntityResolutionException(Exception e) {
            super(e);
        }

        public EntityResolutionException(String s) {
            super(s);
        }
    }

    /*public class StmtSortByObj implements Comparator<Statement>{

        public int compare(Statement s1, Statement s2) {
            return s1.getPredicate().getLocalName().compareTo(s2.getPredicate().getLocalName());
        }
    }*/
}
