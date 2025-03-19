/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package db_testsimple;

import java.util.Properties;

/**
 *
 * @author ctrojahn
 */
public class Main {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
       
           ManagerDB manager = new ManagerDB(new Properties(),Constant.DROPANDCREATETABLES);

           Facet facet1 = new Facet("language", "language");
           FacetValue facetValue1 = new FacetValue("en");
           facet1.addFacetValue(facetValue1);
           facetValue1.setFacet(facet1);

           manager.persistFacet(facet1);

           Document doc = new Document("Title of Bank");
          

           facetValue1.addDocument(doc);
           doc.addFacetValue(facetValue1);
           manager.persistDoc(doc);

           
   

                 

    }

}
