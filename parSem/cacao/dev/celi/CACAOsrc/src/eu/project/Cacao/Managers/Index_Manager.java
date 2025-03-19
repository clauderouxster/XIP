package eu.project.Cacao.Managers;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Vector;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.axis2.AxisFault;
import org.apache.log4j.Logger;
import org.apache.lucene.analysis.KeywordAnalyzer;
import org.apache.lucene.analysis.PerFieldAnalyzerWrapper;
import org.apache.lucene.analysis.StopAnalyzer;
import org.apache.lucene.analysis.de.GermanAnalyzer;
import org.apache.lucene.analysis.fr.FrenchAnalyzer;
import org.apache.lucene.analysis.snowball.SnowballAnalyzer;
import org.apache.lucene.analysis.standard.StandardAnalyzer;
import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;
import org.apache.lucene.index.IndexWriter;
import org.getopt.stempel.lucene.StempelAnalyzer;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

import eu.project.Cacao.Apps.LuceneIndexMerger.MySimpleAnalyzer;
import eu.project.Cacao.Components.Utils.Iso639;
import eu.project.Cacao.Components.Utils.NLPtoken;
import eu.project.Cacao.Components.Utils.RunTimer;
import eu.project.Cacao.Components.Utils.XIPwrapper;
import eu.project.Cacao.Connectors.DB.Library;
import eu.project.Cacao.Connectors.DB.LibraryCollection;
import eu.project.Cacao.Connectors.DB.LibraryRecord;
import eu.project.Cacao.Connectors.DB.MySQL;
import eu.project.Cacao.Connectors.Metadata.DublinCoreRecord;
import eu.project.Cacao.Connectors.Metadata.TelAPRecord;
import eu.project.Cacao.Interfaces.CacaoDB;


/**
 * Utility class that generates a Lucene index from resources stored in DB
 *
 * @author Alessio Bosca
 *
 */
public class Index_Manager {
	final private static String userAttr="user", pwdAttr="pwd",dbNameAttr="dbName", hostAttr="host", dbElementTag="DataBase", 
	IndexCreationElmTag="IndexCreation", allRecordsFromDBAttr="allRecordsFromDB", libraryNameAttr="libraryName", 
	indexPathAttr="indexPath", nlpServiceElementTag="nlpService", urlAttribute="url", createNewIdxAttr="createNewIdx",
	startIDAttr="dbIDstart", stopIDAttr="dbIDstop";
	
	final private static String defaultConfigurationFile="configuration/IndexManagerConfiguration.xml";
	
	private CacaoDB dbConnection;
	private File indexOutputFolder;
	private Hashtable<String,XIPwrapper> xipWrapperByLang;
	
	private static Logger logger = Logger.getLogger(Index_Manager.class);
	
	private Library getLibraryIDfromName(String libraryName){
		String libariesIDs[] = this.dbConnection.getAllLibraries();
		for(String libID: libariesIDs){
			Library lib = this.dbConnection.getLibrary(libID);
			if(lib.getName().equalsIgnoreCase(libraryName))
				return lib;
		}
		return null;
	}
	
	private boolean isRecordMemberOfOAISetsToIndex(LibraryRecord record, String[] oaiSetsToIndex){
		String[] record_OAIsetsMembership = record.getMultiCollection();
		for(String oaiSet2Index: oaiSetsToIndex){
			for(String oaiSetOfRecord: record_OAIsetsMembership){
				if(oaiSetOfRecord.equalsIgnoreCase(oaiSet2Index))
					return true;
			}
		}
		return false;
	}
	
