/*
 * Xerox Research Center Europe
 * Copyright (C) 2009 XRCE, All Rights Reserved
 */
package extui.UI_ws;

import eu.project.Cacao.Components.Utils.NLPtoken;
import eu.project.Cacao.WS_Clients.CACAO_queryWS_client;
import extui.UI_components.Constant;
import extui.UI_components.Facet;
import extui.UI_components.FacetValue;
import extui.UI_components.Query;
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
 * @author ctrojahn
 */
public class QueryWS { //implements Runnable, Serializable{

    private static final Logger logger = Logger.getLogger(QueryWS.class.getName());
    Query query;
    Refresh refresh;
    CACAO_queryWS_client client;

    /**
     *
     * @param query
     * @param refresh 
     */
    public QueryWS(Query query, Refresh refresh) {
        this.query = query;
        this.refresh = refresh;
        logger.info("---------------------- new Search: launch QueryWS   ---------------");
    }

    public void query_and_get_facets_tags() {
        client = null;
        try {
            //client = new CACAO_queryWS_client(Application.getRootWsUrl() + "/axis2/services/CACAO_queryWS");
            //http://demo.celi.it:8095/CacaoSuite/services/CACAO_queryWS
            client = new CACAO_queryWS_client(Application.getRootWsUrl() +
                    "/CacaoSuite/services/CACAO_queryWS");
            String msg = client.processQueryAndSearchCustom(query.getQuery(),
                    query.getSrcLang(), query.getTargetLang(),
                    query.systran(), query.SVexp(), query.WNexp(), query.W2Cexp(),
                    Constant.FACETS, 0, 1);
            client.parseResultMSG(new String(msg.getBytes(), "UTF-8"));
            refresh.setContRecords(client.getRecordsFound());
            refresh.setOriginalSearchQuery(client.getSearchQuery());
            refresh.setSearchQuery(client.getSearchQuery());
            refresh.setSearchTerms(client.getSearchTerms());
        } catch (Exception ex) {
            logger.error("Error first query WS: " + ex);
            logger.error(ex.getMessage());
            //System.out.println("&&&&&&&&&&& " + ex);
        }
        getTagCloud();
        getFacets();
        getTranslations();
    }

    private void getTagCloud() {
        if (client != null) {
            List<Tag> listTags = new ArrayList<Tag>();
            for (int y = 0; y < client.getFacets().size(); y++) {
                if (client.getFacets().get(y).getFacetField().equals("teFacet")) {
                    Tag t = new Tag(client.getFacets().get(y).getFacetValue(), client.getFacets().get(y).getFacetCount());
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

    private void getFacets() {
        if (client != null) {
            //for(int x=0;x<client.getFacets().size();x++) {
            //    logger.info("WS facet " + client.getFacets().get(x).getFacetField());
            //}
            List<Facet> listFacets = new ArrayList<Facet>();
            Enumeration e = Constant.MAPPING.keys();
            while (e.hasMoreElements()) {
                // Create a new object facet: Facet(String nameSolr,String nameUI)
                String nameSolr = (String) e.nextElement();
                Facet f = new Facet(nameSolr, (String) Constant.MAPPING.get(nameSolr));
                List<FacetValue> listValues = new ArrayList<FacetValue>();
                for (int y = 0; y < client.getFacets().size(); y++) {
                    //logger.info(client.getFacets().get(y).getFacetField());
                    if (nameSolr.equalsIgnoreCase(client.getFacets().get(y).getFacetField())) {
                        FacetValue fv = new FacetValue(client.getFacets().get(y).getFacetValue(), client.getFacets().get(y).getFacetCount(), false);
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
            //for(int x=0;x<listFacets.size();x++) {
            //    logger.info("ADDED " + listFacets.get(x).getNameSolr());
            //}
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