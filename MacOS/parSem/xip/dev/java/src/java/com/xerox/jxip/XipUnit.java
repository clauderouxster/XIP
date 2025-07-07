/*
 *   Xerox Research Centre Europe - Grenoble Laboratory
 *           project XIP (Xerox Incremental Parser)
 *  Copyright (C) 2004 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved
 *
 */
package com.xerox.jxip;

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

/**
 * XipUnit Class
 *
 * @author grondeau @created April 8, 2005
 */
public class XipUnit {

    /**
     * The root XipNode
     */
    private XipNode root;
    /**
     * Vector of XipDependency
     */
    private Vector<XipDependency> dependencies;
    /**
     * Vector of XipLeaf
     */
    private Vector<XipLeaf> leaves;
    private String language;
    /**
     * sentence number
     */
    private long sentenceNb;

    /**
     * Constructor for the XipUnit object
     *
     * @param rootNode the root XipNode
     * @param leaves the leaves: Vector of XipLeaf
     * @param dependencies the dependencies: Vector of dependency
     * @param language
     * @param sentNb
     */
    public XipUnit(XipNode rootNode, Vector<XipLeaf> leaves,
            Vector<XipDependency> dependencies, String language, long sentNb) {
        //System.out.println("XipUnit::XipUnit() rootNode=" + rootNode.getCategory() + " language = " + lang);
        this.dependencies = dependencies;
        this.leaves = leaves;
        root = rootNode;
        this.language = JXip.duplicateString(language);
        XipNode previous_parent = null;
        sentenceNb = sentNb;
        long token_nb = 0;
        for (int i = 0; i < leaves.size(); i++) {
            if (leaves.elementAt(i).getMother() != previous_parent) {
                token_nb++;
                previous_parent = leaves.elementAt(i).getMother();
            }
            leaves.elementAt(i).setLeftTokOffset(token_nb);
            leaves.elementAt(i).setRightTokOffset(token_nb);
        }
    }

    /**
     * Gets the root XipNode
     *
     * @return The root XipNode
     */
    public synchronized XipNode getRoot() {
        return root;
    }

    /**
     * Gets The sentence number
     *
     * @return The sentence number
     */
    public synchronized long getSentenceNb() {
        return sentenceNb;
    }

    /**
     * Gets the dependencies
     *
     * @return The dependencies: Vector of XipDependency
     */
    public synchronized Vector<XipDependency> getDependencies() {
        return dependencies;
    }

    /**
     * Gets the dependencies having the specified name.
     *
     * @param name the name of the expected dependencies.
     * @return the dependencies having the appropriate name.
     */
    public synchronized Vector<XipDependency> getDependenciesByName(String name) {
        Vector<XipDependency> result = new Vector<XipDependency>();
        for (XipDependency dep : this.dependencies) {
            if (dep.getName().toLowerCase().equals(name.toLowerCase())) {
                result.add(dep);
            }
        }
        return result;
    }

    /**
     * Tests whether the current unit has the specified dependency or not.
     *
     * @param name the name of the dependency.
     * @return true if the node has the specified dependency, false otherwise.
     */
    public synchronized boolean hasDependency(String name) {
        for (XipDependency dep : this.dependencies) {
            if (dep.getName().toUpperCase().equals(name.toUpperCase())) {
                return true;
            }
        }
        return false;
    }

    /**
     * Returns the node having the specified gram id in the current unit
     *
     * @param id the search node gram id
     * @return the XipNode corresponding to this id if found in the current unit
     * null otherwise.
     */
    public synchronized XipNode getNodeByGramId(long id) {
        return root.getNodeByGramId(id);
    }

    /**
     * Returns the leaf node having the specified token offset (token position
     * in the sentence).
     *
     * @param offset the token offset
     * @return the leaf node having the specified token offset
     */
    public synchronized XipNode getLeafNodeByTokOffset(long offset) {
        // parse all leaves to find the appropriate offset.
        for (XipNode leaf : this.leaves) {
            if (leaf.getLeftTokOffset() == offset) {
                return leaf;
            }
        }
        // if none has been found, return null;
        return null;
    }

    /**
     * Returns the node having the specified uid in the current unit
     *
     * @param uid the search node uid
     * @return the XipNode corresponding to this id if found in the current unit
     * null otherwise.
     */
    public synchronized XipNode getNodeUid(String uid) {
        return root.getNodeByUid(uid);
    }

