/*
 * Xerox Research Center Europe
 * Copyright (C) 2009 XRCE, All Rights Reserved
 */
package extui.UI_html;

import extui.UI_components.Constant;
import extui.UI_components.ManagerDB_Users;
import extui.UI_components.NameWrapper;
import extui.UI_components.UserProfile;
import extui.UI_components.UserSession;
import java.util.ArrayList;
import java.util.List;
import org.apache.log4j.Logger;
import org.apache.wicket.PageParameters;
import org.apache.wicket.ajax.AjaxRequestTarget;
import org.apache.wicket.ajax.form.AjaxFormComponentUpdatingBehavior;
import org.apache.wicket.ajax.markup.html.form.AjaxCheckBox;
import org.apache.wicket.markup.html.basic.Label;
import org.apache.wicket.markup.html.form.Button;
import org.apache.wicket.markup.html.form.CheckBox;
import org.apache.wicket.markup.html.form.CheckGroup;
import org.apache.wicket.markup.html.form.DropDownChoice;
import org.apache.wicket.markup.html.form.Form;
import org.apache.wicket.markup.html.list.ListItem;
import org.apache.wicket.markup.html.list.ListView;
import org.apache.wicket.model.Model;
import org.apache.wicket.model.PropertyModel;
import org.apache.wicket.model.StringResourceModel;

/**
 *
 * @author ctrojahn
 */
public final class Settings extends BasePage {

      UserProfile currentProfile;
      private static final Logger logger = Logger.getLogger(Settings.class.getName());
      private static final long serialVersionUID = 1L;

