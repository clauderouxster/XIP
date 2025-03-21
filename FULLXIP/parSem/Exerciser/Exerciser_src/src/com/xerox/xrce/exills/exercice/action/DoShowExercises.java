/*
 * DoShowExercises.java
 *
 * Created on March 13, 2006, 5:49 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.xerox.xrce.exills.exercice.action;
import javax.servlet.http.*;
import javax.servlet.*;
import org.apache.struts.action.*;
import com.xerox.xrce.exills.*;
import com.xerox.xrce.exills.exercice.form.*;
import com.xerox.xrce.exills.exercice.*;
import java.util.logging.*;

/**
 *
 * @author vgrassau
 */
public class DoShowExercises extends ExerciserAction {
    
     public ActionForward executeAction(ActionMapping mapping, ActionForm  form,HttpServletRequest request, HttpServletResponse response)            throws Exception {
       try {
           if (! isUserPresent()) {
               return sessionTimeout(mapping,request);
           }
           session.removeAttribute("PAGE_TEST");
           ServletContext context = servlet.getServletContext();
           info("Access to the ExerciserListener");
           
           ExerciceListener el = (ExerciceListener)session.getAttribute(Constants.EXERCICE_LISTENER);
              
           if ( form == null) {
               session.setAttribute("List_exercises",el.getList(el.SORT_BY_DATE));
           } else {
               ShowExercisesForm sf = (ShowExercisesForm)form;
               session.setAttribute("List_exercises",el.getList(sf.getType()));
           }
           
           return mapping.findForward(SUCCESS);
        } catch (Exception e) {
           throw e;
        }
        
            
        }
    
}
