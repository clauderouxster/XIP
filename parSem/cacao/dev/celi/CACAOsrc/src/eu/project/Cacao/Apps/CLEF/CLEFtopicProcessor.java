package eu.project.Cacao.Apps.CLEF;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.io.Reader;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.TreeSet;
import java.util.Vector;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.log4j.Logger;
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
import org.apache.lucene.document.Field;
import org.apache.lucene.queryParser.QueryParser;
import org.apache.lucene.search.Hits;
import org.apache.lucene.search.IndexSearcher;
import org.apache.lucene.search.Query;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

import eu.project.Cacao.Components.Utils.NLPtoken;
import eu.project.Cacao.Managers.QueryManager;


public class CLEFtopicProcessor extends QueryManager{
    static private Logger logger = Logger.getLogger(QueryManager.class.getName());
    
    private String langFrom;
    private String targetCollection;
    private String fname;
    private String runID;
    private String outFname;
    private String evalResultsFilename;
    private String title = "";
    private String queryID = "";
    private String content = "";
    
    private boolean isMonoLanguage;
    private boolean expandWithClassificationCategories;
    private boolean expandWithSemVect;
    private boolean queryOnContentType;
    private boolean expandWithWordnet;
    private boolean queryOnStemmed;
    private boolean printRelRetStatistics;
    
    private PrintWriter out;
    private IndexSearcher luceneSearcher = null;
    private Vector<NLPtoken> titleKW;
    
    private float topic_Boost = 1.4f;
    private float expandedKWboost = 0.2f;
    private float contentType_Boost = 1f;
    private float NEboost = 2f;
    private float translatedKWBoost = 0.2f;
    private float titleKW_boost = 1.4f;
    private float ANDboost = 9.5f;
    private float w2cboost =2f;
    private float ORboost = 2.4f;
    
    final int queryResultsNumberUpperBound = 1000;
    
    private Hashtable<String, HashSet<String>> relevantDocs;

    public CLEFtopicProcessor(String langFrom, String targetCollection, String topicFname, String runID, boolean isQueryExpanded,
        boolean queryOnContentType, boolean isMonolingual, boolean expandWithWN, boolean expandWithW2C, boolean queryOnStemmed, boolean relRetStats,
        String luceneIDX) throws Exception {
        super(true);
       
    	this.langFrom = langFrom;
        this.targetCollection = targetCollection;
        this.fname = topicFname;
        this.runID = runID;
        this.expandWithSemVect = isQueryExpanded;
        this.queryOnContentType = queryOnContentType;
        this.isMonoLanguage = isMonolingual;
        this.expandWithWordnet = expandWithWN;
        this.queryOnStemmed = queryOnStemmed;
        this.printRelRetStatistics = relRetStats;
        this.expandWithClassificationCategories = expandWithW2C; 
        
        //Create the lucene searcher
        this.luceneSearcher = new IndexSearcher(luceneIDX);

        this.outFname = "/data/trec_eval.8.1/ClefOut_" + this.runID + ".txt";
        this.evalResultsFilename = "/data/trec_eval.8.1/official_en";

        if (this.printRelRetStatistics) {
            this.relevantDocs = CLEFtopicProcessor.getRelDocs(this.evalResultsFilename);
        }
    }

    public static void main(String[] args){
        CLEFtopicProcessor ctp = CLEFtopicProcessor.createCLEFtopicProcessorFromInitFile("/data/Sviluppo/CACAO/configuration/initClefTopicRun.txt");
        
        try {
			ctp.processTopics();
			ctp.performEvaluation();
		} catch (IOException e) {
			e.printStackTrace();
		}
    }
    
    /**
	 * Extract the value from a String in the form name = value
	 * 
	 * @param s
	 *            the property in the form nameProperty = value
	 * @return the value as a String
	 */
    private static String extractValueFromNameValuePairs(String s) {
    	Logger logger = Logger.getLogger(QueryManager.class.getName());
        String[] tmps = s.split("=");

        if ((tmps != null) && (tmps.length == 2)) {
            logger.info("Reading property " + tmps[0] + " = " + tmps[1]);
            return tmps[1].trim();
        } else {
            logger.error("Error reading property -> " + s);
            return null;
        }
    }
    
