/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 *
 * GoldminerContext.java
 *
 * Created on September 5, 2006, 4:55 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer;

import com.xerox.xrce.xipui.XMLProject;
import com.xerox.xrce.xipui.util.Context;
import com.xerox.xrce.xipui.util.XML;
import java.util.*;
import java.io.*;
import org.dom4j.*;

/**
 *
 * @author vgrassau
 */
public class GoldminerContext extends Context {

    public static final String host = "host";
    public static final String port = "port";
    public static final String user = "user";
    public static final String password = "password";
    public static final String database = "database";
    public static final String file_conf = "file_conf";

    //cle pour recuperer la liste des types possibles
    protected static final String typesList = "typesList";
    //cle pour recuperer la liste des relations possible
    protected static final String relationsList = "relationsList";
    //table d'offsets pour un fichier
    public static final String OFFSET_MAP = "OFFSET_MAP";
    //table d'offset de phrase pour un fichier
    public static final String OFFSET_SENTENCES = "OFFSET_SENTENCES";
    public static final String MAP_CONCEPT = "MAP_CONCEPT";
    public static final String LIST_SOURCE = "LIST_SOURCE";
    public static final String LEMMA_RELATIONS = "LEMMA_RELATIONS";
    public static final String LEMMA_TYPES = "LEMMA_TYPES";
    public static final String LEMMA_SENTENCES = "LEMMA_SENTENCES";
    public static final String MAP_QUERIES = "MAP_QUERIES";

    //table reference des types et relations possible
    private Map<Integer, BasicElement> basicElementTable;
    //table pour indexer la recherhce dans la base de donnees et ajouter les resultats
    private Map<GoldminerKey, Parameters> mapQueries;
    private Map<String, Parameter> mapParameter;
    //vector d'id pour les types
    private Vector idForTypeElement;
    //vector d'id pour les relations
    private Vector idForRelationElement;

    /** Creates a new instance of GoldminerContext
     *@param projectFullName the abosulte path of a project
     */
    public GoldminerContext(String projectFullName) {
        setProperty(file_conf, projectFullName + "/goldminer.xml");
        pre_init();
    }

    /**
     *Sets the hostname for a connection to a database
     *@param host_ a host name
     *
     *@see getHost()
     */
    public void setHost(String host_) {
        setProperty(host, host_);
    }

    /**
     *Sets the port for a connection to a database
     *@param port_ the port for sever, by default with MySQL is 3306
     *
     *@see getPort()
     */
    public void setPort(String port_) {
        setProperty(port, port_);
    }

    /**
     *Sets the password for the connection to the database.
     *@param password_ the password not cyrpted
     *
     *@see getPassword()
     */
    public void setPassword(String password_) {
        setProperty(password, password_);
    }

    /**
     *Sets the name of the database to use
     *@param database_ the name of the database
     *
     *@see getDatabase()
     */
    public void setDatabase(String database_) {
        setProperty(database, database_);
    }

    /**
     *Sets the user for the connection to the database
     *@param user_ the user to use
     *
     *@see getUser()
     *
     */
    public void setUser(String user_) {
        setProperty(user, user_);
    }

    /**
     *Does a pre-initialization with some default property for a connection to a database
     *
     */
    private void pre_init() {
        //setHost("etretat");
        setHost("localhost");
        setPort("3306");
        setUser("root");
        setPassword("root");
        setDatabase("factspotter");
        Map<String, BasicElement> mapTypes = new HashMap<String, BasicElement>();
        Map<String, BasicElement> mapRelations = new HashMap<String, BasicElement>();
        setProperty(GoldminerContext.typesList, mapTypes); //map temporaire

        setProperty(GoldminerContext.relationsList, mapRelations); //map temporaire

        mapQueries = new Hashtable<GoldminerKey, Parameters>();
        mapParameter = new Hashtable<String, Parameter>();
        basicElementTable = new Hashtable<Integer, BasicElement>();
        idForTypeElement = new Vector();
        idForRelationElement = new Vector();
    }

    /**
     *Returns a map of the relations to use
     *@return a map containing BasicElement and their ids.
     *
     *@see getTypesList()
     */
    public Map<Integer, BasicElement> getRelationsList() {
        return createSubMap(this.idForRelationElement);
    }

