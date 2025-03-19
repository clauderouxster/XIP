package extui.UI_components.tagcloud;

import java.io.Serializable;
import org.apache.log4j.Logger;

/**
 *
 * @author ctrojahn
 */
public class Tag implements Serializable {

    private static final Logger logger = Logger.getLogger(Tag.class.getName());
    private String name;
    private long freq;
    private TagCloud tagCloud;

    /**
     *
     */
    public Tag(TagCloud tagCloud) {
           this.tagCloud = tagCloud;
    }

    /**
     *
     * @param name
     * @param freq
     */
    public Tag(String name, long freq, TagCloud tagCloud) {
        this.name = name;
        this.freq = freq;
        this.tagCloud = tagCloud;
    }

    /**
     *
     * @return
     */
    public String getName() {
        return name;
    }

    /**
     *
     * @return
     */
    public long getFreq() {
        long f = ((100 * freq) / tagCloud.getTotTags());
        return f;
    }

    /**
     * @param name
     */
    public void setName(String name) {
        this.name = name;
    }

    /**
     *
     * @param freq
     */
    public void setFreq(long freq) {
        this.freq = freq;
    }

    @Override
    public String toString() {
        return name;
    }

    //fontColorR = fontColorRMin + num.round((fontColorRMax - fontColorRMin) * weight);
    //fontColorG = fontColorGMin + num.round((fontColorGMax - fontColorGMin) * weight);
    //fontColorB = fontColorBMin + num.round((fontColorBMax - fontColorBMin) * weight);
}
