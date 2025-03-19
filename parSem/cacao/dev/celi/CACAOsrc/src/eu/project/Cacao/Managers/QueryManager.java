package eu.project.Cacao.Managers;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.InputStream;
import java.io.StringWriter;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.Set;
import java.util.Vector;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.log4j.Logger;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import com.sun.org.apache.xml.internal.serialize.OutputFormat;
import com.sun.org.apache.xml.internal.serialize.XMLSerializer;

import edu.emory.mathcs.backport.java.util.Arrays;
import eu.project.Cacao.Components.CorpusThesaurus_Component;
import eu.project.Cacao.Components.NamedEntitiesDetector;
import eu.project.Cacao.Components.Search_Component;
import eu.project.Cacao.Components.WordToCategory_Component;
import eu.project.Cacao.Components.Utils.NLPtoken;
import eu.project.Cacao.Components.Utils.RunTimer;
import eu.project.Cacao.WS_Clients.CACAO_ThesauriWS_client;
import eu.project.Cacao.WS_Clients.CACAO_TranslationWS_client;
import eu.project.Cacao.WS_Clients.CACAO_nlpWS_client;
import eu.project.Cacao.WS_Clients.CACAO_queryWS_client;
import eu.project.Cacao.WS_Clients.CACAO_searchWS_client;

/**
 * This class is charged to process the monolingual query string submitted by
 * users and generate a set of queries in the target languages. It coordinates
 * different software components and web services
 * 
 * Here is a summary of the process: 1 - User query is analyzed by means of NLP,
 * recognizing Named Entities and lemmatizing the rest of the terms. (Whenever
 * possible multi-word are detected) 2 - The terms lemmatized (not N.E.) are
 * translated using proper resources 3 - Disambiguation of the possible multiple
 * translations in order to select the best combination of terms and increase
 * the precision (2 different approaches: LSA based using a semantic distance
 * in a Word Space or exploiting the natural clustering of terms in categories
 * from digital libraries) 4 - Expansion of the terms translated in a target
 * language in order to increase the recall
 * 
 * @author Alessio Bosca - alessio.bosca@celi.it
 * 
 */
public class QueryManager {
    private static final String initConfigurationFile = "configuration/QueryManagerConfiguration.xml";
    
    public static final String translationServiceElementTag="translationService", nlpServiceElementTag="nlpService", 
    thesauriServiceElementTag="thesauriService", searchServiceElementTag="searchService", corpusThesaurusConfigurationElementTag="corpusThesaurusConfiguration", 
    Word2CategoryElementTag="word2Category", supportedLanguagesElementTag="supportedLanguages", urlAttribute="url", pwdAttribute="pwd", 
    userAttribute="user", hostAttribute="host", rootElmTag = "CACAO_ResponseDocument", SVexpansionAttr="SVexpansion", 
    WNexpansionAttr="WNexpansion", W2CexpansionAttr="W2Cexpansion", translateAsSentenceAttr="translateAsSentence", 
    disambiguateWithSVAttr="disambiguateWithSV", disambiguateWithConfidenceAttr="disambiguateWithConfidence", neDetectorTag="neDetector";
    
    private static final float boostPlain=1f, boostExpanded=0.7f, w2cBoost=0.5f, titleBoost=2f, NEboost=3f, subjBoost=1.5f;
    
    private static final int maxExpansionElm = 3, maxTranslationElm=3;
    
    protected static String translationWS_endpoint, nlpWS_endpoint, searchWS_endpoint, thesauriWS_endpoint ;
    protected static Element corpusThesaurusConfigurationElm;
    protected static String w2c_user, w2c_pwd, w2c_host;
    protected static HashSet<String> supportedLanguages;
    protected static NamedEntitiesDetector neDetector;
    protected static CorpusThesaurus_Component semVectSearcher;

    protected static boolean isSuccesfullyInited=false, SVexpansion=false, WNexpansion=false, W2Cexpansion=false, 
    disambiguateTranslationsWithSV=false, disambiguateTranslationsWithAvgConfidence=true, translateAsSentence=false ;
    
    protected Logger logger = Logger.getLogger(QueryManager.class.getName());
   
    protected QueryManager(boolean skipInit){
    	if(!skipInit)
    		QueryManager.isSuccesfullyInited = this.initService(QueryManager.initConfigurationFile);
    }
    
    public QueryManager() {
    	QueryManager.isSuccesfullyInited = this.initService(QueryManager.initConfigurationFile);
    }
    
    
    /**
     * 
     * @param query
     * @param langFrom
     * @param targetLanguages
     * @return
     */
    public String processQuery(String query, String langFrom, String[] targetLanguages){
    	return this.processQueryCustom(query, langFrom, targetLanguages, 
    			QueryManager.translateAsSentence, QueryManager.SVexpansion, QueryManager.WNexpansion, QueryManager.W2Cexpansion);
    }
    
