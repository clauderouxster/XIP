/*
 *   Xerox Research Centre Europe - Grenoble Laboratory
 *           project XIP (Xerox Incremental Parser)
 *  Copyright (C) 2004 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved
 *
 */
package com.xerox.jxip;

/**
 * XipLeaf Class
 *
 * @author grondeau @created 22 mars 2006
 */
public class XipLeaf extends XipNode {

    private String surface;
    private String lemma;
    private String initialfeatures;
    private long lTokOffset = 0;
    private long rTokOffset = 0;

    /**
     * Constructor for the XipLeaf object
     *
     * @param category the node name
     * @param infos
     * @param surface surface
     * @param lemma lemma
     * @param initialfeatures
     * @param left offset of the beginning of the word in the sentence
     * @param right offset of the end of the word in the sentence
     * @param index index used as link with c++ data
     * @param gramNodeId
     * @param fileName
     * @param sentenceNb
     * @param leftChar
     * @param rightChar
     */
    public XipLeaf(String category, String infos, String surface, String lemma, String initialfeatures,
            long left, long right, long index, long gramNodeId, String fileName,
            long sentenceNb, long leftChar, long rightChar) {
        super(category, infos, left, right, index, gramNodeId, fileName, sentenceNb, leftChar, rightChar);
        //String mes = "XipLeaf::XipLeaf() category=" + category + " surface=" + sur;
        //mes += " lemma=" + lem + " left=" + left + " right=" + right + " index=" + index;
        //System.out.println(mes);
        this.surface = JXip.duplicateString(surface);
        this.lemma = JXip.duplicateString(lemma);
        this.initialfeatures = JXip.duplicateString(initialfeatures);

    }

    /**
     * Gets the lemma
     *
     * @return The lemma
     */
    @Override
    public synchronized String getLemma() {
        return lemma;
    }

    /**
     * Gets the surface
     *
     * @return The surface
     */
    @Override
    public synchronized String getSurface() {
        return surface;
    }

    /**
     * Gets the initial features
     *
     * @return The initial features
     */
    public synchronized String getInitialFeatures() {
        return initialfeatures;
    }

    /**
     * Gets the left token-based offset
     *
     * @return The left token based offset
     */
    @Override
    public synchronized long getLeftTokOffset() {
        return lTokOffset;
    }

    /**
     * Gets the right token-based offset
     *
     * @return The right token based offset
     */
    @Override
    public synchronized long getRightTokOffset() {
        return rTokOffset;
    }

    /**
     * Sets the left token-based offset
     *
     * @param n the left token based offset
     */
    public synchronized void setLeftTokOffset(long n) {
        lTokOffset = n;
    }

    /**
     * Sets the right token-based offset
     *
     * @param n the right token based offset
     */
    public synchronized void setRightTokOffset(long n) {
        rTokOffset = n;
    }

    /**
     * return always true
     *
     * @return return always true
     */
    @Override
    public synchronized boolean isLeaf() {
        return true;
    }

    /**
     * return always XipNode.LEAF
     *
     * @return return always XipNode.LEAF
     */
    @Override
    public synchronized int getType() {
        return XipNode.LEAF;
    }

    /**
     * return the surface
     *
     * @return return the surface
     */
    @Override
    public synchronized String toString() {
        return surface;
    }

    /**
     * return XipLeaf data as a string
     *
     * @param withFeature if true build the string with the XipNode features
     * @param tab tabulation
     * @param deeply unused
     * @return XipLeaf to a string
     */
    @Override
    public synchronized String toString(boolean withFeature, String tab, boolean deeply) {
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append(tab);
        stringBuilder.append(getCategory());
        stringBuilder.append(" {");
        stringBuilder.append(getIndex());
        stringBuilder.append("}");
        stringBuilder.append(" ");
        stringBuilder.append(surface);
        stringBuilder.append(":");
        stringBuilder.append(lemma);
        stringBuilder.append(" (");
        stringBuilder.append(getLeft());
        stringBuilder.append("-");
        stringBuilder.append(getRight());
        stringBuilder.append(") ");
        stringBuilder.append(featuresToString(withFeature));
        stringBuilder.append("\n");
        return stringBuilder.toString();
    }
}
