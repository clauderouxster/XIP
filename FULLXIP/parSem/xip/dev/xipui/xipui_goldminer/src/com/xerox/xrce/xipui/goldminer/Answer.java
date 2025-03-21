/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 *
 * Answer.java
 *
 * Created on October 4, 2006, 9:45 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer;

import java.util.*;
import org.apache.log4j.*;

/**
 *
 * @author vgrassau
 */
public class Answer {

    private Logger log = Logger.getLogger("debug");
    private int[] priorityFile;
    //the answser : file -> vector of sentence
    private Map<String, Vector<Sentence>> answerMap;
    private Vector<String> filesSorted;
    private Map<String, Integer> reverseFileMap;
    private Collection idconceptQuery;

    /** Creates a new instance of Answer
     * @param engineMap
     * @param mapFile
     * @param basicElements
     * @param idconceptQuery
     * @param constraints 
     */
    public Answer(Map<GoldminerKey, Parameters> engineMap, Map mapFile, Map<Integer, BasicElement> basicElements, Collection idconceptQuery, Map<Integer, Collection> constraints) {
        answerMap = new Hashtable<String, Vector<Sentence>>();
        priorityFile = new int[mapFile.keySet().size()];
        filesSorted = new Vector<String>();
        reverseFileMap = new Hashtable<String, Integer>();
        this.idconceptQuery = idconceptQuery;
        createAnswer(engineMap, mapFile, basicElements, constraints);
    }

    /**
     *Returns the priority of a file
     *@param filePath the absolute path of the file
     *@return the priority, if the file doesn't found in the answer returns 1 
     */
    public int getPriorityOfFile(String filePath) {
        int priority = 1;
        try {
            Integer source = reverseFileMap.get(filePath);
            if (source != null) {
                priority = this.priorityFile[source];
            }
        } catch (Exception e) {
            log.error("ERROR in answer", e);
            priority = 1;
        }
        return priority;
    }

    /**
     *Returns the answer it means the collection of file matching with the request.
     *@return the files sorted by thier priority.
     */
    public Vector<String> getAnswer() {
        return this.filesSorted;
    }

    /**
     *Returns a Sentence
     *@param file the file where the sentence is
     *@param idsent the id of the sentence
     *@return the sentence, if not found null
     */
    public Sentence getSentence(String file, int idsent) {
        Sentence sentence = null;
        Vector<Sentence> sentences = this.answerMap.get(file);
        if (sentences != null) {
            int index = sentences.indexOf(new Sentence(idsent));
            if (index != -1) {
                sentence = sentences.get(index);
            }
        }
        return sentence;
    }

    /**
     *Returns all the sentences of file.
     *@param file the file to get the sentences
     *@return a collection of Sentence that matche with the request.
     */
    public Collection<Sentence> getTheSentences(String file) {
        return answerMap.get(file);
    }

    /**
     *Returns the sentences of a file in Map with their ids as entry
     *@param file the file to found
     *@return a map with id and sentences or null if the file is not found in the answer
     */
    public Map<Integer, Sentence> getSentences(String file) {
        Collection c = answerMap.get(file);
        Map<Integer, Sentence> map = null;
        if (c != null) {
            map = new Hashtable<Integer, Sentence>();
            Iterator<Sentence> iteSent = c.iterator();
            while (iteSent.hasNext()) {
                Sentence s = iteSent.next();
                map.put(s.getIdSent(), s);
            }
        }
        return map;
    }

    /**
     *Creates the answer
     *@param engineMap, the map queries of the GoldminerEngine
     *@param mapFile, the map of the files
     *@param basicElements the TypeElement and the RelationElement
     *@param constraints the contrainst to apply on the answer
     *
     */
    private void createAnswer(Map<GoldminerKey, Parameters> engineMap, Map mapFile, Map<Integer, BasicElement> basicElements, Map<Integer, Collection> constraints) {
        Set queriesSet = engineMap.keySet();
        //
        for (Iterator<Parameters> it = engineMap.values().iterator(); it.hasNext();) {
            Parameters ps = it.next();
            ps.setConstraint(false);
        }
        //
        //set an iterator of the goldminerkey
        Iterator<GoldminerKey> ite = queriesSet.iterator();
        while (ite.hasNext()) {
            GoldminerKey info = ite.next();
            //get the document
            int idDocument = info.getIdDocument();
            //the sentence
            int idsent = info.getIdsent();
            //the dependency
            String iddep = info.getIdDependency();
            //the parameters found for the key
            Parameters parameters = engineMap.get(info);
            Sentence sentence = null;
            Vector<Sentence> sentences = null;
            String file = (String) mapFile.get(idDocument);
            // if the document is found
            if (file != null) {
                //set the reverserfilemap
                reverseFileMap.put(file, idDocument);
                //we try to found the Vector of Sentence for the file
                if (answerMap.containsKey(file)) {
                    // si la source est deja dans notre reponse
                    sentence = getSentence(file, idsent);
                    if (sentence == null) {
                        sentence = new Sentence(idsent);
                        sentences = answerMap.get(file);
                        sentences.add(sentence);
                    }
                } else {
                    // si la source n'est pas dans la reponse
                    sentences = new Vector<Sentence>();
                    sentence = new Sentence(idsent);
                    sentences.add(sentence);
                    answerMap.put(file, sentences);
                }
                sentence.addDependency(iddep);
                // si dans le requete on recherche un type
                if (isLevelStrong(parameters, idconceptQuery)) {
                    sentence.setLevel(Sentence.STRONG);
                } else if (isLevelMedium(parameters, idconceptQuery)) {
                    sentence.setLevel(Sentence.MEDIUM);
                } else {
                    sentence.setLevel(Sentence.NORMAL);
                }
                //gestion des contraintes et calcul des priorites
                Iterator<Parameter> iteParam = parameters.iterator();
                while (iteParam.hasNext()) {
                    Parameter param = iteParam.next();
                    sentence.addParameter(param);
                    Vector<Integer> constraintParam = param.getConstraints();
                    Iterator<Integer> iteConstParam = constraintParam.iterator();
                    while (iteConstParam.hasNext()) {
                        Integer idBasicElement = iteConstParam.next();
                        //get the basicElement :
                        BasicElement element = basicElements.get(idBasicElement);
                        if (element != null) {
                            if (constraints.isEmpty()) {
                                sentence.addPriority(element.getWeight());
                            } else {
                                if (element instanceof TypeElement) {
                                    TypeElement type = (TypeElement) element;
                                    Collection c = constraints.get(idBasicElement);
                                    if (c != null && !Collections.disjoint(c, param.getConcepts())) {
                                        //constraint respected
                                        sentence.addPriority(element.getWeight() + 5);
                                        param.setConstraintOk(true);
                                    } else {
                                        //constraint not respected
                                        sentence.addPriority(element.getWeight());
                                    }
                                } else {
                                    //relation not a type
                                    sentence.addPriority(element.getWeight());
                                }
                            }
                        }
                    }
                }
                priorityFile[idDocument] += sentence.getPriority();
            }
        }
        createFilesSorted(mapFile);
        log.info("The anwser is created");
    }

