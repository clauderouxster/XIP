
package extui.UI_html;

import extui.UI_components.LoginService;
import extui.UI_components.LoginInfo;
import extui.UI_components.UserSession;
import org.apache.log4j.Logger;
import org.apache.wicket.PageParameters;
import org.apache.wicket.markup.html.basic.Label;
import org.apache.wicket.markup.html.form.Button;
import org.apache.wicket.markup.html.form.Form;
import org.apache.wicket.markup.html.form.PasswordTextField;
import org.apache.wicket.markup.html.form.TextField;
import org.apache.wicket.markup.html.panel.FeedbackPanel;
import org.apache.wicket.model.CompoundPropertyModel;
import org.apache.wicket.model.StringResourceModel;

/**
 *
 * @author ctrojahn
 */
public class Login extends BasePage {

    private static final Logger logger = Logger.getLogger(Login.class.getName());
    private static final long serialVersionUID = 1L;

    /**
     *
     */
    public Login() {
        super();

        Label titleForm = new Label("titleForm", new StringResourceModel("labelFormLogin", this, null,"XXXX"));
        Label labelUserName = new Label("labelUserName", new StringResourceModel("labelUserName", this, null,"XXXX"));
        Label labelPassword = new Label("labelPassword", new StringResourceModel("labelPassword", this, null,"XXXX"));
        
        Button login = new Button("buttonLogin", new StringResourceModel("linkLogin", this, null,"XXXX"));
        Button clear = new Button("buttonClear", new StringResourceModel("labelButtonClear", this, null,"XXXX"));

        // Model corresponding to the form
        final LoginInfo loginInfo = new LoginInfo();
        // Feedback in case of login errors
        final FeedbackPanel feedback = new FeedbackPanel("feedback");
        add(feedback);

        @SuppressWarnings("unchecked")
        Form loginForm = new Form("loginForm", new CompoundPropertyModel(loginInfo)) {
            private static final long serialVersionUID = 1L;
            @Override
            protected void onSubmit() {
                LoginService loginService = new LoginService();
                if (loginService.authenticate(loginInfo)) {
                    // When user is validated, change the profile of the current session
                    ((UserSession)getSession()).setUserProfile(loginService.getValidatedUser());
                    Index1 home = new Index1();
                    setResponsePage(home);
                } else {
                    feedback.error("User name or password is not valid");
                }
            }
        };

        TextField username = new TextField("username");
        PasswordTextField password = new PasswordTextField("password");

        loginForm.add(titleForm);
        loginForm.add(username);
        loginForm.add(labelUserName);
        loginForm.add(password);
        loginForm.add(labelPassword);
        loginForm.add(login);
        loginForm.add(clear);
        add(loginForm);
    }

    /**
     *
     * @param params
     */
    public Login(PageParameters params) {
        //TODO:  process page parameters
    }
}

