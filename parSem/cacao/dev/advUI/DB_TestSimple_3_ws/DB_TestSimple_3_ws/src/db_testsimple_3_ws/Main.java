/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package db_testsimple_3_ws;

import eu.project.Cacao.Components.Utils.DocsSearchResult;
import eu.project.Cacao.WS_Clients.CACAO_queryWS_client;
import eu.project.Cacao.WS_Clients.CACAO_searchWS_client;
import java.io.UnsupportedEncodingException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Properties;
import java.util.Vector;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import javax.persistence.EntityManager;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;

/**
 *
 * @author ctrojahn
 */
public class Main {


    /**
     * All search are done in the database directly
     */
    public static ManagerDB manager = new ManagerDB(new Properties(),Constant.DROPANDCREATETABLES);
   

    /**
     * @param args the command line arguments
     * @throws NoSuchMethodException
     * @throws IllegalAccessException
     * @throws IllegalArgumentException
     * @throws InvocationTargetException 
     * @throws ClassNotFoundException
     * @throws UnsupportedEncodingException
     */

    @SuppressWarnings({"unchecked", "unchecked","unchecked"})
    public static void main(String[] args) throws NoSuchMethodException, IllegalAccessException, IllegalArgumentException, InvocationTargetException, ClassNotFoundException, UnsupportedEncodingException {
           Constant.initializeMapping();
           long timeTot = System.currentTimeMillis();
           long time = System.currentTimeMillis();

           CACAO_queryWS_client client = new CACAO_queryWS_client("http://213.215.252.8:8080/axis2/services/CACAO_queryWS");
           String msg = client.processQueryAndSearchCustom("bank", "en",new String[]{"en","de"},false,false,false,false,
                                                                       Constant.FACETS,0,1);
           client.parseResultMSG(new String(msg.getBytes(), "UTF-8"));
           long totRetrieved = client.getRecordsFound();
           System.out.println("TOT RETRIEVED: " + totRetrieved);

           time = System.currentTimeMillis() - time;
           System.out.println("Time first query WS: " + time);
           time = System.currentTimeMillis();

           // Persist all fields (that are considered facets)
           Enumeration e = Constant.MAPPING.elements();
           while(e.hasMoreElements()) {
                 String facet = (String)e.nextElement();
                 // Title is a field but is not needed to be a facet
                 System.out.println("FACET: " + facet);
                 if (!facet.equalsIgnoreCase("title"))
                 {
                    List listFacetValue = new ArrayList(); // new
                    Facet f = new Facet(facet);
                    manager.persistFacet(f);
                    boolean is = false;
                    for(int y=0;y<client.getFacets().size();y++) {
                         if(facet.equalsIgnoreCase(client.getFacets().get(y).getFacetField())) {
                               FacetValue  fv = new FacetValue(client.getFacets().get(y).getFacetValue());
                               fv.setFacet(f);
                               manager.persistFacetValue(fv);
                               is = true;
                         }
                     }
                    if (!is) {
                       FacetValue  fv = new FacetValue("any");
                       fv.setFacet(f);
                       manager.persistFacetValue(fv);
                    }
                  }
           }
           time = System.currentTimeMillis() - time;
           System.out.println("Time persist facets: " + time);

           /*
           time = System.currentTimeMillis();
           EntityManager em = manager.getEntityManagerFactory().createEntityManager();
           persistDocs(client.getRecords());
           em.close();
           time = System.currentTimeMillis() - time;
           System.out.println("Time to persist each " + Constant.MAX_DOCS_SEARCH + " docs: " + time);
           String searchQuery = client.getSearchQuery();
     /

           // Test ---------------------------------------------------------------------------------------------
           
              time = System.currentTimeMillis();
              CACAO_searchWS_client client1 = null;
              try {
                                                        // http://demo.celi.it:8095/axis2/services/CACAO_searchWS
                    client1 = new CACAO_searchWS_client("http://213.215.252.8:8080/axis2/services/CACAO_searchWS_2");
                    String msg1 =  client1.searchRecords(searchQuery,null,2,(int)totRetrieved);
                    client1.parseResultMSG(new String(msg1.getBytes(), "UTF-8"));
              } catch (Exception ex) {
                   System.out.println("Erro: " + ex);
                   client1 = null;
              }
              time = System.currentTimeMillis() - time;
              System.out.println("Time of ws execution: " + time);

            if (client1!= null) {
            try {
                // time = System.currentTimeMillis();
     //            EntityManager em1 = manager.getEntityManagerFactory().createEntityManager();
                 System.out.println("TOT retrieved (query 2): " + client1.getRecords().size());
                 persistDocs(client1.getRecords());
                 //Main.persistDocs_withHash(client1.getRecords());
       //          em1.managerclose();
               //  time = System.currentTimeMillis() - time;
                /// System.out.println("Time to persist each " + Constant.MAX_DOCS_SEARCH + " docs: " + time);

            } catch (Exception ex) {
               // Logger.getLogger(QueryWSThread.class.getName()).log(Level.SEVERE, null, ex);
                System.out.print("Erro 2(a): " + ex);
            
              }

       
            List titles = manager.getAllTitles();
            System.out.println("Titles: " + titles.size());
            */

    
    }