    /**
     *Creates the list of the sorted file by priorrity
     *@param mapFile the Map of the files
     */
    private void createFilesSorted(Map<Integer, String> mapFile) {
        HashMap<Integer, Collection> map = new HashMap<Integer, Collection>();
        for (int i = 0; i < priorityFile.length; i++) {
            Collection c = null;
            int priority = priorityFile[i];
            if (priority != 0) {
                if (map.containsKey(priority)) {
                    c = map.get(priority);
                } else {
                    c = new Vector();
                    map.put(priority, c);
                }
                c.add(i);
            }
        }
        Set priorities = map.keySet();
        Object[] keys = priorities.toArray();
        Arrays.sort(keys);
        for (int i = keys.length - 1; i >= 0; i--) {
            Collection c = map.get(keys[i]);
            Iterator<Integer> ite = c.iterator();
            while (ite.hasNext()) {
                filesSorted.add(mapFile.get(ite.next()));
            }
        }
    }

    /**
     *Returns the number of sentence for each level for a file
     *@param file 
     * @return a the number of sentences that match
     */
    public int[] getNumberOfLevelSentences(String file) {
        Collection<Sentence> sentences = this.getTheSentences(file);
        int[] levels = new int[3];
        int strong = 0;
        int medium = 1;
        int normal = 2;
        if (sentences != null) {
            Iterator<Sentence> ite = sentences.iterator();
            while (ite.hasNext()) {
                Sentence s = ite.next();
                int level = s.getLevel();
                switch (level) {
                    case Sentence.STRONG: {
                        levels[strong]++;
                        break;
                    }
                    case Sentence.MEDIUM: {
                        levels[medium]++;
                        break;
                    }
                    case Sentence.NORMAL: {
                        levels[normal]++;
                        break;
                    }
                }
            }
        }
        return levels;
    }

    /**
     *Checks if a parameters collection corresponding to a strong level
     *@param parameters the collection of parameters
     *@param ref the condition to respect, the ids of concept to search.
     *@return true if conditions are respected false otherwise
     */
    private boolean isLevelStrong(Parameters parameters, Collection<Integer> ref) {
        Vector ref_ = new Vector(ref);
        int countConceptInParam = 0;
        Iterator<Parameter> ite = parameters.iterator();
        int cpt = 0;
        while (ite.hasNext()) {
            Parameter p = ite.next();
            if (!p.isCoreferent()) {
                Iterator<Integer> iteConcept = p.getConcepts().iterator();
                while (iteConcept.hasNext()) {
                    Integer idConcept = iteConcept.next();
                    if (ref_.remove(idConcept)) {
                        cpt += 1;
                    }
                }
            }
        }
        return cpt >= ref.size();
    }

    /**
     *Checks if a parameters collection corresponding to a medium level
     *@param parameters the collection of parameters
     *@param ref the condition to respect, the ids of concept to search.
     *@return true if conditions are respected false otherwise
     */
    private boolean isLevelMedium(Parameters parameters, Collection<Integer> ref) {
        Vector ref_ = new Vector(ref);
        int min = 0;
        Iterator<Parameter> ite = parameters.iterator();
        while (ite.hasNext() && min <= 2) {
            Parameter p = ite.next();
            Iterator<Integer> iteConcept = p.getConcepts().iterator();
            while (iteConcept.hasNext()) {
                Integer idConcept = iteConcept.next();
                if (ref_.remove(idConcept)) {
                    min += 1;
                }
            }
        }
        return min >= 2;
    }
} 
