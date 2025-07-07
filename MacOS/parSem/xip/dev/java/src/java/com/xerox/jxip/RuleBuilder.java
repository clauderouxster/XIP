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
 * The purpose of this class is to build a Xip rule
 *
 * @author grondeau @created May 19, 2005
 */
public class RuleBuilder {

    private Vector<String> categoryDependencies;

    /**
     * Constructor
     */
    public RuleBuilder() {
    }

    /**
     * Gets the categoryDependencies attribute of the RuleBuilder object
     *
     * @return The categoryDependencies value
     */
    public synchronized Vector<String> getCategoryDependencies() {
        return categoryDependencies;
    }

    /**
     * Sort the selectedObjects as requested by the rule generator
     *
     * @param selectedObjects
     * @return
     */
    private Vector<SelectedObject> sort(Vector<SelectedObject> selectedObjects) {
        Vector<SelectedObject> objOrdered = new Vector<SelectedObject>();
        //XipDependency selected Objects
        Vector<SelectedObject> dep = new Vector<SelectedObject>();
        long maxIndex = 0;
        // get the greater value of the index of the selected XipNode
        for (Enumeration enumeration = selectedObjects.elements(); enumeration.hasMoreElements();) {
            SelectedObject selectedObject = (SelectedObject) enumeration.nextElement();
            java.lang.Object obj = selectedObject.getObject();
            if (obj.getClass().getName().compareTo("com.xerox.jxip.XipNode") == 0) {
                XipNode xipNode = (XipNode) obj;
                //System.out.println("RuleBuilder::sort() XipNode category = "
                // + xipNode.getCategory() + " index = " + xipNode.getIndex());
                if (xipNode.getIndex() > maxIndex) {
                    maxIndex = xipNode.getIndex();
                }
            } else if (obj.getClass().getName().compareTo("com.xerox.jxip.XipLeaf") == 0) {
                XipLeaf xipLeaf = (XipLeaf) obj;
                //System.out.println("RuleBuilder::sort() XipLeaf category = "
                // + xipLeaf.getCategory() + " index = " + xipLeaf.getIndex());
                if (xipLeaf.getIndex() > maxIndex) {
                    maxIndex = xipLeaf.getIndex();
                }
            }
        }
        // the index start to 1, so it's mandatory to add 1 to maxIndex
        SelectedObject[] objArray = new SelectedObject[(int) maxIndex + 1];
        //System.out.println("RuleBuilder::sort() objArray.length = " + objArray.length);
        for (Enumeration enumeration = selectedObjects.elements(); enumeration.hasMoreElements();) {
            SelectedObject selectedObject = (SelectedObject) enumeration.nextElement();
            java.lang.Object obj = selectedObject.getObject();
            if (obj.getClass().getName().compareTo("com.xerox.jxip.XipDependency") == 0) {
                XipDependency xipDependency = (XipDependency) obj;
                dep.add(selectedObject);
                //System.out.println("RuleBuilder::sort() XipDependency name = "
                // + xipDependency.getName() + " index = " + xipDependency.getIndex());
            } else if (obj.getClass().getName().compareTo("com.xerox.jxip.XipNode") == 0) {
                XipNode xipNode = (XipNode) obj;
                objArray[(int) xipNode.getIndex()] = selectedObject;
                //System.out.println("RuleBuilder::sort() XipNode category = "
                // + xipNode.getCategory() + " index = " + xipNode.getIndex());
            } else if (obj.getClass().getName().compareTo("com.xerox.jxip.XipLeaf") == 0) {
                XipLeaf xipLeaf = (XipLeaf) obj;
                //System.out.println("RuleBuilder::sort() XipLeaf category = "
                // + xipLeaf.getCategory() + " index = " + xipLeaf.getIndex());
                objArray[(int) xipLeaf.getIndex()] = selectedObject;
            }
        }
        // build the ordered vector
        for (int ind = 0; ind < objArray.length; ind++) {
            //System.out.println("RuleBuilder::sort() ind = " + ind);
            if (objArray[ind] != null) {
                objOrdered.add(objArray[ind]);
            }
        }
        // add the dependencies at the end(not ordered)
        for (Enumeration enumeration = dep.elements(); enumeration.hasMoreElements();) {
            objOrdered.add((SelectedObject) enumeration.nextElement());
        }
        // just to display
        /*
         * for (Enumeration enumeration = objOrdered.elements();
         * enumeration.hasMoreElements(); ) { SelectedObject selectedObject =
         * (SelectedObject) enumeration.nextElement(); java.lang.Object obj =
         * selectedObject.getObject(); if
         * (obj.getClass().getName().compareTo("com.xerox.jxip.XipDependency")
         * == 0) { XipDependency xipDependency = (XipDependency) obj;
         * System.out.println("RuleBuilder::sort() XipDependency name = " +
         * xipDependency.getName() + " index = " + xipDependency.getIndex()); }
         * else if (obj.getClass().getName().compareTo("com.xerox.jxip.XipNode")
         * == 0) { XipNode xipNode = (XipNode) obj;
         * System.out.println("RuleBuilder::sort() XipNode category = " +
         * xipNode.getCategory() + " index = " + xipNode.getIndex()); } else if
         * (obj.getClass().getName().compareTo("com.xerox.jxip.XipLeaf") == 0) {
         * XipLeaf xipLeaf = (XipLeaf) obj;
         * System.out.println("RuleBuilder::sort() XipLeaf category = " +
         * xipLeaf.getCategory() + " index = " + xipLeaf.getIndex()); } }
         */
        return objOrdered;
    }

