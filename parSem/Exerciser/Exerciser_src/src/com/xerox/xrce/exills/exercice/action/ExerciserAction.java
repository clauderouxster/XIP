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

package com.xerox.xrce.exills.exercice.action;

import javax.servlet.http.*;
import javax.servlet.*;
import java.util.*;
import org.apache.struts.action.*;
import com.xerox.xrce.exills.*;
import com.xerox.xrce.exills.exercice.form.*;
import com.xerox.xrce.exills.exercice.*;
import java.util.logging.*;

/**
 * Cette classe permet de creer des actions dans l'exerciser. 
 * Afin de personnaliser son action, la classe est abstraite et le developpeur 
 * doit implementer la methode executeAction 
 *
 * cette classe est utile car elle permet la gestion du fichier de log. Et ajoute 
 * automatiquement les exceptions capturees pour etres lue par la balise <html:errors/>
 * et par la page d'erreur de l'Exerciser. 
 * 
 * NB : pour connaitre la localisation du fichier de log, il faut se referer au fichier properties 
 * loggin.properties se trouvant dans le package: com.xerox.xrce.exills.resource
 *
 * @version 1.1
 * @author vgrassau
 */
public abstract class ExerciserAction extends Action {
    
    /**
     * gestion des log pour l'action
     */
    protected Logger log;  
    /**
     *Liste des erreurs capte par la page d'erreur de l'application
     */
    protected Errors errorsList;  
    /**
     *Liste des erreurs captees par la balise struts : <html:errors/>
     */
    protected ActionErrors errors;  
    /**
     *forward a renvoyer au controleur
     */
    protected ActionForward forward; 
    /**
     *Session 
     */
    protected HttpSession session; 
    /**
     *L'utilisateur utilisant l'exerciser pour la session en cours
     */
    protected User user;
    
  
       
    /** Construit une action 
     * le forward est a null
     *
     * @see org.apache.struts.action.Action;
     */
    public ExerciserAction() {
      super();
      
      log = Logger.getLogger(getClass().getName());
      errorsList = new Errors();
      errors = new ActionErrors();
      forward = null;   
      
      
    }
        
    /**
     *Ecrit dans le journal le fait que l'action a echoue et rajoute le message d'erreur associe a l'echec
     *@param failure message d'erreur expliquant l'echec
     *
     *NB : le nom de l'action est calcule dynamiquement a l'aide de l'appel des fonctions getClass() et getName()
     *
     * @see com.xerox.xrce.exills.exercice.action.ExerciserAction#warning(String)
     * @see com.xerox.xrce.exills.exercice.action.ExerciserAction#logExecuted()
     */
    public void logFailed(String failure) {
        warning(getClass().getName() + " failed : " + failure);
    }
    
    /***
     *Ecrit dans le journal le fait que l'action est executee et, ecrit aussi le nom de l'utilisateur qui lance 
     *l'execution si l'utilisateur est connu dans la session
     *
     * *NB : le nom de l'action est calcule dynamiquement a l'aide de l'appel des fonctions getClass() et getName()
     *
     * @see com.xerox.xrce.exills.exercice.action.ExerciserAction#fine(String)
     * @see com.xerox.xrce.exills.exercice.action.ExerciserAction#logFailed(String)
     */
     public void logExecuted(){
         info(getClass().getName() + " is executed.");
     }
    
     /**
      *Ecrit dans le journal le message avec un niveau de type fine.
      *on rajoute le login de l'utilisateur s'il est non null
      *@version 1.1
      *@param message message a ecrire
      *
      *bogue rencontre avec la version 1.0 pas d'ecriture dans le journal
      *avec la version 1.1, le message est ecrit mets avec un niveau INFO. 
      *
      *@see java.util.logging.Logger#fine(String) 
      *@see java.util.logging.Logger#info(String)
      *
      *@todo corriger le bogue afin d'obtenir un message de niveau FINE et non INFO
      */
     public void fine(String message) {
         if ( user != null) {
           // bogue rencontrer les messages en fine ne s'ecrivent pas dans le journal, alors que level est a ALL
           //log.fine(message + " (" + user.getLogin() + " )");
           log.info(message + " (" + user.getLogin() + " )");
         }  else  {
           //log.fine(message);  
           log.info(message);  
         }
     }
     
     
     /**
      *Ecrit dans le journal le message avec un niveau de type fine.
      *on rajoute le login de l'utilisateur s'il est non null
      *@param message message a ecrire
      *
      *@see java.util.logging.Logger#fine(String)
      *@see java.util.logging.Logger#warning(String)
      */
     public void info(String message) {
         if ( user != null)
           log.info(message + " (" + user.getLogin() + " )");
         else 
           log.info(message);  
     }
     
