/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package evaluationthread;

import eu.project.Cacao.WS_Clients.CACAO_searchWS_client;
/**
 *
 * @author ctrojahn
 */
public class QueryWSThread implements Runnable {

       private String searchQuery;
       private int start;
       private String msg;
       /**
        *
        * @param start
        * @param searchQuery
        */
       public QueryWSThread(String searchQuery,int start){
              this.start = start;
              this.searchQuery = searchQuery;
       }
     
       @Override
       public void run() {
              long time = System.currentTimeMillis();
              System.out.println("From: "+  + start + "------------------------------------------------------------------");
              CACAO_searchWS_client client = null;
              try {
                    client = new CACAO_searchWS_client("http://213.215.252.8:8080/axis2/services/CACAO_searchWS_2");
                    msg = client.searchRecords(searchQuery,Constant.FACETS,start,Constant.MAX_DOCS_SEARCH);
                    client.parseResultMSG(new String(msg.getBytes(), "UTF-8"));
                    for (int x=0;x<client.getRecords().size();x++) {
                        System.out.println(client.getRecords().get(x).getTitle() + " " + client.getRecords().get(x).getScore());
                    }
              } catch (Exception ex) {
                   System.out.println("Erro: " + ex);
                   System.out.println("Start: " + start);
                   System.out.println("Msg: " + msg);
              }
              time = System.currentTimeMillis() - time;
              System.out.println("Time of ws execution: " + time);
       }
}
