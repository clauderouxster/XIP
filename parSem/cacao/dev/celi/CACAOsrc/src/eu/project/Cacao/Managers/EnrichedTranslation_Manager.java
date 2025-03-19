package eu.project.Cacao.Managers;

import java.util.Date;


/**
 * This class should generate candidate translations by analyzing user logs from multilingual libraries
 * and searching for certain pattern of requests (consecutive requests in different languages..)
 *
 * Manual translations can be manually added too..
 *
 * -Not useful for tel@CLEF
 *
 * @author Alessio
 *
 */
public class EnrichedTranslation_Manager {
    public boolean processLogs(String libraryID, Date from, Date to) {
        return false;
    }

    //method for librarians manual fine tuning
    public boolean addManualTranslation(String srcTerm, String srcLang,
        String dstTerm, String dstLang, String libraryID) {
        return false;
    }

    //method for librarians manual fine tuning
    public boolean setManualTranslation(String srcTerm, String srcLang,
        String[] dstTerms, String dstLang, String libraryID) {
        return false;
    }
}
