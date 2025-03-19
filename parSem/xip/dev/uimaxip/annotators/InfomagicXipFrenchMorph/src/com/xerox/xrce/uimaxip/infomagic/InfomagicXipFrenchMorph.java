package com.xerox.xrce.uimaxip.infomagic;

import infomagic.types.xerox.Lemma;
import it.celi.uima.lts.Token;

import java.lang.reflect.Array;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

import org.apache.uima.UIMARuntimeException;
import org.apache.uima.jcas.JCas;
import org.apache.uima.jcas.cas.FSArray;

import com.xerox.jxip.XipLeaf;
import com.xerox.jxip.XipNode;
import com.xerox.xrce.uimaxip.core.UimaXip_Base;

public class InfomagicXipFrenchMorph extends UimaXip_Base {
		
	private static String features[] = {
		"sg", "pl", "masc", "fem", "p1", "p2", "p3", "nom", "acc", "dat", "gen" 
	};
	
	private static int maxLemmas = 128;
	
	private static Map<String,Integer> categories;
	
	// todo: read categories from a config file?  Or even directly from the grammar?  Or get it from XIP?
	// Same for the mapping from unary dependencies to annotations.
	static {
		HashMap<String,Integer> cats = new HashMap<String,Integer>();
		int i=0;
		cats.put("GROUPE",i++);
		cats.put("CM",i++);
		cats.put("ADV",i++);
		cats.put("ADJ2",i++);
		cats.put("ADJ",i++);
		cats.put("COORD",i++);
		cats.put("VERB",i++);
		cats.put("NOUN",i++);
		cats.put("VAUX",i++);
		cats.put("PAP",i++);
		cats.put("PRP",i++);
		cats.put("DET",i++);
		cats.put("PREP",i++);
		cats.put("PRON",i++);
		cats.put("PC",i++);
		cats.put("RELPRO",i++);
		cats.put("CONJQUE",i++);
		cats.put("NEG",i++);
		cats.put("NUM",i++);
		cats.put("VOICILA",i++);
		cats.put("CONJ",i++);
		cats.put("PUNCT",i++);
		cats.put("OPAR",i++);
		cats.put("CPAR",i++);
		cats.put("SENT",i++);
		cats.put("SGML",i++);
		cats.put("PREFIX",i++);
		categories = Collections.unmodifiableMap(cats);
		
	}

	@Override
	protected void processToken(XipNode xipToken, JCas cas, int offset) {
		
		Token uimaToken = new Token(cas, (int) xipToken.getLeftChar()+offset, (int) xipToken.getRightChar()+offset);
		Lemma[] uimaLemmas = new Lemma[maxLemmas];
		uimaToken.setForm(xipToken.getSurface());
		
		Vector<XipNode> xipReadings = xipToken.getDaughters();
		int lemmaCount = 0;
		for (XipNode xipReading : xipReadings) {
			Lemma uimaLemma = new Lemma(cas);
			uimaLemma.setLemma(xipReading.getLemma());
			if (xipReading.getFeature("bd") != null) {
				uimaLemma.setSemCat("comicbooks");  
			} else if (xipReading.getFeature("cinema") != null) {
				uimaLemma.setSemCat("cinema");  
			} else if (xipReading.getFeature("danse") != null) {
				uimaLemma.setSemCat("dance");  
			} else if (xipReading.getFeature("theatre") != null) {
				uimaLemma.setSemCat("theatre");  
			} else if (xipReading.getFeature("musique") != null) {
				uimaLemma.setSemCat("music");  
			} else if (xipReading.getFeature("jeu") != null) {
				uimaLemma.setSemCat("games");  
			} else if (xipReading.getFeature("litterature") != null) {
				uimaLemma.setSemCat("literature");  
			} else if (xipReading.getFeature("peinture") != null) {
				uimaLemma.setSemCat("painting");  
			} else if (xipReading.getFeature("architecture") != null) {
				uimaLemma.setSemCat("architecture");  
			} else if (xipReading.getFeature("sculpture") != null) {
				uimaLemma.setSemCat("sculpture");  
			} else if (xipReading.getFeature("photo") != null) {
				uimaLemma.setSemCat("photography");  
			} else if (xipReading.getFeature("culturel") != null) {
					uimaLemma.setSemCat("cultural");
			} else if (xipReading.getFeature("guerre") != null) {
				uimaLemma.setSemCat("military");  
			} else if (xipReading.getFeature("sport") != null) {
				uimaLemma.setSemCat("sports");  
			}
			//uimaLemma.setSemCat(xipReading.getFeature("genre"))
			String catS = xipReading.getCategory();
			Integer catI = categories.get(catS);
			if (catI == null) {
				throw new UIMARuntimeException(stringsResourceName, "UnknownCategory", new Object[] {catS});
			}
			uimaLemma.setLexCatS(catS);
			uimaLemma.setLexCatI(catI);
			uimaLemma.setLexSymbol(uimaLemma.getLemma() + "/" + catS);
			
			// currently not exporting morphological information.
			//setMorphFeatures(xipReading, uimaLemma);

			// Add to list of UIMA lemmas only if it's distinguishable from those already
			// stored, considering only the features we're translating.
			int i;
			for (i = 0; i < lemmaCount; i++) {
				if (lemmaEquiv(uimaLemmas[i], uimaLemma)) {
					break;
				}
			}
			if (i >= lemmaCount) {
				uimaLemmas[lemmaCount++] = uimaLemma;
			}
		}
		FSArray Lemmas = new FSArray(cas, lemmaCount);
		Lemmas.copyFromArray(uimaLemmas, 0, 0, lemmaCount);
		uimaToken.setLemmas(Lemmas);
		uimaToken.addToIndexes();
	}
	// todo: subclass Lemma and make this a method of the new class?
	private boolean lemmaEquiv(Lemma x, Lemma y) {
		return x.getLemma().equals(y.getLemma()) && 
			x.getLexCatI() == y.getLexCatI() &&
			x.getSynCatI() == y.getSynCatI() &&
			x.getMorphCatI() == y.getMorphCatI(); 
	}
	
	@SuppressWarnings("unused")
	private static void setMorphFeatures(XipLeaf source, Lemma dest) {
		int resulti = 0;
		String results = "";
		for (int i = 0; i < Array.getLength(features); i++) {
			if (source.getFeature(features[i]) != null) {
				resulti |= 1 << i;
				results += "+" + features[i];
			}
		}
		dest.setMorphCatI(resulti);
		dest.setMorphCatS(results);
	}
}
