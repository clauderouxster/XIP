/*
 * MetaExercise.java
 *
 * Created on March 28, 2006, 6:30 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.xerox.xrce.exills.exercice;

import java.util.*;
import java.io.*;
/**
 * Classe representant un exercice tel qu'il est stocke dans la table 
 * exerciser_list. 
 * @author vgrassau
 */
public class MetaExercise {
    
    public static String TITLE ="file_name";
    public static String AUTHOR ="author";
    public static String LEVEL ="level";
    public static String LOGIN ="login";
    public static String TOPIC ="topic";
    public static String DESCRIPTION ="description";
    public static String PROJECT ="projet";
    public static String LANGUAGE ="language";
    public static String DATE ="date";
    public static String CATEGORIES ="categories";
    
    /**
     *proprietes de l'exercice
     */
    private Hashtable properties; 
    /**
     *id unique de l'exercice 
     */
    private int id; 
    /**
     *fichier JSP de l'exerice
     */
    private File file; 
    
    /**
     *Creation d'un MetaExercicse 
     *@param id numero de l'exercice dans la table
     *@param file fichier JSP de l'exercice
     */
    public MetaExercise(int id,File file) {
      this.id = id;
      this.properties = new Hashtable();
      this.file = file;
    }
    
    
    /**
     *Renvoie l'id de l'exercice
     *@return id 
     */
    public int getId() {
        return this.id;
    }
    
    /**
     *Initialise une propriete de l'exercice
     *@param name nom de la propriete
     *@param property objet pour la valeur de la propriete
     *
     *NB : les parametres ne doivent pas etre null sinon une exception sera levee
     */
    public void setProperty(String name,Object property) {
        properties.put(name,property);
    }
    
    /**
     *Renvoie la propriete de nom name de l'exercice
     *@return un objet null si la propriete n'est pas trouvee
     */
    public Object getProperty(String name) {
        return properties.get(name);
    }
    
    /**
     *Verifie si le fichier JSP de l'exercice existe reellement sur un support physique
     *selon le contexte ou il a ete donne lors de l'appel du constructeur. 
     *@return true s'il existe false sinon
     * 
     *@see MetaExercise(int, File)
     *
     */
    public boolean exists() {
        if ( file != null)
          return this.file.exists();
        else 
          return false;
    }
    
    /**
     *Initialise le niveau de l'exercice
     *@param level entier indiquant le niveau de difficulte de l'exercice
     *@see setProperty(String,Object)
     */
    public void setLevel(int level) {
        this.setProperty(LEVEL,new Integer(level));
    }

    
    public String getLevel() {
        Integer level = (Integer)getProperty(LEVEL);
        if (level != null) {
            return level.toString();
        }
        return null;
    }
    
    public String getDate() {
        java.sql.Date date = (java.sql.Date)this.getProperty(DATE);
        return date.toString();
    }

    
    public void setDate(java.sql.Date date) {
        this.setProperty(DATE,date);
    }
    
    
    public String getCategories() {
        return (String)getProperty(CATEGORIES);
    }

    public void setProperties(Hashtable properties) {
        this.properties = properties;
    }
    
    public void setCategories(String cat) {
        this.setProperty(CATEGORIES,cat);
    }
    
    
    public String getProject() {
        return (String)getProperty(PROJECT);
    }

    public void setProject(String project) {
        this.setProperty(PROJECT,project);
    }
    
    public String getTitle() {
      try {  
          String fileName = file.getName();
          String[] meta = fileName.split("_");
          return  meta[0];
        } catch (Exception e) {
            return getPage();
        }
    }
    
    public String getPage() {
      return this.file.getName();  
    }
    

    public void setTitle(String title) {
        this.setProperty(TITLE,title);
    }
    
    public String getLanguage() {
        return (String)getProperty(LANGUAGE);
    }

    public void setLanguage(String language) {
        setProperty(LANGUAGE,language);
    }
    
    public String getAuthor() {
        return (String)getProperty(AUTHOR);
    }
    
    public void setAuthor(String author) {
        setProperty(AUTHOR, author);
    }
    
    public String getTopic() {
        return (String)getProperty(TOPIC);
    }
    
    public void setTopic(String topic) {
        setProperty(TOPIC,topic);
    }
    
    
    public String getLogin() {
        return (String)getProperty(LOGIN);
    }

    public void setLogin(String login) {
        this.setProperty(LOGIN,login);
    }
    
    public String getDescription() {
        return (String)getProperty(DESCRIPTION);
    }

     public void setDescription(String desc) {
         this.setProperty(DESCRIPTION, desc);
     }
}//end
