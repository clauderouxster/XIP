package eu.project.Cacao.Apps.CLEF;

import java.io.File;
import java.io.IOException;
import java.util.Date;
import java.util.Hashtable;
import java.util.Vector;

import org.apache.axis2.AxisFault;
import org.apache.lucene.analysis.Analyzer;
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

import eu.project.Cacao.Apps.LuceneIndexMerger.MySimpleAnalyzer;
import eu.project.Cacao.Components.Utils.Iso639;
import eu.project.Cacao.Components.Utils.NLPtoken;
import eu.project.Cacao.Connectors.Clef.ClefAccess;
import eu.project.Cacao.Connectors.DB.Library;
import eu.project.Cacao.Connectors.DB.LibraryCollection;
import eu.project.Cacao.Connectors.DB.LibraryRecord;
import eu.project.Cacao.Connectors.DB.MySQL;
import eu.project.Cacao.Connectors.Metadata.DublinCoreRecord;
import eu.project.Cacao.WS_Clients.CACAO_nlpWS_client;

public class ClefIndexer {
	private File INDEX_DIR;
	private ClefAccess clef;
	private CACAO_nlpWS_client lemmatizer;
	private String libraryID = null;
	private int rid = 0;

	public ClefIndexer(String host, String user, String pwd, File outputFolder, String libraryID, String nlpWS_endpoint) throws AxisFault {
		// Connect to DB
		MySQL cacaodb = new MySQL();
		cacaodb.setServer(host);
		cacaodb.setDBname("CACAO");
		cacaodb.setUsername(user);
		cacaodb.setPassword(pwd);
		boolean success = cacaodb.connect();

		// Create new ClefAccess Object
		this.clef = new ClefAccess(cacaodb);

		// Creating the index directory if not exists
		this.libraryID = libraryID;
		this.INDEX_DIR = new File(outputFolder, "idx_" + this.libraryID);

		if (!this.INDEX_DIR.exists()) {
			this.INDEX_DIR.mkdirs();
		}

		// lemmatizers
		this.lemmatizer = new CACAO_nlpWS_client(nlpWS_endpoint);
	}

	public void index(boolean createIDX) {
		Date start = new Date();

		try {
			PerFieldAnalyzerWrapper pfan = new PerFieldAnalyzerWrapper(new MySimpleAnalyzer());
			pfan.addAnalyzer("title", new StandardAnalyzer());
			pfan.addAnalyzer("subject", new StandardAnalyzer());
			pfan.addAnalyzer("description", new StandardAnalyzer());
			pfan.addAnalyzer("OAIset", new KeywordAnalyzer());
			pfan.addAnalyzer("classificationCategory", new KeywordAnalyzer());
			pfan.addAnalyzer("type", new KeywordAnalyzer());
			
			IndexWriter writer = new IndexWriter(INDEX_DIR, pfan, createIDX, IndexWriter.MaxFieldLength.LIMITED);
			System.out.println("Indexing to directory '" + INDEX_DIR + "'...");

			indexClefData(writer);

			writer.optimize();
			writer.close();

			Date end = new Date();
			System.out.println(end.getTime() - start.getTime() + " total milliseconds");
		} catch (IOException e) {
			System.out.println(" caught a " + e.getClass() + "\n with message: " + e.getMessage());
		}
	}


	private void indexClefData(IndexWriter writer) {
        String[] libraries = clef.retrieveLibrariesID();

        String databaseLibraryID=null;
        for (int lib = 0; lib < libraries.length; lib++) {
            Library library = clef.retrieveLibrary(libraries[lib]);
            if(library.getName().equals(this.libraryID))
            	databaseLibraryID = library.getID();
        }
        
        if(databaseLibraryID== null){
        	System.out.println("Library with name: "+this.libraryID+" not present in the DB.");
        	return;
        }
        
		// get collections By Library ID
		String[] OAIsets = clef.retrieveCollectionsID(databaseLibraryID);

		for (String OAIset : OAIsets) {
			// Index all collection's records

			LibraryCollection oaiSet = clef.retrieveCollection(OAIset);

			// Retrieve all records of Collection
			String[] records = clef.retrieveRecordsID(oaiSet);

			System.out.println("Retrieve records from Collection: " + oaiSet.getDescription() + "(Library: " + oaiSet.getLibrary().getName() + ")");

			// For every record fetched...
			int cnt = 0;

			for (String recordID : records) {
				if ((this.rid != 0) && (Integer.parseInt(recordID.trim()) <= this.rid)) {
					continue;
				}

				// Get LibraryRecord object
				LibraryRecord record = clef.retrieveRecord(recordID, oaiSet);

				try {
					this.libraryRecord2LuceneDocument(record, libraryID, oaiSet.getCode(), writer);
					System.out.println("Indexed doc " + recordID);
				} catch (Throwable e) {
					e.printStackTrace();
				}
			}

			System.out.println("Retrieved " + records.length + " records\n");
		}

	}

