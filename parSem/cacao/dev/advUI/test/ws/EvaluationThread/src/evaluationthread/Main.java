/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package evaluationthread;

import eu.project.Cacao.WS_Clients.CACAO_queryWS_client;
import java.io.UnsupportedEncodingException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import org.apache.log4j.Logger;

/**
 *
 * @author ctrojahn
 */
public class Main {

    private final Logger logger = Logger.getLogger(Main.class.getName());

    /**
     * @param args the command line arguments
     * @throws UnsupportedEncodingException 
     */
    public static void main(String[] args) throws UnsupportedEncodingException {

        long timeTot = System.currentTimeMillis();
        long time = System.currentTimeMillis();
        if (args.length != 1) {
            System.out.println("give the number of thread as argument");
            System.exit(-1);
        }
        try {
            CACAO_queryWS_client client = new CACAO_queryWS_client("http://213.215.252.8:8080/axis2/services/CACAO_queryWS");
            String msg = client.processQueryAndSearchCustom("bank", "en", new String[]{"en", "de"},
                    false, false, false, false, Constant.FACETS, 0, 100);
            client.parseResultMSG(new String(msg.getBytes(), "UTF-8"));
            time = System.currentTimeMillis() - time;
            System.out.println("Time first query WS: " + time);
            time = System.currentTimeMillis();
            long totRetrieved = client.getRecordsFound();
            int numThread = (int) (totRetrieved / Constant.MAX_DOCS_SEARCH);
            String searchQuery = client.getSearchQuery();
            int numThreadParallel = Integer.parseInt(args[0]);
            System.out.println("Number threads in parallel: " + numThreadParallel);
            ExecutorService pool = Executors.newFixedThreadPool(numThreadParallel);
            int start = 0;
            int max = Constant.MAX_DOCS_SEARCH;
            for (int i = 0; i < (numThread + 1); i++) {
                start = max + 1;
                max = max + Constant.MAX_DOCS_SEARCH;
                pool.submit(new QueryWSThread(searchQuery, start));
            }
            time = System.currentTimeMillis() - time;
            System.out.println("Time of thread creations: " + time);
            pool.shutdown();
            while (!pool.isTerminated()) {
            }
            timeTot = System.currentTimeMillis() - timeTot;
            System.out.println("Total (ms): " + timeTot);
        } catch (Exception ex) {
            System.out.println(ex.toString());
            ex.printStackTrace();
        }
    }
}


