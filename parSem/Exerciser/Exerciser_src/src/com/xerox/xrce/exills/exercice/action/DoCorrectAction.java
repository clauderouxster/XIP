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

import com.xerox.xrce.exills.exercice.form.*;
import com.xerox.xrce.exills.exercice.*;
import com.xerox.xrce.exills.*;
import java.io.File;

import javax.servlet.http.*;
import org.apache.struts.action.*;
import java.util.*;
import java.sql.*;

/**
 *
 * @author vgrassau
 * @version
 */

public class DoCorrectAction extends ExerciserAction {
    
    
    /**
     *
     */
    public ActionForward executeAction(ActionMapping mapping, ActionForm  form, HttpServletRequest request, HttpServletResponse response) throws Exception {
        
        try {
            long endTimeExercice = System.currentTimeMillis();
            ExerciceListener el = (ExerciceListener)session.getAttribute(Constants.EXERCICE_LISTENER);
            ExerciceFillForm exerciceFillForm = (ExerciceFillForm)form;
            Exercice exercice = (Exercice)session.getAttribute(Constants.EXERCICE);
            if (!this.isUserPresent()) {
                return sessionTimeout(mapping,request);
            }
            session.removeAttribute("PAGE_TEST");
            Analysis analysis = null;
            String page = exerciceFillForm.getPage();
            // si on a pas d'exercice dans la session alors va recreer l'analyse.
            if ( exercice != null && exercice.getExerciceFileName().equals(page))
                analysis = exercice.getAnalysis();
            else {
                String text = el.getText(el.getPathTexts() + File.separator + page +".txt");
                Object[] param = {exerciceFillForm.getLanguage()};
                analysis = AnalysisFactory.getInstance(param,text);
            }
            Map v =  exerciceFillForm.getValues();
            // creation de la correction
            Corrector corrector = new Corrector(v,analysis);
            corrector.eval();
            
            storeResultInDB(el.getIDExercise(page),corrector,exerciceFillForm.getStartTimeExercice(),endTimeExercice);
            
            session.setAttribute(Constants.CORRECTOR,corrector);
            //session.setAttribute("PAGE_CORRECTED"  ,el.FOLDER_EXERCICE+ "/" + user.getProject() + "/" +exerciceFillForm.getPage());
            session.setAttribute("PAGE_CORRECTED"  ,exerciceFillForm.getPage());
            session.setAttribute("TITLE_CORRECTED" ,exerciceFillForm.getTitle());
            session.setAttribute("AUTHOR_CORRECTED",exerciceFillForm.getAuthor());
            
            return mapping.findForward(SUCCESS);
            
            
        } catch(Exception e) {
            throw e;
        }
        
        
    }
    
    private boolean storeResultInDB(int id,Corrector c, long start, long end) throws Exception {
        boolean success= false;
        try {
            Connection con;
            if ( user.isConnected()) {
                con = user.getConnection();
            } else {
                con =  user.getDataSource().getConnection();
            }
            String sql = new String("INSERT INTO `exercises_results` ( `exercise_id`,`login`,`start_time`,`end_time`,`score` ) VALUES (?,?,?,?,?)");
            this.info(sql);
            PreparedStatement statement = con.prepareStatement(sql);
            statement.setInt(1,id);
            statement.setString(2,user.getLogin());
            statement.setDate(3,new java.sql.Date(start));
            statement.setDate(4,new java.sql.Date(end));
            statement.setFloat(5,c.getPercent());
            
            int res = statement.executeUpdate();
            success =true;
            
            
            
        } catch (Exception e) {
            success =false;
            throw e;
        }
        return success;
    }
    
}
