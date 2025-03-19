package eu.project.Cacao.WS_Clients;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Vector;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.log4j.Logger;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

import com.sun.org.apache.xml.internal.serialize.OutputFormat;
import com.sun.org.apache.xml.internal.serialize.XMLSerializer;

import eu.project.Cacao.Components.Utils.DocsSearchResult;
import eu.project.Cacao.Components.Utils.FacetsSearchResult;
import eu.project.Cacao.Components.Utils.NLPtoken;
import eu.project.Cacao.Components.Utils.RunTimer;
import eu.project.Cacao.WS_Clients.CACAO_queryWS_stub.ProcessQueryAndSearchCustomResponse;
import eu.project.Cacao.WS_Clients.CACAO_queryWS_stub.ProcessQueryAndSearchResponse;
import eu.project.Cacao.WS_Clients.CACAO_queryWS_stub.ProcessQueryCustomResponse;
import eu.project.Cacao.WS_Clients.CACAO_queryWS_stub.ProcessQueryResponse;


public class CACAO_queryWS_client {
	private Vector<NLPtoken> searchTerms;
	private Vector<FacetsSearchResult> facets;
	private Vector<DocsSearchResult> records;
	private String searchQuery, inputLanguage;
	private int retrievedRecordsStartFrom, recordsRetrieved;
	private long recordsFound;
	
	public static final String isTranslatedAttribute="isTranslation", isExpandedAttribute="isExpansion",
	isTranslExpAttribute="isExpandedTranslation", searchTermTag="SearchTerm", facetResultElmTag="Facet", 
	recordElementTag="Record", searchQueryTag="SearchQuery", inputLangAttr="inputLanguage", 
	queryElementTag = "CACAO_InputQuery", retrievedDocsElementTag="CACAO_RetrievedRecords", resultStartsFromAttr="recordsStartFrom", 
	returnedRecordsAttr="recordsReturned", foundRecordsAttr="recordsFound";

	String endpoint="http://demo.celi.it:8095/axis2/services/CACAO_queryWS";
	protected Logger logger = Logger.getLogger(CACAO_queryWS_client.class.getName());
	
	public CACAO_queryWS_client(String endpoint) {
		this.endpoint = endpoint;
		this.searchTerms = new Vector<NLPtoken>();
		this.facets = new Vector<FacetsSearchResult>();
		this.records = new Vector<DocsSearchResult>();
	}
	
	
	public static void main(String[] args){
		//localhost:8080/axis2/services/CACAO_WS
		RunTimer rt=new RunTimer();
		CACAO_queryWS_client a=new CACAO_queryWS_client("http://hermes.celi.it:8080/axis2/services/CACAO_queryWS");
		System.out.println("created WS client in "+rt.getElapsedTimeLabel());
		String msgg=a.processQueryAndSearchCustom("law","en", new String[]{"de","fr","en","it","pl"}, 
				false, false, false, false, new String[]{"language"},0,50);
		System.out.println("got in "+rt.getElapsedTimeLabel()+"\n"+msgg);
		
//		a.parseResultMSG(msgg);
//		System.out.println(a.getInputLanguage()+" "+a.getSearchQuery());
//		for(DocsSearchResult doc: a.records){
//			System.out.println(doc.getRecordID()+" "+doc.getTitle());
//		}
	}
	
	public String processQueryAndSearch(String query, String langFrom, String[] targetLanguages,
			String[] facetFieldNames, int startFrom, int totResults) {
		try {
			CACAO_queryWS_stub a=new CACAO_queryWS_stub(this.endpoint);
			CACAO_queryWS_stub.ProcessQueryAndSearch pq=new CACAO_queryWS_stub.ProcessQueryAndSearch();
			pq.setQuery(query);
			pq.setLangFrom(langFrom);
			pq.setTargetLanguages(targetLanguages);
			pq.setFacetFieldNames(facetFieldNames);
			pq.setStartFrom(startFrom);
			pq.setTotResults(totResults);
				
			ProcessQueryAndSearchResponse result=a.processQueryAndSearch(pq);
			return new String(result.get_return());
		} catch (Exception e) {
			logger.error(e);
		}
		
		return "";
	}
	
	public String processQueryAndSearchCustom(String query, String langFrom, String[] targetLanguages, 
			boolean translateWithSystran, boolean expandQueryWithSemVect, boolean expandWithWordNet, 
			boolean expandWithWordToCat, String[] facetFieldNames, int startFrom, int totResults) {
		try {
			CACAO_queryWS_stub a=new CACAO_queryWS_stub(this.endpoint);
			CACAO_queryWS_stub.ProcessQueryAndSearchCustom pq=new CACAO_queryWS_stub.ProcessQueryAndSearchCustom();
			pq.setQuery(query);
			pq.setLangFrom(langFrom);
			pq.setTargetLanguages(targetLanguages);
			
			pq.setTranslateWithSystran(translateWithSystran);
			pq.setExpandQueryWithSemVect(expandQueryWithSemVect);
			pq.setExpandWithWordNet(expandWithWordNet);
			pq.setExpandWithWord2Cat(expandWithWordToCat);
			
			pq.setFacetFieldNames(facetFieldNames);
			pq.setStartFrom(startFrom);
			pq.setTotResults(totResults);
			
			ProcessQueryAndSearchCustomResponse result=a.processQueryAndSearchCustom(pq);
			return new String(result.get_return());
		} catch (Exception e) {
			logger.error(e);
		}
		
		return "";
	}
	