    public static void persistDocs(Vector<DocsSearchResult> docs) throws ClassNotFoundException, NoSuchMethodException, IllegalAccessException, IllegalArgumentException, InvocationTargetException
    {

          EntityManager em = manager.getEntityManagerFactory().createEntityManager();
          try {
          // Persist docs
          for(int x=0;x<docs.size();x++) {
              Document doc = new Document();
              //String descriptionDoc = "";
              List<FacetValue> listFacetsDoc = new ArrayList<FacetValue>();
              Enumeration e = Constant.MAPPING.keys();
              while (e.hasMoreElements()) {
                     long time = System.currentTimeMillis();
                     String key = (String) e.nextElement();
                     Class cl = Class.forName("eu.project.Cacao.Components.Utils.DocsSearchResult");
                     Method mthd = cl.getMethod("get" + key);
                     boolean isString = false;
                     HashSet hash = null;
                     String value = null;
                     /* Field can be a single String or a HashSet */
                     //descriptionDoc = descriptionDoc + "<p></b>" + key + "</b>: ";
                     if ((mthd.invoke(docs.get(x))) instanceof HashSet) {
                         hash = (HashSet) mthd.invoke(docs.get(x));
                         if (hash == null) {
                             hash = new HashSet();
                         }
                         if(hash.size() == 0) {
                            hash.add(new String("any"));
                         }
                         //descriptionDoc = descriptionDoc + hash.toString() + "</p>";
                     }
                     else {
                         isString = true;
                         value = (String) mthd.invoke(docs.get(x));
                         if (value == null) {
                             value = "any";
                         }

                         //descriptionDoc = descriptionDoc + value + "</p>";
                         // Duplicated fields
                         //if(key.equalsIgnoreCase("title")) {
                         //   doc.setTitle(value);
                        // }
                         //if (key.equalsIgnoreCase("score")) {
                         //    doc.setScore(value);
                        // }
                     }

                     // If key == title, it do not need to put as facet
                     //if (!key.equalsIgnoreCase("title")) {

                         if (Constant.FACETS_TEMP.containsKey(key)) {
                            String nameFacet = (String) Constant.FACETS_TEMP.get(key);
                            if (isString) {
                                 FacetValue fvDoc = manager.getFacetValue(nameFacet,value,em);
                                listFacetsDoc.add(fvDoc);
                             }
                            else {
                              listFacetsDoc.addAll(manager.getListFacetValues(nameFacet,hash,em));
                             }
                         }
                        else {
                          String nameFacet = (String) Constant.MAPPING.get(key);
                          Facet f = manager.getFacet(nameFacet,em);
                          if (isString) {
                              FacetValue fvDoc = new FacetValue(value);
                              fvDoc.setFacet(f);
                              manager.persistFacetValue(fvDoc);
                              listFacetsDoc.add(fvDoc);
                          }
                          else {
                               Iterator itr = hash.iterator();
                               while (itr.hasNext()) {
                                       FacetValue fv = new FacetValue((String)itr.next());
                                       fv.setFacet(f);
                                       manager.persistFacetValue(fv);
                                       listFacetsDoc.add(fv);
                               }
                           }
                        }
                     //}
                     // -----------------------------------------------------------------------------------

              } // while fields each doc
              //doc.setDescription(descriptionDoc);
              Iterator itr = listFacetsDoc.iterator();
              while (itr.hasNext()) {
                    FacetValue fvDoc = (FacetValue) itr.next();
                    doc.addFacetValue(fvDoc);
                    fvDoc.addDocument(doc);
              }
              manager.persistDoc(doc,em);
        } // for each doc
          } catch(Exception e){
              System.out.println("ERRO PERSIST: " + e);
          }

        em.close();
    }  // method












