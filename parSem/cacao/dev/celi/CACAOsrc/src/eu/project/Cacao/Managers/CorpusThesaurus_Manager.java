package eu.project.Cacao.Managers;

import it.celi.RandomIndexing.termFilters.RegExpTermFilter;
import it.celi.RandomIndexing.utils.VectorStoreWriter;
import it.celi.RandomIndexing.vectorsGenerators.TermsWindowAsContext;

import java.io.File;
import java.io.IOException;
import java.util.HashSet;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.log4j.Logger;
import org.apache.lucene.analysis.KeywordAnalyzer;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

import eu.project.Cacao.Components.Utils.RunTimer;

/**
 * Generates semantic wordspaces using HAL approach on documents metadata from a lucene index and stores them in  .bin files.
 * 
 * @author Alessio
 * 
 */
public class CorpusThesaurus_Manager {
	// Tag & attributes names used in the xml configuration file
	public final static String svSearcherTagname = "corpusThesaurus", svRootTagname = "corpusThesaurusConfiguration", langAttrLabel = "lang", luceneAttrLabel = "luceneIDX",
			contentFieldsLabel = "contents", lCaseSearchKeysAttrLabel = "lowerCaseSearchKeys", semanticVectorsSizeAttrLabel = "semanticVectorsSize",
			textWindowSizeAttrLabel = "textWindowSize", collectionsAttrLabel = "collections", termFilterRegExpAttrLabel = "termFilterRegExp",
			termFilterInvalidSuffixesAttrLabel = "termFilterInvalidSuffixes", collectionFieldNameAttrLabel = "collectionFieldName", langFieldNameAttrLabel = "langFieldName",
			seedLenghtAttrLabel = "seedLenght", subtractMeanAttrLabel = "subtractMean", minimumTermFrequencyAttrLabel = "minimumTermFrequency";

	private static Logger logger = Logger.getLogger(CorpusThesaurus_Manager.class);

	static public void main(String args[]) {
		CorpusThesaurus_Manager mng = new CorpusThesaurus_Manager();
		mng.retrieveTasksFromConfigurationFile("configuration/CorpusThesaurusManagerConfiguration.xml");
	}

	public CorpusThesaurus_Manager() {
	}

