/**
 *
 */
package eu.project.Cacao.Connectors.OAIPMH;

import java.util.Hashtable;

import org.xml.sax.SAXException;


/**
 *
 * Class for ListIdentifiers OAI-PMH request.
 *
 * @author Igor Barsanti - <a href="http://www.gonetwork.it"
 *         target="_blank">Gonetwork Srl</a>
 *
 */
public class ListIdentifiers extends GenericRequest {
    private String from;
    private String until;
    private String metadataPrefix;
    private String set;
    private String responseDate;
    private Hashtable<String, String> identifiers = new Hashtable<String, String>();
    private String idName;
    private String idDate;

    /**
     *
     * Class constructor
     *
     * @param url
     *            The OAI-PMH Repository Url
     * @param metadataPrefix
     *            Record's identifiers preferred metadata format
     */
    public ListIdentifiers(String url, String metadataPrefix) {
        super(url, "ListIdentifiers");
        this.metadataPrefix = metadataPrefix;
        setOptions();
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
        }

        if (qualifiedName.equalsIgnoreCase("identifier")) {
            idName = elementValue;

            if (idDate != null) {
                identifiers.put(idName, idDate);
                idName = null;
                idDate = null;
            }
        }

        if (qualifiedName.equalsIgnoreCase("datestamp")) {
            idDate = elementValue;

            if (idName != null) {
                identifiers.put(idName, idDate);
                idName = null;
                idDate = null;
            }
        } else if (qualifiedName.equalsIgnoreCase("error")) {
            errorDescription = elementValue;
            oaierror = true;
        }
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

        if (from != null) {
            opts += ("&from=" + from);
        }

        if (until != null) {
            opts += ("&until=" + until);
        }

        if (metadataPrefix != null) {
            opts += ("&metadataPrefix=" + metadataPrefix);
        }

        if (set != null) {
            opts += ("&set=" + from);
        }

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
     * Return a Hashtable with identifiers received from the OAI repository. The
     * key of the Hashtable is the identifier, the value is the timestamp
     *
     * @return Identifiers
     */
    public Hashtable<String, String> getIdentifiers() {
        return identifiers;
    }
}
