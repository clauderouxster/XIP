/*
 * ShowExercisesForm.java
 *
 * Created on March 13, 2006, 6:18 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.xerox.xrce.exills.exercice.form;

import java.util.*;
import javax.servlet.http.*;
import org.apache.struts.action.*;
/**
 *
 * @author vgrassau
 */
public class ShowExercisesForm extends ActionForm {
    
    private int type = 0;
    /** Creates a new instance of ShowExercisesForm */
    public ShowExercisesForm() {}
     
    public void setType(int type) {
           this.type = type;
       }
    
    public int  getType() {
           return this.type;
       }
    
    
}
