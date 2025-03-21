/*
 * DoViewCorrectionAction.java
 *
 * Created on January 20, 2006, 6:05 PM
 */

package com.xerox.xrce.exills.exercice.action;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.struts.action.Action;
import org.apache.struts.action.ActionForm;
import org.apache.struts.action.ActionMapping;
import org.apache.struts.action.ActionForward;
/**
 *
 * @author vgrassau
 * @version
 */

public class DoViewCorrectionAction extends ExerciserAction {
    
  
    /**
     * This is the action called from the Struts framework.
     * @param mapping The ActionMapping used to select this instance.
     * @param form The optional ActionForm bean for this request.
     * @param request The HTTP Request we are processing.
     * @param response The HTTP Response we are processing.
     * @throws java.lang.Exception
     * @return
     */
    public ActionForward executeAction(ActionMapping mapping, ActionForm  form,
            HttpServletRequest request, HttpServletResponse response)
            throws Exception {
        
        return mapping.findForward(SUCCESS);
        
    }
}
