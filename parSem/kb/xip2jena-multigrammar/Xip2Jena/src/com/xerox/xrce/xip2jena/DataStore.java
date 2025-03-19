/*
 * Copyright Xerox 2009
 */

package com.xerox.xrce.xip2jena;

import com.hp.hpl.jena.query.ResultSet;
import com.hp.hpl.jena.rdf.model.Model;
import java.io.OutputStream;
import java.sql.SQLException;

public interface DataStore {
    public void close();
    public Model getDefaultModel();
    public void dump(OutputStream str, String format);
    public ResultSet query(String queryString) throws SQLException;
    public Model queryDescribe(String queryString) throws SQLException;
    public Model queryDescribe(String queryString, Model model) throws SQLException;
    public void clean() throws SQLException;
}
