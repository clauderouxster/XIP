/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 *
 * Concept.java
 *
 * Created on September 26, 2006, 10:15 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer;

import java.util.*;

/**
 * A Concept representation in Goldminer
 * @author vgrassau
 */
public class Concept {

    /** id */
    private int idConcept = -1;
    /** types */
    private Vector<BasicElement> elements;
    private boolean hasConstraint = false;

    /**
     *creates a new Concept with a given id
     *@param idConcept the id for the concept
     */
    public Concept(int idConcept) {
        this.idConcept = idConcept;
        elements = new Vector();
    }

    /**
     *Checks if this concept was constraint on a specific type
     *@return true if it has a constraint, otherwise false
     */
    public boolean hasConstraint() {
        return hasConstraint;
    }

    /**
     *Sets if this concept has to respect a constraint
     *@param b true in order to have a constraint, by default, this attribute is set to false
     */
    public void setHasConstraint(boolean b) {
        this.hasConstraint = b;
    }

    /**
     *Returns the BasicElement for this concept.
     *@return a Collection of BasicElement, can't be null
     */
    public Collection<BasicElement> getElements() {
        return this.elements;
    }

    /**
     *Adds a BasicElement for this Concept
     *@param element the BasicElement to add, if the concept contains the given elment then it will not be added.
     *       if null is given, it will not be added also
     *@return true if the element is added or false if not
     */
    public boolean addIdElement(BasicElement element) {
        boolean success = false;
        if (element != null && !elements.contains(element)) {
            this.elements.add(element);
            success = true;
        }
        return success;
    }

    /**
     *Returns the id of this Concept
     *@return an id
     */
    public int getIdConcept() {
        return this.idConcept;
    }

    /**
     *Checks if this Object is equal to the given Object
     *@param o the object to check the equality
     *@return true if the given object is a Concept and it has the same id, or if the object is an integer equals to
     *        the id of this Concept.
     */
    public boolean equals(Object o) {
        boolean equals = false;
        if (o instanceof Concept) {
            equals = (getIdConcept() == ((Concept) o).getIdConcept());
        } else if (o instanceof Integer) {
            equals = (getIdConcept() == ((Integer) o).intValue());
        }
        return equals;
    }

    /**
     *Returns a String representation of this Concept
     *@return the idConcept concatenates to the BasicElement collection
     */
    public String toString() {
        return " id " + idConcept + " elements : " + elements;
    }
}
