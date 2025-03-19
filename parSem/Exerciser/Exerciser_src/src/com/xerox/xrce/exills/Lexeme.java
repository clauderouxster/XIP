/** 
 ****************************************************************************************
 * Copyright (c) 2007 Xerox Corporation                                                 *
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

package com.xerox.xrce.exills;

import java.util.*;
import java.io.*;


/**
 *
 * @author vgrassau
 */
public class Lexeme implements Serializable {
    /** id of the lexeme */
    private int id;
    /** id of the sentence */
    private int idSent;
    /** surface for this lexeme*/
    private String surface;
    /** lemma for this lexeme */
    private String lemma;
    /** the different senses */
    private Hashtable senseList;
    
    //for the extraction :
    /** hidden or not */
    private boolean hidden;
    /** checked or not */
    private boolean isChecked;
    
    
    /** Creates a new instance of Lexeme */
    public Lexeme(int id, int idSent) {
        this.id = id;
        this.idSent = idSent;
        this.surface = new String();
        this.lemma = new String();
        this.senseList = new Hashtable();
        this.hidden = false;
        this.isChecked = false;
    }
    
    /**
     *returns the part os speech found for this lexeme
     *@return an array of string or null
     */
    public String[] getAllPOS() {
        Collection collec = this.getAllSenses();
        String[] array = null;
        if ( collec != null) {
            array = new String[collec.size()];
            Iterator ite = collec.iterator();
            for (int i=0; i < array.length && ite.hasNext();i++) {
                array[i] = ((Sense)ite.next()).getPos();
            }
        }
        return array;
    }
    
    
    /**
     *Set the surface for this lexeme
     *@param surface
     */
    public void setSurface(String surface) {
        this.surface = surface;
    }
    
    /**
     *Returns the surface for this lexeme
     *@return the surface (the word written)
     */
    public String getSurface() {
        return this.surface;
    }
    
    /**
     *Set the lemma for this lexeme.
     *Note we use only one lemma for a lexeme, should be changed in a next release.
     *@param lemma the lemma for the surface attribute
     */
    public void setLemma(String lemma) {
        this.lemma = lemma;
    }
    
    /**
     *Return the lemma of this lexeme and so of the surface
     *@return lemma form
     */
    public String getLemma() {
        return this.lemma;
    }
    
    /**
     *Check if this lexeme is checked
     *@return true if checked or not, default false
     */
    public boolean isChecked() {
        return this.isChecked;
    }
    /**
     *Check if this lexeme is hidden
     *@return true if it is false, otherwise, default false
     */
    public boolean isHidden() {
        return this.hidden;
    }
    
    /**
     *Set if this lexeme have to be checked
     *@param b true if it has to be checked
     */
    public void setChecked(boolean b) {
        this.isChecked = b;
    }
    /**
     *Set if this lexeme have to be hidden
     *@param b true if it has to be hidden
     */
    public void setHidden(boolean b) {
        this.hidden = b;
    }
    
    /**
     *Return the sense list like a dictionnary for this lexeme
     *@return a hashtable with the ids of the senses and the senses
     */
    public Hashtable getSenseList() {
        return this.senseList;
    }
    
    /**
     *Add a new Sense to this lexeme
     *@param s a sense for this lexeme.
     */
    public void addSense(Sense s) {
        this.senseList.put(new Integer(s.getId()),s);
    }
    
    /**
     *Return all the senses like a collection of this lexeme
     *@param return a collection of Sense
     */
    public Collection getAllSenses() {
        return this.senseList.values();
    }
    
    /**
     *Return all the morphologic result calculated for this lexeme
     *@return a collection of Morpho object
     */
    public Collection getAllMorpho() {
        Vector v = new Vector(this.getAllSenses());
        Vector result = new Vector();
        for (int i=0; i < v.size();i++) {
            Sense s = (Sense)v.get(i);
            result.addAll(s.getListMorpho());
        }
        return result;
    }
    
    /**
     *Return the id of this lexeme
     *@return id
     */
    public int getId() {
        return this.id;
    }
    /**
     *Return the id of the sentence containing this lexeme*
     *@return idsent
     */
    public int getIdSent() {
        return this.idSent;
    }
    
    /**
     *Return a String representation of this lexeme
     *@return a string with id, lemma, surface and  the list of Sense
     */
    public String toString() {
        StringBuffer sb = new StringBuffer("Lexeme :" + this.getId() + "\n");
        sb.append("\t lemma : " + this.getLemma());
        sb.append("\n\t surface : " + this.getSurface());
        sb.append("\n\t Senses (" + this.senseList.size() + ") : \n");
        Iterator ite = this.getAllSenses().iterator();
        while (ite.hasNext()) {
            sb.append("\t\t " + ite.next());
        }
        return sb.toString();
    }
    
}//end of class
