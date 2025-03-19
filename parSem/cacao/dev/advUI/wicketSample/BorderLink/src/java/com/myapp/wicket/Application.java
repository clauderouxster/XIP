/*
 * Application.java
 *
 * Created on January 14, 2009, 1:47 PM
 */
 
package com.myapp.wicket;           

import org.apache.wicket.protocol.http.WebApplication;
/** 
 *
 * @author ctrojahn
 * @version 
 */

public class Application extends WebApplication {

    public Application() {
    }

    public Class getHomePage() {
        return Page1.class;
    }
}
