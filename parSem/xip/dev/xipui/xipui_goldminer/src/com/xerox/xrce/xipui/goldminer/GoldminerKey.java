/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 *
 * GoldminerKey.java
 *
 * Created on October 17, 2006, 9:44 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer;

import java.util.*;

/**
 * A key for a map, the key is a document id, and an id of dependency
 * an id of a dependency have to be this format : 1_2 where 1 is the number of a sentence and 2 the number of the dependency in this
 * sentence.
 * @author vgrassau
 */
public class GoldminerKey {

    /** id of the document */
    private int idDocument = -1;
    /** the id dependency */
    private String idDependency = "";

    /**
     *Creates a new instance of GoldminerKey
     *@param idDocument the id of a document
     *@param idDependency an id of a dependeny
     */
    public GoldminerKey(int idDocument, String idDependency) {
        this.idDocument = idDocument;
        this.idDependency = idDependency;
    }

    /**
     *Returns the id of the documents
     *@return a number
     */
    public int getIdDocument() {
        return this.idDocument;
    }

    /**
     *Returns the id of the sentence, calculated from the id of the dependency
     *@return a number, -1 if the if of the dependency is null
     */
    public int getIdsent() {
        if (idDependency != null) {
            String[] split = idDependency.split("_");
            if (split.length == 2) {
                return Integer.parseInt(split[1]);
            }
        }
        return -1;
    }

    /**
     *Return the id of the dependency
     *@return a string id
     */
    public String getIdDependency() {
        return this.idDependency;
    }

    /**
     *Checks if the given object is equal to this GoldminerKey
     *@return true if equal false otherwise
     */
    public boolean equals(Object o) {
        boolean equal = false;
        if (o instanceof GoldminerKey) {
            GoldminerKey compare = (GoldminerKey) o;
            // equal = ( idDependency.equals(compare.getIdDependency()));
            equal = toString().equals(compare.toString());
        }
        return equal;
    }

    /**
     *Returns a String representation of this object
     *@return the id of document with the id of dependency in a same string
     */
    public String toString() {
        return idDocument + "," + idDependency;
    }

    /**
     *Returns an hashcode of this object
     *@return the hashcode of the toString() method
     */
    public int hashCode() {
        return toString().hashCode();
    //return idDependency.hashCode();
    }

    /**
     *Don't use, it has been writen for test only
     * @param args 
     */
    static public void main(String[] args) {
        Hashtable<GoldminerKey, String> map = new Hashtable();
        GoldminerKey key1 = new GoldminerKey(2, "13_1");
        GoldminerKey key2 = new GoldminerKey(2, "13_2");
        GoldminerKey key3 = new GoldminerKey(1, "13_1");
        map.put(key2, "cle1");
        map.put(key3, "cle3");
        System.out.println(key1.equals(key2));
        System.out.println(key1.equals(key3));
        System.out.println(map.containsKey(key1));
        System.out.println(map.get(key3));
    }
}//end of class
