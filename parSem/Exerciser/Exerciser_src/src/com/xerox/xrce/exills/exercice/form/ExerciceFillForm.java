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

package com.xerox.xrce.exills.exercice.form;
import java.util.*;

/**
 *
 * @author vgrassau
 * @version
 */

public class ExerciceFillForm extends org.apache.struts.action.ActionForm {
    
    private  Map values = new HashMap();
    private String page = "";
    private String author ="";
    private String title ="";
    private String language ="";
    private String instruction = "";
    private long startTimeExercice = System.currentTimeMillis();

    public void setValue(String key, Object value) {
        values.put(key, value);
    }

    public Object getValue(String key) {
        return values.get(key);
    }
    
    public Map getValues() {
        return this.values;
    }
    
    public void setPage(String page) {
        this.page = page;
    }

     public String getPage() {
         return this.page;
     }

     public String getAuthor() {
         return this.author;
     }
     
     
     public String getTitle() {
         return this.title;
     }

     public String getLanguage() {
         return this.language;
     }

     
     public void setTitle(String title) {
         this.title = title;
     
     }
     
     public void setAuthor(String author) {
         this.author = author;
     }
     
     public void setLanguage(String language) {
         this.language = language;
      }
     
     public void setInstruction(String instruction) {
         this.instruction = instruction;
     }
     
     public String getInstruction() {
         return this.instruction;
     }
     
     
     public long getStartTimeExercice() {
         return this.startTimeExercice;
     }
}// end of class
