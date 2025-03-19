package eu.project.Cacao.Connectors.OAIPMH;

import java.util.Vector;

import org.xml.sax.SAXException;


/**
 *
 * Class for ListIdentifiers OAI-PMH request.
 *
 * @author Igor Barsanti - <a href="http://www.gonetwork.it"
 *         target="_blank">Gonetwork Srl</a>
 *
 */
public class ListMetadataFormats extends GenericRequest {
    private String responseDate;
    private Vector<String> metadataFormats = new Vector<String>();

    /**
     * Class constructor
     *
     * @param url The OAI-PMH Repository Url
     */
    public ListMetadataFormats(String url) {
        super(url, "ListMetadataFormats");
    }

    /* (non-Javadoc)
     * @see org.xml.sax.helpers.DefaultHandler#endElement(java.lang.String, java.lang.String, java.lang.String)
     */
    public void endElement(String namespaceURI, String localName,
        String qualifiedName) throws SAXException {
        if (qualifiedName.equalsIgnoreCase("responseDate")) {
            responseDate = elementValue;
        } else if (qualifiedName.equalsIgnoreCase("metadataPrefix")) {
            metadataFormats.add(elementValue);
        } else if (qualifiedName.equalsIgnoreCase("error")) {
            errorDescription = elementValue;
            oaierror = true;
        }
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
     * Return a Vector with the metadata formats
     * supported by the OAI repository
     *
     * @return metadata formats
     */
    public Vector<String> getMetadataFormats() {
        return metadataFormats;
    }
}
