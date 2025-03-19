// Copyright (C) 2007 Xerox Corporation, XRCE, All Rights Reserved.

package eu.sapir.contentmanagement.text;

import java.util.Vector;

import eu.sapir.contentmanagement.uimatypes.text.Lemma;
import eu.sapir.contentmanagement.uimatypes.text.Person;
import eu.sapir.contentmanagement.uimatypes.text.Place;
import eu.sapir.contentmanagement.uimatypes.text.Token;

import org.apache.uima.analysis_engine.AnalysisEngineProcessException;
import org.apache.uima.jcas.cas.FSArray;
import org.apache.uima.jcas.JCas;
import org.apache.uima.jcas.tcas.Annotation;
import com.xerox.jxip.XipDependency;
import com.xerox.jxip.XipNode;
import com.xerox.xrce.uimaxip.core.UimaXip_Base;

public class SapirXipAnnotator extends UimaXip_Base {

	private static int maxLemmas = 128;

	protected void processDependency(XipDependency dep, JCas cas) 
	  throws AnalysisEngineProcessException {
		Annotation annot;
		if (dep.getName().equals("PERSON")) {
			annot = new Person(cas);
		} else if (dep.getName().equals("LOCATION") || dep.getName().equals("LOCORG")) {
			annot = new Place(cas);
		} else {
			return;
		}
		Vector<XipNode> params = dep.getParameters();
		if (params.size() != 1) {
			throw new AnalysisEngineProcessException(stringsResourceName,
												"DepWrongNumArgs",
												new Object[] {dep.getName(), params.size(), 1});
		}
		XipNode node = params.firstElement();
		annot.setBegin((int) node.getLeftChar());
		annot.setEnd((int) node.getRightChar());
		annot.addToIndexes();
	}

	@Override
	protected void processToken(XipNode xipToken, JCas cas) {
		Token uimaToken = new Token(cas, (int) xipToken.getLeftChar(), (int) xipToken.getRightChar());
		Lemma[] uimaLemmas = new Lemma[maxLemmas];
		
		Vector<XipNode> xipReadings = xipToken.getDaughters();
		int lemmaCount = 0;
		for (XipNode xipReading : xipReadings) {
			Lemma uimaLemma = new Lemma(cas);
			uimaLemma.setLemma(xipReading.getLemma());
			uimaLemma.setPOS(xipReading.getCategory());

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
			x.getPOS() == y.getPOS(); 
	}
	

}
