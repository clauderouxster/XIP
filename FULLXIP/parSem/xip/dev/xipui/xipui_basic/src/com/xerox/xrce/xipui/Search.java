/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 ****************************************************************************************
 * Copyright (c) 2005 Xerox Corporation                                                 *
 * Xerox Research Centre Europe - Grenoble                                              *
 * All Rights Reserved                                                                  *
 * Copyright protection claimed includes all forms and matters of copyrightable         *
 * material and information now allowed by statutory or judicial law or                 *
 * hereinafter granted, including without limitation, material generated from           *
 * the software programs which are displayed on the screen such as icons,               *
 * screen display looks, etc. All rights reserved. No part of this document             *
 * may be reproduced, stored in a retrieval system, or transmitted, in any              *
 * form or by any means, electronic, mechanical, photocopied, recorded, or              *
 * otherwise, without prior written consent from XEROX corporation.                     *
 ****************************************************************************************
 */
package com.xerox.xrce.xipui;

import java.util.regex.*;
import java.util.*;

/**
 *
 * @author Vianney Grassaud
 */
public class Search {

    private Vector offsets; 
    private Pattern p;
    private Matcher m;
    private String fileName = null;

    /**
     *create a search with an empty vector of Search.Offset
     */
    public Search() {
        offsets = new Vector();
    }

    /**
     * Creates a new instance of Search
     *this method call the default constructor and the setSearch method
     *@param regex the regular expression to search
     *@param text the text where we want ot find the regex
     *@param flag the flag to use for this search
     *@param fileName the fileName to search can be null, it's only use for Search.offset
     *
     */
    public Search(String regex, String text, int flag, String fileName) {
        this();
        setSearch(regex, text, flag, fileName);

    }

    /**
     *Return the vector of Search.Offset founds
     *@return a vector of Search.Offset
     */
    public Vector getOffsets() {
        return this.offsets;
    }

    /**
     *Set the search
     *@param regex the regular expression to search
     *@param text the text where we want ot find the regex
     *@param flag the flag to use for this search
     *@param fileName the fileName to search can be null, it's only use for Search.offset
     */
    public void setSearch(String regex, String text, int flag, String fileName) {
        if (regex != null && text != null) {
            p = Pattern.compile(regex, flag);
            m = p.matcher(text);

        }
        this.fileName = fileName;
        offsets = new Vector();
    }

    /**
     *Return the number of offset the search had found
     *@return the number of offset in the Vector of Search.Offset
     */
    public int count() {
        return offsets.size();
    }

    /**
     *return the file name linked with the text of the search.
     *@return the file name
     */
    public String getFileName() {
        return this.fileName;
    }

    /**
     *Load the search.
     *
     */
    public void find() {
        while (next()) {
            offsets.add(new Offset(offsetStart(), offsetEnd(), getFileName()));
        }
    }

    /**
     *Return an enumeration of the vector of offset found.
     *@return Enumeration can be empty
     */
    public Enumeration getAllOffsets() {
        return offsets.elements();
    }

    /**
     *Search the next offset
     *@return true if a next offset was found false otherwise
     */
    public boolean next() {
        if (m != null) {
            return m.find();
        }
        return false;
    }

    /**
     *Return the right offset of a pattern found
     *@return the right offset or -1 if no right offset
     */
    public int offsetEnd() {
        if (m != null) {
            return m.end();
        } else {
            return -1;
        }
    }

    /**
     *Return the left offset of a pattern found
     *@return the left offset or -1 if no right offset
     */
    public int offsetStart() {
        if (m != null) {
            return m.start();
        } else {
            return -1;
        }

    }

    /***
     *The Offset sub class.
     *
     *
     */
    public class Offset {

        // the left position of the pattern
        int start;
        // the right position of the pattern
        int end;
        // the file name usefull to identify the membership of this offset
        String fileName = null;

        /**
         *Create an Offset
         *@param start the left position of the pattern
         *@param end  the right position of the pattern
         * @param fileName 
         */
        public Offset(int start, int end, String fileName) {
            setStart(start);
            setEnd(end);
            this.fileName = fileName;
        }

        /**
         *Set the left pattern position
         **@param start the left position of the pattern
         */
        public void setStart(int start) {
            this.start = start;
        }

        /**
         *Set the right pattern position
         * @param end 
         */
        public void setEnd(int end) {
            this.end = end;
        }

        /**
         *Return the file name of this offset
         *@return the filename
         */
        public String getFileName() {
            return this.fileName;
        }

        /**
         *Return the right position of the offset
         *@return the right position
         */
        public int getEnd() {
            return this.end;
        }

        /**
         *Return the left position of the offset
         *@return the left position
         */
        public int getStart() {
            return this.start;
        }

        /**
         *Give a String representation of the offset
         *@return a string like that : start : end
         */
        public String toString() {
            return getStart() + " : " + getEnd();
        }
    }
}
