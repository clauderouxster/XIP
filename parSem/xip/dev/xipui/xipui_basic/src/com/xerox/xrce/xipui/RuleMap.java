/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 ****************************************************************************************
 * Copyright (c) 2005 Xerox Corporation                                                 *
 * Xerox Research Centre Europe - Grenoble                                              *
 * All Rights Reserved                                                                  *
 * Copyright protection claimed includes all forms and matters of copyrightable         *
 * material and information now allowed by statutory or judicial law or                 *
 * hereinafter granted, including without limitation, material generated from           *
 * the software programs which are displayed on the screen such as icons,               *
 * screen display looks, etc. All rights reserved. No part of this document             *
 * may be reproduced, stored in a retrieval system, or transmitted, in any              *
 * form or by any means, electronic, mechanical, photocopied, recorded, or              *
 * otherwise, without prior written consent from XEROX corporation.                     *
 ****************************************************************************************
 */
package com.xerox.xrce.xipui;

import com.xerox.jxip.*;
import java.util.*;
import java.io.*;

/**
 *
 * @author Vianney Grassaud
 */
public class RuleMap {

    private RuleLoadedManager ruleManager;
    private Map<Object, Vector<RuleLoaded>> currentTable;

    /** constructeur par defaut */
    public RuleMap() {
        ruleManager = new RuleLoadedManager();
        currentTable = new Hashtable();
    }

    /** Initialisation du RuleMap
     *@param gHandler numero de la grammaire chargee
     */
    public void init(int gHandler) {
        ruleManager.init(gHandler);
        currentTable.clear();
    }

    public int getRuleMax() {
        return ruleManager.getRuleCounter() - 1;
    }

    public int getCountRules() {
        return ruleManager.getRuleCounter();
    }

    /**
     *Return the vector of RuleLoaded referenced in a file
     *@param file the file to search the rules
     *@return a Vector of RuleLoaded or an empty vector
     */
    public Vector<RuleLoaded> getRulesFromFile(String file) {
        Map<Integer, RuleLoaded> rules = ruleManager.getRulesByRuleNb();
        Vector<RuleLoaded> res = new Vector();
        Iterator<RuleLoaded> ite = rules.values().iterator();
        while (ite.hasNext()) {
            RuleLoaded rule = ite.next();
            String fileName = rule.getFileName();
            if (fileName.equals(file)) {
                res.add(rule);
            }
        }
        return res;
    }

    public boolean isAFileLimit(int limit, File file) {
        boolean success = false;
        File fileLimit = this.getFileByRuleNb(limit);
        if (fileLimit != null) {
            success = fileLimit.equals(file);
        }
        return success;
    }

    /**
     *Return the RuleLoaded identified with the given rule number
     *@param numRule the rule number to find
     *@return a RuleLoaded if the rule exists null otherwise
     */
    public RuleLoaded getRuleLoaded(int numRule) {
        return ruleManager.getRulebyRuleNb(numRule);
    }

    /**
     *Return the File that contains the given rule number
     *@param numRule the rule number to find
     *@return a File if the rule exists null otherwise
     */
    public File getFileByRuleNb(int numRule) {
        File ruleFile = null;
        RuleLoaded rule = getRuleLoaded(numRule);
        if (rule != null) {
            ruleFile = rule.getFile();
        }
        return ruleFile;
    }

    /**
     *Return the offset of the given rule number
     *@param numRule the rule number to find
     *@return a long representing the offset if the rule was found or 0 if it was not found
     *
     */
    public long getOffsetByRuleNb(int numRule) {
        long offset = 0;
        RuleLoaded rule = getRuleLoaded(numRule);
        if (rule != null) {
            offset = rule.getOffset();
        }
        return offset;
    }

    /**
     *Return a Vector of key sorted from the current table.
     *The current table must have to contains keys that implement Comparable...
     *@return a vector of object sorted
     */
    public Vector getKeysSorted() {
        Vector sorted = new Vector(currentTable.keySet());
        Collections.sort(sorted);
        return sorted;
    }

    /**
     *Add a RuleLoaded with the given key to the current table
     *@param key the key to use for the table
     *@param rule the RuleLoaded associasted to the key
     *
     */
    private void completeTable(Object key, RuleLoaded rule) {
        Vector<RuleLoaded> v = currentTable.get(key); // on regarde si on a deja inserer la regle pour la cle
        if (v == null) {
            // on crer la nouvelle entree dans la table et on insere la regle
            v = new Vector();
            v.add(rule);
            currentTable.put(key, v);
        } else {
            //sinon on insere uniquement la regle
            v.add(rule);
        }
    }

    /**
     *Renvoie le rule map classe par le type des regles
     *@return une table de hachage indexee par les types de regles utilises contenant des Vecteur de RuleLoaded
     */
    public Map<Object, Vector<RuleLoaded>> getTypeTable() {
        for (Iterator<RuleLoaded> it = getTheRuleLoaded().iterator(); it.hasNext();) {
            RuleLoaded rule = it.next();
            String type = rule.getStringRuleType();
            completeTable(type, rule);
        }
        return currentTable;
    }

