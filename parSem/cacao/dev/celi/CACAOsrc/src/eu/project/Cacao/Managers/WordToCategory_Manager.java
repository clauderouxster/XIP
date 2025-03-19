package eu.project.Cacao.Managers;

import java.io.File;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.BitSet;
import java.util.Date;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.List;
import java.util.Set;
import java.util.Vector;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.log4j.Logger;
import org.apache.lucene.analysis.KeywordAnalyzer;
import org.apache.lucene.analysis.PerFieldAnalyzerWrapper;
import org.apache.lucene.document.Document;
import org.apache.lucene.index.IndexReader;
import org.apache.lucene.index.Term;
import org.apache.lucene.queryParser.QueryParser;
import org.apache.lucene.search.Query;
import org.apache.lucene.search.QueryFilter;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

import classificationExplorer.ClassificationExplorer;
import classificationExplorer.Item;
import eu.project.Cacao.Apps.CLEF.CLEFtopicProcessor.MySimpleAnalyzer;
import eu.project.Cacao.Components.Utils.NLPtoken;
import eu.project.Cacao.Components.Utils.RegExpTermFilter;
import eu.project.Cacao.Components.Utils.RunTimer;
import eu.project.Cacao.Components.Utils.TermFilter;
import eu.project.Cacao.Connectors.DB.MySQL;

/**
 * The class computes a membership values for each term in relation with each
 * library category given a common categorization (as DDC) and stores them in a
 * DB
 * 
 * @author Alessio, Daniele
 * 
 */

public class WordToCategory_Manager {
	
	public static final String classificationExplorerConfigurationFileAttrLabel = "classificationExplorerConfigurationFile",
	dbHostAttrLabel="dbHost", dbNameAttrLabel="dbName", userAttrLabel="user", pwdAttrLabel="pwd", word2CategoryResourceElm="word2CategoryResource",
	nlpServiceClientEndpointAttrLabel="nlpServiceEndpoint", contentsFieldNamesAttrLabel="contentsFieldNames", langAttrLabel="lang",
	langFieldNameAttrLabel="langFieldName", librarySetsAttrLabel="librarySets", librarySetsFieldNameAttrLabel="librarySetsFieldName",
	classificationSystemIDAttrLabel="classificationSystemID", classificationSystemFieldAttrLabel="classificationSystemFieldName", 
	luceneIndexAttrLabel="luceneIndex", dropAndCreateClassificationExplorerTablesAttrLabel="dropAndCreateClassificationExplorerTables",
	classificationSystemPrefixAttrLabel="classificationSystemPrefix";
	
	private ClassificationExplorer classificationExplorer;
	private MySQL cacaodb=null;
	private TermFilter registeredTF;
	
	protected Logger logger = Logger.getLogger(WordToCategory_Manager.class.getName());
	
    static public void main(String[] args) {
        try { // demo.celi.it:8095
        	System.err.close();
        	WordToCategory_Manager a=new WordToCategory_Manager();
        	a.retrieveTasksFromConfigurationFile("configuration/Word2CategoryManagerConfiguration.xml");
        } catch (Exception e) {
            e.printStackTrace();
        }
    } 
	
	public WordToCategory_Manager() {	}
	
	private boolean initDBConnection(String dbServer, String dbName, String user,  String pwd){
		try {
			this.cacaodb = new MySQL(user, pwd, dbServer, dbName);
			return cacaodb.connect();
		} catch (Exception e) {
			logger.error("Error in DB initialization host: "+dbServer+" dbName: "+dbName+" user: "+user+" pwd: "+pwd+".\n"+e);
			return false;
		}
		
	}
	
	private boolean initClassificationExplorer(String classificationExplorerConfiguarationFile, boolean dropAndCreateTables){
		try {
			this.classificationExplorer = new ClassificationExplorer(classificationExplorerConfiguarationFile, dropAndCreateTables);
			return true;
		} catch (Exception e) {
			logger.error("Error in ClassificationExplorer initialization from: "+classificationExplorerConfiguarationFile+" dropAndRecreateTables: "+dropAndCreateTables+".\n"+e);
			return false;
		}
	}
	
