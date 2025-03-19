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
import org.apache.commons.dbcp.*;
import javax.sql.*;
import java.sql.*;
import java.util.logging.*;

/**
 *
 * @author vgrassau
 * @version
 */

import java.util.*;

public class DoLoginAction extends ExerciserAction {
    
    /**
     */
    public ActionForward executeAction(ActionMapping mapping, ActionForm  form, HttpServletRequest request, HttpServletResponse response)throws Exception {
     
        try {
            //cast : from AcionFrom to LoginForm
            LoginForm loginForm = (LoginForm)form;
            //check the login
            info(getClass().getName() + " executed : " + loginForm.getLogin());
            session = request.getSession();
            cleanSession(); 
            if (Constants.DEBUG_LEVEL >= 4 ) 
                    session.setMaxInactiveInterval(120);
            
            user = new User(session.getId());
            user.setLogin(loginForm.getLogin());
            user.setPassword(loginForm.getPassword());
            user.setProject(loginForm.getProject());
            if ( checkLogin(request)){
                //insert the user in the session object
                session.setAttribute(Constants.USER,user);
                ExerciceListener el = new ExerciceListener(Constants.APPLICATION_NAME,user);
                log.info("add in session"+ el.getClass().getName() + " ok");
                session.setAttribute(Constants.EXERCICE_LISTENER,el);
                return mapping.findForward(SUCCESS);    

            } else {
                  return mapping.findForward(FAILURE);    
            }
            } catch(Exception e) {
                throw e;
            }
}

    
    /**
     *Check the login and return if it's correct or not.
     *@param loginForm the login form to check
     *@return true if the login is valide false otherwise
     */
    private boolean checkLogin(HttpServletRequest request) throws Exception {
       Connection con = null;
        try {
           BasicDataSource dSource = new BasicDataSource();
           dSource.setDriverClassName(getResource("driver.database"));
           dSource.setUrl(getResource("url.database." + user.getProject()));
           dSource.setUsername(getResource("user.database"));           
           dSource.setPassword(getResource("password.database"));
           con = dSource.getConnection();
           if ( con!=null) {
              info("conncetion was created");
              user.setConnection(dSource,con);
              String sql = new String("SELECT * FROM users WHERE login= ? and password = ?");
              
              PreparedStatement statement = con.prepareStatement(sql);
              statement.setString(1,user.getLogin());
              statement.setString(2,user.getPassword());
              
              ResultSet rs = statement.executeQuery();
              if (rs.first()) {
                user.setName(rs.getString("last_name"));
                user.setFirstname(rs.getString("first_name"));
                user.setProfile(rs.getString(8));
                
              } else {
                  //con.close();
                  return false;
              }
              rs.close();
              //con.close();
              return true;
          } else {
               catchException(new Exception(),"errors.database.connection","errors.database.connection",request);
               return false;
          }
       } catch (SQLException e) {
            catchException(e,"errors.exerciser.login","errors.exerciser.login",request);
            return false;
       }
       catch (Exception e) {
         throw e;
      }
   }

    

    
    
    
    public static void main(String args[] ) {
         try {
           
            
          BasicDataSource dSource = new BasicDataSource();
        dSource.setDriverClassName(getResource("driver.database"));
           dSource.setUrl(getResource("url.database"));
           dSource.setUsername(getResource("user.database"));           
           dSource.setPassword(getResource("password.database"));
          Connection con = dSource.getConnection();
          if ( con!=null) {
              System.out.println("-----------------------------------\n conncetion reussie");
         
          } else {
              System.out.println("-----------------------------------\n conncetion ECHOUEE");
         
          }
         
        } catch (Exception e) {
         System.out.println("-----------------------------------\n connection ECHOUEE " + e);
         
         
        }
    }
    
    

}//end of class
