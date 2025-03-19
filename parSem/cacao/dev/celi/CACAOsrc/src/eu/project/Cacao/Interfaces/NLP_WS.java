package eu.project.Cacao.Interfaces;

/**
 * This interface should be implemented by all the WS providing NLP services to Cacao.
 *
 * The web service should work in a UIMA framework and CACAO components expect the output of the WS
 * as a CAS object. CAS is the subsystem in the Unstructured Information Management Architecture (UIMA)
 * that handles data exchanges between the various UIMA components.
 *
 * In order to increase the efficiency of transferring the CAS object over the network a proper consumer/serialization
 * could be used. In that case an additional component (i.e. CacaoCAS_Serializer) will be internally designed
 * and provided to the consortium of NLP service providers.
 *
 *
 * Each of the exposed operation performs a different analysis on the input text.
 * @author Alessio
 *
 */
public interface NLP_WS {
    /* informs about the supported language
     * ?? Should each NLP service deals with only one language ??
     * if so maybe whe should remove the lang param from others methods*/
    public String getSupportedLanguage();

    /* analyzes the text and lemmatize it */
    public String parsePhrase(String text, String lang);

    /* analyzes the text and extract Named Entities */
    public String parseTerm(String text, String lang);

}
