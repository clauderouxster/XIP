/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.xerox.xrce.xip2jena;

import com.hp.hpl.jena.ontology.OntModel;
import com.hp.hpl.jena.query.Query;
import com.hp.hpl.jena.query.QueryExecution;
import com.hp.hpl.jena.query.QueryExecutionFactory;
import com.hp.hpl.jena.query.QueryFactory;
import com.hp.hpl.jena.query.QuerySolution;
import com.hp.hpl.jena.query.ResultSet;
import com.hp.hpl.jena.rdf.model.Model;
import com.hp.hpl.jena.rdf.model.Property;
import com.hp.hpl.jena.rdf.model.Resource;
import com.hp.hpl.jena.rdf.model.Statement;
import com.hp.hpl.jena.rdf.model.StmtIterator;
import java.util.ArrayList;
import java.util.List;
import org.apache.log4j.Logger;
import static com.xerox.xrce.xip2jena.ParsemGenericOntology.*;

/**
 *
 * @author nlagos
 */
public class PersonsEntityMerger extends ModelMerger {

    private static Logger log = Logger.getLogger(PersonsEntityMerger.class);

    public PersonsEntityMerger (Model aDestModel, OntModel aOntology) {
        super(aDestModel,aOntology);
    }

    @Override
    Resource mapResource(Resource srcNode) throws EntityResolutionException {
        Resource result = null;
        List<Property> types = getTypes(srcNode);
        try{
            if (types.contains(Person))  {
                result = getEntity(srcNode);
            }
        }catch (EntityResolutionException EREx){
            log.error("Map resource problem", EREx);
        }

        if (result == null) {
            result = (Resource) srcNode.inModel(getDestModel());
        }

        return result;
    }


    /**
     * 
     * @param origEntity
     * @return
     * @throws com.xerox.xrce.xip2jena.ModelMerger.EntityResolutionException
     */
    public Resource getEntity(Resource origEntity) throws EntityResolutionException {
        List<Resource> results = new ArrayList<Resource>();
        String lastNameString = null;
        try {
            StmtIterator stmts = origEntity.listProperties(hasPersonName);

            while (stmts.hasNext()) {

                Resource personName = stmts.nextStatement().getResource();
                if (personName.hasProperty(hasLastName)){
                    Statement lastName = personName.getProperty(hasLastName);
                    lastNameString = lastName.getResource().getProperty(hasPersonNameAtomForm).getString();
                }
                Resource result;
                result = getEntity(lastNameString);
               //Add a method to get details of result and then use Sallah's method for name compatibility

                if (result != null) {
                    results.add(result);
                }
            }
        } catch (Exception e) {
            throw new EntityResolutionException(e);
        }
        if (results.size() > 1) {
            log.warn("Retrieved multiple entities, choosing one arbitrarily.");
        }
        if (results.size() >= 1) {
            return results.get(0);
        } else {
            return null;
        }
    }

    Resource getEntity(String lastNameString) {
        String queryString =
                "PREFIX parsem:<" + ParsemGenericOntology.NS + "> " +
                "SELECT ?indiv " +
                "WHERE {" +
                "    ?lastName parsem:hasPersonNameAtomForm \"" + lastNameString + "\" . " +
                "    ?personName parsem:hasLastName ?lastName . " +
                "    ?indiv parsem:hasPersonName ?personName . " +
                "}";
        log.debug(queryString);
        Query query = QueryFactory.create(queryString);
        QueryExecution qe = QueryExecutionFactory.create(query, getDestModel());
        ResultSet results = qe.execSelect();

        Resource result = null;
        if (results.hasNext()) {
            QuerySolution sol = results.nextSolution();
            log.debug(sol.toString());
            if (results.hasNext()) {
                log.warn("Multiple answers unexpected");
            }
            result = (Resource) sol.get("?indiv").as(Resource.class);
        }
        log.debug("done with query");
        return result;
    }

}
