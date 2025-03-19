package eu.project.Cacao.Connectors.OAIPMH;


/**
 *
 * Exception for OAI-PMH errors in the XML response
 *
 * @author Igor Barsanti - <a href="http://www.gonetwork.it" target="_blank">Gonetwork Srl</a>
 *
 */
public class OAIPMHXmlException extends Exception {
    public OAIPMHXmlException() {
    }

    public OAIPMHXmlException(String message) {
        super(message);
    }

    public OAIPMHXmlException(Throwable cause) {
        super(cause);
    }

    public OAIPMHXmlException(String message, Throwable cause) {
        super(message, cause);
    }
}
