/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 *
 * GoldminerEngine.java
 *
 * Created on September 26, 2006, 10:11 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer;

import com.xerox.jxip.JXip;
import com.xerox.xrce.xipui.CorpusElement;
import com.xerox.xrce.xipui.Offset;
import com.xerox.xrce.xipui.Project;
import java.util.*;
import java.sql.*;
import org.apache.log4j.*;
import java.io.*;
import com.xerox.xrce.xipui.exception.BadGrammarHandlerException;
import jcoref.CorefResolver;

/**
 *
 * @author vgrassau
 */
public class GoldminerEngine {

    /** type any */
    public static final int ANY_TYPE = -10;
    /** context de goldminer */
    private GoldminerContext context;
    /** connexion sur la base de donnee */
    private GoldminerDataBase database;
    /** les recherches a effectuer */
    private Vector<QueryNode> queries;
    /** table des documents disponibles */
    private Map fileMap;
    private Logger log = Logger.getLogger("debug");
    /** reponse une requete */
    private Answer answer = null;

    /** Creer une instance de goldminer a partir d'un nom d'un projet
     *@param projectFullName path complet d'un projet afin de lire le fichier de configuration
     */
    public GoldminerEngine(String projectFullName) {
        context = new GoldminerContext(projectFullName);
        queries = new Vector<QueryNode>();
        fileMap = new Hashtable();
    }

    /**
     *Set the host for the database connection
     *@param host_ hostname
     *
     *@see GoldminerContext#setHost(String)
     */
    public void setHost(String host_) {
        context.setHost(host_);
    }

    /**
     *Returns the host name of the database connection
     *@return an hostname
     *
     **@see GoldminerContext#getHost()
     */
    public String getHost() {
        return context.getHost();
    }

    /**
     *returns the port used for the connection
     *@return a String in number format
     *
     *@see GoldminerContext#getPort()
     */
    public String getPort() {
        return context.getPort();
    }

    /**
     *Returns the database name used
     *@return a name of the database
     *
     **@see GoldminerContext#getDatabase()
     */
    public String getDatabase() {
        return context.getDatabase();
    }

    /**
     *Returns the user for the connection
     *@return a username
     *
     **@see GoldminerContext#getUser()
     */
    public String getUser() {
        return context.getUser();
    }

    /**
     *Returns the password used for the connection no crypted
     *@return the password
     *
     *@see GoldminerContext#getPassword()
     */
    public String getPassword() {
        return context.getPassword();
    }

    /**
     *Sets the port for the connection to the server database.
     *3306 is the default port used by MySQL
     *@param port_ 
     *@see GoldminerContext#setPort(String)
     */
    public void setPort(String port_) {
        context.setPort(port_);
    }

    /**
     *Sets the password to use for the connection
     *@param password_ , the password no crypted
     *
     *@see GoldminerContext#setPassword(String)
     */
    public void setPassword(String password_) {
        context.setPassword(password_);
    }

    /**
     *Sets the database name to use
     *@param database_ the database to use.
     *
     *@see GoldminerContext#setDatabase(String)
     */
    public void setDatabase(String database_) {
        context.setDatabase(database_);
    }

    /**
     *Sets the user to use for the connection
     *@param user_ the user to use default is root
     *
     *@see GoldminerContext#setUser(String)
     */
    public void setUser(String user_) {
        context.setUser(user_);
    }

    /**
     *Checks if the connection parameters are valides. If not, return false
     *@return true if the connection parameters are corrects, false otherwise
     *
     *@see isConnected()
     *@see GoldminerDataBase#checkConnection()
     */
    public boolean checkConnection() {
        return database.checkConnection();
    }

    /**
     *Checks if the GoldminerEngine is connected to a database.
     *Doesn't check if the connection parameters are corrects or not
     *@return true if currently connected, false otherwise
     *
     *@see checkConnection()
     *@see GoldminerDataBase#isConnected()
     */
    public boolean isConnected() {
        return database.isConnected();
    }

