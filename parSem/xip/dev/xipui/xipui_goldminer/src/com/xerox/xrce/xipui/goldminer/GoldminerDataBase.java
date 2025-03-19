/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 *
 * GoldminerDataBase.java
 *
 * Created on September 25, 2006, 3:17 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer;

import com.xerox.xrce.xipui.Offset;
import com.xerox.xrce.xipui.modules.IModule;
import java.util.*;
import java.io.*;
import org.apache.log4j.*;
import javax.sql.*;
import java.sql.*;
import org.apache.commons.dbcp.*;
import com.sun.rowset.*;

/**
 * Class for reading and store information for goldminer
 * this class should be instanciate by the GoldminerEngine object
 * @author vgrassau
 */
public class GoldminerDataBase {

    public static final String id_param_col = "id_param";
    public static final String id_param_ant_col = "id_param_ant";
    public static final String id_param_coref_col = "id_param_coref";
    public static final String id_concept_col = "id_concept";
    public static final String id_sent_col = "id_sent";
    public static final String id_function_col = "id_function";
    public static final String id_dep_col = "id_dep";
    public static final String id_corpus_col = "id_corpus";
    public static final String id_document_col = "id_document";
    public static final String occurence_col = "occurence";
    public static final String begin_col = "begin";
    public static final String end_col = "end";
    public static final String concepts_col = "concepts";
    public static final String lemma_col = "lemma";
    public static final String name_col = "name";
    public static final String arity_col = "arity";
    public static final String path_col = "path";
    public static final String id_surface_col = "id_surface";

    //the table created
    protected enum TABLE {

        parameter, concept, dependency, sentence, function, document, corpus, concept_parameter,
        dependency_parameter, function_parameter, conceptsurface, coreference_parameter
    }
    public static final String[] parameter_def = {id_param_col, occurence_col, begin_col, end_col};
    public static final String[] concept_def = {id_concept_col, lemma_col, occurence_col};
    public static final String[] concept_parameter_def = {id_param_col, id_concept_col, id_sent_col, occurence_col};
    public static final String[] concept_surface_def = {id_surface_col, id_concept_col, id_sent_col, begin_col, end_col};
    public static final String[] dependency_def = {id_dep_col, id_sent_col, id_function_col, concepts_col, arity_col};
    public static final String[] dependency_parameter_def = {id_dep_col, id_param_col, id_function_col, occurence_col};
    public static final String[] function_def = {id_function_col, name_col, occurence_col};
    public static final String[] corpus_def = {id_corpus_col, path_col};
    public static final String[] document_def = {id_document_col, id_corpus_col, name_col};
    public static final String[] sentence_def = {id_sent_col, id_document_col, begin_col, end_col};
    public static final String[] function_parameter_def = {id_param_col, id_function_col, id_sent_col, occurence_col};
    public static final String[] coreference_parameter_def = {id_param_coref_col, id_param_ant_col, id_sent_col};
    public static final int LIMIT_PREPARE = 50;
    private static final int cache_time_limit = 500;
    private static final int cacheSize = 5;
    //the prepared sql command for each table
    private Hashtable<TABLE, Vector<Object[]>> sqlTable;
    // the sql command to execute
    private Vector<String> sql;
    private GoldminerContext context = null;
    private Logger log = Logger.getLogger("debug");
    private DataSource dataBase;
    protected Connection connection;
    private String URL = null;
    private Hashtable<Object, RowSet> queryCache;
    private Hashtable<Object, Integer> queryFrequency;

    private synchronized RowSet getQueryCached(Object identifier) {
        Integer freq = queryFrequency.get(identifier);
        if (freq != null) {
            int newFreq = freq.intValue() + 1;
            log.trace("new frequence for : " + identifier + " , " + newFreq);
            queryFrequency.put(identifier, newFreq);
        }
        return queryCache.get(identifier);
    }

    /**
     * launch the sql request to store all the info in the DB
     * This is the main function which consume most of the time 
     */
    public void store() {
        getConnection();
        try {
            connection.setAutoCommit(false);
            prepareBatch();
            Statement statement = connection.createStatement();
            Iterator<String> iteSQL = sql.iterator();
            while (iteSQL.hasNext()) {
                //String sqlReq = iteSQL.next(); 
                //System.out.println(sqlReq);
                //statement.addBatch(sqlReq);
                statement.addBatch(iteSQL.next());
            }
            statement.executeBatch();
            connection.commit();
            connection.setAutoCommit(false);
            statement.clearBatch();
            statement.close();
        } catch (SQLException e) {
            e.printStackTrace();
            System.out.println("SQL STATE :" + e.getSQLState());
        }
        clear();
    }

    /** Put the result of query in a cache
     * @param identifier the identifeir of the query
     * @param rowset the rowset to cache.
     *
     */
    private synchronized void putQueryCached(int identifier, RowSet rowset) {
        if (queryCache.size() > cacheSize) {
            //on enleve la requete la moins utilisee
            log.trace("cache is Full");
            Collection frequencies = queryFrequency.keySet();
            Iterator ite = frequencies.iterator();
            int index = 0;
            int min = -1;
            Object minIdentifier = null;
            while (ite.hasNext()) {
                Object _identifier = ite.next();
                int frequency = queryFrequency.get(_identifier);
                if (index == 0) {
                    min = frequency;
                    minIdentifier = _identifier;
                } else {
                    if (frequency < min && frequency != min) {
                        min = frequency;
                        minIdentifier = _identifier;
                    }
                }
                index++;
            }
            log.trace("remove from cache the less used object : " + minIdentifier);
            queryFrequency.remove(minIdentifier);
            RowSet rowsetRemove = queryCache.remove(minIdentifier);
            try {
                rowsetRemove.close();
            } catch (Exception e) {
            }
        }
        int cpt = 1;
        log.trace("Add new Object in the cache : " + identifier);
        queryFrequency.put(identifier, cpt);
        queryCache.put(identifier, rowset);
    }

