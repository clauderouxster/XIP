package eu.project.Cacao.Apps.CLEF;

import java.io.File;
import java.util.Date;
import java.util.Iterator;
import java.util.TreeSet;

import eu.project.Cacao.Connectors.Clef.ClefData;
import eu.project.Cacao.Connectors.DB.Library;
import eu.project.Cacao.Connectors.DB.LibraryCollection;
import eu.project.Cacao.Connectors.DB.LibraryRecord;
import eu.project.Cacao.Connectors.DB.MySQL;
import eu.project.Cacao.Connectors.Metadata.DublinCoreRecord;

/**
 * Test apps for CLEF Harvesting
 * 
 * NOTE The default heap size of some JVM must be increased with -Xmx512m JVM
 * param
 * 
 * This app parse files from a directory, every subdir in the folder will be
 * used for collection name:
 * 
 * clefdatadir \___________Collection1 \___________file1.xml
 * \___________file2.xml \___________file3.xml \___________Collection2
 * \___________file4.xml \___________file5.xml
 * 
 * Records of file1.xml, file2.xml, file3.xml will be inserted in Collection1,
 * file4.xml and file5.xml in collection2
 * 
 * @author Igor Barsanti - <a href="http://www.gonetwork.it"
 *         target="_blank">Gonetwork Srl</a>
 * 
 */
public class ClefHarvester {
	// DB object
	private static MySQL cacaodb = new MySQL();

	// Some counters...
	private static int collections = 0;
	private static long records = 0;
	private static long badRecords = 0;

	public static void main(String[] args) {
		args = new String[] { "demo.celi.it", "cacaoUser", "cacao", "C:\\Bolzano", "uniBZ_smallTest" };

		// Check arguments
		if (args.length != 5) {
			System.out.println("Too few parameters only " + args.length + " over 4");
		} else {
			start(args);
		}
	}

	public static void start(String[] args) {
		// Start timestamp
		Date start = new Date();

		// Connect to DB
		cacaodb.setServer(args[0].trim());
		cacaodb.setDBname("CACAO");
		cacaodb.setUsername(args[1].trim());
		cacaodb.setPassword(args[2].trim());
		cacaodb.connect();
		String clefDataDir = args[3].trim();
		String libraryID = args[4].trim();

		System.out.println("Start collecting data from :" + clefDataDir);

		// List directories in clefDataDir
		// Every directory is a collection
		File folder = new File(clefDataDir);

		if (folder.exists()) {

			// Create main Library
			Library clefLibrary = new Library(libraryID, new String[0], "file:///", "Clef");

			// add library to DB
			cacaodb.addLibrary(clefLibrary);

			// Find subdirectories with Collection
			File[] listOfFiles = folder.listFiles();

			for (int i = 0; i < listOfFiles.length; i++) {
				if (listOfFiles[i].isDirectory()) {
					// Parse collection in subdirectory
					parseCollection(listOfFiles[i].getName(), listOfFiles[i].getAbsolutePath(), clefLibrary);
				}
			}

		} else {
			// Error message if folder is missing
			System.out.println("Missing folder " + clefDataDir);
		}

		// Report results...
		System.out.println("Done.");
		System.out.println("Harvested " + records + " records from " + collections + " collections.");

		if (badRecords > 0) {
			System.out.println("Found " + badRecords + " malformed XML records...");
		}

		Date end = new Date();
		long seconds = (end.getTime() - start.getTime()) / 1000;

		System.out.println("Enlapsed time " + seconds + " seconds\n");
	}

	// Find collections subdirectories
	private static void parseCollection(String name, String dir, Library library) {
		System.out.println("Read clef files from " + dir + " ...");

		// Create collection with subdirectory name
		// and add it to DB
		LibraryCollection clefCollection = new LibraryCollection(library, name, name, null);

		cacaodb.addCollection(clefCollection);
		collections++;

		// List files in subdirectory
		File folder = new File(dir);
		File[] listOfFiles = folder.listFiles();

		for (int i = 0; i < listOfFiles.length; i++) {
			// Parse files in subdirectories
			parseFile(listOfFiles[i].getAbsolutePath(), clefCollection);
		}
	}

	// Parse files in subdirectories
	private static void parseFile(String file, LibraryCollection collection) {
		ClefData clef;

		System.out.println("Parsing file " + file);

		try {
			// Find valid CLEF files...
			clef = new ClefData(file);

			System.out.println("Saving records...");

			Iterator<DublinCoreRecord> clefrecs = clef.getRecords().iterator();

			System.out.println("Records found: " + (long) (clef.getRecords().size() + clef.getBadRecords().size()));

			System.out.print("Records saved: ");

			// Save CLEF records to DB
			int r = 0;

			while (clefrecs.hasNext()) {
				// Extract DublinCore data from ClefData object
				DublinCoreRecord dcrec = (DublinCoreRecord) clefrecs.next();

				TreeSet<String> order = new TreeSet<String>();

				for (String s : dcrec.getIdentifier())
					if ((s != null) && (s.trim().length() > 0)) {
						order.add(s);
					}

				// Create a new LibraryRecord for record
				LibraryRecord rec = new LibraryRecord(clef.getRecordID(),collection.getLibrary(), collection, dcrec, null, null, false);

				// Add record to DB
				if (cacaodb.addRecord(rec)>=0) {
					records++;
					r++;
				}
				if ((r % 1000) == 0) {
					System.out.print(r + " ");
				}
			}

			System.out.println(r);

			System.out.println("File " + file + " - saved records: " + r + " bad records: " + clef.getBadRecords().size());

			badRecords += clef.getBadRecords().size();
		} catch (Exception e) {
			System.out.println(e.getMessage());
		}
	}
}
