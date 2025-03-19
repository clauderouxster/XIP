package wsTest1;

import eu.project.Cacao.Components.Utils.*;
import eu.project.Cacao.WS_Clients.CACAO_queryWS_client;
import java.io.UnsupportedEncodingException;
import java.rmi.RemoteException;
import java.util.Vector;
import org.apache.axis2.client.Options;
import org.apache.axis2.transport.http.*;
import org.apache.log4j.Logger;

/**
 *
 * @author ctrojahn
 */
public class Ws_client1 {

    final static String CACAOwsEndpoint = "http://213.215.252.8:8080/axis2/services/CACAO_queryWS";

    /**
     * @param args the command line arguments
     * @throws RemoteException
     * @throws UnsupportedEncodingException 
     */
    public static void main(String[] args) throws RemoteException, UnsupportedEncodingException {

        Logger logger = Logger.getLogger(Ws_client1.class.getName());
//        Options options = new Options();
//        timeout setting
//        logger.info("default timeout: " + options.getTimeOutInMilliSeconds());
//        options.setTimeOutInMilliSeconds(10000);
//        logger.info("new timeout: " + options.getTimeOutInMilliSeconds());
//        // proxy setting
//        HttpTransportProperties.ProxyProperties proxyProperties = new HttpTransportProperties.ProxyProperties();
//        proxyProperties.setProxyName("cornillon.grenoble.xrce.xerox.com");
//        proxyProperties.setDomain("cornillon.grenoble.xrce.xerox.com");
//        proxyProperties.setProxyPort(8000);
//        //in order to makesure that we use HTTP 1.0
//        options.setProperty(org.apache.axis2.transport.http.HTTPConstants.HTTP_PROTOCOL_VERSION,
//                HTTPConstants.HEADER_PROTOCOL_10);
//        options.setProperty(org.apache.axis2.transport.http.HTTPConstants.PROXY, proxyProperties);
        logger.info(CACAOwsEndpoint);
        // Interface parameters
        String inputQuery = "bank";
        String srcLang = "en";
        String[] targetLang = {"en", "fr"};
        String[] facetsFields = {"language"};
        Vector<FacetsSearchResult> facets = null;
        Vector<DocsSearchResult> records = null;
        Vector<NLPtoken> searchTerms = null;
        String searchQuery = null;
        int start = 0;
        int max = 20;
        boolean systran = false, SVexp = false, WNexp = false, W2Cexp = false;
        CACAO_queryWS_client a = new CACAO_queryWS_client(CACAOwsEndpoint);
        String msg = a.processQueryAndSearchCustom(inputQuery, srcLang, targetLang, systran,
                SVexp, WNexp, W2Cexp, facetsFields, start, max);
        a.parseResultMSG(new String(msg.getBytes(), "UTF-8"));
        facets = a.getFacets();
        records = a.getRecords();
        searchTerms = a.getSearchTerms();
        searchQuery = a.getSearchQuery();
        System.out.println("Facets: " + facets.size());
        for (int x = 0; x < facets.size(); x++) {
            System.out.println(facets.elementAt(x).getFacetField() + " " + facets.elementAt(x).getFacetValue() + "\n");
        }
        /*
        DocsSearchResult(HashSet<String> contributors, HashSet<String> languages, HashSet<String> creators)
         */
        System.out.println("\n\nRecords: " + records.size());
        for (int x = 0; x < records.size(); x++) {
            DocsSearchResult doc = records.elementAt(x);
            System.out.println(doc.getScore() + "," +
                    doc.getLibraryID() + "," +
                    doc.getTitle() + "," +
                    doc.getPublisher() + "," +
                    doc.getURL() + "," +
                    doc.getISSN() + "," +
                    doc.getISBN() + "," +
                    doc.getTelRecordID_URL() + "," +
                    doc.getIdentifier() + "," +
                    doc.getRecordID() + "," +
                    doc.getType() + "," +
                    doc.getSubjects() + "," +
                    doc.getOaiSets() + "," +
                    doc.getDescriptions() + "," +
                    doc.getTableOfContents() + "," +
                    doc.getContributors() + "," +
                    doc.getLanguages() + "," +
                    doc.getCreators() + "\n");
        }
        System.out.println("\n\nTerms: " + searchTerms.size());
        System.out.println("\n\nQuery: " + searchQuery);


        for (int x = 0; x < searchTerms.size(); x++)
        {
            System.out.println(searchTerms.get(x).lemma);
        }


    }

    /**
     *
     * @return
     */
    public Vector<FacetsSearchResult> getFacets1() {
        return new Vector<FacetsSearchResult>();
    }

    /**
     *
     * @return
     */
    public Vector<DocsSearchResult> getRecords1() {
        return new Vector<DocsSearchResult>();
    }
}