    /**
     * 
     * @param query
     * @param langFrom
     * @param targetLanguages
     * @param translateWithSystran
     * @param expandQueryWithSemVect
     * @param expandWithWordNet
     * @return
     */
    public String processQueryCustom(String query, String langFrom, String[] targetLanguages,	boolean translateWithSystran, 
    		boolean expandQueryWithSemVect, boolean expandWithWordNet, boolean expandWithWordToCat ) {
		try {
			RunTimer rt =new RunTimer();
			logger.info("Input Query: "+query+" in "+langFrom);
			DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
			DocumentBuilder db = dbf.newDocumentBuilder();
			Document doc = db.newDocument();
			Element rootElm = doc.createElement(QueryManager.rootElmTag);
			doc.appendChild(rootElm);
			this.translateAndExpandQuery(doc, query, langFrom, targetLanguages, translateWithSystran, expandQueryWithSemVect, expandWithWordNet, expandWithWordToCat);
			logger.info("elapsed time: "+rt.getElapsedTimeLabel());
			return QueryManager.writeResponseDocumentToString(doc);
		}catch (Exception e) {
			return QueryManager.writeResponseDocumentToString(null);
		}
	}
    
    /**
     * 
     * @param query
     * @param langFrom
     * @param targetLanguages
     * @param facetFieldNames
     * @param startFrom
     * @param totResults
     * @return
     */
    public String processQueryAndSearch(String query, String langFrom, String[] targetLanguages, String[] facetFieldNames, 
    		int startFrom, int totResults) {
    	return this.processQueryAndSearchCustom(query, langFrom, targetLanguages, facetFieldNames, startFrom, totResults,
    			QueryManager.translateAsSentence, QueryManager.SVexpansion, QueryManager.WNexpansion, QueryManager.W2Cexpansion);
    }
    
    public String processQueryAndSearchCustom(String query, String langFrom, String[] targetLanguages,
    		String[] facetFieldNames, int startFrom, int totResults, boolean translateWithSystran, 
    		boolean expandQueryWithSemVect, boolean expandWithWordNet, boolean expandWithWord2Cat) {
    	try {
    		RunTimer rt =new RunTimer();
			DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
			DocumentBuilder db = dbf.newDocumentBuilder();
			Document doc = db.newDocument();
			Element rootElm = doc.createElement(QueryManager.rootElmTag);
			doc.appendChild(rootElm);
			
			String searchQuery = this.translateAndExpandQuery(doc, query, langFrom, targetLanguages, translateWithSystran, 
					expandQueryWithSemVect, expandWithWordNet, expandWithWord2Cat);
			logger.debug("SOLR search query: "+searchQuery);
			
			CACAO_searchWS_client searchServiceClient = new CACAO_searchWS_client(QueryManager.searchWS_endpoint);
			String msg = searchServiceClient.searchRecords(searchQuery, facetFieldNames, startFrom, totResults);
			Element[] elems = this.extractResponseElmFromSearch(msg);
			
			String retrieved=elems[1].getAttribute("recordsReturned");
			logger.info("Input Query: "+query+" in "+langFrom+" requested #doc: "+totResults+" retrieved #docs: "+retrieved);
			logger.info("elapsed time: "+rt.getElapsedTimeLabel());
			this.addFacetAndRecordsElements(doc, elems[0], elems[1]);
			return QueryManager.writeResponseDocumentToString(doc);
		}catch (Exception e) {
			return QueryManager.writeResponseDocumentToString(null);
		}
    }
    
