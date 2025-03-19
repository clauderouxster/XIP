package eu.project.Cacao.Connectors.Metadata;

/**
 * 
 * Exception for XML error in TELAP data 
 * 
 * @author Igor Barsanti - <a href="http://www.gonetwork.it" target="_blank">Gonetwork Srl</a>
 *
 */
public class TELAPXmlException extends Exception {

	public TELAPXmlException() {}

	public TELAPXmlException(String message) {
		super(message);
	}

	public TELAPXmlException(Throwable cause) {
		super(cause);
	}

	public TELAPXmlException(String message, Throwable cause) {
		super(message, cause);
	}
}