	private PerFieldAnalyzerWrapper getAnalyzer(){
		PerFieldAnalyzerWrapper pfan = new PerFieldAnalyzerWrapper(new MySimpleAnalyzer());
		pfan.addAnalyzer("title", new StandardAnalyzer());
		pfan.addAnalyzer("subject", new StandardAnalyzer());
		pfan.addAnalyzer("description", new StandardAnalyzer());
		pfan.addAnalyzer("tableOfContents", new StandardAnalyzer());
		pfan.addAnalyzer("contents", new StandardAnalyzer());
		pfan.addAnalyzer("OAIset", new KeywordAnalyzer());
		pfan.addAnalyzer("subject_ClassificationCategory", new KeywordAnalyzer());
		pfan.addAnalyzer("type", new KeywordAnalyzer());
		pfan.addAnalyzer("telRecordID", new KeywordAnalyzer());
		pfan.addAnalyzer("telRecordID_URL", new KeywordAnalyzer());
		pfan.addAnalyzer("telRecordID_ISSN", new KeywordAnalyzer());
		pfan.addAnalyzer("telRecordID_ISBN", new KeywordAnalyzer());
		pfan.addAnalyzer("identifier", new KeywordAnalyzer());
		pfan.addAnalyzer("identifier_URL", new KeywordAnalyzer());
		pfan.addAnalyzer("identifier_ISSN", new KeywordAnalyzer());
		pfan.addAnalyzer("identifier_ISBN", new KeywordAnalyzer());
		pfan.addAnalyzer("namedEntity_PERSON", new KeywordAnalyzer());
		pfan.addAnalyzer("namedEntity_ORGANISATION", new KeywordAnalyzer());
		pfan.addAnalyzer("namedEntity_GEO", new KeywordAnalyzer());
		
		pfan.addAnalyzer("tableOfContents_enStem", new SnowballAnalyzer("English", StopAnalyzer.ENGLISH_STOP_WORDS));
		pfan.addAnalyzer("description_enStem", new SnowballAnalyzer("English", StopAnalyzer.ENGLISH_STOP_WORDS));
		pfan.addAnalyzer("title_enStem", new SnowballAnalyzer("English", StopAnalyzer.ENGLISH_STOP_WORDS));
		pfan.addAnalyzer("subject_enStem", new SnowballAnalyzer("English", StopAnalyzer.ENGLISH_STOP_WORDS));
		
		pfan.addAnalyzer("tableOfContents_frStem", new SnowballAnalyzer("French", FrenchAnalyzer.FRENCH_STOP_WORDS));
		pfan.addAnalyzer("description_frStem", new SnowballAnalyzer("French", FrenchAnalyzer.FRENCH_STOP_WORDS));
		pfan.addAnalyzer("title_frStem", new SnowballAnalyzer("French", FrenchAnalyzer.FRENCH_STOP_WORDS));
		pfan.addAnalyzer("subject_frStem", new SnowballAnalyzer("French", FrenchAnalyzer.FRENCH_STOP_WORDS));
		
		pfan.addAnalyzer("tableOfContents_deStem", new SnowballAnalyzer("German", GermanAnalyzer.GERMAN_STOP_WORDS));
		pfan.addAnalyzer("description_deStem", new SnowballAnalyzer("German", GermanAnalyzer.GERMAN_STOP_WORDS));
		pfan.addAnalyzer("title_deStem", new SnowballAnalyzer("German", GermanAnalyzer.GERMAN_STOP_WORDS));
		pfan.addAnalyzer("subject_deStem", new SnowballAnalyzer("German", GermanAnalyzer.GERMAN_STOP_WORDS));
		
		String[] itStopwords=new String[]{"a", "adesso", "ai", "al", "alla", "allo", "allora", "anche", "avere", "aveva", "avevano", "ben", "buono", "che", "chi", "con", "cui", "da", "del", "della", "dello", "di", 
				 "e", "fare", "fra", "ha", "hai", "hanno", "ho", "il", "invece", "io", "la", "le", "lei", "lo", "loro", "lui", "ma", "me", "nei", "nella", "no", "noi", "o", "oltre", "qua", "quello", "questo", "qui", 
				 "quindi", "senza", "sia", "siamo", "siete", "sono", "stati", "stato", "stesso", "sul", "sulla", "te", "tra", "un", "una", "uno" };
		pfan.addAnalyzer("tableOfContents_itStem", new SnowballAnalyzer("Italian", itStopwords ));
		pfan.addAnalyzer("description_itStem", new SnowballAnalyzer("Italian", itStopwords ));
		pfan.addAnalyzer("title_itStem", new SnowballAnalyzer("Italian", itStopwords ));
		pfan.addAnalyzer("subject_itStem", new SnowballAnalyzer("Italian", itStopwords));
		
		String[] huStopwords=new String[]{"a", "az", "egy", "be", "ki", "le", "fel", "meg", "el", "�t", "r�", "ide", "oda", "sz�t", "�ssze", "vissza", "de", "h�t", "�s", "vagy", "hogy", "van", 
				"lesz", "volt", "csak", "nem", "igen", "mint", "�n", "te", "�", "mi", "ti", "�k", "�n"};
		pfan.addAnalyzer("tableOfContents_huStem", new SnowballAnalyzer("Hungarian", huStopwords ));
		pfan.addAnalyzer("description_huStem", new SnowballAnalyzer("Hungarian", huStopwords ));
		pfan.addAnalyzer("title_huStem", new SnowballAnalyzer("Hungarian", huStopwords ));
		pfan.addAnalyzer("subject_huStem", new SnowballAnalyzer("Hungarian", huStopwords));
		
		pfan.addAnalyzer("tableOfContents_plStem", new StempelAnalyzer() );
		pfan.addAnalyzer("description_plStem", new StempelAnalyzer() );
		pfan.addAnalyzer("title_plStem", new StempelAnalyzer() );
		pfan.addAnalyzer("subject_plStem", new StempelAnalyzer() );
		
		return pfan;
	}
	
