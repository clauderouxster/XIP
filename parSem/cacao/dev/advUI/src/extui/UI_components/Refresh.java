/*
 * Xerox Research Center Europe
 * Copyright (C) 2009 XRCE, All Rights Reserved
 */
package extui.UI_components;

import eu.project.Cacao.Components.Utils.NLPtoken;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.List;
import java.util.Vector;
import org.apache.wicket.markup.html.basic.Label;
import org.apache.wicket.model.Model;
import org.apache.log4j.Logger;

/**
 *
 * @author ctrojahn
 */
public class Refresh {

    private static final Logger logger = Logger.getLogger(Refresh.class.getName());
    
    private Label labelDescriptionResults;
    private Label labelDescriptionTranslations;
    private Label labelDescriptionNavigator;

    private String descriptionResults;
    private String descriptionNavigator;
    private String descriptionTranslations;

    private Vector<NLPtoken> searchTerms;
    // Lucene query: initialized in QueryWS and used in ResultsPage and SearchWS
    private String searchQuery;
    private String originalSearchQuery;

    //
    private long contRecords;

    // Elements of the navigation that must be updated
    private int first;
    private int last;

    // These two lists are used for the FacetDataProvider and DocumentDataProvider
    //  (in order to provide the refresh of facets and documents)
    private List<Facet> listFacets;
    private List<Document> listDocuments;

    // This list is used to manipulate the facets (add and remove)
    private List<Facet> listOriginalFacets;

    // Used to the manipulation of facets and searchQuery
    private Hashtable listSearchFacets;
    private List<Tag> tagCloud;

    // Used to normalize the size of the tags
    private long greater;
    private long lower;
    private Hashtable facetOrder;

    // Used as parameter between ResultsPage and ModalWindowBookmark (there is not another good way to do that!)
    private Document documentBookmark;

    /* List of translations: for instance: fr banque river .... the structure of facets are used (just structure of classes)
     * Such list is manipulated during the interaction wich the user (the user can add new translations)
    */
    private List<Facet> listTranslations;
   
    public Refresh() {
        labelDescriptionResults = new Label("currentQuery", new Model());
        labelDescriptionResults.setEscapeModelStrings(false);
        labelDescriptionResults.setOutputMarkupId(true);
        labelDescriptionNavigator = new Label("res", new Model());
        labelDescriptionNavigator.setOutputMarkupId(true);
        labelDescriptionTranslations = new Label("descriptionTranslations", new Model());
        labelDescriptionTranslations.setOutputMarkupId(true);
        listSearchFacets = new Hashtable();
        tagCloud = new ArrayList<Tag>();
        facetOrder = new Hashtable();
        listTranslations = new ArrayList<Facet>();

        /* Hashtable that specifies the *initial* order of facets 
         * Such order can dinamically be changed by the user
         */
        facetOrder.put(1, "subject_ClassificationCategory");
        facetOrder.put(2, "language");
        facetOrder.put(3, "publisher");
        facetOrder.put(4, "libraryID");
        facetOrder.put(5, "OAIset");
   
    }

    

    /**
     *
     * @return
     */
    public Label getLabelDescriptionResults() {
        labelDescriptionResults.setDefaultModelObject(descriptionResults);
        return labelDescriptionResults;
    }

    public Label getLabelDescriptionNavigator() {
        labelDescriptionNavigator.setDefaultModelObject(descriptionNavigator);
        return labelDescriptionNavigator;
    }

    public Label getLabelDescriptionTranslations() {
        labelDescriptionTranslations.setDefaultModelObject(descriptionTranslations);
        return labelDescriptionTranslations;
    }