    /**
     *Returns a map of the types to use
     *@return a map containing BasicElement and their ids
     *
     *@see getRelationsList()
     */
    public Map<Integer, BasicElement> getTypesList() {
        return createSubMap(this.idForTypeElement);
    }

    /**
     *Adds a TypeElement in the context
     *@param element new TypeElement  to add
     */
    public void addTypeElement(TypeElement element) {
        int id = element.getId();
        if (!idForTypeElement.contains(id)) {
            idForTypeElement.add(id);
        }
        this.putBasicElement(element);
    }

    /**
     *Adds a RelationTypeElement
     *
     * @param element 
     */
    public void addRelationElement(RelationElement element) {
        int id = element.getId();
        if (!idForRelationElement.contains(id)) {
            idForRelationElement.add(id);
        }
        this.putBasicElement(element);
    }

    /**
     *Creates a map of BasicElement with only the keys in the given collection.
     *@param c collection of ids of BasicElement
     *@return a map with only the necessary BasicElement, if no BasicElement was found return an empty map
     */
    private Map<Integer, BasicElement> createSubMap(Collection<Integer> c) {
        Map<Integer, BasicElement> map = new HashMap<Integer, BasicElement>();
        Iterator<Integer> ite = c.iterator();
        while (ite.hasNext()) {
            Integer key = ite.next();
            map.put(key, basicElementTable.get(key));
        }
        return map;
    }

    /**
     *Returns the Map queries
     *@return the map with the Goldminerkey and the collection of the id of concept
     */
    public synchronized Map<GoldminerKey, Parameters> getMapQueries() {
        return mapQueries;
    }

    /**
     *Returns the map of the parameters extracted
     *@return a map, the key are the ids of the parameters, the values the parameters themselves
     */
    public synchronized Map<String, Parameter> getMapParameter() {
        return this.mapParameter;
    }

    /**
     *Returns the host for the connection to the databse
     *@return a name of the host or the IP adresse in a string representation
     *
     *@see setHost(String)
     */
    public String getHost() {
        return (String) getProperty(host);
    }

    /**
     *Returns the port for the connection to a database
     *@return the port
     *
     *@see setPort(String)
     */
    public String getPort() {
        return (String) getProperty(port);
    }

    /**
     *Returns the user for the connection to a database
     *@return a username
     *
     *@see setUser(String)
     */
    public String getUser() {
        return (String) getProperty(user);
    }

    /**
     *Returns the database name to use for the connection
     *@return a database name
     *
     *@see setDatabase(String)
     */
    public String getDatabase() {
        return (String) getProperty(database);
    }

    /**
     *Returns the password for the connection to the database.
     *@return the password not crypted
     *
     *@see setPassword(String)
     */
    public String getPassword() {
        return (String) getProperty(password);
    }

    /**
     *Adds a BasicElement in the context
     *@param element the new BasicElement to add
     */
    protected void putBasicElement(BasicElement element) {
        this.basicElementTable.put(element.getId(), element);
    }

    /**
     *Remove a TypeElement to list of the TypeElement to used.
     * @param id 
     */
    public void removeTypeElement(int id) {
        this.idForTypeElement.remove(new Integer(id));
    }

    /**
     *Remove a RelationElement to list of the TypeElement to used.
     * @param id 
     */
    public void removeRelationElement(int id) {
        this.idForRelationElement.remove(new Integer(id));
    }

    /**
     *Creates the map off All BasicElement available.
     *the split of the given map between the RelationElement and TypeElement is done by the
     *initialization of the context call the method <code> initContext()</code> first.
     *@param mapOfTypeAndRelation the map with the id and the realnames of the futur BasicElement.
     *
     */
    public void setTableOfBasicElement(Map<Integer, String> mapOfTypeAndRelation) {
        Iterator<Integer> ite = mapOfTypeAndRelation.keySet().iterator();
        Map<String, RelationElement> relations = (Map) this.remove(relationsList);
        Map<String, TypeElement> types = (Map) this.remove(typesList);
        while (ite.hasNext()) {
            Integer id = ite.next();
            String typeOrRelation = mapOfTypeAndRelation.get(id);
            //les relations :
            BasicElement basic = relations.get(typeOrRelation);
            if (basic != null) {
                basic.setId(id);
                basicElementTable.put(id, basic);
                this.idForRelationElement.add(id);
            } else {
                basic = types.get(typeOrRelation); //on cherche si c'est un type

                if (basic != null) {
                    basic.setId(id);
                    basicElementTable.put(id, basic);
                    this.idForTypeElement.add(id);
                } else {
                    RelationElement rel = new RelationElement(typeOrRelation);
                    rel.setId(id);
                    basicElementTable.put(id, rel);
                }
            }
        }
    }