	/**
	 * 
	 * @param oaiSetsToIndexGroupedByLibrary if null it means index all available data (all OAI sets from all Libraries)
	 * @param createIDX
	 * @param indexName
	 * @param nlpWS_endpoint
	 */
	public void index(HashMap<String,String[]> oaiSetsToIndexGroupedByLibrary, String indexName, boolean createIDX, String nlpWS_endpoint) {
		// Creating the index directory if not exists
		this.indexOutputFolder = new File(indexName);

		if (!this.indexOutputFolder.exists()) {
			this.indexOutputFolder.mkdirs();
		}
		
		RunTimer rt=new RunTimer();
		
		try {
			
			PerFieldAnalyzerWrapper pfan = this.getAnalyzer();
			
			IndexWriter writer = new IndexWriter(indexOutputFolder, pfan, createIDX, IndexWriter.MaxFieldLength.LIMITED);
			logger.info("Indexing to directory '" + indexOutputFolder + "'...");
			
			if(oaiSetsToIndexGroupedByLibrary!=null){
				for(String libraryName: oaiSetsToIndexGroupedByLibrary.keySet()){
					Library library = this.getLibraryIDfromName(libraryName);
					logger.info("Indexing library " + library.getName() + "...");
					if(library!=null){
						String[] oaiSetsToIndex = oaiSetsToIndexGroupedByLibrary.get(libraryName);
						String[] recordIDs = this.dbConnection.getAllRecords(library);
						int cnt=0;
						for(String recordIDfromDB: recordIDs){
							long dbID = Long.parseLong(recordIDfromDB);
							LibraryRecord record = this.dbConnection.getRecordByDatabaseID(dbID);
							if(oaiSetsToIndex==null || this.isRecordMemberOfOAISetsToIndex(record, oaiSetsToIndex)){
								cnt++;
								try {
									this.addLibraryRecordToLuceneIndex(record, writer);
									if(cnt % 1000 == 0){
										logger.info(cnt+"... ");
									}
								} catch (Exception e) {
									logger.error("Error trying to add record "+recordIDfromDB+".\n"+e);
								}
							}
						}
					}
				}
			}
			else{
				String[] libraryIDs = this.dbConnection.getAllLibraries();
				for(String libID: libraryIDs){
					Library library = this.dbConnection.getLibrary(libID);
					logger.info("Indexing library " + library.getName() + "...");
					String[] recordIDs = this.dbConnection.getAllRecords(library);
					int cnt=0;
					for(String recordIDfromDB: recordIDs){
						long dbID = Long.parseLong(recordIDfromDB);
						LibraryRecord record = this.dbConnection.getRecordByDatabaseID(dbID);
						try {
							cnt++;
							this.addLibraryRecordToLuceneIndex(record, writer);
							if(cnt % 1000 == 0){
								logger.info(cnt+"... ");
							}
						} catch (Exception e) {
							logger.error("Error trying to add record "+recordIDfromDB+".\n"+e);
						}
					}
				}
			}

			writer.optimize();
			writer.close();
			
			logger.info("Created index "+indexOutputFolder+". "+rt.getElapsedTimeLabel());
		} catch (IOException e) {
			System.out.println(" caught a " + e.getClass() + "\n with message: " + e.getMessage());
		}
	}
	
	/**
	 * 
	 * @param oaiSetsToIndexGroupedByLibrary if null it means index all available data (all OAI sets from all Libraries)
	 * @param createIDX
	 * @param indexName
	 * @param nlpWS_endpoint
	 */
	public void indexRecordsByDBidentifier(long start, long stop, String indexName, boolean createIDX, String nlpWS_endpoint) {
		// Creating the index directory if not exists
		this.indexOutputFolder = new File(indexName);

		if (!this.indexOutputFolder.exists()) {
			this.indexOutputFolder.mkdirs();
		}
		
		RunTimer rt=new RunTimer();
		
		try {
			
			PerFieldAnalyzerWrapper pfan = this.getAnalyzer();
			
			IndexWriter writer = new IndexWriter(indexOutputFolder, pfan, createIDX, IndexWriter.MaxFieldLength.LIMITED);
			logger.info("Indexing to directory '" + indexOutputFolder + "' Records with DB ID "+start+" - "+stop);
			
			int cnt=0;
			for(long dbID=start;dbID<=stop;dbID++){
					LibraryRecord record = this.dbConnection.getRecordByDatabaseID(dbID);
					if(record!=null){
						try {
							cnt++;
							this.addLibraryRecordToLuceneIndex(record, writer);
							if(cnt % 1000 == 0){
								logger.info(">>"+indexOutputFolder+" "+cnt+"... ");
							}
						} catch (Exception e) {
							logger.error("Error trying to add record with DB ID: "+dbID+".\n"+e);
						}
					}
					else
						logger.debug("skipping.. No element with ID: "+dbID+".\n");
				
			}


			writer.optimize();
			
			writer.close();
			
			logger.info("Created index "+indexOutputFolder+". "+rt.getElapsedTimeLabel());
		} catch (IOException e) {
			System.out.println(" caught a " + e.getClass() + "\n with message: " + e.getMessage());
		}
	}
	
