package eu.project.Cacao.Components.Utils;


import java.io.UnsupportedEncodingException;
import java.nio.charset.Charset;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.Vector;

import org.apache.log4j.Logger;

import com.xerox.jxip.JXip;
import com.xerox.jxip.JXip2;
import com.xerox.jxip.XipDependency;
import com.xerox.jxip.XipLeaf;
import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipResult;
import com.xerox.jxip.XipResultManager;
import com.xerox.jxip.XipUnit;

public class XIPwrapper implements Lemmatizer{
	private Logger logger = Logger.getLogger(XIPwrapper.class);
	
	static private final String prefix="CACAO_";
	static public final String enGram=prefix+"data/XIP/grammar/english/entity/gram_gen_entit.grm";
	static public final String frGram=prefix+"data/XIP/grammar/french/BASIC/french_entityOnly.grm";
	static public final String deGram=prefix+"data/XIP/grammar/german/GRMFILES/german.grm";
	static public final String itGram=prefix+"data/XIP/grammar/italian/italian.grm";

	
	private JXip jxip;
	private JXip2 jxip2;
	private int gHandler;
	private XipResultManager xiprm;
	private XipResult xipres;
	private String lang;

	public XIPwrapper(String gramFile, String lang) {
		this.lang = lang;
		
		logger.debug("Using " + gramFile+" for "+lang);
		try {
			jxip = new JXip();
			jxip2=new JXip2();
			xiprm = new XipResultManager();
			gHandler = jxip.XipGrmFile(gramFile, 0, true);
		} catch (Exception ex) {
			logger.error("Error launching XIP with grammar "+gramFile+"\n"+ex.getMessage());
		}
	}
	
	
	public Vector<NLPtoken> parseTerm(String src) {
		try {
			jxip2.XipSetRuleLimit(gHandler, 1);
			Vector<NLPtoken> result=new Vector<NLPtoken>();
			
			int res = xiprm.buildXipResult(gHandler, src);
			if (res != 0) {
				logger.error("Error getting xip results..");
				return null;
			}
			
			xipres = xiprm.getXipResult();
			Iterator<XipUnit> unitIter = xipres.getXipUnits().iterator();
			if( unitIter.hasNext() ) {
				XipUnit unit =  unitIter.next();
				result.addAll( this.analyzeXipUnit(unit) );
			}
			
			return result;
		} catch (Exception e) {
			e.printStackTrace();
			return null;
		}

	}
	
	//Each string[] has 4 entries surface - category - lemma - isNamedEntity (true or false)
	public Vector<NLPtoken> parsePhrase(String src) {
		try {
			jxip2.XipSetRuleLimit(gHandler, -1);
			Vector<NLPtoken> results = new Vector<NLPtoken>();
		
			int res = xiprm.buildXipResult(gHandler, src);
			if (res != 0) {
				logger.error("Error getting xip results..");
				return null;
			}
			
			xipres = xiprm.getXipResult();
			for (Iterator<XipUnit> unitIter = xipres.getXipUnits().iterator(); unitIter.hasNext(); ) {
				XipUnit unit = unitIter.next();
				Vector<NLPtoken> analyzedTerms = this.analyzeXipUnit(unit);
				
				NLPtoken prev=null;
				for(NLPtoken tok: analyzedTerms){
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
			}
			
			return results;
		} catch (Exception e) {
			logger.error("Error parsing phrase with XIP.. "+e.getMessage());
			return null;
		}

	}
	
	private NLPtoken selectTokWithPOS(NLPtoken termA, NLPtoken termB, String POS){
		if(termA.category.equalsIgnoreCase(POS))
				return termA;
		else 	return termB;
	}
	
    private NLPtoken selectDominantTermUsingQueryHeuristic(NLPtoken termA, NLPtoken termB){
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

	private Vector<NLPtoken> analyzeXipUnit(XipUnit unit){
		
		Hashtable<String,String> namedEntities=new Hashtable<String,String>();
		for (Iterator<XipDependency> depIter = unit.getDependencies().iterator(); depIter.hasNext(); ) {
			XipDependency dep = depIter.next();
			String relName=dep.getName();
			if(relName.equals("PERSON")||relName.equals("ORGANISATION")||relName.equals("LOCORG")||relName.equals("LOCATION")){
				Vector<XipNode> params=dep.getParameters();
				for(XipNode xipToken: params){
						namedEntities.put(xipToken.getSurface(), relName);
				}
			}
		}
		
		Vector<NLPtoken> results=new Vector<NLPtoken>();
		for (Iterator<XipLeaf> leafIter = unit.getLeaves().iterator(); leafIter.hasNext(); ) {
			XipLeaf xipToken = leafIter.next();
			
			String surface = "", category="", lemma="";
			if(this.lang.equals("it")){
				try {
					surface= new String(xipToken.getSurface().getBytes("ISO-8859-1"),"UTF-8");
					category = xipToken.getCategory();
					lemma= new String(xipToken.getLemma().getBytes("ISO-8859-1"),"UTF-8");
				} catch (UnsupportedEncodingException e) {
					surface = xipToken.getSurface();
					category = xipToken.getCategory();
					lemma = xipToken.getLemma();
				}
			}else{
				surface = xipToken.getSurface();
				category = xipToken.getCategory();
				lemma = xipToken.getLemma();
			}
			logger.debug("surface "+surface+" in "+Charset.defaultCharset().displayName());
			if(category == null || lemma== null )
				continue;
			
			String neCategory=null;
			if(namedEntities.keySet().contains(surface)){
				neCategory = namedEntities.get(surface);
			}
			NLPtoken element = new NLPtoken(surface, category, lemma, neCategory, this.lang);
			
			if(!results.contains(element)){
				results.add(element);
				logger.debug("Added "+element+" as NLP analysis token");
			}
		}
		return results;
	}
	
}
