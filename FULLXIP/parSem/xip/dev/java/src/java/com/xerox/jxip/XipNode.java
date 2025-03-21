/*
 *   Xerox Research Centre Europe - Grenoble Laboratory
 *           project XIP (Xerox Incremental Parser)
 *  Copyright (C) 2004 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved
 *
 */
package com.xerox.jxip;

import java.lang.reflect.Method;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.Vector;

/**
 * XipNode Class
 *
 * @author grondeau @created 22 mars 2006
 */
public class XipNode implements Comparable {

    /**
     * to specify a node as XipNode
     */
    public static final int NODE = 1;
    /**
     * o specify a node as XipLeaf
     */
    public static final int LEAF = 2;
    /**
     * name of the node
     */
    private String category;
    /**
     * infos of the node
     */
    private String infos;
    /**
     * offset (in bytes) of the beginning of the word
     */
    private long left;
    /**
     * offset (in bytes) of the end of the word
     */
    private long right;
    /**
     * offset (in char) of the beginning of the word
     */
    private long leftChar;
    /**
     * offset (in char) of the end of the word
     */
    private long rightChar;
    /**
     * index of the node from the c++ struct
     */
    private long index;
    /**
     * same index used in the grammar (#n)
     */
    private long gramIdNode;
    /**
     * unique id: fileName+phraseNB+indexNode
     */
    private String uid;
    /**
     * vector of XipFeature
     */
    protected Vector<XipFeature> features;
    /**
     * vector of XipNode :the daughters node of this node
     */
    protected Vector<XipNode> daughters;
    /**
     * The mother of this node
     */
    protected XipNode mother;
    /**
     * Vector of XipDependency
     */
    protected Vector<XipDependency> dependencies;
    /**
     * Any object. Can be used by an application client to store specific info
     * related to this node
     */
    private Object miscInfo;

    /**
     * Constructor for the XipNode object
     *
     * @param category category name
     * @param infos
     * @param left offset of the beginning of the word in the sentence
     * @param right offset of the end of the word in the sentence
     * @param index
     * @param gramIdNode
     * @param fileName
     * @param sentenceNb
     * @param leftChar
     * @param rightChar
     */
    public XipNode(String category, String infos, long left, long right, long index, long gramIdNode,
            String fileName, long sentenceNb, long leftChar, long rightChar) {
        //String mes = "XipNode::XipNode() category=" + cat;
        //mes += " left=" + lef + " right=" + righ + " index=" + ind;
        //System.out.println(mes);
        this.category = JXip.duplicateString(category);
		this.infos = JXip.duplicateString(infos);
        this.left = left;
        this.right = right;
        this.leftChar = leftChar;
        this.rightChar = rightChar;
        this.index = index;
        this.gramIdNode = gramIdNode;
        daughters = new Vector<XipNode>();
        features = new Vector<XipFeature>();
        dependencies = new Vector<XipDependency>();
        mother = null;
        miscInfo = null;
        // build a unique ID
        uid = JXip.duplicateString(fileName) + "-" + sentenceNb + "-" + index;
        //System.out.println("XipNode::XipNode() uid = " + uid );
    }

    /**
     * get the name of the XipNode
     *
     * @return the name of the XipNode
     */
    public synchronized String getCategory() {
        return category;
    }

    /**
     * get the XipNode infos
     *
     * @return the XipNode infos
     */
    public synchronized String getInfos() {
        return infos;
    }

    /**
     * Gets the left position in the sentence
     *
     * @return the left position (offset in byte)
     */
    public synchronized long getLeft() {
        return left;
    }

    /**
     * gets the right position in the sentence
     *
     * @return the right position (offset in byte)
     */
    public synchronized long getRight() {
        return right;
    }

    /**
     * Gets the left position in the sentence
     *
     * @return the left position (offset in char)
     */
    public synchronized long getLeftChar() {
        return leftChar;
    }

    /**
     * gets the right position in the sentence
     *
     * @return the right position (offset in char)
     */
    public synchronized long getRightChar() {
        return rightChar;
    }

    /**
     * Gets the left token-based offset in the sentence
     *
     * @return the left token-based offset
     */
    public synchronized long getLeftTokOffset() {
        if (this.isLeaf()) {
            return this.getLeftTokOffset();
        } else if (!getDaughters().isEmpty()) {
            return getDaughters().elementAt(0).getLeftTokOffset();
        } else {
            return -1;
        }
    }

    /**
     * gets the right position in the sentence
     *
     * @return the right position
     */
    public synchronized long getRightTokOffset() {
        if (this.isLeaf()) {
            return this.getRightTokOffset();
        } else if (!getDaughters().isEmpty()) {
            return getDaughters().lastElement().getRightTokOffset();
        } else {
            return -1;
        }
    }

    /**
     * Gets the node index in the c++ struct
     *
     * @return The node index
     */
    public synchronized long getIndex() {
        return index;
    }

    /**
     * Gets the grammar node index
     *
     * @return The node index used in the grammar: #n
     */
    public synchronized long getGramIdNode() {
        return gramIdNode;
    }

    /**
     * Gets the a unique node identifier
     *
     * @return The uid value
     */
    public synchronized String getUid() {
        return uid;
    }

    /**
     * Gets the Id string of this node.
     *
     * @return The Id string of this node
     */
    public synchronized String getId() {
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append(getLeft());
        stringBuilder.append("-");
        stringBuilder.append(getRight());
        stringBuilder.append(":");
        stringBuilder.append(getIndex());
        return stringBuilder.toString();
    }

    /**
     * Gets the Id string of this node taking into account the sentence where it
     * occurs. Hence a node Id is unique over the input string/file
     *
     * @param sentid
     * @return The Id string of this node
     */
    public final synchronized String getId(long sentid) {
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append(sentid);
        stringBuilder.append(":");
        stringBuilder.append(getLeft());
        stringBuilder.append("-");
        stringBuilder.append(getRight());
        stringBuilder.append(":");
        stringBuilder.append(getIndex());
        return stringBuilder.toString();
    }

