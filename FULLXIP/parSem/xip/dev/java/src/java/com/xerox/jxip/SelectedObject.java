/*
 *   Xerox Research Centre Europe - Grenoble Laboratory
 *           project XIP (Xerox Incremental Parser)
 *  Copyright (C) 2004 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved
 *
 */
package com.xerox.jxip;

import java.util.Vector;

/**
 * A selected node used to generate a xip rule
 *
 * @author grondeau
 */
public class SelectedObject {

    private boolean any;
    // a pointer to a XipNode, a XipLeaf, or a dependency
    private java.lang.Object object;
    // the XipFeature selected with this node
    private Vector<XipFeature> selectedFeatures;

    /**
     * Constructor
     *
     * @param obj a XipNode, XipLeaf, or Xipdependency
     */
    public SelectedObject(java.lang.Object obj) {
        any = false;
        object = obj;
        selectedFeatures = new Vector<XipFeature>();
    }

    /**
     * Gets the object attribute of the SelectedObject object
     *
     * @return The object value
     */
    public synchronized java.lang.Object getObject() {
        return object;
    }

    /**
     * Gets the selected Features
     *
     * @return The selectedFeatures value
     */
    public synchronized Vector<XipFeature> getSelectedFeatures() {
        return selectedFeatures;
    }

    /**
     * Adds a feature to the SelectedFeatures Vector
     *
     * @param feature The feature to be added to the SelectedFeatures Vector
     */
    public synchronized void addSelectedFeature(XipFeature feature) {
        selectedFeatures.add(feature);
    }

    /**
     * Removes a feature to the SelectedFeatures Vector
     *
     * @param feature The feature to be removed to the SelectedFeatures Vector
     */
    public synchronized void removeSelectedFeature(XipFeature feature) {
        selectedFeatures.remove(feature);
    }

    /**
     * Sets the any attribute
     *
     * @param val The any value
     */
    public synchronized void setAny(boolean val) {
        any = val;
    }

    /**
     * Gets the any attribute
     *
     * @return The any value
     */
    public synchronized boolean getAny() {
        return any;
    }
}