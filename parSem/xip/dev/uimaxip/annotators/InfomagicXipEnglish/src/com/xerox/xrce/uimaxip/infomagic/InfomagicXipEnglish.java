package com.xerox.xrce.uimaxip.infomagic;

import infomagic.types.NamedEntity;
import infomagic.types.City;
import infomagic.types.Company;
import infomagic.types.Country;
import infomagic.types.Location;
import infomagic.types.Organisation;
import infomagic.types.Person;
import infomagic.types.Date;
import infomagic.types.TemporalExpression;
import infomagic.types.Event;
import infomagic.types.Money;
import it.celi.uima.lts.Lemma;
import it.celi.uima.lts.Token;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

import org.apache.uima.UIMARuntimeException;
import org.apache.uima.analysis_engine.AnalysisEngineProcessException;
import org.apache.uima.cas.CASException;
import org.apache.uima.jcas.JCas;
import org.apache.uima.jcas.cas.FSArray;
import org.apache.uima.jcas.tcas.Annotation;

import com.xerox.jxip.XipDependency;
import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipUnit;
import com.xerox.xrce.uimaxip.core.UimaXip_Base;

public class InfomagicXipEnglish extends UimaXip_Base {
		
	private static int maxLemmas = 128;
	private HashMap<XipNode,Person> xipNode2Person = new HashMap<XipNode,Person>();
	
	private static Map<String,Integer> categories;
	
	private static boolean txtCas = false;
	private static boolean displayToken = false;

	
	// todo: read categories from a config file?  Or even directly from the grammar?  Or get it from XIP?
	// Same for the mapping from unary dependencies to annotations.
	static {
		HashMap<String,Integer> cats = new HashMap<String,Integer>();
		int i=0;
		cats.put("NOUN",i++);
		cats.put("VERB",i++);
		cats.put("AUX",i++);
		cats.put("ADJ",i++);
		cats.put("NADJ",i++);
		cats.put("PREP",i++);
		cats.put("ADV",i++);
		cats.put("CONJ",i++);
		cats.put("NUM",i++);
		cats.put("DIG",i++);
		cats.put("DET",i++);
		cats.put("PRON",i++);
		cats.put("PUNCT",i++);
		cats.put("QUANT",i++);
		cats.put("SGML",i++);
		cats.put("PART",i++);
		cats.put("MEAS",i++);
		cats.put("INTERJ",i++);
		cats.put("INFTO",i++);
		cats.put("IV",i++);
		cats.put("FV",i++);
		cats.put("GV",i++);
		cats.put("NFV",i++);
		cats.put("AP",i++);
		cats.put("NP",i++);
		cats.put("PP",i++);
		cats.put("ADVP",i++);
		cats.put("NUMP",i++);
		cats.put("BG",i++);
		cats.put("INS",i++);
		cats.put("SC",i++);
		cats.put("ST",i++);
		cats.put("PART",i++);
		cats.put("FICT",i++);
		categories = Collections.unmodifiableMap(cats);
		
	}

	@Override
	protected void processXipUnit(XipUnit unit, JCas cas, int offset) 
		throws AnalysisEngineProcessException {
		xipNode2Person.clear();
		if (txtCas){
			JCas xmlView;
			try {
				xmlView = cas.getView("txt");
			} catch (CASException e) {
				throw new AnalysisEngineProcessException(e);
			}
			cas = xmlView;
		}

		for (XipDependency dep : unit.getDependencies()) {
			processDependency(dep,cas,offset);
		}
		if (displayToken){
			for (XipNode xipToken : unit.getLeafNodes()) {
				processToken(xipToken, cas, offset);
			}
		}
	}

