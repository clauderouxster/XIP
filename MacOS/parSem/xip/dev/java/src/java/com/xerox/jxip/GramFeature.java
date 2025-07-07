/*
 *   Xerox Research Centre Europe - Grenoble Laboratory
 *           project XIP (Xerox Incremental Parser)
 *  Copyright (C) 2004 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved
 *
 */
package com.xerox.jxip;

import java.util.Collections;
import java.util.Hashtable;
import java.util.Vector;

/**
 * Description of the Class
 *
 * @author grondeau @created 17 janvier 2006
 */
public class GramFeature implements Comparable {

    private String name;
    // Vector of GramFeature
    private Vector<GramFeature> child;
    // listOfFeatureFileName.get(fileIndex) return the file name of the feature declaration
    private int fileIndex;
    private Hashtable listOfFeatureFileName;
    // offset in the source file of the feature declaration
    private int fileOffset;

    /**
     * Constructor for the GramFeature object
     *
     * @param nam feature name
     * @param ch vector of child (GramFeature)
     * @param index file index in the Hashtable
     * @param listOfFeatureFile Hastable of Feature file declaration
     * @param offset offset in the source file of the feature declaration
     */
    @SuppressWarnings("unchecked")
    public GramFeature(String nam, Vector<GramFeature> ch, int index, Hashtable listOfFeatureFile, int offset) {
        name = nam;
        child = ch;
        fileIndex = index;
        listOfFeatureFileName = listOfFeatureFile;
        fileOffset = offset;
        //System.out.println("GramFeature() fileName = " + listOfFeatureFileName.get(fileIndex) + " fileIndex = " + fileIndex);
        if (child != null) {
            Collections.sort(child);
        }
    }

    /**
     * Gets the feature name
     *
     * @return The name
     */
    public synchronized String getName() {
        return name;
    }

    /**
     * Gets the fileName of the feature declaration
     *
     * @return The fileName
     */
    public synchronized String getFileName() {
        //System.out.println("fileIndex = " + fileIndex + " listOfFeatureFileName = " + listOfFeatureFileName);
        if (fileIndex == -1) {
            return null;
        }
        String file = (String) listOfFeatureFileName.get(new Integer(fileIndex));
        if (file.compareTo("") == 0) {
            return null;
        } else {
            return file;
        }
    }

    /**
     * Gets the offset of the feature declaration in the declaration file
     *
     * @return the offset
     */
    public synchronized int getFileOffset() {
        return fileOffset;
    }

    /**
     * Gets the child
     *
     * @return The child: vector of GramFeature
     */
    public synchronized Vector<GramFeature> getChild() {
        return child;
    }

    /**
     * @return the name
     */
    @Override
    public String toString() {
        return name;
    }

    /**
     *
     *
     * @param obj
     * @return
     */
    @Override
    public int compareTo(java.lang.Object obj) {
        String str1 = ((GramFeature) obj).getName();
        return name.compareTo(str1);
    }
}