	public boolean visitAllClassificationCategoriesAndGenerateWordBags(String luceneIDX, String lang, String langFieldName, 
			String[] librarySetsIDs, String librarySetsFieldName, String[] contentsFieldsNames, String classificationSystemName,
			String classificationSystemFieldName, String classificationSystemPrefix, String mapID) {
		
		RunTimer rt = new RunTimer();
		
		try {
			IndexReader indexReader=IndexReader.open(luceneIDX);
			
			long mappingID_DB =this.addW2CMappingToDB(mapID,luceneIDX, librarySetsIDs);
			long classificationSystemID_DB=this.getWord2Cat_ClassificationSystemEntry(classificationSystemName);
			if(classificationSystemID_DB==-1)
				classificationSystemID_DB=this.addW2CatClassificationSystemToDB(classificationSystemName);
			
			List<String> toplevel = classificationExplorer.getTopLevelCategories(classificationSystemName);
			// go through the topmost categories
			for (String topLevelCat: toplevel) {
				// get the children of each top category please note that the whole branch is returned
				List<Item> currentChildren = classificationExplorer.getChildrenItems(classificationSystemName, topLevelCat);

				// go through each category
				for (Item category: currentChildren) {
					// do something with the ID
					logger.debug("Computing category membership for: "+category);
					HashSet<NLPtoken> terms = this.computeCategoryMembership(indexReader, librarySetsIDs, librarySetsFieldName,
							lang, langFieldName, category.getItemId(), classificationSystemFieldName, contentsFieldsNames, classificationSystemPrefix);
					this.setCategoryRelatedTerms(mappingID_DB, classificationSystemID_DB, category, terms, lang);
				}
			}
			
			logger.info("Generated word bags with Word2Cat using index "+luceneIDX+" on library OAI sets: ["+librarySetsIDs+"]. "+rt.getElapsedTimeLabel());
			return true;
		} catch (Exception e) {
			logger.error("Error in creating W2C resource for: "+luceneIDX+" lang: "+lang+".\n"+e);
			return false;
		}

	}
	

	private HashSet<NLPtoken> filterLabelTerms(Vector<NLPtoken> termsVector, float defaultConfidence){
		HashSet<NLPtoken> results = new HashSet<NLPtoken>();
		for(NLPtoken tok: termsVector){
			if(tok.category.matches("(NOUN|NADJ|VERB|ADJ|ADJA)")){
				tok.setConfidence(defaultConfidence);
				results.add(tok);
			}
		}
		return results;
	}
	

	protected HashSet<NLPtoken> computeCategoryMembership(IndexReader indexReader, String[] librarySetsIDs, String librarySetsFieldName,
			String lang, String langFieldName, String categoryID , String categoryField, String[] contentsFields, String classificationSystemPrefix) {
		HashSet<NLPtoken> nlpTokenSet = new HashSet<NLPtoken>();
		
		if (this.registeredTF == null)
			this.registeredTF = this.createDefaultTermFilter(indexReader);

		BitSet toAccept = this.createQueryFilterBitSet(indexReader, librarySetsIDs, librarySetsFieldName, lang, langFieldName, categoryID, categoryField, classificationSystemPrefix);
		for (int doc = 0; doc < indexReader.numDocs(); doc++) {
			try {
				// check against query filter BitSet
				if (toAccept != null && !toAccept.get(doc)) {
					continue;
				}

				// Retrieve the tsextual contents from designated fields
				Document luceneDoc = indexReader.document(doc);
				Hashtable<String, String[]> documentWordsByFields = new Hashtable<String, String[]>();
				for (String contentsFieldname : contentsFields) {
					StringBuffer buff = new StringBuffer("");
					String[] tmps = indexReader.document(doc).getValues(contentsFieldname);
					for (String s : tmps) {
						buff = buff.append(s + " ");
					}
					String[] words = buff.toString().trim().split(" ");
					documentWordsByFields.put(contentsFieldname, words);
				}

				// Filter terms and creates a bag of Words (that can be further
				// post-processed)
				for (String fieldName : documentWordsByFields.keySet()) {
					for (String token : documentWordsByFields.get(fieldName)) {
						Term term = new Term(fieldName, token.toLowerCase());
						if (this.registeredTF.acceptTerm(term)) {
							String POS = "", lemma = token;
							int idx = token.lastIndexOf('_');
							if (idx > 0) {
								lemma = token.substring(0, idx);
								POS = token.substring(idx + 1);
								POS = POS.toUpperCase();
							}

							NLPtoken tok = new NLPtoken(lemma, POS, lemma, null, lang);
							float relativeTF = (float) (this.registeredTF.getTermFrequency() / toAccept.cardinality());
							tok.setConfidence(relativeTF);
							nlpTokenSet.add(tok);
						}
					}
				}

			} catch (Exception e) {
				logger.error("Error while processing document num. " + doc + ".\n" + e.getMessage());
			}
		}

		return nlpTokenSet;
	}

