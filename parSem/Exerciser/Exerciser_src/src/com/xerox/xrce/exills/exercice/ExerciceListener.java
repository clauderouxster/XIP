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

package com.xerox.xrce.exills.exercice;

import com.xerox.xrce.exills.*;
import java.util.*;
import java.io.*;
import org.apache.commons.dbcp.*;
import javax.sql.*;
import java.sql.*;
import com.xerox.xrce.exills.exercice.action.*;
import java.util.logging.*;

/**
 * Ecouteur sur la liste d'exerices cree par les professeurs. 
 * @version 1.0 
 * @author vgrassau
 * @todo : reorganiser la structure de stockage des exercices de façon a pointer uniquement vers les exercices propres
 * a un projet. 
 * exemple pour Vowel, il ne faudrait lister que les exercices cree par les utilisateurs de Vowel
 * 
 */
public class ExerciceListener implements Serializable {
    
    /**
     *Dossier des exercices
     */
    public final static  String FOLDER_EXERCICE = "exercises";
    /**
     *Dossier des textes qui ont permis de creer les exercices
     */
    public final static  String FOLDER_TEXT = "texts";
    
   
    
    public static final int SORT_BY_AUTHOR   = 1;
    public static final int SORT_BY_LANGUAGE = 2;
    public static final int SORT_BY_DATE     = 3;
    public static final int SORT_BY_LEVEL    = 4;
    public static final int SORT_BY_TITLE    = 5;
    public static final int ONLY_LOGIN       = 6;
    
    /**liste de MetaExercise */
    private Vector list; 
    /**
     *Table nom exercice --> idExercice 
     */
    private Hashtable idTable;
    
    /**
     * repertoire racine des exercices
     */
    private File root; 
    /**
     *Utilisateur connecte
     */
    private User user;
    /**
     *log
     */
    private Logger log = Logger.getLogger(getClass().getName());
    
    /** 
     * Instancie l'ecouteur d'exercices. 
     * @param applicationName nom de l'application afin de positionner le repertoire parent pour trouver 
     *                        les exercices et les textes
     * @throws Exception probleme lors de la lecture du repertoire 
     */
    public ExerciceListener(String applicationName,User user) throws Exception {
        try { 
            this.user = user;
            
            this.list = new Vector();
            this.idTable = new Hashtable();
         if ( Constants.EXERCISER_HOME == null ) {
             throw new Exception("EXERCISER_HOME is undefined");
         } else {        
           root = new File(Constants.EXERCISER_HOME + File.separator + "Projects" + File.separator  + user.getProject());
           //if ( !root.exists()) {
               root.mkdirs();
           //}
           //create the exercices folder and the texts folder : 
           File f_exercices  = new File(root.getAbsolutePath() + File.separator + FOLDER_EXERCICE);
           f_exercices.mkdirs();
           File f_texts      = new File(root.getAbsolutePath() + File.separator + FOLDER_TEXT);
           f_texts.mkdirs(); 
           
          readDatabase(SORT_BY_DATE);
         }
        } catch (Exception e) {
            throw e;
        }
    }
    
    
    private  boolean readDatabase(int type) {
        //connection a la bse de donnees : 
        try {
            Connection con;
            if ( user.isConnected()) {
                con = user.getConnection();
            } else {
                log.warning(user.getLogin() +" is not connected trying to reconnect...");
                con = user.getDataSource().getConnection();
                
           }
                     
             String sql = new String("SELECT * FROM exerciser_list order by date");
              
             switch( type) {
               case SORT_BY_LEVEL: {
                 sql =new String("SELECT * FROM exerciser_list order by level");
                 break;
            }
            case SORT_BY_AUTHOR: {
               sql =new String("SELECT * FROM exerciser_list order by author");
                break;
            }
            case SORT_BY_LANGUAGE: {
               sql =new String("SELECT * FROM exerciser_list order by language");
                break;
            }
            case SORT_BY_TITLE: {
                sql =new String("SELECT * FROM exerciser_list order by file_name");
                break;
            }
            case SORT_BY_DATE: {
               sql =new String("SELECT * FROM exerciser_list order by date");
                break;
            }
            case ONLY_LOGIN: {
               sql =new String("SELECT * FROM exerciser_list where login='"+ user.getLogin() + "' order by date");
                break;
            }
        }//end of switch
              
                          
              
              PreparedStatement statement = con.prepareStatement(sql);
              ResultSet rs = statement.executeQuery();
              list = new Vector();
              idTable = new Hashtable();
              while ( rs.next()) {
                  String title  = rs.getString(MetaExercise.TITLE);
                  File file = new File(getPathExercice() + File.separator + title);
                  if ( file.exists()) {
                    int id = rs.getInt("exercise_id");
                    this.idTable.put(title,new Integer(id));
                  
                    MetaExercise meta = new MetaExercise(id,file);
                    meta.setLogin(rs.getString(MetaExercise.LOGIN));
                    meta.setAuthor(rs.getString(MetaExercise.AUTHOR));
                    meta.setTitle(title);
                    meta.setTopic(rs.getString(MetaExercise.TOPIC));
                    meta.setProject(rs.getString(MetaExercise.PROJECT));
                    meta.setDescription(rs.getString(MetaExercise.DESCRIPTION));
                    meta.setLanguage(rs.getString(MetaExercise.LANGUAGE));
                    meta.setDate(rs.getDate(MetaExercise.DATE));
                    meta.setCategories(rs.getString(MetaExercise.CATEGORIES));
                    meta.setLevel(rs.getInt(MetaExercise.LEVEL));
                    
                    this.list.add(meta);
                  } else {
                      log.warning( " file : " + file.getAbsolutePath()+ " doesn't exist !!! ");
                  }
              }
              // si tout ce passe bien on reaffecte la connection a l'utilisateur
              user.setConnection(con);
            
            
        } catch (Exception e) {
            log.log(Level.SEVERE,user.getLogin() + " can not be connected or access to database " ,e);
            return false;
        }
       return true;    
    }
    
    
    public int getIDExercise(String fileName) {
        Integer id = (Integer)this.idTable.get(fileName);
        if ( id==null) {
            return -1;
        } else {
            return id.intValue();
        }
    }
    
   
    /**
     *Renvoie le chemin absolue du repertoire des textes
     *@return chemin abloslue où tous les \ sont remplaces par des / 
     */
    public String getPathTexts() {
       File dirText = new File( root.getAbsolutePath() + File.separator + FOLDER_TEXT);
       if (!dirText.exists()) {
           dirText.mkdir();
       }
       return dirText.getAbsolutePath();
    }
    
