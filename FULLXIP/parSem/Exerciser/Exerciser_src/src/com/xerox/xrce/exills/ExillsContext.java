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
import javax.servlet.*;
import javax.servlet.http.HttpServlet;
import com.xerox.lg.*;
import com.xerox.xrce.exills.exercice.*;
import java.util.logging.*;
import java.util.*;

/**
 * Cette classe initialise le contexte de l'application web
 * Elle permet aussi d'initialiser le fichier de log de l'application
 * @author vgrassau
 */
public class ExillsContext extends HttpServlet implements ServletContextListener, Serializable  {
    
   private ServletContext context = null;
   private Logger log;
  
   /**
    *Met en place le fichier de log pour l'application web.
    *@see com.xerox.xrce.exills.resource.logging
    */
   public ExillsContext() {
     super();
     try {
          LogManager logManage = LogManager.getLogManager();
          logManage.readConfiguration(getClass().getResourceAsStream("/com/xerox/xrce/exills/resource/logging.properties"));
     } catch (Exception e) {
          System.err.println("ERROR for logger setup " + e );
          e.printStackTrace();
     }
      log = Logger.getLogger(getClass().getName()); 
   
   }
   
   
   /**
    *Initialise le context, insere dans le domaine de l'application, le
    *LanguageGuesser ainsi que l'ExerciceListener. 
    *Le languageguesser permet d'identifier la langue d'un texte.
    *L'ExerciceListener permet de lire les exerices qui ont ete crees.
   *
    *@param sce context event
    *
    *NB : si une initialisation est incorrecte alors l'application web n'est pas utilisable.
    
    *
    */
   public void contextInitialized(ServletContextEvent sce) {
    // on recupere le contexte de la servlet
    try {
        context = sce.getServletContext();
        Constants.APPLICATION_NAME = context.getServletContextName();
       
        log.info("Initialisation of " + Constants.APPLICATION_NAME);
        String version = ResourceBundle.getBundle("com/xerox/xrce/exills/resource/exerciser").getString("version");
        String debugLevel = ResourceBundle.getBundle("com/xerox/xrce/exills/resource/exerciser").getString("debug.level");
        try {
        Constants.DEBUG_LEVEL = Integer.parseInt(debugLevel);
        } catch (Exception e) {}
        
        context.setAttribute(Constants.VERSION,version);
        LanguageGuesser lg = (LanguageGuesser)context.getAttribute(Constants.LANGUAGE_GUESSER);
        //ExerciceListener el = new ExerciceListener(Constants.APPLICATION_NAME);
        if ( lg == null) {
           lg = new LanguageGuesser();
        }
        if ( lg !=  null ) {
          lg.initGuesser();
          context.setAttribute(Constants.LANGUAGE_GUESSER,lg);
          log.info("add "+ lg.getClass().getName() + " ok");
        //  context.setAttribute(Constants.EXERCICE_LISTENER,el);
          //log.info("add "+ el.getClass().getName() + " ok");
          log.info(Constants.APPLICATION_NAME + " is initilalized correctly");
        } else {
          log.severe("ERROR:ExillsContext : "+  Constants.APPLICATION_NAME + " is not initilalized correctly");
        }
  
    } catch (Exception e) {
       log.log(Level.SEVERE,"ERROR:ExillsContext : can't initialize application",e);
        
    }
   }

/**
 * methode executee a l'arret du serveur de l'application web
 * Suppression du LanguageGuesser et de l'ExerciceListener
 * @param sce ServletContextEvent evenements survenant sur le contexte de la servlet
 *
 */
public void contextDestroyed(ServletContextEvent sce) {
    try {
        context = sce.getServletContext();
        context.removeAttribute(Constants.LANGUAGE_GUESSER);
        context.removeAttribute(Constants.EXERCICE_LISTENER);
        context.removeAttribute(Constants.VERSION);
        log.info("Context of ExillsExercice is correctly cleaned");
    } catch (Exception e) {
        log.log(Level.SEVERE,"ERROR:ExilsContext : Context of ExillsExercice is not correctly cleaned",e);
       
    }
    
}
   
   
}//end of class