    /**
     * Generate a xip rule
     *
     * @param focus can be null, or Vector of Long (index)
     * @param ruleType the type of rule
     * @param boundaryIndex
     * @param category
     * @param dependencyArity
     * @param selectedObjs
     * @return the rule
     * @exception Exception
     */
    public synchronized String buildRule(Vector<SelectedObject> selectedObjs,
            Vector<Long> focus, long boundaryIndex, String category, int ruleType,
            int dependencyArity) throws Exception {
        String ruleString;
        Vector selectedObjects = sort(selectedObjs);
        categoryDependencies = new Vector<String>();
        clearGlobalsImplementation();
        for (Enumeration enumeration = selectedObjects.elements(); enumeration.hasMoreElements();) {
            SelectedObject selectedObject = (SelectedObject) enumeration.nextElement();
            java.lang.Object obj = selectedObject.getObject();
            boolean any = selectedObject.getAny();
            Vector selectedFeatures = selectedObject.getSelectedFeatures();
            if (obj.getClass().getName().compareTo("com.xerox.jxip.XipDependency") == 0) {
                XipDependency xipDependency = (XipDependency) obj;
                addSelectedXipDependencyImplementation(xipDependency.getIndex());
                for (Enumeration enumeration1 = selectedFeatures.elements(); enumeration1.hasMoreElements();) {
                    XipFeature xipFeature = (XipFeature) enumeration1.nextElement();
                    addSelectedFeatureToSelectedDependencyImplementation(xipDependency.getIndex(),
                            xipFeature.getIndex());
                }
            } else if (obj.getClass().getName().compareTo("com.xerox.jxip.XipNode") == 0) {
                XipNode xipNode = (XipNode) obj;
                //System.out.println("RuleBuilder::buildRule() XipNode category = "
                // + xipNode.getCategory() + " index = " + xipNode.getIndex());
                addSelectedXipNodeImplementation(xipNode.getIndex(), any);
                for (Enumeration enumeration2 = selectedFeatures.elements(); enumeration2.hasMoreElements();) {
                    XipFeature xipFeature = (XipFeature) enumeration2.nextElement();
                    //System.out.println("RuleBuilder::buildRule() XipFeature name = "
                    // + xipFeature.getName() + " index = " + xipFeature.getIndex());
                    //changement effectue par Vianney et Claude le 21/06/05
                    //addSelectedFeatureToSelectedNodeImplementation(xipNode.getIndex(), xipFeature.getIndex());
                    addSelectedFeatureStringToSelectedNodeImplementation(xipFeature.getName(), xipFeature.getValue());
                }
            } else if (obj.getClass().getName().compareTo("com.xerox.jxip.XipLeaf") == 0) {
                XipLeaf xipLeaf = (XipLeaf) obj;
                //System.out.println("RuleBuilder::buildRule() XipLeaf category = "
                // + xipLeaf.getCategory() + " index = " + xipLeaf.getIndex());
                addSelectedXipLeafImplementation(xipLeaf.getIndex(), any);
                for (Enumeration enumeration3 = selectedFeatures.elements(); enumeration3.hasMoreElements();) {
                    XipFeature xipFeature = (XipFeature) enumeration3.nextElement();
                    addSelectedFeatureToSelectedNodeImplementation(xipLeaf.getIndex(), xipFeature.getIndex());
                }
            } else {
                System.out.println("RuleBuilder::buildRule() Error class name unknown");
            }
        }
        if (focus != null) {
            for (Enumeration enumeration = focus.elements(); enumeration.hasMoreElements();) {
                long index = ((Long) enumeration.nextElement()).longValue();
                //System.out.println("RuleBuilder::buildRule() focus index = " + index);
                addFocusImplementation(index);
            }
        }
        ruleString = buildRuleImplementation(boundaryIndex, category, ruleType, dependencyArity);
        return ruleString;
    }

