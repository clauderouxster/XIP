package eu.project.Cacao.Components.Utils;
import java.io.IOException;
import java.util.Iterator;
import java.util.List;
import java.util.Vector;

import javax.xml.parsers.ParserConfigurationException;

import org.apache.log4j.Logger;
import org.apache.solr.client.solrj.SolrQuery;
import org.apache.solr.client.solrj.SolrServer;
import org.apache.solr.client.solrj.SolrServerException;
import org.apache.solr.client.solrj.embedded.EmbeddedSolrServer;
import org.apache.solr.client.solrj.response.FacetField;
import org.apache.solr.client.solrj.response.QueryResponse;
import org.apache.solr.common.SolrDocument;
import org.apache.solr.common.SolrDocumentList;
import org.apache.solr.common.SolrInputDocument;
import org.apache.solr.core.CoreContainer;
import org.apache.solr.core.CoreDescriptor;
import org.apache.solr.core.SolrCore;
import org.xml.sax.SAXException;

import eu.project.Cacao.Connectors.DB.LibraryRecord;
import eu.project.Cacao.Connectors.Metadata.DublinCoreRecord;


public class SolrWrapper {
	protected static final String recordID_field="recordID", title_field="title", lang_field="language", publisher_field="publisher",
	oaiSet_field="OAIset", telRecordID_URL_field="telRecordID_URL", type_field="type", subject_field="subject", score_field="score",
	ISSN_field="identifier_ISSN", ISBN_field="identifier_ISBN", inferredURL_field="URL", identifierUrl_field="identifier_URL", 
	libraryID_field="libraryID", description_field="description", contributor_field="contributor", identifier_field="identifier", 
	tableOfContents_field="tableOfContents", creator_field="creator";

	
	SolrServer embeddedServer;
	SolrCore core;
	
	protected Logger logger = Logger.getLogger(SolrWrapper.class.getName());
	
	public static void main(String args[]){
		Logger logger = Logger.getLogger(SolrWrapper.class.getName());
		//System.err.close();
		SolrWrapper sc=null;
		try {
			sc=new SolrWrapper("D:\\Sviluppo\\CACAO\\data\\solr");
			sc.getSearchResults("roman law", new String[]{"language","typeFacet"}, 0, 20);
			sc.getSearchResults("title_lemmatized:poetry_noun latin", new String[]{"typeFacet"}, 10, 20);
			sc.close();
		} catch (Exception e) {
			logger.error(e.getMessage());
			if(sc!=null)
				sc.close();
		}
	}
	
	public boolean deleteAllDocs(){
		try {
			this.embeddedServer.deleteByQuery( "*:*" );
			this.embeddedServer.commit();
			return true;
		} catch (Exception e) {
			logger.error("Error removing all documents from the Solr Index.\n"+e.getMessage());
			return false;
		}
	}
	
	public void close(){
		core.closeSearcher();
		core.close();
		embeddedServer=null;
	}
	
	public SolrWrapper(String solrHome) throws IOException, ParserConfigurationException, SAXException{
		System.setProperty("solr.solr.home", solrHome );
		String coreName="cacaoCore";
		CoreContainer container = new CoreContainer();
		CoreDescriptor descriptor = new CoreDescriptor(container, coreName, System.getProperty("solr.solr.home") );
		core = container.create(descriptor);
		container.register(coreName, core, false);
		embeddedServer = new EmbeddedSolrServer(container, coreName);
	}
	
	
	public SearchResult getSearchResults(String query, String[] facetFieldNames, int startFrom, int tot ) {
		
		SolrQuery sq = new SolrQuery(query);
		sq.setStart(startFrom);
		sq.setRows(tot);
		sq.setIncludeScore(true);
		if(facetFieldNames!=null){
			for(String facetFieldName: facetFieldNames)
				if(facetFieldName!=null&&facetFieldName.length()>0)
					sq.addFacetField(facetFieldName);
		}
		
		try {
			QueryResponse resp = this.embeddedServer.query( sq );
			
			SolrDocumentList docList = resp.getResults();
			long totFound = docList.getNumFound();
			Vector<DocsSearchResult> docs = new Vector<DocsSearchResult>(docList.size());
			for(int numDoc=0;numDoc<docList.size();numDoc++){
				SolrDocument doc=docList.get(numDoc);
				DocsSearchResult sr = DocsSearchResult.createSearchResultFromSolrDocument(doc);
				docs.add(sr);
			}
			
			List<FacetField> facetList = resp.getFacetFields();
			Vector<FacetsSearchResult> facets = new Vector<FacetsSearchResult>(facetList.size());
			for(Iterator<FacetField>it=facetList.iterator();it.hasNext();){
				FacetField facet=it.next();
				facets.addAll(FacetsSearchResult.createFacetsSearchResultFromFacetField(facet));
			}
			
			return new SearchResult(docs, facets, totFound);
		} catch (SolrServerException e) {
			logger.error("Error submitting the query "+sq+" to the solrEmbeddedServer.\n"+e.getMessage());
			return new SearchResult(null,null, 0);
		}
	}
}

