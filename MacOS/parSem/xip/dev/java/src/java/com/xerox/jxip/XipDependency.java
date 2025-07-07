/*
 *   Xerox Research Centre Europe - Grenoble Laboratory
 *           project XIP (Xerox Incremental Parser)
 *  Copyright (C) 2004 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved
 *
 */
package com.xerox.jxip;

import java.util.Enumeration;
import java.util.Vector;

/**
 * XipDependency class
 *
 * @author grondeau @created 22 mars 2006
 */
public class XipDependency {

    /**
     * name of the dependency
     */
    private String name;
    /**
     * Index in the c++ Vector of XipDependency
     */
    private long index;
    /**
     * Vector of XipFeature
     */
    private Vector<XipFeature> features;
    /**
     * vector of XipNode
     */
    private Vector<XipNode> parameters;

    /**
     * Constructor
     *
     * @param name name of the dependency
     * @param index
     */
    public XipDependency(String name, long index) {
        this.name = JXip.duplicateString(name);
        this.index = index;
        this.features = new Vector<XipFeature>();
        this.parameters = new Vector<XipNode>();
    }

    /**
     * Gets the dependency name
     *
     * @return The name
     */
    public synchronized String getName() {
        return name;
    }

    /**
     * Gets the nameAndParamName attribute of the XipDependency object
     *
     * @return The nameAndParamName value
     */
    public synchronized String getNameAndParamName() {
        StringBuilder nameAndParamName = new StringBuilder();
        nameAndParamName.append(name);
        nameAndParamName.append(" ");
        for (Enumeration enumeration = parameters.elements(); enumeration.hasMoreElements();) {
            XipNode xipNode = (XipNode) enumeration.nextElement();
            nameAndParamName.append("<");
            nameAndParamName.append(xipNode.getSurface());
            nameAndParamName.append(">");
        }
        return nameAndParamName.toString();
    }

    /**
     * Gets the node index
     *
     * @return The index value
     */
    public synchronized long getIndex() {
        return index;
    }

    /**
     * Gets the features
     *
     * @return The features: a Vector of XipFeatures
     */
    public synchronized Vector<XipFeature> getFeatures() {
        return features;
    }

    /**
     * Gets the value of the specified feature
     *
     * @param name
     * @return a String value of the specified feature, or null if the feature
     * does not exist
     */
    public synchronized String getFeature(String name) {
        String value = null;
        for (XipFeature feat : this.features) {
            if (feat.getName().equalsIgnoreCase(name)) {
                return feat.getValue();
            }
        }
        return value;
    }

    /**
     * Tests if the dependency has a given feature.
     *
     * @param name the name of the feature
     * @param value the value of the feature ("*" if all values are fine) Value
     * can be negated ("~" or "~value")
     * @return true whether the dependency has the feature with the appropriate
     * value false otherwise
     */
    public synchronized boolean hasFeature(String name, String value) {
        boolean neg = false;
        if (value.startsWith("~")) {
            neg = true;
            value = value.substring(1);
        }
        if (value.equals("")) {
            value = "*";
        }
        for (XipFeature feature : this.features) {
            if (feature.getName().equalsIgnoreCase(name)) {
                if (value.equals("*") || feature.getValue().equalsIgnoreCase(value)) {
                    return !neg;
                }
            }
        }
        return neg;
    }

    /**
     * Tests if the dependency has given features. These features are separated
     * by ",", and the names are separated from values by ":". All is in XIP
     * format
     *
     * @param features the list features in XIP format
     * @return true whether the dependency has the features with the appropriate
     * value false otherwise
     */
    public synchronized boolean hasFeatures(String features) {
        String[] feats = features.split(",");
        if (features.equals("")
                || features.equals("*")
                || (feats.length == 1 && feats[0].compareTo("") == 0)) {
            return true;
        } else {
            String fv;
            String f;
            String v;
            String[] temp;
            for (int i = 0; i < feats.length; i++) {
                fv = feats[i];
                fv = fv.replaceAll("!", "");
                temp = fv.split(":");
                f = temp[0];
                if (temp.length == 2) {
                    v = temp[1];
                } else {
                    v = "+";
                }
                if (!this.hasFeature(f, v)) {
                    return false;
                }
            }
            return true;
        }
    }

    /**
     * Sets the features
     *
     * The features: a Vector of XipFeatures
     *
     * @param feat The new features value
     */
    public synchronized void setFeatures(Vector<XipFeature> feat) {
        features = feat;
    }

    /**
     * Gets the parameters
     *
     * @return The parameters: a Vector of XipNode
     */
    public synchronized Vector<XipNode> getParameters() {
        return parameters;
    }

    /**
     * Gets the Nth node argument of this dependency.
     *
     * @param n The order of the required argument ( n >= 1, argument 1 being
     * the first argument).
     * @return The XipNode that is the n-th argument of this dependency relation
     * if such argument exists; null otherwise.
     */
    public synchronized XipNode getArg(int n) {
        if (n == 0 || getParameters().size() < n) {
            return null;
        } else {
            return getParameters().elementAt(n - 1);
        }
    }

    /**
     * return the dependency name
     *
     * @return the dependency name
     */
    @Override
    public synchronized String toString() {
        return name;
    }

    /**
     * return XipDependency data as a string
     *
     * @param withFeature if true build the string with the dependency features
     * @return XipDependecy to a string
     */
    public synchronized String toString(boolean withFeature) {
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append(name);
        stringBuilder.append("   (");
        stringBuilder.append(getNameAndParamName());
        stringBuilder.append(")\n");
        for (Enumeration enumeration = parameters.elements(); enumeration.hasMoreElements();) {
            XipNode xipNode = (XipNode) enumeration.nextElement();
            stringBuilder.append("    ");
            stringBuilder.append(xipNode.toString(false, "", false));
        }
        stringBuilder.append("    feature = ");
        for (Enumeration enumeration = features.elements(); enumeration.hasMoreElements();) {
            XipFeature feature = (XipFeature) enumeration.nextElement();
            stringBuilder.append(" ");
            stringBuilder.append(feature.toString(withFeature));
        }
        stringBuilder.append("\n");
        return stringBuilder.toString();
    }
}
