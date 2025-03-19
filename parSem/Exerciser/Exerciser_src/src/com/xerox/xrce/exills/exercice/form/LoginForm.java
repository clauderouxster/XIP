/*
 * LoginForm.java
 *
 * Created on October 7, 2005, 4:35 PM
 */

package com.xerox.xrce.exills.exercice.form;


import javax.servlet.http.HttpServletRequest;

import org.apache.struts.action.*;



/**
 *
 * @author vgrassau
 * @version
 */

public class LoginForm extends ActionForm {
    
    private String login;
    
    private String password;
    
    private String project;
    /**
     * @return
     */
    public String getLogin() {
        return login;
    }
    
    /**
     * @param string
     */
    public void setLogin(String string) {
        login = string;
    }
    
    
    public String getProject() {
        return project;
    }
    
    
    public void setProject(String project) {
        this.project = project;
    }
    
    /**
     * @return
     */
    public String getPassword() {
        return password;
    }
    
    /**
     * @param i
     */
    public void setPassword(String string) {
        this.password = string;
    }
    
    /**
     *
     */
    public LoginForm() {
        super();
        // TODO Auto-generated constructor stub
    }
    
    
      /**
     * Check to make sure the client enter a login and a password
     */
    public ActionErrors validate(ActionMapping mapping,HttpServletRequest request) {
            
        ActionErrors errors = null;
               
        if ( this.getLogin() == null || this.getPassword() == null ||
             this.getLogin().equals("") || this.getPassword().equals("") )  {
            errors = new ActionErrors();
            errors.add(ActionMessages.GLOBAL_MESSAGE , new ActionMessage("errors.form.login.valueNull"));
        }
        return errors;

    }

    
    
}
