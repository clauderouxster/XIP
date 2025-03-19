/*
 * Xerox Research Center Europe
 * Copyright (C) 2009 XRCE, All Rights Reserved
 */
package extui.UI_html;

import extui.UI_components.Constant;
import extui.UI_components.NameWrapper;
import extui.UI_components.Query;
import extui.UI_components.UserProfile;
import extui.UI_components.UserSession;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import org.apache.log4j.Logger;
import org.apache.wicket.AttributeModifier;
import org.apache.wicket.PageParameters;
import org.apache.wicket.ajax.AjaxRequestTarget;
import org.apache.wicket.ajax.form.OnChangeAjaxBehavior;
import org.apache.wicket.markup.html.WebPage;
import org.apache.wicket.markup.html.basic.Label;
import org.apache.wicket.markup.html.form.Button;
import org.apache.wicket.markup.html.form.CheckBox;
import org.apache.wicket.markup.html.form.CheckGroup;
import org.apache.wicket.markup.html.form.ChoiceRenderer;
import org.apache.wicket.markup.html.form.DropDownChoice;
import org.apache.wicket.markup.html.form.Form;
import org.apache.wicket.markup.html.form.Radio;
import org.apache.wicket.markup.html.form.RadioGroup;
import org.apache.wicket.markup.html.form.TextField;
import org.apache.wicket.markup.html.link.BookmarkablePageLink;
import org.apache.wicket.markup.html.link.Link;
import org.apache.wicket.markup.html.list.ListItem;
import org.apache.wicket.markup.html.list.ListView;
import org.apache.wicket.markup.html.resources.StyleSheetReference;
import org.apache.wicket.model.Model;
import org.apache.wicket.model.PropertyModel;
import org.apache.wicket.model.StringResourceModel;

/**
 *
 * @author ctrojahn
 */
public final class Index1 extends WebPage {

    private static final Logger logger = Logger.getLogger(Index1.class.getName());
    private TextField query;
    private Form form;
    private ListView<NameWrapper> listLanguagesQuery;
    private ListView listLanguagesResult;
    private ListView listAdvanced;
    private static final long serialVersionUID = 1L;

