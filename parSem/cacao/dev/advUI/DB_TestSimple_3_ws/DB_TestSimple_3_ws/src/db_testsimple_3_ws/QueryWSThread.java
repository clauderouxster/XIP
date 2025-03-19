/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package db_testsimple_3_ws;

import eu.project.Cacao.WS_Clients.CACAO_searchWS_client;
import java.lang.reflect.InvocationTargetException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.persistence.EntityManager;
/**
 *
 * @author ctrojahn
 */
public class QueryWSThread implements Runnable {

       private String searchQuery;
       private int start;
       private ManagerDB manager;

       /**
        *
        * @param start
        * @param manager
        * @param searchQuery
        */
       public QueryWSThread(String searchQuery,int start, ManagerDB manager){
              this.start = start;
              this.manager = manager;
              this.searchQuery = searchQuery;
       }
     
       @Override
       public void run() {
//              long time = System.currentTimeMillis();
//              CACAO_searchWS_client client = null;
//              try {
//                    client = new CACAO_searchWS_client("http://213.215.252.8:8080/axis2/services/CACAO_SearchWS");
//                    String msg = client.searchRecords(searchQuery,Constant.FACETS,start,Constant.MAX_DOCS_SEARCH);
//                    client.parseResultMSG(new String(msg.getBytes(), "UTF-8"));
//              } catch (Exception ex) {
//                   System.out.println(ex);
//                   client = null;
//              }
//              time = System.currentTimeMillis() - time;
//              System.out.println("Time of ws execution: " + time);
//
//            if (client!= null) {
//            try {
//                 time = System.currentTimeMillis();
//                 EntityManager em = manager.getEntityManagerFactory().createEntityManager();
//                 //Main.persistDocs(client.getRecords(),em);
//                 Main.persistDocs_withHash(client.getRecords());
//                 em.close();
//                 time = System.currentTimeMillis() - time;
//                 System.out.println("Time to persist each " + Constant.MAX_DOCS_SEARCH + " docs: " + time);
//
//            } catch (ClassNotFoundException ex) {
//                Logger.getLogger(QueryWSThread.class.getName()).log(Level.SEVERE, null, ex);
//                System.out.print(ex);
//            } catch (NoSuchMethodException ex) {
//                Logger.getLogger(QueryWSThread.class.getName()).log(Level.SEVERE, null, ex);
//                System.out.print(ex);
//            } catch (IllegalAccessException ex) {
//                Logger.getLogger(QueryWSThread.class.getName()).log(Level.SEVERE, null, ex);
//                   System.out.print(ex);
//            } catch (IllegalArgumentException ex) {
//                Logger.getLogger(QueryWSThread.class.getName()).log(Level.SEVERE, null, ex);
//                   System.out.print(ex);
//            } catch (InvocationTargetException ex) {
//                Logger.getLogger(QueryWSThread.class.getName()).log(Level.SEVERE, null, ex);
//                   System.out.print(ex);
//            }
//              }
       }
}