	// method for librarians manual fine tuning mapID, classificationSystemID, category.getItemId(), lang, terms
	public boolean setCategoryRelatedTerms(long mapID, long classificationSystemID, Item category, Set<NLPtoken> terms, String lang) {
		
		long catID = this.getWord2Cat_ClassificationCategoryEntry(classificationSystemID, category.getItemId());
		if(catID<0){
			catID = this.addWord2Cat_ClassificationCategoryEntry(classificationSystemID, category.getItemId());
			if(catID>=0)
				this.addWord2Cat_ClassificationCategoryLabel(catID, lang, category.getLabel());
		}
		else{
			if(!this.existWord2Cat_ClassificationCategoryLabel(catID, lang))
				this.addWord2Cat_ClassificationCategoryLabel(catID, lang, category.getLabel());
		}
		if(catID>=0){
			for(NLPtoken tok: terms){
				long termID = this.getWord2Cat_TermEntry(tok);
				if(termID<0)
					termID=this.addWord2Cat_TermEntry(tok);
				if(termID>=0){
					if(this.addW2CatMappingResourceToDB(mapID, catID, termID)<0 )
						logger.error("Cannot insert mapping resource between "+tok+" and " +catID+" "+category.getItemId()
								+" for mapID (DB id):"+mapID+"\n");
				}
				else{
					logger.error("Cannot insert term "+tok+" for mapID (DB id):"+mapID+"\n");
				}
			}
			return true;
		}	
		return false;
	}

	private BitSet createQueryFilterBitSet(IndexReader indexReader, String[] librarySetsIDs, String librarySetsFieldName,
			String lang, String langFieldName, String classificationCategoryID, String classificationCategoryField, String classificationSystemPrefix) {
		
		if(classificationSystemPrefix==null)
			classificationSystemPrefix="";
		String queryString ="+"+classificationCategoryField+":"+classificationSystemPrefix+classificationCategoryID+" ";
		if(lang!=null)
			queryString+="+"+langFieldName+":"+lang+" ";

		String librarySetsSubFilterQuery = "";
		for (int i=0; librarySetsIDs!=null && i<librarySetsIDs.length;i++){
			String librarySet=librarySetsIDs[i];
			librarySetsSubFilterQuery += "\""+librarySet + "\" ";
		}
		if (librarySetsSubFilterQuery.length() > 0)
			queryString += "+(" + librarySetsSubFilterQuery.trim() + ")";

		PerFieldAnalyzerWrapper pfan = new PerFieldAnalyzerWrapper(new MySimpleAnalyzer());
		pfan.addAnalyzer(librarySetsFieldName, new KeywordAnalyzer());
		pfan.addAnalyzer(classificationCategoryField, new KeywordAnalyzer());
		QueryParser queryParser = new QueryParser(librarySetsFieldName, pfan);

		try {
			queryParser.setLowercaseExpandedTerms(false);
			Query q = queryParser.parse(queryString);
			logger.info("Parsed query:{" + q.toString() + "}");
			QueryFilter myQueryFilter = new QueryFilter(q);

			BitSet toAccept = myQueryFilter.bits(indexReader);
			logger.info("Filtering should keep:" + toAccept.cardinality() + "|" + toAccept.size());
			return toAccept;
		} catch (Exception e) {
			logger.error("Error parsing query filter expression\n " + e.getMessage());
			return null;
		}

	}

