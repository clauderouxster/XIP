package test;

import eu.project.Cacao.WS_Clients.CACAO_queryWS_client;
import eu.project.Cacao.WS_Clients.CACAO_searchWS_client;

public class Main {

    public static final int MAX_DOCS_SEARCH = 100;
    public static final String[] FACETS = {"language", "libraryID"};
    public static final String rootWsUrl = "http://213.215.252.8:8080/axis2/services";

    public static void main(String[] args) {
        String searchQuery = null;
        boolean y;
        try {
            CACAO_queryWS_client queryWS_client = new CACAO_queryWS_client(rootWsUrl + "/CACAO_queryWS");
            String queryWS_res = queryWS_client.processQueryAndSearchCustom("bank", "en", new String[]{"en", "de"},
                    false, false, false, false, FACETS, 0, 5);
            boolean parseRes1 = queryWS_client.parseResultMSG(new String(queryWS_res.getBytes(), "UTF-8"));
            long totRetrieved = queryWS_client.getRecordsFound();
            System.out.println("totRetrieved = " + totRetrieved);

             System.out.println("--------------");
            for(int x=0;x<queryWS_client.getRecords().size();x++) {
                System.out.println(queryWS_client.getRecords().get(x).getTitle());
                System.out.println(queryWS_client.getRecords().get(x).getLibraryID());
            }

            searchQuery = queryWS_client.getSearchQuery();
            System.out.println("searchQuery = " + searchQuery);
            searchQuery += " +language:it +libraryID:\"Test telap\"";

            CACAO_searchWS_client searchWS_client;
            int start = 0;
            String searchWS_res = null;
            searchWS_client = new CACAO_searchWS_client(rootWsUrl + "/CACAO_SearchWS");
            searchWS_res = searchWS_client.searchRecords(searchQuery, FACETS, start, 5);
            boolean parseRes2 = searchWS_client.parseResultMSG(new String(searchWS_res.getBytes(), "UTF-8"));

            System.out.println("--------------");
            for(int x=0;x<searchWS_client.getRecords().size();x++) {
                System.out.println(searchWS_client.getRecords().get(x).getTitle());
                System.out.println(searchWS_client.getRecords().get(x).getLibraryID());
            }
        } catch (Exception ex) {
            System.out.println(ex.toString());
        }
    }
}