    /**
     * method for Collections.sort() to work properly
     */
    @Override
    public int compareTo(Object o) {
        if (this.getLeft() < ((XipNode) o).getLeft()) {
            return -1;
        } else if (this.getLeft() == ((XipNode) o).getLeft()) {
            if (this.getRight() < ((XipNode) o).getRight()) {
                return -1;
            } else if (this.getRight() == ((XipNode) o).getRight()) {
                return 0;
            } else {
                return 1;
            }
        } else {
            return 1;
        }
    }

    /**
     * Returns the node having the specified gram id among the current node or
     * its descendants.
     *
     * @param id the search node gram id
     * @return the XipNode corresponding to this id if found in the current node
     * null otherwise.
     */
    public synchronized XipNode getNodeByGramId(long id) {
        if (this.getGramIdNode() == id) {
            return this;
        } else {
            for (XipNode node : this.daughters) {
                XipNode result = node.getNodeByGramId(id);
                if (result != null) {
                    return result;
                }
            }
            return null;
        }
    }

    /**
     * Returns the node having the specified uid among the current node or its
     * descendants.
     *
     * @param uid the search node uid
     * @return the XipNode corresponding to this id if found in the current node
     * null otherwise.
     */
    public synchronized XipNode getNodeByUid(Object uid) {
        if (this.getUid().equals(uid)) {
            return this;
        } else {
            for (XipNode node : this.daughters) {

                XipNode result = node.getNodeByUid(uid);
                if (result != null) {
                    return result;
                }
            }
            return null;
        }
    }

    /**
     * Returns all XipNodes descendant of the current XipNode that have the
     * feature name with any value.
     *
     * @param name the name of the feature
     * @return a Vector of XipNode having the appropriate feature.
     */
    public synchronized Vector<XipNode> getDescendantsByFeature(String name) {
        return getDescendantsByFeature(name, "*");
    }

    /**
     * Returns all XipNodes descendant of the current XipNode that have the
     * feature name with the specified value.
     *
     * @param name the name of the feature
     * @param value the value of the feature ("*" if all values are fine)
     * @return a Vector of XipNode having the appropriate feature.
     */
    public synchronized Vector<XipNode> getDescendantsByFeature(String name,
            String value) {
        Vector<XipNode> result = new Vector<XipNode>();
        for (XipNode node : this.daughters) {
            if (node.hasFeature(name, value)) {
                result.add(node);
            }
            result.addAll(node.getDescendantsByFeature(name, value));
        }
        return result;
    }

    /**
     * Returns all XipNodes descendant of the current XipNode that have the
     * feature name with any value. Does not return XipNodes having an ancestor
     * with the same specified feature
     *
     * @param name the name of the feature
     * @return a Vector of XipNode having the appropriate feature.
     */
    public synchronized Vector<XipNode> getFirstDescendantsByFeature(String name) {
        return getFirstDescendantsByFeature(name, "*");
    }

    /**
     * Returns all XipNodes descendant of the current XipNode that have the
     * feature name with the specified value. Does not return XipNodes having an
     * ancestor with the same specified feature
     *
     * @param name the name of the feature
     * @param value the value of the feature ("*" if all values are fine)
     * @return a Vector of XipNode having the appropriate feature.
     */
    public synchronized Vector<XipNode> getFirstDescendantsByFeature(String name,
            String value) {
        Vector<XipNode> result = new Vector<XipNode>();
        for (XipNode node : this.daughters) {
            if (node.hasFeature(name, value)) {
                result.add(node);
            } else {
                result.addAll(node.getDescendantsByFeature(name, value));
            }
        }
        return result;
    }

    /**
     * Returns all XipNodes descendant of the current XipNode that have the
     * category name.
     *
     * @param category the name of the category.
     * @return a Vector of XipNode having the appropriate categories.
     * @see getDescendantsByCategory(String category, String features)
     */
    public synchronized Vector<XipNode> getDescendantsByCategory(String category) {
        return this.getDescendantsByCategory(category, "");
    }

    /**
     * Returns all XipNodes descendant of the current XipNode that have the
     * category name and the specified features. Features String format are the
     * XIP traditional format.
     *
     * @param category the name of the category.
     * @param features the String representation of the feature lists (XIP
     * format)
     * @return a Vector of XipNode having the appropriate categories.
     */
    public synchronized Vector<XipNode> getDescendantsByCategory(String category,
            String features) {
        Vector<XipNode> result = new Vector<XipNode>();
        for (XipNode node : this.daughters) {
            if (node.getCategory().equalsIgnoreCase(category) && node.hasFeatures(features)) {
                result.add(node);
            }
            result.addAll(node.getDescendantsByCategory(category, features));
        }
        return result;
    }

    /**
     * Returns all XipNodes descendant of the current XipNode that have the
     * category name. Does not return XipNodes having an ancestor with the same
     * specified category
     *
     * @param category the name of the category.
     * @return a Vector of XipNode having the appropriate categories.
     */
    public synchronized Vector<XipNode> getFirstDescendantsByCategory(String category) {
        Vector<XipNode> result = new Vector<XipNode>();
        for (XipNode node : this.daughters) {
            if (node.getCategory().equalsIgnoreCase(category)) {
                result.add(node);
            } else {
                result.addAll(node.getDescendantsByCategory(category));
            }
        }
        return result;
    }

    /**
     * Returns all XipNodes descendant of the current XipNode that have the
     * named dependency.
     *
     * @param dependency the name of the dependency.
     * @return a Vector of XipNode having the appropriate dependency.
     * @see getDescendantsByDependency(String dependency, String features, int
     * position)
     */
    public synchronized Vector<XipNode> getDescendantsByDependency(String dependency) {
        return this.getDescendantsByDependency(dependency, "", 0);
    }