    @SuppressWarnings("unchecked")
    public void removeFacet(String nameSolr) {
        for (int x = 0; x < this.listFacets.size(); x++) {
            if (this.listFacets.get(x).getNameSolr().equalsIgnoreCase(nameSolr)) {
                this.listFacets.remove(x);
            }
        }

        // When remove the facet, update also the order of facets
        // For instance: when removing the language
        // 1, subject              // 1, subject
        // 2, language             // 2, publisher
        // 3, publisher     == >
        Integer index = this.getKeyFacet(nameSolr);
        this.facetOrder.remove(index);
        Hashtable newOrderFacets = new Hashtable();
        @SuppressWarnings("unchecked")
        Vector v = new Vector(this.facetOrder.keySet());
        Collections.sort(v);
        Enumeration en = v.elements();
        int cont = 1;
        while(en.hasMoreElements()) {
            // Change the index of the elements after the element being removed
            Integer index1 = (Integer) en.nextElement();
            String f = (String) this.facetOrder.get(index1);
            newOrderFacets.put(cont,f);
            cont++;
            logger.info("NEW VECTOR " + cont + " " + f);
        }
        this.setFacetOrder(newOrderFacets);
    }

    @SuppressWarnings({"unchecked", "unchecked"})
    public void addFacet(Facet f) {
        this.listFacets.add(f);
        // When add a new facet, update also the order of facets
        Vector v = new Vector(this.facetOrder.keySet());
        Collections.sort(v);
        // Put in the last position of the order
        if (v.size() > 0) {
            Integer i = (Integer) v.get(v.size()-1); // get the last index
            this.facetOrder.put(i+1,f.getNameSolr());
        } else {
             this.facetOrder.put(1,f.getNameSolr());
        }
    }

    public void setDescriptionResults(String l) {
        this.descriptionResults = l;
    }

    public void setDescriptionsNavigator(String l) {
        this.descriptionNavigator = l;
    }

    public void setDescriptionTranslations(String l) {
        this.descriptionTranslations = l;
    }

    public int getFirst() {
        return first;
    }

    public int getLast() {
        return last;
    }

    public void setFirst(int first) {
        this.first = first;
    }

    public void setLast(int last) {
        this.last = last;
    }

    public List<Document> getListDocuments() {
        return listDocuments;
    }

    public void setListDocuments(List<Document> listDocuments) {
        this.listDocuments = listDocuments;
    }

    public void setOriginalSearchQuery(String searchQuery) {
        this.originalSearchQuery = searchQuery;
    }

    public String getOriginalSearchQuery() {
        return originalSearchQuery;
    }

    /**
     *
     * @param terms
     */
    public void setSearchTerms(Vector<NLPtoken> terms) {
        searchTerms = terms;
    }

    /**
     *
     * @return
     */
    public Vector<NLPtoken> getSearchTerms() {
        return searchTerms;
    }

    /**
     *
     * @param facetValueDOC
     * @return
     */
    public String highlight(String facetValueDOC) {
        for (int x = 0; x < searchTerms.size(); x++) {
            if (facetValueDOC.toLowerCase().indexOf(searchTerms.get(x).lemma.toLowerCase()) >= 0) {
                facetValueDOC = facetValueDOC.replaceAll(searchTerms.get(x).lemma,
                        "<font color=" + Constant.COLOR_HIGHLIGHT + ">" +
                        searchTerms.get(x).lemma + "</font>");
            }
        }
        return facetValueDOC;
    }

    /**
     *
     * @param facetValueDOC
     * @param str
     * @return
     */
    public String highlight(String facetValueDOC, String str) {
        if (facetValueDOC.toLowerCase().indexOf(str.toLowerCase()) >= 0) {
            facetValueDOC = facetValueDOC.replaceAll(str,
                    "<font color=" + Constant.COLOR_HIGHLIGHT + ">" + str + "</font>");
        }
        return facetValueDOC;
    }

    /**
     *
     * @param freq
     * @param greater
     * @param lower
     * @return
     */
    public String getColor(long freq, long greater, long lower) {
        String color = Constant.COLOR_2;
        if (freq == lower) {
            color = Constant.COLOR_3;
        } else {
            if (freq == greater) {
                color = Constant.COLOR_1;
            }
        }
        return color;
    }

