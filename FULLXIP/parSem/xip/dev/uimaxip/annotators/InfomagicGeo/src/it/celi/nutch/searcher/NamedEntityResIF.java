package it.celi.nutch.searcher;

public interface NamedEntityResIF {
	public String getSearchName();

	public String getName();
	public String getNormalizedName();
	/** either on doc or on index. **/
	public long getOccurrences();
	public String getType();
	public String toHtml();

}
