/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 *
 * GoldminerDataWriter.java
 *
 * Created on January 17, 2007, 5:58 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer;

import java.util.*;
import java.io.*;
import jcoref.*;
import com.xerox.jxip.*;
import com.xerox.xrce.xipui.Project;
import org.apache.log4j.Logger;

/**
 * This class should be instanciate by the GoldminerEngine object.
 * It permits to read xip analysis and coreference results given by the JCOREF API.
 *
 * @author vgrassau
 */
public class GoldminerDataWriter {

    private Logger log = Logger.getLogger("debug");
    private GoldminerDataBase dataBase;
    // the bad categories for leaves.

    enum BAD_CATEGORY {

        PUNCT, DEP, PEID, PENORM, FEAT, MARKUP, ATTRIBUTE, DET, PREP
    };
    // the function and the entities to store
//    enum FUNCTION {
//        DATE, YEAR, HOUR, TEMP,
//        LOCATION, ORGANISATION, PRODUCT, PROFESSION, EMAIL, PERSON,
//        URL, PHONE, PERCENT, MONEY, LOCORG, PRIZE,
//        EVENT, AGENT, PATIENT, SAY, DETD,
//        SUBJ, SUBJ_N, OBJ, OBJ_N, MOD, MAIN,
//        //add for risk grammar
//        COORDINATE, COORDINATE_ACTOR, COORDINATE_TARGET, COORDINATE_TIME, COORDINATE_LOC_PR,
//        CORE, CORE_TOPIC, CORE_INFORMATION, CORE_DRIVE, INFO_TOPIC, FACTUAL, POSSIBLE, FACTUAL_SRC, POSSIBLE_SRC, EXTENSION, MODALITIES,
//        //add french 
//        PERSONNE, PERIODE, CELEB, VILLE, PAYS, REGION, LIEU, QUARTIER, BATIMENT, BATIMENTORG, ORG, LABEL
//    };
    // the result of the coreference
    private ReferentNodeList referents;
    // the result of XIP
    private Vector<SentenceParse> sentences;
    //the table of all concept stored in the database
    // this table can be huge
    private Hashtable<String, Integer> concepts;
    //list of functions names extract from the grammar
    private Vector<String> functionNames;
    //the table of all function stored in the database
    //this table is not huge, it depends of the grammar it's  about 300 if the
    //all the functions are stored.  
    private Hashtable<String, Integer> functions;
    //the table of all parameters found in a parsed file
    private Hashtable<String, Vector<Integer>> key_parameters;
    //the current id corpus
    private int ID_CORPUS = 0;
    //the current id docuement
    private int ID_DOCUMENT = 0;
    //the  current id sentence
    private int ID_SENTENCE = 0;
    //the current id concept
    private int ID_CONCEPT = 0;
    //the current id function
    private int ID_FUNCTION = 0;

    /** Creates a new instance of GoldminerDataWriter
     * @param dataBase
     * @param project
     */
    public GoldminerDataWriter(GoldminerDataBase dataBase, Project project) {
        this.dataBase = dataBase;
        concepts = dataBase.getConcepts();
        functions = dataBase.getFunctions();
        key_parameters = new Hashtable<String, Vector<Integer>>();
        JXip jxip = project.getXip().getJXip();
        int gHandler = project.getGhandler();
        Vector<GramDependency> gramDependencies = new Vector<GramDependency>();
        try {
            gramDependencies = project.getXip().getListDependencies(gHandler);
        } catch (Exception ex) {
            log.error(ex);
        }
        functionNames = new Vector<String>();
        for (GramDependency gramDep : gramDependencies) {
            functionNames.add(gramDep.getName());
        }
//        System.out.println("\n+++++++++++++++++++++++++++++\n" + functionNames.toString() + "\n+++++++++++++++++++++++++++++\n");
    }