    /**
     *Load the GoldminerEngine and create 5 QueryNode if the loading succeed.
     *This method initialize the context for the engine and the connection to a database
     *@return true if the loading succeed, false otherwise
     *
     *@see GoldminerContext#initContext()
     *@see initConnection2DB()
     */
    public boolean load() {
        boolean res = context.initContext() && initConnection2DB();
        if (res) {
            createQueryNode(5);
        }
        return res;
    }

    /**
     *Try to init the connection to the database. At the same time,
     *this metho create the table of available files in the database
     *@return true if the connection succeed or false otherwise
     *
     *@see load()
     */
    private boolean initConnection2DB() {
        boolean succeed = false;
        try {
            database = new GoldminerDataBase(context);
        } catch (Exception e) {
            database = null;
            succeed = false;
        }
        if (database != null) {
            try {
                boolean listSource = database.getListOfSource();
                //boolean mapRelationOrType = database.getMapConceptOfPOS(4);
                boolean mapRelationOrType = database.initFunctions();
                if (listSource && mapRelationOrType) {
                    Map<Integer, String> map = (Map) context.remove(GoldminerContext.MAP_CONCEPT);
                    context.setTableOfBasicElement(map);
                    fileMap = (Map) context.remove(GoldminerContext.LIST_SOURCE);
                } else {
                    succeed = false;
                }
            } catch (Exception e) {
                log.error("Problem,can't create the table of documents", e);
                succeed = false;
            }
            succeed = true;
        }
        return succeed;
    }

    /**
     *Removes the research of lemma from a QueryNode.
     *After ,the QueryNode becomes an empty QueryNode.
     *@param index the index of the QueryNode to determine the lemma
     *@return true if the lemma was well removed, false otherwise.
     *
     *@see GoldminerDataBase#removeConcept(int)
     */
    public synchronized boolean removeLemmaForQueryNode(int index) {
        boolean succeed = false;
        try {
            QueryNode query = queries.get(index);
            int idConcept = query.getIdLemma();
            if (idConcept != -1) {
                if (database.removeConcept(idConcept)) {
                    query.setLemma(-1, null);
                } else {
                    succeed = false;
                }
            }
            //on eneleve les natures associe au querynode :
            int idType = query.getSelectedTypeElement();
            if (idType != ANY_TYPE) {
                database.removeConcept(idType);
            }
            //le on remet a jour la liste des types disponibles
            Vector<Integer> types = new Vector<Integer>(getTypesList().keySet());
            types.add(0, ANY_TYPE);
            query.setListTypeElement(types);
            succeed = true;
        } catch (Exception e) {
            log.debug("le QueryNode demande n'existe pas", e);
            succeed = false;
        }
        return succeed;
    }

    /**
     *Saves the context and reloads the GoldminerEngine.
     *the context is re-initialize and the connection to the database also.
     *All the QueryNode are cleaned.
     *@return true if the reloading succeed
     *
     *@see GoldminerContext#save()
     *@see GoldminerContext#initContext()
     *@see initConnection2DB()
     */
    public boolean reload() {
        boolean success = false;
        try {
            //on sauvegarde le context
            this.context.save();
            // on relit le context
            boolean contextState = context.initContext();
            boolean connectionDataState = false;
            if (contextState) {
                connectionDataState = initConnection2DB();
                this.queries.clear();
                this.createQueryNode(5);
                this.answer = null;
            }
            success = contextState && connectionDataState;
        } catch (Exception e) {
            log.error(e);
            success = false;
        }
        return success;
    }

    /**
     *Returns the database connection object.
     *@return the connection,null if there is no connection.
     *   NB : it's not advised to use this method to get the connection and make some lemme research.
     *
     *@see GoldminerDataBase#getConnection()
     */
    public Connection getConnection() {
        Connection current = null;
        if (database != null) {
            current = this.database.getConnection();
        }
        return current;
    }