	private void addLibraryRecordToLuceneIndex(LibraryRecord record, IndexWriter writer) throws IOException{
		if(record.conformsToDCMetaData())
			this.dcRecord2LuceneDocument(record, writer);
		else if(record.conformsToTelApMetaData())
			this.telapRecord2LuceneDocument(record, writer);
		logger.debug("Indexed doc "+record.getLibraryID());
	}
	
	/**
	 * Makes a document for a LibraryRrecord.
	 * 
	 * The document has three fields:
	 * 
	 */
	private void dcRecord2LuceneDocument(LibraryRecord record, IndexWriter writer) throws java.io.IOException {
		// Extract DublinCoreRecord Object with DublinCore data
		DublinCoreRecord dcdata = record.getDublinCoreData();

		Vector<String> langs = dcdata.getLanguage();
		String lang = "";

		if ((langs != null) && (langs.size() > 0)) {
			lang = langs.firstElement();
			Iso639 documentLanguage = Iso639.find(lang);
			if (documentLanguage != null && documentLanguage.twoCharCode != null)
				lang = documentLanguage.twoCharCode;
		}

		// make a new, empty document
		Document doc = new Document();
		
		doc.add(new Field("libraryID", record.getLibrary().getName(), Field.Store.YES, Field.Index.NOT_ANALYZED));
		for(String oaiSetID: record.getMultiCollection()){
			LibraryCollection oaiSet = this.dbConnection.getCollection(oaiSetID);
			if(oaiSet!=null){
				String code = oaiSet.getCode();
				if(code.equalsIgnoreCase("DEFAULT"))
					doc.add(new Field("OAIset", code, Field.Store.YES, Field.Index.NOT_ANALYZED));
			}
			doc.add(new Field("OAIset", oaiSet.getCode(), Field.Store.YES, Field.Index.NOT_ANALYZED));
		}
		
		doc.add(new Field("recordID", record.getDatabaseID(), Field.Store.YES, Field.Index.NOT_ANALYZED));
		
		for (String id : dcdata.getIdentifier()) {
			String fieldName=null;
			if(id.startsWith("http://"))
				fieldName= "identifier_URL";
			else
				fieldName= ("identifier");
			doc.add(new Field(fieldName, id, Field.Store.YES, Field.Index.NOT_ANALYZED));
		}
		
		for (String language : langs) {
			Iso639 documentLanguage = Iso639.find(language);
			if (documentLanguage != null && documentLanguage.twoCharCode != null)
				language = documentLanguage.twoCharCode;
			doc.add(new Field("language", language, Field.Store.YES, Field.Index.ANALYZED));
		}
		
		this.addDCelement("creator", dcdata.getCreator(), doc);
		this.addDCelement("date", dcdata.getDate(), doc);
		this.addDCelement("contributor", dcdata.getContributor(), doc);
		this.addDCelement("coverage", dcdata.getCoverage(), doc);
		this.addDCelement("publisher", dcdata.getPublisher(), doc);
		//this.addDCelement("rights", dcdata.getRights(), doc);
		this.addDCelement("type", dcdata.getType(), doc);
		
		Hashtable<String,String> neEntities=new Hashtable<String, String>();
		neEntities.putAll( this.addDCelement("description", dcdata.getDescription(), null, doc, lang) );
		neEntities.putAll( this.addDCelement("title", removeTrailingPunctation(dcdata.getTitle()), null, doc, lang) );
		neEntities.putAll( this.addDC_SubjectElement("subject", dcdata.getSubject(), dcdata.getSubjectAtts(), doc, lang) );
		
		for(String ne: neEntities.keySet()){
			String neCategory=neEntities.get(ne);
			if(neCategory.equalsIgnoreCase("PERSON")){
				doc.add(new Field("namedEntity_PERSON", ne, Field.Store.YES, Field.Index.ANALYZED));
			}
			else if(neCategory.equalsIgnoreCase("ORGANISATION")){
				doc.add(new Field("namedEntity_ORGANISATION", ne, Field.Store.YES, Field.Index.ANALYZED));
			}
			else if(neCategory.equalsIgnoreCase("LOCATION") || neCategory.equalsIgnoreCase("LOCORG")){
				doc.add(new Field("namedEntity_GEO", ne, Field.Store.YES, Field.Index.ANALYZED));
			}
			
		}
		
		this.addTextualContentAsContentsField(doc);
		
		writer.addDocument(doc);
	}
	
