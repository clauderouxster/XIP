/*
 * Xerox Research Center Europe
 * Copyright (C) 2009 XRCE, All Rights Reserved
 */
package extui.UI_components;

import eu.project.Cacao.Components.Utils.NLPtoken;
import eu.project.Cacao.WS_Clients.CACAO_queryWS_client;
import extui.UI_html.Application;
import java.util.List;
import java.util.Vector;
import org.apache.log4j.Logger;

/**
 *
 * @author ctrojahn
 */
public class SearchQueryManipulate {

    private List<Facet> list;
    private Vector<NLPtoken> listOriginal;
    private String newSearch;
    private String currentSearchQuery;
    private Logger logger = Logger.getLogger(SearchQueryManipulate.class.getName());


    public SearchQueryManipulate(Refresh refresh) {
        this.list = refresh.getListTranslations();
        this.listOriginal = refresh.getSearchTerms();
        this.currentSearchQuery = refresh.getSearchQuery();
        manipulate();
    }


    public String getNewSearchQuery() {
        return newSearch;
    }

    private void manipulate() {
        /* For each element in the list of translations:
         * 1) add new translation; or
         * 2) delete an existing one
         */
         for (int x = 0; x < list.size(); x++) {
            Facet f = list.get(x);
            List<FacetValue> listValues = f.getValues(f.getSizeListOriginal());
            for (int y = 0; y < listValues.size(); y++) {
                FacetValue fv = listValues.get(y);
                /* Depending on the value of the property "selected" of the translation
                 * add (true) or remove (false).
                 */
                if (fv.getSelected()) {
                    /* if selected = true, need to be verified if the terms is a new one or not
                     * if yes, add new translation in the query
                     */
                    if (!isInOriginalList(fv)) {
                         logger.info("ADD --------------------------------------" + fv.getValue());
                         String newTranslationSearchQuery = getSearchWS(fv);
                         this.newSearch = newTranslationSearchQuery.concat(currentSearchQuery);
                    }
                } else {
                    /* if selected = false, the corresponding translation needs to be deleted of the search query */
                    if (isInOriginalList(fv)) {
                         logger.info("REMOVE------------------------------------" + fv.getValue());
                         String remTranslationSearchQuery = getSearchWS(fv);
                         this.newSearch = currentSearchQuery.replace(new StringBuffer(remTranslationSearchQuery),new StringBuffer());
                    }
                }
            }
        }

        /* without using the ws ----------------------------------------------------------------------------------------------------
        for (int x = 0; x < list.size(); x++) {
            Facet f = list.get(x);
            List<FacetValue> listValues = f.getValues(f.getSizeListOriginal());
            for (int y = 0; y < listValues.size(); y++) {
                FacetValue fv = listValues.get(y);
                if (fv.getSelected()) {
                    /* if selected = true, need to be verified if the terms is a new one
                     * if yes, add new translation in the query
                     */
                    // The translation is selected and is not yet in the string search; it means that the translation has been added
                    /*
                    if (!existPattern(fv)) {
                         logger.info("ADD: ------------------------------------- " + fv.getValue());
                         String pattern = getPattern(fv);
                         logger.info("ADD: ---------------------------------- " + pattern);
                         this.newSearch = pattern.concat(currentSearchQuery);
                         
                    }
                } else {
                    /* if selected = false, the corresponding translation needs to be deleted of the search query */
                    /*
                    if (existPattern(fv)) {
                        logger.info("REMOVE:------------------------------------ " + fv.getValue());
                        String pattern = getPattern(fv);
                        logger.info("REMOVE: -----------------------------------" + pattern);
                        this.newSearch = currentSearchQuery.replace(new StringBuffer(pattern),new StringBuffer());
                    }
                }
            }
        }
        */
    }

    private boolean isInOriginalList(FacetValue fv) {
        boolean isIn = false;
        for (int x = 0; x < listOriginal.size(); x++) {
            
            String lang = listOriginal.get(x).lang;
            String trans = listOriginal.get(x).lemma;
            
            if ( (fv.getValue().equals(trans)) &&
                 (fv.getFacet().getNameSolr().equals(lang))) {
                    isIn = true;
                    break;
            }
            
        }
        logger.info("---------------------------------------------- " + fv.getValue() + "  " + isIn);
        return isIn;
    }