    static private CLEFtopicProcessor createCLEFtopicProcessorFromInitFile(
        String initConfigurationFile) {
        try {
            logger.info("Initing the CACAO WS from file " + initConfigurationFile + ".");
            
            BufferedReader br = new BufferedReader(new FileReader(initConfigurationFile));
            String topicFname = extractValueFromNameValuePairs(br.readLine().trim());
            String langFrom = extractValueFromNameValuePairs(br.readLine().trim());
            String collectionID = extractValueFromNameValuePairs(br.readLine().trim());
            String runID = extractValueFromNameValuePairs(br.readLine().trim());
            CLEFtopicProcessor.translationWS_endpoint = extractValueFromNameValuePairs(br.readLine().trim());
            CLEFtopicProcessor.nlpWS_endpoint = extractValueFromNameValuePairs(br.readLine().trim());
            String luceneIDX = extractValueFromNameValuePairs(br.readLine().trim());
            String semVectConfigFile = extractValueFromNameValuePairs(br.readLine().trim());
            CLEFtopicProcessor.corpusThesaurusConfigurationElm = CLEFtopicProcessor.extractCorpusThesaurusConfigurationElm(semVectConfigFile);
            CLEFtopicProcessor.thesauriWS_endpoint = extractValueFromNameValuePairs(br.readLine().trim());
            boolean isQueryExpanded = Boolean.parseBoolean(extractValueFromNameValuePairs(br.readLine().trim()));
            boolean queryOnContentType = Boolean.parseBoolean(extractValueFromNameValuePairs(br.readLine().trim()));
            boolean isMonolingual = Boolean.parseBoolean(extractValueFromNameValuePairs(br.readLine().trim()));
            boolean expandWithWN = Boolean.parseBoolean(extractValueFromNameValuePairs(br.readLine().trim()));
            boolean expandWithW2C = Boolean.parseBoolean(extractValueFromNameValuePairs(br.readLine().trim()));
            boolean queryOnStemmed = Boolean.parseBoolean(extractValueFromNameValuePairs(br.readLine().trim()));
            boolean relRetStats = Boolean.parseBoolean(extractValueFromNameValuePairs(br.readLine().trim()));

            CLEFtopicProcessor ctp = new CLEFtopicProcessor(langFrom,collectionID, topicFname, runID, isQueryExpanded,
                    queryOnContentType, isMonolingual, expandWithWN, expandWithW2C,queryOnStemmed, relRetStats, luceneIDX);

            logger.info(ctp.getRunDescription());

            return ctp;
        } catch (Exception e) {
            logger.error("Cant init CLEFtopicProcessor from conf: " +
                initConfigurationFile + "\n" + e.getMessage());

            return null;
        }
    }

    private void parseTopicXMLInfo(Element e) {
        title = "";
        queryID = "";
        content = "";

        NodeList nl0 = e.getElementsByTagName("identifier");

        if ((nl0 == null) || (nl0.getLength() != 1)) {
            logger.error("Error getting topic ID:" + e.getTextContent());
        } else {
            Element k = (Element) nl0.item(0);
            queryID = k.getTextContent().trim();
        }

        nl0 = e.getElementsByTagName("title");

        if ((nl0 == null) || (nl0.getLength() != 1)) {
            logger.error("Error getting topic TITLE :" + e.getTextContent());
        } else {
            Element k = (Element) nl0.item(0);
            title = k.getTextContent().trim();
        }

        nl0 = e.getElementsByTagName("description");

        if ((nl0 == null) || (nl0.getLength() != 1)) {
            logger.error("Error getting topic DESC:" + e.getTextContent());
        } else {
            Element k = (Element) nl0.item(0);
            content = k.getTextContent().trim();
        }
    }

