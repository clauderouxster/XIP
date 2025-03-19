package eu.project.Cacao.Connectors.Metadata;


/**
 *
 * Exception for XML error in DublinCore data
 *
 * @author Igor Barsanti - <a href="http://www.gonetwork.it" target="_blank">Gonetwork Srl</a>
 *
 */
public class DCXmlException extends Exception {
    public DCXmlException() {
    }

    public DCXmlException(String message) {
        super(message);
    }

    public DCXmlException(Throwable cause) {
        super(cause);
    }

    public DCXmlException(String message, Throwable cause) {
        super(message, cause);
    }
}
