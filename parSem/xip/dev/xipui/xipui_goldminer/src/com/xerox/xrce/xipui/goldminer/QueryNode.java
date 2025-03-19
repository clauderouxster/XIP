/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 *
 * QueryNode.java
 *
 * Created on September 26, 2006, 10:15 AM
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
public class QueryNode {

    /** the lemma */
    private String lemma;
    /** the defauld id for a lemma */
    private int idLemma = -1;
    /** the list of types (list of ids) */
    private Vector<Integer> listTypeElement;
    /** the selected index type in the list not the id of the type */
    private int selected = 0;
    private Logger log = Logger.getLogger("debug");

    /* Creates a new instance of QueryNode
     * The lemma is null, the list of type is empty
     */
    public QueryNode() {
        lemma = null;
        this.listTypeElement = new Vector();
    }

    /**
     *Check if the QueryNode is empty.
     *A QueryNode is empty if the lemma is null or, the idLemma is equal to -1
     *@return true is empty, false otherwise
     */
    public boolean isEmpty() {
        return (lemma == null || idLemma == -1);
    }

    /**
     *Sets the lemma to search
     *@param idconcept the id for the lemma
     *@param lemma the lemma
     */
    public void setLemma(int idconcept, String lemma) {
        this.lemma = lemma;
        this.idLemma = idconcept;
    }

    /**
     *Returns the lemma
     *@return lemma
     */
    public String getLemma() {
        return this.lemma;
    }

    /**
     *Returns the id of the lemma in this QueryNode
     *@return an id -1 if no lemma or means an unknown id
     */
    public int getIdLemma() {
        return this.idLemma;
    }

    /**
     *Returns the list of the available types for this QueryNode.
     *The collection contains the ids of each types.
     *@return Collection of ids can be empty
     */
    public Vector<Integer> getListTypeElement() {
        return this.listTypeElement;
    }

    /**
     *Returns the selected type.By default the selected type is the first element of the
     *type list in the QueryNode.
     *@return the id of the selected type for this QueryNode.if problem was occured, returns -10
     */
    public Integer getSelectedTypeElement() {
        try {
            return this.listTypeElement.get(selected);
        } catch (Exception e) {
            log.error("Problem selection du TypeElement a la position : " + selected, e);
            return -10;
        }
    }

    /**
     *Sets the list of TypeElement for this QueryNode
     *if a list existed then it is cleaned before the set
     *@param listTypeElement_ the list of TypeElement
     */
    public void setListTypeElement(Collection<Integer> listTypeElement_) {
        this.listTypeElement.clear();
        this.listTypeElement.addAll(listTypeElement_);
        this.selected = 0;
    }

    /**
     *Sets the selected TypeElement. if the given id is not found, the selected type will be the first TypeElement
     *in the list
     *
     *@param idForType an id of TypeElement.
     */
    public void setSelectedTypeElement(int idForType) {
        selected = listTypeElement.indexOf(idForType);
        if (selected == -1) {

            selected = 0;
        }
    }

    /**
     *Returns a String representation of the QueryNode
     *@returns, the lemma and  its id and the list of ids of TypeElement
     */
    public String toString() {
        StringBuilder s = new StringBuilder();
        s.append("lemma : " + getIdLemma() + "," + getLemma());
        s.append("\n types :\n");
        s.append(this.listTypeElement);
        return s.toString();
    }
}//end of class