    @SuppressWarnings("unchecked")
      public Settings() {

        super();

        final Label feedback = new Label("feedback", new StringResourceModel("labelSettingUpdated", this, null,"XXXX"));
        feedback.setOutputMarkupId(true);
        feedback.setVisible(false);

        currentProfile = ((UserSession)getSession()).getUserProfile();

        Label labelInterfaceSettings = new Label("labelInterfaceSettings", new StringResourceModel("labelInterfaceSettings", this, null,"XXXX"));
        Label labelInterfaceFeatures = new Label("labelInterfaceFeatures", new StringResourceModel("labelInterfaceFeatures", this, null,"XXXX"));
        Label labelLanguageInterface = new Label("labelLanguageInterface", new StringResourceModel("labelLanguageInterface", this, null,"XXXX"));
        Label labelStyleCSS = new Label("labelStyleCSS", new StringResourceModel("labelStyleCSS", this, null,"XXXX"));

        Label labelVisibilityOptions = new Label("labelVisibilityOptions", new StringResourceModel("labelVisibilityOptions", this, null,"XXXX"));
        Label labelLanguageSearchVisible = new Label("labelLanguageSearchVisible", new StringResourceModel("labelLanguageSearch", this, null,"XXXX"));
        Label labelLanguagesResultsVisible = new Label("labelLanguagesResultsVisible", new StringResourceModel("labelLanguagesResult", this, null,"XXXX"));
        Label labelAdvancedOptionVisible = new Label("labelAdvancedOptionsVisible", new StringResourceModel("labelAdvancedOptions", this, null,"XXXX"));

        Label labelDefaultOptions = new Label("labelDefaultOptions", new StringResourceModel("labelDefaultOptions", this, null,"XXXX"));
        Label LabelDefaultLanguageSearch = new Label("labelDefaultLanguageSearch", new StringResourceModel("labelLanguageSearch", this, null,"XXXX"));
        Label labelDefaultLanguagesResults = new Label("labelDefaultLanguagesResults", new StringResourceModel("labelLanguagesResult", this, null,"XXXX"));
        Label labelDefaultAdvancedOptions = new Label("labelDefaultAdvancedOptions", new StringResourceModel("labelAdvancedOptions", this, null,"XXXX"));

        Label SVexp = new Label("SVexp", new StringResourceModel("SVexp", this, null,"XXXX"));
        Label WNexp = new Label("WNexp", new StringResourceModel("WNexp", this, null,"XXXX"));
        Label W2Cexp = new Label("W2Cexp", new StringResourceModel("W2Cexp", this, null,"XXXX"));

        /* According to the current user profile, initialize the values of each field */

        @SuppressWarnings("unchecked")
        final DropDownChoice languagesInterface = new DropDownChoice("languageInterface", new Model(),Constant.listLanguages);
        languagesInterface.setOutputMarkupId(true);
        languagesInterface.setDefaultModelObject(currentProfile.getDefaultLanguageSearch());
        languagesInterface.add(new AjaxFormComponentUpdatingBehavior("onchange") {
            private static final long serialVersionUID = 1L;
            @Override
            protected void onUpdate(AjaxRequestTarget target) {
                feedback.setVisible(false);
                target.addComponent(feedback);
            }
        });

        @SuppressWarnings("unchecked")
        final DropDownChoice filesCSS = new DropDownChoice("fileCSS",new Model(), Constant.filesCSS);
        filesCSS.setOutputMarkupId(true);
        filesCSS.setDefaultModelObject(currentProfile.getFileCSS());
        filesCSS.add(new AjaxFormComponentUpdatingBehavior("onchange") {
            private static final long serialVersionUID = 1L;
            @Override
            protected void onUpdate(AjaxRequestTarget target) {
                feedback.setVisible(false);
                target.addComponent(feedback);
            }
        });

        @SuppressWarnings("unchecked")
        final AjaxCheckBox c1 = new AjaxCheckBox("visibilityLanguageSearch", new Model()) {
              private static final long serialVersionUID = 1L;
              protected void onUpdate(final AjaxRequestTarget target) {
                   feedback.setVisible(false);
                   target.addComponent(feedback);
              }
        };
        c1.setDefaultModelObject(currentProfile.isVisibleLanguageSearch());

        @SuppressWarnings("unchecked")
        final CheckBox c2 = new CheckBox("visibilityLanguagesResult", new Model()){
              private static final long serialVersionUID = 1L;
              protected void onUpdate(final AjaxRequestTarget target) {
                   feedback.setVisible(false);
                   target.addComponent(feedback);
              }
        };
        c2.setDefaultModelObject(currentProfile.isVisibleLanguageResults());

        final CheckBox c3 = new CheckBox("visibilityAdvancedOptions", new Model()) {
              private static final long serialVersionUID = 1L;
              protected void onUpdate(final AjaxRequestTarget target) {
                   feedback.setVisible(false);
                   target.addComponent(feedback);
              } 
        };
        c3.setDefaultModelObject(currentProfile.isVisibleAdvancedOptions());

        @SuppressWarnings("unchecked")
        final DropDownChoice languageSearch = new DropDownChoice("languageSearch",new Model(), Constant.listLanguages);
        languageSearch.setOutputMarkupId(true);
        languageSearch.setDefaultModelObject(currentProfile.getDefaultLanguageSearch());
        languageSearch.add(new AjaxFormComponentUpdatingBehavior("onchange") {
            private static final long serialVersionUID = 1L;
            @Override
            protected void onUpdate(AjaxRequestTarget target) {
                feedback.setVisible(false);
                target.addComponent(feedback);
            }
        });

        @SuppressWarnings("unchecked")
        final AjaxCheckBox SV = new AjaxCheckBox("SV", new Model()) {
              private static final long serialVersionUID = 1L;
              protected void onUpdate(final AjaxRequestTarget target) {
                   feedback.setVisible(false);
                   target.addComponent(feedback);
              }
        };
        SV.setDefaultModelObject(currentProfile.getDefaultAdvancedOptions().contains(new StringBuffer("SVexp=true")));

        @SuppressWarnings("unchecked")
        final AjaxCheckBox WN = new AjaxCheckBox("WN", new Model()){
              private static final long serialVersionUID = 1L;
              protected void onUpdate(final AjaxRequestTarget target) {
                   feedback.setVisible(false);
                   target.addComponent(feedback);
              }
        };
        WN.setDefaultModelObject(currentProfile.getDefaultAdvancedOptions().contains(new StringBuffer("WNexp=true")));

        @SuppressWarnings("unchecked")
        final AjaxCheckBox W2C = new AjaxCheckBox("W2C", new Model()){
              private static final long serialVersionUID = 1L;
              protected void onUpdate(final AjaxRequestTarget target) {
                    feedback.setVisible(false);
                   target.addComponent(feedback);
              }
        };
        W2C.setDefaultModelObject(currentProfile.getDefaultAdvancedOptions().contains(new StringBuffer("W2Cexp=true")));


        @SuppressWarnings("unchecked")
        final CheckGroup groupResults = new CheckGroup("groupLanguagesResult", new ArrayList());
        @SuppressWarnings("unchecked")
        final ListView listLanguagesResult = new ListView("languagesResult", Constant.listLanguages) {
            private static final long serialVersionUID = 1L;
            protected void populateItem(ListItem item) {
                item.add(new AjaxCheckBox("checkLanguageResult", new PropertyModel(item.getModel(), "selected")) {
                    @Override
                    protected void onUpdate(AjaxRequestTarget target) {
                          feedback.setVisible(false);
                          target.addComponent(feedback);
                    }
                });
                item.add(new Label("nameLanguageResult", new PropertyModel(item.getModel(), "name")));
            }
        };
        //groupResults.add(new CheckGroupSelector("groupSelectorResults"));
        groupResults.add(listLanguagesResult);

        Button saveSettings = new Button("saveSettings", new StringResourceModel("labelButtonSaveSettings", this, null,"XXXX"));

        Form form = new Form("formSettings") {
            private static final long serialVersionUID = 1L;
            @Override
            public void onSubmit() {

                // Get the values of each parameter -----------------------------------------------------------------
        
                currentProfile.setFileCSS(filesCSS.getDefaultModelObjectAsString());
                logger.info(filesCSS.getDefaultModelObjectAsString());

                currentProfile.setLanguageInterface(languagesInterface.getDefaultModelObjectAsString());
                logger.info("Language interface: " + languagesInterface.getDefaultModelObjectAsString());
                currentProfile.setDefaultLanguageSearch(languageSearch.getDefaultModelObjectAsString());
                logger.info("Language search: " + languageSearch.getDefaultModelObjectAsString());

                List l = listLanguagesResult.getList();
                String selected = "";
                for (int x = 0; x < l.size(); x++) {
                    if (((NameWrapper) l.get(x)).getSelected()) {
                        logger.info("-------------------- " + l.get(x));
                        selected += l.get(x) + ";";
                    }
                }
                int i = selected.lastIndexOf(";");
                selected = selected.substring(0,i);

                logger.info("Languages result: " + selected);
                currentProfile.setDefaultLanguageResults(selected);

                //Format for advanced options: "systran=false;SVexp=false;WNexp=false;W2Cexp=false"
                String advancedOptions = "systran=false"; //This option is *always* not shown to the user! check it!!
                if (SV.getValue().equals("true")) {
                    advancedOptions += ";SVexp=true";
                } else {
                    advancedOptions += ";SVexp=false";
                }

                if (WN.getValue().equals("true")) {
                    advancedOptions += ";WNexp=true";
                } else {
                    advancedOptions += ";WNexp=false";
                }

                if (W2C.getValue().equals("true")) {
                    advancedOptions += ";W2Cexp=true";
                } else {
                    advancedOptions += ";W2Cexp=false";
                }
                currentProfile.setDefaultAdvancedOptions(advancedOptions);
                logger.info("Advanced options: " + advancedOptions);


                // Visibility of the options; langauge search, languages results, and advanced options
                if (c1.getValue().equals("true")) {
                    currentProfile.setVisibleLanguageSearch(true);
                    logger.info("Visibility language search: true");
                } else {
                    logger.info("Visibility language search: false");
                    currentProfile.setVisibleLanguageSearch(false);
                }

                if (c2.getValue().equals("true")) {
                    logger.info("Visibility language result: true");
                    currentProfile.setVisibleLanguageResults(true);
                } else {
                    logger.info("Visibility language result: false");
                    currentProfile.setVisibleLanguageResults(false);
                }

                if (c3.getValue().equals("true")) {
                    logger.info("Visibility advanced options: true");
                    currentProfile.setVisibleAdvancedOptions(true);
                } else {
                    currentProfile.setVisibleAdvancedOptions(false);
                    logger.info("Visibility advanced options: false");
                }

                feedback.setVisible(true);
               
               // Update the user settings
               ManagerDB_Users manager = Application.getManagerDB_Users();
               UserProfile newUser1 = manager.setNewSettings(currentProfile);
               ((UserSession)getSession()).setUserProfile(newUser1);
               
            }
        };

        form.add(labelInterfaceSettings);
        form.add(labelInterfaceFeatures);
        form.add(labelLanguageInterface);
        form.add(labelStyleCSS);

        form.add(labelVisibilityOptions);
        form.add(labelLanguageSearchVisible);
        form.add(labelLanguagesResultsVisible);
        form.add(labelAdvancedOptionVisible);

        form.add(labelDefaultOptions);
        form.add(LabelDefaultLanguageSearch);
        form.add(labelDefaultLanguagesResults);
        form.add(labelDefaultAdvancedOptions);

        form.add(SVexp);
        form.add(WNexp);
        form.add(W2Cexp);

        form.add(languagesInterface);
        form.add(filesCSS);
        form.add(c1);
        form.add(c2);
        form.add(c3);
        form.add(SV);
        form.add(WN);
        form.add(W2C);
        form.add(languageSearch);
        form.add(groupResults);
        form.add(saveSettings);
        form.add(feedback);
        add(form);
    }

    public Settings(PageParameters params) {
    }
}

