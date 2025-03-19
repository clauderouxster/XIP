/**
 *
 */
package eu.project.Cacao.Interfaces;

import java.util.Hashtable;
import java.util.Vector;

import eu.project.Cacao.Connectors.DB.Library;
import eu.project.Cacao.Connectors.DB.LibraryCollection;
import eu.project.Cacao.Connectors.DB.LibraryRecord;


/**
 *
 * Interface for DataBase access
 * For every different DataBase backend used developer's must
 * instantiate a correct implementation of this interface.
 *
 * @author Igor Barsanti - <a href="http://www.gonetwork.it" target="_blank">Gonetwork Srl</a>
 *
 */
public interface CacaoDB {
    /** Connect to database
     *
     * @return false in case of error
     */
    public boolean connect();

    /** Close connection to database
     *
     * @return false in case of error
     */
    public boolean close();

    /**
     * Add a Library to database
     *
     * @param library Library object to add
     *
     * @return false in case of error
     */
    public boolean addLibrary(Library library);

    /** Fetch a Library object from database
     *
     * @param ID ID of the Library
     *
     * @return Library object
     */
    public Library getLibrary(String ID);

    /**
     * Delete a Library from database
     * @param ID ID of the Library
     * @return false in case of error
     */
    public boolean deleteLibrary(String ID);

    /**
     * Add a collection to database
     *
     * @param collection LibraryCollection object
     * @return false in case of error
     */
    public boolean addCollection(LibraryCollection collection);

    /**
     * Fetch a LibraryCollection object from database
     *
     * @param ID ID of the collection
     * @return LibraryCollection object
     */
    public LibraryCollection getCollection(String ID);

    /**
     * Delete a collection from database
     * @param ID ID of the collection
     * @return false in case of error
     */
    public boolean deleteCollection(String ID);

    /**
     * Add a record to database
     *
     * @param record LibraryRecord object
     *
     * @return 1 in case of insert, 0 in case of update, -1 in case of error
     */
    public int addRecord(LibraryRecord record);

    /**
     * Fetch a Record from DataBase
     *
     * @param ID library ID of the record
     *
     * @return LibraryRecord object
     */
    public LibraryRecord getRecordByLibraryID(String ID);
    
    /**
     * Fetch a Record from DataBase
     *
     * @param ID database ID of the record
     *
     * @return LibraryRecord object
     */
    public LibraryRecord getRecordByDatabaseID(long ID);

  
    /**
     * Delete a Record from database
     *
     * @param ID ID of the Record
     *
     * @return false in case of error
     */
    public boolean deleteRecord(String ID);

    /**
     * Retrieve all records within a specified Library and Collection
     *
     * @param library Library object of the Library
     * @param collection LibraryCollection of the Collection
     *
     * @return Record's ID array
     */
    public String[] getAllRecords(Library library, LibraryCollection collection);
    
    /**
     * Retrieve all records within a specified Library 
     *
     * @param library Library object of the Library
     *
     * @return Record's ID array
     */
    public String[] getAllRecords(Library library);
    
    /** Retrieve all Collections of a Library
     *
     * @param library Library object of the Library
     *
     * @return Collection's ID array
     */
    public String[] getAllCollections(Library library);

    /**
     * Retrieve all Collections in database
     *
     * @return Collection's ID array
     */
    public String[] getAllCollections();

    /**
     * Retrieve all Libraries in database
     *
     * @return Library's ID array
     */
    public String[] getAllLibraries();

    /**
     * Return a String with values in Vector<String> v separated for
     * insert in a single DB field.
     *
     * @param v Vector with Strings
     *
     * @return Separated String
     */
    public String getDBSeparatedString(Vector<String> v);

    /**
     * Return a String with values in Vector<Hashtable<String,String>> v separated for
     * insert in a single DB field.
     *
     * @param v Vector with Hashtables<String,String>
     *
     * @return Separated String
     */
    public String getDBSeparatedHashString(Vector<Hashtable<String, String>> h);

    public String getValuesSeparator();

    public String getHashSeparator();
}
