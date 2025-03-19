/*
 * WicketExamplePage.java
 *
 * Created on 30 avril 2009, 10:38
 */
package com.myapp.wicket;

import org.apache.wicket.markup.html.WebPage;
import org.apache.wicket.markup.html.basic.Label;
import org.apache.wicket.model.IModel;
import org.apache.wicket.markup.html.resources.StyleSheetReference;
import org.apache.wicket.util.string.Strings;

/** 
 *
 * @author grondeau
 * @version 
 */
public class BasePage extends WebPage {

    /**
     * Constructor
     */
    public BasePage() {
        //this(null);
    }

    /**
     * Construct.
     * @param model
     */
    public BasePage(IModel model) {
        super(model);
        final String packageName = getClass().getPackage().getName();
        add(new HeaderPanel("mainNavigation", Strings.afterLast(packageName, '.')));
        add(new StyleSheetReference("stylesheet", BasePage.class, "style.css"));
        // add(new Label("message", "Hello World!"));  //This is working
        //This is nor working
        Task t1 = new Task();//this);
        //t1.run();
        new Thread(t1).start();
    }
}
