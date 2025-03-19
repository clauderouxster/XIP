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

package com.xerox.xrce.exills.exercice;

import java.util.*;
import com.xerox.xrce.exills.*;
/**
 *
 * @author vgrassau
 */
public class ExerciceFill extends Exercice {
    
    private Vector categories;
    private int level;
    public static final String LEVEL0 = "0";
    public static final String LEVEL1 = "1";
    public static final String LEVEL2 = "2";
    
    public static final int LEVEL0_ = 0;
    public static final int LEVEL1_ = 1;
    public static final int LEVEL2_ = 2;
    
    public static final String VERB ="VERB";
    public static final String NOUN ="NOUN";
    public static final String ADJECTIVE ="ADJ";
    public static final String ADVERB = "ADV";
    public static final String PREPOSITION = "PREP";
    public static final String ARTICLE = "ART";
    public static final String PRONOUN =  "PRONOUM";
    public static final String PONCTUATION = "punct";
    public static final String AUX ="AUX";    
    public static final String PROPER_NAME = "PROPER_NAME";
    public static final String QUANT ="QUANT";
    //public static final String PHRASAL_VERB =""; eg Take off
    //public static final String RELATIVE_PRONOUN =""; 
    
    
    /** Creates a new instance of ExerciceFill 
     *@param title the title of this exercice 
     *@param text the text of this exercice
     *NB : just call the constructor of the class Exercice
     */
    public ExerciceFill(User user ,String title,String description) {
      super(user,title,description);
      categories = new Vector();
      level = 0;
    }
    
    
    public ExerciceFill(Exercice exo) {
        this(exo.getUser(),exo.getTitle(),exo.getDescription());
        this.setText(exo.getText());
        this.setAuthor(exo.getAuthor());
        this.analysis = exo.getAnalysis();
        this.setInstruction(exo.getInstruction());
    }
    
    /** initialize the level for this exercice 
     @param level the level for this exercie
     */
    public void setLevel(int level){
        this.level = level;
    }
    
    /**
     *initialize the categories to extract for the text of this exercice
     *@param vector of name category
     */
    public void setCategories(Vector categories) {
        this.categories = categories;
    }

    /**
     *initialize the categories to extract for the text of this exercice
     *@param string array of categories
     */
    public void setCategories(String[] categories_) {
        this.categories.clear();
        for (int i = 0 ; i < categories_.length ; i++) {
             this.categories.add(categories_[i]);
        }
        
    }

    /** return the categories 
     @return vector of name catgeories
     */
    public Vector getCategories() {
        return this.categories;
    }
    
    
    public String getCategoriesString() {
        StringBuffer buf = new StringBuffer();
        if ( !this.categories.isEmpty()) {
           String cat = (String)this.categories.firstElement();
           buf.append(cat);
           for (int i=1; i < categories.size();i++) {
             buf.append( ", " + categories.get(i));
           }
        }
        return buf.toString();
    }
    
    /**
     *return the level for this exercice
     */
    public int getLevel() {
        return this.level;
    }

    /**
     *Methode permettant d'extraire les catégories du texte. 
     *@return Renvoie un vecteur de lexemes contenant uniquement des lexemes correspondant 
     * aux categories precedemment choisies. 
     * On renvoie null si aucun lexeme n'est trouve
     */
    public Vector extractLexeme() {
        if (analysis != null) {
        Vector v = new Vector();
        int nbLexeme = 0;
        analysis.setHiddenLexeme(analysis.getAllLexemeInArray(),false);// on garde tous les lexemes
        if (this.categories!=null) {
            Enumeration eCat = this.categories.elements();
            while (eCat.hasMoreElements()) { // parcous des catégories choisies...
                String cat = (String)eCat.nextElement();
                if ( cat.equals(this.VERB)) { // les verb 
                    Lexeme[] verbs = analysis.getAllVerb();
                    v.add(verbs);
                    analysis.setHiddenLexeme(verbs,true);
                }
                if ( cat.equals(this.NOUN)) { //les noms 
                    Lexeme[] nouns = analysis.getAllNoun();
                    v.add(nouns);
                    analysis.setHiddenLexeme(nouns,true);
                }
                if ( cat.equals(this.ADJECTIVE)) { //les adjectifs
                     Lexeme[] nouns = analysis.getAllAdj();
                     v.add(nouns);
                    analysis.setHiddenLexeme(nouns,true);
                }
                if ( cat.equals(this.ADVERB)) { //les adverbes 
                    Lexeme[] nouns = analysis.getAllAdverbs();
                    v.add(nouns);
                    analysis.setHiddenLexeme(nouns,true);
                }
                if ( cat.equals(this.ARTICLE)) { //les articles
                    Lexeme[] nouns = analysis.getAllArticles();
                    v.add(nouns);
                    analysis.setHiddenLexeme(nouns,true);
                }
                if ( cat.equals(this.PONCTUATION)) { //les ponctuations 
                    Lexeme[] nouns = analysis.getAllPunctuations();
                    v.add(nouns);
                    analysis.setHiddenLexeme(nouns,true);
                }
                if ( cat.equals(this.PRONOUN)) { //les pronoms 
                     Lexeme[] nouns = analysis.getAllPronouns();
                    v.add(nouns);
                    analysis.setHiddenLexeme(nouns,true);
                }
                if ( cat.equals(this.PREPOSITION)) { //les prépositions 
                    Lexeme[] nouns = analysis.getAllPreposition();
                    v.add(nouns);
                    analysis.setHiddenLexeme(nouns,true);
                }
                if ( cat.equals(this.AUX)) { //les auxiliares
                    Lexeme[] nouns = analysis.getAllAuxiliary();
                    v.add(nouns);
                    analysis.setHiddenLexeme(nouns,true);
                }
               if ( cat.equals(this.PROPER_NAME)) { //les auxiliares
                    Lexeme[] nouns = analysis.getAllProperName();
                    v.add(nouns);
                    analysis.setHiddenLexeme(nouns,true);
                }
               if ( cat.equals(this.QUANT)) { //les auxiliares
                    Lexeme[] array = analysis.getAllQuantifiers();
                    v.add(array);
                    analysis.setHiddenLexeme(array,true);
                }
            }
        }
      
        
        
        Vector lexemes = new Vector();
        for (int i=0; i < v.size(); i++) {
            Lexeme[] tmp = (Lexeme[])v.get(i);
            for (int j=0; j < tmp.length; j++) {
                lexemes.add(tmp[j]);
            }
        }
        return lexemes;    
        } else {
            return null;
        }
      }
   
    
    
    
    
}//end of class