    /**
     * call back for buildRuleImplementation: build a vector of
     * categoryDependency associated with this rule during buildRule call
     *
     * @param catDep a category dependency
     */
    private void addCategoryDependency(String catDep) {
        //System.out.println("RuleBuilder::addCategoryDependency() categoryDependency = " + catDep);
        categoryDependencies.add(catDep);
    }

    /**
     * @param gHandler
     * @param fileName
     * @param categoryDependencies
     */
    public synchronized void generateRuleCategory(int gHandler, String fileName,
            Vector<String> categoryDependencies) {
        //System.out.println("RuleBuilder::generateRuleCategory() gHandler = " + gHandler + " fileName = " + fileName);
        for (Enumeration enumeration = categoryDependencies.elements(); enumeration.hasMoreElements();) {
            String catDep = (String) enumeration.nextElement();
            addCategoryDependencyImplementation(catDep);
        }
        generateRuleCategoryImplementation(gHandler, fileName);
    }

    private native void addCategoryDependencyImplementation(String catDep);

    private native int generateRuleCategoryImplementation(int gHandler, String fileName);

    private native void clearGlobalsImplementation();

    private native void addSelectedFeatureToSelectedNodeImplementation(
            long indSelectNode, long indSelectFeature);

    private native void addSelectedFeatureStringToSelectedNodeImplementation(
            String featureAttribute, String featureValue);

    private native void addSelectedFeatureToSelectedDependencyImplementation(
            long indSelectDependency, long indSelectFeature);

    private native void addSelectedXipNodeImplementation(long index, boolean any);

    private native void addSelectedXipLeafImplementation(long index, boolean any);

    private native void addSelectedXipDependencyImplementation(long index);

    private native void addFocusImplementation(long index);

    private native String buildRuleImplementation(long boundaryIndex, String category, int ruleType,
            int dependencyArity) throws Exception;
    //______________________________ test -----------------------------

