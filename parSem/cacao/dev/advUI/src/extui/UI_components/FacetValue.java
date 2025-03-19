/*
 * Xerox Research Center Europe
 * Copyright (C) 2009 XRCE, All Rights Reserved
 */
package extui.UI_components;

/**
 *
 * @author ctrojahn
 */
public class FacetValue implements Comparable{

    private Facet facet;
    private String value;
    private boolean selected;
    private long freq;
    private String nameAndValue;

    /**
     *
     */
    public FacetValue() {
    }

    /**
     * @param value
     * @param freq
     * @param selected 
     */
    public FacetValue(String value, long freq, boolean selected) {
        this.value = value;
        this.selected = selected;
        this.freq = freq;
    }

    public FacetValue(String value) {
        this.value = value;
        this.selected = true;
        this.freq = 0;
    }

    /**
     * @return
     */
    public String getValue() {
        return value;
    }

    /**
     *
     * @param value
     */
    public void setValue(String value) {
        this.value = value;
    }

    /**
     *
     * @return
     */
    public boolean getSelected() {
        return this.selected;
    }

    /**
     * @param selected
     */
    public void setSelected(boolean selected) {
        this.selected = selected;
    }

    public String toString() {
        return facet.getNameSolr() + " " + value;
        //return value;
    }

    /**
     *
     * @param facet
     */
    public void setFacet(Facet facet) {
        this.facet = facet;
    }

    /**
     *
     * @return
     */
    public Facet getFacet() {
        return facet;
    }

    public String getFreq() {
        return "(" + freq + ")";
    }

    public void setFreq(long freq) {
        this.freq = freq;
    }
    
    public String getNameAndValue() {
           return facet.getNameSolr() + ";" + value;
    }

    public int compareTo(Object o) {
        FacetValue comp = (FacetValue) o;
        return this.value.compareTo(comp.getValue());
    }
}
