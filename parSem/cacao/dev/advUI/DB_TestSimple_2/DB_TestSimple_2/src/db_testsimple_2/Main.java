/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package db_testsimple_2;

import eu.project.Cacao.Components.Utils.DocsSearchResult;
import eu.project.Cacao.Components.Utils.FacetsSearchResult;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Properties;
import java.util.Set;
import java.util.Vector;

/**
 *
 * @author ctrojahn
 */
public class Main {

    /**
     * @param args the command line arguments
     * @throws NoSuchMethodException
     * @throws IllegalAccessException
     * @throws IllegalArgumentException
     * @throws InvocationTargetException 
     * @throws ClassNotFoundException
     */
    @SuppressWarnings({"unchecked", "unchecked","unchecked"})
    public static void main(String[] args) throws NoSuchMethodException, IllegalAccessException, IllegalArgumentException, InvocationTargetException, ClassNotFoundException {

          ManagerDB manager = new ManagerDB(new Properties(),Constant.DROPANDCREATETABLES);

          // From CACAO framework
          FacetsSearchResult facet1_1 = new FacetsSearchResult("language","en",1);
          FacetsSearchResult facet1_2 = new FacetsSearchResult("language","fr",2);

          FacetsSearchResult facet2_1 = new FacetsSearchResult("libraryID","libID",1);
          

          HashSet<String> subjects = new HashSet<String>();
          subjects.add("Subject 1");
          HashSet<String> oaiSets = new HashSet<String>();
          oaiSets.add("OaiSets 1");
          HashSet<String> descriptions = new HashSet<String>();
          descriptions.add("Description 1");
          HashSet<String> tableOfContents = new HashSet<String>();
          tableOfContents.add("Table of Contents 1");
          HashSet<String> contributors = new HashSet<String>();
          contributors.add("Contributors 1");
          HashSet<String> languages = new HashSet<String>();
          languages.add("en");
          languages.add("fr");
          HashSet<String> creators = new HashSet<String>();
          creators.add("Creator 1");
          DocsSearchResult doc1 = new DocsSearchResult("0","libID","Bank Management","Springer","URL",
                                  "ISSN", "ISBN", "TEL-URL", "identifier", "recordID","type",subjects,oaiSets,descriptions,
                                  tableOfContents,contributors,languages,creators);


          Vector<FacetsSearchResult> facets = new Vector<FacetsSearchResult>();
          facets.add(facet1_1);
          facets.add(facet1_2);
          facets.add(facet2_1);
          
          Vector<DocsSearchResult> docs = new Vector<DocsSearchResult>();
          docs.add(doc1);

          String[] FACETS = {"language","libraryID"};

          // Position 0: field Position 1: correponding facet
          Vector<Vector<String>> mapping = new Vector<Vector<String>>();
          Vector<String> v1 = new Vector<String>(); v1.add("Score"); v1.add("score");
          mapping.add(v1);
          Vector<String> v2 = new Vector<String>(); v2.add("LibraryID"); v2.add("libraryID");
          mapping.add(v2);
          Vector<String> v3 = new Vector<String>(); v3.add("Title"); v3.add("title");
          mapping.add(v3);
          Vector<String> v4 = new Vector<String>(); v4.add("Publisher"); v4.add("publisher");
          mapping.add(v4);
          Vector<String> v5 = new Vector<String>(); v5.add("URL"); v5.add("url");
          mapping.add(v5);
          Vector<String> v6 = new Vector<String>(); v6.add("ISSN"); v6.add("issn");
          mapping.add(v6);
          Vector<String> v7 = new Vector<String>(); v7.add("ISBN"); v7.add("isbn");
          mapping.add(v7);
          Vector<String> v8 = new Vector<String>(); v8.add("TelRecordID_URL"); v8.add("relRecordID_URL");
          mapping.add(v8);
          Vector<String> v9 = new Vector<String>(); v9.add("Identifier"); v9.add("identifier");
          mapping.add(v9);
          Vector<String> v10 = new Vector<String>(); v10.add("RecordID"); v10.add("recordID");
          mapping.add(v10);
          Vector<String> v11 = new Vector<String>(); v11.add("Type"); v11.add("type");
          mapping.add(v11);
          Vector<String> v12 = new Vector<String>(); v12.add("Subjects"); v12.add("subjects");
          mapping.add(v12);
          Vector<String> v13 = new Vector<String>(); v13.add("OaiSets"); v13.add("oaiSets");
          mapping.add(v13);
          Vector<String> v14 = new Vector<String>(); v14.add("Descriptions"); v14.add("descriptions");
          mapping.add(v14);
          Vector<String> v15 = new Vector<String>(); v15.add("TableOfContents"); v15.add("tableOfContents");
          mapping.add(v15);
          Vector<String> v16 = new Vector<String>(); v16.add("Contributors"); v16.add("contributors");
          mapping.add(v16);
          Vector<String> v17 = new Vector<String>(); v17.add("Languages"); v17.add("language");
          mapping.add(v17);
          Vector<String> v18 = new Vector<String>(); v18.add("Creators"); v18.add("creator");
          mapping.add(v18);


          // Persist facets
          for(int x=0;x<FACETS.length;x++) {
            Facet f = new Facet(FACETS[x]);
            for(int y=0;y<facets.size();y++) {
                if(FACETS[x].equals(facets.get(y).getFacetField())) {
                  FacetValue fv = new FacetValue(facets.get(y).getFacetValue());
                  fv.setFacet(f);
                  f.addFacetValue(fv);
                }
            }
            manager.persistFacet(f);
          }


          // Persist docs
          for(int x=0;x<docs.size();x++) {
              Document doc = new Document();
              for(int y=0;y<mapping.size();y++) {
                  // Get value of the field
                  Class cl = Class.forName("eu.project.Cacao.Components.Utils.DocsSearchResult");
                  Method mthd = cl.getMethod("get" + mapping.get(y).get(0));
                  boolean isHash = false;
                  boolean isString = false;
                  HashSet hash = null;
                  String value = null;
                  // Field can be a single String or a HashSet
                  if ((mthd.invoke(docs.get(x))) instanceof HashSet) {
                      isHash = true;
                      hash = (HashSet) mthd.invoke(docs.get(x));
                  }
                  else {
                      isString = true;
                      value = (String) mthd.invoke(docs.get(x));
                  }
                  /* If the current field is a facet, retrieve the corresponding values:
                   * one value: if field is a single string
                   * set of values: if the field is a HashSet
                   */
                  if (isFacet(FACETS,mapping.get(y).get(1))) {
                      if (isString) {
                          System.out.println("Looking facet with value= " + value);
                          FacetValue fvDoc = manager.getFacetValue(value);
                          fvDoc.addDocument(doc);
                          doc.addFacetValue(fvDoc);
                      }
                      else {
                          List<FacetValue> fvsDoc = manager.getListFacetValues(hash);
                          System.out.println("SIZE RET: " + fvsDoc.size());
                          for(int z=0;z<fvsDoc.size();z++) {
                              fvsDoc.get(z).addDocument(doc);
                              doc.addFacetValue(fvsDoc.get(z));
                          }
                     }
                  }
                  else {
                       if (isString) {
                          Facet f = new Facet(mapping.get(y).get(1));
                          FacetValue fv = new FacetValue(value);
                          fv.setFacet(f);
                          f.addFacetValue(fv);
                          manager.persistFacet(f);
                          fv.addDocument(doc);
                          doc.addFacetValue(fv);
                      }
                      else {
                           // 1) create and persist facets and facets values
                           // 2) associate facets and documents
                           Facet f = new Facet(mapping.get(y).get(1));
                           Set<FacetValue> temp = new HashSet<FacetValue>();
                           Iterator itr = hash.iterator();
                           while (itr.hasNext()) {
                               FacetValue fv = new FacetValue((String)itr.next());
                               fv.setFacet(f);
                               f.addFacetValue(fv);
                               temp.add(fv);
                           }
                           manager.persistFacet(f);
                           itr = temp.iterator();
                           while(itr.hasNext()) {
                                 FacetValue fv = ((FacetValue)itr.next());
                                 fv.addDocument(doc);
                                 doc.addFacetValue(fv);
                           }
                      }
                  }
              }
              manager.persistDoc(doc);
         }

         for(int x=0;x<manager.getListDocs().size();x++) {
             Document doc = (Document) manager.getListDocs().get(x);
             System.out.println("Document " + x);
             Iterator itr = doc.getFacetValues().iterator();
             while (itr.hasNext()) {
                   FacetValue fv = (FacetValue) itr.next();
                   System.out.println(fv.getFacet().getNameSolr() + " " + fv.getValue());
             }
         }
    }

    public static boolean isFacet(String[] facets, String field) {
           boolean is = false;
           
           for(int x=0;x<facets.length;x++) {
               //System.out.println("Comparing " + field + " with " + facets[x]);
               if (facets[x].equalsIgnoreCase(field)) {
                  is = true;
                 
               }
           }
           return is;
    }
}
