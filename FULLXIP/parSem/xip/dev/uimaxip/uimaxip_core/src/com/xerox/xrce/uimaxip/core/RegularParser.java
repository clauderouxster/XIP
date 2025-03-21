package com.xerox.xrce.uimaxip.core;

import java.util.Hashtable;
import java.util.Vector;

import jcoref.Referent;
import jcoref.ReferentNode;
import jcoref.ReferentSet;

import com.xerox.jxip.Const;
import com.xerox.jxip.JXip;
import com.xerox.jxip.XipResult;
import com.xerox.jxip.XipResultManager;
import com.xerox.jxip.XipUnit;

public class RegularParser implements Parser {
	
	private XipResultManager xiprm;
	private int ghandler;
	private Vector<XipUnit> xipUnits = null;
	
	public static String stringsResourceName = UimaXip_Base.class.getName()+ "Strings";

	public RegularParser(String grmFilePath) throws Exception {
	        JXip jxip = UimaXip_Base.getJXip(); 
		xiprm = new XipResultManager();
		ghandler = jxip.XipGrmFile(grmFilePath, 0, true);
		jxip.XipAddFlagDisplay(ghandler, Const.UTF8_INPUT, 80);
	}

	@Override
	public ReferentSet getReferents() {
		throw new UnsupportedOperationException();
	}

	@Override
	public Vector<XipUnit> getXipUnits() {
		return xipUnits;
	}

	@Override
	public void parseString(String s) throws Exception {
		int res;
		res = xiprm.buildXipResult(ghandler, s);
		if (res != 0) {
			throw new Exception("buildXipResult failed");
		}
		XipResult xipres = xiprm.getXipResult();
		xipUnits = xipres.getXipUnits();
	}

//		@Override
//	public void parseTextFile(String s) throws Exception {
//		int res;
//		res = xiprm.buildXipResultFromFile(ghandler, s);
//		if (res != 0) {
//			throw new Exception("buildXipResult failed");
//		}
//		XipResult xipres = xiprm.getXipResult();
//		xipUnits = xipres.getXipUnits();
//	}
//
//	@Override
//	public void parseXmlFile(String s) throws Exception {
//		int res;
//		res = xiprm.buildXipResultFromXmlFile(ghandler, s);
//		if (res != 0) {
//			throw new Exception("buildXipResult failed");
//		}
//		XipResult xipres = xiprm.getXipResult();
//		xipUnits = xipres.getXipUnits();
//	}
//	
//	@Override
//	public Hashtable<Long,Long> getBeginTextBlockOfSentence() {
//		return null;
//	}


}