    /**
     * Creates a new instance of GoldminerDataBase
     * @param context 
     * @throws java.lang.Exception
     */
    public GoldminerDataBase(GoldminerContext context) throws Exception {
        try {
            this.context = context;
            queryCache = new Hashtable<Object, RowSet>();
            queryFrequency = new Hashtable<Object, Integer>();
            ///for the storage :
            sql = new Vector<String>();
            sqlTable = new Hashtable<TABLE, Vector<Object[]>>(12);
            sqlTable.put(TABLE.concept, new Vector<Object[]>(LIMIT_PREPARE, 1));
            sqlTable.put(TABLE.concept_parameter, new Vector<Object[]>(LIMIT_PREPARE, 1));
            sqlTable.put(TABLE.conceptsurface, new Vector<Object[]>(LIMIT_PREPARE, 1));
            sqlTable.put(TABLE.coreference_parameter, new Vector<Object[]>(LIMIT_PREPARE, 1));
            sqlTable.put(TABLE.corpus, new Vector<Object[]>(LIMIT_PREPARE, 1));
            sqlTable.put(TABLE.dependency, new Vector<Object[]>(LIMIT_PREPARE, 1));
            sqlTable.put(TABLE.dependency_parameter, new Vector<Object[]>(LIMIT_PREPARE, 1));
            sqlTable.put(TABLE.document, new Vector<Object[]>(LIMIT_PREPARE, 1));
            sqlTable.put(TABLE.function, new Vector<Object[]>(LIMIT_PREPARE, 1));
            sqlTable.put(TABLE.function_parameter, new Vector<Object[]>(LIMIT_PREPARE, 1));
            sqlTable.put(TABLE.parameter, new Vector<Object[]>(LIMIT_PREPARE, 1));
            sqlTable.put(TABLE.sentence, new Vector<Object[]>(LIMIT_PREPARE, 1));
            BasicDataSource dSource = new BasicDataSource();
            dSource.setDriverClassName("com.mysql.jdbc.Driver");
            URL = "jdbc:mysql://" + context.getHost() + ":" + context.getPort() + "/" + context.getDatabase();
            dSource.setUrl(URL);
            dSource.setUsername(context.getUser());
            dSource.setPassword(context.getPassword());
            dSource.getConnection().close();
            dataBase = dSource;
        } catch (Exception e) {
            log.error("ERROR CONNECTION TO DB", e);
            log.error(URL + " " + context.getUser() + " " + context.getPassword());
            throw e;
        }
    }

    public boolean checkConnection() {
        try {
            BasicDataSource dSource = new BasicDataSource();
            dSource.setDriverClassName("com.mysql.jdbc.Driver");
            URL = "jdbc:mysql://" + context.getHost() + ":" + context.getPort() + "/" + context.getDatabase();
            dSource.setUrl(URL);
            dSource.setUsername(context.getUser());
            dSource.setPassword(context.getPassword());
            dSource.getConnection().close();
            return true;
        } catch (Exception e) {
            log.warn(e);
            log.warn(URL + " " + context.getUser() + " " + context.getPassword());
            return false;
        }
    }

    public boolean isConnected() {
        Connection con = getConnection();
        return con != null;
    }

    /**
     *Return the SQL connection to the database. If the current connection
     *is closed or null, try to create a new connection
     *@return the connection or null if connection failed.
     */
    public Connection getConnection() {
        try {
            if (connection == null || connection.isClosed()) {
                connection = dataBase.getConnection();
            }
        } catch (Exception e) {
            log.error("ERROR DB: no connection  ", e);
            connection = null;
        }
        return connection;
    }

    /**
     *Close the connection
     */
    public void closeConnection() {
        try {
            if (this.connection != null && !connection.isClosed()) {
                connection.close();
            }
        } catch (Exception e) {
            connection = null;
            log.error("ERROR DB : ", e);
        }
    }

    /**
     *Get the available functions stored in the database.
     *@return true if succeed false otherwise
     *
     */
    public boolean initFunctions() {
        try {
            //we get the connection or re-create it if it was lost.
            getConnection();
            Map<Integer, String> mapFunction = new HashMap<Integer, String>();
            PreparedStatement query = connection.prepareStatement("SELECT SQL_CACHE DISTINCT  id_function, name FROM function ORDER BY name");
            ResultSet rs = query.executeQuery();
            while (rs.next()) {
                mapFunction.put(rs.getInt("id_function"), rs.getString("name"));
            }
            rs.close();
            context.setProperty(GoldminerContext.MAP_CONCEPT, mapFunction);
            return true;
        } catch (Exception e) {
            log.error("ERROR DB", e);
        }
        return false;
    }

