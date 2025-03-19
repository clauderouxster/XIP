/*
 * Copyright Xerox 2009
 */

package com.xerox.xrce.xip2jena.visualization;

import com.hp.hpl.jena.ontology.Individual;
import com.hp.hpl.jena.ontology.OntClass;
import com.hp.hpl.jena.query.Query;
import com.hp.hpl.jena.query.QueryExecution;
import com.hp.hpl.jena.query.QueryExecutionFactory;
import com.hp.hpl.jena.query.QueryFactory;
import com.hp.hpl.jena.query.ResultSet;
import com.hp.hpl.jena.rdf.model.Literal;
import com.hp.hpl.jena.rdf.model.Property;
import com.hp.hpl.jena.rdf.model.RDFNode;
import com.hp.hpl.jena.rdf.model.Statement;
import com.hp.hpl.jena.rdf.model.StmtIterator;
import com.xerox.xrce.xip2jena.ParsemGenericOntology;
import java.util.NoSuchElementException;
import prefuse.render.LabelRenderer;
import prefuse.visual.VisualItem;
import static com.xerox.xrce.xip2jena.visualization.InstanceDisplay.C_INDIV;

public class IndividualRenderer extends LabelRenderer {

    private static String getIndivName(Individual i) throws NoSuchElementException {
        StringBuilder sb = new StringBuilder();
        String name = "";
        String uri = i.getURI();
        if (uri == null) {
            uri = "_:" + i.getId().getLabelString();
        }
        if (name.equals("")) {
            String queryString = "PREFIX parsem:<" + ParsemGenericOntology.NS + "> " +
                    "SELECT ?nameString " +
                    "WHERE { <" + uri + "> parsem:hasName ?name ." +
                    "        ?name parsem:hasNameForm ?nameString" + "}";
            Query query = QueryFactory.create(queryString);
            QueryExecution qe = QueryExecutionFactory.create(query, i.getOntModel());
            ResultSet results = qe.execSelect();
            if (results.hasNext()) {
                name = results.nextSolution().get("?nameString").toString();
            }
        }
        if (name.equals("")) {
            String queryString = "PREFIX parsem:<" + ParsemGenericOntology.NS + "> " +
                    "SELECT ?nameString " +
                    "WHERE { <" + uri + "> parsem:hasMention ?mention ." +
                    "        ?mention parsem:hasForm ?nameString" + "}";
            Query query = QueryFactory.create(queryString);
            QueryExecution qe = QueryExecutionFactory.create(query, i.getOntModel());
            ResultSet results = qe.execSelect();
            if (results.hasNext()) {
                name = results.nextSolution().get("?nameString").toString();
            }
        }
        if (name.equals("")) {
            name = i.getLocalName();
        }
        return name;
    }

    private static String getIndivClasses(Individual i) {
        StringBuilder sb = new StringBuilder();
        for (Object o : i.listOntClasses(true).toList()) {
            OntClass oc = (OntClass) o;
            if (!oc.isAnon()) {
                sb.append(oc.getLocalName());
                sb.append(" ");
            }
        }
        return sb.toString();
    }
    
    private static String getIndivProperties(Individual i) {
        StringBuilder sb = new StringBuilder();
        StmtIterator stmts = i.listProperties();
        while (stmts.hasNext()) {
            Statement stmt = stmts.nextStatement();
            Property prop = stmt.getPredicate();
            RDFNode obj = stmt.getObject();
            if (obj.isLiteral()) {
                sb.append("\n");
                sb.append(prop.getLocalName() + ": " +
                        ((Literal) obj.as(Literal.class)).getString());
            }
        }
        return sb.toString();
    }

    @Override
    protected String getText(VisualItem item) {
        String result = "";
        if (item.canGet(C_INDIV, Individual.class)) {
            Individual i = (Individual) item.get(C_INDIV);
            result = getIndivClasses(i) + "\n" + getIndivName(i) + getIndivProperties(i);
        }
        return result;
    }

}
