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

/**
 * Implementation de l'interface pour utiliser les resultats d'une analysis morphologique
 * @author vgrassau
 */
public class DefaultAnalysis implements Analysis {
    
    
    /***
     *Table des lexemes
     */
    private Hashtable tableLexeme;
    /**
     *Table des phrases de l'analyse, pour une phrase on a une liste d' index des lexemes lui correspondant.
     */
    private Hashtable tableSentences;
    /**
     *L'analyse est propre pour une langue donnee
     */
    private String language;
     
    /**
     * Construit l'analyse a partir de la table des lexemes et du langage
     * @param tableLexeme table des lexemes 
     * @param language langage du texte analyse
     *
     * @see com.xerox.xrce.exills.AnalysisFactory#getInstance(Object[],String)
     */
    public DefaultAnalysis(Hashtable tableLexeme, String language) {
      this.tableLexeme = tableLexeme;
      this.language = language;  
      this.tableSentences = initTableSentences();
     }

    
    /**
     *Renvoie le langage du texte analyse
     *@return une chaine de caractere representant le langage
     */
    public String getLanguage() {
        return this.language;
    }
    
    /**
     *Cree la table des correspondances entre phrases et lexeme.
     *@return table de hachage cle id de la pharse valeur : liste d'id lexemme
     */
    private Hashtable initTableSentences() {
        Hashtable table = new Hashtable();
        Iterator ite = this.tableLexeme.values().iterator();
        while (ite.hasNext()) {
            Lexeme l = (Lexeme)ite.next();
            Integer keySent = new Integer(l.getIdSent());
            Integer idLex = new Integer(l.getId());
            if (table.containsKey(keySent)) {
                Vector v = (Vector)table.get(keySent);
                v.add(idLex);
            } else {
                Vector w = new Vector();
                w.add(idLex);
                table.put(keySent,w);
            }
        }
       return table;
    }
    
   
    /**
     *@see com.xerox.xrce.exills.Anlysis#getIdSentence(int)
     */
   public int getIdSentence(int idLexeme) {
       Lexeme l = this.getLexeme(idLexeme);
       if ( l!= null) {
           return l.getIdSent();
       } else {
           return -1;
       }
   }
    
    
        
    /** 
     *@see com.xerox.xrce.exills.Anlysis#getText()
     */
    public String getText(){
       StringBuffer text = new StringBuffer();
       for (int i=1; i <= this.countSentences();i++) {
        text.append(this.getSentence(i));    
       }
       return text.toString();
    } 
    
    
   
    
    /**
     *@see com.xerox.xrce.exills.Anlysis#getSentence(int)
     */
    public String getSentence(int k){
        Integer key = new Integer(k);
        StringBuffer sentence = new StringBuffer();
        if (this.tableSentences.containsKey(key)) { 
         Vector listIdLexeme = (Vector)this.tableSentences.get(key);
         //on tri le vector du plus petit au plus grand.
          Vector sorted = sortId(listIdLexeme);
          // on crer la phrase
          for (int j=0; j < sorted.size(); j++) {
              Integer idLex = (Integer)sorted.get(j);
              Lexeme lexeme = this.getLexeme(idLex.intValue());
              if (isPunct(lexeme)) {
                  int index = sentence.length()-1;
                  if ( index != -1 )
                     sentence.deleteCharAt(sentence.length()-1);
                  sentence.append(lexeme.getSurface()+" ");
              } else {
                 sentence.append(lexeme.getSurface()+" ");
              } 
               
          }
      
        return sentence.toString(); 
        } else {
            return null;
        }
         
    }
    
   /**
    *@see com.xerox.xrce.exills.Anlysis#countSentences()
    */
    public int countSentences(){
       if ( this.tableSentences != null ) {
           return this.tableSentences.size();
       } else {
           return 0;
       }
    
    }

    /**
     *@see com.xerox.xrce.exills.Anlysis#getLexeme(String)
     */
    public Lexeme[] getLexeme(String lemma){
        Vector v = new Vector();
        Collection coll = this.getAllLexeme();
        if (coll != null) {
            Iterator ite = coll.iterator();
            while (ite.hasNext()) {
                Lexeme l = (Lexeme)ite.next();
                if (l.getLemma().equalsIgnoreCase(lemma)) {
                    v.add(l);
                }
            }
           return this.toArray(v);
        }
        return null;
    }
   
