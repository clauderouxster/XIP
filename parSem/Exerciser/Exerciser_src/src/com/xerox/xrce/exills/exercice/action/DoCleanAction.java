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

/**
 *
 * @author vgrassau
 * @version
 */

public class DoCleanAction extends ExerciserAction {
    
   
    /**
    
     */
    public ActionForward executeAction(ActionMapping mapping, ActionForm  form, HttpServletRequest request, HttpServletResponse response)
            throws Exception {
               try {
                 if (!this.isUserPresent()) {
                     return sessionTimeout(mapping,request);
                 }
            
                   session.removeAttribute(Constants.EXERCICE);
                return mapping.findForward(CLEAN);    
           
            } catch(Exception e) {
                throw e;
            }
        
        
    }
}
