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

import java.io.*;
import org.apache.commons.dbcp.*;
import java.util.logging.*;
import javax.sql.*;
import java.sql.*;
/**
 * This class permits to represent a user connected to the application web. 
 * Only a user with a specific profile can make an exercice.
 * @author vgrassau
 */
public class User implements Serializable {
    
    /**
     *Fied for the real name of the user
     */
    private String name;
    /**
     *Field for the firstname of the user
     */
    private String firstname;
    /**
     *Field for the idsession of the user
     */
    private String idsession;
    /**
     *Field for the password of the user
     */
    private String password;
    /**
     *Field for the login of the user
     */
     private String login;
    
     private String project;
     private String profile;
     
     private Connection connection;
     private BasicDataSource dataSource;
     
    //------------------------------------------------- constructors
    
    /** Creates a new instance of User, all the  
     *@param idsession the id session for this user
     */
    public User(String idsession) {
     setIdsession(idsession);
     setName("");
     setFirstname("");
     setLogin("");
     setPassword("");
     
    }
  
    /** 
     * Creates a new instance of user from a User, we make a copy.
     * @param user the user to copy 
     */
    public User(User user) {
        setIdsession(new String(user.getIdsession()));
        setName(new String(user.getName()));
        setFirstname(new String(user.getFirstname()));
        setLogin(new String(user.getLogin()));
        setPassword(new String(user.getPassword()));
    }
    
    

//----------------------------------------- methods ---------------------------
    
    public void setProject(String project) {
        this.project = project;
    }
    
    public String getProject() {
        return this.project;
    }
    
    /**
     *Renvoie le profil de l'utilisateur
     */
    public String getProfile() {
        return this.profile;
    }
    
    
    
    public void setConnection(BasicDataSource data,Connection con) {
        this.dataSource = data;
       setConnection(con);
    }
    
     public void setConnection(Connection con) {
        this.connection = con;
    }
    
    
    
    
    public BasicDataSource getDataSource() {
        return this.dataSource;
    }
    
    
    public Connection getConnection() {
        try {
           return this.connection;
        } catch (Exception e) {
            return null;
        }
    }
    
    public boolean isConnected() {
      try {
          if ( connection != null ) {
            return !connection.isClosed(); 
        } 
      } catch (Exception e) {
          return false;
      }
      return false;  
    }
    
    
    public void setProfile(String profile) {
        this.profile = profile;
    }
    
    /**
     *Initialize le nom de l'utilisateur
     *@param name nom de l'utilisateur
     */
    public void setName(String name) {
        this.name = name;
    }
    
    /**
     *initialize the user firstname
     *@param firstname the user's firstname 
     */
    public void setFirstname(String firstname) {
        this.firstname = firstname;
    }
    
    /**
     *initialize the idsession for this user
     *@param the idssession of the connection
     */
    public void setIdsession(String idsession) {
        this.idsession = idsession;
    }
    
    /**
     *Initialize the password for this user
     *@param password the user's password
     */
    public void setPassword(String password) {
        this.password = password;
    }
    
    /**
     *Initialize the login for this user.
     *@param login the login which permitted the connection
     */
    
    public void setLogin(String login) {
        this.login = login;
    }
    
    /**
     *Return the login
     *@return login of the connection
     */
    public String getLogin() {
        return this.login;
    }
    
    /**Return the password not encrypted 
     *@return the password (not encrypted)
     */ 
    public String getPassword() {
        return this.password;
    }
    
    /**
     *Return the idsession of this user
     *@return the idsession 
     */
    public String getIdsession() {
        return this.idsession;
    }
    
    /**
     *Return the name of this user
     *@return user's name
     */
    public String getName() {
        return this.name;
    }
    
    /**
     *Return the firstname of this user
     *@return user's firstname
     */
    public String getFirstname() {
     return this.firstname;
    }
    
    
   
    
}//end of the class