    private String getSearchWS(FacetValue fv) {
        CACAO_queryWS_client client = null;
        String newSearch = "";
        try {
            client = new CACAO_queryWS_client(Application.getRootWsUrl() + "/axis2/services/CACAO_queryWS");
            String[] target = { fv.getFacet().getNameSolr()};
            String msg = client.processQueryAndSearchCustom(fv.getValue(), fv.getFacet().getNameSolr(), target,
                                                            false, false, false, false,Constant.FACETS, 0, 1);
            client.parseResultMSG(new String(msg.getBytes(), "UTF-8"));
            newSearch = client.getSearchQuery();
            logger.info("NEW SEARCH --------------------------------------------" + newSearch);
        } catch (Exception ex) {
            logger.error("Error first query WS: " + ex);
        }
        return newSearch;
    }



    // Methods used to manipulate the searchQuery without using the web service -----------------------------------------------------

    private boolean existPattern(FacetValue fv) {
         
         /*  title:(banque)^2.0 subject:(banque)^1.5 tableOfContents:(banque)^1.0 description:(banque)^1.0
             title_lemmatized:(banque_noun)^2.0 subject_lemmatized:(banque_noun)^1.0 description_lemmatized:(banque_noun)^1.0
             tableOfContents_lemmatized:(banque_noun)^1.0 subject_frStem:(banque)^1.0 title_frStem:(banque)^1.0
             tableOfContents_frStem:(banque)^1.0 description_frStem:(banque)^1.0 title:(...)*/
        
         boolean exists;
         String language = fv.getFacet().getNameSolr();
         String translation = fv.getValue();
         String p = "";

         // If is a Name Entity, put " "
         if (translation.contains("\"")) {
              String translation_noun = translation.replaceAll("\"", "");
               p = "title:(" + translation +")^2.0 " +
                    "subject:(" + translation +")^1.5 " +
                    "tableOfContents:(" + translation + ")^1.0 " +
                    "description:(" + translation + ")^1.0 " +
                    "title_lemmatized:(\"" + translation_noun.toLowerCase() + "_noun\")^2.0 " +
                    "subject_lemmatized:(\"" + translation_noun.toLowerCase() + "_noun\")^1.0 " +
                    "description_lemmatized:(\"" + translation_noun.toLowerCase() + "_noun\")^1.0 " +
                    "tableOfContents_lemmatized:(\"" + translation_noun.toLowerCase() + "_noun\")^1.0 " +
                    "subject_" + language + "Stem:(" + translation + ")^1.0 " +
                    "title_" + language + "Stem:(" + translation + ")^1.0 " +
                    "tableOfContents_" + language + "Stem:(" + translation + ")^1.0 " +
                    "description_" + language + "Stem:(" + translation  + ")^1.0";
         }
         else {
                p = "title:(" + translation +")^2.0 " +
                    "subject:(" + translation +")^1.5 " +
                    "tableOfContents:(" + translation + ")^1.0 " +
                    "description:(" + translation + ")^1.0 " +
                    "title_lemmatized:(" + translation.toLowerCase() + "_noun)^2.0 " +
                    "subject_lemmatized:(" + translation.toLowerCase() + "_noun)^1.0 " +
                    "description_lemmatized:(" + translation.toLowerCase() + "_noun)^1.0 " +
                    "tableOfContents_lemmatized:(" + translation.toLowerCase() + "_noun)^1.0 " +
                    "subject_" + language + "Stem:(" + translation + ")^1.0 " +
                    "title_" + language + "Stem:(" + translation + ")^1.0 " +
                    "tableOfContents_" + language + "Stem:(" + translation + ")^1.0 " +
                    "description_" + language + "Stem:(" + translation  + ")^1.0";
         }

         int index = currentSearchQuery.indexOf(p);
         /* If not trying to remove a translation that was already deleted
          (considering that the translation is always in the list with selected = false) */
         if (index > -1) {
              exists = true;
         }
         else {
             exists = false;
         }
         logger.info("INDEX: "  + index  + " " + exists);
         return exists;
    }