	/**
	 * Generates a vector model based on the corpus-based distributional
	 * semantic approach from a lucene index
	 * 
	 * @param luceneSrcDir
	 *            Folder containing the lucene index
	 * @param semanticVectorsIdxFile
	 *            the name for the index file (.bin) to be generated and stored
	 *            on the folder specified by the baseDir attribute of the class
	 * @param contentsFields
	 *            the names of lucene fields bearing contents to be analyzed
	 * @param collectionsIDs
	 *            Strings representing the collections [oai sets] (i.e.
	 *            Österreichische Nationalbibliothek) (both lang and library
	 *            collections IDs are used for generate a filter query on the
	 *            lucene index)
	 * @param lang
	 *            the language of the generated resource (both lang and library
	 *            collections IDs are used for generate a filter query on the
	 *            lucene index)
	 * @param collectionIDfieldName
	 *            The name of lucene field containing the collectionID
	 * @param languageFieldName
	 *            The name of lucene field containing the record language
	 * @param seedLength
	 *            used to specify the number of initial 1 and -1 in the random
	 *            vectors (use 4 if not sure..)
	 * @param semanticVectorsSize
	 *            Represent the cardinality of random indexes used to model the
	 *            different domain contexts (here text windows, see HAL approach
	 *            in literature) usual values range from 300 to 1500 depending
	 *            on the lexicon size.
	 * @param textWindowSize
	 *            the size of the text window around a team defining its context
	 * @param subtractMean
	 *            Specify if subtract the mean value from each semantic vector
	 *            in order to remove noise (use true if not sure..)
	 * @param textValidationRegularExpression
	 *            regular expression that defines which textual tokens are
	 *            accepted by the TermFilter
	 * @param invalidSuffixes
	 *            an array of String representing invalid word suffixes (to be
	 *            used with fields lemmatized in the form lemma_POS)
	 * @param minFreq
	 *            the minimum frequency a Term should have in order to be
	 *            included in the lexicon (used to remove misspelled forms)
	 * @param lowerCaseSearch
	 *            it tells if the tokens in the search index (and pos suffixes
	 *            as well) are lowercase
	 * @return Exit Status
	 */
	public boolean generateSemanticWordSpace(String luceneSrcDir, String semanticVectorsFile, String[] contentsFields, String[] collectionsIDs, String lang,
			String collectionIDfieldName, String languageFieldName, int seedLength, int semanticVectorsSize, int textWindowSize, boolean subtractMean,
			String textValidationRegularExpression, String[] invalidSuffixes, int minFreq, boolean lowerCaseSearch) {
		
		RunTimer rt = new RunTimer();
		
		try {

			// Create TermFilter
			RegExpTermFilter tf = new RegExpTermFilter(minFreq, luceneSrcDir, textValidationRegularExpression); // a-zàèéìòù
			for (String suffix : invalidSuffixes) {
				if (lowerCaseSearch)
					tf.addInvalidSuffix(suffix.toLowerCase());
				else
					tf.addInvalidSuffix(suffix);
			}

			for (String field : contentsFields)
				tf.addAllowedField(field);

			// Create lucene filterQuery
			String filterQuery = this.createFilterQuery(lang, languageFieldName, collectionsIDs);

			HashSet<String> contentFields = new HashSet<String>();
			for (String cf : contentsFields)
				contentFields.add(cf);

			TermsWindowAsContext vecStore = new TermsWindowAsContext(luceneSrcDir, seedLength, minFreq, textWindowSize, contentFields, subtractMean);
			if (filterQuery != null && collectionIDfieldName != null) {
				vecStore.setQueryFilter(filterQuery, new KeywordAnalyzer(), collectionIDfieldName);
				tf.setBitSet(vecStore.getQueryFilterBitSet(vecStore.getIndexReader()));
			}
			vecStore.setTermFilter(tf);

			vecStore.generateTermVector();
			if (subtractMean) {
				vecStore.subtractMean();
				logger.info("Subtracting mean value");
			}
			VectorStoreWriter vecWriter = new VectorStoreWriter();
			// String user="cacaoUser", pwd="cacao", server="demo.celi.it",db="CACAO";
			// DBVectorStore dbVecWriter = new DBVectorStore( user, pwd, server,db);
			logger.info("Writing term vectors to " + semanticVectorsFile+". "+rt.getElapsedTimeLabel());
			vecWriter.WriteVectors(semanticVectorsFile, vecStore);
			// dbVecWriter.writeVectors("telBL","British Library","en", vecStore);
			// vecWriter.WriteVectorsAsText(termFile, vecStore);

			return true;
		} catch (IOException e) {
			logger.error("Error generating corpus-based semantic vectors for: " + luceneSrcDir + ", for lang: " + lang + ", on collections: " + collectionsIDs + "\n"
					+ e.getMessage());
			return false;
		}

	}

	private String createFilterQuery(String lang, String languageFieldName, String[] collectionsIDs) {
		String filterQuery = "";
		if (lang != null)
			filterQuery += "+" + languageFieldName + ":" + lang + " ";

		String collectionSubFilterQuery = "";
		for (int i=0; collectionsIDs!=null && i<collectionsIDs.length;i++){
			String collection=collectionsIDs[i];
			collectionSubFilterQuery += collection + " ";
		}
		if (collectionSubFilterQuery.length() > 0)
			filterQuery += "+(\"" + collectionSubFilterQuery.trim() + "\")";

		if (filterQuery.length() == 0)
			filterQuery = null;
		else
			filterQuery = filterQuery.trim();

		return filterQuery;
	}