    /**
     *Put in the context of goldminer a map with idconcept and the word for the concept
     *the key to use is MapConcept
     *@param pos the part of speech to use
     *@return true if the request succeed false otherwise
     *@deprecated
     */
    public boolean getMapConceptOfPOS(int pos) {
        try {
            getConnection();
            Map<Integer, String> mapConcept = new HashMap<Integer, String>();
            PreparedStatement query = connection.prepareStatement("SELECT SQL_CACHE DISTINCT word, idconcept FROM concepts WHERE pos = ? ORDER BY word");
            query.setInt(1, pos);
            ResultSet rs = query.executeQuery();
            while (rs.next()) {
                mapConcept.put(rs.getInt("idconcept"), rs.getString("word"));
            }
            rs.close();
            context.setProperty(GoldminerContext.MAP_CONCEPT, mapConcept);
            //connection.close();
            return true;
        } catch (SQLException e) {
            log.error("ERROR DB", e);
            return false;
        } catch (NullPointerException e) {
            log.error("ERROR DB", e);
            return false;
        }
    }

    public boolean getListOfSource() {
        Map<Integer, String> res = new HashMap<Integer, String>();
        try {
            getConnection();
            String SQL = "SELECT SQL_CACHE id_document,path,name FROM document as d join corpus as c on d.id_corpus = c.id_corpus";
            Statement statement = connection.createStatement();
            ResultSet rs = statement.executeQuery(SQL);
            while (rs.next()) {
                String path = rs.getString("path") + File.separator + rs.getString("name");
                res.put(rs.getInt("id_document"), path);
            }
            rs.close();
            context.setProperty(GoldminerContext.LIST_SOURCE, res);
            return true;
        } catch (Exception e) {
            log.error("Error ", e);
            return false;
        }
    }

    public static boolean clearDataBase(Connection connection) {
        try {
            Statement statement = connection.createStatement();
            connection.setAutoCommit(false);
            IModule.DEBUG_LOG.info("Erase all the tables");
            statement.addBatch("DROP TABLE corpus");
            statement.addBatch("DROP TABLE document;");
            statement.addBatch("DROP TABLE sentence");
            statement.addBatch("DROP TABLE concept");
            statement.addBatch("DROP TABLE conceptsurface");
            statement.addBatch("DROP TABLE dependency");
            statement.addBatch("DROP TABLE parameter");
            statement.addBatch("DROP TABLE concept_parameter");
            statement.addBatch("DROP TABLE dependency_parameter");
            statement.addBatch("DROP TABLE coreference_parameter");
            statement.addBatch("DROP TABLE function");
            statement.addBatch("DROP TABLE function_parameter");
            IModule.DEBUG_LOG.info("Create tables for goldminer");
            statement.addBatch("CREATE TABLE concept (id_concept  INT UNSIGNED NOT NULL,lemma VARCHAR(100)  NOT NULL, occurence INT UNSIGNED DEFAULT 1, PRIMARY KEY (id_concept), INDEX lemma (lemma(16)))");
            statement.addBatch("CREATE TABLE function (id_function INT UNSIGNED NOT NULL,name VARCHAR(100) NOT NULL,occurence INT DEFAULT 1,PRIMARY KEY (id_function), INDEX name_function (name(16)))");
            statement.addBatch("CREATE TABLE corpus (id_corpus INT UNSIGNED  NOT NULL,path VARCHAR(255) NOT NULL,PRIMARY KEY (id_corpus ))");
            statement.addBatch("CREATE TABLE document (id_document INT UNSIGNED NOT NULL,id_corpus INT UNSIGNED NOT NULL,name VARCHAR(100) NOT NULL,PRIMARY KEY(id_document),INDEX name_document (name))");
            statement.addBatch("CREATE TABLE sentence (id_sent INT UNSIGNED NOT NULL,id_document INT UNSIGNED NOT NULL,begin  INT  UNSIGNED DEFAULT 0,end INT  UNSIGNED   DEFAULT 0,PRIMARY KEY(id_sent))");
            statement.addBatch("CREATE TABLE dependency (id_dep CHAR(16) NOT NULL,id_sent INT UNSIGNED  NOT NULL,id_function INT UNSIGNED   NOT NULL,arity SMALLINT(2)  UNSIGNED   DEFAULT 1,concepts  TEXT NOT NULL, PRIMARY KEY(id_dep),INDEX dep_sent(id_sent),INDEX dep_function(id_function))");
            statement.addBatch("CREATE TABLE parameter (id_param  CHAR(16) NOT NULL,occurence INT UNSIGNED  DEFAULT 0,begin INT UNSIGNED  DEFAULT 0,end INT UNSIGNED  DEFAULT 0, PRIMARY KEY(id_param))");
            statement.addBatch("CREATE TABLE dependency_parameter (id_dep CHAR(16) NOT NULL,id_param CHAR(16) NOT NULL,id_function INT(4) UNSIGNED NOT NULL,occurence INT UNSIGNED DEFAULT 1,PRIMARY KEY(id_dep,id_param), INDEX dep_param(id_param))");
            statement.addBatch("CREATE TABLE concept_parameter (id_param CHAR(16) NOT NULL,id_concept INT NOT NULL,id_sent INT UNSIGNED  NOT NULL,occurence INT UNSIGNED DEFAULT 1,PRIMARY KEY(id_param,id_concept), INDEX concept_param (id_concept))");
            statement.addBatch("CREATE TABLE function_parameter (id_param CHAR(16) NOT NULL,id_function INT(4) UNSIGNED NOT NULL,id_sent INT UNSIGNED  NOT NULL,occurence INT UNSIGNED DEFAULT 1,PRIMARY KEY(id_param,id_function), INDEX function_param (id_function))");
            statement.addBatch("CREATE TABLE conceptsurface (id_surface  CHAR(16) NOT NULL,id_concept  INT UNSIGNED  NOT NULL,id_sent INT UNSIGNED  NOT NULL,begin  INT  UNSIGNED  DEFAULT 0,end INT UNSIGNED  DEFAULT 0,PRIMARY KEY(id_surface),INDEX concept_surf (id_concept))");
            statement.addBatch("CREATE TABLE coreference_parameter (id_param_coref   CHAR(16) NOT NULL,id_param_ant CHAR(16) NOT NULL,id_sent INT UNSIGNED  NOT NULL,PRIMARY KEY(id_param_coref))");
            int[] res = statement.executeBatch();
            // on teste pas.... :-(
            connection.commit();
            connection.setAutoCommit(false);
            return true;
        } catch (Exception e) {
            IModule.DEBUG_LOG.error("ERROR", e);
            return false;
        }
    }

