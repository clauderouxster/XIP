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
public class GramCategory implements Comparable {

    //catgegory name
    private String name;
    // listOfCategoryFileName.get(fileIndex) return the file name where there is the Category declaration
    private int fileIndex;
    private Hashtable listOfCategoryFileName;
    // offset in the source file of the Category declaration
    private int fileOffset;

    /**
     * Constructor for the GramCategory object
     *
     * @param nam category name
     * @param index file index in the hastable
     * @param listOfCategoryFileNam Hastable of category file declaration
     * @param offset offset of the declaration in the declaration file
     */
    public GramCategory(String nam, int index, Hashtable listOfCategoryFileNam, int offset) {
        name = nam;
        fileIndex = index;
        listOfCategoryFileName = listOfCategoryFileNam;
        fileOffset = offset;
    }

    /**
     * Gets the Category name
     *
     * @return The name
     */
    public synchronized String getName() {
        return name;
    }

    /**
     * Gets the fileName of the Category declaration
     *
     * @return The fileName
     */
    public synchronized String getFileName() {
        if (fileIndex == -1) {
            return null;
        }
        String file = (String) listOfCategoryFileName.get(new Integer(fileIndex));
        if (file.compareTo("") == 0) {
            return null;
        } else {
            return file;
        }
    }

    /**
     * Gets the offset of the feature Category in the declaration file
     *
     * @return the offset
     */
    public synchronized int getFileOffset() {
        return fileOffset;
    }

    /**
     * @return the name
     */
    @Override
    public synchronized String toString() {
        return name;
    }

    /**
     *
     *
     * @param obj
     * @return
     */
    @Override
    public synchronized int compareTo(java.lang.Object obj) {
        String str1 = ((GramCategory) obj).getName();
        return name.compareTo(str1);
    }
}