    protected String translateAndExpandQuery(Document doc, String query, String langFrom, String[] targetLanguages,
    		boolean translateWithSystran, boolean expandQueryWithSemVect, boolean expandWithWordNet, boolean expandWithW2C ){
    	
    	try {
    		//Initialization
    		RunTimer rt=new RunTimer();
			Hashtable<String, Vector<NLPtoken>> enrichedQueryOutput = new Hashtable<String, Vector<NLPtoken>>();
			String categoryQuery=null; //used for w2c expansion
			HashSet<String> translated=new HashSet<String>();
			HashSet<String> translatedExpanded=new HashSet<String>();
			HashSet<String> expanded=new HashSet<String>();
			HashSet<NLPtoken> mwInSrcLangInductedByTranslation=new HashSet<NLPtoken>();
			Hashtable<String,HashSet<NLPtoken>> notTranslatedByLang=new Hashtable<String, HashSet<NLPtoken>>();
			
			//Select the target languages (if no ones has been specified by users, all the supported languages are used as target)
			HashSet<String> targetLanguagesSet=null;
			if(targetLanguages!=null && targetLanguages.length>0)
				targetLanguagesSet = new HashSet<String>(Arrays.asList(targetLanguages));
			else
				targetLanguagesSet = QueryManager.supportedLanguages;
			
			//extract keywords from user query
			Vector<NLPtoken> topics = this.performLinguisticAnalysis(query, langFrom);
			topics = QueryManager.neDetector.detectNE(topics, langFrom);

			//translate user keywords (and expand them if so specified..)
			for (String langTo : targetLanguagesSet) {
			    if (!langFrom.equals(langTo) && QueryManager.supportedLanguages.contains(langTo)) {
			    	Vector<NLPtoken> translatedTopics=null;
			    	if(translateWithSystran){
			    		String translation = this.translatePhrase(query, langFrom, langTo);
			    		translatedTopics = this.extractKWsFromText(translation, langTo, boostPlain);
			    	}
			    	else{
			    		Hashtable<NLPtoken,Vector<NLPtoken>> results = this.translateTerms(topics, langFrom, langTo);

			    		//Compound translations into a vector of kw in 'langTo' language and search for MW
			    		//identified by the translation service (a set of token from the NLP WS) <-> a token MW in the response
			    		translatedTopics=new Vector<NLPtoken>();
			    		for(NLPtoken src: results.keySet()){
			    			Vector<NLPtoken> toAdd=results.get(src);
			    			for(NLPtoken ta: toAdd){
			    				translatedTopics.add(ta);
			    				if(ta.isMWexpr){
			    					String[] tmps=ta.getLemma_Category().split(" ");
			    					HashSet<NLPtoken> mwExprComponents = new HashSet<NLPtoken>();
			    					for(String tmp: tmps){
			    						int beg=tmp.lastIndexOf('_');
			    						if(beg>0){
			    							String key=tmp.substring(0,beg);
			    							String pos=tmp.substring(beg+1);
				    						NLPtoken mwExpressionComponent=new NLPtoken(key,pos,key,null,ta.lang);
				    						mwExprComponents.add(mwExpressionComponent);
			    						}
			    					}
			    					mwExprComponents = this.filterKeywordByPartOfSpeech(mwExprComponents, boostPlain);
			    					for(NLPtoken t:mwExprComponents){
			    						if(!translatedTopics.contains(t))
			    							translatedTopics.add(t);
			    					}
			    				}
			    			}
			    			
			    			boolean toAddAsMW=true;
			    			for(NLPtoken tok: topics){//MW recognized by the translation service
			    				if(tok.surface.equalsIgnoreCase(src.surface)){
			    					toAddAsMW=false;
			    					break;
			    				}
			    			}
			    			if(toAddAsMW)
			    				mwInSrcLangInductedByTranslation.add(new NLPtoken(src.surface,null, null, null, langFrom));
			    		}
			    		
			    		//Search for terms that are not translated (NE with no translation in the target lang)
			    		//that will be included as Search Terms in the response but will not be used for writing the lucene query
			    		//in order to avoid repetitions 
			    		HashSet<NLPtoken> notTranslated=new HashSet<NLPtoken>();
			    		for(NLPtoken t: topics){
			    			boolean found=false;
			    			for(NLPtoken test: results.keySet()){
			    				if(test.surface.equalsIgnoreCase(t.surface)){
			    					found=true;
			    					break;
			    				}
			    			}
			    			for(NLPtoken test:  mwInSrcLangInductedByTranslation){
			    				if(test.surface.contains(t.surface)){
			    					found=true;
			    					break;
			    				}
			    			}
			    			if(!found)
			    				notTranslated.add(new NLPtoken(t.surface,t.category,t.lemma,t.neCategory,langTo));
			    		}
			    		if(notTranslated.size()>0){
			    			notTranslated = this.filterKeywordByPartOfSpeech(notTranslated, boostPlain);
			    			if(notTranslated.size()>0)
			    				notTranslatedByLang.put(langTo, notTranslated);
			    		}
			    	}
			    	//mark them as translated
			    	for(NLPtoken tok: translatedTopics)
			    		translated.add(tok.getLemma_Category());
			    	
			    	enrichedQueryOutput.put(langTo, translatedTopics);
			    	
					if (expandWithW2C ) {
						HashSet<String> categories = this.getRelatedCategories("British Library", "Dewey", topics, langFrom);
						String tmp =  this.writerCategoryQuery(categories);
						if(tmp!=null)
							categoryQuery+=tmp+" ";
					}
			    }
			}
			
			if(targetLanguagesSet.contains(langFrom)){
				topics = this.filterKeywordByPartOfSpeech(topics, boostPlain);
				
				//expand the keywords extracted from original user query (if so specified)
				if(expandQueryWithSemVect){
					Vector<NLPtoken> expTopics= this.getKWexpansionWithSemVect(topics, langFrom, boostExpanded) ;
					topics.addAll(expTopics);
					//mark them as expanded
					for(NLPtoken tok: expTopics)
						expanded.add(tok.getLemma_Category());
				}
	
				if (expandWithWordNet) {
					HashSet<NLPtoken> expTopics = this.getKWexpansionWithWN(topics, boostExpanded);
					topics.addAll(expTopics);
					//mark them as expanded
					for(NLPtoken tok: expTopics)
						expanded.add(tok.getLemma_Category());
				}
				
				//adding mwInductedToResponseDoc
				for(NLPtoken mw: mwInSrcLangInductedByTranslation)
					topics.add(mw);
				
				enrichedQueryOutput.put(langFrom, topics);
				
				if (expandWithW2C ) {
					HashSet<String> categories = this.getRelatedCategories("British Library", "Dewey", topics, langFrom);
					String tmp =  this.writerCategoryQuery(categories);
					if(tmp!=null)
						categoryQuery+=tmp+" ";
				}
			}
			
			
			String searchQuery=this.writeQuery(enrichedQueryOutput, categoryQuery);

			this.createProcessQueryResponseDocument(doc, enrichedQueryOutput, translated, translatedExpanded, expanded, searchQuery, langFrom, notTranslatedByLang);
			
			logger.info("Translate and Expand, elapsed time: "+rt.getElapsedTimeLabel());
			return searchQuery;
		} catch (Exception e) {
			logger.error("Error processing request: "+query+" from "+langFrom+" to "+targetLanguages+".\n"+e);
			doc=null;
			return "";
		}
    }
    
    private String writerCategoryQuery(HashSet<String> categories){
		if(categories!=null && categories.size()>0){
			String categoryQuery="( ";
			for(String category: categories){
				category=category.replaceAll("0$", "")+"*";
				categoryQuery += "subjectDDC:dcterms\\:DDC>"+category+" ";
			}
			categoryQuery+=")^"+this.w2cBoost+" ";
			return categoryQuery;
		}
		else
			return null;
    }
    
