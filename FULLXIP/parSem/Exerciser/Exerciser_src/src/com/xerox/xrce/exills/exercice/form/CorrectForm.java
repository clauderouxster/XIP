/*
 * CorrectForm.java
 *
 * Created on November 23, 2005, 10:22 AM
 */

package com.xerox.xrce.exills.exercice.form;

import com.xerox.xrce.exills.*;
import java.util.*;
/**
 *
 * @author vgrassau
 * @version
 */

public class CorrectForm extends org.apache.struts.action.ActionForm {
    
    private Vector lexeme;   
    
    
    public void setLexeme(int i,String value) {
        int newSize = lexeme.size() + 1;
        lexeme.setSize(newSize);
        lexeme.add(i-1,value);
    }
    
    
    
    public String getLexeme(int i) {
        int index = i-1;
        if ( index < lexeme.size() && index >= 0)
                return (this.lexeme.get(index)).toString();
        else 
              return new String();
    }
    
    
    public Vector getLexeme() {
        return this.lexeme;
    }
    
    
    public void setLexeme(Vector v) {
        this.lexeme = v;
    }
    
    /**
     *
     */
    public CorrectForm() {
        super();
        this.lexeme = new Vector();
        // TODO Auto-generated constructor stub
    }
    
}
