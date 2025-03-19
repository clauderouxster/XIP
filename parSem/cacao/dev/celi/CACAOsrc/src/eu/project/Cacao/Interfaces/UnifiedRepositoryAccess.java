package eu.project.Cacao.Interfaces;

import java.net.URL;
import java.util.Date;

import eu.project.Cacao.Connectors.DB.Library;
import eu.project.Cacao.Connectors.DB.LibraryCollection;
import eu.project.Cacao.Connectors.DB.LibraryRecord;
import eu.project.Cacao.Connectors.Metadata.DublinCoreRecord;


/**
 * This interface should contain all functionalities related to harvesting metadata (and contents
 * if available) from libraries. The interface will be implemented by different �connectors�,
 * whose task is to mediate between the library standards (protocols and metadata) and CACAO.
 *
 * The records IDs returned by methods must be URI or OAI identifiers.
 *
 * The logs retrieved by retrieveUserLogs method consists of a timestamp followed by the log text.
 *
 * ?? Retrieving logs and related functionalities not needed for Tel@CLEF ??
 *
 * !! should be invoked by a managar class also charged to store the retrieved info in a DB !!
 * @author Alessio
 *
 */
public interface UnifiedRepositoryAccess {
    /**
     * Retrieve a String array with ID's of all Libraries
     * in the archive
     *
     * @return ID's of the retrieved Libraries
     */
    public String[] retrieveLibrariesID();

    /**
     * Retrieve a Library from archive
     *
     * @param libraryID The ID of the Library
     * @return The Library object with the library data
     */
    public Library retrieveLibrary(String libraryID);

    /**
     * Retrieve all collections ID of all collection
     * used in a library.
     *
     * @param libraryID ID of the Library
     *
     * @return ID's of the retrieved Collections
     */
    public String[] retrieveCollectionsID(String libraryID);

    /**
     * Retrieve all collections ID of all collection
     * used in a library.
     *
     * @param library The Library object of a Library
     * @return ID's of the retrieved Collections
     */
    public String[] retrieveCollectionsID(Library library);

    /**
     * Retrieve a collection from archive
     *
     * @param collectionID The ID of the collection
     *
     * @return The LibraryCollection object of the collection
     */
    public LibraryCollection retrieveCollection(String collectionID);

    /**
     * Retrieve all records in a collection
     *
     * @param collectionID The ID of the collection
     *
     * @return ID's of the retrieved Records
     */
    public String[] retrieveRecordsID(String collectionID);

    /**
     * Retrieve all records in a collection between two date
     * NOT YET IMPLEMENTED !!!
     *
     * @param collectionID The ID of the collection
     * @param from        start date
     * @param to end date
     *
     * @return ID's of the retrieved Records
     */
    public String[] retrieveRecordsID(String collectionID, Date from, Date to);

    /**
     * Retrieve all records in a collection
     *
     * @param collection The LibraryCollection object of the collection
     *
     * @return ID's of the retrieved Records
     */
    public String[] retrieveRecordsID(LibraryCollection collection);

    /**
     * Retrieve all records in a collection between two date
     * NOT YET IMPLEMENTED !!!
     *
     * @param collection The LibraryCollection object of the collection
     * @param from        start date
     * @param to end date
     *
     * @return ID's of the retrieved Records
     */
    public String[] retrieveRecordsID(LibraryCollection collection, Date from,
        Date to);

    /**
     * Retrieve a LibraryRecord Object with Record data
     *
     * @param recordID The ID of the Record
     * @param collection The library collection of the record
     *
     * @return LibraryRecord object
     */
    public LibraryRecord retrieveRecord(String recordID,
        LibraryCollection collection);

    /**
     * Retrieve deleted record from a Library
     *
     * @param libraryID The ID of the Library
     *
     * @return ID's of records marked as Deleted
     */
    public String[] retrieveDeletedRecordsID(String libraryID);

    /**
     * Retrieve deleted record from a Library
     *
     * @param library The Library object of a Library
     *
     * @return ID's of records marked as Deleted
     */
    public String[] retrieveDeletedRecordsID(Library library);

    /**
     * Retrieve deleted record from a Collection
     *
     * @param libraryID The ID of a Library
     * @param collectionID The ID of a Collection
     *
     * @return ID's of records marked as Deleted
     */
    public String[] retrieveDeletedRecordsID(String libraryID,
        String collectionID);

    /**
     * Retrieve deleted record from a Collection
     *
     * @param library The Library object of a Library
     * @param collection The LibraryCollection object of a Collection
     *
     * @return ID's of records marked as Deleted
     */
    public String[] retrieveDeletedRecordsID(Library library,
        LibraryCollection collection);

    /**
     * Retrieve XML record data
     *
     * @param recordID The record ID
     *
     * @return XML Record Data (for now DublinCore XML)
     */
    public String retrieveAllTextFromRecord(String recordID);

    /**
     * Return a DublinCoreRecord object with record dublin core data
     *
     * @param recordID Record ID
     *
     * @return        Dublin Core data
     */
    public DublinCoreRecord retrieveMetadata(String recordID);

    /**
     * NOT YET IMPLEMENTED
     *
     * @param recordID
     * @return
     */
    public URL retrieveResourceURL(String recordID);

    /**
     * NOT YET IMPLEMENTED
     *
     * @param record
     * @return
     */
    public URL retrieveResourceURL(LibraryRecord record);

    /**
     * NOT YET IMPLEMENTED
     *
     * @param libraryID
     * @param from
     * @param to
     * @return
     */
    public String[] retrieveUserLogs(String libraryID, Date from, Date to);

    /**
     * NOT YET IMPLEMENTED
     *
     * @param library
     * @param from
     * @param to
     * @return
     */
    public String[] retrieveUserLogs(Library library, Date from, Date to);

    /**
     * NOT YET IMPLEMENTED
     *
     * @param resourceURL
     * @return
     */
    public String getDCMI_Type(URL resourceURL);

    /**
     * NOT YET IMPLEMENTED
     *
     * @param resourceURL
     * @return
     */
    public String getDC_Format(URL resourceURL);
}
