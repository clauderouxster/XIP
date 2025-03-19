/*
 * Xerox Research Center Europe
 * Copyright (C) 2009 XRCE, All Rights Reserved
 */

package extui.UI_components;

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
    private int id;
    private static int cont = 0;

    /**
     *
     * @param name
     * @param freq
     */
    public Tag(String name, long freq) {
        this.name = name;
        this.freq = freq;
        cont++;
        id=cont;
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
          return freq;
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

    public int getId() {
        return id;
    }



}