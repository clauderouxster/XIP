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
import javax.servlet.*;
import org.apache.struts.action.*;
import com.xerox.xrce.exills.*;
import com.xerox.xrce.exills.exercice.form.*;
import com.xerox.xrce.exills.exercice.*;
import java.util.logging.*;

/**
 * L'action d'analyser permet la creation d'exercice ainsi que l'analyse du texte de l'exercice.
 * @author vgrassau
 * @version
 */

public class DoAnalyseAction extends ExerciserAction {
    
     
    /**
     *Cette methode vas analyser un texte pour un exercice. 
     *L'exercie va donc etre pret pour sa creation, et sera place dans la session
     */
    public ActionForward executeAction(ActionMapping mapping, ActionForm  form,HttpServletRequest request, HttpServletResponse response)            throws Exception {
       try {
           if (! isUserPresent()) {
               return sessionTimeout(mapping,request);
           }
           
           ServletContext context = servlet.getServletContext();
           info("Access to the language guesser");
           Object[]  param = new Object[] {context.getAttribute(Constants.LANGUAGE_GUESSER)};
              
           CreateForm createForm = (CreateForm)form;
                   
            // create exercice
            Exercice exercice = new Exercice(user,createForm.getTitle(),createForm.getComment());      
            exercice.setAuthor(createForm.getAuthor());
            exercice.setText(createForm.getText());
            exercice.setInstruction(createForm.getInstruction());
            exercice.analyseText(param);
            //TODO :add type of exercice
               
            //insert the exercice and the analyse in the session 
             session.setAttribute(Constants.EXERCICE,exercice);
             switch (exercice.getType()) {
                    default: {
                    return mapping.findForward(SUCCESS);
                    
                    }
                }
           
        } catch (Exception e) {
           throw e;
        }
        
            
        }
}//end of class
