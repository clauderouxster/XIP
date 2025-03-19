/*
 * Xerox Research Center Europe
 * Copyright (C) 2009 XRCE, All Rights Reserved
 */
package extui.UI_html;

import extui.UI_components.Constant;
import extui.UI_components.Document;
import extui.UI_components.DocumentDataProvider;
import extui.UI_components.Facet;
import extui.UI_components.FacetAvailableAddProvider;
import extui.UI_components.FacetDataProvider;
import extui.UI_components.FacetValue;
import extui.UI_components.Query;
import extui.UI_components.Refresh;
import extui.UI_components.SearchQueryManipulate;
import extui.UI_components.Tag;
import extui.UI_components.TagDataProvider;
import extui.UI_components.TranslationsDataProvider;
import extui.UI_components.UserSession;
import extui.UI_ws.QueryWS;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashSet;
import java.util.List;
import org.apache.log4j.Logger;
import org.apache.wicket.AttributeModifier;
import org.apache.wicket.Page;
import org.apache.wicket.ajax.AjaxRequestTarget;
import org.apache.wicket.ajax.IAjaxIndicatorAware;
import org.apache.wicket.ajax.form.AjaxFormComponentUpdatingBehavior;
import org.apache.wicket.ajax.markup.html.AjaxLink;
import org.apache.wicket.ajax.markup.html.form.AjaxButton;
import org.apache.wicket.ajax.markup.html.form.AjaxCheckBox;
import org.apache.wicket.ajax.markup.html.navigation.paging.AjaxPagingNavigator;
import org.apache.wicket.extensions.ajax.markup.html.modal.ModalWindow;
import org.apache.wicket.markup.html.WebMarkupContainer;
import org.apache.wicket.markup.html.WebPage;
import org.apache.wicket.markup.html.basic.Label;
import org.apache.wicket.markup.html.form.Button;
import org.apache.wicket.markup.html.form.CheckBox;
import org.apache.wicket.markup.html.form.CheckGroup;
import org.apache.wicket.markup.html.form.DropDownChoice;
import org.apache.wicket.markup.html.form.Form;
import org.apache.wicket.markup.html.form.TextField;
import org.apache.wicket.markup.html.link.BookmarkablePageLink;
import org.apache.wicket.markup.html.list.ListItem;
import org.apache.wicket.markup.html.list.ListView;
import org.apache.wicket.markup.html.resources.StyleSheetReference;
import org.apache.wicket.markup.repeater.Item;
import org.apache.wicket.markup.repeater.data.DataView;
import org.apache.wicket.model.AbstractReadOnlyModel;
import org.apache.wicket.model.Model;
import org.apache.wicket.model.PropertyModel;
import org.apache.wicket.model.StringResourceModel;

/**
 * @author ctrojahn
 */
public class ResultsPage extends WebPage implements IAjaxIndicatorAware {

    private static final long serialVersionUID = 1L;
    private static final Logger logger = Logger.getLogger(ResultsPage.class.getName());

    Refresh refresh;
    
    DataView dataView;

    private WebMarkupContainer containerListDocs;
    private WebMarkupContainer containerTagCloud;
    private WebMarkupContainer containerFacets;
    private WebMarkupContainer containerTranslation;
    private WebMarkupContainer containerDescriptionResults;
    private Query query;

    private String titleDuplicated;
    private boolean duplicated;


    /**
     *
     */
    public ResultsPage() {
    }

