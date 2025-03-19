/**
 *
 */
package eu.project.Cacao.Connectors.DB;

import eu.project.Cacao.Interfaces.CacaoDB;


/**
 *
 * Class for store Collection information.
 *
 * @author Igor Barsanti - <a href="http://www.gonetwork.it" target="_blank">Gonetwork Srl</a>
 *
 */
public class LibraryCollection {
    private CacaoDB cacaodb;
    private String ID;
    private Library library;
    private String library_id;
    private String description;
    private String code;
    private String lastUpdate;
    private boolean libraryMustFetched;

    /**
     * Class constructor , mainly used from DB connectors classes for create a
     * LibraryCollection object from data retrieved from DataBase
     *
     * @param ID        The Collection ID
     * @param library_id        The Library ID of the collection's Library
     * @param description        Description of Collection
     * @param code        Code of the collection
     * @param lastUpdate last update timestamp
     * @param db CacaoDB object
     */
    public LibraryCollection(String ID, String library_id, String description,
        String code, String lastUpdate, CacaoDB db) {
        super();
        this.ID = ID;
        this.library_id = library_id;
        this.description = description;
        this.code = code;
        this.lastUpdate = lastUpdate;
        this.libraryMustFetched = true;
        this.cacaodb = db;
    }

    /**
     * Class Constructor
     *
     * @param library Collection's Library
     * @param description Description fo Collection
     * @param code        Code of the collection
     * @param lastUpdate last update timestamp
     *
     */
    public LibraryCollection(Library library, String description, String code,
        String lastUpdate) {
        super();
        this.library = library;
        this.description = description;
        this.code = code;
        this.lastUpdate = lastUpdate;
        this.libraryMustFetched = false;
    }

    /**
     * Return Library of the Collection
     *
     * @return the library
     */
    public Library getLibrary() {
        if (libraryMustFetched) {
            this.library = cacaodb.getLibrary(library_id);
            libraryMustFetched = false;
        }

        return library;
    }

    /**
     * Set the Library that contains this collection
     *
     * @param library the library to set
     */
    public void setLibrary(Library library) {
        this.library = library;
    }

    /**
     * Return Description of Collection
     *
     * @return the description
     */
    public String getDescription() {
        return description;
    }

    /**
     * Set Description of Collection
     *
     * @param description the description to set
     */
    public void setDescription(String description) {
        this.description = description;
    }

    /**
     * Return code of the collection
     *
     * @return the code
     */
    public String getCode() {
        return code;
    }

    /**
     * Set code of the collection
     *
     * @param code the code to set
     */
    public void setCode(String code) {
        this.code = code;
    }

    /**
     * Return last update timestamp
     *
     * @return the lastUpdate
     */
    public String getLastUpdate() {
        return lastUpdate;
    }

    /**
     * Set last update timestamp
     *
     * @param lastUpdate the lastUpdate to set
     */
    public void setLastUpdate(String lastUpdate) {
        this.lastUpdate = lastUpdate;
    }

    /**
     * Return ID of the collection
     * NOTE: ID are assigned only when LibraryCollection object is saved to DataBase,
     * or calling setID() method.
     *
     * @return Collection ID
     */
    public String getID() {
        return ID;
    }

    /**
     * Set the ID of the Collection
     *
     * @param id the ID to set
     */
    public void setID(String id) {
        ID = id;
    }

    /** Return the ID as int instead as a String
     *
     *@return ID the ID of Collection
     */
    public int getIDasInteger() {
        return Integer.parseInt(ID);
    }
}