     /**
      *Ecrit dans le jounral le message avec un niveau de type warning.
      *on rajoute le login de l'utilisateur s'il est non null
      *@param message message a ecrire
      *
      **@see java.util.logging.Logger#info(String)
      **@see java.util.logging.Logger#fine(String)
      */
     public void warning(String message) {
         if ( user != null)
           log.warning(message + " (" + user.getLogin() + " )");
         else 
           log.warning(message);  
     }
     
     
     /**
      *Ecrit dans le jounral le message avec un niveau de type severe.
      *on rajoute le login de l'utilisateur s'il est non null et la pile d'execution jusqu'a l'erreur
      *@param message message a ecrire
      *@param e exception rencontree donant suite a ce message
      *
      */
     public void severe(String message,Exception e) {
         if ( user != null)
           log.log(Level.SEVERE,message + " (" + user.getLogin() + " )",e);
         else 
           log.log(Level.SEVERE,message,e);
     }
     
     
     /***
      *Initialise le forward pour cette action
      *@param f forward passer au controleur afin de changer d'action ou de vue
      */
      public void setForward(ActionForward f) {
          this.forward = f;
      }
    
      /**
       *Renvoie le forward de l'action
       *@return forward pour le passer au controleur
       */
      public ActionForward getForward() {
          return forward;
      }
      
      
      public  boolean isUserPresent() {
          user = (User)session.getAttribute(Constants.USER);
          return (user != null);
      }
      
      /**
       *Cette methode statique permet de donnee une valeur stockee dans le 
       *fichier properties de l'exerciser. 
       *@param key cle a chercher dans le fichier properties
       *@return la valeur de la cle 
       */
     static public String getResource(String key) {
        return ResourceBundle.getBundle("com/xerox/xrce/exills/resource/exerciser").getString(key);
    }
    
      
      /**
       *Methode appele par execute
       *Cette methode est abstraite, elle doit etre implementee par le developpeur afin 
       *qu'il personalise son action pour l'exerciser.
       *
       *@param mapping mapping permettant de retourner un ActionForward et donc de changer d'action
       *@param form formulaire 
       *@param request requete 
       *@param response reponse
       *@return un forward que le controleur recupere
       *
       *@throws Exception on force la methode a jeter une exception si un probleme survient
       *
       */
      public abstract ActionForward executeAction(ActionMapping mapping, ActionForm  form,HttpServletRequest request, HttpServletResponse response)throws Exception;
      
       /**
       *Methode appele par le controleur struts
       *Cette methode fait un appel a la methode executeAction.
       *qu'il personalise son action pour l'exerciser.
       *On regarde si la session est toujours vivante. Si elle est null alors on forward vers LOGIN
       *sinon on recupere l'utilisateur afin d'ameliorer les differents messages pour le journal
       * de l'application.
       *
       *@param mapping mapping permettant de retourner un ActionForward et donc de changer d'action
       *@param form formulaire 
       *@param request requete 
       *@param response reponse
       *@return un forward que le controleur recupere
       *
       *@throws Exception on force la methode a jeter une exception si un probleme non capture ou non traite survient
       *
       *@see com.xerox.xrce.exills.exercice.action.ExerciserAction#executeAction(ActionMapping,ActionForm,HttpServletRequest,HttpServletResponse)
       *
       */
      public  ActionForward execute(ActionMapping mapping, ActionForm  form,HttpServletRequest request, HttpServletResponse response)throws Exception {
          try {
               ActionMessages msg = this.getErrors(request);
               if (msg != null) {
                   msg.clear();
               }
             
              //la methode suivante va instancier la session si elle est vivante
               if (!isSessionAlive(request)) {
                  //return mapping.findForward(LOGIN);  
                   return sessionTimeout(mapping,request);
                  
               }
               user = (User)session.getAttribute(Constants.USER);
               session.removeAttribute(Constants.ERRORS);
               logExecuted();
                
               return executeAction(mapping,form,request,response);
          
          } catch (Exception e) {
              logFailed(e.toString());
              catchException(e,request); 
              return mapping.findForward(FAILURE);
          }
      }
    
