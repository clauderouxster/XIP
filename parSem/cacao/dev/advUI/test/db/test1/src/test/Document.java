/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package test;

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
    @NamedQuery(name = "getListDocs",
    query = "SELECT DISTINCT doc FROM Document doc JOIN doc.facetValues facetsValues JOIN facetsValues.facet facet"),
    @NamedQuery(name = "getNumberDocuments",
    query = "SELECT count(doc) FROM Document doc"),
    @NamedQuery(name = "getDocument",
    query = "SELECT doc FROM Document doc WHERE doc.documentID= :id")
})
public class Document implements Serializable {

    @Transient
    private static final long serialVersionUID = 1L;
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    private long documentID;

    /* Many to many with documents */
   // @ManyToMany(cascade = {CascadeType.PERSIST}) ?????????????????????????????
    @ManyToMany()
    @JoinTable(name = "Document_FacetValue",
    joinColumns = @JoinColumn(name = "docID", nullable = true),
    inverseJoinColumns = @JoinColumn(name = "facet_valueID", nullable = true))
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
