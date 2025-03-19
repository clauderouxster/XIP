package eu.project.Cacao.Components.Utils;

import java.util.Vector;

public interface Lemmatizer {
	public Vector<NLPtoken> parseTerm(String text);
	public Vector<NLPtoken> parsePhrase(String text);
}
