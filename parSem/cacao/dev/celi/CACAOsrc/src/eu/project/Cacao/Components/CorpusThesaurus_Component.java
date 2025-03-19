package eu.project.Cacao.Components;

import it.celi.RandomIndexing.search.RAMSearch;
import it.celi.RandomIndexing.search.SearchResult;
import it.celi.RandomIndexing.utils.ObjectVector;

import java.util.Arrays;
import java.util.Comparator;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.List;
import java.util.Vector;

import org.apache.log4j.Logger;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

import eu.project.Cacao.Components.Utils.NLPtoken;
import eu.project.Cacao.Managers.CorpusThesaurus_Manager;


public class CorpusThesaurus_Component {
	private static final int maxTranslations=3;
	private static final float disambiguationThresholdFactor=0.6f;
	
	private class SemDistanceComparator implements Comparator<NLPtoken>{

		public int compare(NLPtoken o1, NLPtoken o2) {
			return (int)((o2.getConfidence()-o1.getConfidence())*100000);
		}
		
	}
	
    private static Hashtable<String, String[]> supportedSemVectSpaces;
    private static Hashtable<String, RAMSearch> semVectSpaces;
    
    private Logger logger = Logger.getLogger(CorpusThesaurus_Component.class);
    private boolean lowerCaseSearchKey=false;

    
    /**
     * corpusThesaurusConfiguration element of QueryManager configuration document
     * @param configElement
     */
    public CorpusThesaurus_Component(Element configElement) {
    	CorpusThesaurus_Component.supportedSemVectSpaces=new Hashtable<String, String[]>();
    	CorpusThesaurus_Component.semVectSpaces = new Hashtable<String, RAMSearch>();
    	this.initComponent(configElement);
    }
    
    public CorpusThesaurus_Component(Hashtable<String, String[]> supportedSemSpaces) {
    	CorpusThesaurus_Component.supportedSemVectSpaces=supportedSemSpaces;
    	CorpusThesaurus_Component.semVectSpaces = new Hashtable<String, RAMSearch>();
    	for(String lang: CorpusThesaurus_Component.supportedSemVectSpaces.keySet()){
    		String[] initParams=CorpusThesaurus_Component.supportedSemVectSpaces.get(lang);
    		CorpusThesaurus_Component.semVectSpaces.put(lang, new RAMSearch(initParams[0],initParams[1],this.compoundContentFields(initParams[2])));
    	}
    }

    
    public Hashtable<NLPtoken,Vector<NLPtoken>> disambiguateTranslations(Hashtable<NLPtoken,Vector<NLPtoken>> translatedKW, String lang){
        if (!supportedSemVectSpaces.containsKey(lang)) {
            return null;
        } 
        RAMSearch mySemVectSearcher = semVectSpaces.get(lang);
        if(mySemVectSearcher==null || !mySemVectSearcher.isSuccessfulInit()){
        	logger.error("Not Loaded SemVect for lang "+lang);
        	return null;
        }
        
    	Hashtable<NLPtoken,Vector<NLPtoken>> disambiguated=new Hashtable<NLPtoken, Vector<NLPtoken>>();
    	
    	for(NLPtoken srcTok: translatedKW.keySet()){
    		Vector<NLPtoken> vec=translatedKW.get(srcTok);
    		if(vec.size()>1){
    			Vector<NLPtoken> vectWithUpdatedConfidence=new Vector<NLPtoken>();
    			for(NLPtoken testTok: vec){
    				//testTok.setConfidence(0);
    				for(NLPtoken srcTokX: translatedKW.keySet()){
        	    		if(!srcTok.equals(srcTokX)){
        	    			float max=0;
        	    			for(NLPtoken testTokX:  translatedKW.get(srcTokX)){
        	    				float distance=this.getTermsDistance(testTokX, testTok, lang);
        	    				if(distance>max)
        	    					max=distance;
        	    			}
        	    			testTok.setConfidence(testTok.getConfidence()+max);
        	    			vectWithUpdatedConfidence.add(testTok);
        	    		} 		
        	    	}	
    	    	}
    			
    			SemDistanceComparator comp = new SemDistanceComparator();
    			NLPtoken[] tokArray=new NLPtoken[vectWithUpdatedConfidence.size()];
    			tokArray = vectWithUpdatedConfidence.toArray(tokArray);
    			Arrays.sort(tokArray, comp);
    			Vector<NLPtoken> disambiguatedEntry=new Vector<NLPtoken>();
    			int cnt=0;
    			for(NLPtoken tok: tokArray){
    				if(tok.getConfidence()>= tokArray[0].getConfidence()*CorpusThesaurus_Component.disambiguationThresholdFactor 
    						&& cnt<=CorpusThesaurus_Component.maxTranslations){
    					disambiguatedEntry.add(tok);
    					cnt++;
    				}
    				else
    					break;
    			}
    			disambiguated.put(srcTok, disambiguatedEntry);
    		}
    		else{
    			disambiguated.put(srcTok, vec);
    		}
    	}
    	
    	return disambiguated;
    }
    