    private String writeQuery(Hashtable<String, Vector<NLPtoken>> enrichedQueryOutput, String categoryesQuery){
    	StringBuffer solrQuery = new StringBuffer();
    	
		for(String lang: enrichedQueryOutput.keySet()){
			solrQuery = solrQuery.append( this.writeLuceneQueryFromKWs(enrichedQueryOutput.get(lang), lang));
		}
		
		if(categoryesQuery!= null && categoryesQuery.length()>0)
				solrQuery = solrQuery.append(categoryesQuery);
		
		return solrQuery.toString().trim();
    }
    
    protected HashSet<String> getRelatedCategories(String collection, String classificationSystemID, Vector<NLPtoken> terms, String lang){
    	try {
			WordToCategory_Component w2cat = new WordToCategory_Component(QueryManager.w2c_user, QueryManager.w2c_pwd, QueryManager.w2c_host);
			return w2cat.getCategoriesByWords(collection, classificationSystemID, terms, lang);
		} catch (Exception e) {
			logger.error("Error trying to get expansion with W2C in "+lang+" for "+terms+".\n"+e.getMessage());
			return new HashSet<String>();
		}
    }
    
    protected HashSet<NLPtoken> getKWexpansionWithWN( Vector<NLPtoken> kws, float boost) {
    	HashSet<NLPtoken> results = new HashSet<NLPtoken>();
        try {
        	CACAO_ThesauriWS_client ww = new CACAO_ThesauriWS_client(this.thesauriWS_endpoint);
			for (NLPtoken token : kws) {
				if(!token.isNE){
				    HashSet<NLPtoken> synsets = ww.getRelatedTerms(token.lemma, token.category, null, token.lang);
				    for(NLPtoken syn: synsets){
				    	if(!kws.contains(syn))
				    		results.add(syn);
				    }
				    
				}
			}
		} catch (Exception e) {
			logger.error("Error trying to get expansion with WordNet for "+kws+".\n"+e.getMessage());
		}

        return results;
    }

	private Element[] extractResponseElmFromSearch(String msg){
    	try {
			// getting the document builder Factory
			DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();

			// if the document has to be validated against the schema source

			// getting the document builder from the Factory
			DocumentBuilder builder = factory.newDocumentBuilder();

			// parses the xml
			InputStream is = new ByteArrayInputStream(msg.getBytes("UTF-8"));
			Document myDocument = builder.parse(is, "UTF-8");
			Element facets = (Element) myDocument.getElementsByTagName(Search_Component.facetsElementTag).item(0);
			Element result = (Element) myDocument.getElementsByTagName(Search_Component.retrievedDocsElementTag).item(0);
			
			Element[] results = new Element[2];
			results[0] =  facets;
			results[1] = result;
			return results;
    	}catch(Exception e){
    		logger.error("Error parsing search response from: "+msg+"+.\n"+e.getMessage());
    		return null;
    	}
    }
    
	protected Hashtable<NLPtoken,Vector<NLPtoken>> filterTranslations(Hashtable<NLPtoken,Vector<NLPtoken>> termsTranslations, String dstLang){
		Hashtable<NLPtoken,Vector<NLPtoken>> result=null;
		
		if(disambiguateTranslationsWithSV && termsTranslations.keySet().size()>1){
        	result = QueryManager.semVectSearcher.disambiguateTranslations(termsTranslations, dstLang);        		
        }
        
		if(result==null){
			result = new Hashtable<NLPtoken, Vector<NLPtoken>>();
			for (NLPtoken src : termsTranslations.keySet()) {
				Vector<NLPtoken> translations = termsTranslations.get(src);
	        	if(translations!=null && termsTranslations.keySet().size()>0 && disambiguateTranslationsWithAvgConfidence){
	            	float confidenceThreshold=(translations.firstElement().getConfidence()+translations.lastElement().getConfidence())*.5f;
	            	int cnt=0;
	            	Vector<NLPtoken> v=new Vector<NLPtoken>();
	            	for (NLPtoken target: translations ) {
	                    if (target.getConfidence() >= confidenceThreshold && cnt< maxTranslationElm) {
	                       v.add(target);
	                       cnt++;
	                    }
	                }
	            	result.put(src,v);
	        	}
	        }
		}
		
        for(Vector<NLPtoken> vec:result.values()){
        	for(NLPtoken tok:vec){
        		tok.setConfidence(boostPlain);
        	}
        }
        
        return result;
	}

