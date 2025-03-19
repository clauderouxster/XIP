package extui.UI_components;

/**
 *
 * @author ctrojahn
 */
import java.util.Properties;
import java.util.Locale;
import org.apache.log4j.Logger;
import org.apache.wicket.Request;
import org.apache.wicket.protocol.http.WebSession;

/**
 *
 * @author ctrojahn
 */
public class UserSession extends WebSession {

    private static final Logger logger = Logger.getLogger(UserSession.class.getName());
    private UserProfile userProfile;
    
    /**
     *
     * @param request
     */
    public UserSession(Request request) {
        super(request);
        userProfile = null;
        Locale locale = getLocale();
        logger.info("language: " + locale.getISO3Language());

    }

    protected void onEndRequest() {
        logger.trace("onEndRequest-------");
        invalidate();
    }

    public synchronized UserProfile getUserProfile() {
        return userProfile;
    }



    public synchronized void setUserProfile(UserProfile userProfile) {
        this.userProfile = userProfile;
    }

   
    public synchronized boolean isAuthenticated() {
        return (userProfile != null);
    }

    
}
