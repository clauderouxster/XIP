/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 ****************************************************************************************
 * Copyright (c) 2006 Xerox Corporation                                                 *
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
package com.xerox.xrce.xipui.ide.generator.ui;

import com.xerox.xrce.xipui.ide.generator.CommonMethod;
import com.xerox.xrce.xipui.ui.graph.*;

import com.xerox.xrce.xipui.modules.ModuleContext;
import java.util.*;
import com.xerox.jxip.*;

/**
 *
 * @author csaillet
 */
public class AddRule extends GeneralPanel {

    /** Creates a new instance of AddRule
     * @param context
     * @param id 
     */
    public AddRule(ModuleContext context, int id) {
        super(context, id);
        initComponents();
        setPageDoc("/com/xerox/xrce/lingdexip/generateRules/doc/chooseOutput.htm");
        setProperty("panel_", this);
    }

    private void initComponents() {
        String typeRule = (String) this.getProperty("choiceRule");
        choiceRuleBox.setSelectedItem(typeRule);
        if (typeRule.equals("Dependency")) {
            String dependency = (String) this.getProperty("dependency");
            dependencyBox.setSelectedItem(dependency);
        } else if (typeRule.equals("Sequence") || typeRule.equals("ID Rule") || typeRule.equals("Term")) {
            String choiceCategory = (String) this.getProperty("category");
            choiceCategoryBox.setSelectedItem(choiceCategory);
        }
    }

    public void initPanel() {
        String typeRule = (String) this.getProperty("choiceRule");
        ruleLabel.setEnabled(false);
        choiceRuleBox.setEnabled(false);
        String longestmatch = (String) this.getProperty("longestMatch");
        if (longestmatch.equals("true")) {
            longestBox.setSelected(true);
        } else {
            longestBox.setSelected(false);
        }
        String indlastwrd = (String) this.getProperty("indexlastword");
        if (indlastwrd.equals("true")) {
            indexBox.setSelected(true);
        } else {
            indexBox.setSelected(false);
        }
        if (typeRule.equals("Dependency")) {
            dependencyLabel.setEnabled(false);
            dependencyBox.setEnabled(false);
            categoryLabel.setVisible(false);
            choiceCategoryBox.setVisible(false);
            longestBox.setVisible(false);
            indexBox.setVisible(false);
        } else if (typeRule.equals("Sequence")) {
            categoryLabel.setEnabled(false);
            choiceCategoryBox.setEnabled(false);
            longestBox.setEnabled(false);
            dependencyLabel.setVisible(false);
            dependencyBox.setVisible(false);
            indexBox.setVisible(false);
        } else if (typeRule.equals("ID Rule")) {
            categoryLabel.setEnabled(false);
            choiceCategoryBox.setEnabled(false);
            dependencyLabel.setVisible(false);
            dependencyBox.setVisible(false);
            longestBox.setVisible(false);
            indexBox.setVisible(false);
        } else if (typeRule.equals("Term")) {
            categoryLabel.setEnabled(false);
            choiceCategoryBox.setEnabled(false);
            longestBox.setEnabled(false);
            indexBox.setEnabled(false);
            dependencyLabel.setVisible(false);
            dependencyBox.setVisible(false);
        } else if (typeRule.equals("Tagging") || typeRule.equals("Marking")) {
            categoryLabel.setVisible(false);
            choiceCategoryBox.setVisible(false);
            dependencyLabel.setVisible(false);
            dependencyBox.setVisible(false);
            longestBox.setVisible(false);
            indexBox.setVisible(false);
        }
        addRuleButton.setVisible(false);
        clearTempFileButton.setVisible(false);
        clearButton.setEnabled(false);
        nextButton.setEnabled(false);
        String rule = (String) this.getProperty("rule");
        jTextArea2.setText(rule);
        jTextArea2.setToolTipText(rule);
    }

    public void next() {
    }

    public void clear() {
    }

    public void close() {
    }

    public void back() {
        String typeRule = (String) this.getProperty("choiceRule");
        if (typeRule.equals("Dependency")) {
            FocusPanel focus = new FocusPanel(getModuleContext(), id);
            focus.focusP();
        } else if (typeRule.equals("Sequence") || typeRule.equals("Term")) {
            IndexLastWordPanel indexWord = new IndexLastWordPanel(getModuleContext(), id);
            indexWord.indexWordP();
        } else if (typeRule.equals("ID Rule") || typeRule.equals("Tagging")) {
            RightContextPanel right = new RightContextPanel(getModuleContext(), id);
            right.rightContP();
        } else if (typeRule.equals("Marking")) {
            MarkingPanel mark = new MarkingPanel(getModuleContext(), id);
            mark.markingP();
        }
    }

