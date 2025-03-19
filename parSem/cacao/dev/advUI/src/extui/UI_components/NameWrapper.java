package extui.UI_components;

import org.apache.log4j.Logger;
import org.apache.wicket.IClusterable;

/**
 *
 * @author ctrojahn
 */
public class NameWrapper implements IClusterable {

    private static final Logger logger = Logger.getLogger(NameWrapper.class.getName());
    private String name;
    private String shortName;
    private String hit;
    private boolean selected;

    public NameWrapper() {
    }


    public NameWrapper(String name) {
        this.name = name;
    }



    /**
     *
     * @param name
     * @param shortName
     * @param hit
     * @param selec
     */
    public NameWrapper(String name, String shortName, String hit, boolean selec) {
        this.name = name;
        this.shortName = shortName;
        this.hit = hit;
        this.selected = selec;
    }

    public NameWrapper(String name, String shortName, String hit) {
        this.name = name;
        this.shortName = shortName;
        this.hit = hit;
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
    public String getShortName() {
        return shortName;
    }

    /**
     *
     * @param name
     */
    public void setName(String name) {
        this.name = name;
    }

    /**
     * 
     * @return
     */
    public String getHit() {
        return hit;
    }

    /**
     *
     * @return
     */
    public boolean getSelected() {
        return selected;
    }

    @Override
    public String toString() {
        return shortName;
    }

    public void setSelected(boolean selected) {
        this.selected = selected;
    }

  

}