    /**listSearchFacets
     *
     * @param freq
     * @param greater
     * @param lower
     * @return
     */
    public String getSize(long freq, long greater, long lower) {
        int s = 1;
        if (freq == lower) {
            s = Constant.LOWER_FONT_SIZE;
        } else {
            if (freq == Constant.GREATER_FONT_SIZE) {
                s = Constant.GREATER_FONT_SIZE;
            } else {
                s = (int) ((freq * Constant.GREATER_FONT_SIZE) / greater);
            }
        }
        return String.valueOf(s);
    }

    /**
     *
     * @return
     */
    public long getContRecords() {
        return contRecords;
    }

    /**
     *
     * @param l
     */
    public void setContRecords(long l) {
        this.contRecords = l;
    }

    @SuppressWarnings("unchecked")
    public void setListFacets(List<Facet> listFacets) {
        this.listFacets = listFacets;
    }

    @SuppressWarnings("unchecked")
    public List<Facet> getListFacets() {
        /* How this method is called when the user change the order of facets,
         * the order must be done when the application get the list of facets
         * We can not put this method in the method that gets the facets from the WS because
         * it is not the unique case where the facets are manipulated
         */

        /* Put in the order */
        List<Facet> listFacetsOrder = new ArrayList<Facet>();
        // Sort hashtable.
        @SuppressWarnings("unchecked")
        Vector v = new Vector(this.facetOrder.keySet());
        Collections.sort(v);
        Enumeration en = v.elements();
        while (en.hasMoreElements()) {
            int key = ((Integer) en.nextElement()).intValue();
            String facetOrder = (String) this.facetOrder.get(key);
            boolean found = false;
            int x = 0;
            while (x < listFacets.size() && !found) {
                if (facetOrder.equalsIgnoreCase(listFacets.get(x).getNameSolr())) {
                    listFacetsOrder.add(listFacets.get(x));
                    found = true;
                }
                x++;
            }
        }
        return listFacetsOrder;
    }

    public void setSearchQuery(String searchQuery) {
        this.searchQuery = searchQuery;
    }

    public String getSearchQuery() {
        return searchQuery;
    }

    public Document getDoc(long identifier) {
        Document doc = null;
        for (int x = 0; x < listDocuments.size(); x++) {
            if (listDocuments.get(x).getDocID() == identifier) {
                doc = listDocuments.get(x);
            }
        }
        return doc;
    }

    public Facet getFacet(int identifier) {
        Facet f = null;
        for (int x = 0; x < listFacets.size(); x++) {
            if (listFacets.get(x).getId() == identifier) {
                f = listFacets.get(x);
            }
        }
        return f;
    }

    @SuppressWarnings("unchecked")
    public List<String> getVectorFacet(String nameSolr) {
        List<String> list = new ArrayList<String>();
        if (listSearchFacets.containsKey(nameSolr)) {
            list = (List<String>) listSearchFacets.get(nameSolr);
        }
        return list;
    }

    @SuppressWarnings("unchecked")
    public void setNewItemListSearchFacets(String nameSolr) {
        listSearchFacets.put(nameSolr, new ArrayList<String>());
    }

    @SuppressWarnings("unchecked")
    public void add(String nameSolr, String value) {
        List<String> list = new ArrayList<String>();
        if (listSearchFacets.containsKey(nameSolr)) {
            list = (List<String>) listSearchFacets.get(nameSolr);
            list.add(value);
        }
    }

    @SuppressWarnings("unchecked")
    public void remove(String nameSolr, String value) {
        List<String> list = new ArrayList<String>();
        if (listSearchFacets.containsKey(nameSolr)) {
            list = (List<String>) listSearchFacets.get(nameSolr);
            list.remove(value);
        }
    }

    @SuppressWarnings("unchecked")
    public boolean exists(String nameSolr, String value) {
        boolean in = false;
        if (listSearchFacets.containsKey(nameSolr)) {
            List list = (List<String>) listSearchFacets.get(nameSolr);
            for (int x = 0; x < list.size(); x++) {
                if (list.get(x).equals(value)) {
                    in = true;
                }
            }
        }
        return in;
    }

