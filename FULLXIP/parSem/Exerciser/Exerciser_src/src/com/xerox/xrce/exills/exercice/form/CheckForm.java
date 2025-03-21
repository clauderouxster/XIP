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
import javax.servlet.http.*;
import org.apache.struts.action.*;
/**
 *
 * @author vgrassau
 * @version
 */

public class CheckForm extends ActionForm {
    
    private String[] temps; //the temps avaibles
    private String[] lemma; // the lemmas avaibles
   
    
    
    /***
     *Initializes the array of temp
     *@param temps, array of temps
     */
    public void setTemps(String[] temps) {
        this.temps = temps;
    }
    
    /***
     *Initializes the array of lemma
     *@param lemma array of lemma
     */
    public void setLemma(String[] lemma) {
        this.lemma = lemma;
    }
    
    /**
     *Return all the lemma
     *@param return an array of lemma
     */
    public String[] getLemma(){
        return lemma;
    }
    
    /** 
     *Returns all the temps
     *@return an array of strings
     */
    public String[] getTemps() {
        return this.temps;
    }
    /**
     *
     */
    public CheckForm() {
        super();
        // TODO Auto-generated constructor stub
    }
    
  
    
       
}