    private HashSet<Vector<NLPtoken>> groupTerms(Vector<NLPtoken> terms, String lang){
        if (!supportedSemVectSpaces.containsKey(lang)) {
            return null;
        } 
        RAMSearch mySemVectSearcher = semVectSpaces.get(lang);
        if(mySemVectSearcher==null || !mySemVectSearcher.isSuccessfulInit()){
        	logger.error("Not Loaded SemVect for lang "+lang);
        	return null;
        }
    	
    	float avgDistance=0; int tot=0;
    	Hashtable<String,Float> distances=new Hashtable<String, Float>();
    	for(int i=0;i<terms.size();i++){
    		NLPtoken candidate =terms.elementAt(i);
        	for(int j=i+1;j<terms.size();j++){
        		NLPtoken comparison=terms.get(j);
        		float d = this.getTermsDistance(candidate, comparison, lang);
        		distances.put(candidate.getLemma_Category()+"<>"+comparison.getLemma_Category(), d);
        		avgDistance += d;
        		tot++;
        	}
    	}
    	avgDistance /= tot;
    	float acceptanceThreshold = avgDistance ;
    	
    	HashSet<Vector<NLPtoken>> groups=new HashSet<Vector<NLPtoken>>();
    	HashSet<NLPtoken> assigned=new HashSet<NLPtoken>();
    	for(NLPtoken pivot: terms){
    		if(!assigned.contains(pivot)){
    			assigned.add(pivot);
	    		Vector<NLPtoken> group=new Vector<NLPtoken>();
	    		group.add(pivot);
	        	for(NLPtoken candidate: terms){
	        		if(!group.contains(candidate)){
	        			boolean accepted=true;
	        			for(NLPtoken groupMember: group){
	        				String key=candidate.getLemma_Category()+"<>"+groupMember.getLemma_Category();
	        				String invKey=groupMember.getLemma_Category()+"<>"+candidate.getLemma_Category();
	        				float distance = (distances.containsKey(key))?distances.get(key):distances.get(invKey);
	        				//Remember: distance> 1 = equal ; 0 = totally unrelated
	    	        		if(distance < acceptanceThreshold)
	    	        			accepted = false;
	        			}
	        			if(accepted){
	        				group.add(candidate);
	        				assigned.add(candidate);
	        			}
	        		}
	        	}
	        	groups.add(group);
    		}
    	}
    	
    	return groups;
    }
    
    public float getTermsDistance(NLPtoken termA, NLPtoken termB, String lang) {
        
        if (!supportedSemVectSpaces.containsKey(lang)) {
            return -1;
        } 
        RAMSearch mySemVectSearcher = semVectSpaces.get(lang);
        if(mySemVectSearcher==null || !mySemVectSearcher.isSuccessfulInit()){
        	logger.error("Not Loaded SemVect for lang "+lang);
        	return -1;
        }
        
        String termA_SVsearckKey=termA.getLemma_Category();
        String termB_SVsearchKey=termB.getLemma_Category();
        if(this.lowerCaseSearchKey){
        	termA_SVsearckKey=termA_SVsearckKey.toLowerCase();
        	termB_SVsearchKey=termB_SVsearchKey.toLowerCase();
        }
        logger.debug("Searching distance in SV for "+termA_SVsearckKey+" and "+termB_SVsearchKey+" in lang "+lang);
        return mySemVectSearcher.getDistance(termA_SVsearckKey, termB_SVsearchKey);
    }

