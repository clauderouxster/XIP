/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 ****************************************************************************************
 * Copyright (c) 2006 Xerox Corporation                                                 *
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

import java.util.*;
import org.apache.log4j.*;
import java.io.*;

/**
 *
 * @author Vianney Grassaud
 */
public class Specialisations {

    /** the differents available types */
    static public final short LEXICON = 0;
    static public final short PARAMETER = 1;
    static public final short DECLARATION = 2;
    static public final short RULE = 3;
    static public final short NO_TYPE = 4;
    static public final short SCRIPTS = 5;
    /** the list of Specialisation elements */
    private Vector<Specialisation> list;
    /** the current type used for the set of Specialisation */
    private short type;
    /** the log */
    private Logger log;

    /**
     *Create an empty set of Specialisation 
     */
    public Specialisations() {
        log = Logger.getLogger("grammar");
        list = new Vector();
        setType(NO_TYPE);
    }

    /**
     *Creates a set of Specialisation elements from an Enumeration of a String
     *@param e an Enumeration of path
     *@param type the type to use for this set
     *@param crypted indicate if the Specialisation for this set are crypted or not.
     *@see setList(Enumeration,boolean)
     *@see setType(short)
     */
    public Specialisations(Enumeration e, short type, boolean crypted) {
        this();
        setType(type);
        setList(e, crypted);
    }

    public Iterator<Specialisation> iterator() {
        return list.iterator();
    }

    /**
     *Set the type of this set of Specialisation
     *@param type the type to set
     */
    public void setType(short type) {
        this.type = type;
    }

    /**
     *Return the type of this set of Specialisation
     *@return a short number 
     */
    public short getType() {
        return this.type;
    }

    /**
     *Create the list from an Enumeration of String.
     *@param e the Enumeration of String like a path.
     *@param crypted  indicates if all the Specialisation are crypted or not
     *@see addSpecialisation(Specialisation)
     */
    public void setList(Enumeration e, boolean crypted) {
        while (e.hasMoreElements()) {
            Object o = e.nextElement();
            //we can create a Specialisation only if the enumeration contains String element
            //the string should be a path of a file
            if (o instanceof String) {
                Specialisation spe = new Specialisation((String) o);
                if (getType() == PARAMETER) {
                    spe.setParameter(true);
                }
                spe.setCrypted(crypted);
                addSpecialisation(spe);
            }
        }
    }

    /**
     *Add a Specialisation element in the list. If the set already contains the given argument then 
     *the element won't be added
     *@param s specialisation to add
     */
    public void addSpecialisation(Specialisation s) {
        if (!list.contains(s)) {
            list.add(s);
        }
    }

    /**
     *Return the last added Specialisation in the set
     *@return the last Specialisation or null, if there are no elements 
     */
    public Specialisation lastElement() {
        try {
            return list.lastElement();
        } catch (NoSuchElementException e) {
            return null;
        }
    }

    /**
     *Check if the given Specialisation is contained in this set.
     *@param s The Specialisation to find
     *@return true if the Specialisation is found, false otherwise
     */
    public boolean contains(Specialisation s) {
        return list.contains(s);

    }

    /**
     *Return the size of this set
     *@return the size of this set
     */
    public int getSize() {
        return list.size();
    }

    /**
     *Return an Enumeration of all Specialistion contained in this set.
     *@return Enumeration of Specialisation, can be empty.
     */
    public Enumeration<Specialisation> getElements() {
        return list.elements();
    }

    /**
     *Return the Specialisation associated with the given File.
     *@param file the file to find
     *@return the Specialisation element associated with the file if it was found otherwise return null
     */
    public Specialisation getSpecialisation(File file) {
        Specialisation found = null;
        for (int i = 0; i < list.size() && found == null; i++) {
            Specialisation toCheck = list.get(i);
            if (toCheck.getFile().equals(file)) {
                found = toCheck;
            }
        }
        return found;
    }
}//end of class