    /**
     * 
     */
    public Index1() {
        super();
        UserProfile userProfile = ((UserSession) getSession()).getUserProfile();
        String fileCSS = userProfile.getFileCSS();
        add(new StyleSheetReference("stylesheet", Index1.class, fileCSS));
        // Declaration of the components 
        // property name in application_x.properties
        Label titleLogin = new Label("titleLogin", new StringResourceModel("linkLogin",
                this, null, "XXXX"));
        // tooltip
        AttributeModifier a1 = new AttributeModifier("title", true,
                new StringResourceModel("tooltip.login", this, null, "XXXX"));
        titleLogin.add(a1);

        Label titleLogout = new Label("titleLogout", new StringResourceModel("linkLogout",
                this, null, "XXXX"));
        AttributeModifier a2 = new AttributeModifier("title", true,
                new StringResourceModel("tooltip.logout", this, null, "XXXX"));
        titleLogout.add(a2);

        Label l1 = new Label("titleLanguageSearch",
                new StringResourceModel("labelLanguageSearch", this, null, "XXXX"));
        AttributeModifier a3 = new AttributeModifier("title", true,
                new StringResourceModel("tooltip.languageSearch", this, null, "XXXX"));
        l1.add(a3);

        Label l2 = new Label("titleLanguageResults",
                new StringResourceModel("labelLanguagesResult", this, null, "XXXX"));
        AttributeModifier a4 = new AttributeModifier("title", true,
                new StringResourceModel("tooltip.languageResult", this, null, "XXXX"));
        l2.add(a4);

        Label l3 = new Label("titleAdvancedOptions",
                new StringResourceModel("labelAdvancedOptions", this, null, "XXXX"));
        AttributeModifier a5 = new AttributeModifier("title", true,
                new StringResourceModel("tooltip.advancedOptions", this, null, "XXXX"));
        l3.add(a5);

        Label descriptionCacao = new Label("descriptionCacao",
                new StringResourceModel("descriptionCacao", this, null, "XXXX"));
        Label meaningCACAO = new Label("meaningCACAO",
                new StringResourceModel("meaningCACAO", this, null, "XXXX"));
        // Label to link to the advanced search
        Label advancedSearch = new Label("labelAdvancedSearch",
                new StringResourceModel("labelAdvancedSearch", this, null, "XXXX"));
        AttributeModifier a = new AttributeModifier("title", true,
                new StringResourceModel("msgNotImplemented", this, null, "XXXX"));
        advancedSearch.add(a);
        // Just the title of the table containing the advanced search form
        Label titleAdvancedSearch = new Label("titleAdvancedSearch",
                new StringResourceModel("labelAdvancedSearch", this, null, "XXXX"));
        Label virtualKeyboard = new Label("labelVirtualKeyboard",
                new StringResourceModel("labelVirtualKeyboard", this, null, "XXXX"));
        AttributeModifier a6 = new AttributeModifier("title", true,
                new StringResourceModel("tooltip.virtualKeyboard", this, null, "XXXX"));
        virtualKeyboard.add(a6);
        Label selectLanguages = new Label("labelSelectLanguages",
                new StringResourceModel("labelSelectLanguages", this, null, "XXXX"));
        AttributeModifier a7 = new AttributeModifier("title", true,
                new StringResourceModel("tooltip.listLangsInterface", this, null, "XXXX"));
        selectLanguages.add(a7);
        // Used when the user did not put some string into the query field
        final Label labelError = new Label("msgError",
                new StringResourceModel("msgErrorInputQuery", this, null, "XXXX"));
        Button btnSearch = new Button("btnSearch",
                new StringResourceModel("labelButtonSearch", this, null, "XXXX"));
        AttributeModifier a8 = new AttributeModifier("title", true,
                new StringResourceModel("tooltip.buttonSearch", this, null, "XXXX"));
        btnSearch.add(a8);
        final RadioGroup groupQuery = new RadioGroup("groupLanguagesQuery", new Model());
        final CheckGroup groupResults = new CheckGroup("groupLanguagesResult", new ArrayList());
        final CheckGroup groupAdvanced = new CheckGroup("groupAdvanced", new ArrayList());
        BookmarkablePageLink settings = new BookmarkablePageLink("settings", Settings.class);
        BookmarkablePageLink libraries = new BookmarkablePageLink("linkLibraries", Libraries.class);
        BookmarkablePageLink partners = new BookmarkablePageLink("linkPartners", Partners.class);
        BookmarkablePageLink help = new BookmarkablePageLink("linkHelp", HelpPage.class);
        Label labelSettings = new Label("titleLinkSettings",
                new StringResourceModel("linkSettings", this, null, "XXXX"));
        AttributeModifier a9 = new AttributeModifier("title", true,
                new StringResourceModel("tooltip.settings", this, null, "XXXX"));
        labelSettings.add(a9);
        Label labelLibraries = new Label("titleLinkLibraries",
                new StringResourceModel("linkLibraries", this, null, "XXXX"));
        AttributeModifier a10 = new AttributeModifier("title", true,
                new StringResourceModel("tooltip.libraries", this, null, "XXXX"));
        labelLibraries.add(a10);
        Label labelPartners = new Label("titleLinkPartners",
                new StringResourceModel("linkPartners", this, null, "XXXX"));
        AttributeModifier a11 = new AttributeModifier("title", true,
                new StringResourceModel("tooltip.partners", this, null, "XXXX"));
        labelPartners.add(a11);
        Label labelHelp = new Label("titleLinkHelp",
                new StringResourceModel("linkHelp", this, null, "XXXX"));
        AttributeModifier a12 = new AttributeModifier("title", true,
                new StringResourceModel("tooltip.help", this, null, "XXXX"));
        labelHelp.add(a12);
        Label msgWait = new Label("msgWait",
                new StringResourceModel("msgWait", this, null, "XXXX"));
        Link login = null;
        Link logout = null;
        List<Locale> listLocales = new ArrayList();
        final DropDownChoice<Locale> languagesCombo =
                new DropDownChoice("languages",
                new Model<Locale>(), listLocales, new LocaleChoiceRenderer()) {

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

        // Initialization of components proprieties 
        labelError.setVisible(false);
        // TO DO: put these options in a file
        listLocales.add(Locale.ENGLISH);
        listLocales.add(Locale.FRENCH);
        listLocales.add(Locale.GERMAN);
        listLocales.add(Locale.ITALIAN);
        listLocales.add(new Locale("nl", "NL"));
        listLocales.add(new Locale("pt", "BR"));
        // If the user types any query, put nothing in the msgError (see onSubmit in the form)
        labelError.setOutputMarkupId(true);
        query = new TextField("query", new Model(""));
        // used for the virtual keyboard
        query.setMarkupId("textfield");
        query.add(new OnChangeAjaxBehavior() {

            private static final long serialVersionUID = 1L;

            @Override
            protected void onUpdate(AjaxRequestTarget target) {
                labelError.setVisible(false);
                target.addComponent(labelError);
            }
        });

        // The values of the lists depend on the user profile 
        List<NameWrapper> listSearchLangProfile = new ArrayList<NameWrapper>();
        for (int x = 0; x < Constant.listLanguages.size(); x++) {
            //"English", "en", "", false
            NameWrapper copy = Constant.listLanguages.get(x);
            listSearchLangProfile.add(new NameWrapper(copy.getName(),
                    copy.getShortName(), copy.getHit(), copy.getSelected()));
        }
        String profile = ((UserSession) getSession()).getUserProfile().
                getDefaultLanguageSearch();
        NameWrapper test = listSearchLangProfile.get(0);
        int index = 0;
        for (int x = 0; x < listSearchLangProfile.size(); x++) {
            String lang = listSearchLangProfile.get(x).getShortName();
            if (lang.equals(profile)) {
                listSearchLangProfile.get(x).setSelected(true);
                test = listSearchLangProfile.get(x);
                logger.debug(lang + "------> true");
                index = x;
            } else {
                listSearchLangProfile.get(x).setSelected(false);
                logger.debug(lang + "------> false");
            }
            listSearchLangProfile.set(x, listSearchLangProfile.get(x));
        }
        groupQuery.setDefaultModelObject(test);
        listLanguagesQuery = new ListView("listLanguagesQuery", listSearchLangProfile) {

            private static final long serialVersionUID = 1L;

            protected void populateItem(ListItem item) {
                String lang = ((NameWrapper) item.getModelObject()).getName();
                item.add(new Label("nameLanguageQuery",
                        new StringResourceModel(lang, this, null, "XXXX")));
                item.add(new Radio("checkLanguageQuery", item.getModel()));
            }
        };
        groupQuery.add(listLanguagesQuery);
        logger.debug("ok1");
        // The values of the lists depend on the user profile 
        List<NameWrapper> listResultsLangProfile = Constant.listLanguages;
        String profile1[] = ((UserSession) getSession()).getUserProfile().
                getDefaultLanguageResults().split(";");
        for (int x = 0; x < listResultsLangProfile.size(); x++) {
            String lang = listResultsLangProfile.get(x).getShortName();
            boolean has = false;
            for (int y = 0; y < profile1.length; y++) {
                if (lang.equals(profile1[y])) {
                    has = true;
                }
            }
            if (has) {
                listResultsLangProfile.get(x).setSelected(true);

            } else {
                listResultsLangProfile.get(x).setSelected(false);
            }
            listResultsLangProfile.set(x, listResultsLangProfile.get(x));
        }
        listLanguagesResult = new ListView("listLanguagesResult", listResultsLangProfile) {

            private static final long serialVersionUID = 1L;

            protected void populateItem(ListItem item) {
                String lang = ((NameWrapper) item.getModelObject()).getName();
                item.add(new Label("nameLanguageResult",
                        new StringResourceModel(lang, this, null, "XXXX")));
                item.add(new CheckBox("checkLanguageResult",
                        new PropertyModel(item.getModel(), "selected")));
            }
        };
        groupResults.add(listLanguagesResult);
        //groupResults.add(new CheckGroupSelector("groupSelectorResults"));
        logger.debug("ok2");
        // The values of the lists depend on the user profile 
        List<NameWrapper> listOptionsProfile = Constant.advancedOptions;
        String profile2 = ((UserSession) getSession()).getUserProfile().
                getDefaultAdvancedOptions();
        for (int x = 0; x < listOptionsProfile.size(); x++) {
            String option = listOptionsProfile.get(x).getShortName();
            if (profile2.contains(option + "=true")) {
                listOptionsProfile.get(x).setSelected(true);
            } else {
                listOptionsProfile.get(x).setSelected(false);
            }
            listOptionsProfile.set(x, listOptionsProfile.get(x));
        }
        listAdvanced = new ListView("listAdvanced", listOptionsProfile) {

            private static final long serialVersionUID = 1L;

            protected void populateItem(ListItem item) {
                String option = ((NameWrapper) item.getModelObject()).getShortName();
                AttributeModifier a = new AttributeModifier("title", true,
                        new StringResourceModel("hit." + option, this, null, "XXXX"));
                Label l = new Label("nameAdvanced",
                        new StringResourceModel(option, this, null, "XXXX"));
                l.add(a);
                item.add(new CheckBox("checkAdvanced",
                        new PropertyModel(item.getModel(), "selected")));
                item.add(l);
            }
        };
        groupAdvanced.add(listAdvanced);
        //groupAdvanced.add(new CheckGroupSelector("groupSelectorAdv"));
        logger.debug("ok3");
        login = new Link("login", new StringResourceModel("linkLogin", this, null, "XXXX")) {

            private static final long serialVersionUID = 1L;

            @Override
            public void onClick() {
                // Go to page to login, which will ask for username and password
                setResponsePage(Login.class);
            }
        };
        logger.debug("ok4");
        logout = new Link("logout", new StringResourceModel("linkLogout", this, null, "XXXX")) {

            private static final long serialVersionUID = 1L;

            @Override
            public void onClick() {
                /*
                 * When the user click in logout, the user = "default".
                 *  The value "defaul" is used to configure the page 
                 * (for instance, if login or logout is show or not - see CONFIGURATION PAGE)
                 */
                String locale = ((UserSession) getSession()).getLocale().getDisplayLanguage();
                ((UserSession) getSession()).setUserProfile(UserProfile.createUserDefault(locale));
                setResponsePage(Index1.class);
            }
        };
        logger.debug("ok5");
        /* 
         * All graphical elements are added to the interface, but according
         * to the user profile, such elements
         * can not be visible:
         */
        /* 
         * 1) Language used to query
         */
        if (!((UserSession) getSession()).getUserProfile().isVisibleLanguageSearch()) {
            listLanguagesQuery.setVisible(false);
            l1.setVisible(false);
            groupQuery.setVisible(false);
        }
        logger.debug("ok6");
        /* 
         * 2) Languages of the results
         */
        if (!((UserSession) getSession()).getUserProfile().isVisibleLanguageResults()) {
            listLanguagesResult.setVisible(false);
            l2.setVisible(false);
            groupResults.setVisible(false);
        }
        logger.debug("ok7");
        /* 
         * 3) Advanced options
         */
        if (!((UserSession) getSession()).getUserProfile().isVisibleAdvancedOptions()) {
            listAdvanced.setVisible(false);
            l3.setVisible(false);
            groupAdvanced.setVisible(false);
        }
        logger.debug("ok8");
        /*
         * CONFIGURATION PAGE:
         * As for the options, the settings option must appear only if the
         * user is different of "default" (see class
         * Application). Also, if the user is logged, the login must be replaced by logout
         */
        if (((UserSession) getSession()).getUserProfile().getLogin().equals("default")) {
            settings.setVisible(false);
            //titleLogin.setDefaultModel(new StringResourceModel("linkLogin", this, null,"XXXX"));
            titleLogin.setVisible(true);
            /* logout is not showed (not use of visible propriety because using visibility the
             * the space in the interface is used.
             */
            //titleLogout.setDefaultModelObject(
            //new StringResourceModel("linkLogout.empty", this, null,""));
            titleLogout.setVisible(false);
        } else {
            settings.setVisible(true);
            //titleLogin.setDefaultModelObject(
            //new StringResourceModel("linkLogin.empty", this, null,""));
            //titleLogout.setDefaultModelObject(
            //new StringResourceModel("linkLogout", this, null,"XXXX"));
            titleLogin.setVisible(false);
            titleLogout.setVisible(true);
        }
        logger.debug("ok9");
        /*
         * Click in the submit generate an object Query, that represents all
         * parameters informed by the user. When clickin on the submit button,
         * the object Query is then sent to
         * the ResultPage, where the query will be processed and sent to WS
         */
        form = new Form("form") {

            private static final long serialVersionUID = 1L;

            @Override
            public void onSubmit() {
                String inputQuery = query.getDefaultModelObjectAsString();
                // if the query is "vide" send a msg to the user
                if (inputQuery.equals("")) {
                    labelError.setVisible(true);
                } else {
                    /*
                     * Retrieve all parameters (values of the graphical objects), but
                     * just if they are visible. Otherwise, the parameters specified in the
                     * user profile are used
                     */
                    UserProfile profile = ((UserSession) getSession()).getUserProfile();
                    String srcLang = null;
                    String[] targetLang = null;
                    boolean systran = false;
                    boolean SVexp = false;
                    boolean WNexp = false;
                    boolean W2Cexp = false;
                    /*
                     * 1) Language used to query: the model of listLanguagesQuery is a NameWrapper
                     * containing the values of the selections (true or false - getSelected)
                     */
                    /*
                    if (profile.isVisibleLanguageSearch()) {
                    List<NameWrapper> listSrcLang = listLanguagesQuery.getModelObject();
                    for(int x=0;x < listSrcLang.size();x++) {
                    if (listSrcLang.get(x).getSelected()) {
                    srcLang = listSrcLang.get(x).getShortName();
                    System.out.println("====================== " +srcLang + " TRUE");
                    }
                    else {
                    System.out.println("====================== " +srcLang + " FALSE");
                    }
                    }
                    }
                    else {
                    srcLang = profile.getDefaultLanguageSearch();
                    }
                     */
                    if (profile.isVisibleLanguageSearch()) {
                        srcLang = groupQuery.getDefaultModelObjectAsString();
                    } else {
                        srcLang = profile.getDefaultLanguageSearch();
                    }
                    logger.debug("Lang search: " + srcLang);
                    //System.out.println("LANG: " + srcLang);
                    /* 2) Languages of the results: the model of listLanguagesResult
                     * is a NameWrapper
                     */
                    if (profile.isVisibleLanguageResults()) {
                        String langs = "";
                        List<NameWrapper> listSrcLang = listLanguagesResult.getModelObject();
                        for (int x = 0; x < listSrcLang.size(); x++) {
                            if (listSrcLang.get(x).getSelected()) {
                                langs += listSrcLang.get(x).getShortName() + ";";
                            }
                        }
                        targetLang = langs.split(";");
                    } else {
                        /* As set of languages are put in the string profile: "en;fr;it" */
                        targetLang = profile.getDefaultLanguageResults().split(";");
                    }
                    for (int x = 0; x < targetLang.length; x++) {
                        logger.debug(targetLang[x]);
                        //System.out.println(targetLang[x]);
                    }
                    /*
                     * 3) Advanced options
                     */
                    if (profile.isVisibleAdvancedOptions()) {
                        List<NameWrapper> options = listAdvanced.getModelObject();
                        for (int x = 0; x < options.size(); x++) {
                            if (options.get(x).getSelected()) {
                                if (options.get(x).getShortName().equals("systran")) {
                                    systran = true;
                                }
                                if (options.get(x).getShortName().equals("SVexp")) {
                                    SVexp = true;
                                }
                                if (options.get(x).getShortName().equals("WNexp")) {
                                    WNexp = true;
                                }
                                if (options.get(x).getShortName().equals("W2Cexp")) {
                                    W2Cexp = true;
                                }
                            }
                        }
                    } else {
                        // In settings: "systran=true;SVexp=true;WNexp=true;W2Cexp=true"
                        systran = profile.getDefaultAdvancedOptions().contains(
                                new StringBuffer("systran=true"));
                        SVexp = profile.getDefaultAdvancedOptions().contains(
                                new StringBuffer("SVexp=true"));
                        WNexp = profile.getDefaultAdvancedOptions().contains(
                                new StringBuffer("WNexp=true"));
                        W2Cexp = profile.getDefaultAdvancedOptions().contains(
                                new StringBuffer("W2Cexp=true"));
                    }
                    logger.debug("SVexp = " + SVexp + " WNexp = " + WNexp + " W2Cexp = " + W2Cexp);
                    /* After getting all parameters, create a query */
                    Query q = new Query(inputQuery, srcLang, targetLang,
                            systran, SVexp, WNexp, W2Cexp);
                    ResultsPage rp = null;
                    try {
                        rp = new ResultsPage(q);
                        this.setResponsePage(rp);
                    } catch (Exception ex) {
                        logger.error(ex);
                    }
                }
            }
        };
        settings.add(labelSettings);
        libraries.add(labelLibraries);
        partners.add(labelPartners);
        help.add(labelHelp);
        form.add(selectLanguages);
        form.add(libraries);
        form.add(partners);
        form.add(help);
        form.add(msgWait);
        form.add(advancedSearch);
        form.add(titleAdvancedSearch);
        form.add(virtualKeyboard);
        form.add(languagesCombo);
        form.add(query);
        form.add(btnSearch);
        form.add(groupQuery);
        form.add(groupResults);
        form.add(groupAdvanced);
        form.add(labelError);
        form.add(l1);
        form.add(l2);
        form.add(l3);
        login.add(titleLogin);
        logout.add(titleLogout);
        form.add(login);
        form.add(logout);
        form.add(settings);
        form.add(descriptionCacao);
        form.add(meaningCACAO);
        add(form);
        addSectionAdvancedSearch();
    }

    /**
     * 
     * @param params
     */
    public Index1(PageParameters params) {
        //TODO:  process page parameters
    }

    private void addSectionAdvancedSearch() {
        /* Advanced Search */
        Button clear = new Button("clearAdvancedSearch",
                new StringResourceModel("labelButtonClear", this, null, "XXXX"));
        Button submit = new Button("submitAdvancedSearch",
                new StringResourceModel("labelButtonSearch", this, null, "XXXX"));
        logger.debug("ok10");
        AttributeModifier a = new AttributeModifier("title", true,
                new StringResourceModel("msgNotImplemented", this, null, "XXXX"));
        submit.add(a);
        logger.debug("ok11");
        final List<String> fields = new ArrayList<String>();
        fields.add("Any field");
        fields.add("Title");
        fields.add("Creator");
        fields.add("Subject");
        fields.add("Language");
        fields.add("ISBN");
        fields.add("ISSN");
        logger.debug("ok12");
        @SuppressWarnings("unchecked")
        final DropDownChoice fields1 = new DropDownChoice("fields1",
                new Model<String>(), fields, new FieldsChoiceRenderer());
        fields1.setDefaultModelObject("Any field");
        @SuppressWarnings("unchecked")
        final DropDownChoice fields2 = new DropDownChoice("fields2",
                new Model<String>(), fields, new FieldsChoiceRenderer());
        fields2.setDefaultModelObject("Any field");
        @SuppressWarnings("unchecked")
        final DropDownChoice fields3 = new DropDownChoice("fields3",
                new Model<String>(), fields, new FieldsChoiceRenderer());
        fields3.setDefaultModelObject("Any field");
        @SuppressWarnings("unchecked")
        final DropDownChoice fields4 = new DropDownChoice("fields4",
                new Model<String>(), fields, new FieldsChoiceRenderer());
        fields4.setDefaultModelObject("Any field");
        form.add(fields1);
        form.add(fields2);
        form.add(fields3);
        form.add(fields4);
        final List<String> op = new ArrayList<String>();
        op.add("AND");
        op.add("OR");
        @SuppressWarnings("unchecked")
        final DropDownChoice op1 = new DropDownChoice("op1",
                new Model<String>(), op, new FieldsChoiceRenderer());
        op1.setDefaultModelObject("AND");
        @SuppressWarnings("unchecked")
        final DropDownChoice op2 = new DropDownChoice("op2",
                new Model<String>(), op, new FieldsChoiceRenderer());
        op2.setDefaultModelObject("AND");
        @SuppressWarnings("unchecked")
        final DropDownChoice op3 = new DropDownChoice("op3",
                new Model<String>(), op, new FieldsChoiceRenderer());
        op3.setDefaultModelObject("AND");
        form.add(op1);
        form.add(op2);
        form.add(op3);
        form.add(clear);
        form.add(submit);
    }

    private final class LocaleChoiceRenderer extends ChoiceRenderer {

        private static final long serialVersionUID = 1L;

        public LocaleChoiceRenderer() {
        }

        @Override
        public Object getDisplayValue(Object object) {
            Locale locale = (Locale) object;
            String display = locale.getDisplayName(getLocale());
            return display;
        }
    }

    private final class FieldsChoiceRenderer extends ChoiceRenderer {

        private static final long serialVersionUID = 1L;

        public FieldsChoiceRenderer() {
        }

        @Override
        public Object getDisplayValue(Object object) {
            String current = (String) object;
            return "XXX";
        }
    }
}
