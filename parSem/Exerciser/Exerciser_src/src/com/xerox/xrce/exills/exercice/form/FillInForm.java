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

import org.apache.struts.action.*;
import javax.servlet.http.*;
import java.util.*;

/**
 *
 * @author vgrassau
 * @version
 */

public class FillInForm extends ActionForm {
    
   private String[] categories; // the categories to extract
   private int level = 1; // the level of the exercie
     
  
   /**
    *Initializes the set of categorie to extract
    *@param categories an array of names
    */
   public void setCategories(String[] categories) {
      this.categories = categories;
  }

   /**
    *returns the categories to extract
    *@return an array 
    */
   public String[] getCategories() {
       return this.categories;
   }
   
   /**
    *Initializes the level for an exercice fill
    *@param level 
    */
   public void setLevel(int level) {
       this.level = level;
   }
   
   /**
    *returns the level of the exerice
    *@param return a level
    */
   public int getLevel() {
        return this.level;
   }
   /**
     *Create a new instance of a FillInForm
     */
    public FillInForm() {
        super();
       
    }
    
    
       /**
     * Check to make sure the client enter a login and a password
     */
    public ActionErrors validate(ActionMapping mapping,HttpServletRequest request) {
            
        ActionErrors errors = null;
        String[] cat =this.getCategories();        
        if ( cat == null || cat.length == 0 )  {
            errors = new ActionErrors();
            errors.add(ActionMessages.GLOBAL_MESSAGE , new ActionMessage("errors.form.fillInForm.valueNull"));
        }
        return errors;

    }
    
    
}
