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

package com.xerox.xrce.exills.exercice.action;

import com.xerox.xrce.exills.*;
import com.xerox.xrce.exills.exercice.*;
import com.xerox.xrce.exills.exercice.form.*;
import javax.servlet.http.*;
import javax.servlet.*;
import java.util.*;

import org.apache.struts.action.*;
/**
 *
 * @author vgrassau
 * @version
 */

public class DoExtractionAction extends ExerciserAction {
    
   
    
    /**
   
     */
    public ActionForward executeAction(ActionMapping mapping, ActionForm  form,HttpServletRequest request, HttpServletResponse response) throws Exception {
          try {
           if (!this.isUserPresent()) {
                     return sessionTimeout(mapping,request);
            }
              //get the form
            FillInForm fillInForm = (FillInForm)form;
           //cast the exercice to a fill in exercice
            Exercice ex_ = (Exercice)session.getAttribute(Constants.EXERCICE);
            ExerciceFill exercice = new ExerciceFill(ex_);
            
            exercice.setLevel(fillInForm.getLevel());
            exercice.setCategories(fillInForm.getCategories());
            //make an extraction -----
            //met les lexemme en hidden, c'est a dire les lexemes a extraire a extraire
            Vector lemmes = exercice.extractLexeme(); 
            Vector lemmaVerb = getOnlyLemma(lemmes); 
            Vector tenses = getOnlyTenses(lemmes,exercice.getAnalysis());
            //write the form to generare the exercice : 
            Analysis a = exercice.getAnalysis();
            // methode devenue obsolete car pas optimale au niveau ergonomique
            // changement de methode avec a.writeHTMLExtraction() qui affiche phrase par phrase.
            // ce changement a ete demande par Mathilde LeGoff 
            //String formExercice = a.writeFormCheckText();
            String formExercice = a.writeHTMLExtraction();
            session.setAttribute(Constants.LEMMA_LIST,lemmaVerb);
            session.setAttribute(Constants.TEMPS_LIST,tenses);
            session.setAttribute(Constants.EXERCICE,exercice);
            if ( formExercice != null)
              session.setAttribute(Constants.PRE_EXTRACT_FORM,formExercice);
           
            
            return mapping.findForward(SUCCESS);
        
          } catch (Exception e) {
           throw e;
        }
        
        
    }


   /**
    *Renvoie une liste de Bean, un Bean est un couple (label,value)
    *A partir d'une liste de lexemes on récupère tous les lemmes de cette liste
    *@param lexemes vector de lexemes
    *@retunr liste de Bean 
    */
   private Vector getOnlyLemma(Vector lexemes) {
       if ( lexemes != null) {
         Vector v = new Vector();
         for (int i=lexemes.size()-1; i >= 0 ; i--) {
             Lexeme lex = (Lexeme)lexemes.get(i);
             Bean lemma = new Bean(lex.getLemma(),lex.getLemma());
             
             if (!v.contains(lemma))
               v.add(lemma);
         }
         // on va trie le vecteur : 
         
         return sortBean(v);
       } else {
           return null;
       }
   }
   
   
   private Vector sortBean(Vector v) {
      //on tri le vector du plus petit au plus grand.
          Vector sorted = new Vector();
          Enumeration eLex = v.elements();
          while (eLex.hasMoreElements()) {
            Bean bean = (Bean)eLex.nextElement();
            int indexToInsert = 0;
            boolean stop = false;
            for(int i=0; i < sorted.size() && !stop; i++) {
                //Bean beanSorted  = (Bean)sorted.get(i);
                if (bean.compareTo(sorted.get(i)) > 0 ) {
                   indexToInsert = i;  
                   stop = true;
                } else {
                     indexToInsert = i+1;
                }
           }
           sorted.add(indexToInsert, bean);
        }//end of eLex enum
       return sorted;
 }
  
   
   
   /**
    *Renvoie tous les temps utilisés dans une liste de Lexeme, pour connaître un temps on regarde les 
    *part of speech des lexemes
    *@param lexemes liste des lexemes
    *@param a analyse permettant des calculs sur les lexemes$
    *@return Vector de Bean
    */
   private Vector getOnlyTenses(Vector lexemes,Analysis a) {
       if ( lexemes != null) {
           Vector v = new Vector();
             for (int i=lexemes.size()-1; i >= 0 ; i--) {
             Lexeme lex = (Lexeme)lexemes.get(i);
               if (a.isVerb(lex)) {
                 Iterator ite = lex.getAllSenses().iterator();
                 while (ite.hasNext()) {
                     Sense sense =(Sense)ite.next();
                     Bean tense = new Bean(sense.getPos(),sense.getPos());
                     if (!v.contains(tense))
                         v.add(tense);
                  }
               }
             }
           return v;
       } else {
           return null;
       }
   }
}