    /**
     *Prend une collection la parcours si elle est non vide.
     *et, renvoie la liste sous la forme d'un String : (e1,e2,e3,...,en)
     * @param c
     * @return 
     */
    private String joinList(Collection c) {
        StringBuilder joined;
        if (!c.isEmpty()) {
            int size = c.size();
            int cpt = 0;
            Iterator ite = c.iterator();
            joined = new StringBuilder("(");
            while (ite.hasNext()) {
                Object o = ite.next();
                if (o instanceof String) {
                    joined.append("'" + o + "'");
                } else {
                    joined.append(o);
                }
                if (cpt >= 0 && cpt < size - 1) {
                    joined.append(",");
                }
                cpt++;
            }
            joined.append(")");
            return joined.toString();
        } else {
            return null;
        }
    }

    public synchronized boolean getMapOfOffset(Collection<Sentence> sentences) {
        Map<String, Offset> offsetParam = new HashMap<String, Offset>();
        Map<Integer, Offset> offsetSentence = new HashMap<Integer, Offset>();
        try {
            Vector params = new Vector();
            Vector sentences_id = new Vector();
            Iterator<Sentence> iteSent = sentences.iterator();
            while (iteSent.hasNext()) {
                Sentence sentence = iteSent.next();
                sentences_id.add(sentence.getIdSent());
                params.addAll(sentence.getAllIdParameter());
            }
            String SQL_sentence = "SELECT SQL_CACHE id_sent, begin, end FROM sentence WHERE id_sent in  " + joinList(sentences_id);
            PreparedStatement statement = connection.prepareStatement(SQL_sentence);
            ResultSet rs = statement.executeQuery();
            while (rs.next()) {
                Offset off = new Offset(rs.getInt("begin"), rs.getInt("end"));
                int idsent = rs.getInt("id_sent");
                offsetSentence.put(idsent, off);
            }
            rs.close();
            String SQL_param = "SELECT SQL_CACHE id_param, begin, end FROM parameter WHERE id_param in  " + joinList(params);
            PreparedStatement statement2 = connection.prepareStatement(SQL_param);
            ResultSet rs2 = statement2.executeQuery();
            while (rs2.next()) {
                Offset offset = new Offset(rs2.getInt("begin"), rs2.getInt("end"));
                String idParam = rs2.getString("id_param");
                offsetParam.put(idParam, offset);
            }
            rs2.close();
            context.setProperty(GoldminerContext.OFFSET_SENTENCES, offsetSentence);
            context.setProperty(GoldminerContext.OFFSET_MAP, offsetParam);
            return true;
        } catch (Exception e) {
            log.error("ERROR : ", e);
            return false;
        }
    }

    private synchronized boolean searchFunction(int id_function, boolean add) {
        try {
            //cette requete permet de recuperer les dependances et phrase ou le concept apparait
            String SQL = "SELECT  SQL_NO_CACHE dp.id_dep,dp.id_param, doc.id_document " + " FROM  sentence as doc,function_parameter as fp use index (function_param) " + " RIGHT JOIN (dependency_parameter as dp use index (dep_param))  " + " ON  ( fp.id_param = dp.id_param)  WHERE   fp.id_function = ?  and doc.id_sent = fp.id_sent";
            long start = System.currentTimeMillis();
            RowSet rowset = getQueryCached(id_function);
            long start2 = System.currentTimeMillis();
            if (rowset == null) {
                rowset = new JdbcRowSetImpl();
                rowset.setCommand(SQL);
                rowset.setInt(1, id_function);
                rowset.setUrl(URL);
                rowset.setPassword(context.getPassword());
                rowset.setUsername(context.getUser());
                start = System.currentTimeMillis();
                rowset.execute();
                start2 = System.currentTimeMillis();
                if ((start2 - start) > cache_time_limit) {
                    putQueryCached(id_function, rowset);
                }
            }
            int iddep = -1;
            int idsent = -1;
            Vector<Integer> conceptForIddep = new Vector<Integer>();
            rowset.beforeFirst();
            while (rowset.next()) {
                String id_dependency = rowset.getString("id_dep");
                String id_parameter = rowset.getString("id_param");
                int id_document = rowset.getInt("id_document");
                //on change de dependance, on regarde si on accepte ou pas la dependance trouvee
                // en fonction du concept, et des BasicElements possible
                if (add) {
                    context.addEntryForMapQueries(id_document, id_dependency, id_parameter, id_function, id_function);
                } else {
                    context.removeEntryForMapQueries(id_document, id_dependency, id_parameter, id_function, id_function);
                }
            }
            long end = System.currentTimeMillis();
            System.out.println("Lecture resultat pour : " + id_function + " en " + (end - start2) + " ms");
            System.out.println("temps de requete : " + id_function + " en " + (start2 - start) + " ms");
            return true;
        } catch (Exception e) {
            log.error("ERROR DATABASE:", e);
            return false;
        }
    }

