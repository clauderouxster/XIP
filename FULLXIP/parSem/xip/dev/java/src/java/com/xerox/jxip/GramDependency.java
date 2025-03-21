/*
 *   Xerox Research Centre Europe - Grenoble Laboratory
 *           project XIP (Xerox Incremental Parser)
 *  Copyright (C) 2004 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved
 *
 */
package com.xerox.jxip;

import java.util.Hashtable;

/**
 * Description of the Class
 *
 * @author grondeau @created 17 janvier 2006
 */
public class GramDependency implements Comparable {

    private String name;
    // listOfDependencyFileName.get(fileIndex) return the file name of the dependency declaration
    private int fileIndex;
    private Hashtable<Integer, String> listOfDependencyFileName;
    // offset in the source file of the dependency declaration
    private int fileOffset;

    /**
     * Constructor for the GramDependency object
     *
     * @param nam
     * @param index
     * @param listOfDependencyFileNam
     * @param offset offset in the source file of the feature declaration
     */
    public GramDependency(String nam, int index, Hashtable<Integer, String> listOfDependencyFileNam, int offset) {
        name = nam;
        fileIndex = index;
        listOfDependencyFileName = listOfDependencyFileNam;
        fileOffset = offset;
    }

    /**
     * Gets the name attribute of the GramDependency object
     *
     * @return The name value
     */
    public synchronized String getName() {
        return name;
    }

    /**
     *
     *
     * @return
     */
    @Override
    public String toString() {
        return name;
    }

    /**
     * @return The fileName
     */
    public synchronized String getFileName() {
        if (fileIndex == -1) {
            return null;
        }
        String file = listOfDependencyFileName.get(new Integer(fileIndex));
        if (file.compareTo("") == 0) {
            return null;
        } else {
            return file;
        }
    }

    /**
     * @return the offset
     */
    public synchronized int getFileOffset() {
        return fileOffset;
    }

    /**
     *
     *
     * @param obj
     * @return
     */
    @Override
    public int compareTo(java.lang.Object obj) {
        String str1 = ((GramDependency) obj).getName();
        return name.compareTo(str1);
    }
}