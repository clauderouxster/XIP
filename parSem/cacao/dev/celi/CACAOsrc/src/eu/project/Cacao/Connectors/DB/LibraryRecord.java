/**
 *
 */
package eu.project.Cacao.Connectors.DB;

import eu.project.Cacao.Connectors.Metadata.DublinCoreRecord;
import eu.project.Cacao.Connectors.Metadata.TelAPRecord;
import eu.project.Cacao.Interfaces.CacaoDB;

/**
 * Class for store Record information
 *
 * @author Igor Barsanti - <a href="http://www.gonetwork.it" target="_blank">Gonetwork Srl</a>
 *
 */
public class LibraryRecord {
    private CacaoDB cacaodb;
    private String ID;
    private String library_id;
    private String collection_id;
    private Library library;
    private LibraryCollection collection;
    private DublinCoreRecord DublinCoreData;
    private TelAPRecord TelAPData;
    private String lastUpdate;
    private String[] multiCollection = new String[99];
    private boolean deleted;
    private boolean collectionMustFetched;
    
    /**
     * Class constructor , mainly used from DB connectors classes for create a
     * LibraryRecord object from data retrieved from DataBase
     *
     * @param id        Record ID
     * @param library_id        ID of the Library that contains this record
     * @param collection_id ID of the Collection that contains this record
     * @param dublinCoreData DublinCoreRecord object with DublinCore information
     * @param TelAPData TelAPRecord object with TEL-AP information
     * @param lastUpdate last update timestamp
     * @param deleted true if record is marked as deleted
     * @param db CacaoDB object
     */
    public LibraryRecord(String id, String library_id, String collection_id,
        DublinCoreRecord dublinCoreData, TelAPRecord telAPData, String lastUpdate, boolean deleted,
        String strMultiCollection, CacaoDB db) {
        super();
        ID = id;
        this.library_id = library_id;
        this.library = db.getLibrary(library_id);
        this.collection_id = collection_id;
        this.collection = db.getCollection(collection_id);
        DublinCoreData = dublinCoreData;
        TelAPData = telAPData;
        this.lastUpdate = lastUpdate;
        this.deleted = deleted;
        collectionMustFetched = true;
        multiCollection = strMultiCollection.split("<");
        this.cacaodb = db;
    }

    /**
     * Class constructor
     *
     * @param id        Record ID
     * @param library Library that contains this record
     * @param collection Collection that contains this record
     * @param dublinCoreData DublinCoreRecord object with DublinCore information
     * @param TelAPData TelAPRecord object with TEL-AP information
     * @param lastUpdate last update timestamp
     * @param deleted true if record is marked as deleted
     */
    public LibraryRecord(String record_id, Library library, LibraryCollection collection,
        DublinCoreRecord dublinCoreData, TelAPRecord telAPData, String lastUpdate, boolean deleted) {
        super();
        this.ID = record_id;
        this.library = library;
        this.collection = collection;
        DublinCoreData = dublinCoreData;
        TelAPData = telAPData;
        this.lastUpdate = lastUpdate;
        this.deleted = deleted;
        collectionMustFetched = false;
    }

    /**
     * Return Record ID
     * NOTE: ID are assigned only when LibraryRecord object is saved to DataBase,
     * or calling setID() method.
     *
     * @return the ID
     */
    public String getDatabaseID() {
        return ID;
    }
    
    /**
     * Return Record ID
     * NOTE: ID are assigned only when LibraryRecord object is saved to DataBase,
     * or calling setID() method.
     *
     * @return the ID
     */
    public String getLibraryID() {
        if(this.DublinCoreData != null)
        	return this.DublinCoreData.getRecordID();
        else if(this.TelAPData != null)
        	return this.TelAPData.getRecordID();
        else
        	return null;
    }

    /**
     * Set the Record ID
     *
     * @param id the ID to set
     */
    public void setID(String id) {
        ID = id;
    }

    /**
     * Return Library object of the library that contains this Record
     *
     * @return the library
     */
    public Library getLibrary() {
        if (collectionMustFetched) {
            this.collection = cacaodb.getCollection(this.collection_id);
            this.library = collection.getLibrary();
            collectionMustFetched = false;
        }

        return library;
    }

    /**
     * Set Library object with the Library that contains that Record
     *
     * @param library Library to set
     */
    public void setLibrary(Library library) {
        this.library = library;
    }

    /**
     * Return LibraryCollection object of the collection that contains this Record
     *
     * @return the collection
     */
    public LibraryCollection getCollection() {
        if (collectionMustFetched) {
            this.collection = cacaodb.getCollection(this.collection_id);
            this.library = collection.getLibrary();
            collectionMustFetched = false;
        }

        return collection;
    }

    /**
     * Set LibraryCollection object with the collection that contains this Record
     *
     * @param collection Collection to set
     */
    public void setCollection(LibraryCollection collection) {
        this.collection = collection;
    }

    /**
     * Return DublinCore data from Record
     *
     * @return the dublinCoreData
     */
    public DublinCoreRecord getDublinCoreData() {
        return DublinCoreData;
    }

    /**
     * Set DublinCore data of the Record
     *
     * @param dublinCoreData the dublinCoreData to set
     */
    public void setDublinCoreData(DublinCoreRecord dublinCoreData) {
        DublinCoreData = dublinCoreData;
    }

    /**
     * Return TelAP data from Record
     *
     * @return the TelAP Data
     */
    public TelAPRecord getTelAPData() {
        return TelAPData;
    }

    /**
     * Set DublinCore data of the Record
     *
     * @param dublinCoreData the dublinCoreData to set
     */
    public void setTelAPData(TelAPRecord telAPData) {
        TelAPData = telAPData;
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
     * Return TRUE if Record is marked as Deleted
     *
     * @return the deleted
     */
    public boolean getDeleted() {
        return deleted;
    }

    /**
     * Set Record Deleted status
     *
     * @param deleted the deleted status to set
     */
    public void setDeleted(boolean deleted) {
        this.deleted = deleted;
    }

	/**
	 * @return the multiCollection
	 */
	public String[] getMultiCollection() {
		return multiCollection;
	}

	/**
	 * @param multiCollection the multiCollection to set
	 */
	public void setMultiCollection(String[] multiCollection) {
		this.multiCollection = multiCollection;
	}
	
	public boolean conformsToTelApMetaData(){
		if(this.TelAPData!=null)
			return true;
		else
			return false;
	}
	
	public boolean conformsToDCMetaData(){
		if(this.DublinCoreData!=null)
			return true;
		else
			return false;
	}
	
}
