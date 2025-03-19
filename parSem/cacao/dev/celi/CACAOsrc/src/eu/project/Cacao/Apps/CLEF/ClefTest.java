package eu.project.Cacao.Apps.CLEF;

import eu.project.Cacao.Connectors.Clef.ClefAccess;
import eu.project.Cacao.Connectors.DB.Library;
import eu.project.Cacao.Connectors.DB.LibraryCollection;
import eu.project.Cacao.Connectors.DB.LibraryRecord;
import eu.project.Cacao.Connectors.DB.MySQL;
import eu.project.Cacao.Connectors.Metadata.DublinCoreRecord;


/**
 *
 * Test ClefAccess object
 *
 * @author Igor Barsanti - <a href="http://www.gonetwork.it" target="_blank">Gonetwork Srl</a>
 *
 */
public class ClefTest {
    public static void main(String[] args) {
        // Connect to DB
        MySQL cacaodb = new MySQL();
        cacaodb.setServer("localhost");
        cacaodb.setDBname("CacaoProject");
        cacaodb.setUsername("cacao");
        cacaodb.setPassword("cacaopwd");
        cacaodb.connect();

        // Create new ClefAccess Object
        ClefAccess clef = new ClefAccess(cacaodb);

        // Retrieve all libraries
        String[] libraries = clef.retrieveLibrariesID();

        // Retrieve all collections
        String[] collections = null;

        for (int lib = 0; lib < libraries.length; lib++) {
            Library library = clef.retrieveLibrary(libraries[lib]);
            collections = clef.retrieveCollectionsID(library);
        }

        // Check all connection's records
        String[] records = null;

        for (int coll = 0; coll < collections.length; coll++) {
            LibraryCollection collection = clef.retrieveCollection(collections[coll]);

            // Retrieve all records of Collection
            records = clef.retrieveRecordsID(collection);

            System.out.println("Retrieve records from Collection: " +
                collection.getDescription() + "(Library: " +
                collection.getLibrary().getName() + ")");

            // For every record fetched...
            for (int r = 0; r < records.length; r++) {
                // Get LibraryRecord object
                LibraryRecord record = clef.retrieveRecord(records[r], null);

                // Extract DublinCoreRecord Object with DublinCore data 				
                DublinCoreRecord dcdata = record.getDublinCoreData();

                // Print data to output
                if (!dcdata.getTitle().isEmpty()) {
                    String title = dcdata.getTitle().firstElement();
                    System.out.println("Record ID " + record.getDatabaseID() +
                        " Title: " +
                        title.substring(0,
                            ((title.length() > 40) ? 39 : title.length())) +
                        "...");
                }
            }

            System.out.println("Retrieved " + records.length + " records\n");
        }
    }
}