     /**
     *@see com.xerox.xrce.exills.Anlysis#getAllLexeme()
     */
    public Collection getAllLexeme() {
        if (this.tableLexeme != null )
            return this.tableLexeme.values();
        else {
           return null; 
        }
    }
    
    /**
     *@see com.xerox.xrce.exills.Anlysis#getLexemeFromSurface()
     */
    public Lexeme[] getLexemeFromSurface(String surface){
        Vector v = new Vector();
        Collection coll = this.getAllLexeme();
        if (coll != null) {
            Iterator ite = coll.iterator();
            while (ite.hasNext()) {
                Lexeme l = (Lexeme)ite.next();
                if (l.getSurface().equalsIgnoreCase(surface)) {
                    v.add(l);
                }
            }
           return this.toArray(v);
        }
        return null;
    
    }
    
    /**
     *@see com.xerox.xrce.exills.Anlysis#countLexeme()
     */
    public int countLexeme(){
        if (this.tableLexeme == null )
            return 0;
        else {
            return this.tableLexeme.size();
        }
        
    }
    
    
   
    /**
     *@see com.xerox.xrce.exills.Anlysis#getLexeme(int)
     */
    
    public Lexeme getLexeme(int idLexeme){
        Integer key = new Integer(idLexeme);
        return (Lexeme)this.tableLexeme.get(key);
    
    }
    
    /**
     *@see com.xerox.xrce.exills.Anlysis#isVerb(Lexeme)
     *@see com.xerox.xrce.exills.DefaultAnlysis#containsPOS(Lexeme,String[])
     */
     public boolean isVerb(Lexeme l){
        return containsPOS(l,this.VERB_POS);
     }
    
     /**
     *@see com.xerox.xrce.exills.Anlysis#isNoun(Lexeme)
     *@see com.xerox.xrce.exills.DefaultAnlysis#containsPOS(Lexeme,String[]) 
     *@see com.xerox.xrce.exills.DefaultAnlysis#containsMorpho(Lexeme,Morpho)
     *@version 1.1, modification a cause des noms propres pour le francais
     *
     */
    public boolean isNoun(Lexeme l){
        Morpho m = new Morpho(-1,"+Proper");
        Morpho m2 = new Morpho(-1,"+Prop");
        boolean isNounCat = containsPOS(l,this.NOUN_POS);
        boolean isProperMorpho = containsMorpho(l, m ) || containsMorpho(l, m2 );
       // System.out.println( l.getSurface() + " cat :" + isNounCat + " morpho Proper : " + isProperMorpho );
        return isNounCat && !isProperMorpho;
        
        
    }
    
    /**
     *@see com.xerox.xrce.exills.Anlysis#isPronoun(Lexeme)
     *@see com.xerox.xrce.exills.DefaultAnlysis#containsPOS(Lexeme,String[])
     */
    public boolean isPronoun(Lexeme l) {
       return containsPOS(l,this.PRONOUN_POS);
    }
    
     /**
     *@see com.xerox.xrce.exills.Anlysis#isArticle(Lexeme)
      @see com.xerox.xrce.exills.DefaultAnlysis#containsPOS(Lexeme,String[])
     */
    public boolean isArticle(Lexeme l) {
       return containsPOS(l,this.ARTICLE_POS);
    }
    
   /**
    *@see com.xerox.xrce.exills.Anlysis#isAdverb(Lexeme)
    *@see com.xerox.xrce.exills.DefaultAnlysis#containsPOS(Lexeme,String[])
    */
    public boolean isAdverb(Lexeme l) {
      return containsPOS(l,this.ADVERB_POS);
    }
    
   /**
    *@see com.xerox.xrce.exills.Anlysis#isAuxiliary(Lexeme)
     *@see com.xerox.xrce.exills.DefaultAnlysis#containsPOS(Lexeme,String[])
    */
    public boolean isAuxiliary(Lexeme l) {
      return containsPOS(l,this.AUX_POS);
    }
    
   /**
    *@see com.xerox.xrce.exills.Anlysis#isPreposition(Lexeme)
    * *@see com.xerox.xrce.exills.DefaultAnlysis#containsPOS(Lexeme,String[])
    */
    public boolean isPreposition(Lexeme l) {
        return containsPOS(l,this.PREPOSITION_POS);
    }
    
