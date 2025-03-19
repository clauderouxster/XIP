package eu.project.Cacao.Apps;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.Reader;
import java.text.Normalizer.Form;
import java.util.HashSet;
import java.util.List;

import org.apache.lucene.analysis.Analyzer;
import org.apache.lucene.analysis.KeywordAnalyzer;
import org.apache.lucene.analysis.LowerCaseFilter;
import org.apache.lucene.analysis.PerFieldAnalyzerWrapper;
import org.apache.lucene.analysis.StopAnalyzer;
import org.apache.lucene.analysis.TokenStream;
import org.apache.lucene.analysis.WhitespaceTokenizer;
import org.apache.lucene.analysis.WordlistLoader;
import org.apache.lucene.analysis.de.GermanAnalyzer;
import org.apache.lucene.analysis.fr.FrenchAnalyzer;
import org.apache.lucene.analysis.snowball.SnowballAnalyzer;
import org.apache.lucene.analysis.standard.StandardAnalyzer;
import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;
import org.apache.lucene.index.CorruptIndexException;
import org.apache.lucene.index.IndexReader;
import org.apache.lucene.index.IndexWriter;
import org.apache.lucene.store.LockObtainFailedException;
import org.getopt.stempel.lucene.StempelAnalyzer;

import classificationExplorer.ClassificationExplorer;
import classificationExplorer.Item;
import eu.project.Cacao.Components.Utils.Iso639;

public class LuceneIndexMerger {

	static private IndexWriter writer;
	static private ClassificationExplorer classificationExplorer;

	// static private Hashtable<String,Vector<String>> idmap;
	public static HashSet<String> loadStopwordsFromArray(String[] array) {
		HashSet<String> stopwords = new HashSet<String>();

		for (String tmp : array)
			stopwords.add(tmp);

		return stopwords;
	}

