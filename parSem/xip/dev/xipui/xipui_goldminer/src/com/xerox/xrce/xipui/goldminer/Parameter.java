/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 *
 * Parameter.java
 *
 * Created on December 12, 2006, 10:51 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer;

import java.util.*;

/**
 *
 * @author vgrassau
 */
public class Parameter {

    private boolean coreferent = false;
    private String id = null;
    /** the concept according to this parameter */
    private Vector<Integer> concepts = null;
    /** the functions according to this parameter */
    private Vector<Integer> constraints = null;
    private boolean constraintOk = false;

    /**
     *Creates a new instance of Parameter with an empty
     *collection of concepts and constraints
     *@param id the id to identify this parameter
     */
    public Parameter(String id) {
        this.id = id;
        concepts = new Vector();
        constraints = new Vector();
    }

    /**
     *Sets this paramter as a coreferent
     *@param b true for coreferent false otherwise
     */
    public void setCoreferent(boolean b) {
        this.coreferent = b;
    }

    /**
     *Returns if this parameter is a coreferent of a concept
     *@return true if coreferent
     */
    public boolean isCoreferent() {
        return this.coreferent;
    }

    /*
     *
     *Returns the id of the parameter
     *@return the string id
     */
    public String getId() {
        return this.id;
    }

    /**
     *Compares the id string to the string representation of the given object
     *@param o 
     * @return true if the id and the string repesentation are equals, false otherwise
     *
     *NB if o is null return false
     */
    public boolean equals(Object o) {
        if (o != null) {
            return id.equals(o.toString());
        } else {
            return false;
        }
    }

    /**
     *Add a function constraint to the parameter
     *if the parameter already contains the given function, the function will not be added
     *@param function the function to add
     *
     */
    public void addConstraint(int function) {
        if (!constraints.contains(function)) {
            constraints.add(function);
        }
    }

    /**
     *Sets if this parameter respects a constraint
     *@param b
     */
    public void setConstraintOk(boolean b) {
        this.constraintOk = b;
    }

    /**
     *Checks if this parameter respects a constraint
     *@return true if the constraint is respected
     */
    public boolean isConstraintOk() {
        return this.constraintOk;
    }

    /**
     *Removes a constraint
     *@param function the id of the function (constraint) to remove
     */
    public void removeConstraint(int function) {
        constraints.remove(new Integer(function));
    }

    /**
     *Removes a concept
     *@param id_concept the id of the concept to remove
     */
    public void removeConcept(int id_concept) {

        concepts.remove(new Integer(id_concept));

    }

    /**
     *Adds a concept, only the non conains concept are added
     *@param id_concept the concept to add
     *
     */
    public void addConcept(int id_concept) {
        if (!concepts.contains(id_concept)) {
            concepts.add(id_concept);
        }
    }

    /**
     *Returns the collection of constraint according to this parameter.
     *@return the collection of the constaints ids, or an empty collection if no constraint
     */
    public Vector<Integer> getConstraints() {
        return this.constraints;
    }

    /**
     *Returns the collection of the concepts
     *@return the collection of the ids of the concepts
     */
    public Vector<Integer> getConcepts() {
        return this.concepts;
    }
}
