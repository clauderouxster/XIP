package extui.UI_components;

import java.io.Serializable;
import org.apache.log4j.Logger;

/**
 *
 * @author ctrojahn
 */
public class Query implements Serializable {

    private static final Logger logger = Logger.getLogger(Query.class.getName());
    private String query;
    private String srcLang;
    private String[] targetLang;
    boolean systran;
    boolean SVexp;
    boolean WNexp;
    boolean W2Cexp;

    /**
     *
     */
    public Query() {
        query = null;
        srcLang = null;
        targetLang = null;
        systran = false;
        SVexp = false;
        WNexp = false;
        W2Cexp = false;
    }

    /**
     *
     * @param query
     * @param srcLang
     * @param targetLang
     * @param systran
     * @param SVexp
     * @param WNexp
     * @param W2Cexp
     */
    public Query(String query, String srcLang, String[] targetLang,
            boolean systran, boolean SVexp, boolean WNexp, boolean W2Cexp) {
        this.query = query;
        this.srcLang = srcLang;
        setTargetLang(targetLang);
        this.systran = systran;
        this.SVexp = SVexp;
        this.WNexp = WNexp;
        this.W2Cexp = W2Cexp;
    }

    /**
     *
     * @return
     */
    public String getQuery() {
        return query;
    }

    /**
     *
     * @return
     */
    public String getSrcLang() {
        return srcLang;
    }

    /**
     *
     * @return
     */
    public String[] getTargetLang() {
        return targetLang;
    }

    /**
     *
     * @return
     */
    public boolean systran() {
        return systran;
    }

    /**
     *
     * @return
     */
    public boolean SVexp() {
        return SVexp;
    }

    /**
     *
     * @return
     */
    public boolean W2Cexp() {
        return W2Cexp;
    }

    /**
     *
     * @return
     */
    public boolean WNexp() {
        return WNexp;
    }

    /**
     *
     * @param query
     */
    public void setQuery(String query) {
        this.query = query;
    }

    /**
     *
     * @param srcLang
     */
    public void setSrcLang(String srcLang) {
        this.srcLang = srcLang;
    }

    /**
     *
     * @param target
     */
    public void setTargetLang(String[] target) {
        this.targetLang = new String[target.length];
        for (int x = 0; x < target.length; x++) {
            this.targetLang[x] = target[x];
        }
    }

    /**
     *
     * @param systran
     */
    public void setSystran(boolean systran) {
        this.systran = systran;
    }

    /**
     *
     * @param SVexp
     */
    public void setSVexp(boolean SVexp) {
        this.SVexp = SVexp;
    }

    /**
     *
     * @param WNexp
     */
    public void setWNexp(boolean WNexp) {
        this.WNexp = WNexp;

    }

    /**
     *
     * @param W2Cexp
     */
    public void setW2Cexp(boolean W2Cexp) {
        this.W2Cexp = W2Cexp;
    }
}

