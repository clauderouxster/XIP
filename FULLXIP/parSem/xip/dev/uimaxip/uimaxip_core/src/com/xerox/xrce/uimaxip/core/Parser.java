package com.xerox.xrce.uimaxip.core;

import java.util.Hashtable;
import java.util.Vector;

import jcoref.Referent;
import jcoref.ReferentNode;
import jcoref.ReferentSet;

import com.xerox.jxip.XipUnit;

public interface Parser {
	
	void parseString(String s) throws Exception;
//	void parseTextFile(String s) throws Exception;
//	void parseXmlFile(String s) throws Exception;
//	Hashtable<Long,Long> getBeginTextBlockOfSentence();
	Vector<XipUnit> getXipUnits();
	ReferentSet getReferents();
}
