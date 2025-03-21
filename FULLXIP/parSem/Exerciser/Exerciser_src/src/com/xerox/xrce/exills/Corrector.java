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
 *
 * @author vgrassau
 */
public class Corrector implements Serializable{
    
    private Hashtable response;
    private Analysis analysis;
    private Hashtable answers;
    private int emptyCpt = 0;
    private int correctCpt = 0;
    
    
    /**Creer un objet corrector, prends les differentes reponses stockees dans une Table 
     * et utilise l'analyse pour verifier les reponses.
     *@param response reponses inserer dans une table (cle id du lexeme, valeur String)
     *@param a Analyse 
     *
     */
    public Corrector(Map response, Analysis a) {
       this.response = new Hashtable(response);
       this.analysis = a;
       this.answers = new Hashtable();   
    }
    
    
    /**
     *Evalue la reponse
     *Utilise la methode createAnswer
     */
    public void eval() throws Exception {
       
       try {
        Enumeration keys = response.keys();
        while (keys.hasMoreElements()) {
            Object key = keys.nextElement().toString();
            String value = response.get(key).toString();
            int idLexeme = -1;
            try {
             idLexeme = Integer.parseInt(key.toString());
            } catch (Exception e) {
             idLexeme = -1;      
            }
            Lexeme  lexeme = this.analysis.getLexeme(idLexeme);
            if ( lexeme != null) {
              Answer ans = this.createAnswer(lexeme,value);
              this.answers.put(new Integer(ans.getIdLexeme()),ans);
            }
        }
        
       } catch (Exception e) {
           throw e;
       }
        
        
    }

    /**
     *Cree un objet Answer pour la correction d'une valeur et du Lexeme associe.
     *@param lexeme permettant de verifier la valeur
     *@param value valeur de surface à verifier
     *@return objet answer
     *
     */
    public Answer createAnswer(Lexeme lexeme,String value) throws Exception {
        try { 
        Answer ans = new Answer(lexeme.getId(),value,lexeme.getSurface());
        
        ans.setCorrect(lexeme.getSurface().equals(value));
         if (ans.isCorrect()) {
             this.correctCpt++;
             return ans;
         } else {
            if (!value.equals("")) {
             Object[] param = new Object[] {analysis.getLanguage()};
            Analysis tmp = AnalysisFactory.getInstance(param,value);
             
            Iterator ite = tmp.getAllLexeme().iterator();
            while (ite.hasNext()) {
              Lexeme l = (Lexeme)ite.next();
              Vector allErrors = new Vector();
              Comparison compareLexeme = new Comparison(lexeme);
              Comparison compareSense  = new Comparison(new Vector(lexeme.getAllSenses()));
              Comparison compareMorpho = new Comparison(new Vector(lexeme.getAllMorpho()));
              
              if (compareLexeme.setComparison(l)) 
                allErrors.addAll(compareLexeme.getErrors());
              if (compareSense.setComparison(new Vector(l.getAllSenses()))) 
                allErrors.addAll(compareSense.getErrors());
              if (compareMorpho.setComparison(new Vector(l.getAllMorpho()))) 
                allErrors.addAll(compareMorpho.getErrors());
              
//              if ( allErrors.isEmpty() && l.getLemma().equals(lexeme.getLemma()) && compareSense.isUndefined()) {
//                  ans.setCorrect(true);
//              }
                ans.addErrors(allErrors);
           }
            } else {
                this.emptyCpt++;
                ans.addErrors("+Empty");
                ans.setValue("&nbsp;&nbsp;&nbsp;&nbsp;");
            }
            return ans;
         }
        } catch (Exception e) {
            throw e;
        }
    }

  
    public int getEnptyCpt() {
        return this.emptyCpt;
    }
    
    public int getCorrectCpt() {
        return this.correctCpt;
    }
    
    public float getPercent() {
        return (getCorrectCpt() * 100) / getNbAnswers();
    }
    
    
    
    
    
    
    /**
     *Renvoie le pourcentage d'erreurs
     */
    
    public int getNbAnswers() {
          return this.answers.size();
    }
    
    
    public Analysis getAnalysis() {
        return this.analysis;
    }
    
    
    /**
     *Renvoie la liste des reponses après une evaluation
     *@return vector de answer
     */
    public Hashtable getAnswers(){
        return this.answers;
    }

    
    public Answer getAnswer(int i) {
        return (Answer)this.answers.get(new Integer(i));
    }
}//end of class
