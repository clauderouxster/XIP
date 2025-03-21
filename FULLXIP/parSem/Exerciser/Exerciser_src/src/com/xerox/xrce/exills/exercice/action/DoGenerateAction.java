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
import com.xerox.xrce.exills.exercice.*;
import com.xerox.xrce.exills.exercice.form.*;
import javax.sql.*;
import java.sql.*;

/**
 *
 * @author vgrassau
 * @version
 */

public class DoGenerateAction extends ExerciserAction {
    
    
    /**
     * This is the action called from the Struts framework.
     *
     */
    public ActionForward executeAction(ActionMapping mapping, ActionForm  form, HttpServletRequest request, HttpServletResponse response)throws Exception {
        try {
            if (!this.isUserPresent()) {
                return sessionTimeout(mapping,request);
            }
            
            
            ExerciceListener el = (ExerciceListener)session.getAttribute(Constants.EXERCICE_LISTENER);
            
            GenerateForm generateForm = (GenerateForm)form;
            Object exercice = session.getAttribute(Constants.EXERCICE);
            
            if ( exercice instanceof ExerciceFill) {
                ExerciceFill exFill = (ExerciceFill)exercice;
                setLexemeToCheck(generateForm.getBlank(),exFill.getAnalysis());
                JSPExerciceFill jsp = new JSPExerciceFill(el.getPathExercice(),exFill);
                jsp.write();
                el.writeTextFile(exFill.getExerciceFileName(),exFill.getText());
                //mettre a jour la base de donnee
                addExerciseToDB(exFill);
            } else {
                catchException(new Exception("unknown type Exercice : " + exercice.getClass().getName()),"errors.unknown.exercice","errors.unknown.exercice",request);
                return mapping.findForward(FAILURE);
            }
            
            
            return mapping.findForward(SUCCESS);
            
            
        } catch(Exception e) {
            throw e;
        }
        
    }
    
    /**
     *Check lexeme in Analysis to generate the correct JSP.
     *@param idLex array of idLexeme to check
     *@param a Analysis permitting to check the lexemes
     */
    private void setLexemeToCheck(String[] idLex, Analysis a) {
        a.setCheckLexeme(a.getAllLexemeInArray(),false);
        for (int i=0; i < idLex.length ; i++) {
            Lexeme l = a.getLexeme(Integer.parseInt(idLex[i]));
            l.setChecked(true);
        }
        
    }
    
    
    private boolean  addExerciseToDB(ExerciceFill exFill) throws Exception {
        try {
            Connection con;
            if ( user.isConnected()) {
                con = user.getConnection();
            } else {
                con =  user.getDataSource().getConnection();
            }
            //    StringBuffer sql = new StringBuffer("INSERT INTO `exerciser_list` ( `exercise_id`,`description`,`file_name`,`level`,`date`,`author`,`login`, `language` ,`projet`,"+
            //                                         "`topic` ,`categories`) VALUES ( '', ?,?,?,?,?,?,?,?,?,?)" );
            StringBuffer sql = new StringBuffer("INSERT INTO `exerciser_list` ( `description`,`file_name`,`level`,`date`,`author`,`login`, `language` ,`projet`,"+
                    "`topic` ,`categories`) VALUES ( ?,?,?,?,?,?,?,?,?,?)" );
//
            this.info(sql.toString());
            PreparedStatement statement = con.prepareStatement(sql.toString());
            statement.setString(1,exFill.getInstruction());
            statement.setString(2,exFill.getExerciceFileName());
            statement.setInt(3,exFill.getLevel());
            statement.setDate(4,new java.sql.Date(exFill.getDate()));
            statement.setString(5,exFill.getAuthor());
            statement.setString(6,user.getLogin());
            statement.setString(7,exFill.getLanguage());
            statement.setString(8,user.getProject());
            statement.setString(9,exFill.getDescription());
            statement.setString(10,exFill.getCategoriesString());
            
            int res = statement.executeUpdate();
            
            
            
            
        } catch (Exception e) {
            throw e;
        }
        return false;
    }
    
    
}//end of class