	protected TermFilter createDefaultTermFilter(IndexReader indexReader) {
		int minFreq = 2;
		RegExpTermFilter tf = new RegExpTermFilter(minFreq, indexReader, "[^ 1234567890]+"); // a-z������
		tf.addInvalidSuffix("_punct");
		tf.addInvalidSuffix("_coord");
		tf.addInvalidSuffix("_det");
		tf.addInvalidSuffix("_prep");
		tf.addInvalidSuffix("_dig");
		tf.addInvalidSuffix("_num");
		tf.addInvalidSuffix("_conj");
		tf.addInvalidSuffix("_sent");

		tf.addAllowedField("title_lemmatized");
		//tf.addAllowedField("subject_lemmatized");

		return tf;
	}

	private String getLucenePrefix4CategoryID(String classificationID) {
		if (classificationID.equals("Dewey"))
			return "DDC>";
		else
			return "";
	}

	public void registerTermFilter(TermFilter registeredTF) {
		this.registeredTF = registeredTF;
	}
	
	/**
	 * Read initialization parameters from an xml file for generating semantic
	 * vector spaces from a lucene index
	 * 
	 * @param xmlConfigurationFile
	 *            Configuration file path
	 * @return exit status
	 */
	public boolean retrieveTasksFromConfigurationFile(String xmlConfigurationFile) {
		try {
			logger.debug("Init Word2Cat Manager from " + xmlConfigurationFile);
			// getting the document builder Factory
			DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();

			// if the document has to be validated against the schema source

			// getting the document builder from the Factory
			DocumentBuilder builder = factory.newDocumentBuilder();

			// parses the xml
			org.w3c.dom.Document myDocument = builder.parse(new File(xmlConfigurationFile));

			Element root = myDocument.getDocumentElement();
			String classificationExplorerPropertiesFile = root.getAttribute(WordToCategory_Manager.classificationExplorerConfigurationFileAttrLabel);
			String tmp = root.getAttribute(WordToCategory_Manager.dropAndCreateClassificationExplorerTablesAttrLabel);
			boolean dropAndCreateTables=Boolean.parseBoolean(tmp);
			//As a source of information for supported classifications
			if(!this.initClassificationExplorer(classificationExplorerPropertiesFile, dropAndCreateTables))
				return false;
			
			String dbHost = root.getAttribute(WordToCategory_Manager.dbHostAttrLabel);
			String dbName = root.getAttribute(WordToCategory_Manager.dbNameAttrLabel);
			String user = root.getAttribute(WordToCategory_Manager.userAttrLabel);
			String pwd = root.getAttribute(WordToCategory_Manager.pwdAttrLabel);
			//For writing computed bag of words
			if(!this.initDBConnection(dbHost, dbName, user, pwd))
				return false;
			
			NodeList nl = root.getElementsByTagName(WordToCategory_Manager.word2CategoryResourceElm);
			for (int i = 0; i < nl.getLength(); i++) {
				Element elm = (Element) nl.item(i);
				String luceneIDX = elm.getAttribute(WordToCategory_Manager.luceneIndexAttrLabel);
				String lang = elm.getAttribute(WordToCategory_Manager.langAttrLabel);
				String langField = elm.getAttribute(WordToCategory_Manager.langFieldNameAttrLabel);
				tmp = elm.getAttribute(WordToCategory_Manager.contentsFieldNamesAttrLabel);
				String[] contentsFields = tmp.split(",");
				tmp = elm.getAttribute(WordToCategory_Manager.librarySetsAttrLabel);
				String[] librarySets = tmp.split(",");
				String librarySetsIDsFieldName = elm.getAttribute(WordToCategory_Manager.librarySetsFieldNameAttrLabel);
				String classificationSystemID = elm.getAttribute(WordToCategory_Manager.classificationSystemIDAttrLabel);
				String classificationSystemFieldName = elm.getAttribute(WordToCategory_Manager.classificationSystemFieldAttrLabel);
				String classificationSystemPrefix = elm.getAttribute(WordToCategory_Manager.classificationSystemPrefixAttrLabel);
				String mapID = elm.getTextContent().replaceAll("\n|\r|\t", "");
				boolean result = this.visitAllClassificationCategoriesAndGenerateWordBags(luceneIDX,  lang, langField, librarySets, 
						librarySetsIDsFieldName, contentsFields, classificationSystemID, classificationSystemFieldName, classificationSystemPrefix, mapID);

			}

			return true;
		} catch (Exception e) {
			logger.error("Error initializing CorpusThesaurus component.\n" + e.getMessage());

			return false;
		}
	}
	