    protected Hashtable<NLPtoken,Vector<NLPtoken>> translateTerms(Vector<NLPtoken> srcKWs, String srcLang, String dstLang) {
    	logger.debug("Translating "+srcKWs.toString()+" from "+srcLang+" "+dstLang);    
    	try {
    		RunTimer rt=new RunTimer();
    		CACAO_TranslationWS_client translationServiceClient = new CACAO_TranslationWS_client(QueryManager.translationWS_endpoint);
    		Hashtable<NLPtoken,NLPtoken[]> translatedTerms = translationServiceClient.translateTerms(srcKWs, srcLang, dstLang);
    		Hashtable<NLPtoken,Vector<NLPtoken>> results = new Hashtable<NLPtoken, Vector<NLPtoken>>();
    		
    		HashSet<NLPtoken> filtered=this.filterKeywordByPartOfSpeech(translatedTerms.keySet(), 1f);
    		for(NLPtoken src: filtered){
    			NLPtoken[] array=translatedTerms.get(src);
    			Vector<NLPtoken> vect=new Vector<NLPtoken>();
    			for(NLPtoken tok: array)
    				vect.add(tok);
    			results.put(src, vect);
    		}
	    		                
    		results = this.filterTranslations(results, dstLang);
    		
    		logger.info("Translate Terms from " + srcLang + " -> " + dstLang +", elapsed time :"+rt.getElapsedTimeLabel());
    		return results;
    		
            } catch (Exception e) {
                logger.error("Error translating " + srcKWs + " from " + srcLang + " - " + dstLang + "\n" + e.getMessage());
            }

            return null;
        }

    
    protected String translatePhrase(String text, String langFrom, String langTo) {
    	logger.debug("Translating "+text+" from "+langFrom+" "+langTo);
        try {
        	CACAO_TranslationWS_client translationServiceClient = new CACAO_TranslationWS_client(QueryManager.translationWS_endpoint);
            
        	Hashtable<NLPtoken, NLPtoken[]> result = translationServiceClient.translatePhrase(text, langFrom, langTo);
           
            for (NLPtoken src : result.keySet()) {
                for (NLPtoken target : result.get(src)) {
                    String tmp = new String(target.surface.getBytes("UTF-8"), "UTF-8");
                    return tmp;
                }
            }
        } catch (Exception e) {
            logger.error("Error translating " + text + " from " + langFrom +
                " - " + langTo + "\n" + e.getMessage());
        }

        return null;
    }
    
    protected Vector<NLPtoken> getKWexpansionWithSemVect(Vector<NLPtoken> tokens, String queryLang, float boost) {
    	 try {
    		RunTimer rt=new RunTimer();
			Vector<NLPtoken> res= QueryManager.semVectSearcher.getRelatedTerms(tokens, queryLang, maxExpansionElm, boostExpanded);
	    	logger.info("get SV expansion, elapsed time :"+rt.getElapsedTimeLabel());
			return res;
		} catch (Exception e) {
			logger.error("Error trying to get expansion with SemVect in "+queryLang+" for "+tokens+".\n"+e.getMessage());
			return new Vector<NLPtoken>();
		}
    }
    
    private NLPtoken selectTokWithPOS(NLPtoken termA, NLPtoken termB, String POS){
		if(termA.category.equalsIgnoreCase(POS))
				return termA;
		else 	return termB;
	}
	
    private NLPtoken selectDominantTermUsingQueryHeuristic(NLPtoken termA, NLPtoken termB){
    	if(termA.category.equalsIgnoreCase(termB.category)){
    		if(termA.isNE && !termB.isNE)
    			return termA;
    		else
    			return termB;
    	}
    	if(termA.category.equalsIgnoreCase("NOUN")||termB.category.equalsIgnoreCase("NOUN"))
    		return selectTokWithPOS(termA,termB,"NOUN");
    	if(termA.category.equalsIgnoreCase("NADJ")||termB.category.equalsIgnoreCase("NADJ"))
    		return selectTokWithPOS(termA,termB,"NADJ");
    	if(termA.category.equalsIgnoreCase("ADJP")||termB.category.equalsIgnoreCase("ADJP"))
    		return selectTokWithPOS(termA,termB,"ADJP");
    	if(termA.category.equalsIgnoreCase("ADJA")||termB.category.equalsIgnoreCase("ADJA"))
    		return selectTokWithPOS(termA,termB,"ADJA");
    	if(termA.category.equalsIgnoreCase("ADJ")||termB.category.equalsIgnoreCase("ADJ"))
    		return selectTokWithPOS(termA,termB,"ADJ");
    	if(termA.category.equalsIgnoreCase("ADV")||termB.category.equalsIgnoreCase("ADV"))
    		return selectTokWithPOS(termA,termB,"ADV");
    	if(termA.category.equalsIgnoreCase("PREP")||termB.category.equalsIgnoreCase("PREP"))
    		return selectTokWithPOS(termA,termB,"PREP");
    	if(termA.category.equalsIgnoreCase("VERB")||termB.category.equalsIgnoreCase("VERB"))
    		return selectTokWithPOS(termA,termB,"VERB");
    	if(termA.category.equalsIgnoreCase("NUM")||termB.category.equalsIgnoreCase("NUM"))
    		return selectTokWithPOS(termA,termB,"NUM");
    	if(termA.category.equalsIgnoreCase("COORD")||termB.category.equalsIgnoreCase("COORD"))
    		return selectTokWithPOS(termA,termB,"COORD");
    	if(termA.category.equalsIgnoreCase("NEG")||termB.category.equalsIgnoreCase("NEG"))
    		return selectTokWithPOS(termA,termB,"NEG");
    	if(termA.category.equalsIgnoreCase("DET")||termB.category.equalsIgnoreCase("DET"))
    		return selectTokWithPOS(termA,termB,"DET");
    	return termA;
    }

    
    protected Vector<NLPtoken> performLinguisticAnalysis(String text, String lang){
    	Vector<NLPtoken> results =new Vector<NLPtoken>();
    	RunTimer rt=new RunTimer();
    	logger.debug("Performing linguistic analysis on "+text+" in "+lang);
	    try{
	    	CACAO_nlpWS_client nlpServiceClient = new CACAO_nlpWS_client(QueryManager.nlpWS_endpoint);
	    	Vector<NLPtoken> terms = nlpServiceClient.parsePhrase(text, lang);
	    	NLPtoken prev=null;
			for(NLPtoken tok: terms){
				if(prev!=null){
					if(tok.surface.equalsIgnoreCase(prev.surface) || tok.lemma.equalsIgnoreCase(prev.lemma))
						prev=this.selectDominantTermUsingQueryHeuristic(tok, prev);
					else{
						results.add(prev);
						prev=tok;
					}
				}
				else {
					prev=tok;
				}
			}
			if(prev!=null)
				results.add(prev);
		} catch (Exception e) {
	        logger.error("Error performing NLP on " + text + " in " +  lang + "\n" + e.getMessage());
	    }
		logger.info("NLP analysis, elapsed time :"+rt.getElapsedTimeLabel());
		return results;
    }
    
