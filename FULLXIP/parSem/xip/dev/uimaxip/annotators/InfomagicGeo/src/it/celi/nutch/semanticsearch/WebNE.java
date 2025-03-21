package it.celi.nutch.semanticsearch;

public interface WebNE {
	public static final String  CONTENT_FIELD = "c";
	public static final String  POSITION_FIELD="position";
	public String getChoiceAppearString();
	public String getChoiceValString();
	public boolean isNE();
	public String getType();


}