    /**
     *Returns the BasicElement  (Relationelement or TypeElement)
     *@param id the id of the BasicElement.
     *@return the BasicElement or null if not found
     */
    public BasicElement getBasicElement(int id) {
        return basicElementTable.get(id);
    }

    /**
     *Renvoie toute la table des BasicElements possibles dans la base de donnees
     *@return Table d'entier (les idconcepts) et le BasicElement associe
     */
    public Map<Integer, BasicElement> getMapOfBasicElement() {
        return this.basicElementTable;
    }

    /**
     *inits the context, reads the configuration file.
     *creates the pre-map of the RelationElement and the TypeElement.
     *calls the method <code>save()</code>
     *@return true, if the context was corretly initialized.
     *
     *@see pre-init()
     *@see save()
     */
    public boolean initContext() {
        boolean success = false;
        try {
            pre_init();
            File setupFile = new File((String) getProperty(file_conf));
            if (setupFile.exists()) {
                XML xml = new XML(setupFile.getAbsolutePath());
                Element root = xml.getDocument().getRootElement();
                Element context = root.element(XMLProject.CONTEXT_);
                if (context != null) {
                    Iterator ite = context.elementIterator(XMLProject.PROPERTY_);
                    while (ite.hasNext()) {
                        Element property = (Element) ite.next();
                        String name = property.attributeValue(XMLProject.NAME_);
                        String value = property.attributeValue(XMLProject.VALUE_);
                        setProperty(name, value);
                    }
                }
                //we have to connect to the database
                //
                Element listProject = root.element("relations");
                Iterator iteRelation = listProject.elementIterator("relation");
                Map<String, BasicElement> mapRelations = new HashMap<String, BasicElement>();
                while (iteRelation.hasNext()) {
                    Element relationXML = (Element) iteRelation.next();
                    RelationElement rEl = new RelationElement(relationXML.attributeValue("name"));
                    rEl.setLabel(relationXML.attributeValue("label"));
                    rEl.setWeight(Integer.parseInt(relationXML.attributeValue("weight")));
                    Element descriptionXML = relationXML.element("description");
                    if (descriptionXML != null) {
                        rEl.setDescription(descriptionXML.getTextTrim());
                    }
                    mapRelations.put(rEl.getRealName(), rEl);
                }
                //map temporaire
                setProperty(GoldminerContext.relationsList, mapRelations); 
                Element listTypes = root.element("types");
                Iterator iteTypes = listTypes.elementIterator("type");
                Map<String, BasicElement> mapTypes = new HashMap<String, BasicElement>();
                while (iteTypes.hasNext()) {
                    Element typeXML = (Element) iteTypes.next();
                    TypeElement tEl = new TypeElement(typeXML.attributeValue("name"));
                    tEl.setLabel(typeXML.attributeValue("label"));
                    tEl.setWeight(Integer.parseInt(typeXML.attributeValue("weight")));
                    Element descriptionXML = typeXML.element("description");
                    if (descriptionXML != null) {
                        tEl.setDescription(descriptionXML.getTextTrim());
                    }
                    mapTypes.put(tEl.getRealName(), tEl);
                }
                //map temporaire
                setProperty(GoldminerContext.typesList, mapTypes); 
            } else {
                //we must create the file setup with the default context !!!
                setupFile.createNewFile();
                save();
            }
            success = true;
        } catch (Error e) {
            log.fatal("FATAL ERROR", e);
            success = false;
        } catch (Exception e) {
            log.fatal("FATAL ERROR", e);
            success = false;
        }
        return success;
    }

