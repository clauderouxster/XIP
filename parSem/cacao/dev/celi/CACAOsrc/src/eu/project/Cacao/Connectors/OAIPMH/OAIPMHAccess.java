/**
 * 
 */
package eu.project.Cacao.Connectors.OAIPMH;

import java.net.URL;
import java.util.Date;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

import org.apache.log4j.Logger;

import eu.project.Cacao.Connectors.DB.Library;
import eu.project.Cacao.Connectors.DB.LibraryCollection;
import eu.project.Cacao.Connectors.DB.LibraryRecord;
import eu.project.Cacao.Connectors.Metadata.DublinCoreRecord;
import eu.project.Cacao.Interfaces.UnifiedRepositoryAccess;
import eu.project.Cacao.Harvester.OAIHarvestingAgent;

/**
 * NOT YET FULLY IMPLEMENTED !!!
 * 
 * @author Igor Barsanti - <a href="http://www.gonetwork.it"
 *         target="_blank">Gonetwork Srl</a>
 * 
 */
public class OAIPMHAccess implements UnifiedRepositoryAccess {

	String repositoryUrl;
	Library library;
	boolean libraryMustFetched = true;

	private Logger logger = Logger.getLogger(OAIHarvestingAgent.class);

	public String resumptionToken;

	public OAIPMHAccess(String url) {
		this.repositoryUrl = url;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#getDCMI_Type(java
	 * .net.URL)
	 */
	public String getDCMI_Type(URL resourceURL) {
		// TODO Auto-generated method stub
		return null;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#getDC_Format(java
	 * .net.URL)
	 */
	public String getDC_Format(URL resourceURL) {
		// TODO Auto-generated method stub
		return null;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveAllTextFromRecord
	 * (java.lang.String)
	 */
	public String retrieveAllTextFromRecord(String recordID) {
		// TODO Auto-generated method stub
		return null;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveCollection
	 * (java.lang.String)
	 */
	public LibraryCollection retrieveCollection(String collectionID) {

		Vector<String> results = new Vector<String>();
		ListSets oai = new ListSets(repositoryUrl);

		try {
			oai.sendRequest();

			Hashtable<String, String> sets = oai.getSets();

			String description = sets.get(collectionID);

			LibraryCollection collection = new LibraryCollection(this.library, description, collectionID, "");

			return collection;
		} catch (Exception e) {
			return null;
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveCollectionsID
	 * (java.lang.String)
	 */
	public String[] retrieveCollectionsID(String libraryID) {

		Vector<String> results = new Vector<String>();
		ListSets oai = new ListSets(repositoryUrl);

		try {
			oai.sendRequest();

			Hashtable<String, String> sets = oai.getSets();
			Enumeration<String> keys = sets.keys();

			while (keys.hasMoreElements()) {
				String name = keys.nextElement();
				results.add(name);
			}

			String[] resultsStr = results.toArray(new String[0]);

			return (resultsStr);
		} catch (Exception e) {
			return null;
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveCollectionsID
	 * (eu.project.Cacao.Connectors.DB.Library)
	 */
	public String[] retrieveCollectionsID(Library library) {
		return retrieveCollectionsID(library.getID());
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveDeletedRecordsID
	 * (java.lang.String)
	 */
	public String[] retrieveDeletedRecordsID(String libraryID) {
		// TODO Auto-generated method stub
		return null;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveDeletedRecordsID
	 * (eu.project.Cacao.Connectors.DB.Library)
	 */
	public String[] retrieveDeletedRecordsID(Library library) {
		// TODO Auto-generated method stub
		return null;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveDeletedRecordsID
	 * (java.lang.String, java.lang.String)
	 */
	public String[] retrieveDeletedRecordsID(String libraryID, String collectionID) {
		// TODO Auto-generated method stub
		return null;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveDeletedRecordsID
	 * (eu.project.Cacao.Connectors.DB.Library,
	 * eu.project.Cacao.Connectors.DB.LibraryCollection)
	 */
	public String[] retrieveDeletedRecordsID(Library library, LibraryCollection collection) {
		// TODO Auto-generated method stub
		return null;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveLibrariesID()
	 */
	public String[] retrieveLibrariesID() {
		String[] libs = new String[1];
		libs[0] = "1";
		return libs;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveLibrary(java
	 * .lang.String)
	 */
	public Library retrieveLibrary(String libraryID) {
		if (libraryMustFetched) {
			Identify oai = new Identify(repositoryUrl);
			try {
				oai.sendRequest();
				this.library = new Library(oai.getRepositoryName(), null, oai.getBaseURL(), "OAIPMH" + oai.getProtocolVersion());
				libraryMustFetched = false;
				return library;
			} catch (Exception e) {
				return null;
			}
		} else {
			return this.library;
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveMetadata(
	 * java.lang.String)
	 */
	public DublinCoreRecord retrieveMetadata(String recordID) {
		// TODO Auto-generated method stub
		return null;
	}

	public LibraryRecord retrieveRecord(String recordID, LibraryCollection collection) {
		return retrieveRecord(recordID, collection, "oai_dc");
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveRecord(java
	 * .lang.String)
	 */
	public LibraryRecord retrieveRecord(String recordID, LibraryCollection collection, String metaDataPrefix) {
		try {
			LibraryRecord record;
			GetRecord oai = new GetRecord(repositoryUrl, recordID, metaDataPrefix);
			oai.sendRequest();
			if (metaDataPrefix.equalsIgnoreCase("oai_dc"))
				record = new LibraryRecord(recordID, library, collection, oai.getDublinCoreRecord(), null, null, false);
			else
				record = new LibraryRecord(recordID, library, collection, null, oai.getTelAPRecord(), null, false);

			return record;
		} catch (Exception e) {
			return null;
		}
	}

	public String[] retrieveRecordsID(String collectionCode) {
		return this.retrieveRecordsID(collectionCode, "oai_dc", null, null, null);
	}

	public String[] retrieveRecordsID(String collectionCode, String metaDataPrefix) {
		return retrieveRecordsID(collectionCode, metaDataPrefix, null, null, null);
	}

	public String[] retrieveRecordsID(String collectionCode, String metaDataPrefix, String resumptionToken) {
		return retrieveRecordsID(collectionCode, metaDataPrefix, resumptionToken, null, null);
	}
	
	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveRecordsID
	 * (java.lang.String)
	 */
	public String[] retrieveRecordsID(String collectionCode, String metaDataPrefix, String resumptionToken, String from, String to) {

		// Vector<String> results = new Vector<String>();
		ListRecords oai = new ListRecords(repositoryUrl, metaDataPrefix);
		oai.setSet(collectionCode);
		
		if ((from != null) && (from.trim().length() > 0)) 
			oai.setFrom(from);
		
		if ((to != null) && (to.trim().length() > 0))
			oai.setUntil(to);
		
		try {
			oai.sendRequest(resumptionToken);
			
			Vector<String> records = oai.getRecords();

			String[] resultsStr = records.toArray(new String[0]);

			this.resumptionToken = oai.getResumptionToken();

			return (resultsStr);
		} catch (Exception e) {
			logger.error("Error retrieving Records IDs from "+repositoryUrl+" for oaiSet "+collectionCode+".\n"+e);
			return null;
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveRecordsID
	 * (java.lang.String, java.util.Date, java.util.Date)
	 */
	public String[] retrieveRecordsID(String collectionID, Date from, Date to) {
		// TODO Auto-generated method stub
		return null;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveRecordsID
	 * (eu.project.Cacao.Connectors.DB.LibraryCollection)
	 */
	public String[] retrieveRecordsID(LibraryCollection collection) {
		return retrieveRecordsID(collection.getCode());
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveRecordsID
	 * (eu.project.Cacao.Connectors.DB.LibraryCollection, java.util.Date,
	 * java.util.Date)
	 */
	public String[] retrieveRecordsID(LibraryCollection collection, Date from, Date to) {
		// TODO Auto-generated method stub
		return null;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveResourceURL
	 * (java.lang.String)
	 */
	public URL retrieveResourceURL(String recordID) {
		// TODO Auto-generated method stub
		return null;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveResourceURL
	 * (eu.project.Cacao.Connectors.DB.LibraryRecord)
	 */
	public URL retrieveResourceURL(LibraryRecord record) {
		// TODO Auto-generated method stub
		return null;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveUserLogs(
	 * java.lang.String, java.util.Date, java.util.Date)
	 */
	public String[] retrieveUserLogs(String libraryID, Date from, Date to) {
		// TODO Auto-generated method stub
		return null;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveUserLogs(
	 * eu.project.Cacao.Connectors.DB.Library, java.util.Date, java.util.Date)
	 */
	public String[] retrieveUserLogs(Library library, Date from, Date to) {
		// TODO Auto-generated method stub
		return null;
	}

	/**
	 * @return the resumptionToken
	 */
	public String getResumptionToken() {
		return resumptionToken;
	}

	/**
	 * @return the library
	 */
	public Library getLibrary() {
		return library;
	}

	/**
	 * @param library the library to set
	 */
	public void setLibrary(Library library) {
		this.library = library;
		libraryMustFetched = false;
	}

}
