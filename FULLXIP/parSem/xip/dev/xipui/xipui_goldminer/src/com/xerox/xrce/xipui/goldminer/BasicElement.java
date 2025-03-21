/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 *
 * BasicElement.java
 *
 * Created on September 6, 2006, 10:58 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer;

/**
 *
 * @author vgrassau
 */
public class BasicElement implements Comparable {

    /** id in a database */
    private int id = -1;
    /** the real name of this element, the name declared in a grammar*/
    private String realName;
    /** the label to present for this element */
    private String label;
    /** some description to explain the label */
    private String description;
    private int weight = 1;

    /** Creates a new instance of BasicElement,
     * the label will have the same value than the realName
     *@param realName the unique name for this element
     *
     */
    public BasicElement(String realName) {
        this.realName = realName;
        label = null;
        //setLabel(realName);
    }

    /**
     *Sets the id, generaly the id is calculted in function of a database
     *@param id the id
     *@see getId()
     */
    public void setId(int id) {
        this.id = id;
    }

    /**
     *Returns the id for this element
     *@return an id, -1 is the default id
     *
     *@see setId(int)
     */
    public int getId() {
        return id;
    }

    /**
     *Returns the Part of Speech of this element
     *@return always 1
     */
    public int getPos() {
        return 1;
    }

    /**
     *Sets the label to use
     *@param label if label is null then when the getLabel() method will be called, the real name will be returned
     *
     *@see getLabel()
     */
    public void setLabel(String label) {
        this.label = label;
    }

    /**
     *Sets the description of this BasicElement
     *@param description the description to set
     *
     *@see getDescription()
     */
    public void setDescription(String description) {
        this.description = description;
    }

    /**
     *Returns the label of this BasicElement
     *@return the label, if no set, returns the realname
     *
     *@see setLabel(String)
     */
    public String getLabel() {
        String lab = null;
        if (label == null) {
            lab = getRealName();
        } else {
            lab = this.label;
        }
        return lab;
    }

    /**
     *Returns the description
     *@return a description, can be null
     *
     *@see setDescription(String)
     */
    public String getDescription() {
        return this.description;
    }

    /**
     *Return the real name of this BasicElement.
     *@return a name of entity, dependency...
     */
    public String getRealName() {
        return this.realName;
    }

    /**
     *set the weight of this basic element
     *@param weight_ the new weight to use
     *
     *@see getWeight()
     */
    public void setWeight(int weight_) {
        this.weight = weight_;
    }

    /**
     *Returns the weitgh of this BasicElement
     *the weight determinates if this BasicElement is more important than an other
     *@return a number , if no set return 1;
     *
     *@see setWeight(int)
     */
    public int getWeight() {
        return this.weight;
    }

    /**
     *Returns a string representation of the BasicElement
     *@return the realname
     *@see getRealName()
     */
    @Override
    public String toString() {
        return getRealName();
    }

    /**
     *Does a comparison of this BasicElement to another obejct
     *the comparison is done on the string representation of both.
     * the code is like this : <code> toString().compareTo(object.toString()) </code>
     *@param object the object to compare
     *@return the value 0 if the argument string is equal to this string; a value less than 0 if this string is lexicographically less than the string argument; and a value
     *        greater than 0 if this string is lexicographically greater than the string argument.
     */
    @Override
    public int compareTo(Object object) {
        return getRealName().compareTo(object.toString());
    }

    /**
     *Check if this BasicElement is equal to the given object
     *@param o object to compare
     *@return true if the given object is a BasicElement and, if its realname is the equals ignoring the case
     */
    public boolean equals(Object o) {
        boolean equal = false;
        if (o instanceof BasicElement) {
            equal = getRealName().equalsIgnoreCase(((BasicElement) o).getRealName());
        }
        return equal;
    }
}
