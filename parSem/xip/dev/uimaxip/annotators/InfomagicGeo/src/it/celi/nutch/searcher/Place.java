package it.celi.nutch.searcher;

//import java.util.Map;
//import java.util.HashMap;
import it.celi.nutch.semanticsearch.WebNE;

import java.lang.Math;
import java.lang.Comparable;

public class Place implements PlaceIF, Comparable {
	private String name;

	private String normalizedName;

	private String shortName;

	private String type; // città, località, regione, stato, ...

	// private int regionCode;
	private long occurrences;

	private double longitude;

	private double latitude;

	private double probability = 0.0;

	private double maxProximity = 3.0;

	private int population;

	private int density;

	private String departement;

	private String region;

	private String insee;

	private String ZIP;

	public static final int MUNICIPALITY_TYPE = 0;

	public static final int DEPARTEMENT_TYPE = 1;

	public static final int REGION_TYPE = 2;

	public static final int GNS_TYPE = 3;

	/*
	 * private static Map geoType = new HashMap(9); geoType.put("A",
	 * "Administrative region"); geoType.put("P", "Populated place");
	 * geoType.put("V", "Vegetation"); geoType.put("L", "Locality or area");
	 * geoType.put("U", "Undersea"); geoType.put("R", "Streets, highways, roads,
	 * or railroad"); geoType.put("T", "Hypsographic"); geoType.put("H",
	 * "Hydrographic"); geoType.put("S", "Spot");
	 */

	public Place(String nameV, String normNameV, String typeV, long occ) {
		name = nameV;
		normalizedName = (null != normNameV) ? normNameV : "";
		type = (null != typeV) ? typeV : "";
		occurrences = occ;
	}

	public Place(String nameV, String normNameV, String typeV, long occ,
			double latitudeV, double longitudeV) {
		this(nameV, normNameV, typeV, occ);
		latitude = latitudeV;
		longitude = longitudeV;
	}

	public Place(String line) {
		String[] sp = line.split("\t");
		if (sp.length < 25) {
			return;
		}
		// regionCode = Integer.valueOf(sp[0]).intValue();
		latitude = Double.valueOf(sp[3]).doubleValue();
		longitude = Double.valueOf(sp[4]).doubleValue();
		shortName = sp[19];
		normalizedName = sp[22];
		type = sp[9];
		occurrences = 0;
	}

	public Place(String line, int typeV) {

		if (typeV != MUNICIPALITY_TYPE) {
			System.out.println("UNCOVERED PLACE TYPE");
			return;
		}

		String[] sp = line.split("\t");

		// regionCode = Integer.valueOf(sp[0]).intValue();
		latitude = Double.valueOf(sp[5]).doubleValue();
		longitude = Double.valueOf(sp[6]).doubleValue();
		shortName = "";
		normalizedName = sp[4];
		type = WebNE.POSITION_FIELD;
		occurrences = 0;
		population = Double.valueOf(sp[7]).intValue();
		density = Double.valueOf(sp[8]).intValue();
		departement = sp[2];
		region = sp[3];
		insee = sp[0];
		ZIP = sp[1];

	}

	public String getName() {
		return this.getNormalizedName() + "(" + this.getZIP() + ")";
	}

	public String getNormalizedName() {
		return normalizedName;
	}

	public String getShortName() {
		return shortName;
	}

	public long getOccurrences() {
		return occurrences;
	}

	public String getType() {
		return type;
	}

	/*
	 * (non-Javadoc) A method for printing identified names with reference to
	 * google maps and unidentified places as a debug string
	 * 
	 * @see it.celi.nutch.searcher.NamedEntityResIF#toHtml()
	 */
	public String toHtml() {
		String nTodispla = (normalizedName != null && !normalizedName
				.equals("")) ? normalizedName : name;
		if (latitude != 0) {
			return "<a class=\"" + occurrences + "\"href=\"../map.jsp?lat="
					+ latitude + "&lon=" + longitude + "&name=" + nTodispla
					+ "\"><i class=\"place\">" + nTodispla + " </i> </a>";
		} else {
			return "<i class=\"place\">" + nTodispla + " </i> ";
		}
	}

	public static String normalize(String string) {
		return string.toLowerCase();
	}

	public double getLatitude() {
		return latitude;
	}

	public double getLongitude() {
		return longitude;
	}

	/*
	 * public String getTypeName() { if (type == null || type == "") { return
	 * ""; } return geoType.get(type); }
	 */

	public double distanceFrom(PlaceIF p) {
		return Math.sqrt(Math.pow(latitude - p.getLatitude(), 2.0)
				+ Math.pow(longitude - p.getLongitude(), (double) 2.0));
	}

	public double getMaxProximity() {
		return maxProximity;
	}

	public void setMaxProximity(double v) {
		maxProximity = v;
	}

	public void setProbability(double p) {
		probability = p;
	}

	public double getProbability() {
		return probability;
	}

	public double getProximityLevel(PlaceIF p) {
		double distance = distanceFrom(p);
		if (distance > maxProximity) {
			return 0;
		}
		if (p.getType().equals("A")) {
			return (1 / (distance * 0.9));
		}
		return (1 / distance);
	}

	public String toString() {
		return normalizedName + "|" + latitude + "|" + longitude + "("
				+ probability + ")";
	}

	public int compareTo(Object p) {
		if (!(p instanceof PlaceIF)) {
			throw new ClassCastException();
		}
		double prob = ((PlaceIF) p).getProbability();
		if (prob < probability) {
			return -1;
		}
		if (prob > probability) {
			return 1;
		}
		return 0;
	}

	public int getDensity() {
		return density;
	}

	public String getDepartement() {
		return departement;
	}

	public String getInsee() {
		return insee;
	}

	public int getPopulation() {
		return population;
	}

	public String getRegion() {
		return region;
	}

	public String getZIP() {
		return ZIP;
	}

	public void setProbabilityWithPopulation(double probabilityV) {
		probability = probabilityV + (population / 4000);
	}

	public String getSearchName() {
		return this.getInsee() + "|n" + this.getLatitude() + "e"
				+ this.getLongitude() + "|" + this.getNormalizedName()
				+ "(" + this.getZIP() + ")" ;
	}
}