    /**
     * Returns all XipNodes descendant of the current XipNode that have the
     * named dependency with the specified features.
     *
     * @param dependency the name of the dependency.
     * @param features the String representation of the feature lists (XIP
     * format)
     * @return a Vector of XipNode having the appropriate dependency.
     * @see getDescendantsByDependency(String dependency, String features, int
     * position)
     */
    public synchronized Vector<XipNode> getDescendantsByDependency(String dependency,
            String features) {
        return this.getDescendantsByDependency(dependency, features, 0);
    }

    /**
     * Returns all XipNodes descendant of the current XipNode that have the
     * named dependency with the specified features. Features String format are
     * the XIP traditional format.
     *
     * @param dependency the name of the dependency.
     * @param features the String representation of the feature lists (XIP
     * format)
     * @param position the expected position of the node in the dependency (0 if
     * any position is OK).
     * @return a Vector of XipNode having the appropriate dependency.
     */
    public synchronized Vector<XipNode> getDescendantsByDependency(String dependency,
            String features, int position) {
        Vector<XipNode> result = new Vector<XipNode>();
        for (XipNode node : this.daughters) {
            if (node.hasDependency(dependency, features, position)) {
                result.add(node);
            }
            result.addAll(node.getDescendantsByCategory(category, features));
        }
        return result;
    }

    /**
     * Invokes a boolean method on the current node and returns its results. The
     * method MUST: <ul> <li> be public static <li> return a boolean <li> have a
     * XipNode as first parameter </ul>
     *
     * @param method the Method to invoke on the node
     * @param params the list of parameters (excepted the node itself)
     * @return the boolean result of the method
     * @throws java.lang.Exception whether the method is not appropriate.
     * @see #getDescendantsByBooleanMethod(String className, String methodName)
     * @see #getDescendantsByBooleanMethod(Method method, Object[] params)
     */
    public synchronized boolean invokeMethodOnNode(Method method, Object[] params)
            throws Exception {
        if (!method.getReturnType().toString().equals("boolean")) {
            throw new XipException("The function should return a boolean!");
        }
        Class[] params1 = method.getParameterTypes();
        if (params1.length != 1
                || !params1[0].equals(Class.forName("com.xerox.jxip.XipNode"))) {
            throw new XipException("The function should concern a XipNode!");
        }
        Object result;
        Object[] allParams = new Object[params.length + 1];
        allParams[0] = this;
        for (int i = 0; i < params.length; ++i) {
            allParams[i + 1] = params[i];
        }
        result = method.invoke(null, allParams);
        boolean boolResult = ((Boolean) result).booleanValue();
        return boolResult;
    }

    /**
     * Invokes a boolean method on every descendants of the current node and
     * returns the ones for which the method has returned true. The method MUST:
     * <ul> <li> be public static <li> return a boolean <li> have a XipNode as
     * first and ONLY parameter </ul>
     *
     * @param className the String name of the class where the method can be
     * found (package included)
     * @param methodName the String name of the method to be invoked
     * @return the Vector containing all descendants of the current XipNode and
     * selected by the method
     * @throws java.lang.Exception whether the method is not appropriate.
     * @see #getDescendantsByBooleanMethod(Method method, Object[] params)
     */
    public synchronized Vector<XipNode> getDescendantsByBooleanMethod(String className,
            String methodName) throws Exception {
        Class classe = Class.forName(className);
        for (Method method : classe.getMethods()) {
            if (method.getName().equals(methodName)) {
                Class[] classParams = method.getParameterTypes();
                if (classParams.length == 1
                        && classParams[0].equals(Class.forName("com.xerox.jxip.XipNode"))) {
                    Object[] params = {};
                    return getDescendantsByBooleanMethod(method, params);
                }
            }
        }
        throw new XipException("Function " + className + "."
                + methodName + " (com.xerox.jxip.XipNode) not found.");
    }

    /**
     * Invokes a boolean method on every descendants of the current node and
     * returns the ones for which the method has returned true. The method MUST:
     * <ul> <li> be public static <li> return a boolean <li> have a XipNode as
     * first parameter </ul>
     *
     * @param method the Method to invoke on the node
     * @param params the list of parameters (excepted the node itself)
     * @return the Vector containing all descendants of the current XipNode and
     * selected by the method
     * @throws java.lang.Exception
     * @Exception an Exception thrown whether the method is not appropriate.
     * @see #getDescendantsByBooleanMethod(String className, String methodName)
     */
    public synchronized Vector<XipNode> getDescendantsByBooleanMethod(Method method,
            Object[] params) throws Exception {
        Vector<XipNode> result = new Vector<XipNode>();
        if (!method.getReturnType().toString().equals("boolean")) {
            throw new XipException("The function should return a boolean!");
        }
        Class[] params1 = method.getParameterTypes();
        if (params1.length < 1 || !params1[0].equals(Class.forName("com.xerox.jxip.XipNode"))) {
            throw new XipException("The function should concern a XipNode!");
        }
        Object[] allParams = new Object[params.length + 1];
        for (int i = 0; i < params.length; ++i) {
            allParams[i + 1] = params[i];
        }
        for (XipNode node : this.daughters) {
            allParams[0] = node;
            if (((Boolean) method.invoke(null, allParams)).booleanValue()) {
                result.add(node);
            }
            result.addAll(node.getDescendantsByBooleanMethod(method, params));
        }
        return result;
    }

    /**
     * Gets the dependencies having the specified name.
     *
     * @param name the name of the expected dependencies.
     * @return the dependencies having the appropriate name.
     */
    public synchronized Vector<XipDependency> getDependenciesByName(String name) {
        return this.getDependenciesByName(name, "", 0);
    }