	/**
	 * Makes a document for a LibraryRrecord.
	 * 
	 * The document has three fields:
	 * 
	 */
	private void telapRecord2LuceneDocument(LibraryRecord record, IndexWriter writer) throws java.io.IOException {
		// Extract DublinCoreRecord Object with DublinCore data
		TelAPRecord telapData = record.getTelAPData();
		Vector<String> langs = telapData.getLanguage();
		String lang = "";

		if ((langs != null) && (langs.size() > 0)) {
			lang = langs.firstElement();
			Iso639 documentLanguage = Iso639.find(lang);
			if (documentLanguage != null && documentLanguage.twoCharCode != null)
				lang = documentLanguage.twoCharCode;
		}

		// make a new, empty document
		Document doc = new Document();
		
		doc.add(new Field("libraryID", record.getLibrary().getName(), Field.Store.YES, Field.Index.NOT_ANALYZED));
		
		for(String oaiSetID: record.getMultiCollection()){
			LibraryCollection oaiSet = this.dbConnection.getCollection(oaiSetID);
			if(oaiSet!=null){
				String code = oaiSet.getCode();
				if(code.equalsIgnoreCase("DEFAULT"))
					doc.add(new Field("OAIset", code, Field.Store.YES, Field.Index.NOT_ANALYZED));
			}
		}
		
		doc.add(new Field("recordID", record.getLibraryID(), Field.Store.YES, Field.Index.NOT_ANALYZED));
		
		this.add_IdentifierElement(doc, "telRecordID", telapData.getTelRecordId(), telapData.getTelRecordIdAtts());
		this.add_IdentifierElement(doc, "identifier", telapData.getIdentifier(), telapData.getIdentifierAtts());
		
		for (String language : langs) {
			Iso639 documentLanguage = Iso639.find(language);
			if (documentLanguage != null && documentLanguage.twoCharCode != null)
				language = documentLanguage.twoCharCode;
			doc.add(new Field("language", language, Field.Store.YES, Field.Index.ANALYZED));
		}
		
		this.addDCelement("creator", telapData.getCreator(), telapData.getCreatorAtts(), doc);
		this.addDCelement("date", telapData.getDate(), telapData.getDateAtts(), doc);
		this.addDCelement("contributor", telapData.getContributor(), telapData.getContributorAtts(), doc);
		this.addDCelement("coverage", telapData.getCoverage(), telapData.getCoverageAtts(), doc);
		this.addDCelement("publisher", telapData.getPublisher(), doc);
		//this.addDCelement("rights", telapData.getRights(), doc);
		this.addDCelement("type", telapData.getType(), telapData.getTypeAtts(), doc);
		
		Hashtable<String,String> neEntities=new Hashtable<String, String>();
		neEntities.putAll( this.addDCelement("description", telapData.getDescription(), telapData.getDescriptionAtts(), doc, lang) );
		neEntities.putAll( this.addDCelement("title", telapData.getTitle(), telapData.getTitleAtts(), doc, lang) );
		neEntities.putAll( this.addDCelement("tableOfContents", telapData.getTableOfContents(), telapData.getTableOfContentsAtts(), doc, lang) );
		neEntities.putAll( this.addDC_SubjectElement("subject", telapData.getSubject(), telapData.getSubjectAtts(), doc, lang) );
		
		for(String ne: neEntities.keySet()){
			String neCategory=neEntities.get(ne);
			if(neCategory.equalsIgnoreCase("PERSON")){
				doc.add(new Field("namedEntity_PERSON", ne, Field.Store.YES, Field.Index.ANALYZED));
			}
			else if(neCategory.equalsIgnoreCase("ORGANISATION")){
				doc.add(new Field("namedEntity_ORGANISATION", ne, Field.Store.YES, Field.Index.ANALYZED));
			}
			else if(neCategory.equalsIgnoreCase("LOCATION") || neCategory.equalsIgnoreCase("LOCORG")){
				doc.add(new Field("namedEntity_GEO", ne, Field.Store.YES, Field.Index.ANALYZED));
			}
		}
		
		this.addTextualContentAsContentsField(doc);
		
		writer.addDocument(doc);
	}
	