     /**
      *Ajoute l'exception dans la requete.
      *Le message d'erreur renvoye sera un message standard, c a d considere comme une 
      *erreur de developpement
      *
      *Quand une exception est capture on utilise deux façon pour presenter cette exception : 
      *
      *- on ajoute l'erreur dans la liste des erreurs, cette liste sera afficher sur la page d'erreur.
      * le message sera aolr plus precis car on affiche une partie la pile d'execution.
      
      *- on ajoute le message d'erreur dans la requete de facon qu'il soit vivisble par la balise 
      * struts <html:errors/> 
      *
      *@param e Exception que l'on a capture
      *@param request requete a laquelle on ajoute l'exception et son message d'erreur associee
      *
      **@see com.xerox.xrce.exills.exercice.action.ExerciserAction#catchException(Exception,HttpServletRequest)
      */
     protected void catchException(Exception e,String keyErrorPage,String keyMessage, HttpServletRequest request) {
         if ( e!=null)
           severe(keyErrorPage,e);
         else 
           warning(keyErrorPage);
         errorsList.addErrorObject(new ErrorObject(keyErrorPage,e));
         session.setAttribute(Constants.ERRORS,errorsList);
         errors.add(ActionMessages.GLOBAL_MESSAGE,new ActionMessage(keyMessage));
         
         addErrors(request,errors);
     }
  
     
     
     public ActionForward sessionTimeout(ActionMapping mapping,HttpServletRequest request) {
            logFailed("session null");
            catchException(new Exception("session timeout"),"errors.session.timeout","errors.session.timeout",request);
            return mapping.findForward(LOGIN);
     }
     
     public void cleanSession() {
         if ( session != null) {   
            Enumeration enum1 = session.getAttributeNames();
            while (enum1.hasMoreElements()) {
               session.removeAttribute(enum1.nextElement().toString());
            }
         }
     }
     
     /**
      *Methode qui ajoute une erreur, le nom de lerreur pour la page d'erreur est celui 
      *utiliser par defaut par la cle errors.exception, ainsi que pour le message d'erreur avec la cle 
      *errors.occured
      *
      *@param e exception a rajouter
      *@param request requete 
      *
      *@see com.xerox.xrce.exills.exercice.action.ExerciserAction#catchException(Exception,String,String,HttpServletRequest)
      */

     protected void catchException(Exception e,HttpServletRequest request) {
         catchException(e,"errors.exception","errors.ocurred", request);
     }
    
     
     /**
      *Verifie si la session en cours est toujours "vivante" c a d differente de null.
      *Si elle est null alors on l'ecrit dans le journal et on cree les differentes erreurs.
      *
      @param request requete permettant d'acceder a la session
      *@return true si la session est non null, false sinon
      *
      *NB : Avec cette methode on instancie l'attribut session de l'action, si elle existe !!! 
      *@version 1.1
      */
     
     public boolean isSessionAlive(HttpServletRequest request) {
         session = request.getSession(false);
         if (session == null ||!request.isRequestedSessionIdValid()) {
//               logFailed("session null");
//               catchException(new Exception("session timeout"),"errors.session.timeout","errors.session.timeout",request);
               return false;
          } else {
             return true;
          }
         
     }
     
     //-------------------------------------------------------------------------------------------- 
     // valeur des forwards a utiliser
     
     /**
      *Echec de l'action
      */
     public static final String FAILURE = "failure";
    /**
     *Succes de l'action
     */
     public static final String SUCCESS = "success";
    /**
     *a utiliser pour rediriger vers une page de login
     */
     public static final String LOGIN = "login";
      
       
    
    static public final String CLEAN = "clean";
     
     
}//end of class