	/**
	 * Makes a document for a LibraryRrecord.
	 * 
	 * The document has three fields:
	 * 
	 */
	private void libraryRecord2LuceneDocument(LibraryRecord record, String libraryID, String collectionID, IndexWriter writer) throws java.io.IOException {
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
		
		doc.add(new Field("libraryID", libraryID, Field.Store.YES, Field.Index.NOT_ANALYZED));
		doc.add(new Field("OAIset", collectionID, Field.Store.YES, Field.Index.NOT_ANALYZED));
		doc.add(new Field("recordID", record.getDatabaseID(), Field.Store.YES, Field.Index.NOT_ANALYZED));
		
		for (String id : dcdata.getIdentifier()) {
			String fieldName=null;
			if(id.startsWith("http://"))
				fieldName= "URL";
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
		this.addDCelement("rights", dcdata.getRights(), doc);
		this.addDCelement("type", dcdata.getType(), doc);
		
		Hashtable<String,String> neEntities=new Hashtable<String, String>();
		neEntities.putAll( this.addDCelement("description", dcdata.getDescription(), doc, lang) );
		neEntities.putAll( this.addDC_SubjectElement("subject", dcdata.getSubject(), dcdata.getSubjectAtts(), doc, lang) );
		neEntities.putAll( this.addDCelement("title", removeTrailingPunctation(dcdata.getTitle()), doc, lang) );
		
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
		
		if(lang!=null){
			Analyzer an=ClefIndexer.getAnalyzerForLang(lang);
			writer.addDocument(doc, an);
		}
		else
			writer.addDocument(doc);
	}

	private Vector<String> removeTrailingPunctation(Vector<String> src) {
		Vector<String> res = new Vector<String>();

		for (String s : src) {
			res.add(s.trim().replaceAll("/$", "").trim());
		}

		return res;
	}

	private Vector<NLPtoken>  lemmatize(String lang, String toBeLemmatized) {

		try {
			Vector<NLPtoken> list = this.lemmatizer.parsePhrase(toBeLemmatized, lang);
			return list;
		} catch (Exception e) {
			return null;
		}
	}

	private void addDCelement(String fieldName, Vector<String> dcElm, Document doc) {
		for (String value : dcElm) {
			doc.add(new Field(fieldName, value, Field.Store.YES, Field.Index.ANALYZED));
		}
	}

	private Hashtable<String,String> addDCelement(String fieldName, Vector<String> dcElm, Document doc, String lang) {
		Hashtable<String,String> neEntities=new Hashtable<String, String>();
		for (String value : dcElm) {		
			Vector<NLPtoken> analyzed = this.lemmatize(lang, value);
			String lemmatizedStr="", surfaceStr="";
			for(NLPtoken tok: analyzed){
				surfaceStr+=tok.surface+" ";
				lemmatizedStr+=tok.getLemma_Category()+" ";
				if(tok.isNE){
					neEntities.put(tok.surface, tok.neCategory);
				}
			}
			doc.add(new Field(fieldName, surfaceStr.trim(), Field.Store.YES, Field.Index.ANALYZED));
			doc.add(new Field(fieldName + "_lemmatized", lemmatizedStr.trim(), Field.Store.YES, Field.Index.ANALYZED));
			doc.add(new Field(fieldName + "_stem", surfaceStr, Field.Store.YES, Field.Index.ANALYZED));
		}
		return neEntities;
	}

	private Hashtable<String,String> addDC_SubjectElement(String fieldName, Vector<String> dcElm, Vector<Hashtable<String, String>> dcElmAttrs, Document doc, String lang) {
		Hashtable<String,String> neEntities=new Hashtable<String, String>();
		for (int i = 0; i < dcElm.size(); i++) {
			String value = dcElm.elementAt(i);

			Hashtable<String, String> attrs = dcElmAttrs.elementAt(i);
			String type = attrs.get("type");

			if ((type == null) || (type.length() == 0)) {
				type = "txt";
			}

			if (type.equals("txt")) {
				Vector<NLPtoken> analyzed = this.lemmatize(lang, value);
				String lemmatizedStr="", surfaceStr="";
				for(NLPtoken tok: analyzed){
					surfaceStr+=tok.surface+" ";
					lemmatizedStr+=tok.getLemma_Category()+" ";
					if(tok.isNE){
						neEntities.put(tok.surface, tok.neCategory);
					}
				}
				doc.add(new Field(fieldName, surfaceStr.trim(), Field.Store.YES, Field.Index.ANALYZED));
				doc.add(new Field(fieldName + "_lemmatized", lemmatizedStr.trim(), Field.Store.YES, Field.Index.ANALYZED));
				doc.add(new Field(fieldName + "_stem", surfaceStr, Field.Store.YES, Field.Index.ANALYZED));
				
			} else {
				// TODO: Get corresponding text from type / value pair of subject..
				doc.add(new Field(fieldName, type + "/" + value, Field.Store.YES, Field.Index.NOT_ANALYZED));
			}
		}
		return neEntities;
	}
	
	static private Analyzer getAnalyzerForLang(String lang) {
		PerFieldAnalyzerWrapper pfan = new PerFieldAnalyzerWrapper(new MySimpleAnalyzer());
		pfan.addAnalyzer("title", new StandardAnalyzer());
		pfan.addAnalyzer("subject", new StandardAnalyzer());
		pfan.addAnalyzer("description", new StandardAnalyzer());
		pfan.addAnalyzer("OAIset", new KeywordAnalyzer());
		pfan.addAnalyzer("classificationCategory", new KeywordAnalyzer());
		pfan.addAnalyzer("type", new KeywordAnalyzer());
		pfan.addAnalyzer("namedEntity_PERSON", new KeywordAnalyzer());
		pfan.addAnalyzer("namedEntity_ORGANISATION", new KeywordAnalyzer());
		pfan.addAnalyzer("namedEntity_GEO", new KeywordAnalyzer());
		

		if (lang.equals("en")) {
			pfan.addAnalyzer("description_stem", new SnowballAnalyzer("English", StopAnalyzer.ENGLISH_STOP_WORDS));
			pfan.addAnalyzer("title_stem", new SnowballAnalyzer("English", StopAnalyzer.ENGLISH_STOP_WORDS));
			pfan.addAnalyzer("subject_stem", new SnowballAnalyzer("English", StopAnalyzer.ENGLISH_STOP_WORDS));
		} else if (lang.equals("fr")) {
			pfan.addAnalyzer("description_stem", new SnowballAnalyzer("French", FrenchAnalyzer.FRENCH_STOP_WORDS));
			pfan.addAnalyzer("title_stem", new SnowballAnalyzer("French", FrenchAnalyzer.FRENCH_STOP_WORDS));
			pfan.addAnalyzer("subject_stem", new SnowballAnalyzer("French", FrenchAnalyzer.FRENCH_STOP_WORDS));
		}
		else if (lang.equals("de")) {
			pfan.addAnalyzer("description_stem", new SnowballAnalyzer("German", GermanAnalyzer.GERMAN_STOP_WORDS));
			pfan.addAnalyzer("title_stem", new SnowballAnalyzer("German", GermanAnalyzer.GERMAN_STOP_WORDS));
			pfan.addAnalyzer("subject_stem", new SnowballAnalyzer("German", GermanAnalyzer.GERMAN_STOP_WORDS));
		}
		else if (lang.equals("it")) {
			String[] itaStopwords=new String[]{"il","la","un","una","e","o"};
			pfan.addAnalyzer("description_stem", new SnowballAnalyzer("Italian", itaStopwords ));
			pfan.addAnalyzer("title_stem", new SnowballAnalyzer("Italian", itaStopwords ));
			pfan.addAnalyzer("subject_stem", new SnowballAnalyzer("Italian", itaStopwords));
		}

		return pfan;
	}

	public static void main(String[] args) throws AxisFault { 
		String nlpWSendpoint = "http://hermes.celi.it:8080/axis2/services/CACAO_nlpWS";
		File outputFolder=new File("stuff");


		ClefIndexer idx = new ClefIndexer("hermes.celi.it", "cacaoUser", "cacao", outputFolder, "uniBZ_test", nlpWSendpoint);

		idx.index(true);
	}
}