    /**
     *Set a lemma for a given index of a QueryNode
     *@param index the index of the QueryNode to modify
     *@param lemma the lemma to search in the database
     *@return true if the lemma was found in the database, false otherwise
     *
     *@see removeLemmaForQueryNode(int)
     *@see GoldminerDataBase#getConceptInformation(String)
     *@see GoldminerDataBase#addConcept(int)
     */
    public synchronized boolean setLemmaForQueryNode(int index, String lemma) {
        //collection d'id pour le type possible des noeuds
        Vector<Integer> v = null;
        boolean success = false;
        int idConcept = -1;
        try {
            QueryNode query = queries.get(index);
            //si la query possede deja un lemme on doit le supprimer.
            if (query.getLemma() != null) {
                removeLemmaForQueryNode(index);
            } else { 
                //sinon on verifie que le recherche d'un type n'a pas ete effectue
                Integer idType = query.getSelectedTypeElement();
                if (idType != ANY_TYPE) {
                    database.removeConcept(idType);
                }
            }
            idConcept = database.getConceptInformation(lemma);
            if (idConcept == -1) {
                success = false;
            } else {
                //on trouve le lemme
                database.addConcept(idConcept);
                //natures possibles pour le lemme
                v = (Vector<Integer>) context.remove(idConcept + GoldminerContext.LEMMA_TYPES);
                //on rajoute a la fin la nautre ANY permettant  les contraintes
                //sur le lemme
                query.setLemma(idConcept, lemma);
                if (v != null) {
                    v.add(ANY_TYPE);
                    query.setListTypeElement(v);
                }
                success = true;
            }
        } catch (Exception e) {
            log.error("ERROR", e);
            success = false;
        }
        return success;
    }

    /**
     *Returns the levels for a file.
     *@param file the file to determine the levels
     *@return an array with  3 integer :
     *  - the first is the number of sentences with a strong level.
     *  - the second is the number  of sentences with a medium level.
     *  - the third is the number of sentences with a normal level.
     *  Return null, if no research had been launch.
     *
     *@see sendRequest()
     *@see Answer#getNumberOfLevelSentences(String)
     */
    public int[] getNumberOfLevelSentences(String file) {
        int[] array = null;
        if (answer != null) {
            array = answer.getNumberOfLevelSentences(file);
        }
        return array;
    }

    /**
     *Sets the selection of TypeElement for a QueyNode.
     *@param index 
     *@param idTypeElement the id of the TypeElement to select.
     *@return true if the QueryNode exist and no error is thrown during the selection.
     *
     *@see getBasicElement(int)
     *@see QueryNode#setSelectedTypeElement(int)
     *@see GoldminerDataBase#getFunctionInformation(String)
     *@see GoldminerDataBase#removeFunction(int)
     */
    public boolean setSelectedTypeElementForQueryNode(int index, int idTypeElement) {
        try {
            QueryNode query = queries.get(index);
            if (idTypeElement != query.getSelectedTypeElement() && query.isEmpty()) {
                //on doit supprimer la recherche du type.
                BasicElement oldElement = getBasicElement(query.getSelectedTypeElement());
                if (oldElement != null) {
                    //int id = database.getFunctionInformation(oldElement.getRealName());
                    if (oldElement.getId() != -1) {
                        database.removeFunction(oldElement.getId());
                    }
                }
                BasicElement el = getBasicElement(idTypeElement);
                if (el != null) {
                    // int id = database.getFunctionInformation(el.getRealName());
                    if (el.getId() != -1) {
                        database.addFunction(el.getId());
                        //idsearched.add(id);
                    }
                }
            }
            query.setSelectedTypeElement(idTypeElement);
            return true;
        } catch (Exception e) {
            log.debug("The QueryNode : " + index + " doesn't exist", e);
            return false;
        }
    }