    public void processTopics() throws IOException {
        this.out = new PrintWriter(new BufferedWriter(new FileWriter(outFname)));

        Element root = this.parseXmlDocument(this.fname);

        NodeList nl = root.getElementsByTagName("topic");

        for (int i = 0; i < nl.getLength(); i++) {
            Element e = (Element) nl.item(i);
            this.parseTopicXMLInfo(e);

            String query = this.createLuceneQueryFromTitleAndContent(title,
                    content, this.langFrom, this.langFrom);

            this.submitQuery(query, queryID);
        }

        this.out.close();
    }

    
    private static TopicDescription extractTopicDescription(String content, String lang) {
        String prefix = "./CACAO_data/ClefTopicAnalyzerFiles/";
        TopicAnalyzer td = new TopicAnalyzer(prefix + lang +
                "ContentTypes.txt", prefix + lang + "Junk.txt");
        TopicDescription topicDesc = td.analyzeString(content);

        return topicDesc;
    }

    private String createLuceneQueryFromTitleAndContent(String title,
        String content, String srcLang, String dstLang) {
        logger.info("\nCreating query from - Title: " + title + "\tContent: " +
            content);

        Vector<NLPtoken> topicKW = this.extractKWsFromText(title, srcLang, this.titleKW_boost);
        titleKW = new Vector<NLPtoken>(topicKW);

        Vector<NLPtoken> contentTypeKW = null;

        if (content != null) {
            TopicDescription descs = this.extractTopicDescription(content,srcLang);
            contentTypeKW = this.extractKWsFromText(descs.what, srcLang,this.contentType_Boost);

            Vector<NLPtoken> kws = this.extractKWsFromText(descs.about, srcLang, this.topic_Boost);

            for (NLPtoken tok : kws) {
            	int idx = topicKW.indexOf(tok);
                if (idx>=0) {
                    float f = topicKW.get(idx).getConfidence();
                    f += tok.getConfidence();
                    topicKW.remove(idx);
                    tok.setConfidence(f);
                    topicKW.add(tok);
                } else {
                    topicKW.add(tok);
                }
            }
        }

        if (!srcLang.equals(dstLang)) {
        	Hashtable<NLPtoken,Vector<NLPtoken>> translationResult = this.translateTerms(topicKW, srcLang, dstLang);
    		for(NLPtoken src: translationResult.keySet()){
    			topicKW.addAll(translationResult.get(src));
    		}
    		
            if (contentTypeKW != null) {
            	translationResult = this.translateTerms(contentTypeKW, srcLang, dstLang);


        		for(NLPtoken src: translationResult.keySet()){
        			contentTypeKW.addAll(translationResult.get(src));
        		}
            }
        }

        logger.info("Topic KWs -> " + topicKW);

        if (contentTypeKW != null) {
            logger.info("Content type KWs-> " + contentTypeKW);
        }

        String query = this.writeLuceneQueryFromKWs(topicKW, srcLang, this.queryOnStemmed, true);

        //query+=" ( "+this.writeLuceneQueryFromKWs(topicKW, contentTypeKW, srcLang, false )+" )^0.8 ";
        if (!this.isMonoLanguage) {
            for (String lng : CLEFtopicProcessor.supportedLanguages) {
                if (!this.langFrom.equalsIgnoreCase(lng)) {
                    String newtTitle = this.translatePhrase(title, langFrom, lng);
                    Vector<NLPtoken> translTopicKW = this.extractKWsFromText(newtTitle, lng, 1f);
                    query += (" ( " +
                    this.writeLuceneQueryFromKWs(translTopicKW, lng, this.queryOnStemmed, false) +
                    " )^" + this.translatedKWBoost + " ");
                }
            }
        }

        if (this.expandWithWordnet && dstLang.equals("en")) {
        	HashSet<NLPtoken> expTopicKW = this.getKWexpansionWithWN(this.titleKW, 1f);
            String tmp = this.writeLuceneQueryFromKWs(new Vector<NLPtoken>(expTopicKW), srcLang, this.queryOnStemmed, false);

            if (tmp.length() > 0) {
                query += (" ( " + tmp + ")^" + this.expandedKWboost + " ");
            }
        }

        if (this.expandWithSemVect && this.langFrom.equals(srcLang)) {
        	Vector<NLPtoken> expTopicKW = this.getKWexpansionWithSemVect(this.titleKW,  dstLang, this.expandedKWboost);
            String tmp = this.writeLuceneQueryFromKWs(expTopicKW, srcLang, this.queryOnStemmed, false);

            if (tmp.length() > 0) {
                query += (" ( " + tmp + ")^" + this.expandedKWboost + " ");
            }
        }
        
        if (this.expandWithClassificationCategories) {
        	HashSet<String> categories = this.getRelatedCategories(this.targetCollection, "Dewey", titleKW, srcLang);
        	String categoryesQuery="( ";//+subject:DDC ";
        	int totCat=0;
        	for(String category: categories){
        		category=category.replaceAll("0$", "")+"*";
        		categoryesQuery += "subjectDDC:dcterms\\:DDC>"+category+" ";
        		totCat++;
        	}
        	categoryesQuery+=")^"+this.w2cboost+" ";
        	if(totCat>0)
        		query += categoryesQuery;
        }

        if (this.queryOnContentType && (contentTypeKW != null)) {
            for (NLPtoken tok : contentTypeKW) {
                float boost = tok.getConfidence();
                query += "(";
                query += ("type:\"" + tok.lemma + "\" ");
                query += ("subject:\"" + tok.lemma + "\"");
                query += (")^" + boost + " ");
            }
        }

        //adding constraint on collection
        query += (" +collectionID:\"" + this.targetCollection + "\"^0.1 ");

        return query;
    }
 