	@Override
	protected void processDependency(XipDependency dep, JCas cas, int offset) 
	  throws AnalysisEngineProcessException {
		NamedEntity newAnnot = null;
		String depname = dep.getName();
		if (depname.equals("LOCORG")) {
			if (dep.hasFeature("CITY", "*")) {
				newAnnot = new City(cas);
			} else if (dep.hasFeature("COUNTRY", "*")) {
				newAnnot = new Country(cas);
			} else {
				newAnnot = new Location(cas);
			}
		} else if (depname.equals("PERSON")) {
			newAnnot = new Person(cas);
			XipNode n = dep.getArg(1);
			if (n == null) {
				throw new AnalysisEngineProcessException(stringsResourceName,
						"DepWrongNumArgs",
						new Object[] {depname, 0, 1});
			}
			xipNode2Person.put(n, (Person) newAnnot);
		} else if (depname.equals("INTERNALS_FIRSTNAME1") ||
				   depname.equals("INTERNALS_LASTNAME") ||
				   depname.equals("INTERNALS_FIRSTNAME2") ||
				   depname.equals("INTERNALS_TITLE")) {
			XipNode xipNode1 = dep.getArg(1);
			XipNode xipNode2 = dep.getArg(2);
			if (xipNode1 == null || xipNode2 == null) {
				throw new AnalysisEngineProcessException(stringsResourceName,
						"DepWrongNumArgs",
						new Object[] {depname, dep.getParameters().size(), 2});
			}
			Person person = xipNode2Person.get(xipNode1);
			if (person == null) {
				person = new Person(cas);
				newAnnot = person;
				xipNode2Person.put(xipNode1, person);
				newAnnot = person;
			}
			if (depname.equals("INTERNALS_FIRSTNAME1")) {
				person.setFirstname(xipNode2.getLemma());
			} else if (depname.equals("INTERNALS_LASTNAME")) {
				person.setLastname(xipNode2.getLemma());
			} else if (depname.equals("INTERNALS_TITLE")) {
				person.setTitle(xipNode2.getLemma());
			} else if (depname.equals("INTERNALS_FIRSTNAME2")) {
				person.setMiddlename(xipNode2.getLemma());
			} else {
				assert false;
			}
		} else if (depname.equals("ORGANISATION")) {
			if (dep.hasFeature("COMP", "*")) {
				newAnnot = new Company(cas);
			} else {
				newAnnot = new Organisation(cas);
			}
		} else if (depname.equals("LOCATION")) {
			newAnnot = new Location(cas);
		} else if (depname.equals("DATE")) {
			newAnnot = new Date(cas);
		} else if (depname.equals("EVENT")) {
			newAnnot = new Event(cas);
		} else if (depname.equals("MONEY")) {
			newAnnot = new Money(cas);
		} else if (depname.equals("TIMEX3")) {
			newAnnot = new TemporalExpression(cas);
		}
		
		if (newAnnot != null) {
			Vector<XipNode> params = dep.getParameters();
			if (params.size() != 1) {
				throw new AnalysisEngineProcessException(stringsResourceName,
						"DepWrongNumArgs",
						new Object[] {depname, params.size(), 1});
			}
			XipNode node = params.firstElement();
			newAnnot.setBegin((int) node.getLeftChar() + offset);
			newAnnot.setEnd((int) node.getRightChar() + offset);
			newAnnot.setSource("xipEnglish");
			newAnnot.addToIndexes();
		}
	}

	@Override
	protected void processToken(XipNode xipToken, JCas cas, int offset) {
		Token uimaToken = new Token(cas, (int) xipToken.getLeftChar() + offset, (int) xipToken.getRightChar() + offset);
		Lemma[] uimaLemmas = new Lemma[maxLemmas];
		uimaToken.setForm(xipToken.getSurface());
		
		Vector<XipNode> xipReadings = xipToken.getDaughters();
		int lemmaCount = 0;
		for (XipNode xipReading : xipReadings) {
			Lemma uimaLemma = new Lemma(cas);
			uimaLemma.setLemma(xipReading.getLemma());
			String catS = xipReading.getCategory();
			Integer catI = categories.get(catS);
			if (catI == null) {
				throw new UIMARuntimeException(stringsResourceName, "UnknownCategory", new Object[] {catS});
			}
			uimaLemma.setLexCatS(catS);
			uimaLemma.setLexCatI(catI);
			uimaLemma.setLexSymbol(uimaLemma.getLemma() + "/" + catS);
			
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
	
}
