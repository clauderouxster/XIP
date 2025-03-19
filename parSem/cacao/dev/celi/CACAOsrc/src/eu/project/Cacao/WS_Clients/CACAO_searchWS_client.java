package eu.project.Cacao.WS_Clients;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.rmi.RemoteException;
import java.util.Vector;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.axis2.AxisFault;
import org.apache.log4j.Logger;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

import eu.project.Cacao.Components.Utils.DocsSearchResult;
import eu.project.Cacao.Components.Utils.FacetsSearchResult;
import eu.project.Cacao.WS_Clients.CACAO_searchWS_stub.SearchRecords;
import eu.project.Cacao.WS_Clients.CACAO_searchWS_stub.SearchRecordsResponse;

public class CACAO_searchWS_client {
	private Logger logger = Logger.getLogger(CACAO_searchWS_client.class);
	private CACAO_searchWS_stub cl;
	
	private Vector<FacetsSearchResult> facets;
	private Vector<DocsSearchResult> records;
	private int retrievedRecordsStartFrom, retrievedRecordsTot;
	private long recordsFound;
	
	public CACAO_searchWS_client(String endpoint) throws AxisFault {
		this.cl = new CACAO_searchWS_stub(endpoint);
	}

	public CACAO_searchWS_client() throws AxisFault {
		this.cl = new CACAO_searchWS_stub();
	}
	
	public String searchRecords(String query, String[] facetFieldNames, int startFrom, int totResults){
		try {
			SearchRecords sr=new SearchRecords();
			sr.setQuery(query);
			sr.setFacetFieldNames(facetFieldNames);
			sr.setStartFrom(startFrom);
			sr.setTotResults(totResults);
			
			SearchRecordsResponse response = this.cl.searchRecords(sr);
			
			return response.get_return();
		} catch (RemoteException e) {
			logger.error("Error searching for records using Search WS.\n"+e.getMessage());
			return null;
		}
	}
	
	 public boolean parseResultMSG(String src){
		 try {	
				DocumentBuilderFactory docFactory = DocumentBuilderFactory.newInstance();
				DocumentBuilder documentBuilder = docFactory.newDocumentBuilder();
				InputStream is = new ByteArrayInputStream(src.getBytes());
				Document doc = documentBuilder.parse(is,"UTF-8");
				
				this.facets=new Vector<FacetsSearchResult>();
				this.records=new Vector<DocsSearchResult>();
				
				NodeList nodes = doc.getElementsByTagName(CACAO_queryWS_client.facetResultElmTag);
				for (int i = 0; nodes!=null && i < nodes.getLength(); i++)
	            {
	                Element e = (Element) nodes.item(i);
	                this.facets.add(FacetsSearchResult.createFromXMLElement(e));
	            }
				
				nodes = doc.getElementsByTagName(CACAO_queryWS_client.retrievedDocsElementTag);
				if(nodes!=null && nodes.getLength()>0){
					Element records= (Element)nodes.item(0);
					this.retrievedRecordsStartFrom = Integer.parseInt( records.getAttribute(CACAO_queryWS_client.resultStartsFromAttr) );
					this.retrievedRecordsTot = Integer.parseInt( records.getAttribute(CACAO_queryWS_client.returnedRecordsAttr) );
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
	
	public static void main(String args[]) throws RemoteException {
		//System.err.close();
		
		CACAO_searchWS_client cl = new CACAO_searchWS_client("http://hermes.celi.it:8080/axis2/services/CACAO_SearchWS");	   
		String s ="title:(roman)^2.0 subject:(roman)^1.5 tableOfContents:(roman)^1.0 description:(roman)^1.0 title_lemmatized:(roman_noun)^2.0 subject_lemmatized:(roman_noun)^1.0 description_lemmatized:(roman_noun)^1.0 tableOfContents_lemmatized:(roman_noun)^1.0 subject_enStem:(roman)^1.0 title_enStem:(roman)^1.0 tableOfContents_enStem:(roman)^1.0 description_enStem:(roman)^1.0 title:(law)^2.0 subject:(law)^1.5 tableOfContents:(law)^1.0 description:(law)^1.0 title_lemmatized:(law_noun)^2.0 subject_lemmatized:(law_noun)^1.0 description_lemmatized:(law_noun)^1.0 tableOfContents_lemmatized:(law_noun)^1.0 subject_enStem:(law)^1.0 title_enStem:(law)^1.0 tableOfContents_enStem:(law)^1.0 description_enStem:(law)^1.0 +language:fr";
		String msg = cl.searchRecords(s, new String[]{"language"}, 0, 10);
		System.out.println(msg);
		
		cl.parseResultMSG(msg);
		System.out.println(cl.retrievedRecordsTot);
		
	}

	public Vector<FacetsSearchResult> getFacets() {
		return facets;
	}

	public void setFacets(Vector<FacetsSearchResult> facets) {
		this.facets = facets;
	}

	public Vector<DocsSearchResult> getRecords() {
		return records;
	}

	public void setRecords(Vector<DocsSearchResult> records) {
		this.records = records;
	}

	public int getRetrievedRecordsStartFrom() {
		return retrievedRecordsStartFrom;
	}

	public void setRetrievedRecordsStartFrom(int retrievedRecordsStartFrom) {
		this.retrievedRecordsStartFrom = retrievedRecordsStartFrom;
	}

	public int getRetrievedRecordsTot() {
		return retrievedRecordsTot;
	}

	public void setRetrievedRecordsTot(int retrievedRecordsTot) {
		this.retrievedRecordsTot = retrievedRecordsTot;
	}

	public long getRecordsFound() {
		return recordsFound;
	}
}