	public String processQuery(String query, String langFrom, String[] targetLanguages){
	    	try {
	    		CACAO_queryWS_stub a=new CACAO_queryWS_stub(this.endpoint);
	    		CACAO_queryWS_stub.ProcessQuery pq=new CACAO_queryWS_stub.ProcessQuery();
				pq.setQuery(query);
				pq.setLangFrom(langFrom);
				pq.setTargetLanguages(targetLanguages);
				
				ProcessQueryResponse result=a.processQuery(pq);
				return new String(result.get_return());
			} catch (Exception e) {
				logger.error(e);
				return null;
			}
	 
	  }
	
	public String processQueryCustom(String query, String langFrom, String[] targetLanguages,	boolean translateWithSystran, 
    		boolean expandQueryWithSemVect, boolean expandWithWordNet, boolean expandWithWordToCat ){
	    	try {
	    		CACAO_queryWS_stub a=new CACAO_queryWS_stub(this.endpoint);
	    		CACAO_queryWS_stub.ProcessQueryCustom pq=new CACAO_queryWS_stub.ProcessQueryCustom();
				pq.setQuery(query);
				pq.setLangFrom(langFrom);
				pq.setTargetLanguages(targetLanguages);
				
				pq.setTranslateWithSystran(translateWithSystran);
				pq.setExpandQueryWithSemVect(expandQueryWithSemVect);
				pq.setExpandWithWordNet(expandWithWordNet);
				pq.setExpandWithWordToCat(expandWithWordToCat);
				
				ProcessQueryCustomResponse result=a.processQueryCustom(pq);
				return new String(result.get_return());
			} catch (Exception e) {
				logger.error(e);
				return null;
			}
	 
	  }
	 
	 public boolean parseResultMSG(String src){
		 try {	
				DocumentBuilderFactory docFactory = DocumentBuilderFactory.newInstance();
				DocumentBuilder documentBuilder = docFactory.newDocumentBuilder();
				InputStream is = new ByteArrayInputStream(src.getBytes());
				Document doc = documentBuilder.parse(is,"UTF-8");
				
				//this.printToFile(doc, "Cacao_prova.xml");
				Element inputQuery= (Element)doc.getElementsByTagName(CACAO_queryWS_client.queryElementTag).item(0);
				this.inputLanguage = inputQuery.getAttribute(CACAO_queryWS_client.inputLangAttr);
				
				Element searchQuery= (Element)doc.getElementsByTagName(CACAO_queryWS_client.searchQueryTag).item(0);
				this.searchQuery = searchQuery.getTextContent();
				
				NodeList nodes = doc.getElementsByTagName(CACAO_queryWS_client.searchTermTag);
				for (int i = 0; i < nodes.getLength(); i++)
	            {
	                Element e = (Element) nodes.item(i);
	                this.searchTerms.add(NLPtoken.createFromXMLElement(e));
	            }
				
				nodes = doc.getElementsByTagName(CACAO_queryWS_client.facetResultElmTag);
				for (int i = 0; nodes!=null && i < nodes.getLength(); i++)
	            {
	                Element e = (Element) nodes.item(i);
	                this.facets.add(FacetsSearchResult.createFromXMLElement(e));
	            }
				
				nodes = doc.getElementsByTagName(CACAO_queryWS_client.retrievedDocsElementTag);
				if(nodes!=null && nodes.getLength()>0){
					Element records= (Element)nodes.item(0);
					this.retrievedRecordsStartFrom = Integer.parseInt( records.getAttribute(CACAO_queryWS_client.resultStartsFromAttr) );
					this.recordsRetrieved = Integer.parseInt( records.getAttribute(CACAO_queryWS_client.returnedRecordsAttr) );
					this.recordsFound = Long.parseLong( records.getAttribute(CACAO_queryWS_client.foundRecordsAttr) );
				}
				
				nodes = doc.getElementsByTagName(CACAO_queryWS_client.recordElementTag);
				for (int i = 0; nodes!=null && i < nodes.getLength(); i++)
	            {
	                Element e = (Element) nodes.item(i);
	                this.records.add(DocsSearchResult.createFromXMLElement(e));
	            }
				
				return true;
			} catch (Exception e) {
				logger.error(e);
				return false;
			}
	 }
	 
	 private void printToFile(Document dom, String fname){
			try
			{	//print
				OutputFormat format = new OutputFormat(dom);
				format.setEncoding("UTF-8");
				format.setIndenting(true);

				//to generate a file output use fileoutputstream instead of system.out
				XMLSerializer serializer = new XMLSerializer(new FileOutputStream(new File(fname)), format);
				serializer.serialize(dom);
			} catch(IOException e) {
			    logger.error(e);
			}
	}

	public Vector<NLPtoken> getSearchTerms() {
		return searchTerms;
	}

	public Vector<FacetsSearchResult> getFacets() {
		return facets;
	}

	public Vector<DocsSearchResult> getRecords() {
		return records;
	}

	public String getSearchQuery() {
		return searchQuery;
	}

	public String getInputLanguage() {
		return inputLanguage;
	}

	public int getRetrievedRecordsStartFrom() {
		return retrievedRecordsStartFrom;
	}

	public int getRetrievedRecordsTot() {
		return recordsRetrieved;
	}

	public long getRecordsFound() {
		return recordsFound;
	}
}
