package it.celi.nutch.searcher;

import java.util.Comparator;
import java.io.Serializable;


/**
 * PlaceIF Comparator
 */
public class PlaceProbabilityComparator 
implements Comparator<PlaceIF>, Serializable
{
	private static final long serialVersionUID = 1L;

	public int compare(PlaceIF a, PlaceIF b) {
		return a.compareTo(b);
		/*
		double p1, p2;
		p1 = a.getProbability();
		p2 = b.getProbability();
		if (p1 < p2) {
			return 1;
		}
		if (p1 > p2) {
			return -1;
		}
		return 0;
		*/
	}
	
	public boolean equals(PlaceIF o) {
		return (o instanceof PlaceProbabilityComparator);
	}
}