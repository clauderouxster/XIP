package test;

import java.util.Hashtable;
import java.util.Properties;

public class Main {

    public static ManagerDB manager;
    private int nbFacet;
    private Hashtable facets;

    public static void main(String[] args) throws Exception {
        manager = new ManagerDB(new Properties(), Constant.DROPANDCREATETABLES);
        Main main = new Main();
        main.facets = new Hashtable();
        main.test1();
    }

    @SuppressWarnings("unchecked")
    Facet createFacet(String name) {
        Facet facet = new Facet();
        facet.setNameSolr(name);
        facet.setNameUI(name);
        manager.persistFacet(facet);
        facets.put(name, facet);
        return facet;
    }

    FacetValue createFacetValue(String value, Document doc, Facet facet) {
        FacetValue facetValue = new FacetValue(value);
        facetValue.setFacet(facet);   
        manager.persistFacetValue(facetValue);
        return facetValue;
    }

    Document createDoc() {
        Document doc = new Document();
        long timeTot = System.currentTimeMillis();
        for (int cnt = 0; cnt < nbFacet; cnt++) {
            String value = "ValueDeLaFacetValueDeLaFacetValueDeLaFacetValueDeLaFacet" + cnt;
            Facet facet = (Facet) facets.get("facetName" + cnt);
            FacetValue facetValue = createFacetValue(value, doc, facet);
            doc.addFacetValue(facetValue);
            facetValue.addDocument(doc);
        }
        //System.out.println("time to create " + nbFacet + " Facetvalue : " + (System.currentTimeMillis() - timeTot));
        manager.persistDoc(doc);
        return doc;
    }

    public void test1() throws Exception {
        nbFacet = 100000;
        long timeTot = System.currentTimeMillis();
        for (int cnt = 0; cnt < nbFacet; cnt++) {
            Facet facet = createFacet("facetNameaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaasssssssssssssssssssssssssssss" + cnt);
        }
        System.out.println("total time to create " + nbFacet + " Facet : " + (System.currentTimeMillis() - timeTot));

//        timeTot = System.currentTimeMillis();
//        int nbDoc = 5000;
//        for (int cnt = 0; cnt < nbDoc; cnt++) {
//            Document doc = createDoc();
//        }
//        System.out.println("total time to create " + nbDoc + " Document : " + (System.currentTimeMillis() - timeTot));
    }
} 