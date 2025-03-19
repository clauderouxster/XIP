/**
 * 
 */
package eu.project.Cacao.Harvester;

import org.apache.log4j.Logger;

import eu.project.Cacao.Components.Utils.RunTimer;
import eu.project.Cacao.Connectors.DB.Library;
import eu.project.Cacao.Connectors.DB.LibraryCollection;
import eu.project.Cacao.Connectors.DB.LibraryRecord;
import eu.project.Cacao.Connectors.DB.MySQL;
import eu.project.Cacao.Connectors.OAIPMH.OAIPMHAccess;

/**
 * @author Igor Barsanti - <a href="http://www.gonetwork.it"
 *         target="_blank">Gonetwork Srl</a>
 * 
 */
public class OAIHarvestingAgent extends HarvestingAgent {

	private String targetUrl;
	private int retrieveStart;
	private int retrieveLimit;
	private String from;
	private String to;
	private String metaDataPrefix;
	private static MySQL cacaodb = MySQL.getInstance();
	private Logger logger = Logger.getLogger(OAIHarvestingAgent.class);
	private boolean isLimitAtCollectionsLevel = false;

	public OAIHarvestingAgent(String identifier, String targetUrl, String metaDataPrefix, String Start, String Limit, String LimitType, String from, String to) {
		super(identifier);
		this.targetUrl = targetUrl;
		this.metaDataPrefix = metaDataPrefix;
		this.from = from;
		this.to = to;

		try {
			this.retrieveStart = Integer.parseInt(Start);
		} catch (NumberFormatException e) {
			this.retrieveStart = 0;
		}

		try {
			this.retrieveLimit = Integer.parseInt(Limit);
			if (LimitType != null && LimitType.equalsIgnoreCase("OAI_SET"))
				isLimitAtCollectionsLevel = true;

		} catch (NumberFormatException e) {
			this.retrieveLimit = Integer.MAX_VALUE;
		}
	}

