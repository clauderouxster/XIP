package eu.project.Cacao.Components;

import java.io.File;
import java.io.IOException;
import java.io.StringWriter;
import java.rmi.RemoteException;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.apache.log4j.Logger;
import org.w3c.dom.Document;
import org.w3c.dom.Element;

import com.sun.org.apache.xml.internal.serialize.OutputFormat;
import com.sun.org.apache.xml.internal.serialize.XMLSerializer;

import eu.project.Cacao.Components.Utils.DocsSearchResult;
import eu.project.Cacao.Components.Utils.FacetsSearchResult;
import eu.project.Cacao.Components.Utils.SearchResult;
import eu.project.Cacao.Components.Utils.SolrWrapper;



/**
 * Searches for resources in a lucene index. Used as a base class exposing public methods via WS.
 * When used as an instance invoke the closeSearcher() when finished using it, otherwise a thread may still be running 
 * also after the destruction of the instance itself
 * 
 * @author Alessio
 *
 */
public class Search_Component {
	private static final String initConfigurationFile = "configuration/SearchServiceConfiguration.xml";
    
	public final static String retrievedDocsElementTag="CACAO_RetrievedRecords", facetsElementTag="CACAO_Facets", 
	facetsQueryElementTag="FacetField", rootElmTag = "CACAO_ResponseDocument", facetFieldAttribute="field", 
	resultStartsFromAttr="recordsStartFrom", returnedRecordsAttr="recordsReturned", foundRecordsAttr="recordsFound", 
	solarHomeAttr="solarHomePath" ;
    
    protected static SolrWrapper solrSearcher;
    protected Logger logger = Logger.getLogger(Search_Component.class.getName());
    protected static boolean isInited=false;
    protected static String solrHome=null;
    protected SearchResult results=null;
    
    public Search_Component(){   }
	
    public String searchRecords(String query, String[] facetFieldNames, int startFrom, int totResults){
    	if(Search_Component.solrSearcher == null || !Search_Component.isInited){
	    	try {
	    		Search_Component.isInited = this.initService(Search_Component.initConfigurationFile);
	    		if(Search_Component.isInited)
	    			Search_Component.solrSearcher = new SolrWrapper(solrHome);
			} catch (Exception e) {
				logger.error("Error creating the SolrEmbeddedServer for the Search_Component from "+solrHome+".\n"+e);
			}
		}
    	
    	try {
			if(Search_Component.isInited){
				results = Search_Component.solrSearcher.getSearchResults(query, facetFieldNames, startFrom, totResults);
				return this.resultsAsXMLDocument(results, facetFieldNames, startFrom);
			}
		} catch (Exception e) {
			logger.error("Error querying SolrEmbeddedServer with "+query+".\n"+e);
			if(Search_Component.solrSearcher!=null){
				Search_Component.solrSearcher.close();
				Search_Component.solrSearcher=null;
			}
		}
    	
    	return this.resultsAsXMLDocument(null, null, 0);
    }
    
    private String resultsAsXMLDocument(SearchResult results, String[] facetFieldNames, int startFrom) {
        DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
		try {
			// get an instance of builder
			DocumentBuilder db = dbf.newDocumentBuilder();
			
			// create an instance of DOM
			Document doc = db.newDocument();
			// create the root element and append it to the XML document
			Element rootElm = doc.createElement(Search_Component.rootElmTag);
			doc.appendChild(rootElm);
			
			if(results != null){
				Element facetElm=doc.createElement(Search_Component.facetsElementTag);
				rootElm.appendChild(facetElm);
				
				if(facetFieldNames != null){
					for(String facet: facetFieldNames){
						Element facetFiled=doc.createElement(Search_Component.facetsQueryElementTag);
						facetFiled.setAttribute(Search_Component.facetFieldAttribute, facet);
						facetElm.appendChild(facetFiled);
					}
				}
				
				if(results.getFacets()!=null){
					for(FacetsSearchResult fsr: results.getFacets()){
						Element facetResponseElm=fsr.toXMLElement(doc);
						facetElm.appendChild(facetResponseElm);
					}
				}
			
				Element records=doc.createElement(Search_Component.retrievedDocsElementTag);
				records.setAttribute(Search_Component.resultStartsFromAttr, ""+startFrom);
				records.setAttribute(Search_Component.returnedRecordsAttr, ""+results.getDocs().size());
				records.setAttribute(Search_Component.foundRecordsAttr, ""+results.getTotFound());
				rootElm.appendChild(records);
				for(DocsSearchResult dsr: results.getDocs()){
					Element recordElm = dsr.toXMLElement(doc);
					records.appendChild(recordElm);
				}
			}
			
			try {
				StringWriter tmp=new StringWriter();
				XMLSerializer serializer = new XMLSerializer(tmp, new OutputFormat(doc,"UTF-8",true));
				serializer.serialize(doc);
				return tmp.toString();
			} catch (IOException e) {
				return null;
			}
			
		} catch (ParserConfigurationException pce) {
			return null;
		}
    }
    
    /**
	 * Read initialization parameters from an xml file for generating semantic
	 * vector spaces from a lucene index
	 * 
	 * @param xmlConfigurationFile
	 *            Configuration file path
	 * @return exit status
	 */
	private boolean initService(String xmlConfigurationFile) {
		try {
			logger.debug("Init Search Service  from " + xmlConfigurationFile);
			// getting the document builder Factory
			DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();

			// if the document has to be validated against the schema source

			// getting the document builder from the Factory
			DocumentBuilder builder = factory.newDocumentBuilder();

			// parses the xml
			Document myDocument = builder.parse(new File(xmlConfigurationFile));

			Element root = myDocument.getDocumentElement();
			Search_Component.solrHome = root.getAttribute(Search_Component.solarHomeAttr);
			
			return true;
		} catch (Exception e) {
			logger.error("Error initializing CorpusThesaurus component.\n" + e.getMessage());

			return false;
		}
	}
    
	protected SearchResult getResults() {
		return results;
	}
	
	protected void closeSearcher(){
		Search_Component.solrSearcher.close();
	}
	
	public static void main(String args[]) throws RemoteException {
		//System.err.close();	
		Search_Component cl = new Search_Component();	   
		String msg = cl.searchRecords("title:(computer)^2.0", new String[]{"language"}, 0, 10);
		System.out.println(msg);
		
	}
}
