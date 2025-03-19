/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package xip2jena_risk;

import com.hp.hpl.jena.query.QuerySolution;
import com.hp.hpl.jena.query.ResultSet;
import com.xerox.xrce.xip2jena.DataStore;
import com.xerox.xrce.xip2jena.ParsemGenericOntology;

/**
 *
 * @author akaplan
 */
public class Query {
    public static void main(String[] args) throws Exception {
        DataStore store = StorageConfigRisk.connect();

        String queryString =
                "PREFIX ns:<" + ParsemGenericOntology.NS + "> " +
                "SELECT ?indiv ?mention " +
                "WHERE {" +
                "      ?indiv ns:hasMention ?mention ." +
                "      ?mention ns:hasUrl \"file:/home/akaplan/parSem/kb/xip2jena-aaron/Xip2Jena_risk/test/test \" . " +
                "      ?mention ns:hasEnd '4'^^<http://www.w3.org/2001/XMLSchema#long> . " +
                "}";
        System.out.println(queryString);
        ResultSet results = store.query(queryString);
        if (!results.hasNext()) {
            System.out.println("found nothing");
        }
        while (results.hasNext()) {
            QuerySolution sol = results.nextSolution();
            System.out.println(sol.get("?indiv") + " " + sol.get("?mention"));
        }
    }
}
