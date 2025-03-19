/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package db_testsimple_2;

/**
 *
 * @author ctrojahn
 */
public class FieldFacet {
       String fieldDB;
       String facet;

       public FieldFacet(String fieldDB,String facet) {
              this.facet = facet;
              this.fieldDB = fieldDB;
       }
       
       public String getFacet() {
              return facet;
       }
       
       public String getFieldDB() {
              return fieldDB;
       }
}