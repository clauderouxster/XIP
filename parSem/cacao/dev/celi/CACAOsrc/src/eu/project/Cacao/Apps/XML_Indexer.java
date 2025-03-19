package eu.project.Cacao.Apps;

import java.io.File;
import java.io.IOException;
import java.io.StringWriter;
import java.util.Hashtable;
import java.util.Vector;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.log4j.Logger;
import org.apache.lucene.analysis.KeywordAnalyzer;
import org.apache.lucene.analysis.PerFieldAnalyzerWrapper;
import org.apache.lucene.analysis.de.GermanAnalyzer;
import org.apache.lucene.analysis.fr.FrenchAnalyzer;
import org.apache.lucene.analysis.snowball.SnowballAnalyzer;
import org.apache.lucene.analysis.standard.StandardAnalyzer;
import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;
import org.apache.lucene.index.CorruptIndexException;
import org.apache.lucene.index.IndexWriter;
import org.apache.lucene.store.LockObtainFailedException;
import org.getopt.stempel.lucene.StempelAnalyzer;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

import com.sun.org.apache.xml.internal.serialize.OutputFormat;
import com.sun.org.apache.xml.internal.serialize.XMLSerializer;

import eu.project.Cacao.Apps.LuceneIndexMerger.MySimpleAnalyzer;
import eu.project.Cacao.Components.NamedEntitiesDetector;
import eu.project.Cacao.Components.Utils.Iso639;
import eu.project.Cacao.Components.Utils.Lemmatizer;
import eu.project.Cacao.Components.Utils.NLPtoken;
import eu.project.Cacao.Components.Utils.PolishLemmatizer;
import eu.project.Cacao.Components.Utils.RunTimer;
import eu.project.Cacao.Components.Utils.XIPwrapper;
import eu.project.Cacao.Connectors.Metadata.DublinCoreRecord;
import eu.project.Cacao.Connectors.Metadata.TelAPRecord;

public class XML_Indexer {

	private Logger logger = Logger.getLogger(XML_Indexer.class);
	private Hashtable<String, Lemmatizer> xipWrapperByLang;
	private IndexWriter writer;
	private String libraryName, defaultCollectionName, metadataTag,indexOutputFolder;
	private boolean isDCQ;
	
	public XML_Indexer(String libraryName, String defaultCollectionName, String metadataTag, 
			String indexOutputFolder, boolean createNewIDX ) throws CorruptIndexException, LockObtainFailedException, IOException {
		super();
		this.libraryName = libraryName;
		this.defaultCollectionName = defaultCollectionName;
		this.metadataTag = metadataTag;
		if (this.metadataTag.contains("tel"))
			isDCQ = true;
		else
			isDCQ = false;
		this.indexOutputFolder = indexOutputFolder;
		this.initXIPGrammarsPaths();
		
		PerFieldAnalyzerWrapper pfan = getAnalyzer();
		this.writer = new IndexWriter(indexOutputFolder, pfan, createNewIDX, IndexWriter.MaxFieldLength.LIMITED);
	}

