/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package db_testsimple;

import eu.project.Cacao.Components.Utils.DocsSearchResult;
import java.io.Serializable;
import java.util.HashSet;
import java.util.Iterator;
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

       @Column(nullable = true)
       private String title;

       @Transient
       private String titleTrunc;

       @Column(nullable = true, length=2000)
       private String url;

       @Column(nullable = true)
       private String issn;

       @Column(nullable = true)
       private String isbn;

       @Column(nullable = true)
       private String telRecordID_URL;

       @Column(nullable = true)
       private String identifier;

       @Column(nullable = true)
       private String recordID;

       @Column(nullable = true, length=2000)
       private String oaiSets;

       @Column(nullable = true, length=2000)
       private String descriptions;

       @Column(nullable = true, length=2000)
       private String tableOfContents;

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
        * @param doc
        */
       public Document(DocsSearchResult doc) {
               
              this.title = doc.getTitle();
		      this.url = doc.getURL();
		      this.issn = doc.getISSN();
		      this.isbn = doc.getISBN();
		      this.telRecordID_URL = doc.getTelRecordID_URL();
		      this.recordID = doc.getRecordID();
		      // oaiSets
              Iterator itr2 = doc.getOaiSets().iterator();
              this.oaiSets = new String();
              while(itr2.hasNext()) {
                    this.oaiSets = this.oaiSets + "#" + ((String)itr2.next());
              }
              
              // Descriptions
              Iterator itr3 = doc.getDescriptions().iterator();
              this.descriptions = new String();
              while(itr3.hasNext()) {
                    this.descriptions = this.descriptions + "#" + ((String)itr3.next());
              }

              // TableOfContents
              Iterator itr4 = doc.getTableOfContents().iterator();
              this.tableOfContents = new String();
              while(itr4.hasNext()) {
                    this.tableOfContents = this.tableOfContents + "#" + ((String)itr4.next());
              }
       }

       public Document(String title) {
              this.title = title;
       }

       /**
        *
        * @return
        */
       public long getId() {
              return documentID;
       }

       /**
        *
        * @return
        */
       public String getIdentifier() {
		      return identifier;
	   }

             /**
        *
        * @return
        */
       public String getTitle()
       {
              return title;
       }

       /**
        *
        * @return
        */
       public String getURL() {
		      return url;
	   }

       /**
        *
        * @return
        */
       public String getRecordID() {
		      return recordID;
	   }

       /**
        *
        * @return
        */
       public String getISSN() {
		      return issn;
	   }

       /**
        *
        * @return
        */
       public String getISBN() {
		      return isbn;
	   }

       /**
        *
        * @return
        */
       public String getTelRecordID_URL() {
		      return telRecordID_URL;
	   }

       /**
        *
        * @return
        */
       public String getOaiSets() {
		      return oaiSets;
       }

       /**
        *
        * @return
        */
       public String getDescriptions() {
		      return descriptions;
	   }

       /**
        *
        * @return
        */
       public String getTableOfContents() {
		      return tableOfContents;
	   }

       

       /**
        *
        * @return
        */
       public String getTitleTrunc() {
              if (title.length() < Constant.TRUNC_TITLE) {
                   return title;
               }
              else  {
                return title.substring(0, Constant.TRUNC_TITLE) + " ...";
              }
       }


       public void addFacetValue(FacetValue facetValue) {
              this.facetValues.add(facetValue);
       }
}