	private void addTextualContentAsContentsField(Document doc){
		StringBuffer buffer=new StringBuffer();
		String[] temps=doc.getValues("title");
		if(temps!=null){
			for(String s: temps)
				buffer = buffer.append(s+" ");
		}
		temps=doc.getValues("tableOfContents");
		if(temps!=null){
			for(String s: temps)
				buffer = buffer.append(s+" ");
		}
		temps=doc.getValues("subject");
		if(temps!=null){
			for(String s: temps)
				buffer = buffer.append(s+" ");
		}
		temps=doc.getValues("description");
		if(temps!=null){
			for(String s: temps)
				buffer = buffer.append(s+" ");
		}
		doc.add(new Field("contents", buffer.toString().trim(), Field.Store.YES, Field.Index.ANALYZED));
		
		buffer=new StringBuffer();
		temps=doc.getValues("title_lemmatized");
		if(temps!=null){
			for(String s: temps)
				buffer = buffer.append(s+" ");
		}
		temps=doc.getValues("tableOfContents_lemmatized");
		if(temps!=null){
			for(String s: temps)
				buffer = buffer.append(s+" ");
		}
		temps=doc.getValues("subject_lemmatized");
		if(temps!=null){
			for(String s: temps)
				buffer = buffer.append(s+" ");
		}
		temps=doc.getValues("description_lemmatized");
		if(temps!=null){
			for(String s: temps)
				buffer = buffer.append(s+" ");
		}
		doc.add(new Field("contents_lemmatized", buffer.toString().trim(), Field.Store.YES, Field.Index.ANALYZED));
		
	}
	

	private Vector<String> removeTrailingPunctation(Vector<String> src) {
		Vector<String> res = new Vector<String>();

		for (String s : src) {
			res.add(s.trim().replaceAll("/$", "").trim());
		}

		return res;
	}
	
	private void initXIPGrammarsPaths(){
		xipWrapperByLang = new Hashtable<String, XIPwrapper>();
		xipWrapperByLang.put("de", new XIPwrapper(XIPwrapper.deGram,"de"));
		xipWrapperByLang.put("en", new XIPwrapper(XIPwrapper.enGram,"en"));
		xipWrapperByLang.put("fr", new XIPwrapper(XIPwrapper.frGram,"fr"));
		xipWrapperByLang.put("it", new XIPwrapper(XIPwrapper.itGram,"it"));
	}

	private Vector<NLPtoken>  lemmatize(String lang, String toBeLemmatized) {
		try {
			// lemmatizers
			//CACAO_nlpWS_client lemmatizer = new CACAO_nlpWS_client(nlpWS_endpoint);
			
			Vector<NLPtoken> list = new Vector<NLPtoken>();
			if(lang.equalsIgnoreCase("it")||lang.equalsIgnoreCase("en")||lang.equalsIgnoreCase("de")||lang.equalsIgnoreCase("fr")){
				XIPwrapper lemmatizer = this.xipWrapperByLang.get(lang);
				list = lemmatizer.parsePhrase(toBeLemmatized);	
			}
			return list;
		} catch (Exception e) {
			return new Vector<NLPtoken>();
		}
	}

	private void addDCelement(String fieldName, Vector<String> dcElm, Document doc) {
		for (String value : dcElm) {
			doc.add(new Field(fieldName, value, Field.Store.YES, Field.Index.ANALYZED));
		}
	}
	
	private void addDCelement(String fieldName, Vector<String> dcElm, Vector<Hashtable<String, String>> dcElmAttrs, Document doc) {
		for (int i=0;dcElm!=null && i<dcElm.size();i++) {
			String value=dcElm.elementAt(i);
			if(dcElmAttrs!=null && dcElmAttrs.size()>0){
				Hashtable<String, String> attrs = dcElmAttrs.elementAt(i);
				if(attrs.containsKey("type")){
					String type=attrs.get("type");
					doc.add(new Field(fieldName, type+" > "+value, Field.Store.YES, Field.Index.ANALYZED));
				}else
					doc.add(new Field(fieldName, value, Field.Store.YES, Field.Index.ANALYZED));
			}else
				doc.add(new Field(fieldName, value, Field.Store.YES, Field.Index.ANALYZED));
		}
	}

	private Hashtable<String,String> addDCelement(String fieldName, Vector<String> dcElm, Vector<Hashtable<String, String>> dcElmAttrs, Document doc, String lang) {
		Hashtable<String,String> neEntities=new Hashtable<String, String>();
		for (int i=0;dcElm!=null&&i<dcElm.size();i++) {	
			String value =dcElm.elementAt(i);
			Hashtable<String, String> attrs=null;
			if(dcElmAttrs !=null)
				attrs = dcElmAttrs.elementAt(i);
			
			String localLang=lang;
			if(attrs!=null && attrs.containsKey("lang")){
				Iso639 documentLanguage = Iso639.find(attrs.get("lang"));
				if (documentLanguage != null && documentLanguage.twoCharCode != null)
					localLang = documentLanguage.twoCharCode;
			}
			
				if(value.trim().length()>0){
				doc.add(new Field(fieldName, value.trim(), Field.Store.YES, Field.Index.ANALYZED));
				if(localLang.equalsIgnoreCase("en")||localLang.equalsIgnoreCase("fr")||localLang.equalsIgnoreCase("de")||
						localLang.equalsIgnoreCase("it")||localLang.equalsIgnoreCase("pl")||localLang.equalsIgnoreCase("hu")){
					doc.add(new Field(fieldName + "_"+localLang+"Stem", value, Field.Store.YES, Field.Index.ANALYZED));
				}
			}
				
			Vector<NLPtoken> analyzed = this.lemmatize(localLang, value);
			if(analyzed.size()>0){
				String lemmatizedStr="";
				for(NLPtoken tok: analyzed){
					lemmatizedStr+=tok.getLemma_Category()+" ";
					if(tok.isNE){
						neEntities.put(tok.surface, tok.neCategory);
					}
				}
				if(lemmatizedStr.trim().length()>0)
					doc.add(new Field(fieldName + "_lemmatized", lemmatizedStr.trim(), Field.Store.YES, Field.Index.ANALYZED));
			}
		}
		return neEntities;
	}

