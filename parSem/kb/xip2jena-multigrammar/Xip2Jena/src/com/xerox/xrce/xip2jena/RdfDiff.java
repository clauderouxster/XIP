/*
 * Copyright Xerox 2009
 */

package com.xerox.xrce.xip2jena;

import com.hp.hpl.jena.rdf.model.Model;
import com.hp.hpl.jena.rdf.model.ModelFactory;
import com.hp.hpl.jena.rdf.model.NodeIterator;
import com.hp.hpl.jena.rdf.model.Property;
import com.hp.hpl.jena.rdf.model.RDFNode;
import com.hp.hpl.jena.rdf.model.ResIterator;
import com.hp.hpl.jena.rdf.model.Resource;
import com.hp.hpl.jena.rdf.model.Statement;
import com.hp.hpl.jena.rdf.model.StmtIterator;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import static org.apache.log4j.Level.DEBUG;
import org.apache.log4j.Logger;
import static com.xerox.xrce.xip2jena.ParsemGenericOntology.*;

/**
 * Checks if two models are isomorphic, and if not, attempts to align isomorphic
 * subgraphs and returns information about the remaining differences.
 * 
 * Jena provides a model isomorphism check, but it's boolean.  If the graphs are
 * not identical, then it doesn't provide partial match information, so it's not
 * useful for identifying differences.  The algorithm used in this class is greedy
 * and deterministic, and therefore not guaranteed to find an isomorphism if one
 * exists.
 *
 * The jena algorithm is faster.  (As the accumulated special cases in the current
 * code get cruftier and cruftier, the Jena algorithm is also starting to look
 * cleaner.) It has a (private) subroutine called obligBindings()
 * which looks like it does just the deterministic part; might want to try
 * adapting that.
 *
 * The partialMatch algorithm is non-deterministic; there's a test case (currently disabled)
 * that sometimes succeeds and sometimes fails.  I imagine the nondeterminism
 * comes from the order of a hash table.  Use compareModels rather than partialMatch
 * in regression tests, because the jena algorithm should be deterministic. (It's
 * nondeterministic in that it might return different ismorphisms from one run to
 * the next, but deterministic in that if it says two graphs are isomorphic on one
 * run, it will say so on the next run too.
 */
public class RdfDiff {

    static Logger log = Logger.getLogger(RdfDiff.class);

    public static ModelComparison compareModels(Model m0, Model m1) throws RdfDiffException {

        if (m0.isIsomorphicWith(m1)) {
            log.debug("Isomorphic according to jena test");
            return new ModelComparison();
        }
        log.debug("Not isomorphic according to jena test");
        return partialMatch(m0, m1);
    }

    public static ModelComparison partialMatch(Model m0, Model m1) throws RdfDiffException {

        Map<Resource, Resource> map01 = new HashMap<Resource, Resource>();
        Map<Resource, Resource> map10 = new HashMap<Resource, Resource>();
        Map<Resource, Resource> newbindings = new HashMap<Resource, Resource>();
        
        // Bind identical literal nodes (and URI resources? not sure.)
        // todo: wtf?  getAllResources doesn't return literal nodes!
        Set<Resource> resources = getAllResources(m0);
        for (Resource r0 : resources) {
            if (m1.containsResource(r0)) {
                Resource r1 = (Resource) r0.inModel(m1).as(Resource.class);
                guardedPut(map01, map10, newbindings, r0, r1);
            }
        }

        // Bind Mentions with the same coordinates
        // No longer necessary as long as we stick to the same scheme for assigning
        // URLs to Mentions.  Keeping it for the time being because it allows
        // me to mess with the URL scheme.
        // NOTE: actually if I comment it out then it breaks, so it is necessary.
        // Why?  Probably because it's a combination of properties, not a single
        // property, that serves to identify a Mention.
        Map<String, Resource> mentionmap0 = mentionmap(m0);
        Map<String, Resource> mentionmap1 = mentionmap(m1);
        for (Map.Entry<String, Resource> binding : mentionmap0.entrySet()) {
            Resource mention0 = binding.getValue();
            Resource mention1 = mentionmap1.get(binding.getKey());
            if (mention1 != null) {
                guardedPut(map01, map10, newbindings, mention0, mention1);
            }
        }

        while (!newbindings.isEmpty()) {
            Map<Resource, Resource> newerbindings = new HashMap<Resource, Resource>();
            for (Map.Entry<Resource, Resource> binding : newbindings.entrySet()) {
                Resource anchor0 = binding.getKey();
                Resource anchor1 = binding.getValue();

                // Statements with the bound pair as subject, then as object
                for (boolean newAsSubject : new boolean[]{true, false}) {
                    StmtIterator stmts0 = m0.listStatements(newAsSubject ? anchor0 : null,
                                                null,
                                                newAsSubject ? (RDFNode) null : anchor0);
                    while (stmts0.hasNext()) {
                        Statement stmt0 = stmts0.next();
                        RDFNode candidateNode0 = newAsSubject ? stmt0.getObject() : stmt0.getSubject();
                        Property predicate = stmt0.getPredicate();
                        if (candidateNode0.isAnon()) {
                            Resource candidate0 = candidateNode0.as(Resource.class);
                            if (map01.containsKey(candidate0)) {
                                // The anchor is already bound.  Check if the new binding
                                // for the other (non-anchor) resource resolves an ambiguity for a property
                                // of the anchor.
                                Resource r0 = uniqueUnbound(candidate0, predicate, map01, !newAsSubject);
                                if (r0 != null) {
                                    Resource r1 = uniqueUnbound(map01.get(candidate0), predicate, map10, !newAsSubject);
                                    if (r1 != null) {
                                        guardedPut(map01, map10, newerbindings, r0, r1);
                                    }
                                }
                            } else {
                                // The anchor is not yet bound.  If there is no other
                                // statement in the model with the same predicate, same
                                // anchor in the same position, and an unbound resource in the
                                // other position, then this is a candidate for a new binding.
                                Resource r0 = uniqueUnbound(anchor0, predicate, map01, newAsSubject);
                                if (r0 != null) {
                                    Resource r1 = uniqueUnbound(anchor1, predicate, map10, newAsSubject);
                                    if (r1 != null) {
                                        guardedPut(map01, map10, newerbindings, r0, r1);
                                    }
                                }
                            }
                        }
                    }
                }

            }
            newbindings = newerbindings;
        }

        return new ModelComparison(m0, m1, map01, map10);
    }

