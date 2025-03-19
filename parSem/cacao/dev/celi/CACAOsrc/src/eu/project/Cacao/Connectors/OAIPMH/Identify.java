/**
 *
 */
package eu.project.Cacao.Connectors.OAIPMH;

import java.util.Vector;

import org.xml.sax.SAXException;


/**
 *
 * Class for Identify OAI-PMH request.
 *
 * @author Igor Barsanti - <a href="http://www.gonetwork.it"
 *         target="_blank">Gonetwork Srl</a>
 *
 */
public class Identify extends GenericRequest {
    private String repositoryName;
    private String baseURL;
    private String protocolVersion;
    private String earliestDatestamp;
    private String deletedRecord;
    private String granularity;
    private Vector<String> adminEmail = new Vector<String>();
    private Vector<String> compression = new Vector<String>();
    private Vector<String> description = new Vector<String>();

    /**
     *
     * Class constructor
     *
     * @param url
     *            The OAI-PMH Repository Url
     */
    public Identify(String url) {
        super(url, "Identify");
    }

    /*
     * (non-Javadoc)
     *
     * @see org.xml.sax.helpers.DefaultHandler#endElement(java.lang.String,
     *      java.lang.String, java.lang.String)
     */
    public void endElement(String namespaceURI, String localName,
        String qualifiedName) throws SAXException {
        if (qualifiedName.equalsIgnoreCase("repositoryName")) {
            repositoryName = elementValue;
        } else if (qualifiedName.equalsIgnoreCase("baseURL")) {
            baseURL = elementValue;
        } else if (qualifiedName.equalsIgnoreCase("protocolVersion")) {
            protocolVersion = elementValue;
        } else if (qualifiedName.equalsIgnoreCase("earliestDatestamp")) {
            earliestDatestamp = elementValue;
        } else if (qualifiedName.equalsIgnoreCase("deletedRecord")) {
            deletedRecord = elementValue;
        } else if (qualifiedName.equalsIgnoreCase("granularity")) {
            granularity = elementValue;
        } else if (qualifiedName.equalsIgnoreCase("adminEmail")) {
            adminEmail.add(elementValue);
        } else if (qualifiedName.equalsIgnoreCase("compression")) {
            compression.add(elementValue);
        } else if (qualifiedName.equalsIgnoreCase("description")) {
            description.add(elementValue);
        } else if (qualifiedName.equalsIgnoreCase("error")) {
            errorDescription = elementValue;
            oaierror = true;
        }
    }

    /**
     *
     * Return the repository name from the XML response
     *
     * @return repository name
     */
    public String getRepositoryName() {
        return repositoryName;
    }

    /**
     * Return the base URL from the XML response
     *
     * @return Base URL
     */
    public String getBaseURL() {
        return baseURL;
    }

    /**
     *
     * Return the Protocol Version from the XML response
     *
     * @return protocol version
     */
    public String getProtocolVersion() {
        return protocolVersion;
    }

    /**
     * Return the earliest datestamp of the repository, that is the guaranteed
     * lower limit of all datestamps
     *
     * @return earliest datestamp
     */
    public String getEarliestDatestamp() {
        return earliestDatestamp;
    }

    /**
     * Return the manner in which the repository supports the notion of deleted
     * records.
     *
     * @return delete record
     */
    public String getDeletedRecord() {
        return deletedRecord;
    }

    /**
     *
     * Return the finest harvesting granularity supported by the repository.
     *
     * @return granularity
     */
    public String getGranularity() {
        return granularity;
    }

    /**
     *
     * Return the emails of the repository administrators listed in the XML
     * response
     *
     * @return emails
     */
    public Vector<String> getAdminEmail() {
        return adminEmail;
    }

    /**
     * Return the compression encodings supported by the repository
     *
     * @return compression encodings
     */
    public Vector<String> getCompression() {
        return compression;
    }

    /**
     * Return the description of the repository
     *
     * @return description
     */
    public Vector<String> getDescription() {
        return description;
    }
}
