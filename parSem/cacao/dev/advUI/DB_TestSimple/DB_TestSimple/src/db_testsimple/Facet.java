/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package db_testsimple;

import java.io.Serializable;
import java.util.HashSet;
import java.util.Set;
import javax.persistence.*;


/**
 *
 * @author ctrojahn
 */


@Entity
@Table(name = "Facet")
public class Facet implements Serializable
{
       @Id
       @GeneratedValue(strategy = GenerationType.AUTO)
       private long facetID;
       private String nameUI;
       private String nameSolr;

       @OneToMany(cascade={CascadeType.PERSIST},fetch=FetchType.LAZY)
       private Set<FacetValue> facetValues = new HashSet<FacetValue>();
       
       /**
        *
        */
       public Facet() {
       }

       /**
        * @param nameUI
        * @param nameSolr
        */
       public Facet(String nameUI, String nameSolr) {
              this.nameUI = nameUI;
              this.nameSolr = nameSolr;
       }


       /**
        *
        * @return
        */
       public String getNameUI() {
              return this.nameUI;
       }

       /**
        * @param nameUI
        */
       public void setNameUI(String nameUI) {
              this.nameUI = nameUI;
       }

        /**
        * @return
        */
       public String getNameSolr() {
              return this.nameSolr;
       }

       /**
        * @param nameSolr
        */
       public void setNameSolr(String nameSolr) {
              this.nameSolr = nameSolr;
       }

       public void addFacetValue(FacetValue facetValue) {
              facetValues.add(facetValue);
              facetValue.setFacet(this);
       }

}