    private synchronized boolean searchConcept(int idConcept, boolean add) {
        Vector<Integer> typeElements = null;
        if (add) {
            typeElements = new Vector<Integer>();
        }
        try {
            //cette requete permet de recuperer les dependances et phrase ou le concept apparait
            String SQL = " SELECT dp.id_function,dp.id_dep,dp.id_param, doc.id_document " + " FROM sentence as doc,concept_parameter as cp use index (concept_param) " + " STRAIGHT_JOIN (dependency_parameter as dp use index (dep_param)) " + " ON   ( cp.id_param = dp.id_param) " + " WHERE   cp.id_concept = ?  and doc.id_sent = cp.id_sent";
            long start = System.currentTimeMillis();
            RowSet rowset = getQueryCached(idConcept);
            long start2 = System.currentTimeMillis();
            if (rowset == null) {
                rowset = new JdbcRowSetImpl();
                rowset.setCommand(SQL);
                rowset.setInt(1, idConcept);
                rowset.setUrl(URL);
                rowset.setPassword(context.getPassword());
                rowset.setUsername(context.getUser());
                start = System.currentTimeMillis();
                rowset.execute();
                start2 = System.currentTimeMillis();
                if ((start2 - start) > cache_time_limit) {
                    putQueryCached(idConcept, rowset);
                }
            }
            int iddep = -1;
            int idsent = -1;
            Vector<Integer> conceptForIddep = new Vector<Integer>();
            rowset.beforeFirst();
            while (rowset.next()) {
                String id_dependency = rowset.getString("id_dep");
                String id_parameter = rowset.getString("id_param");
                int id_document = rowset.getInt("id_document");
                int id_function = rowset.getInt("id_function");
                //on change de dependance, on regarde si on accepte ou pas la dependance trouvee
                // en fonction du concept, et des BasicElements possible
                if (typeElements == null) {
                    context.removeEntryForMapQueries(id_document, id_dependency, id_parameter, idConcept, id_function);
                } else {
                    if (context.getTypesList().containsKey(id_function)) {
                        if (!typeElements.contains(id_function)) {
                            typeElements.add(id_function);
                        }
                    }
                    context.addEntryForMapQueries(id_document, id_dependency, id_parameter, idConcept, id_function);
                }
            }
            long end = System.currentTimeMillis();
            System.out.println("Lecture resultat pour : " + idConcept + " en " + (end - start2) + " ms");
            System.out.println("temps de requete : " + idConcept + " en " + (start2 - start) + " ms");
            if (add) {
                context.setProperty(idConcept + GoldminerContext.LEMMA_TYPES, typeElements);
            }
            return true;
        } catch (Exception e) {
            log.error("ERROR DATABASE:", e);
            return false;
        }
    }

    public synchronized boolean removeConcept(int idConcept) {
        return searchConcept(idConcept, false);
    }

    public synchronized boolean addConcept(int idConcept) {
        return searchConcept(idConcept, true);
    }

    public synchronized boolean addFunction(int id_function) {
        return searchFunction(id_function, true);
    }

    public synchronized boolean removeFunction(int id_function) {
        return searchFunction(id_function, false);
    }

    /**
     *Cherche des informations sur mot donne
     *
     * @param name
     * @return 
     */
    public int getFunctionInformation(String name) {
        int idFunction = -1;
        try {
            getConnection();
            String SQL = "SELECT SQL_CACHE id_function FROM function WHERE name = ? ";
            PreparedStatement statement = connection.prepareStatement(SQL);
            statement.setString(1, name);
            ResultSet rs = statement.executeQuery();
            while (rs.next()) {
                idFunction = rs.getInt("id_function");
            }
            rs.close();
        } catch (Exception e) {
            log.error("ERROR : ", e);
            return -1;
        }
        return idFunction;
    }

    /**
     *Cherche des informations sur mot donne
     *
     * @param lemma
     * @return 
     */
    public int getConceptInformation(String lemma) {
        int idConcept = -1;
        try {
            getConnection();
            String SQL = "SELECT SQL_CACHE id_concept FROM concept WHERE lemma = ?";
            PreparedStatement statement = connection.prepareStatement(SQL);
            statement.setString(1, lemma);
            ResultSet rs = statement.executeQuery();
            while (rs.next()) {
                idConcept = rs.getInt("id_concept");
            }
            rs.close();
        } catch (Exception e) {
            log.error("ERROR : ", e);
            return -1;
        }
        return idConcept;
    }