    private void submitQuery(String query, String queryID) {
        QueryResult.LuceneScoreComparator cmp = new QueryResult.LuceneScoreComparator();
        TreeSet<QueryResult> results = new TreeSet<QueryResult>(cmp);

        try {
            //adjustment to the query: removing { and } from certain German lemmatized terms because they cause exceptions in the lucene query parser
            query = query.replaceAll("\\{|\\}", "");

            PerFieldAnalyzerWrapper pfan = new PerFieldAnalyzerWrapper(new MySimpleAnalyzer());
            pfan.addAnalyzer("collectionID", new KeywordAnalyzer());
            pfan.addAnalyzer("subjectDDC", new KeywordAnalyzer());
            pfan.addAnalyzer("title_enStem",
                new SnowballAnalyzer("English", StopAnalyzer.ENGLISH_STOP_WORDS));
            pfan.addAnalyzer("title_frStem",
                new SnowballAnalyzer("French", FrenchAnalyzer.FRENCH_STOP_WORDS));
            pfan.addAnalyzer("title_deStem",
                new SnowballAnalyzer("German", GermanAnalyzer.GERMAN_STOP_WORDS));
            pfan.addAnalyzer("subject_enStem",
                new SnowballAnalyzer("English", StopAnalyzer.ENGLISH_STOP_WORDS));
            pfan.addAnalyzer("subject_frStem",
                new SnowballAnalyzer("French", FrenchAnalyzer.FRENCH_STOP_WORDS));
            pfan.addAnalyzer("subject_deStem",
                new SnowballAnalyzer("German", GermanAnalyzer.GERMAN_STOP_WORDS));
            pfan.addAnalyzer("type_enStem",
                new SnowballAnalyzer("English", StopAnalyzer.ENGLISH_STOP_WORDS));
            pfan.addAnalyzer("type_frStem",
                new SnowballAnalyzer("French", FrenchAnalyzer.FRENCH_STOP_WORDS));
            pfan.addAnalyzer("type_deStem",
                new SnowballAnalyzer("German", GermanAnalyzer.GERMAN_STOP_WORDS));

            QueryParser queryParser = new QueryParser("title_lemmatized", pfan);

            Query q = queryParser.parse(query);
            logger.info("QUERY: " + q.toString());

            Hits myHits = this.luceneSearcher.search(q);

            HashSet<String> relDocs = null;

            if (this.printRelRetStatistics) {
                relDocs = this.relevantDocs.get(queryID);
            }

            Hashtable<String, String> relRet = new Hashtable<String, String>();

            for (int i = 0;
                    (i < myHits.length()) &&
                    (i < queryResultsNumberUpperBound); i++) {
                int docID = myHits.id(i);
                float score = myHits.score(i);

                org.apache.lucene.document.Document d = this.luceneSearcher.doc(docID);
                String documentID = d.getField("recordID").stringValue();

                if (this.printRelRetStatistics && relDocs.contains(documentID)) {
                    String sub = "";

                    try {
                        Field[] fs = d.getFields("subject");

                        for (Field f : fs)
                            sub += (f.stringValue() + " ");
                    } catch (Exception e) {
                    }

                    relRet.put(documentID,
                        i + "  " + score + " " +
                        d.getField("title").stringValue() + " - " + sub);
                }

                QueryResult qr = new QueryResult(score, documentID);
                results.add(qr);
            }

            if (this.printRelRetStatistics) {
                printRelRetStatistics(relRet, relDocs, true);
            }

            this.printQueryResultsToCLEFoutput(queryID, results);
        } catch (Throwable e) {
            logger.error("Error Processing topic: " + e.getMessage());
        }
    }

