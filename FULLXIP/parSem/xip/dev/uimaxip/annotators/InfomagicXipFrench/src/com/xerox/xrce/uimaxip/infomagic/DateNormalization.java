package com.xerox.xrce.uimaxip.infomagic;

import com.xerox.jxip.*;
import java.util.*;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.regex.Pattern;

/**
 * 
 * Build a local representation of lexical temporal entries (a year, a month, a
 * day)
 * 
 */
public class DateNormalization {

	String normalizedDate;
	String normalizedYear;
	String normalizedMonth;
	String normalizedDay;
	String normalizedSeason;
	boolean shortformat;
	boolean testTermNode;
	boolean shortyear;
	Hashtable DicoInfo;
	Vector<XipNode> nodesToTreat;
	Vector<XipNode> myVector;

	/**
	 * Constructor for the DateNormalization object.
	 * 
	 * 
	 */
	public DateNormalization() {
		DicoInfo = new Hashtable();

	}

	/**
	 * Return a vector a preterminal nodes associated with a node.
	 * 
	 * @param xipnode
	 * @return vector
	 */
	public Vector<XipNode> getNodesToTreat(XipNode xipnode) {
		myVector = new Vector<XipNode>();
		testTermNode = xipnode.isPreTerminalNode();
		if (testTermNode) {
			myVector.addElement(xipnode);

		} else {
			try {
				myVector = xipnode.getDescendantsByBooleanMethod(DateNormalization.class.getName(), "testifTermNodes");
			} catch (Exception ex) {
				Logger.getLogger(TimeNormalization.class.getName()).log(
						Level.SEVERE, null, ex);
			}
		}
		return myVector;
	}

	/**
	 * Build a normalized representation for lexical temporal expressions
	 * founded in DATE dependency. The normalized format is +yyyy-mm-dd.
	 * 
	 * @param argument
	 *            (xipnode) of a dependency (normally a date dependency with one
	 *            arg).
	 * @return the normalized date string.
	 * 
	 */
	public String normalizeDate(XipNode nodeofDateDependency) {

		nodesToTreat = getNodesToTreat(nodeofDateDependency);

		for (XipNode node : nodesToTreat) {
			getDateInfo(node, DicoInfo);
		}

		/* Recuperation infos du traitement et instanciation de la variable norm */
		if (DicoInfo.get("annee") != null) {
			normalizedYear = (String) DicoInfo.get("annee");
		} else {
			normalizedYear = "XXXX";
		}
		if (DicoInfo.get("mois") != null) {
			normalizedMonth = (String) DicoInfo.get("mois");
		} else {
			normalizedMonth = "XX";
		}
		if (DicoInfo.get("jour") != null) {
			normalizedDay = (String) DicoInfo.get("jour");
		} else {
			normalizedDay = "XX";
		}
		if (DicoInfo.get("saison") != null) {
			normalizedSeason = (String) DicoInfo.get("saison");
		}

		if (!shortformat) {
			normalizedDate = "+" + normalizedYear + "-" + normalizedMonth + "-"
					+ normalizedDay;
		} else if (shortformat) {
			normalizedDate = "+" + normalizedYear + "-" + normalizedSeason;

		}

		return normalizedDate;
	}

	/**
	 * Test if a node is a preterminal node
	 * 
	 * @param xipnode
	 * @return true if the node is a preterminal node, false otherwise
	 * @see isPreterminalNode
	 */
	public static boolean testifTermNodes(XipNode xipnode) {
		if (xipnode.isPreTerminalNode()) {
			return true;
		}
		return false;
	}

	/**
	 * Gets the information associated with the nodes of a date dependency and
	 * stores it in a hashtable.
	 * 
	 * @param xipnode
	 * @param hashtable
	 * @return hashtable
	 */
	public Hashtable getDateInfo(XipNode xipnode, Hashtable DicoInfo) {

		boolean shortyear = xipnode.hasFeature("shortyear");
		

		/* Recuperation info sur Annee */
		if ( xipnode.hasFeature("annee", "+") && shortyear != true ) {
		/*if (xipnode.hasFeature("annee", "+")) {*/
			String lemAnnee = xipnode.getLemma();
			DicoInfo.put("annee", lemAnnee);

		} else if (xipnode.hasFeature("annee", "+") && shortyear != false ){
			String lemAnnee = xipnode.getLemma();
			DicoInfo.put("annee", "XX" + lemAnnee);



			/* Recuperation info sur mois */
		} else if (xipnode.hasFeature("mois", "+")) {
			String lemMois = xipnode.getLemma();
			/*
			 * Si le mois est un chiffre, récupérer le lemme, sinon la valeur du
			 * trait "mois".
			 */
			boolean matcher;
			matcher = Pattern.matches("[0-9]{1,2}", lemMois);
			if (matcher) {
				DicoInfo.put("mois", lemMois);
			}
			if (!matcher) {
				DicoInfo.put("mois", xipnode.getFeature("mois"));
			}

			/* Recuperation info sur Jour */
		} else if (xipnode.hasFeature("numjourpot", "+")) {
			String lemNumjour = xipnode.getLemma();

			/*
			 * Pour mettre un 0 devant le numéro de mois s'il ne comporte qu'un
			 * seul chiffre
			 */
			boolean matcher;
			matcher = Pattern.matches("[0-9]{1}", lemNumjour);
			if (matcher) {
				DicoInfo.put("jour", "0" + lemNumjour);
			}
			if (!matcher) {
				DicoInfo.put("jour", lemNumjour);
			}

		} /* Recuperation info saison */else if (xipnode.hasFeature("saison")) {
			DicoInfo.put("saison", xipnode.getFeature("saison"));
			shortformat = true;

		}

		return DicoInfo;
	}
}

