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

import java.util.*;
import java.io.*;
/**
 * classe repesentant une reponse en guise de correction d'un exercice.
 * une reponse est juste ou fausse.
 * si elle est fausse, elle possede au moins erreur non vide.
 *
 * @author vgrassau
 */
public class Answer implements Serializable {
    /** id of the lexeme */
    private int idLexeme;
    /** the correct is right or wrong */
    private boolean correct;
    /** the value of the student */
    private String value;
    /** the errors... */
    private Vector errors;
    /** the sentence */
    private String sentence;
    /** the right value */   
    private String correctValue;
    
//    public Answer() {
//        idLexeme=-1;
//        value= new String();
//        correctValue = new String();
//        correct = false;
//        errors = new Vector();
//    }
    
    
    
    
    
    /** 
     *Create an answer for a Lexeme and a given value
     *@param idLexeme the id of the lexeme
     *@param value the response value
     */
        
//    public Answer(int idLexeme, String value) {
//        this.idLexeme = idLexeme;
//        this.value= value;
//        this.correct = false;
//        this.sentence = new String();
//        errors = new Vector();
//    
//    }
    
    /** 
     *Create an Answer with a lexeme, the value and the correct value
     *@param idLexeme 
     *@param value, the value given by the student
     *@param correctValue, The correct value
     */
        
    public Answer(int idLexeme, String value,String correctValue) {
        this.idLexeme = idLexeme;
        this.value= value;
        this.correctValue = correctValue;
        this.correct = false;
        this.sentence = new String();
        errors = new Vector();
    
    }
    
    /**
     *set the correctValue for this answer
     *@param val
     */
    public void setCorrectValue(String val) {
        this.correctValue = val;
    }
    
    /**
     *Get the correctValue for this answer
     *@return the correctValue
     */
    public String getCorrectValue() {
        return this.correctValue;
    }
    
    /**
     *Set the value for this answer
     *@param value given by the student
     */
    public void setValue(String value) {
        this.value = value;
    }
    
    /**
     *Return the sentence for this answer
     *@return the sentence
     */
    public String getSentence() {
        return this.sentence;
    }
    
    /**
     *Set the sentence for this answer
     *@param s the sentence
     */
    public void setSentence(String s) {
        this.sentence = s;
    }
    
    /**
     *Return the value given by the student
     *@return the student value 
     */
    public String getValue() {
        return this.value;
    }
   
    /**
     *Return the id of the lexeme 
     *@return an id
     */
    public int getIdLexeme() {
        return this.idLexeme;
    }
    
    /**
     *Check if the answer is correct or not
     *@return true if correct false otherwise
     */
    public boolean isCorrect() {
        return this.correct;
    }

    
    /**
     *set if this answer is correct or not
     *@param b true if correct, false otherwise
     */
    public void setCorrect(boolean b) {
        this.correct = b;
    }
    
    
    /***
     *Set the errors if there are some errors
     *@param errors a collection of errors
     */
    public void setErrors(Vector errors) {
       this.errors = errors;
    }
    
    /**
     *Add a collection of errors, or an error for this answer. 
     *An erros could be an object, String , Morpho... 
     *@param o error or errors to add
     */
    public void addErrors(Object o) {
        if (o instanceof Collection ) {
             if (!this.errors.containsAll((Collection)o)) {
               this.errors.addAll((Collection)o);
           }  
        } else {
           if (!this.errors.contains(o)) {
               this.errors.add(o);
           }
        }
    }
    
    

    /**
     *Return the collection of object that represent the errors 
     *@return a collection 
     */
    public Vector getErrors() {
        return this.errors;
    }
    
}//end of class