    /**
     * Gets the dependencies having the specified name.
     *
     * @param name the name of the expected dependencies.
     * @param position the expected position of the current node if the
     * dependency arguments. (0 if any position is OK).
     * @return the dependencies having the appropriate name.
     */
    public synchronized Vector<XipDependency> getDependenciesByName(String name, int position) {
        return this.getDependenciesByName(name, "", position);
    }

    /**
     * Gets the dependencies having the specified name and feature.
     *
     * @param name the name of the expected dependencies.
     * @param features the expected list of features hold by the dependency.
     * Format is XIP usual format.
     * @return the dependencies having the appropriate name and feature.
     */
    public synchronized Vector<XipDependency> getDependenciesByName(String name, String features) {
        return this.getDependenciesByName(name, features, 0);
    }

    /**
     * Gets the dependencies having the specified name and feature.
     *
     * @param name the name of the expected dependencies.
     * @param features the expected list of features hold by the dependency.
     * Format is XIP usual format.
     * @param position the expected position of the current node if the
     * dependency arguments. (0 if any position is OK).
     * @return the dependencies having the appropriate name and features.
     */
    public synchronized Vector<XipDependency> getDependenciesByName(String name,
            String features, int position) {
        Vector<XipDependency> result = new Vector<XipDependency>();
        for (XipDependency dep : this.dependencies) {
            if (dep.getName().equalsIgnoreCase(name)
                    && dep.hasFeatures(features)
                    && (position < 1 || dep.getArg(position).equals(this))) {
                result.add(dep);
            }
        }
        return result;
    }

    /**
     * Tests whether the current node has the specified dependency or not. Same
     * as hasDependency(name, 0);
     *
     * @param name the name of the dependency.
     * @return true if the node has the specified dependency, false otherwise.
     * @see #hasDependency(String name, String features, int position)
     */
    public synchronized boolean hasDependency(String name) {
        return this.hasDependency(name, "", 0);
    }

    /**
     * Tests whether the current node has the specified dependency or not.
     *
     * @param name the name of the dependency.
     * @param features the features of the dependency (in a XIP-like format
     * String representation)
     * @return true if the node has the specified dependency, false otherwise.
     */
    public synchronized boolean hasDependency(String name, String features) {
        return this.hasDependency(name, features, 0);
    }

    /**
     * Tests whether the current node has the specified dependency or not.
     *
     * @param name the name of the dependency.
     * @param position the expected position of the current node if the
     * dependency arguments. (0 if any position is OK).
     * @return true if the node has the specified dependency, false otherwise.
     * @see #hasDependency(String name, String features, int position)
     */
    public synchronized boolean hasDependency(String name, int position) {
        return this.hasDependency(name, "", position);
    }