	private static PerFieldAnalyzerWrapper getAnalyzer() {
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
		pfan.addAnalyzer("recordID", new KeywordAnalyzer());
		pfan.addAnalyzer("libraryID", new KeywordAnalyzer());

		pfan.addAnalyzer("tableOfContents_enStem", new SnowballAnalyzer("English",StopAnalyzer.ENGLISH_STOP_WORDS));
		pfan.addAnalyzer("description_enStem", new SnowballAnalyzer("English",StopAnalyzer.ENGLISH_STOP_WORDS));
		pfan.addAnalyzer("title_enStem", new SnowballAnalyzer("English",StopAnalyzer.ENGLISH_STOP_WORDS));
		pfan.addAnalyzer("subject_enStem", new SnowballAnalyzer("English",StopAnalyzer.ENGLISH_STOP_WORDS));
		
		pfan.addAnalyzer("tableOfContents_frStem", new SnowballAnalyzer("French", FrenchAnalyzer.FRENCH_STOP_WORDS));
		pfan.addAnalyzer("description_frStem", new SnowballAnalyzer("French", FrenchAnalyzer.FRENCH_STOP_WORDS));
		pfan.addAnalyzer("title_frStem", new SnowballAnalyzer("French", FrenchAnalyzer.FRENCH_STOP_WORDS));
		pfan.addAnalyzer("subject_frStem", new SnowballAnalyzer("French", FrenchAnalyzer.FRENCH_STOP_WORDS));

		pfan.addAnalyzer("tableOfContents_deStem", new SnowballAnalyzer("German", GermanAnalyzer.GERMAN_STOP_WORDS));
		pfan.addAnalyzer("description_deStem", new SnowballAnalyzer("German", GermanAnalyzer.GERMAN_STOP_WORDS));
		pfan.addAnalyzer("title_deStem", new SnowballAnalyzer("German", GermanAnalyzer.GERMAN_STOP_WORDS));
		pfan.addAnalyzer("subject_deStem", new SnowballAnalyzer("German", GermanAnalyzer.GERMAN_STOP_WORDS));

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

	public static void main(String[] args) throws CorruptIndexException, IOException {
		if(args.length==0){
			removeDuplicatesRecordsFromOldIndex("idxCACAO");
		}
		else if(args.length==1){
			adjustIndex(args[0]);
		}
		else{
			for(int i=1;i<args.length;i++)
				addIndexToAlreadyExistingOne(args[0],args[i]);
		}
	}

	static public void createNewIndexFromOldOne(String newPath, String oldPath) {
		try {
			File INDEX_DIR = new File(newPath);

			boolean create = true;

			if (!INDEX_DIR.exists()) {
				INDEX_DIR.mkdirs();
			}

			PerFieldAnalyzerWrapper pfan = getAnalyzer();

			writer = new IndexWriter(INDEX_DIR, pfan, create, IndexWriter.MaxFieldLength.LIMITED);

			LuceneIndexMerger.removeDuplicatesRecordsFromOldIndex(oldPath);

			writer.optimize();
			writer.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	static public void removeDuplicatesRecordsFromOldIndex(String dir) throws CorruptIndexException, IOException {
		IndexReader reader = IndexReader.open(dir);
		
		PerFieldAnalyzerWrapper pfan = getAnalyzer();
		writer = new IndexWriter(dir+"_2", pfan, true, IndexWriter.MaxFieldLength.LIMITED);
		
		PrintWriter pwP = new PrintWriter(new FileWriter("PERS.txt"), true);
		PrintWriter pwG = new PrintWriter(new FileWriter("GEO.txt"), true);
		PrintWriter pwO = new PrintWriter(new FileWriter("ORG.txt"), true);
		
		int badDocuments = 0;
		HashSet<String> recordIDs = new HashSet<String>(reader.numDocs());
		for (int i = 0; i < reader.numDocs(); i++) {
			Document doc = reader.document(i);

			String rid = doc.getField("recordID").stringValue().trim();

			if (recordIDs.contains(rid)) {
				badDocuments++;
			} else {
				writer.addDocument(doc);
				recordIDs.add(rid);

				for (String s : doc.getValues("namedEntity_PERSON"))
						pwP.println(s);

				for (String s : doc.getValues("namedEntity_GEO"))
						pwG.println(s);

				for (String s : doc.getValues("namedEntity_ORGANISATION"))
						pwO.println(s);
				
			}
			if ((i % 1000) == 0) {
				System.out.println(i);

			}
		}
		
		pwP.close();
		pwG.close();
		pwO.close();
		
		writer.optimize();
		writer.close();
		reader.close();

		System.out.println("Duplicated Records: " + badDocuments + " out of " + reader.numDocs());
	}
	

	static public void addIndexToAlreadyExistingOne(String baseDir, String newDir) throws CorruptIndexException, IOException {
		PerFieldAnalyzerWrapper pfan = getAnalyzer();
		writer = new IndexWriter(baseDir, pfan, false, IndexWriter.MaxFieldLength.LIMITED);

		IndexReader reader = IndexReader.open(newDir);

		for (int i = 0; i < reader.numDocs(); i++) {
			//adjustDocumentAndAddToIndex(reader.document(i));
			Document doc=reader.document(i);
			writer.addDocument(doc);
		}
		
		writer.optimize();
		reader.close();
		writer.close();
	}
	
	static private void adjustIndex(String dir) throws CorruptIndexException, LockObtainFailedException, IOException{
		IndexReader reader = IndexReader.open(dir);
		
		PerFieldAnalyzerWrapper pfan = getAnalyzer();
		writer = new IndexWriter(dir+"_2", pfan, true, IndexWriter.MaxFieldLength.LIMITED);
		
		for (int i = 0; i < reader.numDocs(); i++) {
			Document doc =reader.document(i);
			Document newDoc = new Document();
			
			List<Field> list = doc.getFields();
			for(Field f: list){
				String s=f.stringValue();
				String t=java.text.Normalizer.normalize(s, Form.NFC);
				
				Field.Store store= Field.Store.YES;
				if(!f.isStored())
					store = Field.Store.NO;
				
				Field.Index indexStatus = Field.Index.ANALYZED;
				if(!f.isIndexed())
					indexStatus = Field.Index.NOT_ANALYZED;
				
				newDoc.add(new Field(f.name(), t, store, indexStatus));
			}
			writer.addDocument(newDoc);
		}
		writer.optimize();
		writer.close();
		reader.close();
	}
	
	static private boolean processDocumentAndAddToIndex(Document doc) {
		String s;
		String docLang = null;
		Document newDoc = new Document();

		Field[] fl = doc.getFields("publisher");
		if (fl != null) {
			for (Field f : fl) {
				s = f.stringValue();
				newDoc.add(new Field("publisher", s, Field.Store.YES, Field.Index.NOT_ANALYZED));
			}
		}

		fl = doc.getFields("URL");
		if (fl != null) {
			for (Field f : fl) {
				s = f.stringValue();
				newDoc.add(new Field("URL", s, Field.Store.YES, Field.Index.NOT_ANALYZED));
			}
		}

		fl = doc.getFields("collectionID");
		if (fl != null) {
			for (Field f : fl) {
				s = f.stringValue();
				newDoc.add(new Field("OAIset", s, Field.Store.YES, Field.Index.ANALYZED));
			}
		}

		fl = doc.getFields("language");
		if (fl != null) {
			for (Field f : fl) {
				s = f.stringValue();
				Iso639 documentLanguage = Iso639.find(s);
				if (documentLanguage != null && documentLanguage.twoCharCode != null)
					s = documentLanguage.twoCharCode;

				newDoc.add(new Field("language", s, Field.Store.YES, Field.Index.NOT_ANALYZED));

				if (fl.length == 1) {
					if (documentLanguage != null && documentLanguage.twoCharCode != null)
						docLang = documentLanguage.twoCharCode;
				}
			}
		}

		fl = doc.getFields("type");
		if (fl != null) {
			for (Field f : fl) {
				s = f.stringValue();
				newDoc.add(new Field("type", s, Field.Store.YES, Field.Index.ANALYZED));
			}
		}

		fl = doc.getFields("identifier");
		if (fl != null) {
			for (Field f : fl) {
				s = f.stringValue();
				newDoc.add(new Field("identifier", s, Field.Store.YES, Field.Index.NOT_ANALYZED));
			}
		}

		fl = doc.getFields("title");
		if (fl != null) {
			for (Field f : fl) {
				s = f.stringValue();
				newDoc.add(new Field("title", s, Field.Store.YES, Field.Index.ANALYZED));

				if ((docLang != null) && (docLang.equalsIgnoreCase("en") || docLang.equalsIgnoreCase("de") || docLang.equalsIgnoreCase("fr"))) {
					newDoc.add(new Field("title_stem", s, Field.Store.YES, Field.Index.ANALYZED));
				}
			}
		}

		fl = doc.getFields("title_lemmatized");
		if (fl != null) {
			for (Field f : fl) {
				s = f.stringValue();
				newDoc.add(new Field("title_lemmatized", s, Field.Store.YES, Field.Index.ANALYZED));
			}
		}

		fl = doc.getFields("subject");
		if (fl != null) {
			for (Field f : fl) {
				s = f.stringValue();
				boolean isCat = false;
				if (s.contains("dcterms")) {
					s = s.replaceAll("dcterms:", "");
					if (s.startsWith("DDC>")) {
						s = s.substring(4);
						s = getClassificationCategoryPath("Dewey", s);
					}
					if (s != null) {
						isCat = true;
						newDoc.add(new Field("classificationCategory", s, Field.Store.YES, Field.Index.ANALYZED));
					}
				} else
					newDoc.add(new Field("subject", s, Field.Store.YES, Field.Index.ANALYZED));

				if (!isCat && s != null && (docLang != null) && (docLang.equalsIgnoreCase("en") || docLang.equalsIgnoreCase("de") || docLang.equalsIgnoreCase("fr"))) {
					newDoc.add(new Field("subject_stem", s, Field.Store.YES, Field.Index.ANALYZED));
				}
			}
		}

		fl = doc.getFields("subject_lemmatized");
		if (fl != null) {
			for (Field f : fl) {
				s = f.stringValue();
				if (!s.contains("dcterms"))
					newDoc.add(new Field("subject_lemmatized", s, Field.Store.YES, Field.Index.ANALYZED));
			}
		}

		Field documentID = doc.getField("recordID");
		if (documentID != null) {
			newDoc.add(new Field("recordID", documentID.stringValue(), Field.Store.YES, Field.Index.NOT_ANALYZED));
			try {
				writer.addDocument(newDoc);
				return true;
			} catch (Exception e) {
				return false;
			}
		}

		return false;

	}

	static private String getClassificationCategoryPath(String classificationSystemID, String categoryID) {
		String path = "";
		List<Item> list = classificationExplorer.getCategoryItems(classificationSystemID, categoryID);
		if (list == null || list.size() == 0) {
			if (classificationSystemID.equals("Dewey")) {
				if (categoryID.indexOf('.') > 0) {
					String guessedParentID = categoryID.substring(0, categoryID.indexOf('.'));
					list = classificationExplorer.getCategoryItems(classificationSystemID, guessedParentID);
					if (list == null || list.size() == 0)
						return null;
					else
						path = guessedParentID + "/";
				} else
					return null;
			} else
				return null;
		}

		String parentID = list.get(0).getParentId();
		while (parentID != null) {
			path = parentID + "/" + path;
			List<Item> lista = classificationExplorer.getCategoryItems(classificationSystemID, parentID);
			if (lista.size() < 1)
				parentID = null;
			else {
				String newparentID = lista.get(0).getParentId();
				if (!newparentID.equals(parentID)) {
					parentID = newparentID;
				} else
					parentID = null;
			}
		}
		path = classificationSystemID + "/" + path + categoryID;

		return path;
	}

	static public class MySimpleAnalyzer extends Analyzer {
		/**
		 * Builds an analyzer with the stop words from the given file.
		 * 
		 * @see WordlistLoader#getWordSet(File)
		 */
		public MySimpleAnalyzer() {
		}

		/** Filters LowerCaseTokenizer with StopFilter. */
		public TokenStream tokenStream(String fieldName, Reader reader) {
			return new LowerCaseFilter(new WhitespaceTokenizer(reader));
		}
	}
}
