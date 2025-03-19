/*
 * Xerox Research Center Europe
 * Copyright (C) 2009 XRCE, All Rights Reserved
 */
package extui.UI_components;

import java.util.ArrayList;
import java.util.List;

/**
 *
 * @author ctrojahn
 */
public class Facet {

    private int id;
    private static int inc;
    private String nameUI;
    private String nameSolr;
    private List<FacetValue> values;
    private int sizeList;

    public Facet() {
    }

    /**
     * @param nameSolr
     * @param nameUI 
     */
    public Facet(String nameSolr,String nameUI) {
        this.nameSolr = nameSolr;
        this.nameUI = nameUI;
        inc++;
        this.id = inc;
        this.sizeList = 5;
    }

     public Facet(String nameSolr) {
        this.nameSolr = nameSolr;
        inc++;
        this.id = inc;
     }


    /**
     *
     * @return
     */
    public String getNameUI() {
        return this.nameUI;
    }

    /**
     * @param nameUI
     */
    public void setNameUI(String nameUI) {
        this.nameUI = nameUI;
    }

    /**
     * @return
     */
    public String getNameSolr() {
        return this.nameSolr;
    }

    /**
     * @param nameSolr
     */
    public void setNameSolr(String nameSolr) {
        this.nameSolr = nameSolr;
    }

    public void setValues(List<FacetValue> list) {
        this.values = list;
    }

    public List<FacetValue> getValues(int size) {
        List<FacetValue> l = new ArrayList<FacetValue>();
        if (size > values.size()) {
            size = values.size();
        }
        for(int x=0;x<size;x++) {
            l.add(values.get(x));
        }

        return l;
    }

    public int getId() {
        return id;
    }

    public int getSizeList() {
        return this.sizeList;
    }

    public int getSizeListOriginal() {
        return this.values.size();
    }


    public void setSizeList(int i) {
        this.sizeList = i;
    }

}