    /**
    *@see com.xerox.xrce.exills.Anlysis#isProperName(Lexeme)
    *@see com.xerox.xrce.exills.DefaultAnlysis#containsPOS(Lexeme,String[])
    *@see com.xerox.xrce.exills.DefaultAnlysis#containsMorpho(Lexeme,Morpho)
    */
    public boolean isProperName(Lexeme l) {
        
        Morpho m = new Morpho(-1,"+Proper");
        Morpho m2 = new Morpho(-1,"+Prop");
        return containsPOS(l,this.PROPER_NOUN) && (containsMorpho(l, m ) || containsMorpho(l, m2 ));    
    }
    
    /**
     *@see com.xerox.xrce.exills.Anlysis#isQuantifier(Lexeme)
     *@see com.xerox.xrce.exills.DefaultAnlysis#containsPOS(Lexeme,String[])
     */
    public boolean isQuantifier(Lexeme l) {
        return containsPOS(l,this.QUANT_POS);
    }
    
    /**
    *@see com.xerox.xrce.exills.Anlysis#containsMorpho(Lexeme, Morpho)
     utilise la methode equals d'un objet Morpho
    */
    public  boolean containsMorpho(Lexeme l, Morpho m ) {
        Iterator ite = l.getAllSenses().iterator();
        while (ite.hasNext()) {
            Sense s = (Sense)ite.next();
            if (s.contains(m)) {
                return true;
            }
            
        }
        return false;
    }
    
    /**
     *@see com.xerox.xrce.exills.Anlysis#containsPOS(Lexeme, String)
     *
     */
    public boolean containsPOS(Lexeme l,String pos) {
        Iterator ite = l.getAllSenses().iterator();
        
         while (ite.hasNext()) {
            Sense s = (Sense)ite.next();
            if (s.getPos().equalsIgnoreCase(pos)) {
                return true;
            }
            
        }
        
        return false;
    }
    
    /**
     *@see com.xerox.xrce.exills.Anlysis#containsPOS(Lexeme, String[])
     *
     */
    public boolean containsPOS(Lexeme l,String[] pos) {
         Iterator ite = l.getAllSenses().iterator();
         while (ite.hasNext()) {
            Sense s = (Sense)ite.next();
             for (int i=0; i < pos.length ; i++) {
                  if (s.getPos().equalsIgnoreCase(pos[i])) {
                      return true;
                  }
            }
            
        }
        
        return false;
    }
    
    
    /**
     *@see com.xerox.xrce.exills.Anlysis#isAdj(Lexeme)
     *@see com.xerox.xrce.exills.DefaultAnlysis#containsPOS(Lexeme,String[])
     */
    public boolean isAdj(Lexeme l){
       return containsPOS(l,this.ADJECTIVE_POS);
    }
    
    /**
     *@see com.xerox.xrce.exills.Analysis#getTempsOfVerb()
     *@verson 0.0 pas implemente
     *@deprecated
     */
    public String[] getTempsOfVerb(){
        return null;
    }
    
    /**
     *@see com.xerox.xrce.exills.Analysis#getModesOfVerb()
     *@verson 0.0 pas implemente
     *@deprecated
     */
    
    public String[] getModesOfVerb(){return null;}
    
    
    /**
     *@see com.xerox.xrce.exills.Analysis#getAllVerb()
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String[])
     */
    public Lexeme[] getAllVerb() {
       return getAllPOS(this.VERB_POS);
    }
    
    /**
     *@see com.xerox.xrce.exills.Analysis#getAllAuxiliary()
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String[])
     */
    public Lexeme[] getAllAuxiliary() {
       return getAllPOS(this.AUX_POS);
    }
    
    /**
     *@see com.xerox.xrce.exills.Analysis#getAllNoun()
      @see com.xerox.xrce.exills.Analysis#getAllPOS(String[])

     *@version 1.1 modifier pour les noms propres
     */
    public Lexeme[] getAllNoun() {
        Lexeme[] allNoun = getAllPOS(this.NOUN_POS);
        Vector keeper = new Vector();
        for (int i =0; i < allNoun.length ; i++) {
            if ( isNoun(allNoun[i])) { //enleve les noms propres
                keeper.add(allNoun[i]);
            }
        }
       return this.toArray(keeper);
    }
    
    /**
     *@see com.xerox.xrce.exills.Analysis#getAllAdj()
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String[])
     */
    public Lexeme[] getAllAdj() {
        return getAllPOS(this.ADJECTIVE_POS);
    }
    
