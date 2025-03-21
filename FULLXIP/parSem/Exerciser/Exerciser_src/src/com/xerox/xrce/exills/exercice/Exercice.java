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
import com.xerox.lg.*;
import java.util.*;
import java.text.*;
import java.io.*;


/**
 *
 * @author vgrassau
 */
public class Exercice implements Serializable {
    
    /**
     *This field is the real user who creates the exercice, the user who was connected.
     *this field is required
     */
    protected User user;
    /**
     *Title of an exercice, this field is required
     */
    protected String title; 
    /**
     *The description for an exercice, this field is required
     *topics
     */
    protected String description; 
    /**The author of the exercice, this field is required
     */
    protected String author;
    /**The original text of the exercice, this field is required
     */
    protected String text;
    /**
     *The language of the text, this field is calculated by the LanguageGuesser of Xerox.
     */
    private String language;
    /**
     *The morphological analysis of the text.
     */
    protected Analysis analysis;
  
    /**
     *Instructions for the exercise
     */
    protected String instruction;
    /**
     *the type of this exercice
     */
    protected int type =0;
    
    /**
     *The index of exercice
     */
    protected String index ="-1";
    
    
    protected long date;
    
    //------------------------------------- Constructors
    
    /** Creates a new instance of Exercice 
     *@param user the user who creates the exrecice
     *@param title the title of this exercice
     *@param description the description of this exercice
     */
    public Exercice(User user,String title,String description) {
      this.user = user; 
      setTitle(title);
      setDescription(description);
      setAuthor(user.getLogin());
      setIndex();
   }
    
    
   //-------------------------------------- methods
    /**
     *Initialize the title of this exercice
     *@param string an explicit title
     */
    protected void setTitle(String string) {
        this.title = string.replaceAll("_"," ");
    }
    
    
    
    protected User getUser() {
        return this.user;
    }
    
    /**
     *initialize the description of this exercice
     *@param string description for the exercie.
     */
    protected void setDescription(String string) {
        this.description = string;
        
    }
    
    /**
     *Initialize the author for this exercice.
     *@param author
     */
    public void setAuthor(String author){
        this.author = author;
    }
    
    /***
     *Initialize the text of the exercice
     *@param text text for this exercice.
     */
    public void setText(String text) {
        this.text = text;
    }
    
   public long getDate() {
       return this.date;
   }
    
    /**
     *Retrun the language of this exercice.
     *@return The language of this exercice
     */
    public String getLanguage() {
        return this.analysis.getLanguage();
    }
    
    
    /**
     *Return the author of this exercice
     *@return auhthor
     */
    public String getAuthor() {
        return this.author;
    }
    
    /**
     *Return the title of this exercice
     *@return an explicit title
     */
    public String getTitle() {
        return this.title;
    }
    
    /***
     *Return the description of this exercice
     *@return description
     */
    public String getDescription() {
        return this.description;
    }
    
    public String getInstruction()  {
        return this.instruction;
    }
   
    public void setInstruction(String instruction) {
        this.instruction = instruction;
    }
     
 
    /**
     *Initialize the type of this exercice
     *@param type the type if this exercice
     */
    public void setType(int type) {
        this.type= type;
    }
    
    /**
     *Return the type of the exercice
     *@return type
     */
    public int getType() {
        return this.type;
}
    /**
     *This method permits to create an index for this exercice.
     *the index is calculated with the date of creation  
     */
    protected void setIndex() {
         date = System.currentTimeMillis();
         Date date_ = new Date(date);
         String sDate = new String();
         SimpleDateFormat sd = new SimpleDateFormat("yyyy-MM-dd--hh-mm-ss-SSS");
         sDate = sd.format(date_);
         index =  sDate;
    }
    
    /**
     *returns the index of the exercice
     *@return a string representing an index i.e. the idsession of the user and the date of creation
     */
    public String getIndex() {
        return this.index;
    }
    
    
    
    /**
     *Initializes the analysis of the text
     *@return true if the analysis was correctly loaded false otherwise
     */
    public void analyseText(Object[] param) throws Exception  {
        try {
          this.analysis = AnalysisFactory.getInstance(param,getText());
          
        }  catch (Exception e) {
            throw e;
        }
        
    }
    
    /**
     *Return the analysis for the text of this exercice
     *@return the analysis.
     */
    public Analysis getAnalysis() {
        return this.analysis;
    }
    
    
    /** 
     * return the original text of this exercice 
     * @return the text
     */
    public String getText() {
        return this.text ;
    }
    
    
    public String getExerciceFileName() {
       return  getTitle().replace(' ','-') + "_" + this.user.getLogin() + "_" + getIndex() + "_"+ getLanguage() +  ".jsp";
    }
    
}//end of class
