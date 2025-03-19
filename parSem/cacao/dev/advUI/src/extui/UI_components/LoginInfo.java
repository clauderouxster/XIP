
package extui.UI_components;

import java.io.Serializable;
import org.apache.log4j.Logger;

/**
 *
 * @author ctrojahn
 */
public class LoginInfo implements Serializable {

    private static final Logger logger = Logger.getLogger(LoginInfo.class.getName());
    private String username;
    private String password;

    /**
     *
     * @return
     */
    public String getUsername() {
        return username;
    }

    /**
     *
     * @return
     */
    public String getPassword() {
        return password;
    }

    /**
     *
     * @param username
     */
    public void setUsername(String username) {
        this.username = username;
    }

    /**
     *
     * @param password
     */
    public void setPassword(String password) {
        this.password = password;
    }
}