    /**
     *Store the analysis of XIP and the coreference result
     *@param file the file parsed
     * @param resolver 
     */
    public void write(File file, CorefResolver resolver) {
        initWriter();
        ID_CORPUS = dataBase.addCorpus(file, ID_CORPUS);
        if (dataBase.addDocument(file, ID_CORPUS, ID_DOCUMENT) == ID_DOCUMENT) {
            //ID_DOCUMENT = dataBase.addDocument(file,ID_CORPUS,ID_DOCUMENT);
            log.info("Parsing of " + file.getName());
            long startParse = System.currentTimeMillis();
            this.referents = resolver.getCoreferenceNodesFromFile(file.getAbsolutePath(), false);
            long endParse = System.currentTimeMillis();
            log.info("Parsed in " + (endParse - startParse) + " ms");
            long start = System.currentTimeMillis();
            readSentences(resolver.getSentenceParses());
            readReferents();
            long end = System.currentTimeMillis();
            System.out.println("\tLecture  : " + (end - start) + " ms");
            start = System.currentTimeMillis();
            dataBase.store();
            end = System.currentTimeMillis();
            System.out.println("\tStockage : " + (end - start) + " ms");
            //we clear the sentences and the referents
            sentences.clear();
            referents.clear();
        } else {
            log.info(file.getName() + " already parsed and stored in the database");
        }
    }

    /**
     *Get the entities of a XipNode
     *@param n the XipNode to get the enties.
     *@return a Vector of integer, the integer are the id of the functions entity
     */
    private Vector<Integer> getEntity(XipNode n) {
        Vector<Integer> entities = new Vector<Integer>();
        Vector<XipDependency> dep = n.getDependencies();
        Iterator<XipDependency> ite = dep.iterator();
        while (ite.hasNext()) {
            XipDependency dependency = ite.next();
            if (dependency.getParameters().size() == 1) {
                String name = dependency.getName();
                Integer id_function = this.functions.get(name);
                if (id_function != null && !entities.contains(id_function)) {
                    entities.add(id_function);
                }
            }
        }
        return entities;
    }

    /** used for coreference result
     * get the concept used in parameters to link between  the antecedent and the coreferent
     *@param keyAntecedent the key of the antecedent parameter
     *@param keyCoreferent the key of the coreferent parameter
     *@return the list of the idconcept to add for the coreferent param.
     */
    private Vector<Integer> getConceptCoreferent(String keyAntecedent, String keyCoreferent) {
        Vector<Integer> result = new Vector<Integer>();
        try {
            Vector<Integer> corefConcept = key_parameters.get(keyCoreferent);
            Vector<Integer> corefAntecedent = key_parameters.get(keyAntecedent);
            if (corefAntecedent != null) {
                result.addAll(corefAntecedent);
            }
            if (corefConcept != null) {
                Iterator<Integer> iteCoref = corefConcept.iterator();
                while (iteCoref.hasNext()) {
                    Integer id = iteCoref.next();
                    result.remove(id);
                }
            }
        } catch (Exception e) {
            //if erro we return an empty collection
            log.error(e);
            result = new Vector<Integer>();
        }
        return result;
    }

    /**
     *like getConceptCoreferent, this method return the id function to add for a coreferent node,
     *the id came from the antecedent node
     *@param antecedent the antecedent node
     *@param coreferent the coreferent node
     *@return a collection of the id function to add
     *
     */
    private Vector<Integer> getEntityCoreferent(ReferentNode antecedent, ReferentNode coreferent) {
        Vector<Integer> result = new Vector<Integer>();
        try {
            Vector antecedentEntities = getEntity(antecedent.getNode());
            Vector referentEntities = getEntity(coreferent.getNode());
            result.addAll(antecedentEntities);
            result.removeAll(referentEntities);
        } catch (NullPointerException e) {
            e.printStackTrace();
            result = new Vector<Integer>();
        }
        return result;
    }

    /**
     *This method is called by readReferent
     *write the information given by the referent node,
     *the function  (see getEntityCoreferent())
     *the concept  (see getConceptCoreferent())
     *@param corefNode the coreferent node
     *@param refNode the antecedent node
     *@param keyAntecedent the key of the antecedent
     */
    private void readCoreferent(ReferentNode corefNode, ReferentNode refNode, String keyAntecedent) {
        if (corefNode.isCoreferent()) {
            long id_sent = ((ID_SENTENCE - sentences.size()) + corefNode.getSentenceId()) - 1;
            XipNode n = corefNode.getNode();
            String keyCoref = n.getIndex() + "_" + id_sent;
            dataBase.addCoreferenceParameter(keyCoref, keyAntecedent, id_sent);
            //add the entity of the antecedent linked to the coreferent node
            Vector<Integer> entitiesToAdd = getEntityCoreferent(refNode, corefNode);
            Iterator<Integer> iteEntity = entitiesToAdd.iterator();
            while (iteEntity.hasNext()) {
                dataBase.addFunctionParameter(keyCoref, iteEntity.next(), id_sent);
            }
            //ad the concept of the antecedent linked to the coreferent node
            Vector<Integer> conceptsToAdd = getConceptCoreferent(keyAntecedent, keyCoref);
            Iterator<Integer> iteConcept = conceptsToAdd.iterator();
            while (iteConcept.hasNext()) {
                dataBase.addConceptParameter(keyCoref, iteConcept.next(), id_sent);
            }
        }
    }