    /**
     *Sends the request to research and create an answer.
     *@return true if an answer is created,even if it's empty,
     *        false if error
     *
     *@see GoldminerDataBase#getFunctionInformation(String)
     *@see GoldminerDataBase#addFunction(int)
     */
    public synchronized boolean sendRequest() {
        boolean success = false;
        try {
            Collection idsearched = new Vector();
            //table des contraintes
            Map<Integer, Collection> constraints = new HashMap<Integer, Collection>();
            Iterator<QueryNode> ite = queries.iterator();
            long start2 = System.currentTimeMillis();
            while (ite.hasNext()) {
                QueryNode query = ite.next();
                if (!query.isEmpty()) {
                    idsearched.add(query.getIdLemma());
                    Integer type = query.getSelectedTypeElement();
                    if (type != ANY_TYPE) {
                        Collection c = null;
                        //ajouter une contrainte :
                        if (constraints.containsKey(type)) {
                            c = constraints.get(type);
                        } else {
                            c = new Vector();
                            constraints.put(type, c);
                        }
                        c.add(query.getIdLemma());
                    }
                } else {
                    int idType = query.getSelectedTypeElement();
                    if (idType > 0) {
                        idsearched.add(idType);
                    }
                }
            }
            answer = new Answer(context.getMapQueries(), fileMap, context.getMapOfBasicElement(), idsearched, constraints);
            success = true;
        } catch (Exception e) {
            log.error("ERROR : ", e);
            success = false;
        }
        return success;
    }

    /**
     *Creates several QueryNode
     *@param number the number of QueryNode to create
     */
    public void createQueryNode(int number) {
        this.queries.clear();
        for (int i = 0; i < number; i++) {
            QueryNode query = new QueryNode();
            Vector<Integer> v = new Vector<Integer>(getTypesList().keySet());
            v.add(0, GoldminerEngine.ANY_TYPE);
            query.setListTypeElement(v);
            queries.add(query);
        }
    }

    /**
     *Returns the list of the ids of TypeElement from a QueryNode
     *@param nQuery the index of the QueryNode
     *@return a Collection of a ids
     */
    public Collection<Integer> getListOfTypeElement(int nQuery) {
        return this.queries.get(nQuery).getListTypeElement();
    }

    /**
     *Returns the number of QueryNode created
     *@return number of QueryNode created
     */
    public int getSizeOfQueries() {
        return this.queries.size();
    }

    /***
     *Returns the lemma from a QueryNode
     *@param nQuery the index of the QueryNode
     *@return the lemma, or null if no lemma is set for the QueryNode
     */
    public String getLemma(int nQuery) {
        return this.queries.get(nQuery).getLemma();
    }

    /**
     *Return the id of a lemma from a QueryNode
     *@param nQuery the index index of the QueryNode
     *@return id of the lemma or -1 if no lemma is set for the QueryNode
     */
    public int getIdLemma(int nQuery) {
        return this.queries.get(nQuery).getIdLemma();
    }

    /**
     *Returns all QueryNode created
     *@return all QueryNode even if they are empty, or an empty list if no QueryNode was created
     */
    public Vector<QueryNode> getAllQueryNode() {
        return this.queries;
    }

    /**
     *Returns the map of the available BasicElement from the database
     *@return a map of all BasicElement stored in the database, it means TypeElement and RelationElement,
     *       the map can be empty
     *
     *@see GoldminerContext#getMapOfBasicElement()
     */
    public Map<Integer, BasicElement> getMapOfAvailableElement() {
        return this.context.getMapOfBasicElement();
    }

    /**
     *Returns a BasicElement
     *@param id the id of the BasicElement to get
     *@return the BasicElement with the given id, or null, if not found
     *
     *@see GoldminerContext#getBasicElement(int)
     */
    public BasicElement getBasicElement(int id) {
        return context.getBasicElement(id);
    }

    /**
     *Returns the map of the results from queries.
     *@return a map, the entries are GoldminerKey, and values are Collection of Parameters.
     *
     *@see GoldminerContext#getMapQueries()
     *
     */
    public Map<GoldminerKey, Parameters> getMapQueries() {
        return context.getMapQueries();
    }

    /**
     *Adds a TypeElement to the context of GoldminerEngine
     *@param element the TypeElement to add
     *
     *@see GoldminerContext#addTypeElement(TypeElement)
     */
    public void addTypeElement(TypeElement element) {
        context.addTypeElement(element);
    }