    protected HashSet<NLPtoken> filterKeywordByPartOfSpeech(Set<NLPtoken> keywords, float boost){
    	HashSet<NLPtoken> results = new HashSet<NLPtoken>();
    	
    	 for (NLPtoken tok : keywords) {
    		 if((tok.category == null || tok.category.trim().length()==0 || tok.category.equalsIgnoreCase("UNK") ) 
    				 && tok.surface!=null && tok.surface.indexOf(' ')>0 ){
    			 tok.setConfidence(boost);
                 results.add(tok);
    		 }
             if (tok.category.matches("(NOUN|DIG|NADJ|VERB|ADV|ADJ|NUM|ADJA)")) {
                 tok.setConfidence(boost);
                 results.add(tok);
             }
         }
    	 logger.debug("KW filtering: " + keywords + " --> " + results);
    	 return results;
    }
    
    protected Vector<NLPtoken> filterKeywordByPartOfSpeech(Vector<NLPtoken> keywords, float boost){
    	Vector<NLPtoken> results = new Vector<NLPtoken>();
    	
    	 for (NLPtoken tok : keywords) {

             if (tok.category.matches("(NOUN|DIG|NADJ|VERB|ADV|ADJ|NUM|ADJA)")) {
                 tok.setConfidence(boost);
                 results.add(tok);
             }
         }
    	 logger.debug("KW filtering: " + keywords + " --> " + results);
    	 return results;
    }
    
    
    protected Vector<NLPtoken> extractKWsFromText(String text, String queryLang, float boost) {
    		Vector<NLPtoken> results = new Vector<NLPtoken>();
            Vector<NLPtoken> nlpTokens = this.performLinguisticAnalysis(text, queryLang);

            for (NLPtoken tok : nlpTokens) {
                boolean keepGoing = false;

                for (NLPtoken x : results) {
                    if (x.surface.equals(tok.surface) && !x.category.equals(tok.category)) {
                        keepGoing = true;
                    }
                }
                if (keepGoing) continue;

                if (tok.category.matches("(NOUN|DIG|NADJ|VERB|ADV|ADJ|NUM|ADJA)")) {
                    float tot = 0f;
                    
                    int idx = results.indexOf(tok);
                    if (idx >= 0) {
                        tot = results.get(idx).getConfidence();
                        results.remove(idx);
                    }
                    
                    tok.setConfidence(tot+boost);
                    results.add(tok);
                }
            }

            logger.debug("KW extraction: " + text + " --> " + results);
            return results;
        }

    
    private String writeLuceneQueryFromKWs(Vector<NLPtoken> topicKWs, String lang) {
            StringBuffer query = new StringBuffer("");

            for (NLPtoken tok : topicKWs) {
                
            	String quoteOrNot="";
            	if(tok.isMWexpr || tok.isMW)
            		quoteOrNot="\"";
                query = query.append("title:("+quoteOrNot+tok.surface + quoteOrNot+")^"+titleBoost*tok.getConfidence()+" ");
                query = query.append("subject:("+quoteOrNot+ tok.surface +quoteOrNot+")^"+tok.getConfidence()*subjBoost+" ");
                query = query.append("tableOfContents:("+quoteOrNot+ tok.surface + quoteOrNot+")^"+tok.getConfidence()+" ");
                query = query.append("description:("+quoteOrNot + tok.surface + quoteOrNot+")^"+tok.getConfidence()+" ");
                
                if (tok.isNE) {
                	if(tok.neCategory.equalsIgnoreCase("PERSON")){
                		query = query.append("namedEntity_PERSON:(" + tok.surface + ")^"+tok.getConfidence()*NEboost+" ");
                		query = query.append("contributor:(" + tok.surface + ")^"+tok.getConfidence()*NEboost+" ");
                		query = query.append("creator:(" + tok.surface + ")^"+tok.getConfidence()*NEboost+" ");
                	}else if(tok.neCategory.equalsIgnoreCase("ORGANISATION")){
                		query = query.append("namedEntity_ORGANISATION:(" + tok.surface + ")^"+tok.getConfidence()+" ");
                	}else if(tok.neCategory.equalsIgnoreCase("LOCATION") || tok.neCategory.equalsIgnoreCase("LOCORG")){
                	   	query = query.append("namedEntity_GEO:(" + tok.surface + ")^"+tok.getConfidence()+" ");
                	}
                } 
                else {
                	 String lemmatizedStr = tok.getLemma_Category().toLowerCase();
                     lemmatizedStr=lemmatizedStr.replaceAll("\\{", "\\\\{");
                     lemmatizedStr=lemmatizedStr.replaceAll("\\}", "\\\\}");
                     lemmatizedStr=lemmatizedStr.replaceAll("\\[", "\\\\[");
                     lemmatizedStr=lemmatizedStr.replaceAll("\\]", "\\\\]");
                     lemmatizedStr=lemmatizedStr.replaceAll("\\!", "\\\\!");
                     
                     query = query.append("title_lemmatized:("+quoteOrNot + lemmatizedStr + quoteOrNot+")^"+titleBoost*tok.getConfidence()+" ");
                     query = query.append("subject_lemmatized:("+quoteOrNot + lemmatizedStr + quoteOrNot+")^"+tok.getConfidence()+" ");
                     query = query.append("description_lemmatized:("+quoteOrNot + lemmatizedStr + quoteOrNot+")^"+tok.getConfidence()+" ");
                     query = query.append("tableOfContents_lemmatized:("+quoteOrNot + lemmatizedStr + quoteOrNot+")^"+tok.getConfidence()+" ");
                }
                
               String tokLang=tok.lang; 
               query = query.append("subject_"+tokLang+"Stem:("+quoteOrNot+ tok.surface + quoteOrNot+")^"+tok.getConfidence()+" ");
               query = query.append("title_"+tokLang+"Stem:("+quoteOrNot+ tok.surface + quoteOrNot+")^"+tok.getConfidence()+" ");
               query = query.append("tableOfContents_"+tokLang+"Stem:("+quoteOrNot+ tok.surface + quoteOrNot+")^"+tok.getConfidence()+" ");
               query = query.append("description_"+tokLang+"Stem:("+quoteOrNot+ tok.surface + quoteOrNot+")^"+tok.getConfidence()+" ");
                
            }// for  
          
            return query.toString();
     }
    