    public Hashtable getListSearchFacets() {
        return listSearchFacets;
    }

    public void setTagCloud(List<Tag> tagCloud) {
        this.tagCloud = tagCloud;
    }

    public List<Tag> getTagCloud() {
        return tagCloud;
    }

    public Tag getTag(long id) {
        Tag f = null;
        for (int x = 0; x < this.tagCloud.size(); x++) {
            if (tagCloud.get(x).getId() == id) {
                f = tagCloud.get(x);
            }
        }
        return f;
    }

    public long getLower() {
        return lower;
    }

    public long getGreater() {
        return greater;
    }

    public void setLower(long l) {
        this.lower = l;
    }

    public void setGreater(long l) {
        this.greater = l;
    }

    public Hashtable getFacetOrder() {
        return this.facetOrder;
    }

    public void setFacetOrder(Hashtable h) {
        this.facetOrder = h;
    }

    public Integer getKeyFacet(String facet) {
        Enumeration en = this.facetOrder.keys();
        Integer keyFacet = null;
        while (en.hasMoreElements()) {
            Integer key = (Integer) en.nextElement();
            String fName = (String) this.facetOrder.get(key);
            if (facet.equalsIgnoreCase(fName)) {
                keyFacet = key;
            }
        }
        return keyFacet;
    }

    public List<Facet> getListOriginalFacets() {
        return this.listOriginalFacets;
    }

    public void setListOriginalFacets(List<Facet> list) {
        this.listOriginalFacets = list;
    }

    public List<Facet> getAvailableFacetToAdd() {
        /* The facets available to add (list to be presented to the user to insert new facets) */
        List<Facet> listAvailable = new ArrayList<Facet>();
        //logger.info("SIZE ORIGINAL " + this.listOriginalFacets.size());
        for (int x = 0; x < this.listOriginalFacets.size(); x++) {
            boolean already = false;
            for (int y = 0; y < this.listFacets.size(); y++) {
                if (this.listOriginalFacets.get(x).getNameSolr().equals(
                        this.listFacets.get(y).getNameSolr())) {
                    already = true;
                }
            }
            if (!already) {
                listAvailable.add(this.listOriginalFacets.get(x));
                //logger.info("ADD ---------------------------" + this.listOriginalFacets.get(x).getNameSolr());
            }
        }
        return listAvailable;
    }

    public Facet getFacetAvailableAdd(int id) {
        Facet f = null;
        for (int x = 0; x < listOriginalFacets.size(); x++) {
            if (listOriginalFacets.get(x).getId() == id) {
                f = listOriginalFacets.get(x);
            }
        }
        return f;
    }


    public void setDocumentBookmark(Document doc) {
        this.documentBookmark = doc;
    }

    public Document getDocumentBookmark() {
        return this.documentBookmark;
    }

    public List<Facet> getListTranslations() {
        return this.listTranslations;
    }

    public void setListTranslations(List<Facet> listTranslations) {
        this.listTranslations = listTranslations;
    }

  
    public Facet getListTranslations(int id) {
        Facet f = null;
        for (int x = 0; x < listTranslations.size(); x++) {
            if (listTranslations.get(x).getId() == id) {
                f = listTranslations.get(x);
            }
        }
        return f;
    }

    public void addNewTranslation(String lang, String value) {
         // Add to the list of current translations the new one
         FacetValue newFv = new FacetValue(value);
         for (int x = 0; x < this.listTranslations.size(); x++) {
              Facet f = this.listTranslations.get(x);
              List<FacetValue> fv = f.getValues(f.getSizeListOriginal());
              if (f.getNameSolr().equals(lang)) {
                newFv.setFacet(f);
                fv.add(newFv);
                logger.info("add facet");
              }
              f.setValues(fv);
              this.listTranslations.set(x,f);
         }
   }

   
}
