package extui.UI_components;

import java.io.Serializable;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.Table;
import javax.persistence.Transient;
import org.apache.log4j.Logger;

/**
 *
 * @author ctrojahn
 */

@Entity
@Table(name = "UserProfile")
@NamedQueries({
    @NamedQuery(name="getUserProfile", 
                query = "SELECT user FROM UserProfile user WHERE user.login = :login AND user.password = :password")
})
public class UserProfile implements Serializable {

    @Transient
    private static final long serialVersionUID = 1L;

    @Transient
    private static final Logger logger = Logger.getLogger(UserProfile.class.getName());

    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    private long userID;

    @Column
    private String login;
    @Column
    private String password;
    @Column
    private String languageInterface;
    @Column
    private String fileCSS;
    @Column
    private boolean visibleLanguageSearch;
    @Column
    private boolean visibleLanguageResults;
    @Column
    private boolean visibleAdvancedOptions;
    @Column
    private String defaultLanguageSearch;
    @Column
    private String defaultLanguageResults;
    @Column
    private String defaultAdvancedOptions;
    @Column(length=2000)
    private String bookmarks;


    public UserProfile() {
        
    }

    public UserProfile(String login, String password) {
           this.login = login;
           this.password = password;
    }


    public long getID() {
           return userID;
    }

    public String getDefaultAdvancedOptions() {
        return defaultAdvancedOptions;
    }

    public String getDefaultLanguageResults() {
        return defaultLanguageResults;
    }

    public String getDefaultLanguageSearch() {
        return defaultLanguageSearch;
    }

    public String getFileCSS() {
        return fileCSS;
    }

    public String getLanguageInterface() {
        return languageInterface;
    }

  


    public String getLogin() {
        return login;
    }

    public String getPassword() {
        return password;
    }

    public boolean isVisibleAdvancedOptions() {
        return visibleAdvancedOptions;
    }

    public boolean isVisibleLanguageResults() {
        return visibleLanguageResults;
    }

    public boolean isVisibleLanguageSearch() {
        return visibleLanguageSearch;
    }

    public void setLanguageInterface(String languageInterface) {
        this.languageInterface = languageInterface;
    }

    public void setDefaultAdvancedOptions(String defaultAdvancedOptions) {
        this.defaultAdvancedOptions = defaultAdvancedOptions;
    }

    public void setDefaultLanguageResults(String defaultLanguageResults) {
        this.defaultLanguageResults = defaultLanguageResults;
    }

    public void setDefaultLanguageSearch(String defaultLanguageSearch) {
        this.defaultLanguageSearch = defaultLanguageSearch;
    }

    public void setFileCSS(String fileCSS) {
        this.fileCSS = fileCSS;
    }

    public void setLogin(String login) {
        this.login = login;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public void setVisibleAdvancedOptions(boolean visibleAdvancedOptions) {
        this.visibleAdvancedOptions = visibleAdvancedOptions;
    }

    public void setVisibleLanguageResults(boolean visibleLanguageResults) {
        this.visibleLanguageResults = visibleLanguageResults;
    }

    public void setVisibleLanguageSearch(boolean visibleLanguageSearch) {
        this.visibleLanguageSearch = visibleLanguageSearch;
    }

    public static UserProfile createUserDefault(String locale) {
        UserProfile user =  new UserProfile();
        user.setLogin("default");
        logger.info("LOCALE USER DEFAULT: " + locale);
        user.setDefaultLanguageSearch(locale);
        user.setDefaultLanguageResults("en;fr;it");
        user.setDefaultAdvancedOptions("systran=false;SVexp=false;WNexp=false;W2Cexp=false");
        user.setVisibleLanguageSearch(true);
        user.setVisibleLanguageResults(true);
        user.setVisibleAdvancedOptions(false);
        user.setFileCSS("styleUI.css");
        return user;
    }

    public String getBookmarks() {
        return bookmarks;
    }

    public void setBookmarks(String bookmarks) {
        this.bookmarks = bookmarks;
    }


}
