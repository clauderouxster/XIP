package it.celi.nutch.searcher;

public interface PlaceIF extends NamedEntityResIF {
	public double getLatitude();
	public double getLongitude();
	public double getMaxProximity();
	public double getProbability();
	public String getShortName();
	public double getProximityLevel(PlaceIF p);
	
	public void setMaxProximity(double v);
	public void setProbability(double p);
	
	public String toString();
	public int compareTo(Object p);
	public void setProbabilityWithPopulation(double probability);

	public int getDensity();

	public String getDepartement() ;

	public String getInsee() ;

	public int getPopulation() ;

	public String getRegion() ;

	public String getZIP();

}