    /**
     *Read the referent result
     *@see readCoreferent
     */
    private void readReferents() {
        Iterator<ReferentNode> ite = referents.iterator();
        int numberSentence = sentences.size();
        while (ite.hasNext()) {
            ReferentNode antecedentNode = ite.next();
            if (antecedentNode.isAntecedent()) {
                Referent referent = antecedentNode.getReferent();
                if (referent != null) {
                    long id_sent_antecedent = ((ID_SENTENCE - numberSentence) + antecedentNode.getSentenceId()) - 1;
                    //calculate the antecedent parameter.
                    String keyAntecedent = antecedentNode.getNode().getIndex() + "_" + id_sent_antecedent;
                    ReferentNodeList list = referent.getReferentNodes();
                    try {
                        Iterator<ReferentNode> iteList = list.iterator();
                        while (iteList.hasNext()) {
                            readCoreferent(iteList.next(), antecedentNode, keyAntecedent);
                        }
                    } catch (NullPointerException e) {
                        System.out.println("no referent nodes for " + referent.getId());
                    }
                }
            }
        }
        //clear all the parameters
        key_parameters.clear();
    }

    /**
     *Init the datawriter
     *
     */
    private void initWriter() {
        //this.sentences = sentences;
        //this.referents = referents_;
        ID_CORPUS = dataBase.createIdCorpus();
        ID_DOCUMENT = dataBase.createIdDocument();
        ID_SENTENCE = dataBase.createIdSentence();
        ID_CONCEPT = dataBase.createIdConcept();
        ID_FUNCTION = dataBase.createIdFunction();
        key_parameters = new Hashtable<String, Vector<Integer>>();
    }

    /**
     *Read the sentences i.e. the XipResult and store it in a database
     * @param sentences 
     */
    private void readSentences(Vector<SentenceParse> sentences) {
        this.sentences = sentences;
        Iterator<SentenceParse> iteSentence = sentences.iterator();
        while (iteSentence.hasNext()) {
            SentenceParse sentence = iteSentence.next();
            dataBase.addSentence(ID_SENTENCE, ID_DOCUMENT, sentence.getLeft(), sentence.getRight());
            writeTheConcept(sentence.getLeaves());
            writeTheDependencies(sentence.getDependencies());
            ID_SENTENCE++;
        }
    }

//    /**
//     *Check if we want to store the givent function
//     *@param name the name of the XipDependency
//     *
//     */
//    private boolean acceptFunction(String name) {
//        boolean ok = false;
//        try {
//            String fct = name.replaceAll("-", "_");
//            //System.out.println(name + " == " + fct);
//            FUNCTION.valueOf(fct.toUpperCase());
//            ok = true;
//        } catch (Exception e) {
//            ok = false;
//        }
//        return ok;
//    }
    /**
     * Check if we want to store the givent function
     * @param name the name of the XipDependency
     * @return
     */
    private boolean acceptFunction(String name) {
        if (functionNames.contains(name)) {
            return true;
        } else {
            return false;
        }
    }

    private void writeTheDependencies(Vector<XipDependency> dependencies) {
        Iterator<XipDependency> iteDep = dependencies.iterator();
        while (iteDep.hasNext()) {
            XipDependency dependency = iteDep.next();
            String function = dependency.getName();
            if (acceptFunction(function)) {
                //on cherche dans notre table si la fonction est presente
                Integer f_id = functions.get(function);
                if (f_id == null) {
                    //si non present on cherche dans la base
                    //nouvelle fonction
                    f_id = ID_FUNCTION;
                    ID_FUNCTION++;
                    functions.put(function, f_id);
                }
                dataBase.addFunction(f_id, function);
                String keyDep = dependency.getIndex() + "_" + ID_SENTENCE;
                Vector<Integer> listConcept = writeTheParameters(keyDep, f_id, dependency.getParameters());
                dataBase.addDependency(keyDep, ID_SENTENCE, f_id, listConcept, dependency.getParameters().size());
            }
        }
    }