	private Hashtable<String,String> addDC_SubjectElement(String fieldName, Vector<String> dcElm, Vector<Hashtable<String, String>> dcElmAttrs, Document doc, String lang) {
		Hashtable<String,String> neEntities=new Hashtable<String, String>();
		for (int i = 0; i < dcElm.size(); i++) {
			String value = dcElm.elementAt(i);		
			Hashtable<String, String> attrs = dcElmAttrs.elementAt(i);
			
			String localLang=lang;
			if(attrs!=null && attrs.containsKey("lang")){
				Iso639 documentLanguage = Iso639.find(attrs.get("lang"));
				if (documentLanguage != null && documentLanguage.twoCharCode != null)
					localLang = documentLanguage.twoCharCode;
			}
			
			String type=null;
			if(attrs!=null && attrs.containsKey("type")){
				type = attrs.get("type");
			}

			boolean isText=false;
			String lemmatizedStr="";
			if (localLang!=null && localLang.length()>0) {
				
				Vector<NLPtoken> analyzed = this.lemmatize(localLang, value);
				for(NLPtoken tok: analyzed){
					lemmatizedStr+=tok.getLemma_Category()+" ";
					
					if(tok.category.equalsIgnoreCase("NOUN")||tok.category.equalsIgnoreCase("VERB")||
							tok.category.equalsIgnoreCase("ADJ")||tok.category.equalsIgnoreCase("NADJ"))
						isText=true;
					
					if(tok.isNE){
						neEntities.put(tok.surface, tok.neCategory);
					}
				}			
			} 
			
			if(isText){
				doc.add(new Field(fieldName + "_lemmatized", lemmatizedStr.trim(), Field.Store.YES, Field.Index.ANALYZED));
				if(localLang.equalsIgnoreCase("en")||localLang.equalsIgnoreCase("fr")||localLang.equalsIgnoreCase("de")||
						localLang.equalsIgnoreCase("it")||localLang.equalsIgnoreCase("pl")||localLang.equalsIgnoreCase("hu")){
					doc.add(new Field(fieldName + "_"+localLang+"Stem", value, Field.Store.YES, Field.Index.ANALYZED));
				}
				doc.add(new Field(fieldName, value, Field.Store.YES, Field.Index.ANALYZED));
			}
			
			if(type!=null && type.length()>0){
				doc.add(new Field(fieldName+"_ClassificationCategory", type + "/" + value, Field.Store.YES, Field.Index.NOT_ANALYZED));				
			}
			else if(!isText){
				doc.add(new Field(fieldName, value, Field.Store.YES, Field.Index.NOT_ANALYZED));
			}
		}
		return neEntities;
	}
	
	private void add_IdentifierElement(Document doc, String fieldName, 
			Vector<String> values, Vector<Hashtable<String, String>> attrsOfValues){
		
	for (int i=0;i<values.size();i++) {
		String id=values.elementAt(i);
		Hashtable<String, String> attrs = attrsOfValues.elementAt(i);
		if(attrs.containsKey("type")){
			String value=attrs.get("type");
			if(value.equalsIgnoreCase("tel:URL")){
				doc.add(new Field(fieldName+"_URL", id, Field.Store.YES, Field.Index.NOT_ANALYZED));
			}else if(value.equalsIgnoreCase("tel:ISBN")){
				doc.add(new Field(fieldName+"_ISBN", id, Field.Store.YES, Field.Index.NOT_ANALYZED));
			}
			if(value.equalsIgnoreCase("tel:ISSN")){
				doc.add(new Field(fieldName+"_ISSN", id, Field.Store.YES, Field.Index.NOT_ANALYZED));
			}

		}
		else doc.add(new Field(fieldName, id, Field.Store.YES, Field.Index.NOT_ANALYZED));
	}
	}
	
