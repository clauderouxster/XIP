package com.xerox.xrce.uimaxip.infomagic;

import com.xerox.jxip.*;

import java.util.*;
import java.util.regex.Pattern;

/**
 * 
 * @author mehrmann Aout 2008
 * 
 */
public class TimeNormalization extends DateNormalization {

	String normalizedTime;
	String normalizedHour;
	String normalizedMinute;
	boolean testTermNode;

	/**
	 * Build a normalized representation for lexical temporal expressions
	 * founded in HEURE dependency. The normalized format is T00:00.
	 * 
	 * @param argument
	 *            (xipnode) of a dependency (normally a "hour" dependency with
	 *            one arg).
	 * @return the normalized hour string.
	 */
	public String normalizeTime(XipNode nodeofTimeDependency) {

		nodesToTreat = getNodesToTreat(nodeofTimeDependency);

		for (XipNode node : nodesToTreat) {
			getTimeInfo(node, DicoInfo);
		}

		if (DicoInfo.get("heure") != null) {
			normalizedHour = (String) DicoInfo.get("heure");
		}
		if (DicoInfo.get("minute") != null) {
			normalizedMinute = (String) DicoInfo.get("minute");
		}

		normalizedTime = "T" + normalizedHour + ":" + normalizedMinute;
		return (normalizedTime);
	}

	/**
	 * Gets the information associated with the nodes of a time dependency and
	 * stores it in a hashtable.
	 * 
	 * @param xipnode
	 * @param hashtable
	 * @return hashtable
	 */
	public Hashtable getTimeInfo(XipNode xipnode, Hashtable DicoInfo) {
		/* Recuperation info sur Heure */
		if (xipnode.hasFeature("numHeure", "+")) {
			String lemNumHeure = xipnode.getLemma();

			/*
			 * Test sur lemNumHeure Si \d alors on met un zero devant le \d
			 */
			boolean matcher;
			matcher = Pattern.matches("[0-9]{1}", lemNumHeure);

			if (matcher) {
				DicoInfo.put("heure", "0" + lemNumHeure);
			}
			if (!matcher) {
				DicoInfo.put("heure", lemNumHeure);
			}

			/* Recuperation info sur Minute */
		} else if (xipnode.hasFeature("numMinute", "+")) {

			String lemNumMinute = xipnode.getLemma();

			/*
			 * Test sur lemNumMinute : Si \d alors on met un zero devant le \d
			 */
			boolean matcher;
			matcher = Pattern.matches("[0-9]{1}", lemNumMinute);

			if (matcher) {
				DicoInfo.put("minute", "0" + lemNumMinute);
			}
			if (!matcher) {
				DicoInfo.put("minute", lemNumMinute);
			}

		}
		return DicoInfo;
	}
}