    private void printRelRetStatistics(Hashtable<String, String> relRet,
        HashSet<String> relDocs, boolean printNotFound) {
        logger.info("\n>>Retrieved rel docs [" + relRet.size() + "] :");

        for (String s : relRet.keySet())
            logger.info("- " + s + " " + relRet.get(s));

        if (printNotFound) {
            logger.info("\n>> !!! NOT RETRIEVED RELEVANT docs [" +
                (relDocs.size() - relRet.size()) + "] :");

            QueryParser queryParser = new QueryParser("recordID",
                    new KeywordAnalyzer());

            for (String rDoc : relDocs) {
                if (!relRet.containsKey(rDoc)) {
                    try {
                        Query q = queryParser.parse(rDoc);
                        Hits myHits = this.luceneSearcher.search(q);

                        for (int i = 0;
                                (i < myHits.length()) &&
                                (i < queryResultsNumberUpperBound); i++) {
                            int docID = myHits.id(i);
                            org.apache.lucene.document.Document d = this.luceneSearcher.doc(docID);
                            String documentID = d.getField("recordID")
                                                 .stringValue();
                            String title = d.getField("title").stringValue();

                            String sub = "", lang="", desc = "", relat = "";
                            
                            try {
                                Field[] tmps = d.getFields("language");
                                for(Field tmp:tmps){
                                	lang+=tmp.stringValue()+" ";
                                }
                                lang=lang.trim();
                            } catch (Exception e) {
                            }
                            
                            try {
                                Field[] fs = d.getFields("subjectDDC");

                                for (Field f : fs)
                                    sub += (f.stringValue() + " ");
                            } catch (Exception e) {
                            }

                            try {
                                desc = d.getField("title_lemmatized").stringValue();
                            } catch (Exception e) {
                            }

                            try {
                                relat = d.getField("subject_lemmatized").stringValue();
                            } catch (Exception e) {
                            }

                            logger.info("-" + documentID + " " + title + " - " +lang+" - "+
                                sub + " ; " + relat + " - " + desc);
                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            }
        }
    }

    private void printQueryResultsToCLEFoutput(String queryID,
        TreeSet<QueryResult> globalResults) {
        int rank = 0;

        for (QueryResult qr : globalResults) {
            if (rank >= this.queryResultsNumberUpperBound) {
                break;
            }

            java.text.DecimalFormatSymbols dfs = new java.text.DecimalFormatSymbols(java.util.Locale.ENGLISH);
            java.text.DecimalFormat df = new java.text.DecimalFormat("0.00000",
                    dfs);
            this.out.println(queryID + " Q0 " + qr.getDocumentID() + " " +
                rank + " " + df.format(qr.getLuceneScore()) + " " + this.runID);
            this.out.flush();
            rank++;
        }
    }

    private String writeLuceneQueryFromKWs(Vector<NLPtoken> topicKWs, String lang, boolean queryOnStemmed, boolean groupWithParentheses) {
        String query = "";

        if (groupWithParentheses) {
            query += " (";
        }

        for (NLPtoken tok : topicKWs) {
            float boost = tok.getConfidence();

            if (tok.isNE || this.titleKW.contains(tok)) {
                query += " (";

                if (tok.isNE || (tok.surface.indexOf(' ') > 0)) {
                    query += ("title:(" + tok.surface + ") ");
                    query += ("subject:(" + tok.surface + ") ");
                } else {
                    query += ("title_lemmatized:(" + tok.getLemma_Category() +
                    ") ");
                    query += ("subject_lemmatized:(" + tok.getLemma_Category() +
                    ") ");
                }

                if (queryOnStemmed) {
                    query += ("subject_" + lang + "Stem:(" + tok.surface +
                    ") ");
                    query += ("title_" + lang + "Stem:(" + tok.surface + ") ");
                }

                if (tok.isNE) {
                    query += (" )^" + (boost * this.NEboost) + " ");
                } else {
                    query += (" )^" + boost + " ");
                }
            }
        }

        if (groupWithParentheses && (query.length() > 5)) {
            query += (")^" + this.ANDboost + " ");
        } else if (groupWithParentheses) {
            query = " ";
        }

        /*for(NLPtoken tok: topicKWs.keySet()){
           float boost=topicKWs.get(tok);
        
           if(tok.isNE || this.titleKW.contains(tok)){
                   query+=" (";
                   if(tok.isNE){
                           query+="title:("+tok.surface+")^"+NEboost+" ";
                           query+="subject:("+tok.surface+")^"+NEboost+" ";
                   }
                   else{
                           query+="title_lemmatized:("+tok.getLemma_Category()+") ";
                           query+="subject_lemmatized:("+tok.getLemma_Category()+") ";
                   }
                   if(this.queryOnStemmed){
                           query+="subject_"+lang+"Stem:("+tok.surface+") ";
                           query+="title_"+lang+"Stem:("+tok.surface+") ";
                   }
                   query+=" )^"+boost*this.ORboost+" ";
           }
           }*/
        int added = 0;

        for (NLPtoken tok : topicKWs) {
            float boost = tok.getConfidence();

            if (!tok.isNE && !this.titleKW.contains(tok)) {
                added++;
                query += " (";

                if (tok.surface.indexOf(' ') > 0) {
                    query += ("title:(" + tok.surface + ") ");
                    query += ("subject:(" + tok.surface + ") ");
                } else {
                    query += ("title_lemmatized:(" + tok.getLemma_Category() +
                    ") ");
                    query += ("subject_lemmatized:(" + tok.getLemma_Category() +
                    ") ");
                }

                if (this.queryOnStemmed) {
                    query += ("subject_" + lang + "Stem:(" + tok.surface +
                    ") ");
                    query += ("title_" + lang + "Stem:(" + tok.surface + ") ");
                }

                query += (" )^" + (boost * this.ORboost) + " ");
            }
        }

        return query;
    }

    
    
    
    private Element parseXmlDocument(String xmlDoc) {
        try {
            // getting the document builder Factory
            DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();

            // getting the document builder from the Factory
            DocumentBuilder builder = factory.newDocumentBuilder();

            // parses the xml
            Document myDocument = builder.parse(new File(xmlDoc));

            // extract the element root
            return myDocument.getDocumentElement();
        } catch (Exception e) {
            return null;
        }
    }

    private void performEvaluation() {
        try {
            ProcessBuilder pb = new ProcessBuilder("./trec_eval.bat",
                    this.evalResultsFilename, this.outFname);
            Process p = pb.start();
            p.waitFor();

            BufferedReader br = new BufferedReader(new InputStreamReader(
                        new FileInputStream("auto.out")));

            String result = "";
            String map = "";
            String rprec = "";
            String p50 = "";
            String p10 = "";
            String p20 = "";
            String p5 = "";
            String nrel = "";
            String nret = "";

            while (br.ready()) {
                result = br.readLine().trim();

                if (result.startsWith("P5 ")) {
                    String[] tmps = result.split("[\t ]+");
                    p5 = tmps[2];
                }

                if (result.startsWith("P10 ")) {
                    String[] tmps = result.split("[\t ]+");
                    p10 = tmps[2];
                }

                if (result.startsWith("P20 ")) {
                    String[] tmps = result.split("[\t ]+");
                    p20 = tmps[2];
                }

                if (result.startsWith("P50 ")) {
                    String[] tmps = result.split("[\t ]+");
                    p50 = tmps[2];
                }

                if (result.startsWith("map")) {
                    String[] tmps = result.split("[\t ]+");
                    map = tmps[2];
                }

                if (result.startsWith("R-prec")) {
                    String[] tmps = result.split("[\t ]+");
                    rprec = tmps[2];
                }

                if (result.startsWith("num_rel ")) {
                    String[] tmps = result.split("[\t ]+");
                    nrel = tmps[2];
                }

                if (result.startsWith("num_rel_ret")) {
                    String[] tmps = result.split("[\t ]+");
                    nret = tmps[2];
                }
            }

            br.close();

            float m = Float.parseFloat(map.trim());
            float r = Float.parseFloat(rprec.trim());

            if (true) {
                PrintWriter pw = new PrintWriter(new FileOutputStream(
                            "Results.log", true));
                pw.println(this.getRunDescription());
                pw.println("Nrel: " + nrel + " Nret:" + nret + " map: " + map +
                    " Rprec: " + rprec + " P5: " + p5 + " P10: " + p10 +
                    " P20: " + p20 + " P50: " + p50 + "\n");
                pw.flush();
                pw.close();
            }

            return;
        } catch (Exception e) {
            logger.error("Error trying to translate with SYSTRAN.\n" +
                e.getMessage());

            return;
        }
    }

    private String getRunDescription() {
        return "Run ID: " + runID + " " + langFrom + "->" + targetCollection +
        ".\n" + " - isMonolingual: " + this.isMonoLanguage + " expandQuery: " +
        this.expandWithSemVect + " expandWithClassificationCategories: "+
        this.expandWithClassificationCategories + " searchOnContentType: " +
        this.queryOnContentType + " expandWithWordnet: " +
        this.expandWithWordnet + " queryOnStemmed: " + this.queryOnStemmed +
        ".\n" + " - TitleKW_Boost: " + this.titleKW_boost + " topic_Boost: " +
        this.topic_Boost + " NE_Boost: " + this.NEboost + " stem_Boost: " +
        this.translatedKWBoost + " contentBoost: " + this.contentType_Boost +
        ".\nAND boost: " + this.ANDboost + " ORboost: " + this.ORboost+ " w2cBoost: "+this.w2cboost;
    }
    
    private static Element extractCorpusThesaurusConfigurationElm(String xmlConfigurationFile) {
        try {
        	logger.debug("Init CorpusThesaurus Component from "+xmlConfigurationFile);
            // getting the document builder Factory
            DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();

            // if the document has to be validated against the schema source

            // getting the document builder from the Factory
            DocumentBuilder builder = factory.newDocumentBuilder();

            // parses the xml
            Document myDocument = builder.parse(new File(xmlConfigurationFile));

            return myDocument.getDocumentElement();
            
        } catch (Exception e) {
            logger.error("Error initializing CorpusThesaurus component.\n" +  e.getMessage());
            return null;
        }
    }

    static private Hashtable<String, HashSet<String>> getRelDocs(String fname) {
        Hashtable<String, HashSet<String>> results = new Hashtable<String, HashSet<String>>();

        try {
            BufferedReader br = new BufferedReader(new FileReader(fname));

            while (br.ready()) {
                String line = br.readLine().trim();
                String[] tmps = line.split(" ");

                if (tmps.length == 4) {
                    if (Integer.parseInt(tmps[3]) == 1) {
                        String key = tmps[0];
                        HashSet<String> rel;

                        if (results.containsKey(key)) {
                            rel = results.get(key);
                            rel.add(tmps[2]);
                        } else {
                            rel = new HashSet<String>();
                            rel.add(tmps[2]);
                        }

                        results.put(key, rel);
                    }
                }
            }
        } catch (Exception e) {
            logger.error("Error loading relevants docset from official qrel\n" +
                e.getMessage());
        }

        return results;
    }

    static public class MySimpleAnalyzer extends Analyzer {
        /** Builds an analyzer with the stop words from the given file.
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
