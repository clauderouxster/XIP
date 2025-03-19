/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 *
 * Overview.java
 *
 * Created on March 13, 2007, 10:25 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer;

import java.util.*;
import java.sql.*;

/**
 *
 * @author vgrassau
 */
public class Overview extends GoldminerDataBase {

    /** Creates a new instance of Overview
     * @param context
     * @throws java.lang.Exception 
     */
    public Overview(GoldminerContext context) throws Exception {
        super(context);
    }

    /**
     *Returns the number of concepts in the database
     *@return a number, 0 if no concept;
     */
    public int getCountConcept() {
        return countKey(Overview.id_concept_col, TABLE.concept);
    }

    /**
     *Returns the number of documents in the database
     *@return a number, 0 if no documents
     */
    public int getCountDocuments() {
        return countKey(Overview.id_document_col, TABLE.document);
    }

    /**
     *Counts the number of lignes of a column in a specifc table and return a new id for table
     *@param col 
     *@param table
     *@return id
     */
    private int countKey(String col, GoldminerDataBase.TABLE table) {
        getConnection();
        int result = 0;
        try {
            Statement s = connection.createStatement();
            ResultSet res = s.executeQuery("SELECT count(" + col + ") FROM " + table);
            while (res.next()) {
                result = res.getInt(1);
            }
            res.close();
            s.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return result;
    }

    /**
     *Returns the number of corpora in the database
     *@return a number, 0 if no corpus
     */
    public int getCountCorpora() {
        return countKey(Overview.id_corpus_col, TABLE.corpus);
    }

    /**
     *Returns the number of functions in the database
     *@return the number of function, 0 if no function
     */
    public int getCountFunction() {
        return countKey(Overview.id_function_col, TABLE.function);
    }

    /**
     *Returns the number of sentences in the database
     *@return a sentence, 0 if no sentence
     */
    public int getCountSentences() {
        return countKey(Overview.id_sent_col, TABLE.sentence);
    }

    /**
     *Returns the concept that have the most frequency in the database.
     *@param n the limit to get
     *@return a map with lemma and their occurency
     */
    public Map<String, Integer> getMostReferedConcepts(int n) {
        String sql = "SELECT lemma, occurence FROM concept ORDER BY occurence DESC LIMIT ?";
        Map<String, Integer> map = new HashMap();
        try {
            getConnection();
            PreparedStatement statement = connection.prepareStatement(sql);
            statement.setInt(1, n);
            ResultSet rs = statement.executeQuery();
            while (rs.next()) {
                map.put(rs.getString(lemma_col), rs.getInt(occurence_col));
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return map;
    }

    public Map<Integer, Integer> getOccurencyForBasicElement() {
        String sql = "SELECT id_function, occurence FROM function ORDER BY occurence DESC";
        Map<Integer, Integer> map = new HashMap();
        try {
            getConnection();
            Statement statement = connection.createStatement();
            ResultSet rs = statement.executeQuery(sql);
            while (rs.next()) {
                map.put(rs.getInt(id_function_col), rs.getInt(occurence_col));
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return map;
    }

    /**
     *Returns some information about a function
     *@param idFunction 
     * @return a Map of details at least the name of the function, the occurency, and the number of sentences that contains this function
     *
     */
    public Map getDetailsForFunction(int idFunction) {
        String sql = " SELECT f.name, f.occurence, count(distinct id_sent) " +
                " FROM (function as f) join dependency as d on f.id_function = d.id_function" +
                " WHERE f.id_function = ? GROUP BY  f.name,f.occurence";
        Map info = new HashMap();
        try {
            getConnection();
            PreparedStatement statement = connection.prepareStatement(sql);
            statement.setInt(1, idFunction);
            ResultSet rs = statement.executeQuery();
            if (rs.first()) {
                info.put("name", rs.getString(1));
                info.put("occurence", rs.getInt(2));
                info.put("sentences", rs.getInt(3));
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return info;
    }

    /**
     *Returns some lemma according to a function
     *@param idFunction the BasicElement to get some lemmas
     *@param limit the limit of the requets if the database is huge  don't use a big limit 200 it's quiet big
     *@return a map with the lemmas and their occurency
     */
    public Map<String, Integer> getLemmasFromFunction(int idFunction, int limit) {
        String sql = " SELECT DISTINCT c.lemma, c.occurence " +
                "   from (function_parameter as f ) left join (concept_parameter as cp) on f.id_param = cp.id_param left join concept as c on cp.id_concept = c.id_concept" +
                "  where f.id_function = ? LIMIT ?";
        Map<String, Integer> info = new HashMap();
        try {
            getConnection();
            PreparedStatement statement = connection.prepareStatement(sql);
            statement.setInt(1, idFunction);
            statement.setInt(2, limit);
            ResultSet rs = statement.executeQuery();
            while (rs.next()) {
                info.put(rs.getString(1), rs.getInt(2));
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return info;
    }
}

