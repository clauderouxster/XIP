package eu.project.Cacao.Interfaces;


/**
 * This interface should be implemented by all the WS providing Thesauri services to Cacao.
 *
 * @author Alessio
 *
 */
public interface Thesauri_WS {
    public abstract String[] getLexicalThesauriResource(String term,
        String lang, String relType);

    public abstract String[] getOntoThesauriResource(String term, String lang,
        int maxHopDistance);
}
