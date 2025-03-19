package eu.project.Cacao.Connectors.OAIPMH;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;

import eu.project.Cacao.Connectors.Metadata.DCXmlException;
import eu.project.Cacao.Connectors.Metadata.DublinCoreRecord;
import eu.project.Cacao.Connectors.Metadata.TelAPRecord;

/**
 * Class for GetRecord OAI-PMH request.
 *
 * @author Igor Barsanti - <a href="http://www.gonetwork.it"
 *         target="_blank">Gonetwork srl</a>
 *
 */
public class GetRecord extends GenericRequest {
    private String identifier;
    private String metadataPrefix;
    private String responseDate;
    DublinCoreRecord dublinCoreRecord;
    TelAPRecord telAPRecord;

    /**
     *
     * Class constructor
     *
     * @param url
     *            The OAI-PMH Repository Url
     * @param identifier
     *            OAI-PMH Record Identifier
     * @param metadataPrefix
     *            Set Metadata format for the response
     */
    public GetRecord(String url, String identifier, String metadataPrefix) {
        super(url, "GetRecord");
        this.metadataPrefix = metadataPrefix;
        this.identifier = identifier;
        setOptions();
    }

    /**
     * Method called after parsing is complete
     *
     * @throws DCXmlException In case of errors in XML DublinCore data
     */
    public void endDocument() {
        try {
        	if (this.metadataPrefix.equalsIgnoreCase("oai_dc")) 
        		dublinCoreRecord = new DublinCoreRecord(this.identifier, response);
        	else
        		telAPRecord = new TelAPRecord(this.identifier, response);	
        } catch (Exception e) {
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see eu.project.Cacao.Connectors.OAIPMH.GenericRequest#startElement(java.lang.String,
     *      java.lang.String, java.lang.String, org.xml.sax.Attributes)
     */
    public void startElement(String namespaceURI, String localName,
        String qualifiedName, Attributes atts) throws SAXException {
        elementValue = "";

        if (qualifiedName.equalsIgnoreCase("Error")) {
            errorCode = atts.getValue("code");
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see org.xml.sax.helpers.DefaultHandler#endElement(java.lang.String,
     *      java.lang.String, java.lang.String)
     */
    public void endElement(String namespaceURI, String localName,
        String qualifiedName) throws SAXException {
        if (qualifiedName.equalsIgnoreCase("responseDate")) {
            responseDate = elementValue;
        } else if (qualifiedName.equalsIgnoreCase("error")) {
            errorDescription = elementValue;
            oaierror = true;
        }
    }

    /**
     * set the Option String
     */
    private void setOptions() {
        String opts = "";

        if (metadataPrefix != null) {
            opts += ("&metadataPrefix=" + metadataPrefix);
        }

        if (identifier != null) {
            opts += ("&identifier=" + identifier);
        }

        super.setOAIOptions(opts);
    }

    /**
     *
     * Return the OAI-PMH record identifier
     *
     * @return Record Identifier
     */
    public String getIdentifier() {
        return identifier;
    }

    /**
     *
     * Return the metadata format requested to the repository
     *
     * @return Metadata Prefix
     */
    public String getMetadataPrefix() {
        return metadataPrefix;
    }

    /**
     *
     * Return date and time of the repository response
     *
     * @return Date and time
     */
    public String getResponseDate() {
        return responseDate;
    }

    /**
     *
     * Return a DublinCoreRecord object with DublinCore data
     *
     * @return the dublinCoreRecord
     */
    public DublinCoreRecord getDublinCoreRecord() {
        return dublinCoreRecord;
    }

    /**
    *
    * Return a TelAPRecord object with TEL-AP data
    *
    * @return the TelAPRecord
    */
   public TelAPRecord getTelAPRecord() {
       return telAPRecord;
   }
}
