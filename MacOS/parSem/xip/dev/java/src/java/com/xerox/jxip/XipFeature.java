/*
 *   Xerox Research Centre Europe - Grenoble Laboratory
 *           project XIP (Xerox Incremental Parser)
 *  Copyright (C) 2004 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved
 *
 */
package com.xerox.jxip;

/**
 * XipFeature Class
 *
 * @author grondeau @created May 19, 2005
 */
public class XipFeature {

    /**
     * feature name
     */
    private String name;
    /**
     * feature value
     */
    private String value;
    /**
     * index in the C++ Vector of XipFeature for a node
     */
    private long index;

    /**
     * Constructor for the XipFeature object
     *
     * @param name feature name
     * @param value feature value
     * @param index
     */
    public XipFeature(String name, String value, long index) {
        //System.out.println("XipFeature::XipFeature() name = " + nam
        // + " value = " + val + " index = " + ind);
        this.name = JXip.duplicateString(name);
        this.value = JXip.duplicateString(value);
        this.index = index;
    }

    /**
     * Gets the feature name
     *
     * @return The feature name
     */
    public synchronized String getName() {
        return name;
    }

    /**
     * Gets the feature value
     *
     * @return The feature value
     */
    public synchronized String getValue() {
        return value;
    }

    /**
     * Gets the C++ index
     *
     * @return The index value
     */
    public synchronized long getIndex() {
        return index;
    }

    /**
     * return the Feature name
     *
     * @return the Feature name
     */
    @Override
    public synchronized String toString() {
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append(name);
        stringBuilder.append(":");
        stringBuilder.append(value);
        return stringBuilder.toString();
    }

    /**
     * return XipFeature data as a string
     *
     * @param withFeature
     * @return XipFeature to a string
     */
    public synchronized String toString(boolean withFeature) {
        if (withFeature) {
            StringBuilder stringBuilder = new StringBuilder();
            stringBuilder.append(name);
            if (!value.equals("+")) {
                stringBuilder.append("=");
                stringBuilder.append(value);
            }
            return stringBuilder.toString();
        } else {
            return "";
        }
    }
}