	public static void main(String[] args) throws IOException {
		Logger logger = Logger.getLogger(XML_Indexer.class);

		String libraryName = args[0];
		String defaultCollectionName = args[1];
		String repositoryXML = args[2];
		String metadataTag = args[3]; //"tel:maybeTEL" "oai_dc:dc";
		String indexOutputFolder = args[4];

		
		XML_Indexer idx = new XML_Indexer(libraryName, defaultCollectionName, metadataTag, indexOutputFolder, true);		
		
		RunTimer rt = new RunTimer();
		logger.info("Indexing to directory '" + indexOutputFolder + "'...");

		int tot = idx.getCollectionsFromFiles(repositoryXML, libraryName, defaultCollectionName);
		
		idx.writer.optimize();
		idx.writer.close();
		logger.info("Indexed tot records :" + tot + " - time elapsed: " + rt.getElapsedTimeLabel());
	}
	
	
	/**
	 * Makes a document for a LibraryRrecord.
	 * 
	 * The document has three fields:
	 * 
	 * @param recordID
	 * 
	 */
	private void dcRecord2LuceneDocument(DublinCoreRecord dcdata, IndexWriter writer, String recordID) throws java.io.IOException {
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

		doc.add(new Field("libraryID", this.libraryName, Field.Store.YES, Field.Index.NOT_ANALYZED));
		doc.add(new Field("OAIset", this.defaultCollectionName, Field.Store.YES, Field.Index.NOT_ANALYZED));

		doc.add(new Field("recordID", recordID, Field.Store.YES, Field.Index.NOT_ANALYZED));

		for (String id : dcdata.getIdentifier()) {
			String fieldName = null;
			if (id.startsWith("http://"))
				fieldName = "identifier_URL";
			else
				fieldName = ("identifier");
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
		// this.addDCelement("rights", dcdata.getRights(), doc);
		this.addDCelement("type", dcdata.getType(), doc);

		Hashtable<String, String> neEntities = new Hashtable<String, String>();
		neEntities.putAll(this.addDCelement("description", dcdata.getDescription(), null, doc, lang));
		neEntities.putAll(this.addDCelement("title", removeTrailingPunctation(dcdata.getTitle()), null, doc, lang));
		neEntities.putAll(this.addDC_SubjectElement("subject", dcdata.getSubject(), dcdata.getSubjectAtts(), doc, lang));

		for (String ne : neEntities.keySet()) {
			String neCategory = neEntities.get(ne);
			if (neCategory.equalsIgnoreCase("PERSON")) {
				doc.add(new Field("namedEntity_PERSON", ne, Field.Store.YES, Field.Index.ANALYZED));
			} else if (neCategory.equalsIgnoreCase("ORGANISATION")) {
				doc.add(new Field("namedEntity_ORGANISATION", ne, Field.Store.YES, Field.Index.ANALYZED));
			} else if (neCategory.equalsIgnoreCase("LOCATION") || neCategory.equalsIgnoreCase("LOCORG")) {
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
	 * @param recordID
	 * 
	 */
	private void telapRecord2LuceneDocument(TelAPRecord telapData, IndexWriter writer, String recordID) throws java.io.IOException {
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

		doc.add(new Field("libraryID", this.libraryName, Field.Store.YES, Field.Index.NOT_ANALYZED));

		doc.add(new Field("OAIset", this.defaultCollectionName, Field.Store.YES, Field.Index.NOT_ANALYZED));

		doc.add(new Field("recordID", recordID, Field.Store.YES, Field.Index.NOT_ANALYZED));

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
		// this.addDCelement("rights", telapData.getRights(), doc);
		this.addDCelement("type", telapData.getType(), telapData.getTypeAtts(), doc);

		Hashtable<String, String> neEntities = new Hashtable<String, String>();
		neEntities.putAll(this.addDCelement("description", telapData.getDescription(), telapData.getDescriptionAtts(), doc, lang));
		neEntities.putAll(this.addDCelement("title", telapData.getTitle(), telapData.getTitleAtts(), doc, lang));
		neEntities.putAll(this.addDCelement("tableOfContents", telapData.getTableOfContents(), telapData.getTableOfContentsAtts(), doc, lang));
		neEntities.putAll(this.addDC_SubjectElement("subject", telapData.getSubject(), telapData.getSubjectAtts(), doc, lang));

		for (String ne : neEntities.keySet()) {
			String neCategory = neEntities.get(ne);
			if (neCategory.equalsIgnoreCase("PERSON")) {
				doc.add(new Field("namedEntity_PERSON", ne, Field.Store.YES, Field.Index.ANALYZED));
			} else if (neCategory.equalsIgnoreCase("ORGANISATION")) {
				doc.add(new Field("namedEntity_ORGANISATION", ne, Field.Store.YES, Field.Index.ANALYZED));
			} else if (neCategory.equalsIgnoreCase("LOCATION") || neCategory.equalsIgnoreCase("LOCORG")) {
				doc.add(new Field("namedEntity_GEO", ne, Field.Store.YES, Field.Index.ANALYZED));
			}
		}

		this.addTextualContentAsContentsField(doc);

		writer.addDocument(doc);
	}

	private void addTextualContentAsContentsField(Document doc) {
		StringBuffer buffer = new StringBuffer();
		String[] temps = doc.getValues("title");
		if (temps != null) {
			for (String s : temps)
				buffer = buffer.append(s + " ");
		}
		temps = doc.getValues("tableOfContents");
		if (temps != null) {
			for (String s : temps)
				buffer = buffer.append(s + " ");
		}
		temps = doc.getValues("subject");
		if (temps != null) {
			for (String s : temps)
				buffer = buffer.append(s + " ");
		}
		temps = doc.getValues("description");
		if (temps != null) {
			for (String s : temps)
				buffer = buffer.append(s + " ");
		}
		doc.add(new Field("contents", buffer.toString().trim(), Field.Store.YES, Field.Index.ANALYZED));

		buffer = new StringBuffer();
		temps = doc.getValues("title_lemmatized");
		if (temps != null) {
			for (String s : temps)
				buffer = buffer.append(s + " ");
		}
		temps = doc.getValues("tableOfContents_lemmatized");
		if (temps != null) {
			for (String s : temps)
				buffer = buffer.append(s + " ");
		}
		temps = doc.getValues("subject_lemmatized");
		if (temps != null) {
			for (String s : temps)
				buffer = buffer.append(s + " ");
		}
		temps = doc.getValues("description_lemmatized");
		if (temps != null) {
			for (String s : temps)
				buffer = buffer.append(s + " ");
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

	private void initXIPGrammarsPaths() {
		xipWrapperByLang = new Hashtable<String, Lemmatizer>();
		xipWrapperByLang.put("de", new XIPwrapper(XIPwrapper.deGram, "de"));
		xipWrapperByLang.put("en", new XIPwrapper(XIPwrapper.enGram, "en"));
		xipWrapperByLang.put("fr", new XIPwrapper(XIPwrapper.frGram, "fr"));
		xipWrapperByLang.put("it", new XIPwrapper(XIPwrapper.itGram, "it"));
		//xipWrapperByLang.put("pl", new PolishLemmatizer());
	}

	private Vector<NLPtoken> lemmatize(String lang, String toBeLemmatized) {
		try {

			Vector<NLPtoken> list = new Vector<NLPtoken>();
			if (lang.equalsIgnoreCase("it") || lang.equalsIgnoreCase("en") || lang.equalsIgnoreCase("de") || lang.equalsIgnoreCase("fr")||lang.equalsIgnoreCase("pl")) {
				Lemmatizer lemmatizer = this.xipWrapperByLang.get(lang);
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
		for (int i = 0; dcElm != null && i < dcElm.size(); i++) {
			String value = dcElm.elementAt(i);
			if (dcElmAttrs != null && dcElmAttrs.size() > 0) {
				Hashtable<String, String> attrs = dcElmAttrs.elementAt(i);
				if (attrs.containsKey("type")) {
					String type = attrs.get("type");
					doc.add(new Field(fieldName, type + " > " + value, Field.Store.YES, Field.Index.ANALYZED));
				} else
					doc.add(new Field(fieldName, value, Field.Store.YES, Field.Index.ANALYZED));
			} else
				doc.add(new Field(fieldName, value, Field.Store.YES, Field.Index.ANALYZED));
		}
	}

	private Hashtable<String, String> addDCelement(String fieldName, Vector<String> dcElm, Vector<Hashtable<String, String>> dcElmAttrs, Document doc, String lang) {
		Hashtable<String, String> neEntities = new Hashtable<String, String>();
		for (int i = 0; dcElm != null && i < dcElm.size(); i++) {
			String value = dcElm.elementAt(i);

			Hashtable<String, String> attrs = null;
			if (dcElmAttrs != null)
				attrs = dcElmAttrs.elementAt(i);

			String localLang = lang;
			if (attrs != null && attrs.containsKey("lang")) {
				Iso639 documentLanguage = Iso639.find(attrs.get("lang"));
				if (documentLanguage != null && documentLanguage.twoCharCode != null)
					localLang = documentLanguage.twoCharCode;
			}

			if (value.trim().length() > 0) {
				doc.add(new Field(fieldName, value.trim(), Field.Store.YES, Field.Index.ANALYZED));
				if (localLang.equalsIgnoreCase("en") || localLang.equalsIgnoreCase("fr") || localLang.equalsIgnoreCase("de") || localLang.equalsIgnoreCase("it")
						|| localLang.equalsIgnoreCase("pl") || localLang.equalsIgnoreCase("hu")) {
					doc.add(new Field(fieldName + "_" + localLang + "Stem", value, Field.Store.YES, Field.Index.ANALYZED));
				}
			}

			Vector<NLPtoken> analyzed = this.lemmatize(localLang, value);
			if (analyzed.size() > 0) {
				String lemmatizedStr = "";
				for (NLPtoken tok : analyzed) {
					lemmatizedStr += tok.getLemma_Category() + " ";
					if (tok.isNE) {
						neEntities.put(tok.surface, tok.neCategory);
					}
				}
				if (lemmatizedStr.trim().length() > 0)
					doc.add(new Field(fieldName + "_lemmatized", lemmatizedStr.trim(), Field.Store.YES, Field.Index.ANALYZED));
			}
		}
		return neEntities;
	}

	private Hashtable<String, String> addDC_SubjectElement(String fieldName, Vector<String> dcElm, Vector<Hashtable<String, String>> dcElmAttrs, Document doc, String lang) {
		Hashtable<String, String> neEntities = new Hashtable<String, String>();
		for (int i = 0; i < dcElm.size(); i++) {
			String value = dcElm.elementAt(i);
			Hashtable<String, String> attrs = dcElmAttrs.elementAt(i);

			String localLang = lang;
			if (attrs != null && attrs.containsKey("lang")) {
				Iso639 documentLanguage = Iso639.find(attrs.get("lang"));
				if (documentLanguage != null && documentLanguage.twoCharCode != null)
					localLang = documentLanguage.twoCharCode;
			}

			String type = null;
			if (attrs != null && attrs.containsKey("type")) {
				type = attrs.get("type");
			}

			boolean isText = false;
			String lemmatizedStr = "";
			if (localLang != null && localLang.length() > 0) {

				Vector<NLPtoken> analyzed = this.lemmatize(localLang, value);
				for (NLPtoken tok : analyzed) {
					lemmatizedStr += tok.getLemma_Category() + " ";

					if (tok.category!=null && (tok.category.equalsIgnoreCase("NOUN") || tok.category.equalsIgnoreCase("VERB") || tok.category.equalsIgnoreCase("ADJ")
							|| tok.category.equalsIgnoreCase("NADJ")) )
						isText = true;

					if (tok.isNE) {
						neEntities.put(tok.surface, tok.neCategory);
					}
				}
			}

			if (isText) {
				doc.add(new Field(fieldName + "_lemmatized", lemmatizedStr.trim(), Field.Store.YES, Field.Index.ANALYZED));
				if (localLang.equalsIgnoreCase("en") || localLang.equalsIgnoreCase("fr") || localLang.equalsIgnoreCase("de") || localLang.equalsIgnoreCase("it")
						|| localLang.equalsIgnoreCase("pl") || localLang.equalsIgnoreCase("hu")) {
					doc.add(new Field(fieldName + "_" + localLang + "Stem", value, Field.Store.YES, Field.Index.ANALYZED));
				}
				doc.add(new Field(fieldName, value, Field.Store.YES, Field.Index.ANALYZED));
			}

			if (type != null && type.length() > 0) {
				doc.add(new Field(fieldName + "_ClassificationCategory", type + "/" + value, Field.Store.YES, Field.Index.NOT_ANALYZED));
			} else if (!isText) {
				doc.add(new Field(fieldName, value, Field.Store.YES, Field.Index.NOT_ANALYZED));
			}
		}
		return neEntities;
	}

	private void add_IdentifierElement(Document doc, String fieldName, Vector<String> values, Vector<Hashtable<String, String>> attrsOfValues) {

		for (int i = 0; i < values.size(); i++) {
			String id = values.elementAt(i);
			Hashtable<String, String> attrs = attrsOfValues.elementAt(i);
			if (attrs.containsKey("type")) {
				String value = attrs.get("type");
				if (value.equalsIgnoreCase("tel:URL")) {
					doc.add(new Field(fieldName + "_URL", id, Field.Store.YES, Field.Index.NOT_ANALYZED));
				} else if (value.equalsIgnoreCase("tel:ISBN")) {
					doc.add(new Field(fieldName + "_ISBN", id, Field.Store.YES, Field.Index.NOT_ANALYZED));
				}
				if (value.equalsIgnoreCase("tel:ISSN")) {
					doc.add(new Field(fieldName + "_ISSN", id, Field.Store.YES, Field.Index.NOT_ANALYZED));
				}

			} else
				doc.add(new Field(fieldName, id, Field.Store.YES, Field.Index.NOT_ANALYZED));
		}
	}

	private PerFieldAnalyzerWrapper getAnalyzer() {
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

		pfan.addAnalyzer("tableOfContents_enStem", new SnowballAnalyzer("English"));
		pfan.addAnalyzer("description_enStem", new SnowballAnalyzer("English"));
		pfan.addAnalyzer("title_enStem", new SnowballAnalyzer("English"));
		pfan.addAnalyzer("subject_enStem", new SnowballAnalyzer("English"));
		
		pfan.addAnalyzer("tableOfContents_frStem", new FrenchAnalyzer());
		pfan.addAnalyzer("description_frStem", new FrenchAnalyzer());
		pfan.addAnalyzer("title_frStem", new FrenchAnalyzer());
		pfan.addAnalyzer("subject_frStem", new FrenchAnalyzer());

		pfan.addAnalyzer("tableOfContents_deStem", new GermanAnalyzer());
		pfan.addAnalyzer("description_deStem", new GermanAnalyzer());
		pfan.addAnalyzer("title_deStem", new GermanAnalyzer());
		pfan.addAnalyzer("subject_deStem", new GermanAnalyzer());

		String[] itStopwords = new String[] { "a", "adesso", "ai", "al", "alla", "allo", "allora", "anche", "avere", "aveva", "avevano", "ben", "buono", "che", "chi", "con",
				"cui", "da", "del", "della", "dello", "di", "e", "fare", "fra", "ha", "hai", "hanno", "ho", "il", "invece", "io", "la", "le", "lei", "lo", "loro", "lui", "ma",
				"me", "nei", "nella", "no", "noi", "o", "oltre", "qua", "quello", "questo", "qui", "quindi", "senza", "sia", "siamo", "siete", "sono", "stati", "stato", "stesso",
				"sul", "sulla", "te", "tra", "un", "una", "uno" };
		pfan.addAnalyzer("tableOfContents_itStem", new SnowballAnalyzer("Italian", itStopwords));
		pfan.addAnalyzer("description_itStem", new SnowballAnalyzer("Italian", itStopwords));
		pfan.addAnalyzer("title_itStem", new SnowballAnalyzer("Italian", itStopwords));
		pfan.addAnalyzer("subject_itStem", new SnowballAnalyzer("Italian", itStopwords));

		String[] huStopwords = new String[] {"a", "az", "egy", "be", "ki", "le", "fel", "meg", "el", "át", "rá", "ide", "oda", "szét", "össze", "vissza", "de", 
				"hát", "és", "vagy", "hogy", "van", "lesz", "volt", "csak", "nem", "igen", "mint", "én", "te", "õ", "mi", "ti", "õk", "ön" };
		pfan.addAnalyzer("tableOfContents_huStem", new SnowballAnalyzer("Hungarian", huStopwords));
		pfan.addAnalyzer("description_huStem", new SnowballAnalyzer("Hungarian", huStopwords));
		pfan.addAnalyzer("title_huStem", new SnowballAnalyzer("Hungarian", huStopwords));
		pfan.addAnalyzer("subject_huStem", new SnowballAnalyzer("Hungarian", huStopwords));

		pfan.addAnalyzer("tableOfContents_plStem", new StempelAnalyzer());
		pfan.addAnalyzer("description_plStem", new StempelAnalyzer());
		pfan.addAnalyzer("title_plStem", new StempelAnalyzer());
		pfan.addAnalyzer("subject_plStem", new StempelAnalyzer());

		return pfan;
	}

	public int getCollectionsFromFiles(String repositoryXML, String libraryName, String defaultCollectionName) throws IOException {
		File dir = new File(repositoryXML);
		logger.info("-Descending into folder :"+dir.getAbsolutePath());	
		File[] files = dir.listFiles();
		int cnt = 0;
		for (File f : files) {
			if (f.isDirectory()) {
				logger.info("-Descending into folder :"+f.getAbsolutePath());
				int n= getCollectionsFromFiles(f.getAbsolutePath(), libraryName, defaultCollectionName);
				logger.info("-Indexed "+f.getAbsolutePath()+" with "+n+" records");
				cnt +=n;
			} else {
				int n= parseRecordsFromFile(f.getAbsolutePath(), libraryName, defaultCollectionName);
				logger.info("Indexed "+f.getAbsolutePath()+" with "+n+" records");
				cnt +=n;
				if(cnt%1000 == 0)
					logger.info(">> "+this.indexOutputFolder+" "+cnt+"... ");
			}
		}
		return cnt;
	}

	public int parseRecordsFromFile(String fname, String libraryName, String defaultCollectionName) {
		Logger logger = Logger.getLogger(XML_Indexer.class);
		int i = 0;
		try {
			// getting the document builder Factory
			DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();

			// if the document has to be validated against the schema source

			// getting the document builder from the Factory
			DocumentBuilder builder = factory.newDocumentBuilder();

			// parses the xml
			org.w3c.dom.Document myDocument = builder.parse(new File(fname));
			NodeList nl = myDocument.getElementsByTagName("record");
			for (; i < nl.getLength(); i++) {
				Element record = (Element) nl.item(i);
				Element idElm = (Element) record.getElementsByTagName("id").item(0);
				String id = idElm.getTextContent();
				
				Element metadata = (Element) record.getElementsByTagName(metadataTag).item(0);// oai_dc:dc
				metadata.setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
				metadata.setAttribute("xmlns:tel", "http://krait.kb.nl/coop/tel/handbook/telterms.html");
				metadata.setAttribute("xmlns:dcmitype", "http://purl.org/dc/dcmitype/");
				metadata.setAttribute("xmlns:oai_dc", "http://www.openarchives.org/OAI/2.0/oai_dc/");
				metadata.setAttribute("xmlns:dcterms", "http://purl.org/dc/terms/");
				metadata.setAttribute("xmlns:dc", "http://purl.org/dc/elements/1.1/");
				metadata.setAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
				metadata.setAttribute("xmlns:mods", "http://www.loc.gov/mods");
				
				StringWriter tmp = new StringWriter();
				XMLSerializer serializer = new XMLSerializer(tmp, new OutputFormat(myDocument, "UTF-8", true));
				serializer.serialize(metadata);

				if (this.isDCQ){
					TelAPRecord taprd = new TelAPRecord(id, tmp.toString());
					this.telapRecord2LuceneDocument(taprd, this.writer, id);
				}
				else{
					DublinCoreRecord dcrd = new DublinCoreRecord(id, tmp.toString());
					this.dcRecord2LuceneDocument(dcrd, this.writer, id);
				}
				
				if(i%100 == 0)
					logger.info(i);
			}

			return i;
		} catch (Exception e) {
			logger.error("Error parsing the message from file: \n" + fname + "\n" + e);

			return i;
		}
	}
}
