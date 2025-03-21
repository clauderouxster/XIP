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

/**
 *
 * @author vgrassau
 * @version
 */

public class GenerateForm extends org.apache.struts.action.ActionForm {
    
    private String[] blank;
    
  
    public void setBlank(String[] blanks) {
        this.blank = blanks;
    } 
    
    
    public String[] getBlank() {
        return this.blank;
    }
    
    
    
    /**
     *
     */
    public GenerateForm() {
        super();
        // TODO Auto-generated constructor stub
    }
    
     public ActionErrors validate(ActionMapping mapping,HttpServletRequest request) {
            
        ActionErrors errors = new ActionErrors();;
               
        if ( this.getBlank() == null  ) {
            errors.add(ActionMessages.GLOBAL_MESSAGE , new ActionMessage("errors.form.generate.check.valueNull"));
        }
        
        return errors;

    }
    
}