    /**
     *Removes a TypeElement to the context of GoldminerEngine
     *@param id the id of the TypeElement
     *
     *@see GoldminerContext#removeTypeElement(int)
     */
    public void removeTypeElement(int id) {
        context.removeTypeElement(id);
    }

    /**
     *Removes a RelationElement to the context of GoldminerEngine
     *@param id the id of the RelationElement
     *
     *@see GoldminerContext#removeRelationElement(int)
     */
    public void removeRelationElement(int id) {
        context.removeRelationElement(id);
    }

    /**
     *Adds a RelationElement to the context of GoldminerEngine
     *@param id 
     * @param element the RelationElement to add
     *
     *@see GoldminerContext#addRelationElement(RelationElement)
     */
    public void addRelationElement(int id, RelationElement element) {
        context.addRelationElement(element);
    }

    /**
     *Returns the map of TypeElement in the context
     *@return a map containing the TypeElement and their ids
     *
     *@see GoldminerContext#getTypesList()
     *
     *NB: the name of this method is not well choosed
     */
    public Map<Integer, BasicElement> getTypesList() {
        return context.getTypesList();
    }

    /**
     *Returns the map of RelationElement in the context
     *@return a map containing the RelationElement and their ids
     *
     *@see GoldminerContext#getRelationsList()
     *
     *NB: the name of this method is not well choosed
     */
    public Map<Integer, BasicElement> getRelationsList() {
        return context.getRelationsList();
    }

    /**
     *Saves the context of this GoldminerEngine
     *@return true if the context was correctly saved, false otherwise
     *
     *@see GoldminerContext#save()
     */
    public boolean save() {
        boolean success = false;
        try {
            context.save();
            success = true;
        } catch (Exception e) {
            log.error("ERROR saving context", e);
            success = false;
        }
        return success;
    }

    /**
     *Returns the offset extracted for highlighting for a file.
     *@param file the file to extract the offsets
     *@return a list of GoldminerOffset, if there some offsets are equals but they are not the same list of constraints,
     *        then a merge of the offset is done.
     *@throws Exception if error, if no answer was created then return null
     */
    public Vector<GoldminerOffset> getOffset(String file) throws Exception {
        Vector<GoldminerOffset> vectorGoldminerOffset = null;
        try {
            vectorGoldminerOffset = new Vector<GoldminerOffset>();
            if (this.answer != null) {
                Map<Integer, Sentence> sentences = answer.getSentences(file);
                if (sentences != null) {
                    boolean res = database.getMapOfOffset(sentences.values());
                    if (res) {
                        //on recupere la table d'offset dans le contexte
                        //param -> offfset
                        Map<String, Offset> map = (Map) context.remove(GoldminerContext.OFFSET_MAP);
                        Map<Integer, Offset> offsetSentence = (Map) context.remove(GoldminerContext.OFFSET_SENTENCES);
                        Iterator<Integer> iteSentence = offsetSentence.keySet().iterator();
                        while (iteSentence.hasNext()) {
                            Integer id_sent = iteSentence.next();
                            Offset off = offsetSentence.get(id_sent);
                            Sentence s = sentences.get(id_sent);
                            GoldminerOffset sOff = new GoldminerOffset(off);
                            sOff.setSentenceOffset(true);
                            sOff.setType(s.getLevel());
                            vectorGoldminerOffset.add(sOff);
                        }
                        Map<String, Parameter> mapParameters = context.getMapParameter();
                        Iterator<String> iteMap = map.keySet().iterator();
                        while (iteMap.hasNext()) {
                            String id_param = iteMap.next();
                            Offset off = map.get(id_param);
                            GoldminerOffset sOff = new GoldminerOffset(off);
                            int ind = vectorGoldminerOffset.indexOf(sOff);
                            if (ind != -1) {
                                sOff = vectorGoldminerOffset.get(ind);
                            } else {
                                vectorGoldminerOffset.add(sOff);
                            }
                            if (mapParameters != null) {
                                Parameter p = mapParameters.get(id_param);
                                if (p != null) {
                                    if (!sOff.isConstraint()) {
                                        sOff.setIsConstraint(p.isConstraintOk());
                                    }
                                    Vector<Integer> v = p.getConstraints();
                                    for (Iterator<Integer> it = v.iterator(); it.hasNext();) {
                                        sOff.addElement(getBasicElement(it.next()));
                                    }
                                    sOff.addConcepts(convertToLemmas(p.getConcepts()));
                                }
                            }
                        }
                    }
                }
            }
        } catch (Exception e) {
            throw e;
        }
        return vectorGoldminerOffset;
    }

