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
 *
 * @author ctrojahn
 */
@Entity
@Table(name = "FacetValue")
@NamedQueries({
    @NamedQuery(name = "getFacetValue",
    query = "SELECT fv FROM FacetValue fv JOIN fv.facet f where fv.value = :v AND f.nameSolr = :f"),
    @NamedQuery(name = "getFacetValueDOC",
    query = "SELECT fv.value FROM FacetValue fv JOIN fv.facet f JOIN fv.documents doc WHERE f.nameSolr = :v AND doc.documentID = :id"),
    @NamedQuery(name = "getValues",
    query = "SELECT fv FROM FacetValue fv JOIN fv.facet f WHERE f.nameSolr = :v")
})
public class FacetValue implements Serializable {

    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    private long valueID;

    /* N - 1 relation with Facet */
    @ManyToOne(fetch = FetchType.LAZY)
    @JoinColumn(name = "facetID", nullable = false)
    private Facet facet;
    @Column(length = 2000)
    private String value;
    @Transient
    private boolean selected = true;

    /* Many to Many with documents */
    @ManyToMany(mappedBy = "facetValues")//,cascade={CascadeType.PERSIST}) //name of the corresponding field in documents
    private Set<Document> documents = new HashSet<Document>();

    /**
     *
     */
    public FacetValue() {
    }

    /**
     * @param facetID
     * @param value
     */
    public FacetValue(String value) {
        this.value = value;
    }

    /**
     * @return
     */
    public String getValue() {
        return value;
    }

    /**
     *
     * @param value
     */
    public void setValue(String value) {
        this.value = value;
    }

    /**
     *
     * @return
     */
    public boolean getSelected() {
        return this.selected;
    }

    /**
     * @param selected
     */
    public void setSelected(boolean selected) {
        this.selected = selected;
    }

    @Override
    public String toString() {
        return facet.getNameUI() + " " + value;
    }

    public void setFacet(Facet facet) {
        this.facet = facet;
    }

    public void addDocument(Document doc) {
        this.documents.add(doc);
    }

    public Facet getFacet() {
        return facet;
    }
}