	public void harvest() {
		RunTimer rt = new RunTimer();
		int totalAdded = 0, retrievedRecords = 0;

		LibraryCollection defaultCollection = null;
		OAIPMHAccess oai = new OAIPMHAccess(targetUrl);

		logger.info("Agent " + identifier + ": start harvesting");

		logger.debug("Agent " + identifier + ": retrieving libraries from " + this.targetUrl);
		String[] libraries = oai.retrieveLibrariesID();
		logger.debug("Agent " + identifier + ": found " + libraries.length + " libraries");

		for (int l = 0; l < libraries.length; l++) {
			
			Library library = oai.retrieveLibrary(libraries[l]);

			if (library == null) {
				logger.error("Error agent " + identifier + " indentifying library ");
				break;
			}

			String[] languages = new String[1];
			languages[0] = "unknown";
			library.setLanguages(languages);
			
			Library checkLibrary = cacaodb.getLibraryByURL(library.getURL());
			if (checkLibrary == null) {
				cacaodb.addLibrary(library);
			}
			else {
				library = checkLibrary;
				oai.setLibrary(library);
			}
			
			logger.debug("Agent " + identifier + ": retrieving collections from library " + library.getName());
			String[] providerCollections = oai.retrieveCollectionsID(library);
			String[] collections;
			
			LibraryCollection checkDefaultCollection = cacaodb.getCollection(library, "DEFAULT");
			if (checkDefaultCollection == null) {
				defaultCollection = new LibraryCollection(library, "DefaultCollection", "DEFAULT", "");
				cacaodb.addCollection(defaultCollection);
			}
			else {
				defaultCollection  = checkDefaultCollection;
			}
			
			if (providerCollections == null) {
				logger.info("Agent " + identifier + ": found no collections, using default one");
												
				collections = new String[1];				
				collections[0] = defaultCollection.getCode();
			} else {
				collections = new String[providerCollections.length + 1];
				collections[0] = defaultCollection.getCode();
				for (int c = 1; c <= providerCollections.length; c++)
					collections[c] = providerCollections[c-1];
				
				logger.debug("Agent " + identifier + ": found " + providerCollections.length + " collections");				
			}

			for (int c = 0; c < collections.length; c++) {
				
				LibraryCollection collection; 
				
				if (c == 0) {
					collection = defaultCollection;
					logger.debug("Agent " + identifier + ": retrieving records from default collection");
				}
				else {
					
					collection = oai.retrieveCollection(collections[c]);
				
					if (collection != null) {
						LibraryCollection checkCollection = cacaodb.getCollection(library, collection.getCode());
						if (checkCollection == null)
							cacaodb.addCollection(collection);
						else
							collection = checkCollection;					
					}
					logger.debug("Agent " + identifier + ": retrieving records from collection " + collection.getCode());
				}
				
				String[] records;
				String resumptionToken = null;

				do {
					if (this.retrieveLimit < Integer.MAX_VALUE) {
						logger.info("Agent " + identifier + ": harvesting limited to " + this.retrieveLimit + " records. Starting from " + this.retrieveStart);
						if (retrievedRecords >= (this.retrieveLimit + this.retrieveStart)) {
							logger.info("Limit already reached, skipping further records from library: " + library.getName());
							logger.info("Agent " + identifier + ": harvesting terminated. " + retrievedRecords + " records retrieved, " + totalAdded + " added. " + rt.getElapsedTimeLabel());
							return;
						}
					}

					if (collection.equals(defaultCollection))
						records = oai.retrieveRecordsID(null, this.metaDataPrefix, resumptionToken, this.from, this.to);
					else
						records = oai.retrieveRecordsID(collection.getCode(), this.metaDataPrefix, resumptionToken, this.from, this.to);
					resumptionToken = oai.getResumptionToken();

					int r, localAdded;
					if (records != null) {
						for (r = 0, localAdded = 0; r < records.length; r++, retrievedRecords++) {
							
							if ( retrievedRecords >= this.retrieveStart && ((totalAdded +localAdded) < (this.retrieveLimit ))
									|| (this.isLimitAtCollectionsLevel && localAdded < (this.retrieveLimit)) ) {
								
								logger.debug("Check record " + records[r]);
								LibraryRecord checkRecord = cacaodb.getRecordByLibraryID(library, records[r]);
								
								int retValue = -1;
								
								if (checkRecord == null) {									
									LibraryRecord record = oai.retrieveRecord(records[r], collection, this.metaDataPrefix);															
									
									if (record == null) {
										retValue = cacaodb.addErrorRecordInfo(collection.getLibrary().getID(), collection.getID(), records[r]);
										logger.debug("Record " + records[r] + " contains error. Set to skipped.");
									}
									 									
									retValue = cacaodb.addRecord(record);
									
									logger.debug("Agent " + identifier + ": harvested record " + records[r]);
								}
								else {
									if (!cacaodb.checkRecordCollection(checkRecord, collection)) {
										checkRecord.setCollection(collection);
										retValue = cacaodb.updateRecordCollection(checkRecord);
										logger.debug("Agent " + identifier + ": updated record " + records[r]);
									}																		
								}
								
								if (retValue >= 0)
									localAdded += retValue;
								
							} else {
								logger.debug("Agent " + identifier + ": skipped record " + records[r]);
							}

						}
						logger.info("Agent " + identifier + " - OAI set: " + collection.getCode() + " - Listed " + r + " records, added " + localAdded);
						totalAdded += localAdded;
					} else {
						logger.info("Agent " + identifier + ": no records for collection " + collection.getCode());
					}

				} while ((resumptionToken != null) && (retrievedRecords < (this.retrieveLimit + this.retrieveStart)));
			}
		}

		logger.info("Agent " + identifier + ": harvesting terminated. " + retrievedRecords + " records retrieved, " + totalAdded + " added. " + rt.getElapsedTimeLabel());
	}

}
