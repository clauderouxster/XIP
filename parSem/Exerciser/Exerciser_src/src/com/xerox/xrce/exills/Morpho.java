/** 
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
package com.xerox.xrce.exills;

import java.io.*;
/**
 * class which represents a feature i.e. a couple of name and value
 * @author vgrassau
 */
public class Morpho implements Serializable {
    
    private int id; // the id of the morpho for a sence of lexeme
    private int idSense;
    private int idLexeme;
    private String value; //the value of this feature
    
    /**
     * Creates a new instance of Morpho 
     * 
     * @param name name of this feature
     * @param value : value of this feature
     */
    public Morpho(int id,String value) {
      this.id = id;
      setValue(value);
    }
    
    
    public Morpho(int id,int idSense, int idLexeme,String value){
        this(id,value);
        this.setIdLexeme(idLexeme);
        this.setIdSense(idSense);
    }
    
    
    public void setIdSense(int id) {
        this.idSense = id;
    }
    
    public void setIdLexeme(int id) {
        this.idLexeme = id;
    }
    
    public int getIdLexeme() {
        return this.idLexeme;
    }
    
    public int getIdSense() {
        return this.idSense;
    }
    
    
    
    /**
     *Initializes a value for this feature
     *@param string a value
     */
    public void setValue(String string) {
        this.value= string;
    }
    
    /***
     *Return the value for this feature
     *@return a value
     */
    public String getValue() {
        return this.value;
    }
    
    /**
     *Return the name of this feature
     *@return the name
     */
    public int getId() {
        return this.id;
    }

    
    /**
     *Checks the equality with an other object given on parameter.
     *@param o object
     */
    public boolean equals(Object o) {
        if ( o instanceof Morpho) {
            Morpho o2f = (Morpho)o;
            return  getValue().equals(o2f.getValue());
        } else {
            return false;
        }
    }
    
    public boolean equalsWithId(Object o) {
        if ( o instanceof Morpho) {
            Morpho o2f = (Morpho)o;
            return getId() == o2f.getId() && getValue().equals(o2f.getValue());
        } else {
            return false;
        }
    }

    /**
     *Return the representation of a feature under a string format
     *@return a string like this : +value 
     */
    public String toString() {
        return getValue();
    }
}//end of class