    // =========================================================================================================================
    /**
     *
     * @param docs
     * @param em
     * @throws ClassNotFoundException
     * @throws NoSuchMethodException
     * @throws IllegalAccessException
     * @throws IllegalArgumentException
     * @throws InvocationTargetException
     */
    /*
    @SuppressWarnings({"unchecked", "unchecked","unchecked","unchecked"})
    public static void persistDocs(Vector<DocsSearchResult> docs, EntityManager em) throws ClassNotFoundException, NoSuchMethodException, IllegalAccessException, IllegalArgumentException, InvocationTargetException
    {
          EntityManager em1 = manager.getEntityManagerFactory().createEntityManager();
          em1.getTransaction().begin();
          // Persist docs
          for(int x=0;x<docs.size();x++) {
              Document doc = new Document();
              List<FacetValue> listFacetsDoc = new ArrayList<FacetValue>();
              Enumeration e = Constant.MAPPING.keys();
              while (e.hasMoreElements()) {
                     long time = System.currentTimeMillis();
                     String key = (String) e.nextElement();
                     Class cl = Class.forName("eu.project.Cacao.Components.Utils.DocsSearchResult");
                     Method mthd = cl.getMethod("get" + key);
                     boolean isString = false;
                     HashSet hash = null;
                     String value = null;
                     /* Field can be a single String or a HashSet 

                     if ((mthd.invoke(docs.get(x))) instanceof HashSet) {
                         hash = (HashSet) mthd.invoke(docs.get(x));
                         if (hash == null) {
                             hash = new HashSet();
                         }
                         if(hash.size() == 0) {
                            hash.add(new String("any"));
                         }
                     }
                     else {
                         isString = true;
                         value = (String) mthd.invoke(docs.get(x));
                         if (value == null) {
                             value = "any";
                         }
                     }
                     time = System.currentTimeMillis() - time;
                     //System.out.println("Time method invoke: " + time);

                     // Code that can be optimized ---------------------------------------------------
                     if (Constant.FACETS_TEMP.containsKey(key)) {
                         String nameFacet = (String) Constant.FACETS_TEMP.get(key);
                         if (isString) {
                             FacetValue fvDoc = manager.getFacetValue(nameFacet,value,em);
                             listFacetsDoc.add(fvDoc);
                         }
                         else {
                             listFacetsDoc.addAll(manager.getListFacetValues(nameFacet,hash,em));
                          }
                     }
                     else {
                         String nameFacet = (String) Constant.MAPPING.get(key);
                         Facet f = manager.getFacet(nameFacet,em);
                         if (isString) {
                             FacetValue fvDoc = new FacetValue(value);
                             fvDoc.setFacet(f);
                             manager.persistFacetValue(fvDoc);
                             listFacetsDoc.add(fvDoc);
                         }
                         else {
                              Iterator itr = hash.iterator();
                              while (itr.hasNext()) {
                                      FacetValue fv = new FacetValue((String)itr.next());
                                      fv.setFacet(f);
                                      manager.persistFacetValue(fv);
                                      listFacetsDoc.add(fv);
                              }
                          }
                     }
                     // -----------------------------------------------------------------------------------

              } // while fields each doc

              Iterator itr = listFacetsDoc.iterator();
              while (itr.hasNext()) {
                    FacetValue fvDoc = (FacetValue) itr.next();
                    doc.addFacetValue(fvDoc);
                    fvDoc.addDocument(doc);
              }
              manager.persistDoc(doc,em1);
          } // for each doc
          em1.getTransaction().commit();
          em1.close();

    }  // method
//
//
//
//
//    // =========================================================================================================================
//
//
//    /**
//     *
//     * @param docs
//     * @throws ClassNotFoundException
//     * @throws NoSuchMethodException
//     * @throws IllegalAccessException
//     * @throws IllegalArgumentException
//     * @throws InvocationTargetException
//     */
//
//    @SuppressWarnings({"unchecked", "unchecked","unchecked","unchecked"})
//    public static void persistDocs_withHash(Vector<DocsSearchResult> docs) throws ClassNotFoundException, NoSuchMethodException, IllegalAccessException, IllegalArgumentException, InvocationTargetException
//    {
//           EntityManager em1 = manager.getEntityManagerFactory().createEntityManager();
//           em1.getTransaction().begin();
//
//          // Persist docs
//          for(int x=0;x<docs.size();x++) {
//              Document doc = new Document();
//              List<FacetValue> listFacetsDoc = new ArrayList<FacetValue>();
//              Enumeration e = Constant.MAPPING.keys();
//              while (e.hasMoreElements()) {
//                     long time = System.currentTimeMillis();
//                     String key = (String) e.nextElement();
//                     Class cl = Class.forName("eu.project.Cacao.Components.Utils.DocsSearchResult");
//                     Method mthd = cl.getMethod("get" + key);
//                     boolean isString = false;
//                     HashSet hash = null;
//                     String value = null;
//                     /* Field can be a single String or a HashSet */
//
//                     if ((mthd.invoke(docs.get(x))) instanceof HashSet) {
//                         hash = (HashSet) mthd.invoke(docs.get(x));
//                         if (hash == null) {
//                             hash = new HashSet();
//                         }
//                         if(hash.size() == 0) {
//                            hash.add(new String("any"));
//                         }
//                     }
//                     else {
//                         isString = true;
//                         value = (String) mthd.invoke(docs.get(x));
//                         if (value == null) {
//                             value = "any";
//                         }
//                     }
//                     time = System.currentTimeMillis() - time;
//                     //System.out.println("Time method invoke: " + time);
//
//                     // Code that can be optimized ---------------e------------------------------------
//                     if (Constant.FACETS_TEMP.containsKey(key)) {
//                         String nameFacet = (String) Constant.FACETS_TEMP.get(key);
//                         if (isString) {
//                             //FacetValue fvDoc = manager.getFacetValue(nameFacet,value);
//                             FacetValue fvDoc = (FacetValue)(((ArrayList)listFacetsAndFacetsValues.get(nameFacet)).get(0));
//                             listFacetsDoc.add(fvDoc);
//                         }
//                         else {
//                             //listFacetsDoc.addAll(manager.getListFacetValues(nameFacet,hash));
//                             ArrayList l = (ArrayList)listFacetsAndFacetsValues.get(nameFacet);
//                             for(int a=0;a<l.size();a++) {
//                                 Iterator i = hash.iterator();
//                                 while (i.hasNext()) {
//                                     FacetValue fvDoc = (FacetValue)l.get(a);
//                                     if (fvDoc.getValue().equalsIgnoreCase((String)i.next())) {
//                                          listFacetsDoc.add(fvDoc);
//                                     }
//                                 }
//                             }
//                          }
//
//                     }
//                     else {
//                         String nameFacet = (String) Constant.MAPPING.get(key);
//                         //Facet f = manager.getFacet(nameFacet);
//                         Facet f = (Facet)listFacets.get(nameFacet);
//                         if (isString) {
//                             FacetValue fvDoc = new FacetValue(value);
//                             fvDoc.setFacet(f);
//                             manager.persistFacetValue(fvDoc);
//                             listFacetsDoc.add(fvDoc);
//                         }
//                         else {
//                              Iterator itr = hash.iterator();
//                              while (itr.hasNext()) {
//                                      FacetValue fv = new FacetValue((String)itr.next());
//                                      fv.setFacet(f);
//                                      manager.persistFacetValue(fv);
//                                      listFacetsDoc.add(fv);
//                              }
//                          }
//                     }
//                     // -----------------------------------------------------------------------------------
//              } // while fields each doc
//
//              Iterator itr = listFacetsDoc.iterator();
//              while (itr.hasNext()) {
//                    FacetValue fvDoc = (FacetValue) itr.next();
//                    doc.addFacetValue(fvDoc);
//                    fvDoc.addDocument(doc);
//              }
//              manager.persistDoc(doc,em1);
//
//          } // for each doc
//          em1.getTransaction().commit();
//          em1.close();
//    }  // method
//

} // main