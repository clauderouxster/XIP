/*
 * WicketExamplePage.java
 *
 * Created on January 13, 2009, 4:06 PM
 */
 
package com.myapp.wicket;           

import org.apache.wicket.markup.html.WebPage;
import org.apache.wicket.model.IModel;
import org.apache.wicket.markup.html.resources.StyleSheetReference;
import org.apache.wicket.util.string.Strings;
import org.apache.wicket.markup.html.basic.Label;

/** 
 *
 * @author ctrojahn
 * @version 
 */

public class BasePage extends WebPage {

    /**
     * Constructor
     */
    public BasePage() {
        this(null);
    }
    
    /**
     * Construct.
     * @param model
     */
    public BasePage(IModel model) {
        super(model);
        //final String packageName = getClass().getPackage().getName();

        add(new HeaderPanel("mainNavigation","CACAO Project"));

        add(new StyleSheetReference("stylesheet", BasePage.class, "style.css"));
    }
}
