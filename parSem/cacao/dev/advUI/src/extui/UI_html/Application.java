/*
 * Xerox Research Center Europe
 * Copyright (C) 2009 XRCE, All Rights Reserved
 */
package extui.UI_html;

import extui.UI_components.Constant;
import extui.UI_components.ManagerDB_Users;
import extui.UI_components.UserProfile;
import extui.UI_components.UserSession;
import java.util.Properties;
import org.apache.log4j.Logger;
import org.apache.wicket.Request;
import org.apache.wicket.Response;
import org.apache.wicket.Session;
import org.apache.wicket.protocol.http.WebApplication;

/** 
 * @author grondeau
 * @version 
 */
public class Application extends WebApplication {

    private static final Logger logger = Logger.getLogger(Application.class.getName());
    // One object per Application (all users share this object)
    private static ManagerDB_Users managerDB_users;
    private static String rootWsUrl;

    /**
     *
     */
    public Application() {
        //do not do any init here, use rather init()
    }

    /**
     * This method is called for each new Application by wicket
     * It's the official place for the application initialization
     */
    @Override
    protected void init() {
        //logger.debug("init -----------------------");
        //IResourceSettings IResourceSettingsttings = getResourceSettings();
        if (DEPLOYMENT.equalsIgnoreCase(getConfigurationType())) {
            //deployment mode
            rootWsUrl = "http://localhost:8080";
        } else {
            //development mode
            //rootWsUrl = "http://demo.celi.it:8095";  //195.110.142.254
            //http://demo.celi.it:8095/CacaoSuite/services/CACAO_queryWS
            // avant rootWsUrl = "http://213.215.252.8:8080";
            //rootWsUrl = "http://195.110.142.254:8095";
            // TO DO: check if parameter in web.xml is considered
            //rootWsUrl = "http://localhost:8080";
            rootWsUrl = "http://www.cross-library.com:8080";
        }
        Constant.initializeMapping();
        managerDB_users = new ManagerDB_Users();
        managerDB_users.newDB(new Properties(), true);
        // Test ---------------------------------------
        UserProfile profile = new UserProfile();
        profile.setLogin("cacao");
        profile.setPassword("cacao");
        profile.setFileCSS("styleUI.css");
        profile.setLanguageInterface("en");
        profile.setDefaultLanguageSearch("en");
        profile.setDefaultLanguageResults("en;fr;it");
        profile.setDefaultAdvancedOptions("systran=true;SVexp=false;WNexp=true;W2Cexp=true");
        profile.setVisibleLanguageSearch(true);
        profile.setVisibleLanguageResults(true);
        profile.setVisibleAdvancedOptions(true);
        managerDB_users.persistUserProfile(profile);
        // Test ----------------------------------------
    }

    /**
     *
     * @return
     */
    public Class getHomePage() {
        return Index1.class;
    }

    public static String getRootWsUrl() {
        return rootWsUrl;
    }

    /**
     *
     * @param request
     * @param response
     * @return
     */
    @Override
    public synchronized Session newSession(Request request, Response response) {
        //logger.debug("--------------------------------------------------new session");
        UserSession userSession = new UserSession(request);
        logger.debug("session locale=" + userSession.getLocale());
        // Profile "default"
        UserProfile profile = UserProfile.createUserDefault(userSession.getLocale().toString().substring(0, 2));
        userSession.setUserProfile(profile);
        getMarkupSettings().setDefaultMarkupEncoding("UTF-8");
        return userSession;
    }

    /**
     * 
     * @return
     */
    public static ManagerDB_Users getManagerDB_Users() {
        return managerDB_users;
    }
}