    private static Resource uniqueUnbound(Resource anchor, Property p,
            Map<Resource,Resource> map, boolean anchorIsSubject) {
        Resource result = null;
        StmtIterator otherstmts = anchor.getModel().listStatements(
                anchorIsSubject ? anchor : null,
                p,
                anchorIsSubject ? null : anchor);
        while (otherstmts.hasNext()) {
            Statement stmt = otherstmts.next();
            RDFNode otherNode = anchorIsSubject ? stmt.getObject() : stmt.getSubject();
            if (otherNode.isAnon() && !map.containsKey(otherNode.as(Resource.class))) {
                if (result != null) {
                    // ambiguous
                    result = null;
                    break;
                }
                result = otherNode.as(Resource.class);
            }
        }
        return result;
    }

    static Set<Resource> getAllResources(Model m) {
        ResIterator ri = m.listSubjects();
        Set<Resource> result = new HashSet<Resource>();
        while (ri.hasNext()) {
            result.add(ri.nextResource());
        }
        NodeIterator ni = m.listObjects();
        while (ni.hasNext()) {
            RDFNode n = ni.nextNode();
            if (n.canAs(Resource.class)) {
                result.add((Resource) n.as(Resource.class));
            }
        }
        return result;
    }

    private static void guardedPut(Map<Resource, Resource> oldbindings01,
            Map<Resource,Resource> oldbindings10,
            Map<Resource, Resource> newbindings,
            Resource indiv1, Resource indiv2) {
        if (!oldbindings01.containsKey(indiv1)) {
            newbindings.put(indiv1, indiv2);
            oldbindings01.put(indiv1, indiv2);
            oldbindings10.put(indiv2, indiv1);
        }
        // Used to handle collision as an error, but actually it can
        // occur legitimately, when a single entity in one model corresponds to
        // two distinct ones in the other.
    }

    
    private static Map<String,Resource> mentionmap(Model m) throws RdfDiffException {
        Map<String, Resource> map = new HashMap<String, Resource>();
        ResIterator it = m.listResourcesWithProperty(m.getProperty("http://www.w3.org/1999/02/22-rdf-syntax-ns#type"),
                Mention);
        while (it.hasNext()) {
            Resource r = it.nextResource();
            String key = r.getProperty(hasUrl).getString() + ":" +
                    r.getProperty(hasStart).getInt() + ":" +
                    r.getProperty(hasEnd).getInt();
            Resource mention = map.get(key);
            if (mention != null) {
                throw new RdfDiffException("Duplicate mentions: " + key);
            }
            map.put(key,r);
        }
        return map;
    }

    public static void main(String[] args) throws Exception {
        if (args.length != 3) {
            System.out.println("Usage: " + RdfDiff.class.getName() + " <file0> <file1> [RDF/XML|N-TRIPLE|TTL|N3]");
            System.exit(1);
        }
        log.setLevel(DEBUG);
        compareModels(args[0], args[1], args[2]);
    }

    public static ModelComparison compareModels(String filename0, String filename1,
            String format)
            throws Exception {
        Model m0 = ModelFactory.createDefaultModel();
        Model m1 = ModelFactory.createDefaultModel();
        m0.read("file:" + filename0, format);
        m1.read("file:" + filename1, format);
        return compareModels(m0, m1);
    }

}

class RdfDiffException extends Exception {
    RdfDiffException(String msg) { super(msg); }
}
