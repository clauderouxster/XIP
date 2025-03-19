/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package db_testsimple_2;

import java.io.Serializable;
import java.util.HashSet;
import java.util.Set;
import javax.persistence.*;

/**
 * @author ctrojahn
 */

@Entity
@Table(name = "Document")
public class Document implements Serializable {

       @Transient
       private static final long serialVersionUID = 1L;
       @Id
       @GeneratedValue(strategy = GenerationType.AUTO)
       private long documentID;

       /* Many to many with documents */
       @ManyToMany//(cascade={CascadeType.PERSIST})
       @JoinTable(name="Document_FacetValue", joinColumns= @JoinColumn(name="docID"),inverseJoinColumns=@JoinColumn(name="facet_valueID"))
       private Set<FacetValue> facetValues = new HashSet<FacetValue>();

    
       /**
        *
        */
       public Document() {
       }

       /**
        * 
        * @param facetValue
        */
       public void addFacetValue(FacetValue facetValue) {
              this.facetValues.add(facetValue);
       }

       public Set<FacetValue> getFacetValues() {
              return facetValues;
       }

}
