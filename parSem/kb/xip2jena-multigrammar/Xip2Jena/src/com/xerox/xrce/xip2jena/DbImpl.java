/*
 * Copyright Xerox 2009
 */

package com.xerox.xrce.xip2jena;

import com.hp.hpl.jena.query.Query;
import com.hp.hpl.jena.query.QueryExecution;
import com.hp.hpl.jena.query.QueryExecutionFactory;
import com.hp.hpl.jena.query.QueryFactory;
import com.hp.hpl.jena.query.ResultSet;
import com.hp.hpl.jena.rdf.model.Model;
import java.sql.SQLException;

public abstract class DbImpl implements DataStore {

    @Override
    public ResultSet query(String queryString) throws SQLException {
        Query query = QueryFactory.create(queryString);
        QueryExecution qe = QueryExecutionFactory.create(query, getDefaultModel());
        ResultSet results = qe.execSelect();
        qe.close();
        return results;
    }

    @Override
    public Model queryDescribe(String queryString) throws SQLException {
        return queryDescribe(queryString, null);
    }

    @Override
    public Model queryDescribe(String queryString, Model model) throws SQLException {
        Query query = QueryFactory.create(queryString);
        QueryExecution qe = QueryExecutionFactory.create(query, getDefaultModel());
        Model results;
        if (model == null) {
            results = qe.execDescribe();
        } else {
            results = qe.execDescribe(model);
        }
        qe.close();
        return results;
    }

}
