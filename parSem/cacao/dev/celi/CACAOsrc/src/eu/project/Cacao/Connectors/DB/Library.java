/**
 *
 */
package eu.project.Cacao.Connectors.DB;


/**
 *
 * Class for store Library information.
 *
 * @author Igor Barsanti - <a href="http://www.gonetwork.it" target="_blank">Gonetwork Srl</a>
 *
 */
public class Library {
    private String ID;
    private String name;
    private String[] languages;
    private String URL;
    private String protocol;

    /**
     * Class constructor, mainly used from DB connectors classes for create a
     * Library object from data retrieved from DataBase
     *
     * @param ID ID
     * @param name Name of Library
     * @param languages Languages used by Library
     * @param URL URL of the Library
     * @param protocol Protocol for accessing data
     */
    public Library(String ID, String name, String[] languages, String URL,
        String protocol) {
        super();
        this.ID = ID;
        this.name = name;
        this.languages = languages;
        this.URL = URL;
        this.protocol = protocol;
    }

    /**
     * Class constructor
     *
     * @param name Name of Library
     * @param languages Languages used by Library
     * @param URL URL of the Library
     * @param protocol Protocol for accessing data
     */
    public Library(String name, String[] languages, String URL, String protocol) {
        super();
        this.name = name;
        this.languages = languages;
        this.URL = URL;
        this.protocol = protocol;
    }

    /**
     * Return ID of the Library<br>
     * NOTE: ID are assigned only when Library object is saved to DataBase, or calling setID() method.
     *
     * @return the iD
     */
    public String getID() {
        return ID;
    }

    /**
     * Set the ID of the Library
     *
     * @param id the iD to set
     */
    public void setID(String id) {
        ID = id;
    }

    /**
     * Return name of the Library
     *
     * @return the name
     */
    public String getName() {
        return name;
    }

    /**
     * Set the name of the Library
     *
     * @param name the name to set
     */
    public void setName(String name) {
        this.name = name;
    }

    /**
     * Return languages used by the Library
     * @return the languages
     */
    public String[] getLanguages() {
        return languages;
    }

    /**
     * Set languages used by the Library
     * @param languages the languages to set
     */
    public void setLanguages(String[] languages) {
        this.languages = languages;
    }

    /**
     * Return the URL used by the Library
     * @return the URL
     */
    public String getURL() {
        return URL;
    }

    /**
     * Set the URL used by the Library
     * @param url the uRL to set
     */
    public void setURL(String url) {
        URL = url;
    }

    /**
     * Return the protocol used for read data
     * @return the protocol
     */
    public String getProtocol() {
        return protocol;
    }

    /**
     * Set the protocol used for read data
     * @param protocol the protocol to set
     */
    public void setProtocol(String protocol) {
        this.protocol = protocol;
    }

    /** Return the ID as int instead as a String
     *
     * @return ID the Library ID
     */
    public int getIDasInteger() {
        return Integer.parseInt(ID);
    }
}
