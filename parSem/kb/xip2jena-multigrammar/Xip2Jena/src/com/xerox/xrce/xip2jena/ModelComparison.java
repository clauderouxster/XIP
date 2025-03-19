/*
 * Copyright Xerox 2009
 */

package com.xerox.xrce.xip2jena;

import com.hp.hpl.jena.rdf.model.Model;
import com.hp.hpl.jena.rdf.model.Property;
import com.hp.hpl.jena.rdf.model.RDFNode;
import com.hp.hpl.jena.rdf.model.Resource;
import com.hp.hpl.jena.rdf.model.Statement;
import com.hp.hpl.jena.rdf.model.StmtIterator;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.NoSuchElementException;
import static com.xerox.xrce.xip2jena.RdfDiff.getAllResources;

/**
 * This is part of RdfDiff.  It should be an inner class,
 * but silly java doesn't let static classes have inner classes.
 */
public class ModelComparison {

    public List<List<Resource>> unmatchedIndivs;
    public List<List<Statement>> unmatchedStatements;   

    {
        unmatchedIndivs = new ArrayList<List<Resource>>(2);
        unmatchedStatements = new ArrayList<List<Statement>>(2);
        for (int i = 0; i < 2; i++) {
            unmatchedIndivs.add(new ArrayList<Resource>());
            unmatchedStatements.add(new ArrayList<Statement>());
        }
    }

    ModelComparison() {};

    ModelComparison(Model m0, Model m1, Map<Resource,Resource> map01, Map<Resource,Resource> map10) {

        for (Resource r : getAllResources(m0)) {
            if (!map01.containsKey(r)) {
                unmatchedIndivs.get(0).add(r);
            }
        }
        for (Resource r : getAllResources(m1)) {
            if (!map10.containsKey(r)) {
                unmatchedIndivs.get(1).add(r);
            }
        }
        StmtIterator it = m0.listStatements();
        while (it.hasNext()) {
            Statement stmt = it.nextStatement();
            if (missing(m1, stmt, map01)) {
                unmatchedStatements.get(0).add(stmt);
            }
        }
        it = m1.listStatements();
        while (it.hasNext()) {
            Statement stmt = it.nextStatement();
            if (missing(m0, stmt, map10)) {
                unmatchedStatements.get(1).add(stmt);
            }
        }
        if (RdfDiff.log.isDebugEnabled()) {
            show();
        }

    }
    
    /**
     * Returns true iff the subject and object are both present in the model, but
     * the statement is not.
     */
    private static boolean missing(Model m, Statement stmt,
                                   Map<Resource,Resource> map)
                            {
        Resource s = stmt.getSubject();

        Resource news = map.get(s);
        if (news == null) {
            s = (Resource) s.inModel(m).as(Resource.class);
        } else {
            s = news;
        }
        if (!m.containsResource(s)) {
            return false;
        }

        Property p = (Property) stmt.getPredicate().inModel(m).as(Property.class);

        RDFNode o = stmt.getObject();
        Resource newo = map.get(o);
        if (newo == null) {
            o = o.inModel(m);
        } else {
            o = newo;
        }
        if (!m.containsResource(o)) {
            return false;
        }

        if (m.contains(s, p, o)) return false;
        return true;
    }

    public boolean isomorphic() {
        for (int i = 0; i < 2; i++) {
            if (!unmatchedIndivs.get(i).isEmpty()) {
                return false;
            }
            if (!unmatchedStatements.get(i).isEmpty()) {
                return false;
            }
        }
        return true;
    }

    private static String describeIndiv(Resource r) throws NoSuchElementException {
        StringBuilder result = new StringBuilder();
        StmtIterator stmtit = r.listProperties();
        while (stmtit.hasNext()) {
            result.append("    " + stmtit.nextStatement() + "\n");
        }
        stmtit = r.getModel().listStatements(null, null, r);
        while (stmtit.hasNext()) {
            result.append("    " + stmtit.nextStatement() + "\n");
        }
        return result.toString();
    }

    private void show() throws NoSuchElementException {
        if (isomorphic()) {
            System.out.println("isomorphic");
        }
        for (int i = 0; i < 2; i++) {
            if (!unmatchedIndivs.get(i).isEmpty()) {
                System.out.println("Individuals in m" + i + " only:");
                for (Resource r : unmatchedIndivs.get(i)) {
                    System.out.println(r);
                    System.out.println(describeIndiv(r));
                }
            }
            if (!unmatchedStatements.get(i).isEmpty()) {
                System.out.println("Statements in m" + i + " only");
                for (Statement s : unmatchedStatements.get(i)) {
                    System.out.println(s);
                }
            }
        }
    }

}