    /**
     *Save the context in XML document
     *
     * @throws java.lang.Exception 
     */
    public void save() throws Exception {
        try {
            XML xml = new XML();
            xml.setRoot("goldminer");
            xml.addNode(XMLProject.CONTEXT_);
            String[] keys = {database, host, port, user, password};
            for (int i = 0; i < keys.length; i++) {
                xml.add(XMLProject.PROPERTY_);
                xml.addAttribute(XMLProject.NAME_, keys[i]);
                xml.addAttribute(XMLProject.VALUE_, getProperty(keys[i]).toString());
                xml.parent();
            }
            xml.addNode("relations");
            writeElement(xml, getRelationsList().values(), "relation");
            xml.addNode("types");
            writeElement(xml, getTypesList().values(), "type");
            xml.save((String) getProperty(file_conf));
        } catch (Exception e) {
            throw e;
        }
    }

    /**
     *Adds to XML document the information of the BasicElement from the given collection.
     *Adds several tag named by <code>xmlNode</code> parameter. These tags contains in attributes the attributes
     *of the BasicElement
     *@param doc the  XML document
     *@param c the collection of BasicElement to write
     *@param xmlNode the name of xml tag to write
     *
     */
    private void writeElement(XML doc, Collection<BasicElement> c, String xmlNode) throws Exception {
        Iterator<BasicElement> ite = c.iterator();
        while (ite.hasNext()) {
            BasicElement element = ite.next();
            doc.add(xmlNode);
            doc.addAttribute("name", element.getRealName());
            doc.addAttribute("label", element.getLabel());
            doc.addAttribute("weight", String.valueOf(element.getWeight()));
            String desc = element.getDescription();
            if (desc != null) {
                doc.add("description", desc);
                doc.parent();
            }
            doc.parent();
        }
    }

    /**
     *Adds or updates a new GoldminerKey in the MapQueries and update some results with collection of Parameter associated.
     *@param id_document the id of a document
     *@param id_dependency the id of a dependency
     *@param id_param the id of a parameter
     *@param id_concept the id of a concept, this id will be added to the parameter
     *@param id_function  the id of a function (Type or Relation), this id will be added to the parameter like a constraint
     *
     */
    public synchronized void addEntryForMapQueries(int id_document, String id_dependency, String id_param, int id_concept, int id_function) {
        //get the parameter
        Parameter param = mapParameter.get(id_param);
        if (param == null) {
            param = new Parameter(id_param);
            mapParameter.put(id_param, param);
        }
        param.addConstraint(id_function);
        param.addConcept(id_concept);
        GoldminerKey key = new GoldminerKey(id_document, id_dependency);
        //ajouter la dep pour le parametre
        Parameters collection = mapQueries.get(key);
        if (collection == null) {
            collection = new Parameters();
            mapQueries.put(key, collection);
        } else {
            collection.addOccurence();
        }
        //collection ne peu pas etre null
        if (!collection.contains(param)) {
            collection.add(param);
        }
    }

    /**
     *Removes or updates a new GoldminerKey in the MapQueries and update some results with collection of Parameter associated.
     *@param id_document the id of a document
     *@param id_dependency the id of a dependency
     *@param id_param the id of a parameter
     *@param id_concept the id of a concept, this id will be removed to the parameter
     *@param id_function  the id of a function (Type or Relation), this id will be removed to the parameter like a constraint
     */
    public synchronized void removeEntryForMapQueries(int id_document, String id_dependency, String id_param, int id_concept, int id_function) {
        //get the parameter
        Parameter param = mapParameter.get(id_param);
        if (param == null) {
            param = new Parameter(id_param);
            mapParameter.put(id_param, param);
        }
        //  param.removeConstraint(id_function); //enlever
        //  param.removeConcept(id_concept); //enlever
        GoldminerKey key = new GoldminerKey(id_document, id_dependency);
        //ajouter la dep pour le parametre
        Parameters collection = mapQueries.get(key);
        if (collection != null) {
            // if ( param.getConstraints().isEmpty()) {
            collection.remove(param);
            collection.removeOccurence();
            //}
            //if ( !collection.isEmpty() && (collection.getOccurence() == 0)) {
            if (collection.isEmpty() || (collection.getOccurence() <= 0)) {
                mapQueries.remove(key);
            }
        }
    }
} //end of class
