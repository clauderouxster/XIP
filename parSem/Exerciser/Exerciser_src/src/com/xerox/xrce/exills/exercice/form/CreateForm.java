/*
 * CreateForm.java
 *
 * Created on October 7, 2005, 4:44 PM
 */

package com.xerox.xrce.exills.exercice.form;

import org.apache.struts.action.*;
import javax.servlet.http.*;
/**
 *
 * @author vgrassau
 * @version
 */

public class CreateForm extends ActionForm {
    
   private String title;
   private String comment;
   private String author;
   private String text;
   private String instruction;
    
   public void setTitle(String string)  {
       title = string;
   }
   
   public void setComment(String string)  {
       comment = string;
   }
   
   public void setAuthor(String string)  {
       author = string;
   }
   
   public void setText(String string)  {
       text = string;
   }
   
   public String getTitle()  {
       return title;
   }
   
   public String getComment()  {
       return comment;
   }
   
   public String getAuthor()  {
       return author;
   }
   
   public String getText()  {
       return text;
   }
   
   public String getInstruction() {
       return this.instruction;
   }
   
   public  void setInstruction(String instruction) {
       this.instruction = instruction;
   }
   /**
     *
     */
    public CreateForm() {
        super();
        // TODO Auto-generated constructor stub
    }
    
        /**
     * Check to make sure the client enter a login and a password
     */
    public ActionErrors validate(ActionMapping mapping,HttpServletRequest request) {
            
        ActionErrors errors = new ActionErrors();;
               
        if ( this.getTitle() == null || this.getTitle().equals("") ) {
            errors.add(ActionMessages.GLOBAL_MESSAGE , new ActionMessage("errors.form.createExercice.title.valueNull"));
        } else if (getTitle().contains("'") ||
                   getTitle().contains("*") ||
                   getTitle().contains("\"")||
                   getTitle().contains("/") ||
                   getTitle().contains("?") ||
                   getTitle().contains("<") ||
                   getTitle().contains(">") ||
                   getTitle().contains("|") || 
                   getTitle().contains(":")
                ) {
              errors.add(ActionMessages.GLOBAL_MESSAGE , new ActionMessage("errors.form.createExercice.title.invalidValue"));
        }
        
        
        if ( this.getText() == null || this.getText().equals("")) {
            errors.add(ActionMessages.GLOBAL_MESSAGE , new ActionMessage("errors.form.createExercice.text.valueNull"));
        }
        if ( this.getComment() == null || this.getComment().equals("")) {
            errors.add(ActionMessages.GLOBAL_MESSAGE , new ActionMessage("errors.form.createExercice.description.valueNull"));
        }
        if ( this.getInstruction() == null || this.getInstruction().equals("")) {
            errors.add(ActionMessages.GLOBAL_MESSAGE , new ActionMessage("errors.form.createExercice.intruction.valueNull"));
        }
        
        
        
        return errors;

    }
    
    
}