    public Vector<NLPtoken> getRelatedTerms(Vector<NLPtoken> queryTermsToExpand, String lang, int nResult, float boost) {
        Vector<NLPtoken> resSet = new Vector<NLPtoken>();
        
        if (!supportedSemVectSpaces.containsKey(lang)) {
            return resSet;
        } 
        RAMSearch mySemVectSearcher = semVectSpaces.get(lang);
        if(mySemVectSearcher==null || !mySemVectSearcher.isSuccessfulInit()){
        	logger.error("Not Loaded SemVect for lang "+lang);
        	return resSet;
        } 
        
        logger.info("exp req: "+queryTermsToExpand);
        HashSet<Vector<NLPtoken>> groups = this.groupTerms(queryTermsToExpand, lang); 
        for(Vector<NLPtoken> vec: groups){
        	String queryString = "";
        	if(vec.size()>1){
		        for (NLPtoken term : vec) {
		        	if(this.lowerCaseSearchKey)
		        		queryString += (term.getLemma_Category().toLowerCase() + " ");
		        	else
		        		queryString += (term.getLemma_Category() + " ");
		        }
        	}
        	else{
        		queryString = vec.firstElement().getLemma_Category();
        		if(this.lowerCaseSearchKey)
        			queryString = queryString.toLowerCase();
        	}
	        logger.debug("SemVect query (getNN): " + queryString);
	
	        List<SearchResult> results = mySemVectSearcher.getNearestNeighbours(queryString.trim(), nResult+queryTermsToExpand.size());
	
	        for (SearchResult res : results) {
	            String affineTerm = ((ObjectVector) res.getObject()).getObject().toString();
	
	            if (queryString.contains(affineTerm)) {
	                continue;
	            }
	
	            float score = res.getScore();
	
	            if ( !queryTermsToExpand.contains(affineTerm)) {
	                String category = "";
	                String lemma = affineTerm;
	                int idx = affineTerm.lastIndexOf('_');
	
	                if (idx > 0) {
	                    lemma = affineTerm.substring(0, idx);
	                    category = affineTerm.substring(idx + 1);
	                }
	                
	                if(this.lowerCaseSearchKey)
	                	category = category.toUpperCase();
	                NLPtoken tok = new NLPtoken(lemma, category, lemma, null, lang);
	                tok.setConfidence(boost);
	                resSet.add(tok);
	                logger.debug(" -query string " +queryString+" -> token : "+ tok );
	            }
	        }
        }

        return resSet;
    }
    
    
    private boolean initComponent(Element configurationElement) {
        try {
        	logger.debug("Init CorpusThesaurus Component from "+configurationElement);
            
            String boolAttr = configurationElement.getAttribute(CorpusThesaurus_Manager.lCaseSearchKeysAttrLabel);
            this.lowerCaseSearchKey=Boolean.parseBoolean(boolAttr);
            
            NodeList nl = configurationElement.getElementsByTagName(CorpusThesaurus_Manager.svSearcherTagname);
            for (int i = 0; i < nl.getLength(); i++) {
                Element elm = (Element) nl.item(i);
                String lang = elm.getAttribute(CorpusThesaurus_Manager.langAttrLabel);
                String luceneIDX = elm.getAttribute(CorpusThesaurus_Manager.luceneAttrLabel);
                String contentFields = elm.getAttribute(CorpusThesaurus_Manager.contentFieldsLabel);
                String semVectFilePath = elm.getTextContent();
                supportedSemVectSpaces.put(lang,  new String[] { semVectFilePath, luceneIDX, contentFields });
                RAMSearch ramSearch = new RAMSearch(semVectFilePath, luceneIDX, this.compoundContentFields(contentFields));
                semVectSpaces.put(lang, ramSearch);
            }

            return true;
        } catch (Exception e) {
            logger.error("Error initializing CorpusThesaurus component.\n" +
                e.getMessage());

            return false;
        }
    }

    private HashSet<String> compoundContentFields(String list) {
        String[] tmps = list.split(",( )*");
        HashSet<String> result = new HashSet<String>();

        for (String s : tmps) {
            result.add(s.trim());
        }

        return result;
    }
}
