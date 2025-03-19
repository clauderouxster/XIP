/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package db_testsimple_3_ws;

import java.io.Serializable;
import java.util.HashSet;
import java.util.Set;
import javax.persistence.*;

/**
 * @author ctrojahn
 */

@Entity
@Table(name = "Document")
@NamedQueries({
      //@NamedQuery(name="getListDocs", query ="SELECT DISTINCT doc FROM Document doc JOIN doc.facetValues facetsValues JOIN facetsValues.facet facet"),

      @NamedQuery(name="getListDocs", query =
      "SELECT DISTINCT doc " +
      "FROM Document doc JOIN doc.facetValues list " +
      "WHERE NOT EXISTS (SELECT fv1 FROM doc.facetValues fv1 WHERE fv1.selected= false) " +
      "ORDER BY list.orderFacet"),

      
       @NamedQuery(name="getAllTitles", query =
      "SELECT DISTINCT list.value " +
      "FROM Document doc JOIN doc.facetValues list JOIN list.facet f " +
      "WHERE NOT EXISTS (SELECT fv1 FROM doc.facetValues fv1 WHERE fv1.selected= false) " +
      "AND f.nameSolr = 'title'"),

      @NamedQuery(name="getAllDocs_withTAG", query =
      "SELECT DISTINCT doc " +
      "FROM Document doc JOIN doc.facetValues list JOIN list.facet f " +
      "WHERE NOT EXISTS (SELECT fv1 FROM doc.facetValues fv1 WHERE fv1.selected= false) " +
      "AND f.nameSolr = 'title' AND UPPER(list.value) LIKE :tag"),

      @NamedQuery(name="getNumber_AllDocs_withTAG", query =
      "SELECT DISTINCT count(doc) " +
      "FROM Document doc JOIN doc.facetValues list JOIN list.facet f " +
      "WHERE NOT EXISTS (SELECT fv1 FROM doc.facetValues fv1 WHERE fv1.selected= false) " +
      "AND f.nameSolr = 'title' AND UPPER(list.value) LIKE :tag"),

      @NamedQuery(name="getNumberDocuments", query ="SELECT count(doc) FROM Document doc"),
      @NamedQuery(name="getDocument", query ="SELECT doc FROM Document doc WHERE doc.documentID= :id")
})
public class Document implements Serializable {

       @Transient
       private static final long serialVersionUID = 1L;
       @Id
       @GeneratedValue(strategy = GenerationType.AUTO)
       private long documentID;

       @Column(length=2000)
       private String title;

       @Column
       private String score;

       @Column(length=5000)
       private String description;

       /* Many to many with documents */
       @ManyToMany(cascade={CascadeType.PERSIST})
       @JoinTable(name="Document_FacetValue", joinColumns= @JoinColumn(name="docID",nullable=true),inverseJoinColumns=@JoinColumn(name="facet_valueID",nullable=true))
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

       public void setScore(String score) {
              this.score = score;
       }

       public void setTitle(String title) {
              this.title = title;
       }

       public void setDescription(String description) {
              this.description = description;
       }

       public String getTitle() {
              return title;
       }

       public String getDescription() {
              return description;
       }

       public String getScore() {
              return score;
       }

}
