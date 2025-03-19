package eu.project.Cacao.Connectors.OAIPMH;


/**
 *
 * Exception for OAI-PMH error
 *
 * @author Igor Barsanti - <a href="http://www.gonetwork.it" target="_blank">Gonetwork Srl</a>
 *
 */
public class OAIPMHErrorException extends Exception {
    public OAIPMHErrorException() {
    }

    public OAIPMHErrorException(String message) {
        super(message);
    }

    public OAIPMHErrorException(Throwable cause) {
        super(cause);
    }

    public OAIPMHErrorException(String message, Throwable cause) {
        super(message, cause);
    }
}
