/*
 * Xerox Research Center Europe
 * Copyright (C) 2009 XRCE, All Rights Reserved
 */
package extui.UI_ws;

import eu.project.Cacao.Components.Utils.DocsSearchResult;
import eu.project.Cacao.Components.Utils.NLPtoken;
import eu.project.Cacao.WS_Clients.CACAO_searchWS_client;
import extui.UI_components.Constant;
import extui.UI_components.Document;
import extui.UI_components.Facet;
import extui.UI_components.FacetValue;
import extui.UI_components.Refresh;
import extui.UI_components.Tag;
import extui.UI_html.Application;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Enumeration;
import java.util.List;
import java.util.Vector;
import org.apache.log4j.Logger;

/**
 *
 * @author ctrojahn
 */
public class SearchWS {

    private static final Logger logger = Logger.getLogger(SearchWS.class.getName());
    private Refresh refresh;
    CACAO_searchWS_client client;

    /**
     *
     * @param refresh
     */
    public SearchWS(Refresh refresh) {
        this.refresh = refresh;
    }

    /**
     * 
     * @param searchQuery
     * @param start
     * @param tot
     */
    @SuppressWarnings({"unchecked", "unchecked", "unchecked"})
    public void getDocs(String searchQuery, int start, int tot) {
        logger.info("Search WS interval: " + start + " - " + tot +
                " ------------------------------------------------------");
        client = null;
        String msg = null;
        List<DocsSearchResult> l = null;
        try {
            //client = new CACAO_searchWS_client(Application.getRootWsUrl() +
            // "/axis2/services/CACAO_SearchWS");
            // /CacaoSuite/services/CACAO_queryWS
            client = new CACAO_searchWS_client(Application.getRootWsUrl() +
                    "/CacaoSuite/services/CACAO_searchWS");
        } catch (Exception ex) {
            logger.error("CACAO_searchWS_client Exception: " + ex);
            logger.error(ex.getMessage());
            client = null;
        }
        try {
            // TO DO: send only the facets that currently in the list
            msg = client.searchRecords(searchQuery, Constant.FACETS, start, tot);
            logger.debug(searchQuery);
        } catch (Exception ex) {
            logger.error("searchRecords Exception: " + ex);
            logger.error(ex.getMessage());
            client = null;
        }
        try {
            client.parseResultMSG(new String(msg.getBytes(), "UTF-8"));
        } catch (Exception ex) {
            logger.error("parseResultMSG Exception: " + ex);
            logger.error(ex.getMessage());
            client = null;
        }
        List<Document> list = new ArrayList<Document>();
        if (client != null) {
            try {
                l = client.getRecords();
                for (int x = 0; x < l.size(); x++) {
                    Document d = new Document(l.get(x));
                    list.add(d);
                }
                refresh.setListDocuments(list);
                refresh.setContRecords(client.getRecordsFound());
                long total = Constant.PAGINIG;
                if (total > refresh.getContRecords()) {
                    total = refresh.getContRecords();
                }
                refresh.getLabelDescriptionNavigator().setDefaultModelObject("Results 1 - " +
                        total + " of about " + refresh.getContRecords());
                getFacets();
                getTagCloud();
            } catch (Exception ex) {
                logger.error("search WS Exception (get docs): " + ex);
                logger.error(ex.getMessage());
            }
        }
        getFacets();
        getTagCloud();
        //getTranslations();
    }

    @SuppressWarnings("unchecked")
    public void getFacets() {
        // TODO: update facet list: avoid the copy from QueryWs == same code for getting
        // Facets: put in a list
        if (client != null) {
            List<Facet> listFacets = new ArrayList<Facet>();
            Enumeration e = Constant.MAPPING.keys();
            while (e.hasMoreElements()) {
                // Create a new object facet: Facet(String nameSolr,String nameUI)
                String nameSolr = (String) e.nextElement();
                Facet f = new Facet(nameSolr, (String) Constant.MAPPING.get(nameSolr));
                List<FacetValue> listValues = new ArrayList<FacetValue>();
                for (int y = 0; y < client.getFacets().size(); y++) {
                    if (nameSolr.equalsIgnoreCase(client.getFacets().get(y).getFacetField())) {
                        // if the facet is in the list of previous selected facets ....
                        boolean selected = false;
                        if (refresh.exists(nameSolr, client.getFacets().get(y).getFacetValue())) {
                            selected = true;
                        }
                        FacetValue fv = new FacetValue(client.getFacets().get(y).getFacetValue(),
                                client.getFacets().get(y).getFacetCount(), selected);
                        fv.setFacet(f);
                        listValues.add(fv);
                    }
                }
                Collections.sort(listValues);
                f.setValues(listValues);
                listFacets.add(f);
            }
            refresh.setListFacets(listFacets);
            // Create new list to avoid modification in listFacet to be also in listOriginalFacets
            List<Facet> copy = new ArrayList<Facet>();
            copy.addAll(listFacets);
            refresh.setListOriginalFacets(copy);
        }
    }

    public void getTagCloud() {
        if (client != null) {
            List<Tag> listTags = new ArrayList<Tag>();
            for (int y = 0; y < client.getFacets().size(); y++) {
                if (client.getFacets().get(y).getFacetField().equals("teFacet")) {
                    Tag t = new Tag(client.getFacets().get(y).getFacetValue(),
                            client.getFacets().get(y).getFacetCount());
                    listTags.add(t);
                }
            }
            refresh.setTagCloud(listTags);
            if (listTags.size() > 0) {
                refresh.setGreater(listTags.get(0).getFreq());
                refresh.setLower(listTags.get(listTags.size() - 1).getFreq());
            }
        }
    }

    private void getTranslations() {
        /*
         * Create a list of a list: language with corresponding translations
         * For instance: fr: banque, river
         * The structure of Facet and FacetValues are used, in the sense of that
         * one language can be have different translations. But it is just the structure
         * of the class that is used (local to this method)
         */

        //1) Get all facet (for instance, it, fr, en)
        Vector<NLPtoken> searchTerms = refresh.getSearchTerms();
        final List<Facet> listLangs = new ArrayList<Facet>();
        for (int x = 0; x < searchTerms.size(); x++) {
            Facet f = new Facet(searchTerms.get(x).lang);
            boolean has = false;
            for (int y = 0; y < listLangs.size(); y++) {
                if (f.getNameSolr().equalsIgnoreCase(listLangs.get(y).getNameSolr())) {
                    has = true;
                }
            }
            if (!has) {
                listLangs.add(f);
            }
        }
        //2) Get values of each facet (for instance, facet = fr values = banque, reiver ..)
        for (int x = 0; x < listLangs.size(); x++) {
            Facet f = listLangs.get(x);
            List<FacetValue> facetValues = new ArrayList<FacetValue>();
            for (int y = 0; y < searchTerms.size(); y++) {
                if (f.getNameSolr().equalsIgnoreCase(searchTerms.get(y).lang)) {
                    String nameFacetValue = searchTerms.get(y).lemma;
                    FacetValue fv = new FacetValue(nameFacetValue);
                    fv.setFacet(f);
                    facetValues.add(fv);
                }
            }
            f.setValues(facetValues);
        }
        refresh.setListTranslations(listLangs);
    }
}