	public boolean initDBConnector(String user, String pwd, String DBname, String host) throws AxisFault {
		// Connect to DB
		MySQL cacaodb = new MySQL();
		cacaodb.setServer(host);
		cacaodb.setDBname(DBname);
		cacaodb.setUsername(user);
		cacaodb.setPassword(pwd);
		
		this.dbConnection = cacaodb;
		return this.dbConnection.connect();
	}
	
	private boolean initDBconnector(Element db){
		boolean retvalue=false;
		Logger logger = Logger.getLogger(Index_Manager.class.getName());
		try {
			String user=db.getAttribute(Index_Manager.userAttr);
			String pwd=db.getAttribute(Index_Manager.pwdAttr);
			String dbName=db.getAttribute(Index_Manager.dbNameAttr);
			String host=db.getAttribute(Index_Manager.hostAttr);

			retvalue=this.initDBConnector(user, pwd, dbName, host);
			logger.info("Successfully inited connection to DB");
		} catch (IOException e) {
			logger.error("Failed creating DB connection "+e);
		}
		return retvalue;
	}
	
	/**
	 * Read initialization parameters from an xml file for generating semantic
	 * vector spaces from a lucene index
	 * 
	 * @param xmlConfigurationFile
	 *            Configuration file path
	 * @return exit status
	 */
	private boolean executeTasksFromConfigurationFile(String xmlConfigurationFile) {
		try {
			logger.debug("Init IndexManager module from " + xmlConfigurationFile);
			// getting the document builder Factory
			DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();

			// if the document has to be validated against the schema source

			// getting the document builder from the Factory
			DocumentBuilder builder = factory.newDocumentBuilder();

			// parses the xml
			org.w3c.dom.Document myDocument = builder.parse(new File(xmlConfigurationFile));

			Element root = myDocument.getDocumentElement();
			String pathOfIdxToCreate = root.getAttribute(Index_Manager.indexPathAttr);
			String tmp = root.getAttribute(Index_Manager.createNewIdxAttr);
			boolean createNewIdx = Boolean.parseBoolean(tmp);
			
			Element dbElm = (Element) root.getElementsByTagName(Index_Manager.dbElementTag).item(0);
			boolean isDbInited = this.initDBconnector(dbElm);
			if(!isDbInited)
				return false;
			
	        //NLP services settings (multiple services support to be added)
			String nlpWS_endpoint="";
			NodeList nl = myDocument.getElementsByTagName(Index_Manager.nlpServiceElementTag);
            for (int i = 0; i < nl.getLength(); i++) {
                Element elm = (Element) nl.item(i);
                nlpWS_endpoint = elm.getAttribute(Index_Manager.urlAttribute);
            }
 
			
			HashMap<String,String[]> oaiSetsToIndexGroupedByLibrary=new HashMap<String, String[]>();
			nl = root.getElementsByTagName(Index_Manager.IndexCreationElmTag);
			for (int i = 0; i < nl.getLength(); i++) {
				Element elm = (Element) nl.item(i);
				String allRecords = elm.getAttribute(Index_Manager.allRecordsFromDBAttr);
				String start = elm.getAttribute(Index_Manager.startIDAttr);
				String stop = elm.getAttribute(Index_Manager.stopIDAttr);
				String libName = elm.getAttribute(Index_Manager.libraryNameAttr);
				//if index operation is issued with tasks param null all rercords from DB will be indexed
				if(allRecords!=null && Boolean.parseBoolean(allRecords)){
					this.index(null, pathOfIdxToCreate, createNewIdx, nlpWS_endpoint);
					return true;
				}
				else if(start!=null && stop!=null){
					long startID=Long.parseLong(start);
					long stopID=Long.parseLong(stop);
					this.indexRecordsByDBidentifier(startID, stopID, pathOfIdxToCreate, createNewIdx, nlpWS_endpoint);
				}
				else if(libName!=null){
					String collections = elm.getTextContent().replaceAll("\n|\r|\t", "").trim();
					if(collections.length()>0){
						String[] oaiSets=collections.split(",( )*");
						oaiSetsToIndexGroupedByLibrary.put(libName, oaiSets);
					}//If no oaiSet is specified as comma separated list in the contents of the element means all of them
					else
						oaiSetsToIndexGroupedByLibrary.put(libName, null);
				}
			}
			
			if(oaiSetsToIndexGroupedByLibrary.size()>0)
				this.index(oaiSetsToIndexGroupedByLibrary, pathOfIdxToCreate, createNewIdx, nlpWS_endpoint);

			return true;
		} catch (Exception e) {
			logger.error("Error initializing Index Manager module.\n" + e.getMessage());
			return false;
		}
	}


    public boolean removeRecord(String recordID) {
        return false;
    }
    
	static public void main(String args[]) throws AxisFault {		
		Index_Manager mng = new Index_Manager();
		mng.initXIPGrammarsPaths();
		mng.executeTasksFromConfigurationFile(Index_Manager.defaultConfigurationFile);

	}
}