    /**
     * Tests whether the current node has the specified dependency or not.
     *
     * @param name the name of the dependency.
     * @param features the features of the dependency (in a XIP-like format
     * String representation)
     * @param position the expected position of the current node if the
     * dependency arguments. (0 if any position is OK).
     * @return true if the node has the specified dependency, false otherwise.
     */
    public synchronized boolean hasDependency(String name, String features, int position) {
        for (XipDependency dep : this.dependencies) {
            if (dep.getName().equalsIgnoreCase(name) && dep.hasFeatures(features)) {
                if (position == 0) {
                    return true;
                } else if (dep.getArg(position).equals(this)) {
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * Tests if the node has a given feature.
     *
     * @param name the name of the feature
     * @return true whether the node has the feature (with any value) false
     * otherwise
     */
    public synchronized boolean hasFeature(String name) {
        for (XipFeature feature : this.features) {
            if (feature.getName().equalsIgnoreCase(name)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Tests if the node has a given feature with a given value
     *
     * @param name the name of the feature
     * @param value the value of the feature ("*" if all values are fine) Value
     * can be negated ("~" or "~value")
     * @return true whether the node has the feature with the appropriate value
     * false otherwise
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
        for (XipFeature feature : this.getFeatures()) {
            if (feature.getName().equalsIgnoreCase(name)) {
                if (value.equals("*")
                        || feature.getValue().equalsIgnoreCase(value)) {
                    return !neg;
                }
            }
        }
        return neg;
    }

    /**
     * Tests if the node has given features. These features are separated by
     * ",", and the names are separated from values by ":". All is in XIP format
     *
     * @param features the list features in XIP format
     * @return true whether the node has the features with the appropriate value
     * false otherwise
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
     * Gets the features
     *
     * @return The features: a Vector of XipFeatures
     */
    public synchronized Vector<XipFeature> getFeatures() {
        return features;
    }

    /**
     * Sets the Vector of features
     *
     * @param nodeFeatures a Vector of XipFeature
     */
    public synchronized void setFeatures(Vector<XipFeature> nodeFeatures) {
        features = nodeFeatures;
    }

    /**
     * Gets the value of the specified feature
     *
     * @param name the name of the feature.
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
     * Gets the daughters
     *
     * @return The daughters: a Vector of XipNode
     */
    public synchronized Vector<XipNode> getDaughters() {
        return daughters;
    }

    /**
     * Adds a new daughter node
     *
     * @param daught The daugther XipNode to add
     */
    public synchronized void addDaughter(XipNode daught) {
        daughters.add(daught);
    }

    /**
     * Gets the mother XipNode of this node
     *
     * @return The mother XipNode
     */
    public synchronized XipNode getMother() {
        return mother;
    }

    /**
     * set the mother node
     *
     * @param moth the mother node
     */
    public synchronized void setMother(XipNode moth) {
        mother = moth;
    }

    /**
     * Gets the dependencies which involve this XipNode
     *
     * @return The dependencies: a Vector of XipDependency
     */
    public synchronized Vector<XipDependency> getDependencies() {
        return dependencies;
    }

    /**
     * Adds a XipDependency
     *
     * @param dependency The feature to be added to the Dependency attribute
     */
    public synchronized void addDependency(XipDependency dependency) {
        dependencies.add(dependency);
    }

    /**
     * return always XipNode.Node
     *
     * @return return always XipNode.Node
     */
    public synchronized int getType() {
        return NODE;
    }

    /**
     * Returns the value of the application-related information field of this
     * node.
     *
     * @return the value of the application-related information field of this
     * node.
     */
    public synchronized Object getMiscInfo() {
        return this.miscInfo;
    }

    /**
     * Sets the value of the application-related information field of this node.
     *
     * @param obj
     */
    public synchronized void setMiscInfo(Object obj) {
        this.miscInfo = obj;
    }

    /**
     * return false
     *
     * @return false
     */
    public synchronized boolean isLeaf() {
        return false;
    }

    /**
     * Checks if the feature list of this node matches the specified feature
     * list string
     *
     * @param featstring A string that represents the required feature list
     * @return true if the required feature list is matched by this node feature
     * list; false otherwise
     */
    public synchronized boolean flMatches(String featstring) {
        String[] feats = featstring.split(",");
        if (feats.length == 1 && feats[0].compareTo("") == 0) {
            return true;
        } else {
            String fv;
            String f;
            String v;
            String[] temp;
            for (int i = 0; i < feats.length; i++) {
                fv = feats[i];
                fv = fv.replace("!", "");
                temp = fv.split(":");
                f = temp[0];
                if (temp.length == 2) {
                    v = temp[1];
                } else {
                    v = "+";
                }
                if (!this.fMatches(f, v)) {
                    return false;
                }
            }
            return true;
        }
    }

    /**
     * Checks if the feature list of this node matches the specified
     * feature-value couple
     *
     * @param f A string that represents the feature name
     * @param v A string that represents the value that is required for the
     * feature
     * @return true if the required feature-value is matched by this node
     * feature list; false otherwise
     */
    public synchronized boolean fMatches(String f, String v) {
        Vector<XipFeature> feats = this.getFeatures();
        for (int i = 0; i < feats.size(); i++) {
            if (f.compareToIgnoreCase(feats.elementAt(i).getName()) == 0) {
                if (v.compareToIgnoreCase(feats.elementAt(i).getValue()) == 0) {
                    return true;
                } else {
                    return false;
                }
            }
        }
        if (v.contentEquals("~")) {
            return true;
        } else {
            return false;
        }
    }

    /**
     * Checks if the feature list of this node matches the specified feature
     * (the feature value that is assumed is {@code +})
     *
     * @param f A string that represents the name of the required feature
     * @return true if the required feature (with value {@code +}) is matched by
     * this node feature list; false otherwise
     */
    public synchronized boolean fMatches(String f) {
        return this.fMatches(f, "+");
    }

    /**
     * Gets the node that is a dependent of this node in the dependency relation
     * with the name {@code depname}. A node is a dependent in a dependency
     * relation if it is the second argument of the relation, and it is ahead in
     * the relation if it is the first argument of the relation.
     *
     * @param depname is the name of the required dependency relation
     * @return the dependent node if it exists; null if this node is not a head
     * in a dependency relation that has the specified name
     */
    public synchronized XipNode getDependentIn(String depname) {
        XipDependency d;
        Iterator<XipDependency> iter = this.getDependencies().iterator();
        while (iter.hasNext()) {
            d = iter.next();
            if (d.getParameters().size() == 2
                    && d.getArg(1) == this
                    && depname.contentEquals(d.getName())) {
                return d.getArg(2);
            }
        }
        return null;
    }

    /**
     * Gets the node that is a dependent of this node in the dependency relation
     * with the name {@code depname}. A node is a dependent in a dependency
     * relation if it is the second argument of the relation, and it is ahead in
     * the relation if it is the first argument of the relation.
     *
     * @param depname is the name of the required dependency relation
     * @return the dependent node if it exists; null if this node is not a head
     * in a dependency relation that has the specified name
     */
    public synchronized XipNode getDependentAhead(String depname) {
        for (XipDependency dep : this.getDependencies()) {
            if (dep.getParameters().size() == 2
                    && dep.getArg(2).equals(this)
                    && depname.equalsIgnoreCase(dep.getName())) {
                return dep.getArg(1);
            }
        }
        return null;
    }

    /**
     * Gets the node that is a dependent of this node in the dependency relation
     * with the name {@code depname}. A node is a dependent in a dependency
     * relation if it is the second argument of the relation, and it is ahead in
     * the relation if it is the first argument of the relation.
     *
     * @param depname is the name of the required dependency relation
     * @param nodefeats the feature list that the required dependent node must
     * satisfy
     * @return the dependent node if it exists; null if this node is not a head
     * in a dependency relation that has the specified name
     */
    public synchronized XipNode getDependentIn(String depname, String nodefeats) {
        XipDependency d;
        Iterator<XipDependency> iter = this.getDependencies().iterator();
        while (iter.hasNext()) {
            d = iter.next();
            if (d.getParameters().size() == 2
                    && d.getArg(1) == this
                    && depname.contentEquals(d.getName())
                    && d.getArg(2).flMatches(nodefeats)) {
                return d.getArg(2);
            }
        }
        return null;
    }

    /**
     * Gets the node that is a dependent of this node in the dependency relation
     * with the name {@code depname}. A node is a dependent in a dependency
     * relation if it is the second argument of the relation, and it is ahead in
     * the relation if it is the first argument of the relation.
     *
     * @param depname is the name of the required dependency relation
     * @param nodefeats the feature list that the required dependent node must
     * satisfy
     * @return the dependent node if it exists; null if this node is not a head
     * in a dependency relation that has the specified name
     */
    public synchronized XipNode getDependentAhead(String depname, String nodefeats) {
        for (XipDependency dep : this.getDependencies()) {
            if (dep.getParameters().size() == 2
                    && dep.getArg(2).equals(this)
                    && depname.toUpperCase().contentEquals(dep.getName())
                    && dep.getArg(1).hasFeatures(nodefeats)) {
                return dep.getArg(1);
            }
        }
        return null;
    }

    /**
     * Gets the node that is a dependent of this node in the dependency relation
     * with the name {@code depname}. A node is a dependent in a dependency
     * relation if it is the second argument of the relation, and it is ahead in
     * the relation if it is the first argument of the relation.
     *
     * @param depname is the name of the required dependency relation
     * @param depfeats the feature list that the required dependency must
     * satisfy
     * @param nodefeats the feature list that the required dependent node must
     * satisfy
     * @return the dependent node if it exists; null if this node is not a head
     * in a dependency relation that has the specified name
     */
    public synchronized XipNode getDependentAhead(String depname, String depfeats,
            String nodefeats) {
        for (XipDependency dep : this.getDependencies()) {
            if (dep.hasFeatures(depfeats)
                    && dep.getParameters().size() == 2
                    && dep.getArg(2).equals(this)
                    && depname.equalsIgnoreCase(dep.getName())
                    && dep.getArg(1).hasFeatures(nodefeats)) {
                return dep.getArg(1);
            }
        }
        return null;
    }

    /**
     * Checks if this node is a dependent in a dependency relation with the name {@code depname}.
     * A node is a dependent in a dependency relation if it is the second
     * argument of the relation, and it is ahead in the relation if it is the
     * first argument of the relation.
     *
     * @param depname is the name of the required dependency relation
     * @return true if this node is a dependent in a dependency relation with
     * the name {@code depname}; false otherwise
     */
    public synchronized boolean isDependentIn(String depname) {
        return this.getHeadIn(depname) != null;
    }

    /**
     * Checks if this node is dependent of a node {@code n} in a relation named
     *  {@code depname}.
     *
     * @param n the node for which this node would be a dependent
     * @param depname the name of the dependency relation
     * @return true if this node is dependent of node {@code n} in a relation
     * named
     * {@code depname}; false otherwise.
     */
    public synchronized boolean isRelDependentOf(XipNode n, String depname) {
        XipDependency d;
        boolean anyrel = depname.contentEquals("$ANY");
        Iterator<XipDependency> iter = this.getDependencies().iterator();
        while (iter.hasNext()) {
            d = iter.next();
            if (d.getParameters().size() == 2
                    && d.getArg(2) == this
                    && d.getArg(1) == n
                    && (anyrel || depname.contentEquals(d.getName()))) {
                return true;
            }
        }
        return false;
    }

    /**
     * Checks if this node is dependent of a node {@code n} in any dependency
     * relation.
     *
     * @param n the node for which this node would be dependent
     * @return true if this node is dependent of node {@code n} in a relation;
     * false otherwise.
     */
    public synchronized boolean isRelDependentOf(XipNode n) {
        return isRelHeadOf(n, "$ANY");
    }

    /**
     * Gets the node that is a head of this node in the dependency relation with
     * the name {@code depname}. A node is a dependent in a dependency relation
     * if it is the second argument of the relation, and it is a head in the
     * relation if it is the first argument of the relation.
     *
     * @param depname the name of the required dependency relation
     * @return the head node if it exists; null if this node is not a dependent
     * in a dependency relation that has the specified name
     */
    public synchronized XipNode getHeadIn(String depname) {
        XipDependency d;
        Iterator<XipDependency> iter = this.getDependencies().iterator();
        while (iter.hasNext()) {
            d = iter.next();
            if (d.getParameters().size() == 2
                    && d.getArg(2) == this
                    && depname.contentEquals(d.getName())) {
                return d.getArg(1);
            }
        }
        return null;
    }

    /**
     * Gets the node that is a head of this node in the dependency relation with
     * the name {@code depname}. A node is a dependent in a dependency relation
     * if it is the second argument of the relation, and it is a head in the
     * relation if it is the first argument of the relation.
     *
     * @param depname the name of the required dependency relation
     * @param nodefeats the feature list that the required dependent node must
     * satisfy
     * @return the head node if it exists; null if this node is not a dependent
     * in a dependency relation that has the specified name
     */
    public synchronized XipNode getHeadIn(String depname, String nodefeats) {
        XipDependency d;
        Iterator<XipDependency> iter = this.getDependencies().iterator();
        while (iter.hasNext()) {
            d = iter.next();
            if (d.getParameters().size() == 2
                    && d.getArg(2) == this
                    && depname.contentEquals(d.getName())
                    && d.getArg(1).flMatches(nodefeats)) {
                return d.getArg(1);
            }
        }
        return null;
    }

    /**
     * Checks if this node is a head in a dependency relation with the name {@code depname}.
     * A node is a dependent in a dependency relation if it is the second
     * argument of the relation, and it is a head in the relation if it is the
     * first argument of the relation.
     *
     * @param depname is the name of the required dependency relation
     * @return true if this node is a head in a dependency relation with the
     * name {@code depname}; false otherwise
     */
    public synchronized boolean isHeadIn(String depname) {
        return this.getDependentIn(depname) != null;
    }

    /**
     * Checks if this node is the head of a node {@code n} in a relation named
     *  {@code depname}.
     *
     * @param n the node for which this node would be the head
     * @param depname the name of the dependency relation
     * @return true if this node is the head of node {@code n} in a relation
     * named
     * {@code depname}; false otherwise.
     */
    public synchronized boolean isRelHeadOf(XipNode n, String depname) {
        XipDependency d;
        boolean anyrel = depname.contentEquals("$ANY");
        Iterator<XipDependency> iter = this.getDependencies().iterator();
        while (iter.hasNext()) {
            d = iter.next();
            if (d.getParameters().size() == 2
                    && d.getArg(1) == this
                    && d.getArg(2) == n
                    && (anyrel || depname.contentEquals(d.getName()))) {
                return true;
            }
        }
        return false;
    }

    /**
     * Checks if this node is the head of a node {@code n} in any dependency
     * relation.
     *
     * @param n the node for which this node would be the head
     * @return true if this node is the head of node {@code n} in a relation;
     * false otherwise.
     */
    public synchronized boolean isRelHeadOf(XipNode n) {
        return isRelHeadOf(n, "$ANY");
    }

    /**
     * return features data as a string
     *
     * @param withFeature if true build the string with the features otherwise
     * return empty string
     * @return features to string
     */
    public synchronized String featuresToString(boolean withFeature) {
        StringBuilder stringBuilder = new StringBuilder();
        if (withFeature) {
            stringBuilder.append("  [");
            boolean isFirst = true;
            for (Enumeration enumeration = features.elements(); enumeration.hasMoreElements();) {
                XipFeature feature = (XipFeature) enumeration.nextElement();
                if (!isFirst) {
                    stringBuilder.append(",");
                } else {
                    isFirst = false;
                }
                stringBuilder.append(feature.toString(withFeature));
            }
            stringBuilder.append("]");
        }
        return stringBuilder.toString();
    }

    /**
     * return dependencies data as a string
     *
     * @return dependencies to string
     */
    public synchronized String dependenciesToString() {
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append("   {");
        boolean isFirst = true;
        for (Enumeration enumeration = dependencies.elements(); enumeration.hasMoreElements();) {
            XipDependency dependency = (XipDependency) enumeration.nextElement();
            if (!isFirst) {
                stringBuilder.append(",");
            }
            stringBuilder.append(" ");
            stringBuilder.append(dependency.toString());
        }
        stringBuilder.append("}");
        return stringBuilder.toString();
    }

    /**
     * Gets the surface form of this node
     *
     * @return A string representing the surface form of this node
     */
    public synchronized String getSurface() {
        StringBuilder s = new StringBuilder();
        int i;
        long previousRight = -1;
        XipNode n = null;
        if (this.isLeaf()) {
            // XipNode.getSurface() is correct when the XipNode is a leaf
            return this.getSurface();
        }
        for (i = 0; i < this.getDaughters().size(); i++) {
            n = this.getDaughters().elementAt(i);
            if (previousRight >= 0 && n.getLeft() > previousRight) {
                s.append(" ");
            }
            if (n.isLeaf()) {
                // XipNode.getSurface() is correct when the XipNode is a leaf
                // return at first leaf because the surface of the mother node
                // is equal to the surface of any of its daughters when they're
                // leaves'
                return n.getSurface();
            } else if (n.isFicticious()) {
                break;
            } else {
                s.append(n.getSurface());
            }
            previousRight = n.getRight();
        }
        return s.toString();
    }

    /**
     * Gets the lemma (base form) of this node
     *
     * @return A string representing the lemma of this node
     */
    public synchronized String getLemma() {
        StringBuilder s = new StringBuilder();
        int i;
        long previousRight = -1;
        XipNode n = null;
        if (this.isLeaf()) {
            // XipNode.getLemma() is correct when the XipNode is a leaf
            return this.getLemma();
        }
        for (i = 0; i < this.getDaughters().size(); i++) {
            n = this.getDaughters().elementAt(i);
            if (previousRight >= 0 && n.getLeft() > previousRight) {
                s.append(" ");
            }
            if (n.isLeaf()) {
                // XipNode.getLemma() is correct when the XipNode is a leaf
                // return at first leaf because the lemma of the mother node
                // is equal to ONE of the lemmas of its daughters when they're
                // leaves (and for now, we arbitrary pick the lemma of the
                // first daughter leaf...)
                return n.getLemma();
            } else {
                s.append(n.getLemma());
            }
            previousRight = n.getRight();
        }
        return s.toString();
    }

    /**
     * Returns a Vector containing leaf nodes descendant of the current node.
     * Leaves that are only different interpretations of a same node are NOT
     * considered.
     *
     * @return a Vector of XipNodes corresponding to the tokenization part.
     */
    public synchronized Vector<XipNode> getLeafNodes() {
        Vector<XipNode> result = new Vector<XipNode>();
        boolean hasDaughters = false;
        for (XipNode node : this.daughters) {
            if (node.getGramIdNode() >= 0) {
                hasDaughters = true;
                result.addAll(node.getLeafNodes());
            }
        }
        if (!hasDaughters) {
            result.add(this);
        }
        return result;
    }

    /**
     * Gets the chunk head of this node
     *
     * @return The chunk head node of this node if this node is a chunk; this
     * node if this node is not a chunk (i.e. if it is preterminal or terminal
     * node)
     */
    public synchronized XipNode getChunkHead() {
        if (!this.isLeaf() && !this.isPreTerminalNode()) {
            return this.getDaughters().lastElement();
        } else {
            return this;
        }
    }

    /**
     * Checks if this node is ficticious (i.e. has not been directly created
     * from the input string but from derivational morphology rules)
     *
     * @return true if this node is ficticious; false otherwise
     */
    public synchronized boolean isFicticious() {
        if (this.getLeft() < 0) {
            return true;
        } else {
            return false;
        }
    }

    /**
     * Checks if this node is a chunk head (i.e. is the last daughter node of a
     * chunk). Hence the TOP node is not a chunk head.
     *
     * @return true if this node is a chunk head, false otherwise
     */
    public synchronized boolean isChunkHead() {
        if (this.getMother() != null && this.getMother().getDaughters().lastElement() == this) {
            return true;
        } else {
            return false;
        }
    }

    /**
     * Checks if this node is a preterminal node (i.e. it is the mother node of
     * terminal (leaf) nodes).
     *
     * @return true if this node is preterminal node, false otherwise
     */
    public synchronized boolean isPreTerminalNode() {
        if (!this.getDaughters().isEmpty() && (this.getDaughters().elementAt(0)).isLeaf()) {
            return true;
        } else {
            return false;
        }
    }

    /**
     * Returns a string representation of this node
     *
     * @return A string representation of this node
     */
    @Override
    public synchronized String toString() {
        return this.category;
    }

    /**
     *
     * @return
     */
    public synchronized String toString2() {
        if (this.getMiscInfo() == null) {
            return this.getId() + ":" + this.getCategory() + ":\""
                    + this.getSurface() + "\"\n" + this.toString(true, "--", true);
        } else {
            return this.getMiscInfo().toString();
        }
    }

    /**
     * return XipNode data as a string
     *
     * @param withFeature if true build the string with the XipNode features
     * @param tab tabulation
     * @param deeply if true, go deeply inside the daughter
     * @return XipNode to a string
     */
    public synchronized String toString(boolean withFeature, String tab, boolean deeply) {
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append(tab);
        stringBuilder.append(category);
        stringBuilder.append(" ");
        stringBuilder.append(index);
        stringBuilder.append(",");
        stringBuilder.append(gramIdNode);
        stringBuilder.append(" ");
        stringBuilder.append(getLeftTokOffset());
        stringBuilder.append("-");
        stringBuilder.append(getRightTokOffset());
        stringBuilder.append(" ");
        stringBuilder.append(" ");
        stringBuilder.append(getLeft());
        stringBuilder.append("-");
        stringBuilder.append(getRight());
        stringBuilder.append(" ");
        stringBuilder.append(getSurface());
        stringBuilder.append(":");
        stringBuilder.append(getLemma());
        stringBuilder.append(" ");
        stringBuilder.append(featuresToString(withFeature));
        stringBuilder.append(dependenciesToString());
        stringBuilder.append("\n");
        if (deeply) {
            boolean isFirst = true;
            for (Enumeration enumeration = daughters.elements(); enumeration.hasMoreElements();) {
                XipNode daughter = (XipNode) enumeration.nextElement();
                if (!isFirst) {
                    stringBuilder.append("\n");
                }
                stringBuilder.append(daughter.toString(withFeature, tab + "   ", deeply));
            }
        }
        return stringBuilder.toString();
    }

    /**
     * return XipNode data as a string
     *
     * @param withFeature if true build the string with the XipNode features
     * @param tab tabulation
     * @param deeply if true, go deeply inside the daughter
     * @return XipNode to a string
     */
    public synchronized String chunkTreeToString(boolean withFeature, String tab, boolean deeply) {
        return chunkTreeToString(withFeature, " ", tab, deeply);
    }

    /**
     *
     * @param withFeature
     * @param cumultab
     * @param tab
     * @param deeply
     * @return
     */
    public synchronized String chunkTreeToString(boolean withFeature, String cumultab,
            String tab, boolean deeply) {
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append(cumultab);
        stringBuilder.append(category);
        stringBuilder.append(" (");
        stringBuilder.append(index);
        stringBuilder.append(")");
        stringBuilder.append(" ");
        stringBuilder.append(getLeftTokOffset());
        stringBuilder.append("-");
        stringBuilder.append(getRightTokOffset());
        stringBuilder.append(" ");
        stringBuilder.append(getSurface());
        stringBuilder.append(":");
        stringBuilder.append(getLemma());
        stringBuilder.append(" ");
        stringBuilder.append(featuresToString(withFeature));
        stringBuilder.append("\n");
        if (deeply) {
            boolean isFirst = true;
            for (Enumeration enumeration = daughters.elements(); enumeration.hasMoreElements();) {
                XipNode daughter = (XipNode) enumeration.nextElement();
                if (!isFirst) {
                    stringBuilder.append("\n");
                }
                stringBuilder.append(daughter.chunkTreeToString(withFeature, cumultab + tab, tab, deeply));
            }
        }
        return stringBuilder.toString();
    }

    /**
     *
     * @param withFeature
     * @param deeply
     * @return
     */
    public synchronized String toHtmlString(boolean withFeature, boolean deeply) {
        String str = "";
        str += "<SPAN class=\"category\">" + category + "</SPAN>";
        str += " (<SPAN class=\"index\">" + index + "</SPAN>)";
        str += " " + getLeftTokOffset() + "-" + getRightTokOffset() + " ";
        str += "<SPAN class=\"surface\">" + getSurface() + "</SPAN>";
        str += ": <LEMMA>" + getLemma() + "</LEMMA> <BR>\n";
        if (withFeature) {
            str += "<SPAN class=\"features\">"
                    + featuresToString(withFeature).toLowerCase() + "</SPAN> <BR>\n";
        }
        for (int i = 0; i < this.getDependencies().size(); i++) {
            XipDependency d = this.getDependencies().elementAt(i);
            str += "<DEPENDENCY>" + "<SPAN class=\"depname\">" + d.getName() + "</SPAN>(";
            if (d.getParameters().size() > 1) {
                for (int j = 0; j < d.getParameters().size(); j++) {
                    XipNode p = d.getParameters().elementAt(j);
                    if (p == this) {
                        str += "<SPAN class=\"indexindep\">" + "#" + "</SPAN>";
                    } else {
                        str += "<SPAN class=\"indexindep\">" + p.getIndex() + "</SPAN>:";
                        str += "<SPAN class=\"surfaceindep\">" + p.getSurface() + "</SPAN>";
                    }
                    if ((j + 1) < d.getParameters().size()) {
                        str += ", ";
                    }
                }
            }
            str += ")</DEPENDENCY>, \n";
        }
        if (deeply && !daughters.isEmpty() && (!daughters.elementAt(0).isLeaf())) {
            boolean isFirst = true;
            str += "<ul>\n";
            for (Enumeration enumeration = daughters.elements(); enumeration.hasMoreElements();) {
                XipNode daughter = (XipNode) enumeration.nextElement();
                str += "<li>\n" + daughter.toHtmlString(withFeature, deeply) + "</li>\n";
            }
            str += "</ul>\n";
        }
        return str;
    }
}
