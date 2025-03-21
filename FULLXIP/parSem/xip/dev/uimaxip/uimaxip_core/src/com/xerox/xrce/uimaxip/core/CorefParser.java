package com.xerox.xrce.uimaxip.core;

import java.util.Hashtable;
import java.util.Vector;

import com.xerox.jxip.JXip;
import com.xerox.jxip.XipUnit;

import jcoref.CorefResolver;
import jcoref.ReferentSet;
import jcoref.SentenceParse;

public class CorefParser implements Parser {

	private CorefResolver resolver;
	private ReferentSet referents;
	
	public CorefParser(String grmFilePath) throws Exception {
	        JXip jxip = UimaXip_Base.getJXip();
	        int handler = jxip.XipGrmFile(grmFilePath, 0, true);
	        resolver = new CorefResolver(jxip, handler);
	}
	
	@Override
	public ReferentSet getReferents() {
		return referents;
	}

	@Override
	public Vector<XipUnit> getXipUnits() {
		Vector<SentenceParse> parses = resolver.getSentenceParses();
		Vector<XipUnit> result = new Vector<XipUnit>(parses.size());
		for (SentenceParse parse : parses) {
			result.add(parse.getXipUnit());
		}
		return result;
	}

	@Override
	public void parseString(String s) {
		referents = resolver.resolveCoreferenceInString(s, true);
	}

//	@Override
//	public void parseTextFile(String s) {
//		referents = resolver.resolveCoreferenceInFile(s, true, false);
//	}
//
//	@Override
//	public void parseXmlFile(String s) {
//		referents = resolver.resolveCoreferenceInFile(s, true, true);
//	}

//	@Override
//	public Hashtable<Long,Long> getBeginTextBlockOfSentence() {
//		return resolver.getBeginTextBlockOfSentence();
//	}

}