    /**
     *
     * @param query
     * @throws java.lang.Exception
     */
    @SuppressWarnings({"unchecked", "unchecked"})
    public ResultsPage(Query query) throws Exception {
        super();
        this.query = query;

        containerListDocs = new WebMarkupContainer("containerListDocs");
        containerTagCloud = new WebMarkupContainer("containerTagCloud");
        containerFacets = new WebMarkupContainer("containerFacets");
        containerTranslation = new WebMarkupContainer("containerTranslation");
        containerDescriptionResults = new WebMarkupContainer("containerDescriptionResults");

        refresh = new Refresh();

        containerListDocs.setOutputMarkupId(true);
        containerTagCloud.setOutputMarkupId(true);
        containerFacets.setOutputMarkupId(true);
        containerTranslation.setOutputMarkupId(true);
        containerDescriptionResults.setOutputMarkupId(true);

        String fileCSS = ((UserSession) getSession()).getUserProfile().getFileCSS();
        add(new StyleSheetReference("stylesheet", Index1.class, fileCSS));

        BookmarkablePageLink bookmark = new BookmarkablePageLink("bookmark", Bookmark.class);
        BookmarkablePageLink settings = new BookmarkablePageLink("settings", Settings.class);
        BookmarkablePageLink help  = new BookmarkablePageLink("help", HelpPage.class);
        BookmarkablePageLink home = new BookmarkablePageLink("home", Index1.class);

        Label labelLinkHome = new Label("labelLinkHome", new StringResourceModel("linkHome", this, null,"XXXX"));
        Label labelLinkHelp = new Label("labelLinkHelp", new StringResourceModel("linkHelp", this, null,"XXXX"));
        Label labelLinkSettings = new Label("labelLinkSettings", new StringResourceModel("linkSettings", this, null,"XXXX"));
        Label labelLinkBookmark = new Label("labelLinkBookmark", new StringResourceModel("linkBookmark", this, null,"XXXX"));
        
        home.add(labelLinkHome);
        help.add(labelLinkHelp);
        bookmark.add(labelLinkBookmark);
        settings.add(labelLinkSettings);

        add(home);
        add(help);
        add(bookmark);
        add(settings);

        // link in the logo CACAO
        add(new BookmarkablePageLink("home1", Index1.class));

        if (((UserSession) getSession()).getUserProfile().getLogin().equals("default")) {
            settings.setVisible(false);
            bookmark.setVisible(false);
        } else {
            settings.setVisible(true);
            bookmark.setVisible(true);
        }
        final QueryWS queryWS = new QueryWS(query, refresh);
        queryWS.query_and_get_facets_tags();

        Label labelMsgAjaxWait = new Label("msgWaitAjaxProcess", new StringResourceModel("msgWait", this, null,"XXXX"));
        add(labelMsgAjaxWait);

        addDescriptionResults();
        addTranslationOptions();
        addFacets();

        addListDocs();
        addSectionNewSearch();
        addTagCloud();
    }


    private void addDescriptionResults() {
        // Section describing the results
        String descriptionQuery = "Searching for <font color='#21446D'><b>" + query.getQuery() + "</b></font>";
        descriptionQuery += " - Total documents retrieved:  " + refresh.getContRecords();

        AjaxLink originalQuery = new AjaxLink("originalQuery") {
            private static final long serialVersionUID = 1L;
            @Override
            public void onClick(AjaxRequestTarget target) {
                /* update query according to the new selection
                 * in the facets */
                refresh.setSearchQuery(refresh.getOriginalSearchQuery());
                if (containerListDocs != null) {
                    target.addComponent(containerListDocs);
                }
                if (containerTagCloud != null) {
                    target.addComponent(containerTagCloud);
                }
                if (containerFacets != null) {
                    target.addComponent(containerFacets);
                }
                if (containerDescriptionResults != null) {
                    target.addComponent(containerDescriptionResults);
                }
                if (containerTranslation != null) {
                    target.addComponent(containerTranslation);
                }
                target.addComponent(refresh.getLabelDescriptionNavigator());
            }
        };

        Label labelOriginalQuery  = new Label("labelOriginalQuery",new Model());
        labelOriginalQuery.setDefaultModelObject("Original Query");
        originalQuery.add(labelOriginalQuery);

        String relatedTerms = "";
        for (int x = 0; x < refresh.getSearchTerms().size(); x++) {
            relatedTerms = relatedTerms + refresh.getSearchTerms().get(x).lemma;
            relatedTerms += "(" + refresh.getSearchTerms().get(x).lang + ") - ";
        }
        String descriptionTranslations = "Translated terms: <b>" + relatedTerms + "</b>";

        refresh.setDescriptionResults(descriptionQuery);
        refresh.setDescriptionTranslations(descriptionTranslations);

        containerDescriptionResults.addOrReplace(originalQuery);
        containerDescriptionResults.addOrReplace(refresh.getLabelDescriptionResults());
        containerDescriptionResults.addOrReplace(refresh.getLabelDescriptionTranslations().setEscapeModelStrings(false));
        addOrReplace(containerDescriptionResults);
    }