    /*-----------------------------------------------------------------------------------------------*/
    /*--------------------- functions for storage ---------------------------------------------------*/
    /**
     *Prepare SQL command update for a table.
     *for performance the SQL command are like this :
     *INSERT INTO table (columns) VALUES (values[0]) , (values[1)
     *values[0] = (a,b,c,...d)
     *The SQL command are not executed but they are stored in a vector to be executed in
     *a batch command by a statement
     *@param table the table to update
     *
     */
    private void prepareTable(TABLE table) {
        SQLBuilder sqlBuilder = null;
        Vector<Object[]> values = sqlTable.get(table);
        switch (table) {
            default: {
                break;
            }
            case parameter: {
                sqlBuilder = new SQLBuilder(TABLE.parameter, parameter_def, values);
                sqlBuilder.append(" ON DUPLICATE KEY UPDATE occurence=occurence+1");
                addSQL(sqlBuilder);
                break;
            }
            case concept: {
                sqlBuilder = new SQLBuilder(TABLE.concept, concept_def, values);
                sqlBuilder.append(" ON DUPLICATE KEY UPDATE occurence=occurence+1");
                addSQL(sqlBuilder);
                break;
            }
            case function: {
                sqlBuilder = new SQLBuilder(TABLE.function, function_def, values);
                sqlBuilder.append(" ON DUPLICATE KEY UPDATE occurence=occurence+1");
                addSQL(sqlBuilder);
                break;
            }
            case dependency: {
                addSQL(new SQLBuilder(TABLE.dependency, dependency_def, values));
                break;
            }
            case sentence: {
                addSQL(new SQLBuilder(TABLE.sentence, sentence_def, values));
                break;
            }
            case corpus: {
                addSQL(new SQLBuilder(TABLE.corpus, corpus_def, values));
                break;
            }
            case document: {
                addSQL(new SQLBuilder(TABLE.document, document_def, values));
                break;
            }
            case concept_parameter: {
                sqlBuilder = new SQLBuilder(TABLE.concept_parameter, concept_parameter_def, values);
                sqlBuilder.append(" ON DUPLICATE KEY UPDATE occurence=occurence+1");
                addSQL(sqlBuilder);
                break;
            }
            case function_parameter: {
                sqlBuilder = new SQLBuilder(TABLE.function_parameter, function_parameter_def, values);
                sqlBuilder.append(" ON DUPLICATE KEY UPDATE occurence=occurence+1");
                addSQL(sqlBuilder);

                break;
            }
            case dependency_parameter: {
                sqlBuilder = new SQLBuilder(TABLE.dependency_parameter, dependency_parameter_def, values);
                sqlBuilder.append(" ON DUPLICATE KEY UPDATE occurence=occurence+1");
                addSQL(sqlBuilder);
                break;
            }
            case coreference_parameter: {
                // addSQL(new SQLBuilder(TABLE.coreference_parameter,coreference_parameter_def,values));
                break;
            }
            case conceptsurface: {
                //  addSQL(new SQLBuilder(TABLE.conceptsurface,concept_surface_def,values));
                break;
            }
        }
        if (values != null) {
            values.clear();
        }
    }

    /**
     *Add a new function_parameter in the database
     *@param id_param the parameter
     *@param id_function the function
     *@param id_sent
     */
    public void addFunctionParameter(String id_param, int id_function, long id_sent) {
        Object[] values = {"'" + id_param + "'", id_function, id_sent, 1};
        addRowValue(TABLE.function_parameter, values);
    }

    /**
     *Add a coreference_parameter in the database
     *@param id_param_coref the coreferent parameter
     *@param id_param_ant 
     * @param id_sent the id of the sentence
     */
    public void addCoreferenceParameter(String id_param_coref, String id_param_ant, long id_sent) {
        Object[] values = {"'" + id_param_coref + "'", "'" + id_param_ant + "'", id_sent};
        addRowValue(TABLE.coreference_parameter, values);
    }

    /**
     *Add a row for a table
     *@param table the table to update
     *@param value the row value to insert
     *
     *this method check if we need ti prepare the SQL command for the table
     */
    private void addRowValue(TABLE table, Object[] value) {
        Vector<Object[]> values = sqlTable.get(table);
        values.add(value);
        if (needToPrepare(table)) {
            this.prepareTable(table);
        }
    }

    /**
     *Check if we need to prerare the SQL command because they too much
     *values for the table
     *@param table the table to check
     *@return true if they are more values than the LIMIT_PREPARE value
     *
     */
    private boolean needToPrepare(TABLE table) {
        Collection c = sqlTable.get(table);
        return c.size() > LIMIT_PREPARE;
    }

    /**
     *clear this object
     *
     */
    public void clear() {
        sql.clear();
        Collection<Vector<Object[]>> c = sqlTable.values();
        Iterator<Vector<Object[]>> iteC = c.iterator();
        while (iteC.hasNext()) {
            iteC.next().clear();
        }
    }

    /**
     *Add a new SQL Command created by a SQLBuilder
     *if the number of SQL command is greater than the LIMIT_PREPARE value then
     *the method call the executeBatch() method.
     * @param sqlBuilder 
     */
    private void addSQL(SQLBuilder sqlBuilder) {
        if (!sqlBuilder.isEmpty()) {
            sql.add(sqlBuilder.getSQL());
        }
    }

