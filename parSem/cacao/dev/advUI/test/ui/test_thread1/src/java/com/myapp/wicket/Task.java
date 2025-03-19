/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.myapp.wicket;

import org.apache.wicket.markup.html.WebPage;
import org.apache.wicket.markup.html.basic.Label;

/**
 *
 * @author grondeau
 */
public class Task extends BasePage implements Runnable {
    private BasePage basePage;

    Task(){//BasePage basePage) {
        super();
        //this.basePage = basePage;

    }
    
    //WicketMessage: Unable to find component with id 'message' in [Page class = com.myapp.wicket.HomePage, id = 0, version = 0].
    //This means that you declared wicket:id=message in your markup,
    //but that you either did not add the component to your page at all, or that the hierarchy does not match.
    public void run() {
        System.out.print("RUNNNNNNNNNNNNNNNNNNNNNN");
        this.add(new Label("message", "Hello World!"));
       
    }

 

}
