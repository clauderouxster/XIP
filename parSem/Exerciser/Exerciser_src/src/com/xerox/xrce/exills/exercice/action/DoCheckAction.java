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

import javax.servlet.http.*;

import org.apache.struts.action.*;
import com.xerox.xrce.exills.*;
import com.xerox.xrce.exills.exercice.form.*;
import com.xerox.xrce.exills.exercice.*;


/**
 *
 * @author vgrassau
 * @version
 */

public class DoCheckAction extends ExerciserAction {
    
    
    /**
    
     */
    public ActionForward executeAction(ActionMapping mapping, ActionForm  form,HttpServletRequest request, HttpServletResponse response)throws Exception {
        try {
            //on recupere le formulaire
            CheckForm chekForm = (CheckForm)form; 
            if (!this.isUserPresent()) {
                     return sessionTimeout(mapping,request);
                 }
             // on recupere l'objet exercice dans la session 
             Exercice exercice = (Exercice)session.getAttribute(Constants.EXERCICE);
             Analysis a = exercice.getAnalysis();
               //on check tous les lemmes a cocher
              if ( request.getParameter("clear") != null && request.getParameter("clear").equals("clear")) {
                   makeChecking(null,null,a);
              } else {
               makeChecking(chekForm.getLemma(),chekForm.getTemps(),a);
              }
               String formExercice = a.writeHTMLExtraction();
               if ( formExercice != null)
                  session.setAttribute(Constants.PRE_EXTRACT_FORM,formExercice);
                     
               return mapping.findForward(SUCCESS);
          
            } catch(Exception e) {
                throw e;
            }
        
    }

   
    private void makeChecking(String[] lemma, String[] tenses,Analysis a) {
           a.setCheckLexeme(a.getAllLexemeInArray(),false);
           if ( lemma != null) {
              for (int i=0; i < lemma.length ; i++) {
                  a.setCheckLexeme(a.getLexeme(lemma[i]),true);
              }
           }
           if ( tenses != null) {
              for (int i=0; i < tenses.length ; i++) {
                a.setCheckLexeme(a.getAllPOS(tenses[i]),true);
              }    
           }
    }


     

    
    
    
    
}//end of class