	/**
	 * method for librarians manual fine tuning, still to be implemented
	 * 
	 * @param libraryID
	 * @param term
	 * @param lang
	 * @param synset
	 * @return
	 */
	public boolean setTermSynset(String libraryID, String term, String lang, String[] synset) {
		return false;
	}

	/**
	 * Read initialization parameters from an xml file for generating semantic
	 * vector spaces from a lucene index
	 * 
	 * @param xmlConfigurationFile
	 *            Configuration file path
	 * @return exit status
	 */
	private boolean retrieveTasksFromConfigurationFile(String xmlConfigurationFile) {
		try {
			logger.debug("Init CorpusThesaurus Component from " + xmlConfigurationFile);
			// getting the document builder Factory
			DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();

			// if the document has to be validated against the schema source

			// getting the document builder from the Factory
			DocumentBuilder builder = factory.newDocumentBuilder();

			// parses the xml
			Document myDocument = builder.parse(new File(xmlConfigurationFile));

			Element root = myDocument.getDocumentElement();
			String tmp = root.getAttribute(CorpusThesaurus_Manager.lCaseSearchKeysAttrLabel);
			boolean lowerCaseSearchKey = Boolean.parseBoolean(tmp);
			tmp = root.getAttribute(CorpusThesaurus_Manager.semanticVectorsSizeAttrLabel);
			int svSize = Integer.parseInt(tmp);
			tmp = root.getAttribute(CorpusThesaurus_Manager.textWindowSizeAttrLabel);
			int txtWindowSize = Integer.parseInt(tmp);
			tmp = root.getAttribute(CorpusThesaurus_Manager.seedLenghtAttrLabel);
			int seedLength = Integer.parseInt(tmp);
			tmp = root.getAttribute(CorpusThesaurus_Manager.subtractMeanAttrLabel);
			boolean subtractMean = Boolean.parseBoolean(tmp);
			String termFilter_RexExp = root.getAttribute(CorpusThesaurus_Manager.termFilterRegExpAttrLabel);
			tmp = root.getAttribute(CorpusThesaurus_Manager.termFilterInvalidSuffixesAttrLabel);
			String[] invalidSuffixes = tmp.split(",");
			tmp = root.getAttribute(CorpusThesaurus_Manager.minimumTermFrequencyAttrLabel);
			int minFreq = Integer.parseInt(tmp);

			NodeList nl = root.getElementsByTagName(CorpusThesaurus_Manager.svSearcherTagname);
			for (int i = 0; i < nl.getLength(); i++) {
				Element elm = (Element) nl.item(i);
				String lang = elm.getAttribute(CorpusThesaurus_Manager.langAttrLabel);
				String luceneIDX = elm.getAttribute(CorpusThesaurus_Manager.luceneAttrLabel);
				tmp = elm.getAttribute(CorpusThesaurus_Manager.contentFieldsLabel);
				String[] contentsFields = tmp.split(",");
				tmp = elm.getAttribute(CorpusThesaurus_Manager.collectionsAttrLabel);
				String[] collections=null;
				if(tmp.trim().length()>0)
				 collections = tmp.split(",");

				String collectionsFname = elm.getAttribute(CorpusThesaurus_Manager.collectionFieldNameAttrLabel);
				String langFname = elm.getAttribute(CorpusThesaurus_Manager.langFieldNameAttrLabel);
				String semVectFilePath = elm.getTextContent().replaceAll("\n|\r|\t", "");

				this.generateSemanticWordSpace(luceneIDX, semVectFilePath, contentsFields, collections, lang, collectionsFname, langFname, seedLength, svSize, txtWindowSize,
						subtractMean, termFilter_RexExp, invalidSuffixes, minFreq, lowerCaseSearchKey);
			}

			return true;
		} catch (Exception e) {
			logger.error("Error initializing CorpusThesaurus component.\n" + e.getMessage());

			return false;
		}
	}
}
