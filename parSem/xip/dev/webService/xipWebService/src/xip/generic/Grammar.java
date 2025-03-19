package xip.generic;

import com.xerox.jxip.JXip;
import org.apache.log4j.Logger;

public class Grammar {

    private String grmFile;
    private JXip jXip;
    private int gHandler;
    private static final Logger logger = Logger.getLogger(Grammar.class.getName());

    public Grammar(JXip jXip) {
        this.jXip = jXip;
    }

    public void loadGrammar() {
        try {
            logger.info("loading grammar file: " + grmFile);
            gHandler = jXip.XipGrmFile(grmFile, 0, true);
        } catch (Exception ex) {
            logger.fatal(ex);
            System.exit(-1);
        }
    }

    /**
     *
     * @param grmFile
     */
    public void setGrm(String grmFile) {
        this.grmFile = grmFile;
    }

    public int getGHandler() {
        return gHandler;
    }

    String getGrm() {
        return grmFile;
    }
}
