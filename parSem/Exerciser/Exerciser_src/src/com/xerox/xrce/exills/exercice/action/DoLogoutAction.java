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


import org.apache.struts.action.*;
import com.xerox.xrce.exills.*;
import com.xerox.xrce.exills.exercice.*;
import com.xerox.xrce.exills.exercice.form.*;
import java.util.*;

/**
 *
 * @author vgrassau
 * @version
 */

public class DoLogoutAction extends ExerciserAction {
    
   
    /**
     * This is the action called from the Struts framework.
     * @param mapping The ActionMapping used to select this instance.
     * @param form The optional ActionForm bean for this request.
     * @param request The HTTP Request we are processing.
     * @param response The HTTP Response we are processing.
     * @throws java.lang.Exception
     * @return
     */
    public ActionForward executeAction(ActionMapping mapping, ActionForm  form,HttpServletRequest request, HttpServletResponse response) throws Exception {
          try {
              String project = user.getProject();
              if (user.isConnected()) {
                  user.getConnection().close();
              }
               cleanSession();
               session.invalidate();       
               info("session was invalidated");
               if ( project.equalsIgnoreCase("vowel")) 
                  return mapping.findForward("success_vowel");    
               else 
                  return mapping.findForward("success_xrce");    
           
            } catch(Exception e) {
               throw e;
            }
        
    }
}//end of class
