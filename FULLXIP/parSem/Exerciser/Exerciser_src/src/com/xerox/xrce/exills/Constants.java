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

import org.apache.struts.action.*;
import java.io.*;
/**
 * This class contains some constants for the application exerciser. 
 * @author vgrassau
 */
public class Constants implements Serializable {

    static public  String APPLICATION_NAME = "Exerciser";
    
    /** constant for Apllication objects */
    
    static public final String VERSION = "VERSION";
    static public final String LANGUAGE_GUESSER = "LANGUAGE_GUESSER";
    static public final String EXERCICE_LISTENER = "EXERCICE_LISTENER";
    
    static public final String EXERCISER_HOME = System.getenv("EXERCISER_HOME");
    
    
    
    /** constants for objects of session or request */
    /** the user represented for a session */
    static public final String USER = "USER";
    static public final String CORRECTOR = "CORRECTOR";
    /** the exercice to create */
    static public final String EXERCICE = "EXERCICE";
    static public final String EXERCICE_JSP_LIST = "EXERCICE_JSP_LIST";
    /** Generally this constants does a reference in the scope request and the objet is a Erros*/
    static public final String ERRORS ="ERRORS";
    
    static public final String PRE_EXTRACT_FORM = "PRE_EXTRACT_FORM";
    
    
    static public final String LEMMA_LIST = "LEMMA_LIST";
    static public final String TEMPS_LIST = "TEMPS_LIST";
    
    static public int DEBUG_LEVEL = 4;
    
     /** the global message when a error is detected */ 
    static public final ActionMessage ERROR_OCCURED = new ActionMessage("errors.occured");
    
    static public final String DATA_TOPDIR ="/Resources/Xelda/xelda_lingdata/export"; 
    static public final String CONFIG_XF_XML = "/Resources/config/morphotag.xml";
    
    static public final String LANGUAGE_FORM = "full";
    

    static public final String LANGUAGE_METHODE = "trig";
    
    
    /** Creates a new instance of Constants */
    public Constants() {
    //nothing to do
    }
    
}