    private void createProcessQueryResponseDocument(Document doc, Hashtable<String, Vector<NLPtoken>> topics, 
    		HashSet<String> translated, HashSet<String> translatedExpanded, HashSet<String> expanded,
    		String searchQuery, String inputLang,Hashtable<String,HashSet<NLPtoken>> notTranslatedByLang) {
        
		try {
			// create the root element and append it to the XML document
			Element rootElm = doc.getDocumentElement();
			Element queryElm = doc.createElement(CACAO_queryWS_client.queryElementTag);			
			queryElm.setAttribute(CACAO_queryWS_client.inputLangAttr, inputLang);
			rootElm.appendChild(queryElm);
			
			Element searchQueryElm=doc.createElement(CACAO_queryWS_client.searchQueryTag);
			searchQueryElm.setTextContent(searchQuery);
			queryElm.appendChild(searchQueryElm);
			
			for(String lang: topics.keySet()){
				Vector<NLPtoken> topics4lang=topics.get(lang);
				if(topics4lang!=null){
					for(NLPtoken item : topics4lang){
						float boost=item.getConfidence();
						Element topicElm = item.toXMLElement(doc, CACAO_queryWS_client.searchTermTag);
						if(translated.contains(item.getLemma_Category()))
							topicElm.setAttribute(CACAO_queryWS_client.isTranslatedAttribute, "true");
						if(expanded.contains(item.getLemma_Category()))
							topicElm.setAttribute(CACAO_queryWS_client.isExpandedAttribute, "true");
						if(translatedExpanded.contains(item.getLemma_Category()))
							topicElm.setAttribute(CACAO_queryWS_client.isTranslExpAttribute, "true");
						queryElm.appendChild(topicElm);
					}
				}
				HashSet<NLPtoken> notTranslated=notTranslatedByLang.get(lang);
				if(notTranslated!=null){
					for(NLPtoken item : notTranslated){
						float boost=item.getConfidence();
						Element topicElm = item.toXMLElement(doc, CACAO_queryWS_client.searchTermTag);
						queryElm.appendChild(topicElm);
					}
				}
			}
			
		} catch (Exception e) {
			logger.error("Error creating response document for query:\n["+searchQuery+"\n."+e);
		}
    }
    
    private void addFacetAndRecordsElements(Document doc, Element facets, Element records) {
        DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
		try {
			Element rootElm = doc.getDocumentElement();
			
			if(facets!=null){
				Node newOneToMove = doc.importNode(facets, true);
				rootElm.appendChild(newOneToMove);
			}
			if(records!=null){
				Node newOneToMove = doc.importNode(records, true);
				rootElm.appendChild(newOneToMove);
			}
			
			
			
		} catch (Exception pce) {
			
		}
    }
    
    private static String writeResponseDocumentToString(Document doc){
    	try {
			StringWriter tmp=new StringWriter();
			if(doc==null){
				DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
				DocumentBuilder db = dbf.newDocumentBuilder();
				doc = db.newDocument();
				Element rootElm = doc.createElement(QueryManager.rootElmTag);
				rootElm.setAttribute("errorProcessingRequest", "true");
				doc.appendChild(rootElm);
			}
			XMLSerializer serializer = new XMLSerializer(tmp, new OutputFormat(doc,"UTF-8",true));
			serializer.serialize(doc);
			return tmp.toString();
		} catch (Exception e) {
			return null;
		}
    }
    
