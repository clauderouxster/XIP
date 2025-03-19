package eu.project.Cacao.Connectors.OAIPMH;

import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringReader;
import java.net.MalformedURLException;

import org.apache.log4j.Logger;
import org.xml.sax.Attributes;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.DefaultHandler;
import org.xml.sax.helpers.XMLReaderFactory;

import eu.project.Cacao.Harvester.OAIHarvestingAgent;


/**
 * Abstract class for generic OAI-PMH request.
 *
 * @author Igor Barsanti - <a href="http://www.gonetwork.it"
 *         target="_blank">Gonetwork srl</a>
 *
 */
public abstract class GenericRequest extends DefaultHandler {
	
    private String OAIverb;
    private String OAIOptions = "";
    protected String resumptionToken;
    protected String repositoryUrl;
    protected String response;
    protected String elementValue;
    protected String errorCode = "";
    protected String errorDescription = "";
    protected boolean oaierror = false;
    static final String prefix="xmlData/";
    static private int cnt=0;
	private Logger logger = Logger.getLogger(GenericRequest.class);

    /**
     *
     * Class constructor
     *
     * @param url
     *            OAI repository url
     * @param verb
     *            OAI-PMH verb param (ex. Identify, ListRecords...)
     */
    public GenericRequest(String url, String verb) {
        repositoryUrl = url;
        OAIverb = verb;
    }

    /**
     *
     * Send request to OAI repository, if OAI repository reply with
     * resumptionToken param, reply the request until all records are read.
     *
     * @throws OAIPMHErrorException
     *             If there's a error field in the server response
     * @throws OAIPMHXmlException
     *             If there's errors in XML response
     * @throws MalformedURLException
     *             If the repository URL isn't correct
     * @throws IOException
     *             If is impossible to contact the OAI repository (ex. network
     *             problem...)
     */
    public void sendRequest()  throws OAIPMHErrorException, OAIPMHXmlException, MalformedURLException,  IOException {
        String resumptionString = "";

        do {
            if (resumptionToken != null) {
                resumptionString = "&resumptionToken=" + resumptionToken;
                OAIOptions = "";
                logger.debug("Got a resumptionToken: " + resumptionToken + " i must get more data...");

                resumptionToken = null;
            } else {
                resumptionString = "";
            }

            try {
                String oaireq = "verb=" + OAIverb + resumptionString +  OAIOptions;
                HTTPRequest req = new HTTPRequest(oaireq, repositoryUrl);
                response = req.getResponse();
            } catch (MalformedURLException e) {
                throw (e);
            } catch (IOException e) {
                throw (e);
            }

            try {
                XMLReader parser = XMLReaderFactory.createXMLReader();
                parser.setContentHandler(this);
                parser.parse(new InputSource(new StringReader(response)));

                if (oaierror) {
                    throw (new OAIPMHErrorException(errorCode));
                }
            } catch (SAXException e) {
                throw (new OAIPMHXmlException("XML Error"));
            } catch (OAIPMHErrorException e) {
                throw (new OAIPMHErrorException(errorCode + ": " +
                    errorDescription));
            } catch (Exception e) {
            }
        } while (resumptionToken != null);
    }

    /**
    *
    * Send request to OAI repository restarting session 
    * from a provided resumptionToken 
    * 
    * @param token
    *			session resumptionToken
    *
    * @return String
    * 			ResumptionToken received from OAI-PMH provider 		
    *
    * @throws OAIPMHErrorException
    *             If there's a error field in the server response
    * @throws OAIPMHXmlException
    *             If there's errors in XML response
    * @throws MalformedURLException
    *             If the repository URL isn't correct
    * @throws IOException
    *             If is impossible to contact the OAI repository (ex. network
    *             problem...)
    */
   public String sendRequest(String resumptionToken)
			throws OAIPMHErrorException, OAIPMHXmlException,
			MalformedURLException, IOException {

		String resumptionString = "";

		if (resumptionToken != null) {
			resumptionString = "&resumptionToken=" + resumptionToken;
			OAIOptions = "";
			logger.debug("Set a resumptionToken: " + resumptionToken);
		}

		try {
			String oaireq = "verb=" + OAIverb + resumptionString + OAIOptions;
			HTTPRequest req = new HTTPRequest(oaireq, repositoryUrl);
			response = req.getResponse();
			//added to overcame db problems by saving xml to file...
			PrintWriter pw=new PrintWriter(new FileWriter(prefix+"tmp_"+cnt+".xml"));
			pw.println(response);
			pw.close();
			cnt++;
			
		} catch (MalformedURLException e) {
			throw (e);
		} catch (IOException e) {
			throw (e);
		}

		try {
			XMLReader parser = XMLReaderFactory.createXMLReader();
			parser.setContentHandler(this);
			parser.parse(new InputSource(new StringReader(response)));

			if (oaierror) {
				throw (new OAIPMHErrorException(errorCode));
			}
		} catch (SAXException e) {
			throw (new OAIPMHXmlException("XML Error"));
		} catch (OAIPMHErrorException e) {
			throw (new OAIPMHErrorException(errorCode + ": " + errorDescription));
		} catch (Exception e) {
		}

		return resumptionToken;
	}

    /*
	 * (non-Javadoc)
	 * 
	 * @see org.xml.sax.helpers.DefaultHandler#startElement(java.lang.String,
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
        if (qualifiedName.equalsIgnoreCase("resumptionToken")) {
            if (elementValue.length() > 0) {
                resumptionToken = elementValue;
            }
        } else if (qualifiedName.equalsIgnoreCase("error")) {
            errorDescription = elementValue;
            oaierror = true;
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see org.xml.sax.helpers.DefaultHandler#characters(char[], int, int)
     */
    public void characters(char[] ch, int start, int length) {
        for (int i = start; i < (start + length); i++) {
            elementValue += ch[i];
        }
    }

    /**
     * Return the URL of the OAI repository target of the request
     *
     * @return The OAI repository URL
     */
    public String getRepositoryUrl() {
        return repositoryUrl;
    }

    /**
     *
     * Return the complete XML response received from the OAI repository.
     *
     * @return The response received from the OAI server
     */
    public String getResponse() {
        return response;
    }

    /**
     *
     * In case of OAIPMHErrorException return the error code
     *
     * @return The OAI-PMH error code
     */
    public String getErrorCode() {
        return errorCode;
    }

    /**
     *
     * In case of OAIPMHErrorException return the error description
     *
     * @return The OAI-PMH error description
     */
    public String getErrorDescription() {
        return errorDescription;
    }

    /**
     *
     * Return the OAI options string that will be appended to query request,
     * after verb and (if necessary...) resumptionToken fields
     *
     * @return The OAI options string
     */
    public String getOAIOptions() {
        return OAIOptions;
    }

    /**
     *
     * Set the OAI-PMH options string that will be appended to query request,
     * after verb and resumptionToken (if necessary...) fields Must start with &
     * char.<br/> example: &matadataFormat=oai_dc&until=2006-05-01
     *
     * @param options
     *            The OAI-PMH options string
     */
    public void setOAIOptions(String options) {
        OAIOptions = options;
    }

	/**
	 * @return the resumptionToken
	 */
	public String getResumptionToken() {
		return resumptionToken;
	}
}