    private void addFacets() {
        @SuppressWarnings("unchecked")

        // List of facet available to add (in case where the user remove one facet)
        FacetAvailableAddProvider facetAddProvider = new FacetAvailableAddProvider(refresh);
        @SuppressWarnings("unchecked")
        final Label labelAddFacet = new Label("addFacet", new StringResourceModel("labelAddFacet", this, null,"XXXX"));
        labelAddFacet.setVisible(false);
        labelAddFacet.setOutputMarkupId(true);
        @SuppressWarnings("unchecked")
        DataView listAddFacets = new DataView("listAddFacet",facetAddProvider) {
            private static final long serialVersionUID = 1L;
            @Override
            protected void populateItem(Item item) {
                final Facet fa = (Facet) item.getModelObject();
                AjaxLink link = new AjaxLink("facetAdd") {
                    private static final long serialVersionUID = 1L;
                    public void onClick(AjaxRequestTarget target) {
                        refresh.addFacet(fa);
                        target.addComponent(containerFacets);
                    }
                };
                link.add(new Label("nameFacetAdd", new StringResourceModel(fa.getNameUI(), this, null,"XXXX")));
                item.add(link);
            }
        };
        containerFacets.addOrReplace(labelAddFacet);
        containerFacets.addOrReplace(listAddFacets);

        // List of facets
        FacetDataProvider facetProvider = new FacetDataProvider(refresh);
        @SuppressWarnings("unchecked")
        final DataView list = new DataView("listFacets", facetProvider) {
            private static final long serialVersionUID = 1L;
            @Override
            protected void populateItem(Item item) {
                final Facet f = (Facet) item.getModelObject();
                item.add(new Label("nameFacet", new StringResourceModel(f.getNameUI(), this, null,"XXXX")));
                item.add(new AjaxLink("up") {
                    private static final long serialVersionUID = 1L;
                    @Override
                    public void onClick(AjaxRequestTarget target) {
                        /* Change the order of the current facet */
                        // 1) what is the key of the facet clicked
                        Integer keyFacet = refresh.getKeyFacet(f.getNameSolr());
                        // 2) if the key > 1 (the facet is not in the first position in the list), change the positions
                        //    between the current facet and the previous one
                        if (keyFacet > 1) {
                            String facetBefore = (String)refresh.getFacetOrder().get(keyFacet-1);
                            refresh.getFacetOrder().put(keyFacet-1,f.getNameSolr());
                            refresh.getFacetOrder().put(keyFacet,facetBefore);
                        }
                        target.addComponent(containerFacets);
                    }
                });

                item.add(new AjaxLink("down") {
                    private static final long serialVersionUID = 1L;
                    @Override
                    public void onClick(AjaxRequestTarget target) {
                        /* Change the order of the current facet */
                        // 1) what is the key of the facet clicked
                        Integer keyFacet = refresh.getKeyFacet(f.getNameSolr());
                        // 2) if the key < size of the list (the facet is not in the last position in the list), change the positions
                        //    between the current facet and the next one
                        if (keyFacet < (refresh.getListFacets().size())) {
                            String facetAfter = (String)refresh.getFacetOrder().get(keyFacet+1);
                            refresh.getFacetOrder().put(keyFacet+1,f.getNameSolr());
                            refresh.getFacetOrder().put(keyFacet,facetAfter);
                        }
                        target.addComponent(containerFacets);
                    }
                });

                 item.add(new AjaxLink("delete") {
                    private static final long serialVersionUID = 1L;
                    @Override
                    public void onClick(AjaxRequestTarget target) {
                        /* Delete the facet from the list */
                        refresh.removeFacet(f.getNameSolr());
                        if (refresh.getAvailableFacetToAdd().size() > 0) {
                              labelAddFacet.setVisible(true);
                        }
                        else {
                             labelAddFacet.setVisible(false);
                        }
                        target.addComponent(containerFacets);
                        target.addComponent(labelAddFacet);
                    }
                });

                CheckGroup group = new CheckGroup("groupValues");
                List facetValues = f.getValues(f.getSizeList());
                ListView values = new ListView("listFacetValues", facetValues) {
                    private static final long serialVersionUID = 1L;
                    protected void populateItem(final ListItem item1) {
                        item1.add(new Label("facetValue", new PropertyModel(item1.getModel(), "value")));
                        item1.add(new Label("facetFreq", new PropertyModel(item1.getModel(), "freq")));
                        item1.add(new AttributeModifier("class", true, new AbstractReadOnlyModel<String>() {
                            private static final long serialVersionUID = 1L;
                            @Override
                            public String getObject() {
                                return (item1.getIndex() % 2 == 1) ? "even" : "odd";
                            }
                        }));
                        final AjaxCheckBox c = new AjaxCheckBox("check", new PropertyModel(item1.getModel(), "selected")) {
                            private static final long serialVersionUID = 1L;
                            protected void onUpdate(final AjaxRequestTarget target) {
                                /* update query according to the new selection
                                 * in the facets */
                                updateSelectionaFacets((FacetValue) item1.getModelObject(), getValue());
                                if (containerListDocs != null) {
                                    target.addComponent(containerListDocs);
                                }
                                if (containerTagCloud != null) {
                                    target.addComponent(containerTagCloud);
                                }
                                if (containerFacets != null) {
                                    target.addComponent(containerFacets);
                                }
                                target.addComponent(refresh.getLabelDescriptionNavigator());
                            }
                         
                        };
                        item1.add(c);
                    }
                };
                group.add(values);
                final Label labelMore = new Label("labelMore",new StringResourceModel("labelMore", this, null,"XXXX"));
                AjaxLink a = new AjaxLink("more") {
                    private static final long serialVersionUID = 1L;
                    @Override
                    public void onClick(AjaxRequestTarget target) {
                        if (f.getSizeList() == 5) {
                            f.setSizeList(f.getSizeListOriginal());
                            //labelMore.setDefaultModelObject("Less ...");
                        } else {
                            f.setSizeList(5);
                            //labelMore.setDefaultModelObject("More ...");
                        }
                        // TO DO: problem here: label "More ..." did not change!!!!
                        //target.addComponent(labelMore);
                        target.addComponent(containerFacets);
                    }
                };
                a.add(labelMore);
                group.add(a);
                item.add(group);
            }
        };
        containerFacets.addOrReplace(list);
        addOrReplace(containerFacets);
      
    }

