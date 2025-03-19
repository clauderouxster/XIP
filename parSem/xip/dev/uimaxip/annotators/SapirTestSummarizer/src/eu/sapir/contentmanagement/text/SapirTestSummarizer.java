// Copyright (C) 2007 Xerox Corporation, XRCE, All Rights Reserved.

package eu.sapir.contentmanagement.text;
import java.util.Vector;

import org.apache.uima.analysis_engine.AnalysisEngineProcessException;
import org.apache.uima.jcas.JCas;
import org.apache.uima.jcas.cas.FSArray;

import com.xerox.jxip.XipDependency;
import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipUnit;

import com.xerox.xrce.uimaxip.core.UimaXip_Base;

import eu.sapir.contentmanagement.uimatypes.text.Lemma;
import eu.sapir.contentmanagement.uimatypes.text.Sentence;
import eu.sapir.contentmanagement.uimatypes.text.Summary;
import eu.sapir.contentmanagement.uimatypes.text.Token;


public class SapirTestSummarizer extends UimaXip_Base {
	
	private int maxLemmas = 128;
	protected int summarySize = 4;
	protected Sentence[] summSentences;
	
	@Override
	protected void startDocument(JCas cas){
		summSentences = new Sentence[summarySize];		
	}

	protected void endDocument(JCas cas) {
		// Add the summary to the annotations index
		FSArray summSentencesFSArray = new FSArray(cas,summarySize);
		summSentencesFSArray.copyFromArray(summSentences,0,0,summarySize);
		Summary summ = new Summary(cas);
		summ.setSummSentences(summSentencesFSArray);
		summ.addToIndexes();
	}
	
	
	protected void processXipUnit(XipUnit unit, JCas cas, int offset) throws AnalysisEngineProcessException {

    	Sentence aSentence = initializeSentence(unit, cas, offset);

    	// Add to summary array
		if (aSentence.getSentenceNumber() <= summarySize){
			summSentences[(int)aSentence.getSentenceNumber()-1] = aSentence;
		}

        for (XipDependency dep : unit.getDependencies()) {
			processDependency(dep,cas,offset);
		}

		for (XipNode xipToken : unit.getLeafNodes()) {
			processToken(xipToken, cas, offset);		
		}


	}

    protected Sentence initializeSentence(XipUnit unit, JCas cas, int offset) {
    	Sentence aSentence = new Sentence(cas);
    	aSentence.setText(unit.getSentenceString());
    	aSentence.setSentenceNumber(unit.getSentenceNb());
        aSentence.setBegin((int)unit.getLeaves().firstElement().getLeftChar()+offset);
        aSentence.setEnd((int)unit.getLeaves().lastElement().getRightChar()+offset);
    	aSentence.addToIndexes();
    	return aSentence;
	}

	
	@Override
	protected void processToken(XipNode xipToken, JCas cas, int offset) {
		Token uimaToken = new Token(cas, (int) xipToken.getLeftChar()+offset, (int) xipToken.getRightChar()+offset);
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