	/**** DB related methods *********/
	
	private long addW2CMappingToDB(String mapID, String luceneIDX, String[] sets) {
		long ID = -1;
		String description="index: "+luceneIDX+" - sets: "+sets;
		try {
			
			PreparedStatement stmt = this.cacaodb.getConnection().prepareStatement("INSERT INTO Word2Cat_Mappings(Name, TaskDescription, Date) VALUES(?,?,?);");
			stmt.setString(1, mapID);
			stmt.setString(2, description);
			DateFormat df=new SimpleDateFormat("yyyyy.MM.dd hh:mm aaa");
			String date = df.format(new Date());
			stmt.setString(3, date);
			stmt.execute();

			ResultSet resID = stmt.getGeneratedKeys();

			if (resID.next()) {
				ID = resID.getLong(1);
				logger.debug("Inserted Wor2Cat Mappings: " + mapID +" - "+description+" as " + ID);
			}

			stmt.close();
		} catch (Exception e) {
			logger.error("Can't insert Wor2Cat Mappings: " + mapID +" - "+description+ "\n" + e.getMessage());
		}
		return ID;
	}
	
	private long getWord2Cat_ClassificationSystemEntry(String classificationSystemName) {
		long ID = -1;
		try {

			PreparedStatement stmt = this.cacaodb.getConnection().prepareStatement("SELECT ID from Word2Cat_ClassificationSystem where Name=? ;");

			stmt.setString(1, classificationSystemName);
			stmt.execute();

			ResultSet rs = stmt.getResultSet();

			if (rs.first()) {
				ID = rs.getLong(1);
			}

			stmt.close();
		} catch (Exception e) {
			logger.error("Can't select ClassificationSystem entry with " + classificationSystemName +"\n" + e.getMessage());
		}
		return ID;
	}
	
	private long addW2CatClassificationSystemToDB(String classificationSystemName) {
		long ID = -1;
		try {
			
			PreparedStatement stmt = this.cacaodb.getConnection().prepareStatement("INSERT INTO Word2Cat_ClassificationSystem(Name) VALUES(?);");
			stmt.setString(1, classificationSystemName);
			stmt.execute();

			ResultSet resID = stmt.getGeneratedKeys();

			if (resID.next()) {
				ID = resID.getLong(1);
				logger.debug("Inserted Wor2Cat ClassificationSystem: " + classificationSystemName +" as " + ID);
			}

			stmt.close();
		} catch (Exception e) {
			logger.error("Can't insert Wor2Cat ClassificationSystem: " + classificationSystemName+ "\n" + e.getMessage());
		}
		return ID;
	}
	
	private long getWord2Cat_ClassificationCategoryEntry(long  classificationSystemID, String categoryID) {
		long ID = -1;
		try {

			PreparedStatement stmt = this.cacaodb.getConnection().prepareStatement("SELECT ID from Word2Cat_ClassificationCategory where ClassificationSystemID=? AND CategoryID=?;");

			stmt.setLong(1, classificationSystemID);
			stmt.setString(2, categoryID);
			stmt.execute();

			ResultSet rs = stmt.getResultSet();

			if (rs.first()) {
				ID = rs.getLong(1);
			}

			stmt.close();
		} catch (Exception e) {
			logger.error("Can't select category entry with " + classificationSystemID +" "+categoryID+"\n" + e.getMessage());
		}
		return ID;
	}
	