    public void indexWord() {
    }

    public void lonMatch() {
    }

    public void generateRule() {
        CommonMethod cmethod = new CommonMethod();
        SessionGraph session = getModuleContext().getSessionGraph(false, id);
        Vector v = session.getNodeGraphSelected();
        Vector selectedFeatures = new Vector();
        if (v != null) {
            Iterator ite = v.iterator();
            while (ite.hasNext()) {
                NodeGraph ng = (NodeGraph) ite.next();
                selectedFeatures.add(ng);
            }
        }
        Vector vecAddRule = cmethod.vectNodeGraphToSelectedObject(getModuleContext(), id, selectedFeatures);
        int sizeTot;
        String choiceRuleType = (String) this.getProperty("choiceRule");
        Vector vecDependency = (Vector) this.getProperty("vectDependencySelect");
        Vector vecBody = (Vector) this.getProperty("vectBodySelect");
        Vector vecLeft = (Vector) this.getProperty("vectLeftSelect");
        Vector vecRight = (Vector) this.getProperty("vectRightSelect");
        Vector vecHead = (Vector) this.getProperty("vectHeadSelect");
        Vector vecMark = (Vector) this.getProperty("vectMarkSelect");
        Vector veckeepselect = (Vector) this.getProperty("vectKeepSelect");
        Vector veckselect = new Vector(veckeepselect);
        Vector veckeep = cmethod.subVector(veckselect, vecHead);
        sizeTot = vecDependency.size() + vecBody.size() + vecLeft.size() + vecRight.size() + vecHead.size() + veckeep.size() + vecMark.size();
        // si un mauvais noeud a ete selectionne alors deselection
        if ((v.size()) > sizeTot) {
            warning("Wrong choice !");
            long index = ((NodeGraph) v.lastElement()).getIndex();
            if (((NodeGraph) v.lastElement()).getSelectedObject().getObject() instanceof XipNode) {
                session.unSelectXipNode(index);
            } else if (((NodeGraph) v.lastElement()).getSelectedObject().getObject() instanceof XipDependency) {
                session.unSelectXipDependency(index);
            }
        } else { // si un noeud a ete deselectionne alors reselection
            //sequence ou DI ou term
            if (choiceRuleType.equals("Sequence") || choiceRuleType.equals("ID Rule") || choiceRuleType.equals("Term")) {
                long res = cmethod.verifyNodeSelection(vecAddRule, vecBody);
                if (res != -1) {
                    warning("Wrong choice !");
                    session.selectXipNode(res);
                }
                long left = cmethod.verifyNodeSelection(vecAddRule, vecLeft);
                if (left != -1) {
                    warning("Wrong choice !");
                    session.selectXipNode(left);
                }
                long right = cmethod.verifyNodeSelection(vecAddRule, vecRight);
                if (right != -1) {
                    warning("Wrong choice !");
                    session.selectXipNode(right);
                }
            } else if (choiceRuleType.equals("Dependency")) { // dependence
                long depend = cmethod.verifyNodeSelection(vecAddRule, vecDependency);
                if (depend != -1) {
                    warning("Wrong choice !");
                    if (session.getXipDependency(depend) != null) {
                        session.selectXipDependency(depend);
                    } else if (session.getXipNode(depend) != null) {
                        session.selectXipNode(depend);
                    }
                }
            } else if (choiceRuleType.equals("Marking")) { // marquage
                long mark = cmethod.verifyNodeSelection(vecAddRule, vecMark);
                if (mark != -1) {
                    warning("Wrong choice !");
                    session.selectXipNode(mark);
                }
            } else if (choiceRuleType.equals("Tagging")) { // tagging
                long head = cmethod.verifyNodeSelection(vecAddRule, vecHead);
                if (head != -1) {
                    warning("Wrong choice !");
                    session.selectXipNode(head);
                }
                long left = cmethod.verifyNodeSelection(vecAddRule, vecLeft);
                if (left != -1) {
                    warning("Wrong choice !");
                    session.selectXipNode(left);
                }
                long right = cmethod.verifyNodeSelection(vecAddRule, vecRight);
                if (right != -1) {
                    warning("Wrong choice !");
                    session.selectXipNode(right);
                }
                if (veckeep.size() != 0) {
                    long keep = cmethod.verifyNodeSelection(vecAddRule, veckeep);
                    if (keep != -1) {
                        warning("Wrong choice !");
                        session.selectXipNode(keep);
                    }
                }
            } else {
                DEBUG_LOG.trace(" type of rule unknown !");
            }
        }
    }

    public void addRule() {
        this.setProperty("panel", "addRule");
        AddRule addR = new AddRule(getModuleContext(), id);
        this.refreshTabbedPane(addR);
    }
}