    private String getPattern(FacetValue fv) {
                 
         /* title:(banque)^2.0 subject:(banque)^1.5 tableOfContents:(banque)^1.0 description:(banque)^1.0
             title_lemmatized:(banque_noun)^2.0 subject_lemmatized:(banque_noun)^1.0 description_lemmatized:(banque_noun)^1.0
             tableOfContents_lemmatized:(banque_noun)^1.0 subject_frStem:(banque)^1.0 title_frStem:(banque)^1.0
             tableOfContents_frStem:(banque)^1.0 description_frStem:(banque)^1.0 title:(...)*/


        /* title:(Ufer)^2.0 subject:(Ufer)^1.5 tableOfContents:(Ufer)^1.0 description:(Ufer)^1.0 title_lemmatized:(ufer_noun)^2.0
           subject_lemmatized:(ufer_noun)^1.0 description_lemmatized:(ufer_noun)^1.0 tableOfContents_lemmatized:(ufer_noun)^1.0
           subject_deStem:(Ufer)^1.0 title_deStem:(Ufer)^1.0 tableOfContents_deStem:(Ufer)^1.0 description_deStem:(Ufer)^1.0
        */

         String translation = fv.getValue();
         String language = fv.getFacet().getNameSolr();
         String pattern = "";

         /* If the term has a space:
          * Example: without space: bank
          *          with space: "Bank XX" and also for special cases: "Bank XX_noun" with " ?????
          * 
          */
         if (translation.contains("\"")) {
              String translation_noun = translation.replaceAll("\"", "");
              pattern = "title:(" + translation +")^2.0 " +
                        "subject:(" + translation +")^1.5 " +
                        "tableOfContents:(" + translation + ")^1.0 " +
                        "description:(" + translation + ")^1.0 " +
                        "title_lemmatized:(\"" + translation_noun.toLowerCase() + "_noun\")^2.0 " +
                        "subject_lemmatized:(\"" + translation_noun.toLowerCase() + "_noun\")^1.0 " +
                        "description_lemmatized:(\"" + translation_noun.toLowerCase() + "_noun\")^1.0 " +
                        "tableOfContents_lemmatized:(\"" + translation_noun.toLowerCase() + "_noun\")^1.0 " +
                        "subject_" + language + "Stem:(" + translation + ")^1.0 " +
                        "title_" + language + "Stem:(" + translation + ")^1.0 " +
                        "tableOfContents_" + language + "Stem:(" + translation + ")^1.0 " +
                        "description_" + language + "Stem:(" + translation  + ")^1.0";
         }
         else {
              pattern = "title:(" + translation +")^2.0 " +
                        "subject:(" + translation +")^1.5 " +
                        "tableOfContents:(" + translation + ")^1.0 " +
                        "description:(" + translation + ")^1.0 " +
                        "title_lemmatized:(" + translation.toLowerCase() + "_noun)^2.0 " +
                        "subject_lemmatized:(" + translation.toLowerCase() + "_noun)^1.0 " +
                        "description_lemmatized:(" + translation.toLowerCase() + "_noun)^1.0 " +
                        "tableOfContents_lemmatized:(" + translation.toLowerCase() + "_noun)^1.0 " +
                        "subject_" + language + "Stem:(" + translation + ")^1.0 " +
                        "title_" + language + "Stem:(" + translation + ")^1.0 " +
                        "tableOfContents_" + language + "Stem:(" + translation + ")^1.0 " +
                        "description_" + language + "Stem:(" + translation  + ")^1.0";
         }

         return pattern;
    }
}


 /* title:(banque)^2.0 subject:(banque)^1.5 tableOfContents:(banque)^1.0 description:(banque)^1.0
             title_lemmatized:(banque_noun)^2.0 subject_lemmatized:(banque_noun)^1.0 description_lemmatized:(banque_noun)^1.0
             tableOfContents_lemmatized:(banque_noun)^1.0 subject_frStem:(banque)^1.0 title_frStem:(banque)^1.0
             tableOfContents_frStem:(banque)^1.0 description_frStem:(banque)^1.0 title:(...)*/

         /* The delete option depends on the language.
            In this case, It is need to get "description_frStem:(banque)^1.0 title:(...)"
          */

         /*  * -------------------------------------------------------------------------------------------------------------------/
          Before: problem if you get the first occurrence of "bank"; title(bank) can be not the correct one because
          * bank can have multiple translations, for different languages
         String p = "title:("+ fv.getValue() + ")";
         //String p1 = "title:(";
         String p1 = "description_" + fv.getFacet().getNameSolr() + "Stem:(" + fv.getValue() + ")^1.0";
         logger.info("p1 " + p1);
         int beginIndex = currentSearchQuery.indexOf(p);

         /* If not trying to remove a translation that was already deleted
          (considering that the translation is always in the list with selected = false)
          */
         /*
         if (beginIndex > -1) {
            int plus = p1.length();
            // First occurence of p1 after beginIndex
            int endIndex = currentSearchQuery.indexOf(p1,beginIndex);
            if (endIndex > -1) {
                pattern =  currentSearchQuery.substring(beginIndex,endIndex+plus);
            }
            else {
                pattern="";
            }
         } else {
             pattern = "";
         }
         return pattern;
         * -------------------------------------------------------------------------------------------------------------------*/

