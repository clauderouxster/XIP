package com.xerox.xrce.uimaxip.infomagic;

import com.xerox.jxip.*;

import java.util.*;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * 
 * @author mehrmann Septembre 2008
 */
public class DurationNormalization extends DateNormalization {

	String durationNum;
	String durationType;
	String normalizedDuration;

	/**
	 * Build a normalized representation for lexical temporal expressions
	 * founded in DUREE dependency. The normalized format is PXXX where - the
	 * numbers are the time duration - the letters are the type of duration
	 * (S,MIN,H,D,W,,M,Y)
	 * 
	 * @param argument
	 *            (xipnode) of a dependency (normally a duration dependency with
	 *            one arg).
	 * @return the normalized duration string.
	 */
	public String normalizeDuration(XipNode nodeofDurationDependency) {

		nodesToTreat = getNodesToTreat(nodeofDurationDependency);

		for (XipNode node : nodesToTreat) {
			getDurationInfo(node, DicoInfo);
		}

		if (DicoInfo.get("typeDuree") != null) {
			durationType = (String) DicoInfo.get("typeDuree");
		} else {
			durationType = "X";
		}
		if (DicoInfo.get("tempsDuree") != null) {
			durationNum = (String) DicoInfo.get("tempsDuree");
		} else {
			durationNum = "XX";
		}

		normalizedDate = "P" + durationNum + durationType;
		return normalizedDate;

	}

	/**
	 * Gets the information associated with the nodes of a duration dependency
	 * and stores it in a hashtable.
	 * 
	 * @param xipnode
	 * @param hashtable
	 * @return hashtable
	 */
	public Hashtable getDurationInfo(XipNode xipnode, Hashtable DicoInfo) {
		/* Recuperation info temps d'une duree */
		if (xipnode.hasFeature("num", "+")) {
			DicoInfo.put("tempsDuree", xipnode.getLemma());

		} /* Recuperation info type d'une duree */else if (xipnode
				.hasFeature("val_duree")) {
			DicoInfo.put("typeDuree", xipnode.getFeature("val_duree"));

		}

		return DicoInfo;
	}
}