	private long addWord2Cat_ClassificationCategoryEntry(long classificationID, String categoryID) {
		long ID = -1;
		try {

			PreparedStatement stmt = this.cacaodb.getConnection().prepareStatement("INSERT INTO Word2Cat_ClassificationCategory(ClassificationSystemID, CategoryID) VALUES(?,?);");
			
			stmt.setLong(1, classificationID);
			stmt.setString(2, categoryID);
			stmt.execute();

			ResultSet resID = stmt.getGeneratedKeys();

			if (resID.next()) {
				ID = resID.getLong(1);
				logger.debug("Inserted Wor2Cat ClassificationCategory: " + classificationID +" "+categoryID+" as " + ID);
			}

			stmt.close();
		} catch (Exception e) {
			logger.error("Can't insert Wor2Cat ClassificationCategory: " + classificationID +" "+categoryID+ "\n" + e.getMessage());
		}
		return ID;
	}
	
	private boolean existWord2Cat_ClassificationCategoryLabel(long classificationCategoryID, String lang) {
		long ID = -1;
		try {

			PreparedStatement stmt = this.cacaodb.getConnection().prepareStatement("SELECT ID from Word2Cat_ClassificationCategoriesLabels where ClassificationCategoryID=? AND lang=?  ;");

			stmt.setLong(1, classificationCategoryID);
			stmt.setString(2, lang);
			stmt.execute();

			ResultSet rs = stmt.getResultSet();

			if (rs.first()) {
				ID = rs.getLong(1);
				return true;
			}

			stmt.close();
		} catch (Exception e) {
			logger.error("Can't select ClassificationCategoryLabel entry with " + classificationCategoryID +" "+lang+"\n" + e.getMessage());
		}
		return false;
	}
	
	private long addWord2Cat_ClassificationCategoryLabel(long classificationCategoryID, String lang, String label) {
		long ID = -1;
		try {

			PreparedStatement stmt = this.cacaodb.getConnection().prepareStatement("INSERT INTO Word2Cat_ClassificationCategoriesLabels(ClassificationCategoryID, lang, label) VALUES(?,?,?);");
			
			stmt.setLong(1, classificationCategoryID);
			stmt.setString(2, lang);
			stmt.setString(3, label);
			stmt.execute();

			ResultSet resID = stmt.getGeneratedKeys();

			if (resID.next()) {
				ID = resID.getLong(1);
				logger.debug("Inserted Wor2Cat ClassificationCategoryLabel: " + classificationCategoryID +" "+lang+" "+label+" as " + ID);
			}

			stmt.close();
		} catch (Exception e) {
			logger.error("Can't insert Wor2Cat ClassificationCategoryLabel: " + classificationCategoryID +" "+lang+" "+label+"\n" + e.getMessage());
		}
		return ID;
	}
	
	private long getWord2Cat_TermEntry(NLPtoken term){
		return getWord2Cat_TermEntry(term.lemma, term.category, term.lang);
	}
	
	private long getWord2Cat_TermEntry(String term, String POS, String lang) {
		long ID = -1;
		try {

			PreparedStatement stmt = this.cacaodb.getConnection().prepareStatement("SELECT ID from Word2Cat_Terms where Term=? AND POS=? AND language=?  ;");

			stmt.setString(1, term);
			stmt.setString(2, POS);
			stmt.setString(2, lang);
			stmt.execute();

			ResultSet rs = stmt.getResultSet();

			if (rs.first()) {
				ID = rs.getLong(1);
			}

			stmt.close();
		} catch (Exception e) {
			logger.error("Can't select Term entry with " + term +"\n" + e.getMessage());
		}
		return ID;
	}
    