    /**
     * Returns all XipNodes descendant all the root node that have the feature
     * name with the specified value.
     *
     * @param name the name of the feature
     * @param value the value of the feature ("*" if all values are fine)
     * @return a Vector of XipNode having the appropriate feature.
     * @see XipNode#getDescendantsByFeature(String name, String value)
     */
    public synchronized Vector<XipNode> getNodesByFeature(String name, String value) {
        return root.getDescendantsByFeature(name, value);
    }

    /**
     * Returns all XipNodes descendant all the root node that have the category
     * name.
     *
     * @param category the name of the category
     * @return a Vector of XipNode having the appropriate category.
     * @see XipNode#getDescendantsByCategory(String category)
     */
    public synchronized Vector<XipNode> getNodesByCategory(String category) {
        return root.getDescendantsByCategory(category);
    }

    /**
     * Returns all XipNodes descendant all the root node that have the category
     * name and the specified features. Features String format are the XIP
     * traditional format.
     *
     * @param category the name of the category
     * @param features the String representation of the feature lists (XIP
     * format)
     *
     * @return a Vector of XipNode having the appropriate category.
     * @see XipNode#getDescendantsByCategory(String category)
     */
    public synchronized Vector<XipNode> getNodesByCategory(String category, String features) {
        return root.getDescendantsByCategory(category, features);
    }

    /**
     * Gets the leaves node
     *
     * @return The leaves: a Vector of XipLeaf
     */
    public synchronized Vector<XipLeaf> getLeaves() {
        return leaves;
    }

    /**
     * Returns a Vector containing leaf nodes descendant of the current unit.
     * Leaves that are only different interpretations of a same node are NOT
     * considered.
     *
     * @return a Vector of XipNodes corresponding to the tokenization part.
     */
    public synchronized Vector<XipNode> getLeafNodes() {
        return this.root.getLeafNodes();
    }

    /**
     * Gets the language attribute
     *
     * @return The language value
     */
    public synchronized String getLanguage() {
        return language;
    }

    /**
     * Gets the sentence
     *
     * @return The string sentence
     */
    public synchronized String getSentenceString() {
        StringBuilder stringBuilder = new StringBuilder();
        Hashtable<String, Long> hTable = new Hashtable<String, Long>();
        for (Enumeration enumeration = leaves.elements(); enumeration.hasMoreElements();) {
            XipLeaf xipLeaf = (XipLeaf) enumeration.nextElement();
            if (!xipLeaf.isFicticious()) {
                String surface = xipLeaf.getSurface();
//                if (surface.compareTo("'") == 0) {
//                    stringBuilder.append(".");
//                } else {
                    // do not insert daughter node with the same surface string
                    Long indexMother = new Long(xipLeaf.getMother().getIndex());
                    Long indexMotherIns = hTable.get(surface);
                    if (indexMotherIns != null) {
                        // check if it is a daughter node
                        if (indexMotherIns.longValue() != indexMother.longValue()) {
                            // it's not a daughter node with the same surface string
                            hTable.put(surface, indexMother);
                            stringBuilder.append(" ");
                            stringBuilder.append(xipLeaf.toString());
                        }
                    } else {
                        // this surface string is found for the first time
                        hTable.put(surface, indexMother);
                        stringBuilder.append(" ");
                        stringBuilder.append(xipLeaf.toString());
                    }
                }
            }
//        }
        return stringBuilder.toString();
    }

    /**
     * return XipUnit data as a string
     *
     * @return XipUnit to a string
     */
    @Override
    public synchronized String toString() {
        return this.getSentenceString();
    }

    /**
     * return XipUnit data as a string
     *
     * @param withFeature if true build the string with the XipNode features
     * @return XipUnit to a string
     */
    public synchronized String toString(boolean withFeature) {
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append("language = ");
        stringBuilder.append(language);
        stringBuilder.append("\t\tsentenceNb =");
        stringBuilder.append(sentenceNb);
        stringBuilder.append("\n");
        stringBuilder.append(root.toString(withFeature, "", true));
        // dependencies
        stringBuilder.append("-------------------DEPENDENCIES--------------\n");
        for (Enumeration enumeration = dependencies.elements(); enumeration.hasMoreElements();) {
            XipDependency dependency = (XipDependency) enumeration.nextElement();
            stringBuilder.append(dependency.toString(withFeature));
        }
        //leaves
        stringBuilder.append("-------------------LEAVES--------------\n");
        for (Enumeration enumeration = leaves.elements(); enumeration.hasMoreElements();) {
            XipLeaf xipLeaf = (XipLeaf) enumeration.nextElement();
            stringBuilder.append(xipLeaf.toString(false, "", false));
        }
        return stringBuilder.toString();
    }
}