    /**
     *@see com.xerox.xrce.exills.Analysis#getAlladverbs()
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String[])
     */
     public Lexeme[] getAllAdverbs() {
        return getAllPOS(this.ADVERB_POS);
     }
    
     
     /**
     *@see com.xerox.xrce.exills.Analysis#getAllProperName()
     *@see com.xerox.xrce.exills.Analysis#getAllPOS(String[])
      @see com.xerox.xrce.exills.Analysis#isProperName(String[])
     */
    public Lexeme[] getAllProperName() {
          Lexeme[] allNoun = getAllPOS(this.PROPER_NOUN);
        Vector keeper = new Vector();
        for (int i =0; i < allNoun.length ; i++) {
            if ( isProperName(allNoun[i])) { //enleve les noms propres
                keeper.add(allNoun[i]);
            }
        }
        
       return this.toArray(keeper);
     }
     
     
     /**
      *@see com.xerox.xrce.exills.Analysis#getAllPOS(String[])
      *@see com.xerox.xrce.exills.DefaultAnalysis#containsPOS(Lexeme,String[])
      */
     public Lexeme[] getAllPOS(String[] pos) {
        Collection c = this.getAllLexeme();
        if ( c!=null) {
           Vector v = new Vector();
            Iterator ite = c.iterator();
            while (ite.hasNext()) {
                Lexeme l = (Lexeme)ite.next();
                if ( containsPOS(l,pos)) {
                    v.add(l);
                }
            }
          return this.toArray(v);
        }
        return null;
     }
     
      /**
      *@see com.xerox.xrce.exills.Analysis#getAllPOS(String)
      *@see com.xerox.xrce.exills.DefaultAnalysis#containsPOS(Lexeme,String)
      */
     public Lexeme[] getAllPOS(String pos) {
        Collection c = this.getAllLexeme();
        if ( c!=null) {
           Vector v = new Vector();
            Iterator ite = c.iterator();
            while (ite.hasNext()) {
                Lexeme l = (Lexeme)ite.next();
                if ( containsPOS(l,pos)) {
                    v.add(l);
                }
            }
          return this.toArray(v);
        }
        return null;
     }
     
     
     /**
      *@see com.xerox.xrce.exills.Analysis#getAllPronouns()
      *@see com.xerox.xrce.exills.DefaultAnalysis#getAllPOS(String[])
      */
      public Lexeme[] getAllPronouns() {
         return getAllPOS(this.PRONOUN_POS);
      }
          /**
      *@see com.xerox.xrce.exills.Analysis#getAllQuantfiers()
      *@see com.xerox.xrce.exills.DefaultAnalysis#getAllPOS(String[])
      */

       public Lexeme[] getAllQuantifiers() {
         return getAllPOS(this.QUANT_POS);
      }
     
           /**
      *@see com.xerox.xrce.exills.Analysis#getAllArticles()
      *@see com.xerox.xrce.exills.DefaultAnalysis#getAllPOS(String[])
      */

     public Lexeme[] getAllArticles() {
       return getAllPOS(this.ARTICLE_POS);
    }  
      
          /**
      *@see com.xerox.xrce.exills.Analysis#getAllPreposition()
      *@see com.xerox.xrce.exills.DefaultAnalysis#getAllPOS(String[])
      */

       public Lexeme[] getAllPreposition() {
         return getAllPOS(this.PREPOSITION_POS);
    }
      
      
            /**
      *@see com.xerox.xrce.exills.Analysis#getAllPunctuations()
      *@see com.xerox.xrce.exills.DefaultAnalysis#getAllPOS(String[])
      */

       
     public Lexeme[] getAllPunctuations() {
         return getAllPOS(this.PUNCT_POS);
    }
    
       
    /**
     *@see com.xerox.xrce.exills.Analysis#isPunct(Lexeme)
     *@see com.xerox.xrce.exills.DefaultAnlysis#containsPOS(Lexeme,String[])
     */
    public boolean isPunct(Lexeme l) {
        return containsPOS(l,this.PUNCT_POS);
   }
 
    /**
     *@see com.xerox.xrce.exills.Analysis#getLexemeInSentence(int)
     */
  public Lexeme[] getLexemeInSentence(int idSentence) {
      Integer key = new Integer(idSentence);
      if ( this.tableSentences!=null) {
        if (this.tableSentences.containsKey(key)) {
            Vector v = sortId((Vector)tableSentences.get(key));
            Lexeme[] array = new Lexeme[v.size()];
            for (int i=0; i < v.size();i++) {
                Integer idLex = (Integer)v.get(i);
                array[i]  = this.getLexeme(idLex.intValue());
            }
            return array;
        }  else {
            return null;
        }
          
      } else {
        return null;       
      }
  }  
    
