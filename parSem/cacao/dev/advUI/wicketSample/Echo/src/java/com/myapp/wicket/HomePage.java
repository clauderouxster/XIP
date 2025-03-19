/*
 * HomePage.java
 *
 * Created on January 14, 2009, 3:41 PM
 */
 
package com.myapp.wicket;

import org.apache.wicket.markup.html.WebPage;
import org.apache.wicket.markup.html.basic.Label;
import org.apache.wicket.markup.html.form.Form;
import org.apache.wicket.markup.html.form.TextField;
import org.apache.wicket.model.PropertyModel;


public class HomePage extends WebPage
{

    private String message1 = "Message here.";
    

    public HomePage()
    {
           PropertyModel<String> messageModel = new PropertyModel<String>(this,"message");

           add(new Label("msg",messageModel));
           Form<?> form = new Form("form");
           form.add(new TextField<String>("input",messageModel));
           add(form);
    }

    public String getMessage()
    {
        return message1;
    }

    /**
     * @param message
     *            the message to set
     */
    public void setMessage(String message)
    {
        this.message1 = message;
    }


}
