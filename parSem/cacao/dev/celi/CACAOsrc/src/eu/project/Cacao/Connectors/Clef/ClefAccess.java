/**
 *
 */
package eu.project.Cacao.Connectors.Clef;

import java.net.URL;
import java.util.Date;

import eu.project.Cacao.Connectors.DB.Library;
import eu.project.Cacao.Connectors.DB.LibraryCollection;
import eu.project.Cacao.Connectors.DB.LibraryRecord;
import eu.project.Cacao.Connectors.Metadata.DublinCoreRecord;
import eu.project.Cacao.Interfaces.CacaoDB;
import eu.project.Cacao.Interfaces.UnifiedRepositoryAccess;


/**
 *
 * Class for access CLEF 2008 data files.
 *
 * @author Igor Barsanti - <a href="http://www.gonetwork.it" target="_blank">Gonetwork Srl</a>
 *
 */
public class ClefAccess implements UnifiedRepositoryAccess {
    private CacaoDB cacaodb;

    public ClefAccess(CacaoDB db) {
        cacaodb = db;
    }

    /* (non-Javadoc)
     * @see eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#getDCMI_Type(java.net.URL)
     */
    public String getDCMI_Type(URL resourceURL) {
        return null;
    }

    /* (non-Javadoc)
     * @see eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#getDC_Format(java.net.URL)
     */
    public String getDC_Format(URL resourceURL) {
        // TODO Auto-generated method stub
        return null;
    }

    /* (non-Javadoc)
     * @see eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveAllTextFromRecord(java.lang.String)
     */
    public String retrieveAllTextFromRecord(String recordID) {
        LibraryRecord record = cacaodb.getRecordByLibraryID(recordID);

        if (record != null) {
            return record.getDublinCoreData().getDublinCoreString();
        }

        return null;
    }

    /* (non-Javadoc)
     * @see eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveCollection(java.lang.String)
     */
    public LibraryCollection retrieveCollection(String collectionID) {
        LibraryCollection collection = cacaodb.getCollection(collectionID);

        if (collection != null) {
            return collection;
        }

        return null;
    }

    /* (non-Javadoc)
     * @see eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveCollectionsID(java.lang.String)
     */
    public String[] retrieveCollectionsID(String libraryID) {
        Library library = cacaodb.getLibrary(libraryID);

        if (library != null) {
            return retrieveCollectionsID(library);
        }

        return null;
    }

    /* (non-Javadoc)
     * @see eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveCollectionsID(eu.project.Cacao.Connectors.DB.Library)
     */
    public String[] retrieveCollectionsID(Library library) {
        return cacaodb.getAllCollections(library);
    }

    /* (non-Javadoc)
     * @see eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveDeletedRecordsID(java.lang.String)
     */
    public String[] retrieveDeletedRecordsID(String libraryID) {
        // TODO Auto-generated method stub
        return null;
    }

    /* (non-Javadoc)
     * @see eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveDeletedRecordsID(eu.project.Cacao.Connectors.DB.Library)
     */
    public String[] retrieveDeletedRecordsID(Library library) {
        // TODO Auto-generated method stub
        return null;
    }

    /* (non-Javadoc)
     * @see eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveDeletedRecordsID(java.lang.String, java.lang.String)
     */
    public String[] retrieveDeletedRecordsID(String libraryID,
        String collectionID) {
        // TODO Auto-generated method stub
        return null;
    }

    /* (non-Javadoc)
     * @see eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveDeletedRecordsID(eu.project.Cacao.Connectors.DB.Library, eu.project.Cacao.Connectors.DB.LibraryCollection)
     */
    public String[] retrieveDeletedRecordsID(Library library,
        LibraryCollection collection) {
        // TODO Auto-generated method stub
        return null;
    }

    /* (non-Javadoc)
     * @see eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveLibrariesID()
     */
    public String[] retrieveLibrariesID() {
        return cacaodb.getAllLibraries();
    }

    /* (non-Javadoc)
     * @see eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveLibrary(java.lang.String)
     */
    public Library retrieveLibrary(String libraryID) {
        return cacaodb.getLibrary(libraryID);
    }

    /* (non-Javadoc)
     * @see eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveMetadata(java.lang.String)
     */
    public DublinCoreRecord retrieveMetadata(String recordID) {
        LibraryRecord record = cacaodb.getRecordByLibraryID(recordID);

        if (record != null) {
            return record.getDublinCoreData();
        }

        return null;
    }

    /* (non-Javadoc)
     * @see eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveRecord(java.lang.String)
     */
    public LibraryRecord retrieveRecord(String recordID, LibraryCollection coll) {
        LibraryRecord record = cacaodb.getRecordByLibraryID(recordID);

        if (record != null) {
            return record;
        }

        return null;
    }

    /* (non-Javadoc)
     * @see eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveRecordsID(java.lang.String)
     */
    public String[] retrieveRecordsID(String collectionID) {
        LibraryCollection collection = cacaodb.getCollection(collectionID);

        if (collection != null) {
            retrieveRecordsID(collection);
        }

        return null;
    }

    /* (non-Javadoc)
     * @see eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveRecordsID(java.lang.String, java.util.Date, java.util.Date)
     */
    public String[] retrieveRecordsID(String collectionID, Date from, Date to) {
        // TODO Auto-generated method stub
        return null;
    }

    /* (non-Javadoc)
     * @see eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveRecordsID(eu.project.Cacao.Connectors.DB.LibraryCollection)
     */
    public String[] retrieveRecordsID(LibraryCollection collection) {
        return cacaodb.getAllRecords(collection.getLibrary(), collection);
    }

    /* (non-Javadoc)
     * @see eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveRecordsID(eu.project.Cacao.Connectors.DB.LibraryCollection, java.util.Date, java.util.Date)
     */
    public String[] retrieveRecordsID(LibraryCollection collection, Date from,
        Date to) {
        // TODO Auto-generated method stub
        return null;
    }

    /* (non-Javadoc)
     * @see eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveResourceURL(java.lang.String)
     */
    public URL retrieveResourceURL(String recordID) {
        return null;
    }

    /* (non-Javadoc)
     * @see eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveResourceURL(eu.project.Cacao.Connectors.DB.LibraryRecord)
     */
    public URL retrieveResourceURL(LibraryRecord record) {
        // TODO Auto-generated method stub
        return null;
    }

    /* (non-Javadoc)
     * @see eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveUserLogs(java.lang.String, java.util.Date, java.util.Date)
     */
    public String[] retrieveUserLogs(String libraryID, Date from, Date to) {
        // TODO Auto-generated method stub
        return null;
    }

    /* (non-Javadoc)
     * @see eu.project.Cacao.Interfaces.UnifiedRepositoryAccess#retrieveUserLogs(eu.project.Cacao.Connectors.DB.Library, java.util.Date, java.util.Date)
     */
    public String[] retrieveUserLogs(Library library, Date from, Date to) {
        // TODO Auto-generated method stub
        return null;
    }
}