    private void writeConceptFromParameter(String id_param, XipNode node, Vector<Integer> listConcept) {
        if (node.isPreTerminalNode()) {
            Iterator<XipNode> leaves = node.getDaughters().iterator();
            XipNode previous = null;
            while (leaves.hasNext()) {
                XipNode leaf = leaves.next();
                if (acceptLeaf(leaf, previous)) {
                    Integer idLemma = concepts.get(node.getLemma().toLowerCase());
                    if (idLemma != null) {
                        if (!listConcept.contains(idLemma)) {
                            listConcept.add(idLemma);
                        }
                        dataBase.addConceptParameter(id_param, idLemma, ID_SENTENCE);
                    }
                }
                previous = leaf;
            }
        } else {
            Iterator<XipNode> leaves = node.getDaughters().iterator();
            while (leaves.hasNext()) {
                writeConceptFromParameter(id_param, leaves.next(), listConcept);
            }
        }
    }

    private void noDoublon(Vector container, Vector toAdd) {
        Iterator ite = toAdd.iterator();
        while (ite.hasNext()) {
            Object object = ite.next();
            if (!container.contains(object)) {
                container.add(object);
            }
        }
    }

    private Vector<Integer> writeTheParameters(String id_dep, int f_id, Vector<XipNode> parameters) {
        Vector<Integer> listConcept = new Vector<Integer>();
        Iterator<XipNode> iteParam = parameters.iterator();
        while (iteParam.hasNext()) {
            XipNode node = iteParam.next();
            if (!node.isFicticious()) {
                String keyParam = node.getIndex() + "_" + ID_SENTENCE;
                if (!key_parameters.containsKey(keyParam)) {
                    writeConceptFromParameter(keyParam, node, listConcept);
                    key_parameters.put(keyParam, listConcept);
                } else {
                    noDoublon(listConcept, key_parameters.get(keyParam));
                }
                dataBase.addParameter(keyParam, node.getLeft(), node.getRight());
                dataBase.addDependencyParameter(id_dep, keyParam, f_id);
                dataBase.addFunctionParameter(keyParam, f_id, ID_SENTENCE);
            }
        }
        return listConcept;
    }

    private boolean acceptLeaf(XipNode leaf, XipNode previousLeaf) {
        boolean ok = false;
        if (previousLeaf == null || previousLeaf.getLeft() != leaf.getLeft() || previousLeaf.getRight() != leaf.getRight()) {
            if (!leaf.isFicticious()) {
                //pas de noeuds fictifs
                try {
                    BAD_CATEGORY.valueOf(leaf.getCategory().toUpperCase());
                // System.out.println(leaf.getCategory() + " not allowed");
                } catch (Exception e) {
                    ok = true;
                }
            }
        }
        return ok;
    }

    private void writeTheConcept(Vector<XipLeaf> leaves) {
        Iterator<XipLeaf> iteLeaves = leaves.iterator();
        XipLeaf previous = null;
        while (iteLeaves.hasNext()) {
            XipLeaf leaf = iteLeaves.next();
            if (acceptLeaf(leaf, previous)) {
                String lemma = leaf.getLemma().toLowerCase();
                lemma = lemma.replaceAll("'", "");
                if (lemma.length() > 100) {
                    //bad input for the parser => some lemma contains a sequence of same char (ex: dejeuner====================)
                    lemma = lemma.substring(0, 100 - 1);
                }
                //on cherche dans notre table si le concept est present
                Integer l_concept = concepts.get(lemma);
                if (l_concept == null) {
                    l_concept = ID_CONCEPT;
                    ID_CONCEPT++;
                    concepts.put(lemma, l_concept);
                }
                //maj de la base de donnees
                dataBase.addConcept(lemma, l_concept);
            //dataBase.addConceptSurface(leaf.getIndex(),ID_SENTENCE, l_concept,leaf.getLeft(),leaf.getRight());
            }
            previous = leaf;
        }
    }
}