    /**
     * Select a set of xipNode
     *
     * @param node
     * @param selectedObjects
     */
    private void selectXipNodes(XipNode node, Vector<SelectedObject> selectedObjects) {
        //System.out.println("RuleBuilder::selectXipNodes() node: " + node.getCategory());
        SelectedObject selectedObject = new SelectedObject(node);
        selectedObject.setAny(false);
        // select all the features of this node
        for (Enumeration enumeration = (node.getFeatures()).elements(); enumeration.hasMoreElements();) {
            XipFeature xipFeature = (XipFeature) enumeration.nextElement();
            selectedObject.addSelectedFeature(xipFeature);
        }
        selectedObjects.add(selectedObject);
        // process recursively the daughters
        for (Enumeration enumeration = (node.getDaughters()).elements(); enumeration.hasMoreElements();) {
            XipNode daughter = (XipNode) enumeration.nextElement();
            selectXipNodes(daughter, selectedObjects);
        }
    }

    /**
     * Test
     *
     * @param args
     */
    public static void main(String[] args) {
        // Vector of SelectedObject
        Vector<SelectedObject> selectedObjects = new Vector<SelectedObject>();
        try {
            if (args.length != 2) {
                System.out.println("Two parameters are required: the fullpath of the grm file, and the string to parse");
                System.exit(-1);
            }
            System.out.println("RuleBuilder::main() Loading grammar file: " + args[0]);
            System.out.println("RuleBuilder::main() parsing the string = " + args[1]);
            // create a JXIP object to load the dynamic library in the VM
            JXip jxip = new JXip();
            XipResultManager xipResultManager = new XipResultManager();
            int gHandler = jxip.XipGrmFile(args[0], 0, true);
            int res = xipResultManager.buildXipResult(gHandler, args[1]);
            if (res != 0) {
                System.out.println("RuleBuilder::main() buildXipResult ERROR");
                System.exit(-1);
            }
            XipResult xipRes = xipResultManager.getXipResult();
            RuleBuilder ruleBuilder = new RuleBuilder();
            // build a vector of selected object
            for (Enumeration enumeration = (xipRes.getXipUnits()).elements(); enumeration.hasMoreElements();) {
                XipUnit xipUnit = (XipUnit) enumeration.nextElement();
                XipNode rootNode = xipUnit.getRoot();
                // select the xipNodes and xipLeaves with all their features
                ruleBuilder.selectXipNodes(rootNode, selectedObjects);
                // select the dependencies with all their features
                for (Enumeration enumeration1 = (xipUnit.getDependencies()).elements(); enumeration1.hasMoreElements();) {
                    XipDependency dependency = (XipDependency) enumeration1.nextElement();
                    SelectedObject selectedObject = new SelectedObject(dependency);
                    // select all the features of this dependency
                    for (Enumeration enumeration2 = (dependency.getFeatures()).elements(); enumeration2.hasMoreElements();) {
                        XipFeature xipFeature = (XipFeature) enumeration2.nextElement();
                        selectedObject.addSelectedFeature(xipFeature);
                    }
                    selectedObjects.add(selectedObject);
                }
            }

            Vector<Long> focus = new Vector<Long>();
            focus.add(new Long(2));
            focus.add(new Long(5));

            int ruleType;
            ruleType = Const.DEPENDENCYRULE;
            //ruleType = Const.SEQUENCERULE;
            //ruleType = Const.LEFTCONTEXT;
            //ruleType = Const.RIGHTCONTEXT;
            //ruleType = Const.IDRULE;
            //ruleType = Const.TAGGINGRULE;
            int dependencyArity = 2;
            long boundaryIndex = 3;
            String category = "focus";
            //generate the rule String
            String ruleString = "";
            ruleString = ruleBuilder.buildRule(selectedObjects, focus, boundaryIndex, category, ruleType, dependencyArity);
            if (ruleString != null) {
                System.out.println("RuleBuilder::main() rule :\n" + ruleString);
            } else {
                System.out.println("RuleBuilder::main() rule generation error");
            }

            String fileName = "cat";
            Vector<String> categoryDependencies = new Vector<String>();
            ruleBuilder.generateRuleCategory(gHandler, fileName, categoryDependencies);
        } catch (Exception exception) {
            System.out.println("RuleBuilder::main() " + exception.toString());
            System.exit(-1);
        }
    }
}