    @SuppressWarnings({"unchecked", "unchecked"})
    private void addListDocs() {
        refresh.setFirst(1);
        refresh.setLast(Constant.PAGINIG);
        refresh.setDescriptionsNavigator("Results 1 - " + Constant.PAGINIG + " of about " + refresh.getContRecords());

        Label labelNumDocsFilter = new Label("labelNumDocsFilter", new StringResourceModel("labelNumDocsFilter", this, null,"XXXX"));

        final List num = new ArrayList();
        num.add("10");
        num.add("20");
        num.add("50");
        num.add("100");
        num.add("All");
        final DropDownChoice numCombo = new DropDownChoice("numDocs", new Model(), num);
        numCombo.setDefaultModelObject("All");
        //{
        //protected boolean wantOnSelectionChangedNotifications() {
        //    return true;
        // }

        //protected void onSelectionChanged(final Object newSelection) {
        //String newValue = (String) newSelection;
        //     System.out.println("NEW VALUE: =============================================================");// + newValue);
        //manager.setNewNumberListDocs(newValue);
        // }

        //};

        numCombo.add(new AttributeModifier("title", true, new Model("N documents more relevant, using score.")));
        numCombo.add(new AjaxFormComponentUpdatingBehavior("onchange") {
            private static final long serialVersionUID = 1L;
            @Override
            protected void onUpdate(AjaxRequestTarget arg0) {
                //arg0.addComponent(refresh.getCurrentResults());
                //refresh.setCurrentTotResults(manager.getListIDsDocs().size());
                //refresh.setNewModelCurrentResuls("Results " + refresh.getFirst() + " - " + refresh.getLast() + " of about " + refresh.getCurrentTotResults());
                //arg0.addComponent(containerListDocs);
                //arg0.addComponent(containerTagCloud);
            }
        });
        containerListDocs.addOrReplace(numCombo);
        // The list did not accept a variable not final
        final Refresh finalRefresh = refresh;
        DocumentDataProvider docProvider = new DocumentDataProvider(refresh);
        
        // Modal window for bookmark
        final ModalWindow modal1 = new ModalWindow("modal1");
        containerListDocs.add(modal1);
        modal1.setResizable(false);
        modal1.setInitialWidth(300);
        modal1.setInitialHeight(150);
        modal1.setWidthUnit("px");

        modal1.setPageCreator(new ModalWindow.PageCreator() {
            private static final long serialVersionUID = 1L;
            public Page createPage() {
                return new ModalWindowBookmark(ResultsPage.this, modal1, refresh);
            }
        });
        modal1.setWindowClosedCallback(new ModalWindow.WindowClosedCallback() {
            private static final long serialVersionUID = 1L;
            public void onClose(AjaxRequestTarget target) {
            }
        });
        modal1.setCloseButtonCallback(new ModalWindow.CloseButtonCallback() {
            private static final long serialVersionUID = 1L;
            public boolean onCloseButtonClicked(AjaxRequestTarget target) {
                return true;
            }
        });

        // to control duplicated titles
        this.setDuplicated(false);
        this.setTitleDuplicated("first");

        dataView = new DataView("docs", docProvider) {
                    private static final long serialVersionUID = 1L;
                    @Override
                    protected void populateItem(final Item item) {
                        final Document doc = (Document) item.getModelObject();

                        String title = doc.getTitle();
                        if (title == null) {
                            title = "";
                        }

                        String publisher = doc.getPublisher();
                        if (publisher == null) {
                            publisher = "";
                        }

                        String libraryID = doc.getLibraryID();
                        if (libraryID == null) {
                            libraryID = "";
                        }

                        HashSet creatorsTemp = doc.getCreators();
                        String creators;
                        if (creatorsTemp == null) {
                            creators = "";
                        } else {
                            creators = creatorsTemp.toString();
                        }
                        String tel_URL = doc.getTelRecordID_URL();
                        if (tel_URL == null) {
                            tel_URL = "";
                        }

                        HashSet languageTemp = doc.getLanguages();
                        String languages;
                        if (languageTemp == null) {
                            languages = "";
                        } else {
                            languages = languageTemp.toString();
                        }

                        HashSet authorsTemp = doc.getCreators();
                        String authors;
                        if (authorsTemp == null) {
                            authors = "";
                        } else {
                            authors = authorsTemp.toString();
                        }

                        HashSet tableTemp = doc.getTableOfContents();
                        String table;
                        if (tableTemp == null) {
                            table = "";
                        } else {
                            table = tableTemp.toString();
                        }

                        HashSet subjectsTemp = doc.getSubjects();
                        String subjects;
                        if (subjectsTemp == null) {
                            subjects = "";
                        } else {
                            subjects = subjectsTemp.toString();
                        }

                        String ISSN = doc.getISSN();
                        if (ISSN == null) {
                            ISSN = "";
                        }

                        String ISBN = doc.getISBN();
                        if (ISBN == null) {
                            ISBN = "";
                        }

                        String URL = doc.getURL();
                        if (URL == null) {
                            URL = "";
                        }

                        HashSet oaiTemp = doc.getOaiSets();
                        String oai;
                        if (oaiTemp == null) {
                            oai = "";
                        } else {
                            oai = oaiTemp.toString();
                        }

                        //String truncTitle = ((title.length() <= Constant.TRUNC_TITLE) ? title : (title.substring(0, Constant.TRUNC_TITLE) + "..."));
                        String itemDoc = ""; // "<a href=" + tel_URL + ">" + truncTitle + "</a>";
                        itemDoc += " / by " + creators;
                        itemDoc += "<br> <b>Publisher</b>: " + publisher + " " + "<b>Library</b>: " + libraryID;
                        itemDoc += " <b>Languages</b>: " + languages;

                        if (title.equalsIgnoreCase(getTitleDuplicated())) {
                            duplicated = true;
                        }

                        if (duplicated) {
                            item.add(new Label("titleDoc", "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + finalRefresh.highlight(title) + " / by " + creators).setEscapeModelStrings(false));
                            itemDoc = "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; <b>Publisher</b>: " + publisher + " " + "<b>Library</b>: " + libraryID;
                            itemDoc += " <b>Languages</b>: " + languages;
                            item.add(new Label("itemDoc", finalRefresh.highlight(itemDoc)).setEscapeModelStrings(false));
                        } else {
                            item.add(new Label("titleDoc", finalRefresh.highlight(title)).setEscapeModelStrings(false));
                            item.add(new Label("itemDoc", finalRefresh.highlight(itemDoc)).setEscapeModelStrings(false));
                        }
                        
                        duplicated = false;
                        setTitleDuplicated(title);

                        item.add(new Label("labelAuthors",new StringResourceModel("labelAuthors", this, null,"XXXX")));
                        item.add(new Label("labelLanguages",new StringResourceModel("labelLanguages", this, null,"XXXX")));
                        item.add(new Label("labelLibrary",new StringResourceModel("labelLibrary", this, null,"XXXX")));
                        item.add(new Label("labelPublisher",new StringResourceModel("labelPublisher", this, null,"XXXX")));
                        item.add(new Label("labelTableContents",new StringResourceModel("labelTableContents", this, null,"XXXX")));
                        item.add(new Label("labelSubjects",new StringResourceModel("labelSubjects", this, null,"XXXX")));
                        item.add(new Label("labelISSN",new StringResourceModel("labelISSN", this, null,"XXXX")));
                        item.add(new Label("labelISBN",new StringResourceModel("labelISBN", this, null,"XXXX")));
                        item.add(new Label("labelURL",new StringResourceModel("labelURL", this, null,"XXXX")));
                        item.add(new Label("labelTEL_URL",new StringResourceModel("labelTEL_URL", this, null,"XXXX")));
                        item.add(new Label("labelOAI",new StringResourceModel("labelOAI", this, null,"XXXX")));

                        item.add(new Label("authors",authors));
                        item.add(new Label("languages",languages));
                        item.add(new Label("library",libraryID));
                        item.add(new Label("publisher",publisher));
                        item.add(new Label("tableContents",table));
                        item.add(new Label("subjects",subjects));
                        item.add(new Label("ISSN",ISSN));
                        item.add(new Label("ISBN",ISBN));
                        item.add(new Label("URL",URL));
                        item.add(new Label("telURL","<a href=" + tel_URL + " target='_blank'>" + tel_URL + "</a>").setEscapeModelStrings(false));
                        item.add(new Label("oai",oai));

                        AjaxLink a = new AjaxLink("bookmark") {
                            private static final long serialVersionUID = 1L;
                            @Override
                            public void onClick(AjaxRequestTarget arg0) {
                                   refresh.setDocumentBookmark(doc);
                                   modal1.show(arg0);
                            }
                        };
                        a.add(new AttributeModifier("title", true, new Model("Bookmark the selected document.")));
                        item.add(a);

                        if (((UserSession) getSession()).getUserProfile().getLogin().equals("default")) {
                            a.setVisible(false);
                        } else {
                            a.setVisible(true);
                        }
                        // used to change the color of the table lines
                        // item.add(new AttributeModifier("class", true, new AbstractReadOnlyModel<String>() {
                        //    private static final long serialVersionUID = 1L;
                        //    @Override
                        //    public String getObject() {
                        //        return (item.getIndex() % 2 == 1) ? "document_list even" : "document_list odd";
                        //    }
                        //}));
                    }
                };
        dataView.setItemsPerPage(Constant.PAGINIG);
        containerListDocs.addOrReplace(dataView);
        containerListDocs.addOrReplace(labelNumDocsFilter);
        add(containerListDocs);
        containerListDocs.addOrReplace(refresh.getLabelDescriptionNavigator());

        Label msgZeroDocs = new Label("msgZeroDocs",new StringResourceModel("msgZeroDocs", this, null,"XXXX"));
        if (refresh.getContRecords() > 0) {
            msgZeroDocs.setVisible(false);
        }
        else {
            msgZeroDocs.setVisible(true);
        }
        containerListDocs.add(msgZeroDocs);


        // TODO: create classe unique for navigatorCACAO: navigator and navigator1 *************************************************
        containerListDocs.addOrReplace(new AjaxPagingNavigator("navigator", dataView) {
            private static final long serialVersionUID = 1L;
            @Override
            protected void onAjaxEvent(AjaxRequestTarget target) {
                super.onAjaxEvent(target);
                // because start with 0 and in the interface is shown from 1
                int current = dataView.getCurrentPage() + 1;
                int first = ((current - 1) * Constant.PAGINIG) + 1;
                int last = current * Constant.PAGINIG;
                refresh.setFirst(first);
                refresh.setLast(last);
                target.addComponent(finalRefresh.getLabelDescriptionNavigator());
            }
        });

        containerListDocs.addOrReplace(new AjaxPagingNavigator("navigator1", dataView) {
            private static final long serialVersionUID = 1L;
            @Override
            protected void onAjaxEvent(AjaxRequestTarget target) {
                super.onAjaxEvent(target);
                int current = dataView.getCurrentPage() + 1;
                int first = ((current - 1) * Constant.PAGINIG) + 1;
                int last = current * Constant.PAGINIG;
                refresh.setFirst(first);
                refresh.setLast(last);
                target.addComponent(finalRefresh.getLabelDescriptionNavigator());
            }
        });
       // TODO: create classe unique for navigatorCACAO: navigator and navigator1 *************************************************
    }

    private void addSectionNewSearch() {
        //Refresh.getBtnSearch().setEnabled(false);
        @SuppressWarnings("unchecked")
        Label labelNewSearch = new Label("labelNewSearch", new StringResourceModel("labelNewSearch", this, null,"XXXX"));
        @SuppressWarnings("unchecked")
        final TextField queryField = new TextField("query", new Model(""));
        Button btnSearch = new Button("btnSearch", new StringResourceModel("labelButtonSearch", this, null,"XXXX"));
        Form form = new Form("formNewSearch") {
            private static final long serialVersionUID = 1L;
            @Override
            public void onSubmit() {
                query.setQuery(queryField.getDefaultModelObjectAsString());
                ResultsPage rp = null;
                try {
                    rp = new ResultsPage(query);
                    this.setResponsePage(rp);
                } catch (Exception ex) {
                    logger.error("Exception" + ex);
                }
            }
        };
        form.add(labelNewSearch);
        form.add(queryField);
        form.add(btnSearch);
        add(form);
    }

    @SuppressWarnings({"unchecked", "unchecked"})
    private void addTranslationOptions() {
        @SuppressWarnings("unchecked")
        Form form = new Form("formTraduction");
        TranslationsDataProvider provider = new TranslationsDataProvider(refresh);
        final DataView listTerms = new DataView("listLangs", provider) {
            private static final long serialVersionUID = 1L;
            @Override
            protected void populateItem(Item item) {
                final Facet f = (Facet) item.getModelObject();
                item.add(new Label("lang", f.getNameSolr()));
                CheckGroup groupTermsTraduction = new CheckGroup("groupTerms", new ArrayList());
                ListView listTrans = new ListView("listTrans", f.getValues(f.getSizeListOriginal())) {
                    private static final long serialVersionUID = 1L;
                    protected void populateItem(final ListItem item1) {
                        item1.add(new Label("langTrans", new PropertyModel(item1.getModel(), "value")));
                        item1.add(new CheckBox("checkTrans", new PropertyModel(item1.getModel(), "selected")));
                    }
                };
                groupTermsTraduction.add(listTrans);
                item.add(groupTermsTraduction);
            }
        };
        
        // Section new translation
        final TextField inputNewTrans = new TextField("inputNewTerm", new Model());
        inputNewTrans.setOutputMarkupId(true);

        final List langs = new ArrayList();
        // TODO: UNIFORME the list of LANGS
        for (int x = 0; x < query.getTargetLang().length; x++) {
            langs.add(query.getTargetLang()[x]);
        }
        final DropDownChoice langTrans = new DropDownChoice("listLangTrans", new Model(), langs);
        langTrans.setOutputMarkupId(true);

        // When the user "update search"
        AjaxButton b = new AjaxButton("submitTraduction", new StringResourceModel("labelButtonUpdateSearch", this, null,"XXXX")) {
            @Override
            protected void onSubmit(AjaxRequestTarget target, Form<?> form) {

                /* Based on the list of current values listLang, that contains all translations
                 * and the corresponding value false or true (and also the new translations proposed by the user)
                 * manipulate the searchQuery
                 */

                // 1) Manipulate the query: listTranslations is refreshed according to the values of the checkbox (use of model)
                SearchQueryManipulate searchQuery = new SearchQueryManipulate(refresh);
                String newSearchQuery = searchQuery.getNewSearchQuery();
                refresh.setSearchQuery(newSearchQuery);

                // 2) Update components according to the new searchQuery
                if (containerListDocs != null) {
                    target.addComponent(containerListDocs);
                }
                if (containerTagCloud != null) {
                    target.addComponent(containerTagCloud);
                }
                if (containerFacets != null) {
                    target.addComponent(containerFacets);
                }

                if (containerFacets != null) {
                    target.addComponent(containerFacets);
                }

                if (containerDescriptionResults != null) {
                    target.addComponent(containerDescriptionResults);
                }

                if (containerTranslation != null) {
                    target.addComponent(containerTranslation);
                }

                target.addComponent(refresh.getLabelDescriptionNavigator());
            }
        };
        
        // When the user indicate a new translation
        AjaxButton b1 = new AjaxButton("buttonNew", new StringResourceModel("labelButtonAddTranslation", this, null,"XXXX")) {
            private static final long serialVersionUID = 1L;
            @Override
            protected void onSubmit(AjaxRequestTarget target, Form<?> form) {
                      // TODO: logger? What is the log to put this information?? lang + newTrans
                      String lang = langTrans.getDefaultModelObjectAsString();
                      String newTrans = inputNewTrans.getDefaultModelObjectAsString();
                      refresh.addNewTranslation(lang,newTrans);
                      // Clean the fields for the next interaction
                      langTrans.setDefaultModelObject("");
                      inputNewTrans.getDefaultModelObjectAsString("");
                      target.addComponent(containerTranslation);
                      target.addComponent(langTrans);
                      target.addComponent(inputNewTrans);
           }
        };

        form.add(b);
        form.add(b1);
        form.add(langTrans);
        form.add(listTerms);
        form.add(inputNewTrans);
                    
        containerTranslation.add(form);
        add(containerTranslation);
    }

    public String getAjaxIndicatorMarkupId() {
        return "proc";
    }

    private void addTagCloud() {
        @SuppressWarnings("unchecked")
        TagDataProvider tagProvider = new TagDataProvider(refresh);
        @SuppressWarnings("unchecked")
        DataView wordTag = new DataView("wordTag", tagProvider) {
            private static final long serialVersionUID = 1L;
            @Override
            protected void populateItem(final Item item) {
               long freq = ((Tag)item.getDefaultModelObject()).getFreq();
                //logger.debug(freq);
                AjaxLink link = new AjaxLink("link", new PropertyModel(item.getModel(), "name")) {
                    private static final long serialVersionUID = 1L;
                    @Override
                    public void onClick(AjaxRequestTarget target) {

                                /* update query according to the new selection
                                 * in the facets. In case of tag cloud, the facet is teFacet and the click means that
                                 * the facet = true ... +teFacet() in searchQuery
                                 */
                                Facet f = new Facet("teFacet");
                                FacetValue fv = new FacetValue(((Tag)item.getModelObject()).getName());
                                fv.setFacet(f);
                                updateSelectionaFacets(fv,"true");
                                if (containerListDocs != null) {
                                    target.addComponent(containerListDocs);
                                }
                                if (containerTagCloud != null) {
                                    target.addComponent(containerTagCloud);
                                }
                                if (containerFacets != null) {
                                    target.addComponent(containerFacets);
                                }
                                target.addComponent(refresh.getLabelDescriptionNavigator());
                    }
                };
                link.add(new Label("sizeBegin", "<font size=" + refresh.getSize(freq,refresh.getGreater(),refresh.getLower()) +
                                   " color=" + refresh.getColor(freq, refresh.getGreater(),refresh.getLower()) + ">").setEscapeModelStrings(false));
                link.add(new Label("name", new PropertyModel(item.getModel(), "name")));
                link.add(new Label("sizeEnd", "</font>").setEscapeModelStrings(false));
                item.add(link);
            }
        };
        wordTag.setOutputMarkupId(true);
        containerTagCloud.addOrReplace(wordTag);

        Label label = new Label("labelTypeTagCloud",new StringResourceModel("labelTypeTagCloud", this, null,"XXXX"));
        final List type = new ArrayList();
        type.add("Named Entity");
        type.add("All");
        final DropDownChoice typeCombo = new DropDownChoice("typeTagCloud", new Model(), type);
        typeCombo.setDefaultModelObject("All");
        typeCombo.add(new AjaxFormComponentUpdatingBehavior("onchange") {
            private static final long serialVersionUID = 1L;
            @Override
            protected void onUpdate(AjaxRequestTarget arg0) {

            }
        });
        containerTagCloud.add(label);
        containerTagCloud.add(typeCombo);

        Label label1 = new Label("labelVisualTagCloud",new StringResourceModel("labelVisualTagCloud", this, null,"XXXX"));
        final List visual = new ArrayList();
        visual.add("Standard");
        visual.add("Graphical");
        final DropDownChoice visualCombo = new DropDownChoice("visualTagCloud", new Model(), visual);
        visualCombo.setDefaultModelObject("Standard");
        visualCombo.add(new AjaxFormComponentUpdatingBehavior("onchange") {
            private static final long serialVersionUID = 1L;
            @Override
            protected void onUpdate(AjaxRequestTarget arg0) {

            }
        });
        containerTagCloud.add(label1);
        containerTagCloud.add(visualCombo);

        addOrReplace(containerTagCloud);
    }


    // TODO: to implement another kind of duplication "strategy"
    public void setDuplicated(boolean duplicated) {
        this.duplicated = duplicated;
    }

    public void setTitleDuplicated(String titleDuplicated) {
        this.titleDuplicated = titleDuplicated;
    }

    public boolean isDuplicated() {
        return duplicated;
    }

    public String getTitleDuplicated() {
        return titleDuplicated;
    }

    // When the facet values are initialized with false
    private void updateSelectionaFacets(FacetValue fv, String selec) {
        //update hashtable
        List<String> v1 = refresh.getVectorFacet(fv.getFacet().getNameSolr());
        if (selec.equals("true")) {
            logger.info("add " + fv.getValue());
            if (v1.size() == 0) {
                refresh.setNewItemListSearchFacets(fv.getFacet().getNameSolr());
            }
            refresh.add(fv.getFacet().getNameSolr(), fv.getValue());
        } else {
            if (v1.size() != 0) {
                refresh.remove(fv.getFacet().getNameSolr(), fv.getValue());
            }
        }
        logger.debug("Vector: " + v1.toString());
        //build the searchQuery from the hashTable
        Enumeration e = refresh.getListSearchFacets().keys();
        String add = "";
        while (e.hasMoreElements()) {
            String key = (String) e.nextElement();
            List<String> listValues = refresh.getVectorFacet(key);
            String facetValues = " +" + key + ":(";
            for (int y = 0; y < listValues.size(); y++) {
                String fv1 = listValues.get(y);
                facetValues += "\"" + fv1 + "\"";
                if (listValues.size() > y + 1) {
                    facetValues += ",";
                }
            }
            facetValues += ")";
            if (listValues.size() > 0) {
                add += facetValues;
            }
        }
        String searchQuery = refresh.getOriginalSearchQuery();
        searchQuery += add;
        refresh.setSearchQuery(searchQuery);
    }


}