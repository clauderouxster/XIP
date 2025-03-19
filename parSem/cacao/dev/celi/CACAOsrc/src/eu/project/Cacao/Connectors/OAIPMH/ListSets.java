package eu.project.Cacao.Connectors.OAIPMH;

import java.util.Hashtable;

import org.xml.sax.SAXException;


/**
 *
 * Class for ListSets OAI-PMH request.
 *
 * @author Igor Barsanti - <a href="http://www.gonetwork.it"
 *         target="_blank">Gonetwork Srl</a>
 *
 */
public class ListSets extends GenericRequest {
    private String responseDate;
    private String setSpec;
    private String setName;
    private Hashtable<String, String> sets = new Hashtable<String, String>();

    /**
     * Class constructor
     *
     * @param url
     *            The OAI-PMH Repository Url
     */
    public ListSets(String url) {
        super(url, "ListSets");

        // TODO Auto-generated constructor stub
    }

    /*
     * (non-Javadoc)
     *
     * @see org.xml.sax.helpers.DefaultHandler#endElement(java.lang.String,
     *      java.lang.String, java.lang.String)
     */
    public void endElement(String namespaceURI, String localName,
        String qualifiedName) throws SAXException {
        if (qualifiedName.equalsIgnoreCase("resumptionToken")) {
            if (elementValue.length() > 0) {
                resumptionToken = elementValue;
            }
        }

        if (qualifiedName.equalsIgnoreCase("responseDate")) {
            responseDate = elementValue;
        }

        if (qualifiedName.equalsIgnoreCase("setSpec")) {
            setSpec = elementValue;

            if (setName != null) {
                sets.put(setSpec, setName);
                setSpec = null;
                setName = null;
            }
        }

        if (qualifiedName.equalsIgnoreCase("setName")) {
            setName = elementValue;

            if (setSpec != null) {
                sets.put(setSpec, setName);
                setSpec = null;
                setName = null;
            }
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
     * Return a Hashtable with Sets received from the OAI repository. The key of
     * the Hashtable is the SetSpec, the value is the Set Name
     *
     * @return Sets
     */
    public Hashtable<String, String> getSets() {
        return sets;
    }
}
