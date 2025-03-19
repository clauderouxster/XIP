package eu.project.Cacao.Components.Utils;


import it.celi.commons.morphoLemma.DisambiguatorI;
import it.celi.commons.morphoLemma.LemmatizedWordI;
import it.celi.commons.morphoLemma.MorphoLemmatizerI;
import it.celi.commons.morphoLemma.TokenizerI;
import it.celi.morphoLemma.CharBasedTokenizer;
import it.celi.morphoLemma.DocDiggerDisambiguator;
import it.celi.morphoLemma.MorphoLemmaFactory;
import it.celi.util.mapper.CatMapper;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.util.Properties;
import java.util.StringTokenizer;
import java.util.Vector;

import org.apache.log4j.Logger;
import com.sun.org.apache.xerces.internal.impl.io.UTF8Reader;

public class PolishLemmatizer implements Lemmatizer{
	private Logger logger = Logger.getLogger(PolishLemmatizer.class);
	private final String lang = "pl";
	private static final String propertyFilePolishLemmatizer = "configuration/lemmatizer-pl-sample.properties";
	private MorphoLemmatizerI sm;
	private CatMapper mapper;
	
	public PolishLemmatizer(){
		this.initializeService();
	}
	
	public boolean initializeService(){
		return this.initializeService(propertyFilePolishLemmatizer);
	}
	
	public boolean initializeService(String propertyFile){
		try {
			
			Properties pr = new Properties();
			FileInputStream fis = new FileInputStream(propertyFile);
			pr.load(fis);
			fis.close();
			this.sm = MorphoLemmaFactory.createTrieMorphology(pr);
			this.sm.initialize(pr);
			
			// ***********************
			// instantiate the Tokenizer
			// ***********************
			TokenizerI t = new CharBasedTokenizer();
			Properties tp = new Properties();
			tp.setProperty(CharBasedTokenizer.KEEP_PUNCT_ATT, "true");
			t.initialize(tp);
			this.sm.setTokenizer(t);
			
			// ***********************
			// instantiate the disambiguator
			// ***********************
			StringTokenizer st = new StringTokenizer(pr.getProperty("priorityList"));
			String[] pcat = new String[st.countTokens()];
			for (int i = 0; i < pcat.length; i++) {
				pcat[i] = st.nextToken();
			}
			String mapFile = pr.getProperty("mapperFile");
			DisambiguatorI d = new DocDiggerDisambiguator(pcat, mapFile);
			this.sm.setDisambiguator(d);
			
			// create a MapperCat for displaying lex cat
			this.mapper = it.celi.morphoLemma.Utils.provideMappers(mapFile);
			
			return true;
		} catch (Exception e) {
			logger.error("Error initializing PolishLemmatizer: "+e);
			return false;
		}
	}
	
	public Vector<NLPtoken> parseTerm(String text){
		Vector<NLPtoken> results = new Vector<NLPtoken>();
		try {
			LemmatizedWordI[] out;
			LemmatizedWordI[][] outAmb = this.sm.analyzeAmb(text, true);
			for (int i = 0; i < outAmb.length; i++) {
				out = outAmb[i];
				for (int ii = 0; ii < out.length; ii++) {
					String srfc = out[ii].getSource();
					String newLemma = out[ii].getLemma().replaceAll("_[A-Z]+", "").trim();
					String POS = this.mapper.sendString(out[ii].getCat());
					NLPtoken tok = new NLPtoken(srfc,POS, newLemma, null, this.lang);
					results.add(tok);
				}

			}
		} catch (Exception e) {
			logger.error("Error while analizing polish term: "+text);
		}
		
		return results;
	}
	
	public Vector<NLPtoken> parsePhrase(String text){
		Vector<NLPtoken> results = new Vector<NLPtoken>();
		try {
			LemmatizedWordI[] out = this.sm.analyze(text, true);
			for (int i = 0; i < out.length; i++) {
				String srfc = out[i].getSource();
				String newLemma = out[i].getLemma().replaceAll("_[A-Z]+", "").trim();
				String POS = this.mapper.sendString(out[i].getCat());
				NLPtoken tok = new NLPtoken(srfc,POS, newLemma, null, this.lang);
				results.add(tok);
			}
		} catch (Exception e) {
			logger.error("Error while analizing polish phrase: "+text+"\n"+e);
		}
		return results;
	}
	
	public static void main(String[] args) {
		try {
			PolishLemmatizer pl=new PolishLemmatizer();
			pl.initializeService();
			
			FileInputStream fis = new FileInputStream("CACAO_data/PolishLemmatizer/lexicon/todel.txt");
			UTF8Reader ur = new UTF8Reader(fis);
			BufferedReader br = new BufferedReader(ur);
			String in=br.readLine();
			
			Vector<NLPtoken> v = pl.parsePhrase(in);
			for(NLPtoken t: v){
				System.out.println(t);
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

}