    /**
     *Renvoie la liste des exercices c a d les pages JSP se trouvant dans le repertoire FOLDER_EXERCICE 
     *@return un vector de MetaExercice. Les MetaExercice sont des objets pour connaitre le titre, 
     * la date, la langue, l'auteur d'un exercice.
     *
     *@see com.xerox.xrce.exills.exercice.ExerciceListener#check()
     */
    public Vector getList(int type) {
       if ( readDatabase(type)) {
           return this.list;
       } else {
           return new Vector();
       }
       
    }
    

   
    
    
    
    
    /**
     *Ecrit le texte associe a un exercice
     *@param page nom de la page JSP correspondant a l'exercice
     *@param text texte a ecrire
     *@throws Exception si on a une probleme lors de l'ecriture dans le fichier ou un objet null
     */
    public void writeTextFile(String page,String text) throws Exception {
       try {
          File f = new File(getPathTexts() + File.separator + page + ".txt");
          if ( !f.exists()) {
              f.createNewFile();
          }
          FileWriter fw = new FileWriter(f);
          BufferedWriter bufW = new BufferedWriter(fw);
          bufW.write(text);
        bufW.close();
       } catch (Exception e) {
           throw e;
       }
      
    }
    
    /**
     *Renvoie l'objet File racine pour lire les differents exercices
     *@return File, un repertoire 
     */
    public File getRoot() {
        return this.root;
    }

    /**
     *Renvoie le chemein absolue du repertoire ou se trouvent les exercices
     */
    public String getPathExercice() {
        if (root != null)
          return root.getAbsolutePath() + File.separator + FOLDER_EXERCICE;
        else 
          return null;
    }
    
    
    public static String getAuthor(String file) throws Exception{
        try {  
          File f = new File(file);
          String fileName = f.getName();
          String[] meta = fileName.split("_");
          return  meta[1];
        } catch (Exception e) {
            throw e;
        }
    }
    
    
    public static String getFileName(String file) {
          File f = new File(file.replace('\\','/'));
          return f.getName();
    }
    
    public static String getText(String file) throws Exception  {
        try {  
             File f = new File(file.replace('\\','/'));
             RandomAccessFile in = new RandomAccessFile(f,"r");
             byte[] buffer = new byte[(int)f.length()];
             in.readFully(buffer);
             in.close();
             return new String(buffer);
        } catch (Exception e) {
            throw e;
        }
    }
    
    
    public static String getTitle(File file) throws Exception {
      try {  
          File f = file;
          String fileName = f.getName();
          String[] meta = fileName.split("_");
          return  meta[0];
        } catch (Exception e) {
            throw e;
        }
    }
    
    public static String getDate(String file) throws Exception {
        try {
        File f = new File(file);
        String fileName = f.getName();
        String[] meta = fileName.split("_");
        String date[] = meta[2].split("--");
        String d = date[0].replace('-','/');
        String h = date[1];
        String h_ = h.split("\\.")[0];
        return d + " " + h_.replace('-',':');
        } catch (Exception e) {
            throw e;
        }
    }
    
    public static String getLanguage(String file) throws Exception {
        try {  
          File f = new File(file);
          String fileName = f.getName();
          String[] meta = fileName.split("_");
          //meta[3] est de type langue.jsp, il faut enlever le .jsp
          String[] lang = meta[3].split("\\.");
          return  lang[0];
        } catch (Exception e) {
            throw e;
        }
    }
    
  
    
    
    

    
    
    static public void main(String[] args) {
        String file = "C:/JAVA/Tomcat-5.5.12/webapps/exillsExercice/exercices/Fill-in-the-blank_naney_2005-12-05--11-36-46-255.jsp";
        try {
        String date = ExerciceListener.getDate(file);
        System.out.println(date);
        } catch (Exception e) {
            System.err.println("Error::ExerciceListener : " + e);
        }
    }
    
    
}//end of class
