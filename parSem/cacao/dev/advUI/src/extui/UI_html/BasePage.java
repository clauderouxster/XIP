
package extui.UI_html;

import extui.UI_components.UserProfile;
import extui.UI_components.UserSession;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import org.apache.wicket.PageParameters;
import org.apache.wicket.markup.html.WebPage;
import org.apache.wicket.markup.html.basic.Label;
import org.apache.wicket.markup.html.form.ChoiceRenderer;
import org.apache.wicket.markup.html.form.DropDownChoice;
import org.apache.wicket.markup.html.form.Form;
import org.apache.wicket.markup.html.link.BookmarkablePageLink;
import org.apache.wicket.markup.html.link.Link;
import org.apache.wicket.markup.html.resources.StyleSheetReference;
import org.apache.wicket.model.Model;
import org.apache.wicket.model.StringResourceModel;

/**
 *
 * @author ctrojahn
 */
public class BasePage extends WebPage {

    /**
     *
     */
    @SuppressWarnings({"unchecked", "unchecked"})
    public BasePage() {

        super();
        String fileCSS = ((UserSession) getSession()).getUserProfile().getFileCSS();
        add(new StyleSheetReference("stylesheet", Index1.class, fileCSS));

        Form form = new Form("form");

        Label titleLogin = new Label("titleLogin", new StringResourceModel("linkLogin", this, null,"XXXX"));
        Label titleLogout = new Label("titleLogout", new StringResourceModel("linkLogout", this, null,"XXXX"));

        BookmarkablePageLink settings = new BookmarkablePageLink("settings", Settings.class);
        BookmarkablePageLink libraries = new BookmarkablePageLink("linkLibraries", Libraries.class);
        BookmarkablePageLink partners = new BookmarkablePageLink("linkPartners", Partners.class);
        BookmarkablePageLink help = new BookmarkablePageLink("linkHelp", HelpPage.class);
        BookmarkablePageLink home = new BookmarkablePageLink("home", Index1.class);

        Label labelSettings = new Label("titleLinkSettings", new StringResourceModel("linkSettings", this, null,"XXXX"));
        Label labelLibraries = new Label("titleLinkLibraries", new StringResourceModel("linkLibraries", this, null,"XXXX"));
        Label labelPartners = new Label("titleLinkPartners", new StringResourceModel("linkPartners", this, null,"XXXX"));
        Label labelHelp = new Label("titleLinkHelp", new StringResourceModel("linkHelp", this, null,"XXXX"));
        Label labelHome = new Label("titleLinkHome", new StringResourceModel("linkHome", this, null,"XXXX"));

        Label selectLanguages = new Label("labelSelectLanguages", new StringResourceModel("labelSelectLanguages", this, null,"XXXX"));

        List<Locale> listLocales = new ArrayList();

        final DropDownChoice<Locale> languagesCombo = new DropDownChoice("languages", new Model<Locale>(), listLocales,new LocaleChoiceRenderer()) {
            private static final long serialVersionUID = 1L;
            @Override
            protected boolean wantOnSelectionChangedNotifications() {
                return true;
            }
            @Override
            protected void onSelectionChanged(final Object newSelection) {
                Locale newValue = (Locale) newSelection;
                ((UserSession) getSession()).setLocale(newValue);
                setResponsePage(Index1.class);
            }
        };

        // Initialization of components proprieties --------------------------------------------------------------------//

        // TO DO: put these options in a file
        listLocales.add(Locale.ENGLISH);
        listLocales.add(Locale.FRENCH);
        listLocales.add(Locale.GERMAN);
        listLocales.add(Locale.ITALIAN);
        listLocales.add(new Locale("nl", "NL"));
        listLocales.add(new Locale("pt", "BR"));

        Link login = new Link("login", new Model()) {
            private static final long serialVersionUID = 1L;
            @Override
            public void onClick() {
                // Go to page to login, which will ask for username and password
                setResponsePage(Login.class);
            }
        };

        Link logout = new Link("logout",new Model()) {
            private static final long serialVersionUID = 1L;
            @Override
            public void onClick() {
                /*  When the user click in logout, the user = "default".
                 *  The value "defaul" is used to configure the page (for instance, if login or logout is show or not -
                    see CONFIGURATION PAGE)
                 */
                String locale = ((UserSession) getSession()).getLocale().getDisplayLanguage();
                ((UserSession) getSession()).setUserProfile(UserProfile.createUserDefault(locale));
                setResponsePage(Index1.class);
            }
        };

        login.add(titleLogin);
        logout.add(titleLogout);

        settings.add(labelSettings);
        libraries.add(labelLibraries);
        partners.add(labelPartners);
        help.add(labelHelp);
        home.add(labelHome);


        form.add(settings);
        form.add(libraries);
        form.add(partners);
        form.add(help);
        form.add(login);
        form.add(logout);
        form.add(home);

        form.add(selectLanguages);
        form.add(languagesCombo);
        add(form);


        /* CONFIGURATION PAGE:
         * As for the options, the settings option must appear only if the user is different of "default" (see class
         * Application). Also, if the user is logged, the login must be replaced by logout
         */
        if (((UserSession) getSession()).getUserProfile().getLogin().equals("default")) {
            settings.setVisible(false);
            /* logout is not showed (not use of visible propriety because using visibility the
             * the space in the interface is used.
             */
            titleLogout.setVisible(false);
            titleLogin.setVisible(true);

        }
        else {
             settings.setVisible(true);
             titleLogin.setVisible(false);
             titleLogout.setVisible(true);
        }
       
    }

    /**
     *
     * @param params
     */
    public BasePage(PageParameters params) {
    }

    private final class LocaleChoiceRenderer extends ChoiceRenderer
    {
                private static final long serialVersionUID = 1L;
                public LocaleChoiceRenderer() {
                }

                @Override
                public Object getDisplayValue(Object object) {
                        Locale locale = (Locale)object;
                        String display = locale.getDisplayName(getLocale());
                        return display;
                }
    }
}

