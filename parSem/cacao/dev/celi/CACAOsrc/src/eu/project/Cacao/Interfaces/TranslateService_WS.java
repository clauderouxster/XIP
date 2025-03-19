package eu.project.Cacao.Interfaces;

import java.util.HashMap;


/**
 * This interface should be implemented by all the WS providing translation services to Cacao.
 *
 * @author Alessio
 *
 */
public interface TranslateService_WS {
    //each original term has multiple possible translation candidates that should be disambiguated... 
    public abstract HashMap<String, String[]> translateText(String text,
        String langFROM, String langTO);

    public abstract boolean supportsMultiWords();

    public abstract boolean supportsIOMappings();

    public abstract boolean supportsMorphoAnalysis();
}