	private long addWord2Cat_TermEntry(NLPtoken term){
		return addWord2Cat_TermEntry(term.lemma, term.category, term.lang);
	}
	
    private long addWord2Cat_TermEntry(String term, String POS, String lang) {
		long ID = -1;
		try {

			PreparedStatement stmt = this.cacaodb.getConnection().prepareStatement("INSERT INTO Word2Cat_Terms(Term, POS, language) VALUES(?,?,?);");
			stmt.setString(1, term);
			stmt.setString(2, POS);
			stmt.setString(3, lang);
			stmt.execute();

			ResultSet resID = stmt.getGeneratedKeys();

			if (resID.next()) {
				ID = resID.getInt(1);
				logger.debug("inserted Term: " + term + " " + POS + " "+lang+" as " + ID);
			}

			stmt.close();
		} catch (Exception e) {
			logger.error("Can't insert Term: " + term + " " + POS +" "+lang+ "\n" + e.getMessage());
		}
		return ID;
	}
    
    private long addW2CatMappingResourceToDB(long mapID, long catID, long termID) {
		long ID = -1;
		try {

			PreparedStatement stmt = this.cacaodb.getConnection().prepareStatement("INSERT INTO Word2Cat_MapResource(TermID, CategoryID, MappingID) VALUES(?,?,?);");
			stmt.setLong(1, termID);
			stmt.setLong(2, catID);
			stmt.setLong(3, mapID);
			stmt.execute();

			ResultSet resID = stmt.getGeneratedKeys();

			if (resID.next()) {
				ID = resID.getInt(1);
				logger.debug("inserted mapping resource between term: " + termID + " and " + catID + " mapID: "+mapID+" as " + ID);
			}

			stmt.close();
		} catch (Exception e) {
			logger.error("Can't insert Term: " + termID + " and " + catID + " mapID: "+mapID+ "\n" + e.getMessage());
		}
		return ID;
	}
    
    private int removeW2CatMappingResourceFromDB(long mapID, long catID, long termID) {
    	int deleted = 0;
		try {

			PreparedStatement stmt = this.cacaodb.getConnection().prepareStatement("DELETE FROM Word2Cat_MapResource WHERE TermID=? AND CategoryID=? AND MappingID=? ;");
			stmt.setLong(1, termID);
			stmt.setLong(2, catID);
			stmt.setLong(3, mapID);
			deleted= stmt.executeUpdate();

			logger.debug("deleted mapping resource between term: " + termID + " and catID: " + catID + " and mapID: "+mapID);

			stmt.close();
		} catch (Exception e) {
			logger.error("Can't insert Term: " + termID + " and catID: " + catID + " and mapID: "+mapID+ "\n" + e.getMessage());
		}
		return deleted;
	}
    
    private int removeW2CatMappingResourceFromDB(long mapID, long catID) {
    	int deleted = 0;
		try {

			PreparedStatement stmt = this.cacaodb.getConnection().prepareStatement("DELETE FROM Word2Cat_MapResource WHERE CategoryID=? AND MappingID=? ;");
			stmt.setLong(1, catID);
			stmt.setLong(2, mapID);
			deleted= stmt.executeUpdate();

			logger.debug("deleted mapping resource for catID: " +  + catID + " mapID: "+mapID);

			stmt.close();
		} catch (Exception e) {
			logger.error("Can't delete mapping resource for catID: " + catID + " mapID: "+mapID+ "\n" + e.getMessage());
		}
		return deleted;
	}
    
    private int removeW2CatMappingResourceFromDB(long mapID) {
    	int deleted = 0;
		try {

			PreparedStatement stmt = this.cacaodb.getConnection().prepareStatement("DELETE FROM Word2Cat_MapResource WHERE MappingID=? ;");
			stmt.setLong(1, mapID);
			deleted= stmt.executeUpdate();

			logger.debug("deleted mapping resource for mapID: "+mapID);

			stmt.close();
		} catch (Exception e) {
			logger.error("Can't delete mapping resource for mapID: "+mapID+ "\n" + e.getMessage());
		}
		return deleted;
	}
}