    public String getLemmaFromQueries(int idLemma) {
        boolean stop = false;
        String lemma = null;
        for (Iterator<QueryNode> it = queries.iterator(); it.hasNext() && !stop;) {
            QueryNode q = it.next();
            if (q.getIdLemma() == idLemma) {
                stop = true;
                lemma = q.getLemma();
            }
        }
        return lemma;
    }

    public Collection<String> convertToLemmas(Collection<Integer> c) {
        Collection<String> lemmas = new Vector<String>();
        for (Iterator<Integer> it = c.iterator(); it.hasNext();) {
            String lemma = getLemmaFromQueries(it.next());
            if (lemma != null) {
                lemmas.add(lemma);
            }
        }
        return lemmas;
    }

    public GoldminerDataWriter getGoldminerDataWriter(Project project) {
        return new GoldminerDataWriter(database, project);
    }

    /**
     *Parses and stores the analysis of the given collection of texts in the database .
     *This method uses the module of Coreference developed by Salah Ait Mokthar.
     *@param project the project using GoldminerEngine
     *@param corpus the collection of CorpusElement to parse
     *@throws Exception if error
     *        BadGrammarHandlerException if the grammar of the project is not loaded
     *
     *@see GoldminerDataWriter#write(File,CorefResolver)
     */
    public void parseAndStore(Project project, Collection<CorpusElement> corpus) throws Exception {
        GoldminerDataWriter writer = new GoldminerDataWriter(database, project);
        CorefResolver resolver = null;
        JXip jxip = project.getXip().getJXip();
        int gHandler = project.getGhandler();
        if (gHandler == -1) {
            throw new BadGrammarHandlerException(gHandler);
        } else {
            resolver = new CorefResolver(jxip, gHandler);
        }
        if (resolver != null) {
            resolver.setTrace(false);
            Iterator<CorpusElement> iteCorpus = corpus.iterator();
            long startFull = System.currentTimeMillis();
            while (iteCorpus.hasNext()) {
                CorpusElement ce = iteCorpus.next();
                if (!ce.exists() || ce.isInput() || ce.isPartOfInput() || ce.isXML()) {
                    throw new Exception(ce + " is not a valid file for goldminer, process interrupted.");
                } else {
                    //try to parse and store the corpus element
                    try {
                        Collection<File> files = ce.getFiles();
                        for (Iterator<File> it = files.iterator(); it.hasNext();) {
                            long start = System.currentTimeMillis();
                            writer.write(it.next(), resolver);
                            long end = System.currentTimeMillis();
                            log.info("Stored in " + (end - start) + " ms");
                            log.info("Elpased time :  " + (System.currentTimeMillis() - startFull) + "  ms \n");
                        }

                    } catch (Exception e) {
                        throw e;
                    }
                }
            }
        } else {
            throw new Exception("the coreference resolver is not initialized");
        }
    }

    /**
     *Returns the files matching with the research
     *@return the list of the absolute path or null, if no answer was created
     */
    public Vector<String> getFiles() {
        Vector<String> result = null;
        if (answer != null) {
            result = answer.getAnswer();
        }
        return result;
    }

    /**
     * 
     * @return
     */
    public Overview getOverview() {
        Overview overview = null;
        try {
            overview = new Overview(context);
        } catch (Exception e) {
            log.error(e);
        }
        return overview;
    }
}