    /**
     *Prepare the next batch update
     *
     */
    private void prepareBatch() {
        Set set = sqlTable.keySet();
        Iterator<TABLE> ite = set.iterator();
        while (ite.hasNext()) {
            prepareTable(ite.next());
        }
    }

    /**
     *Create and return a new id for the corpus
     *@return id
     */
    public int createIdCorpus() {
        return createIntId("id_corpus", "corpus");
    }

    /**
     *Create and return a new id for the functions
     *@return id
     */
    public int createIdFunction() {
        return createIntId("id_function", "function");
    }

    /**
     *Create and return a new id for the concept
     *@return id
     */
    public int createIdConcept() {
        return createIntId("id_concept", "concept");
    }

    /**
     *Create and return a new id for the sentences
     *@return id
     */
    public int createIdSentence() {
        return createIntId("id_sent", "sentence");
    }

    /**
     *Create and return a new id for the document
     *@return id
     */
    public int createIdDocument() {
        return createIntId("id_document", "document");
    }

    /**
     *Create and return a new id for table
     *@param key primary key to check
     *@param table
     *@return id
     */
    private int createIntId(String key, String table) {
        getConnection();
        int result = 1;
        try {
            Statement s = connection.createStatement();
            ResultSet res = s.executeQuery("SELECT count(" + key + ") FROM " + table);
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
     *Get the id of a corpus
     *@param path the corpus to found
     *@return the id or -1 if not found
     */
    public int corpusID(File path) {
        getConnection();
        int result = -1;
        try {
            PreparedStatement s = connection.prepareStatement("SELECT id_corpus FROM corpus WHERE path=?");
            s.setString(1, path.getAbsolutePath());
            ResultSet res = s.executeQuery();
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
     *Gets the functions stored
     *@return a map, the key is the name of the function, the value is the id.
     *In XIP we know the name and not the id, in goldminer we know the id and not the name.
     */
    public Hashtable<String, Integer> getFunctions() {
        getConnection();
        // about 24 functions stored, see FUNCTION definition in the DataWriter
        Hashtable<String, Integer> result = new Hashtable<String, Integer>(24);
        try {
            PreparedStatement s = connection.prepareStatement("SELECT id_function,name FROM function ");
            ResultSet res = s.executeQuery();
            while (res.next()) {
                result.put(res.getString("name"), res.getInt("id_function"));
            }
            res.close();
            s.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return result;
    }

    /**
     *Get the document ID
     *@param name the name of the document
     *@param id_corpus the id of the corpus
     *@return the id of the document or -1 if not found
     */
    public int documentID(String name, int id_corpus) {
        getConnection();
        int result = -1;
        try {
            PreparedStatement s = connection.prepareStatement("SELECT id_document FROM document WHERE name=? and id_corpus=?");
            s.setString(1, name);
            s.setInt(2, id_corpus);
            ResultSet res = s.executeQuery();
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

    public boolean exist(File file) {
        int corpusID = this.corpusID(file);
        boolean document = -1 != this.documentID("", 1);
        return false;
    }

    /***
     *Get the concepts stored
     *@return a map, the keys are the lemmas of the concepts and the values are the ids
     *
     */
    public Hashtable<String, Integer> getConcepts() {
        getConnection();
        //capacity initial 40000, we want to stored a lot of documents
        Hashtable<String, Integer> map = new Hashtable<String, Integer>(40000);
        try {
            PreparedStatement s = connection.prepareStatement("SELECT lemma,id_concept FROM concept");
            ResultSet res = s.executeQuery();
            while (res.next()) {
                map.put(res.getString("lemma"), res.getInt("id_concept"));
            }
            res.close();
            s.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return map;
    }

    /**
     *Add a new corpus in the database
     *@param file the path of the corpus
     *@param id_corpus the id of the corpus
     *@return the id of the corpus
     *
     *NB : the corpus is the parent directory of a file.
     */
    public int addCorpus(File file, int id_corpus) {
        int resID = corpusID(file.getParentFile()); //recherche si le corpus exist

        if (resID == -1) {
            //s'il n'existe pas
            Object[] values = {id_corpus, "'" + file.getParent().replace("\\", "\\\\") + "'"};
            addRowValue(TABLE.corpus, values);
            resID = id_corpus;
        }
        return resID;
    }

    /**
     *Add a new document in the database
     *@param file the file to add
     *@param id_corpus the corpus of the document
     *@param id_document the id (primary key)
     *@return the id of the document
     *NB : we add the filename not the path, the path is in the corpus table
     */
    public int addDocument(File file, int id_corpus, int id_document) {
        int docID = documentID(file.getName(), id_corpus);
        if (docID == -1) {
            //le document n'existe pas
            Object[] values = {id_document, id_corpus, "'" + file.getName() + "'"};
            addRowValue(TABLE.document, values);
            docID = id_document;
        }
        return docID;
    }

    /**
     *Add a new parameter in the database
     *@param key the primary key of the parameter
     *@param begin the begin offset of the parameter
     *@param end the end offset of the parameter
     */
    public void addParameter(String key, long begin, long end) {
        Object[] values = {"'" + key + "\'", 1, begin, end};
        addRowValue(TABLE.parameter, values);
    }

    /**
     *Add a new dependency_parameter in the database
     *@param id_dep the id_dep
     *@param id_param the id_param in the dependency
     *@param function_id the function of the dependency
     */
    public void addDependencyParameter(String id_dep, String id_param, int function_id) {
        Object[] values = {"'" + id_dep + "'", "'" + id_param + "'", function_id, 1};
        addRowValue(TABLE.dependency_parameter, values);
    }

    /**
     *Add a new concept_parameter in the database
     *@param id_param the id_param
     *@param idLemma the id of the concept
     *@param id_sent the id_sent
     */
    public void addConceptParameter(String id_param, int idLemma, long id_sent) {
        Object[] values = {"'" + id_param + "'", idLemma, id_sent, 1};
        addRowValue(TABLE.concept_parameter, values);
    }

    /**
     *Add a new dependency in the database
     *@param key the primary key of the dependency
     *@param id_sent the sentence
     *@param id_function 
     * @param v the concepts used for the dependency
     *@param arity the number of parameters
     */
    public void addDependency(String key, int id_sent, int id_function, Vector<Integer> v, int arity) {
        String list = v.toString().replaceAll("\\[", "");
        list = list.replaceAll("\\]", "");
        Object[] values = {"'" + key + "'", id_sent, id_function, "'" + list + "'", arity};
        addRowValue(TABLE.dependency, values);
    }

    /**
     *Add a new function in the database
     *@param id the id function
     *@param function the name of the function
     */
    public void addFunction(int id, String function) {
        Object[] values = {id, "'" + function + "'", 1};
        addRowValue(TABLE.function, values);
    }

    /**
     *Add a new concept in the database
     *@param lemma the lemma to insert
     *@param id the id for the lemma (primary key)
     *
     */
    public void addConcept(String lemma, int id) {
        Object[] values = {id, "'" + lemma + "'", 1};
        addRowValue(TABLE.concept, values);
    }

    /**
     *Add a new sentence in the database
     *@param id_sent the id (primary key)
     *@param id_doc the document id
     *@param begin the begin offset
     *@param end the end offset
     */
    public void addSentence(int id_sent, int id_doc, long begin, long end) {
        Object[] values = {id_sent, id_doc, begin, end};
        addRowValue(TABLE.sentence, values);
    }

    /**
     *Add a new concpetsurface in the databse
     *@param indexLeaf index of the concept in a sentence
     *@param id_sent index of the sentence
     *@param idconcept 
     * @param begin the begin offset of the concept
     *@param end the end offset of the concept
     *
     *@deprecated This method run correctly but in goldminer we don't use the offset of a concept but
     *the offset of the parameters, and the concept is include in a parameter.
     *
     */
    public void addConceptSurface(long indexLeaf, int id_sent, int idconcept, long begin, long end) {
        String keySurface = indexLeaf + "_" + id_sent;
        Object[] values = {"'" + keySurface + "'", idconcept, id_sent, begin, end};
        addRowValue(TABLE.conceptsurface, values);
    }

    /**
     *Constuct an SQL command.
     *usefull to create command like INSERT INTO table (columns) VALUES (values)
     */
    private class SQLBuilder {

        private StringBuilder builder;

        /**
         *Build a SQL command like INSERT INTO table (columns) VALUES (values)
         *@param table the table to update
         *@param columns columns used for the table
         *@param values the values to insert
         *
         *NB : if values is empty, the SQL command will be empty
         */
        public SQLBuilder(TABLE table, String[] columns, Vector<Object[]> values) {
            builder = new StringBuilder();
            if (!values.isEmpty()) {
                insertSQL(table, columns);
                insertValues(values.get(0));
                for (int i = 1; i < values.size(); i++) {
                    builder.append(" , ");
                    insertValues(values.get(i));
                }
            }
        }

        /**
         *return if the command is empty or not
         *@return true if empty, false otherwise
         */
        public boolean isEmpty() {
            return builder.length() == 0;
        }

        public String toString() {
            return getSQL();
        }

        /**
         *Write the INSERT INTO syntax for the SQL command
         *@param table table to update
         *@param columns the columnf for the table
         */
        private void insertSQL(TABLE table, String[] columns) {
            builder = new StringBuilder();
            builder.append("INSERT INTO ");
            builder.append(table);
            builder.append(" ( ");
            builder.append(columns[0]);
            for (int i = 1; i < columns.length; i++) {
                builder.append(",");
                builder.append(columns[i]);
            }
            builder.append(") VALUES  ");
        }

        /**
         *write the VALUES for an update
         *we use the toString() method to write the value of each object
         *@param values the value to insert
         *
         *example : (a,b,c,d) if values is {a,b,c,d}
         */
        private void insertValues(Object[] values) {
            builder.append(" ( ");
            builder.append(values[0]);
            for (int i = 1; i < values.length; i++) {
                builder.append(",");
                builder.append(values[i]);
            }
            builder.append(" ) ");
        }

        /**
         *Return the SQL command
         *@return the SQL command or null if the command is empty
         */
        public String getSQL() {
            String res = null;
            if (!isEmpty()) {
                res = builder.toString();
            }
            return res;
        }

        /**
         *Append something to the command.
         *@param o the object to append
         *example : append(" ON DUPLICATE KEY UPDATE occurence=occurence+1 ")
         */
        public void append(Object o) {
            if (!isEmpty()) {
                builder.append(o);
            }
        }
    }
}