  /**
   *@see java.lang.Object#toString()
   *@return tous les phrases analysees avec les lexemes. On applique les methodes toString su chaque objet
   */
  public String toString() {
      int nbSentences =  this.countSentences();
      StringBuffer sb = new StringBuffer();
      if ( nbSentences > 0) {
      sb.append("Analysis of " + nbSentences +" sentence(s)\n");
      sb.append(" language :" + this.getLanguage());
      for (int i=1; i<=nbSentences; i++){    
        sb.append("\nsentence : " + i + " " + this.getSentence(i));
        Lexeme[] array = this.getLexemeInSentence(i);
        for (int j=0; j < array.length; j++) {
            sb.append("\n " + array[j]);
        }
      }
      
     } else { 
       sb.append("Analysis of " + this.countLexeme()+ " lexeme(s) \n");
       sb.append(" language :" + this.getLanguage());
       if (this.tableLexeme != null) {
           Iterator ite = this.tableLexeme.values().iterator();
           while (ite.hasNext()) {
           sb.append("\n " + ite.next());
           }
       }
     }
      return sb.toString();
  } 
  
  
  /**
   *Tri un vecteur d'entier dans l'ordre croissant.
   *@param vector a trier
   *@return le vector trie dans l'ordre croissant
   */
  private Vector sortId(Vector v) {
      //on tri le vector du plus petit au plus grand.
          Vector sorted = new Vector();
          Enumeration eLex = v.elements();
          while (eLex.hasMoreElements()) {
            Integer id = (Integer)eLex.nextElement();
            int indexToInsert = 0;
            boolean stop = false;
            for(int i=0; i < sorted.size() && !stop; i++) {
                Integer idSorted  = (Integer)sorted.get(i);
                if (id.intValue() < idSorted.intValue()) {
                   indexToInsert = i;  
                   stop = true;
                } else {
                     indexToInsert = i+1;
                }
           }
           sorted.add(indexToInsert, id);
        }//end of eLex enum
       return sorted;
 }
  
  
/**
 *Convertit un vecteur en tableau 
 *@param v vecteur a convertir
 */  
private Lexeme[] toArray(Vector v) {
    Lexeme[] array = new Lexeme[v.size()];
    for (int i=0; i < array.length; i++) {
        array[i] = (Lexeme)v.get(i);
    }
    return array;
}


/**
 *@see com.xerox.xrce.exills.Analysis#setHiddenLexeme(Lexeme[],boolean)
 *@see com.xerox.xrce.exills.Lexeme#setHidden(boolean)
 */
public Lexeme[] setHiddenLexeme(Lexeme[] array,boolean b) {
    for (int i=0; i < array.length; i ++ ) {
        array[i].setHidden(b);
    }
    return array;
}

/**
 *@see com.xerox.xrce.exills.Analysis#setCheckLexeme(Lexeme[],boolean)
 *@see com.xerox.xrce.exills.Lexeme#setChecked(boolean)
 */

public Lexeme[] setCheckLexeme(Lexeme[] array,boolean b) {
    for (int i=0; i < array.length; i ++ ) {
        if (array[i].isHidden())
           array[i].setChecked(b);
    }
    return array;
}


/**
 *@see com.xerox.xrce.exills.Analysis#writeFormCheckText()
 *@see com.xerox.xrce.exills.DefaultAnalysis#getAllLexemeInArray()
 *@see com.xerox.xrce.exills.Lexeme#isHidden()
 *@see com.xerox.xrce.exills.Lexeme#isChecked()
 *@see com.xerox.xrce.exills.Lexeme#getSurface()
 
 */
public String writeFormCheckText() throws Exception {
    StringBuffer form = new StringBuffer();
       try {
       Lexeme[] array  = getAllLexemeInArray();
       
       for (int i=array.length - 1 ; i >= 0; i-- ) {
           
           if (array[i].isHidden()) {
               if ( array[i].isChecked())
                  form.append("<span id=\""+  array[i].getId()+ "\" class=\"verbChecked\" "+ "title=\"" + titlePOS(array[i])+ "\""  +">");
               else 
                  form.append("<span id=\""+  array[i].getId()+ "\" class=\"verb\"  "+ "title=\"" + titlePOS(array[i])+ "\""  +">");
               form.append(array[i].getSurface());
               form.append("</span> <input id=\""+ array[i].getId()+ "c\" type=\"checkbox\" name=\"blank\" ");
               form.append("value=\"" + array[i].getId() + "\" ");
               form.append("onclick='check(\"" + array[i].getId() + "c\",\""+ array[i].getId() + "\")' ");
               if ( array[i].isChecked())
                   form.append("checked ");
               form.append("/> ");
          } else {
                if (isPunct(array[i])) {
                  int index = form.length()-1;
                    if(index >= 0)
                      form.deleteCharAt(index);
                  form.append(array[i].getSurface()+" ");
              } else {
                 form.append(array[i].getSurface()+" ");
              } 
          }
           
       }
       
       
       return form.toString();
       } catch (Exception e) {
           throw e;
       }
}


/**
 *Renvoie une chaine de caracteres contenant tous les Part of speech d'un Lexeme
 *@param l Lexeme 
 *@return une chaine de caractere dont les part of speech sont separes par des virgules.
 *
 */
private String titlePOS(Lexeme l) {
    StringBuffer title = new StringBuffer();
    String[] tab = l.getAllPOS();
    if ( tab !=null && tab.length > 0) {
        title.append(tab[0]);
        for (int i=1; i < tab.length; i++) {
            title.append(", " + tab[i]);
        }
     
    }
    return title.toString();
}


/**
 *@see com.xerox.xrce.exills.Analysis#getSentences()
 */
public Vector getSentences() {
     Vector v = new Vector();
      Enumeration eKeys = this.tableSentences.keys();
      while ( eKeys.hasMoreElements()) {
            v.add(eKeys.nextElement());
       }
    return sortId(v); 
}


/**
 *@see com.xerox.xrce.exills.Analysis#writeHTMLExtraction()
 *@see com.xerox.xrce.exills.DefaultAnalysis#getLexemeInSentence()
 *@see com.xerox.xrce.exills.DefaultAnalysis#getSentences()
 *@see com.xerox.xrce.exills.Lexeme#isHidden()
 *@see com.xerox.xrce.exills.Lexeme#isChecked()
 *@see com.xerox.xrce.exills.Lexeme#getSurface()
 */
public String writeHTMLExtraction() throws Exception {
    StringBuffer buffer = new StringBuffer();
      try {
           //on va recuperer toute les cles de  table des phrases et trie ces cles.
        
          Vector sentences = getSentences();
          buffer.append("<ol>");
          for (int k=0; k < sentences.size(); k++) {
              Integer idSent = (Integer)sentences.get(k);
              buffer.append("<li>");
              Lexeme[] array = this.getLexemeInSentence(idSent.intValue());
              for (int i=0  ; i < array.length ; i++ ) {
                    if (array[i].isHidden()) {
                        //differe le rendu si le lexeme est selectionnable
                        if ( array[i].isChecked())
                             buffer.append("<span id=\""+  array[i].getId()+ "\" class=\"verbChecked\" "+ "title=\"" + titlePOS(array[i])+ "\""  +">");
                        else 
                             buffer.append("<span id=\""+  array[i].getId()+ "\" class=\"verb\"  "+ "title=\"" + titlePOS(array[i])+ "\""  +">");
                        //ajout de la forme de surface
                        buffer.append(array[i].getSurface());
                        //ajout de la checkbox HTML pour selectionner le mot
                        buffer.append("</span> <input id=\""+ array[i].getId()+ "c\" type=\"checkbox\" name=\"blank\" ");
                        buffer.append("value=\"" + array[i].getId() + "\" ");
                        buffer.append("onclick='check(\"" + array[i].getId() + "c\",\""+ array[i].getId() + "\")' ");
                        if ( array[i].isChecked())
                           buffer.append("checked ");
                           buffer.append("/> ");
                   } else {
                       //si on a une ponctuation
                        if (isPunct(array[i])) {
                           int index = buffer.length()-1;
                           if(index >= 0)
                              buffer.deleteCharAt(index);
                           buffer.append(array[i].getSurface()+" ");
                       } else {
                         buffer.append(array[i].getSurface()+" ");
                       }
                  }
             }//fin de la boucle sur les lexemes
              buffer.append("<br><br></li>");
          }//fin de la boucle sur les phrases
       return buffer.toString();
       } catch (Exception e) {
           throw e;
       }
}


/**
 *@see com.xerox.xrce.exills.Analysis#getAllLexemeInArray()
 */
public Lexeme[] getAllLexemeInArray() {
    return this.toArray(new Vector(this.getAllLexeme()));
}
  


}//end of class
