/*
 * DoGotoExerciseAction.java
 *
 * Created on April 6, 2006, 4:59 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.xerox.xrce.exills.exercice.action;

import javax.servlet.http.*;
import javax.servlet.*;
import org.apache.struts.action.*;
import com.xerox.xrce.exills.*;
import com.xerox.xrce.exills.exercice.form.*;
import com.xerox.xrce.exills.exercice.*;
import org.apache.commons.dbcp.*;
import java.util.logging.*;
import java.sql.*;
import javax.sql.*;
import java.io.*;


/**
 * 2 parametres passer le nickname et l'exercice
 * @author vgrassau
 */
public class DoGotoExerciseAction extends ExerciserAction {
    
    
    public ActionForward executeAction(ActionMapping mapping, ActionForm  form,HttpServletRequest request, HttpServletResponse response)throws Exception {
        try {
            info(getClass().getName() + " executed : ");
            session = request.getSession();
            cleanSession();
            String nickname = request.getParameter("nickname");
            String exercise = request.getParameter("exercise");
            String project =  request.getParameter("PROJECT");
            
            user = new User(session.getId());
            user.setLogin(nickname);
            user.setProject(project);
            
            if ( nickname == null || exercise == null ||project == null) {
                catchException(new Exception(),"errors.exerciser.parameter","errors.exerciser.parameter",request);
                return mapping.findForward(FAILURE);
            }
            
            if ( constructUser(request)){
                //on cherche la page jsp de l'exercice :
                //insert the user in the session object
                session.setAttribute(Constants.USER,user);
                ExerciceListener el = new ExerciceListener(Constants.APPLICATION_NAME,user);
                
                log.info("add in session"+ el.getClass().getName() + " ok");
                if ( !exercise.endsWith("jsp")) {
                    exercise =  getJSPExercise(Integer.parseInt(exercise));
                }
                if ( exercise == null || el.getIDExercise(exercise)==-1) {
                    catchException(new Exception(),"errors.exerciser.unknown.exercise","errors.exerciser.unknown.exercise",request);
                    return mapping.findForward(FAILURE);
                }
                session.setAttribute(Constants.EXERCICE_LISTENER,el);
                
                
                File src = new File(el.getPathExercice()+ "/" + exercise);
                String page_test =  "/"+ user.getProject() +"/" + ExerciceListener.FOLDER_EXERCICE  + "/" + exercise;
                
                File dst = new File(System.getenv("CATALINA_HOME") + "/webapps/Exerciser" + page_test);
                
                File parent = dst.getParentFile();
                parent.mkdirs();
                
                if (!dst.exists()) {
                    InputStream in = new FileInputStream(src);
                    OutputStream out_ = new FileOutputStream(dst);
                    
                    // Transfer bytes from in to out
                    byte[] buf = new byte[1024];
                    int len;
                    while ((len = in.read(buf)) > 0) {
                        out_.write(buf, 0, len);
                    }
                    
                    in.close();
                    out_.close();
                }
                
                
                
                
                
                session.setAttribute("PAGE_TEST",page_test);
                
                return mapping.findForward(SUCCESS);
                
            } else {
                warning("error : " + nickname +"is unknwon for the database");
                catchException(new Exception(),"errors.exerciser.login","errors.exerciser.login",request);
                return mapping.findForward(FAILURE);
            }
        } catch(Exception e) {
            throw e;
        }
        
    }
    
    private boolean constructUser(HttpServletRequest request) throws Exception  {
        Connection con = null;
        
        try {
            BasicDataSource dSource = new BasicDataSource();
            dSource.setDriverClassName(getResource("driver.database"));
            dSource.setUrl(getResource("url.database." + user.getProject()));
            dSource.setUsername(getResource("user.database"));
            dSource.setPassword(getResource("password.database"));
            con = dSource.getConnection();
            if ( con!=null) {
                info("conncetion was created on " + dSource.getUrl());
                user.setConnection(dSource,con);
                String sql = new String("SELECT * FROM users WHERE login=?");
                PreparedStatement statement = con.prepareStatement(sql);
                info("get information about " + user.getLogin());
                statement.setString(1,user.getLogin());
                
                ResultSet rs = statement.executeQuery();
                if (rs.first()) {
                    user.setName(rs.getString("last_name"));
                    user.setFirstname(rs.getString("first_name"));
                    user.setProfile(rs.getString(8));
                    user.setConnection(dSource,con);
                    user.setPassword(rs.getString("password"));
                    user.setLogin(rs.getString("login"));
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
        } catch (Exception e) {
            throw e;
        }
    }
    
    private String getJSPExercise(int id) throws Exception {
        try {
            Connection con;
            String page = null;
            if ( user.isConnected()) {
                con = user.getConnection();
            } else {
                con =  user.getDataSource().getConnection();
            }
            info("trying to found the page of exercices with id " + id);
            String sql = new String("SELECT file_name FROM exerciser_list WHERE exercise_id = ? ");
            PreparedStatement statement = con.prepareStatement(sql);
            statement.setInt(1,id);
            
            ResultSet rs  = statement.executeQuery();
            while (rs.next()) {
                page = rs.getString(1);
            }
            rs.close();
            con.close();
            
            if ( page != null) {
                info("Page with id " + id + " was found : " + page);
            } else {
                warning("Page with id " + id + " was not found");
            }
            return page;
        } catch (Exception e) {
            throw e;
        }
        
    }
    
    
}
