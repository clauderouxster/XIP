package xip.ws.server;

/**
 *
 * @author grondeau
 */
public class XipWebService {

    /**
     *
     * @return the list of language supported by the parser
     */
    public String getSupportedLanguage() {
        XipServer server = XipServer.getInstance();
        return server.getSupportedLanguage();
    }

    /**
     *
     * @param input The input to parse (in XML format) 
     * @param language must be one of the language returned by getSupportedLanguage()
     * @return the output of the parser (in XML format)
     */
    public String parseString(String input, String language) {
        XipServer server = XipServer.getInstance();
        return server.parseString(input, language);
    }

    /**
     * to send to the server the total client response time 
     * @param time
     */
    public void clientTotalResponseTime(long time) {
        XipServer server = XipServer.getInstance();
        server.clientTotalResponseTime(time);
    }
}
