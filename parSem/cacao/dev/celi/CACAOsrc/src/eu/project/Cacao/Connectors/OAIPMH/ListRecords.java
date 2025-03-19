/**
 * 
 */
package eu.project.Cacao.Connectors.OAIPMH;

import java.util.Vector;

import org.xml.sax.SAXException;

/**
 * 
 * Class for ListIdentifiers OAI-PMH request.
 *  
 * @author Igor Barsanti - <a href="http://www.gonetwork.it" target="_blank">Gonetwork Srl</a>
 *
 */
public class ListRecords extends GenericRequest {

	private String from;
	private String until;
	private String metadataPrefix;
	private String set;
	private String responseDate;
	private Vector<String> records = new Vector<String>();
	private String idName;
	//private String dc_description;

	/**
	 * 
	 * Class constructor
	 * 
	 * @param url
	 *            The OAI-PMH Repository Url
	 * @param metadataPrefix
	 *            Record's identifiers preferred metadata format
	 */
	public ListRecords (String url, String metadataPrefix) {
		super(url, "ListRecords");
		this.metadataPrefix = metadataPrefix;
		setOptions();
		// TODO Auto-generated constructor stub
	}
	
	/* (non-Javadoc)
	 * @see org.xml.sax.helpers.DefaultHandler#endElement(java.lang.String, java.lang.String, java.lang.String)
	 */
	public void endElement(String namespaceURI, String localName,
			String qualifiedName) throws SAXException {

		if (qualifiedName.equalsIgnoreCase("responseDate")) {
			responseDate = elementValue;
		}
		if (qualifiedName.equalsIgnoreCase("identifier")) {
			idName = elementValue;
			//if ((dc_description != null) && (dc_description != "")) {
				//records.put(idName, dc_description);
				records.add(idName);
				idName = null;
				//dc_description = null;
			//}
		}
		
		/*if (qualifiedName.equalsIgnoreCase("dc:description")) {
			dc_description = elementValue;
			if ((dc_description != null) && (dc_description != "")) {
				records.put(idName, dc_description);
				idName = null;
				dc_description = null;
			}
		}*/
		else if (qualifiedName.equalsIgnoreCase("error")) {
			errorDescription = elementValue;
			oaierror = true;
		}			
		
		super.endElement(namespaceURI, localName, qualifiedName);
	}

	/**
	 * Return from field of the OAI-PMH request.
	 * 
	 * @return from field
	 */
	public String getFrom() {
		return from;
	}

	/**
	 * Set the options string for OAI-PMH request
	 */
	private void setOptions() {
		String opts = "";

		if (from != null) opts += "&from=" + from;
		if (until != null) opts += "&until=" + until;
		if (metadataPrefix != null) opts += "&metadataPrefix=" + metadataPrefix;
		if (set != null) opts += "&set=" + set;

		super.setOAIOptions(opts);
	}

	/**
	 * Set the from field of the OAI-PMH request, which specifies a lower bound
	 * for datestamp-based selective harvesting.
	 * 
	 * @param from
	 *            from field
	 */
	public void setFrom(String from) {
		this.from = from;
		setOptions();
	}

	/**
	 * Return until field of the OAI-PMH request.
	 * 
	 * @return until field
	 */
	public String getUntil() {
		return until;
	}

	/**
	 * Set the until field of the OAI-PMH request, which specifies a upper bound
	 * for datestamp-based selective harvesting.
	 * 
	 * @param until
	 *            until field
	 */
	public void setUntil(String until) {
		this.until = until;
		setOptions();
	}

	/**
	 * 
	 * Return the metadata format used
	 * 
	 * @return metadata format
	 */
	public String getMetadataPrefix() {
		return metadataPrefix;
	}

	/**
	 * Set the record's metadata format filter
	 * 
	 * @param metadataPrefix
	 *            metadata format
	 */
	public void setMetadataPrefix(String metadataPrefix) {
		this.metadataPrefix = metadataPrefix;
		setOptions();
	}

	/**
	 * Return the set value for selective harvesting
	 * 
	 * @return set value
	 */
	public String getSet() {
		return set;
	}

	/**
	 * Set a set OAI-PMH field value for selective harvesting of a part of a
	 * repository
	 * 
	 * @param set
	 *            set value
	 */
	public void setSet(String set) {
		this.set = set;
		setOptions();
	}

	/**
	 * Return date and time of the repository response
	 * 
	 * @return Date and time
	 */
	public String getResponseDate() {
		return responseDate;
	}

	/**
	 * Return a Vector with records received from the OAI repository. The
	 * Vector contain the identifier
	 * 
	 * @return Records
	 */
	public Vector<String> getRecords() {
		return records;
	}

}
