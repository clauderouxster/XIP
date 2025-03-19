/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package extui.UI_components;

import java.io.Serializable;
import extui.UI_html.Application;

/**
 *
 * @author ctrojahn
 */
public class LoginService implements Serializable {

    UserProfile validedUser;

    /**
     *
     * @param loginInfo
     * @param manager
     * @return
     */
    public boolean authenticate(LoginInfo loginInfo) {
        // Here, validate user name and password from database of users
        ManagerDB_Users manager = Application.getManagerDB_Users();
        UserProfile user = manager.getUserProfile(loginInfo.getUsername(), loginInfo.getPassword());
        boolean exists = false;
        if (user != null) {
            exists = true;
            this.validedUser = user;
        }
        return exists;
    }

    public UserProfile getValidatedUser() {
        return this.validedUser;
    }
}