    private boolean initService(String xmlConfigurationFile) {
        try {
        	logger.debug("Init Query Manager from "+xmlConfigurationFile);
        	
            // getting the document builder Factory
            DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();

            // if the document has to be validated against the schema source

            // getting the document builder from the Factory
            DocumentBuilder builder = factory.newDocumentBuilder();

            // parses the xml
            Document myDocument = builder.parse(new File(xmlConfigurationFile));
            
            //Setting default option for query processing (attributes at document element level)
            Element root = myDocument.getDocumentElement();
            String defaultOption = root.getAttribute(QueryManager.SVexpansionAttr);
            if(defaultOption!=null && defaultOption.length()>0)
            	QueryManager.SVexpansion = Boolean.parseBoolean(defaultOption);
            defaultOption = root.getAttribute(QueryManager.WNexpansionAttr);
            if(defaultOption!=null && defaultOption.length()>0)
            	QueryManager.WNexpansion = Boolean.parseBoolean(defaultOption);
            defaultOption = root.getAttribute(QueryManager.W2CexpansionAttr);
            if(defaultOption!=null && defaultOption.length()>0)
            	QueryManager.W2Cexpansion = Boolean.parseBoolean(defaultOption);
            defaultOption = root.getAttribute(QueryManager.translateAsSentenceAttr);
            if(defaultOption!=null && defaultOption.length()>0)
            	QueryManager.translateAsSentence = Boolean.parseBoolean(defaultOption);
            defaultOption = root.getAttribute(QueryManager.disambiguateWithSVAttr);
            if(defaultOption!=null && defaultOption.length()>0)
            	QueryManager.disambiguateTranslationsWithSV = Boolean.parseBoolean(defaultOption);
            defaultOption = root.getAttribute(QueryManager.disambiguateWithConfidenceAttr);
            if(defaultOption!=null && defaultOption.length()>0)
            	QueryManager.disambiguateTranslationsWithAvgConfidence = Boolean.parseBoolean(defaultOption);
            
            //NLP services settings (multiple services support to be added)
            NodeList nl = myDocument.getElementsByTagName(QueryManager.nlpServiceElementTag);
            for (int i = 0; i < nl.getLength(); i++) {
                Element elm = (Element) nl.item(i);
                QueryManager.nlpWS_endpoint = elm.getAttribute(QueryManager.urlAttribute);
            }
            
            //Translation services settings (multiple services support to be added)
            nl = myDocument.getElementsByTagName(QueryManager.translationServiceElementTag);
            for (int i = 0; i < nl.getLength(); i++) {
                Element elm = (Element) nl.item(i);
                QueryManager.translationWS_endpoint = elm.getAttribute(QueryManager.urlAttribute);
            }
            
            //Thesauri services settings (multiple services support to be added)
            nl = myDocument.getElementsByTagName(QueryManager.thesauriServiceElementTag);
            for (int i = 0; i < nl.getLength(); i++) {
                Element elm = (Element) nl.item(i);
                QueryManager.thesauriWS_endpoint = elm.getAttribute(QueryManager.urlAttribute);
            }
            
            //Search services settings 
            nl = myDocument.getElementsByTagName(QueryManager.searchServiceElementTag);
            if(nl!= null && nl.getLength()==1){
                Element elm = (Element) nl.item(0);
                QueryManager.searchWS_endpoint = elm.getAttribute(QueryManager.urlAttribute);
            }
            
            //Corpus Thesaurus settings
            nl = myDocument.getElementsByTagName(QueryManager.corpusThesaurusConfigurationElementTag);
            if(nl!= null && nl.getLength()==1){
                QueryManager.corpusThesaurusConfigurationElm = (Element) nl.item(0); 
                QueryManager.semVectSearcher= new CorpusThesaurus_Component(QueryManager.corpusThesaurusConfigurationElm);
            }

            //Word2Cat settings
            nl = myDocument.getElementsByTagName(QueryManager.Word2CategoryElementTag);
            if(nl!= null && nl.getLength()==1){
                Element elm = (Element) nl.item(0);
                QueryManager.w2c_user = elm.getAttribute(QueryManager.userAttribute);
                QueryManager.w2c_pwd = elm.getAttribute(QueryManager.pwdAttribute);
                QueryManager.w2c_host = elm.getAttribute(QueryManager.hostAttribute);
            }
            
            //NE detector
            Element neDetector = (Element) myDocument.getElementsByTagName(QueryManager.neDetectorTag).item(0);
            QueryManager.neDetector = new NamedEntitiesDetector(neDetector);
            
            //supported Languages
            nl = myDocument.getElementsByTagName(QueryManager.supportedLanguagesElementTag);
            if(nl!= null && nl.getLength()==1){
                Element elm = (Element) nl.item(0);
                String tmp = elm.getTextContent();
                QueryManager.supportedLanguages = new HashSet<String>();
                for(String lang: tmp.split(",")){
                	QueryManager.supportedLanguages.add(lang.toLowerCase().trim());
                }
            }
            
            return true;
        } catch (Exception e) {
            logger.error("Error initializing XIP NLP service.\n" +  e.getMessage());
            return false;
        }
    }

     static public void main(String[] args) {
        try { // demo.celi.it:8095
        	System.err.close();
            QueryManager a = new QueryManager();//
            String result = a.processQuery("nucleare esplosione bomba", "it", new String[]{"pl","en","it","fr","de"});
            System.out.println(">> " + result);
            
//           result = a.processQueryAndSearch("latin satiric poetry", "en", new String[]{"en","fr","de"}, new String[]{"language"}, 0, 10);
//           System.out.println(">> " + result);
        } catch (Exception e) {
            e.printStackTrace();
        }
    } 
}