    /**
     *Renvoie le rule map classe par les couches relatives des regles
     *@return une table de hachage indexee par les couches relatives des regles utilisees contenant des Vecteur de RuleLoaded
     */
    public Map<Object, Vector<RuleLoaded>> getLayerRelTable() {
        for (Iterator<RuleLoaded> it = getTheRuleLoaded().iterator(); it.hasNext();) {
            RuleLoaded rule = it.next();
            Integer relLayer = new Integer(rule.getRelLayer());
            completeTable(relLayer, rule);
        }
        return currentTable;
    }

    /**
     *Renvoie le rule map classe par les couches absolues des regles
     *@return une table de hachage indexee par les couches absolues des regles utilisees contenant des Vecteur de RuleLoaded
     */
    public Map<Object, Vector<RuleLoaded>> getLayerAbsTable() {
        for (Iterator<RuleLoaded> it = getTheRuleLoaded().iterator(); it.hasNext();) {
            RuleLoaded rule = it.next();
            Integer absLayer = new Integer(rule.getAbsLayer());
            completeTable(absLayer, rule);
        }
        return currentTable;
    }

    /**
     *Methode appelee pour parcourir les regles et reconstruire la table courrente
     *@return une enumeration de RuleLoaded
     */
    private Collection<RuleLoaded> getTheRuleLoaded() {
        currentTable.clear();
        Map<Integer, RuleLoaded> rules = ruleManager.getRulesByRuleNb();
        return rules.values();
    }

    /**
     *Renvoie le rule map classe par les fichiers des regles
     *@return une table de hachage indexee par les fichiers des regles utilises contenant des Vecteur de RuleLoaded
     */
    public Map<Object, Vector<RuleLoaded>> getFileTable() {
        for (Iterator<RuleLoaded> it = getTheRuleLoaded().iterator(); it.hasNext();) {
            RuleLoaded rule = it.next();
            String fileName = rule.getFileName();
            completeTable(fileName, rule);
        }
        return currentTable;
    }

    /**
     *Renvoie le rule map classe par les numeros des regles
     *@return une table de hachage indexee par les numeros des regles utilises contenant un RuleLoaded
     */
    public Map<Object, Vector<RuleLoaded>> getNumberTable() {
        for (Iterator<RuleLoaded> it = getTheRuleLoaded().iterator(); it.hasNext();) {
            RuleLoaded rule = it.next();
            int number = rule.getRuleNb();
            completeTable(number, rule);
        }
        return currentTable;
    }

    private Vector<RuleLoaded> sortRulesByOffset(Vector rules_) {
        Vector rules = new Vector();
        Iterator ite = rules_.iterator();
        while (ite.hasNext()) {
            RuleLoaded rule = (RuleLoaded) ite.next();
            Long offset = new Long(rule.getOffset());
            int indexToInsert = 0;
            boolean stop = false;
            for (int i = 0; i < rules.size() && !stop; i++) {
                RuleLoaded rule2 = (RuleLoaded) rules.get(i);
                if (offset.compareTo(rule2.getOffset()) > 0) {
                    indexToInsert = i;
                    stop = true;
                } else {
                    indexToInsert = i + 1;
                }
            }
            rules.add(indexToInsert, rule);
        }
        return rules;
    }

    /**
     *Return a rule number from a given filename and a given offset
     *@param fileName 
     * @param offset 
     * @return the rule number if it was found otherwise returns -1
     */
    public int getRuleNumber(String fileName, long offset) {
        offset += 1;
        Vector<RuleLoaded> rules = this.sortRulesByOffset(this.getRulesFromFile(fileName));
//        boolean stop = false;
//        int theRuleNumber = -1;
//        for (int i=0; i < rules.size() && !stop ; i++) {
//            RuleLoaded rule  = rules.get(i);
//            if ( i == 0) {
//                if ( start >= rule.getOffset()) {
//                    theRuleNumber =  rule.getRuleNb();
//                    stop = true;
//                }
//            } else {
//                RuleLoaded prec = rules.get(i-1);
//                if ( start < prec.getOffset() && start >= rule.getOffset()) {
//                    theRuleNumber =  rule.getRuleNb();
//                    stop = true;
//                }
//            }
//
//        }
//      return theRuleNumber;
        boolean stop = false;
        int number_ = -1;
        for (int i = rules.size() - 1; i > 0 && !stop; i--) {
            RuleLoaded rule = rules.get(i);
            RuleLoaded rulePrec = rules.get(i - 1);
            int start = (int) rule.getOffset();
            int end = (int) rulePrec.getOffset();
            if (offset >= start && offset <= end) {
                stop = true;
                number_ = rule.getRuleNb();
            }
        }
        if (rules.size() > 0) {
            RuleLoaded lastRule = rules.get(0);
            int startOfLastRule = (int) lastRule.getOffset();
            if (offset >= startOfLastRule) {
                number_ = lastRule.getRuleNb();
            }
        }
        return number_;
    }
}
