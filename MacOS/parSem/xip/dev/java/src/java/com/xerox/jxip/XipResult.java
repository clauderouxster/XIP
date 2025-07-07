/*
 *   Xerox Research Centre Europe - Grenoble Laboratory
 *           project XIP (Xerox Incremental Parser)
 *  Copyright (C) 2004 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved
 *
 */
package com.xerox.jxip;

import java.util.Enumeration;
import java.util.Vector;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * XipResult Class
 *
 * @author grondeau @created April 8, 2005
 */
public class XipResult extends java.lang.Object {

    /**
     * Vector of XipUnit
     */
    private Vector<XipUnit> xipUnits;
    /**
     * the file name which has been parsed
     */
    private String fileName;

    /**
     * Constructor
     *
     * @param xipUnits Vector of XipUnit
     * @param fileName the file name which has been parsed
     */
    public XipResult(Vector<XipUnit> xipUnits, String fileName) {
        //System.out.println("XipResult::XipResult()");
        this.xipUnits = xipUnits;
        this.fileName = JXip.duplicateString(fileName);
    }

    /**
     * Gets the xipUnits
     *
     * @return Vector of XipUnit
     * @deprecated replaced by getXipUnits
     */
    @Deprecated
    public synchronized Vector<XipUnit> getSentences() {
        return xipUnits;
    }

    /**
     * Gets the XipUnit vector
     *
     * @return Vector of XipUnit
     */
    public synchronized Vector<XipUnit> getXipUnits() {
        return xipUnits;
    }

    /**
     * Gets the XipUnit vector
     *
     * @return Vector of XipUnit
     */
    public String getFileName() {
        return fileName;
    }

    /**
     * Gets the sentences
     *
     * @return The string sentence
     */
    public synchronized String getSentenceString() {
        StringBuilder stringBuilder = new StringBuilder();
        boolean first = true;
        for (Enumeration enumeration = xipUnits.elements(); enumeration.hasMoreElements();) {
            XipUnit xipUnit = (XipUnit) enumeration.nextElement();
            if (!first) {
                stringBuilder.append("\n");
            } else {
                first = false;
            }
            stringBuilder.append(xipUnit.getSentenceString());
        }
        return stringBuilder.toString();
    }

    /**
     * return XipResult data as a string
     *
     * @param withFeature if true build the string with the XipNode features
     * @return XipResult to a string
     */
    public synchronized String toString(boolean withFeature) {
        //System.out.println("toString: " + withFeature + " fileName: " + fileName);
        StringBuilder stringBuilder = new StringBuilder();
        if (fileName != null) {
            Pattern p = Pattern.compile(".*[\\\\/](.*)$");
            Matcher m = p.matcher(fileName);
            if (m.find()) {
                stringBuilder.append("fileName: ");
                stringBuilder.append(m.group(1));
                stringBuilder.append("\n");
            } else {
                stringBuilder.append("fileName: \n");
            }
        }
        int cnt = 0;
        for (Enumeration enumeration = xipUnits.elements(); enumeration.hasMoreElements();) {
            XipUnit xipUnit = (XipUnit) enumeration.nextElement();
            cnt++;
            stringBuilder.append("========================= sentence ");
            stringBuilder.append(cnt);
            stringBuilder.append("==========================\n");
            stringBuilder.append(xipUnit.toString(withFeature));
        }
        return stringBuilder.toString();
    }
}
