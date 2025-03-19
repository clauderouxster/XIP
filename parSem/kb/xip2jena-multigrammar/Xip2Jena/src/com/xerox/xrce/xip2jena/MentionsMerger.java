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
import com.hp.hpl.jena.rdf.model.StmtIterator;
import com.hp.hpl.jena.vocabulary.OWL;
import com.hp.hpl.jena.vocabulary.RDF;
import java.io.UnsupportedEncodingException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import org.apache.log4j.Logger;
import static com.xerox.xrce.xip2jena.ParsemGenericOntology.*;

/**
 *
 * @author nlagos
 */
public class MentionsMerger extends ModelMerger {

    private static Logger log = Logger.getLogger(MentionsMerger.class);

    public MentionsMerger (Model aDestModel, OntModel aOntology) {
        super(aDestModel,aOntology);
    }

    @Override
    Resource mapResource(Resource srcNode) throws EntityResolutionException {
        Resource result = null;
        List<Property> types = getTypes(srcNode);
        if (types.contains(Mention))  {
            result = getMention(srcNode);
        } else if (srcNode.hasProperty(hasMention)) {
            result = getEntity(srcNode);
        }

        if (result == null) {
            result = (Resource) srcNode.inModel(getDestModel());
        }

        return result;
    }


        /**
     * Return an entity in destModel that has a mention with
     * the given URL and end offset.
     * @param docUrl
     * @param end
     */
    Resource getEntity(URL docUrl, long end, List<Property> types) throws UnsupportedEncodingException {
        String mentionQueryString =
                "PREFIX parsem:<" + ParsemGenericOntology.NS + "> " +
                "SELECT ?indiv " +
                "WHERE {" +
                "    ?indiv parsem:hasMention ?mention . " +
                "    ?mention parsem:hasEnd \"" + end + "\"^^<http://www.w3.org/2001/XMLSchema#long>  " +
                "}";
        Query mentionquery = QueryFactory.create(mentionQueryString);
        QueryExecution mentionqe = QueryExecutionFactory.create(mentionquery, getDestModel());
        ResultSet results = mentionqe.execSelect();
        Resource indiv = null;
        entityLoop: while (results.hasNext()) {
            QuerySolution sol = results.nextSolution();
            Resource indiv2 = (Resource) sol.get("?indiv").as(Resource.class);
            String id;
            if (indiv2.isAnon()) {
                id = "_:" + indiv2.getId().getLabelString();
            } else {
                id = indiv2.getURI();
            }
            String typeQueryString =
                    "SELECT ?type " +
                    "WHERE {" +
                    "    <" + id + "> <" + RDF.type + "> ?type" +
                    "}";
            Query typeQuery = QueryFactory.create(typeQueryString);
            QueryExecution typeqe = QueryExecutionFactory.create(typeQuery,getDestModel());
            ResultSet typeResults = typeqe.execSelect();
            while (typeResults.hasNext()) {
                Property type = (Property) typeResults.nextSolution().get("?type").as(Property.class);
                for (Property compatType : types) {
                    if (getOntology().contains(type, OWL.disjointWith, compatType)) {
                        continue entityLoop;
                    }
                }
            }
            if (indiv == null) {
                indiv = indiv2;
            } else {
                if (!indiv.equals(indiv2)) {
                    log.warn("Multiple compatible entities mentioned at " + docUrl + ":" + end);
                }
            }
        }
        return indiv;
    }

    /**
     * Return an existing entity in destModel that has a mention with the same URL
     * and the same end offset as some mention of origEntity.  If none exists,
     * return null.
     * @param origEntity
     * @throws com.xerox.xrce.xip2jena.ModelMerger.EntityResolutionException
     */

    public Resource getEntity(Resource origEntity) throws EntityResolutionException {
        List<Resource> results = new ArrayList<Resource>();
        List<Property> types = getTypes(origEntity);
        try {
            StmtIterator stmts = origEntity.listProperties(hasMention);
            while (stmts.hasNext()) {
                Resource mention = stmts.nextStatement().getResource();
                String urlString = mention.getProperty(hasUrl).getString();
                long end = mention.getProperty(hasEnd).getLong();
                Resource result;
                result = getEntity(new URL(urlString), end, types);
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

    /**
     * Retrieve a mention in the destModel that covers the same
     * text as the argument origMention.
     * @param origMention
     * @return
     * @throws com.xerox.xrce.xip2jena.ModelMerger.EntityResolutionException
     */
    public Resource getMention(Resource origMention) throws EntityResolutionException {
        try {
            URL docUrl = new URL(origMention.getProperty(hasUrl).getString());
            long start = origMention.getProperty(hasStart).getLong();
            long end = origMention.getProperty(hasEnd).getLong();
            return getMention(null, docUrl, start, end);
        } catch (MalformedURLException ex) {
            throw new EntityResolutionException(ex);
        }
    }

    /**
     * Retrieve a mention at a known location. If entity is non-null,
     * restrict to a mention that refers to that entity.
     * @param entity
     * @param docUrl
     * @param start
     * @param end
     * @return
     */
    public Resource getMention(Resource entity, URL docUrl, long start, long end) {
        String queryString =
                "PREFIX parsem:<" + ParsemGenericOntology.NS + "> " +
                "SELECT ?mention " +
                "WHERE {" +
                (entity == null ? "" : "<" + entity + "> parsem:hasMention ?mention . ") +
                "    ?mention parsem:hasUrl \"" + docUrl + "\" . " +
                "    ?mention parsem:hasStart \"" + start + "\"^^<http://www.w3.org/2001/XMLSchema#long>  ." +
                "    ?mention parsem:hasEnd \"" + end + "\"^^<http://www.w3.org/2001/XMLSchema#long>  ." +
                "}";
        Query query = QueryFactory.create(queryString);
        QueryExecution qe = QueryExecutionFactory.create(query, getDestModel());
        ResultSet results = qe.execSelect();
        if (results.hasNext()) {
            QuerySolution sol = results.nextSolution();
            if (results.hasNext()) {
                log.warn("Multiple answers unexpected");
            }
            return (Resource) sol.get("?mention").as(Resource.class);
        } else {
            return null;
        }
    }

}
