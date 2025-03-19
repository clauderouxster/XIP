/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 *  Copyright (c) 2005 Xerox Corporation                                                 *
 *  Xerox Research Centre Europe - Grenoble                                              *
 *  All Rights Reserved                                                                  *
 *  Copyright protection claimed includes all forms and matters of copyrightable         *
 *  material and information now allowed by statutory or judicial law or                 *
 *  hereinafter granted, including without limitation, material generated from           *
 *  the software programs which are displayed on the screen such as icons,               *
 *  screen display looks, etc. All rights reserved. No part of this document             *
 *  may be reproduced, stored in a retrieval system, or transmitted, in any              *
 *  form or by any means, electronic, mechanical, photocopied, recorded, or              *
 *  otherwise, without prior written consent from XEROX corporation.                     *
 *
 */
package com.xerox.xrce.xipui.ui.graph;

import com.xerox.jxip.*;
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.RuleMap;
import com.xerox.xrce.xipui.XipUtilities;
import com.xerox.xrce.xipui.modules.IModule;
import com.xerox.xrce.xipui.ui.editor.EditFileAction;

import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.modules.PanelModule;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

/**
 *
 * @author  vgrassau
 */
public class NodeGraph extends PanelModule {

    private EditFileAction editFileAction;
    private SelectedObject node;
    private Vector<XipFeature> features;
    private Vector<XipDependency> dependencies;
    private int id;

    /** Creates new form NodeGraph
     * @param properties
     * @param id
     * @param obj 
     */
    public NodeGraph(ModuleContext properties, int id, Object obj) {
        super(properties);
        this.editFileAction = new EditFileAction(getModuleContext());
        this.editFileAction.setType(EditFileAction.GRAMMAR_EDITOR);
        this.editFileAction.putValue(EditFileAction.ICON, XIPUIUtilities.getIcon(UIConstants.CODE));
        this.id = id;
        this.node = new SelectedObject(obj);
        this.features = new Vector();
        this.dependencies = new Vector();
        initComponents();
        this.labelDependency.setIcon(XIPUIUtilities.getIcon(UIConstants.DEPENDENCIES_LIST));
        this.labelFeatures.setIcon(XIPUIUtilities.getIcon(UIConstants.FEATURES_LIST));
        setFeatures();
        addFeaturesToPanel();
        addLemmaSurfaceFeatures();
        addDependenciesToPanel();
    }

    public String toString() {
        return getName();
    }

    public void close() {
        removeAll();
        this.isClosed = true;
    }

    public String getName() {
        if (isXipNode()) {
            XipNode n = (XipNode) node.getObject();
            return n.getCategory() + "#" + getIndex();
        } else {
            XipDependency d = (XipDependency) node.getObject();
            return d.getName() + "#" + getIndex();
        }
    }

    public long getIndex() {
        Object o = node.getObject();
        if (o instanceof XipNode) {
            return ((XipNode) o).getIndex();
        }
        if (o instanceof XipDependency) {
            return ((XipDependency) o).getIndex();
        }
        return -1;
    }

    public boolean isXipNode() {
        Object o = node.getObject();
        return o instanceof XipNode;
    }

    public boolean isXipDependency() {
        Object o = node.getObject();
        return o instanceof XipDependency;
    }

    private void setFeatures() {
        if (isXipDependency()) {
            XipDependency dependency = (XipDependency) node.getObject();
            this.features = dependency.getFeatures();
        } else if (isXipNode()) {
            XipNode xipNode = (XipNode) node.getObject();
            this.features = xipNode.getFeatures();
            this.dependencies = xipNode.getDependencies();
            int size = features.size();
            if (xipNode.isLeaf()) {
                XipLeaf leaf = (XipLeaf) xipNode;
                XipFeature surfaceFeature = new XipFeature("surface", leaf.getSurface(), size);
                XipFeature lemmaFeature = new XipFeature("lemme", leaf.getLemma(), size + 1);
                if (!containsFeature(surfaceFeature)) {
                    features.add(surfaceFeature);
                }
                if (!containsFeature(lemmaFeature)) {
                    features.add(lemmaFeature);
                }
            }
        }
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc=" Generated Code ">//GEN-BEGIN:initComponents
    private void initComponents() {
        java.awt.GridBagConstraints gridBagConstraints;

        labelFeatures = new javax.swing.JLabel();
        featuresPane = new javax.swing.JPanel();
        labelDependency = new javax.swing.JLabel();
        DependencyPane = new javax.swing.JPanel();

        setLayout(new java.awt.GridBagLayout());

        labelFeatures.setBackground(new java.awt.Color(153, 255, 153));
        labelFeatures.setFont(new java.awt.Font("Arial", 0, 11));
        labelFeatures.setText("Features");
        labelFeatures.setBorder(javax.swing.BorderFactory.createMatteBorder(0, 0, 2, 0, new java.awt.Color(255, 102, 51)));
        labelFeatures.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        add(labelFeatures, gridBagConstraints);

        featuresPane.setLayout(new javax.swing.BoxLayout(featuresPane, javax.swing.BoxLayout.Y_AXIS));

        featuresPane.setBackground(new java.awt.Color(255, 255, 255));
        featuresPane.setFont(new java.awt.Font("Arial", 0, 11));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        add(featuresPane, gridBagConstraints);

        labelDependency.setBackground(new java.awt.Color(153, 255, 153));
        labelDependency.setFont(new java.awt.Font("Arial", 0, 11));
        labelDependency.setText("Dependencies");
        labelDependency.setBorder(javax.swing.BorderFactory.createMatteBorder(0, 0, 2, 0, new java.awt.Color(255, 102, 51)));
        labelDependency.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        add(labelDependency, gridBagConstraints);

        DependencyPane.setLayout(new javax.swing.BoxLayout(DependencyPane, javax.swing.BoxLayout.Y_AXIS));

        DependencyPane.setBackground(new java.awt.Color(255, 255, 255));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        add(DependencyPane, gridBagConstraints);

    }// </editor-fold>//GEN-END:initComponents

    private void addFeaturesToPanel() {
        Iterator ite = features.iterator();
        while (ite.hasNext()) {
            this.featuresPane.add(createCheckFeature((XipFeature) ite.next()));
        }
    }

    private void addDependenciesToPanel() {
        Iterator ite = dependencies.iterator();
        while (ite.hasNext()) {
            this.DependencyPane.add(createJLabelDependency((XipDependency) ite.next()));
        }
    }

    private void addLemmaSurfaceFeatures() {
        if (isXipNode()) { //on va afficher les traits et les dependances
            XipNode xipNode = (XipNode) node.getObject();
            Iterator ite2 = xipNode.getDaughters().iterator();
            while (ite2.hasNext()) {
                Object o = ite2.next();
                if (o instanceof XipLeaf) {
                    XipLeaf leaf = (XipLeaf) o;
                    int size = features.size();
                    XipFeature surfaceFeature = new XipFeature("surface", leaf.getSurface(), size);
                    XipFeature lemmaFeature = new XipFeature("lemme", leaf.getLemma(), size + 1);
                    if (!containsFeature(surfaceFeature)) {
                        features.add(surfaceFeature);
                        this.featuresPane.add(createCheckFeature(surfaceFeature));
                    }
                    if (!containsFeature(lemmaFeature)) {
                        features.add(lemmaFeature);
                        this.featuresPane.add(createCheckFeature(lemmaFeature));
                    }
                }
            }
        }
    }

    public JComponent createCheckFeature(final XipFeature feature) {
        String feat_ = feature.getName() + ":" + feature.getValue();
        if (!feature.getName().equalsIgnoreCase("#rule")) {
            JCheckBox featureCheck = new JCheckBox(feat_);
            featureCheck.setBackground(Color.WHITE);
            featureCheck.setForeground(Color.black);
            featureCheck.setFont(UIConstants.DEFAULT_FONT);
            featureCheck.setSelectedIcon(XIPUIUtilities.getIcon(UIConstants.SELECTED_FEATURE));
            featureCheck.setRolloverEnabled(true);
            featureCheck.setRolloverIcon(XIPUIUtilities.getIcon(UIConstants.FEATURE_TO_CHECK));
            featureCheck.setIcon(XIPUIUtilities.getIcon(UIConstants.FEATURE));
            featureCheck.addActionListener(new java.awt.event.ActionListener() {

                public void actionPerformed(java.awt.event.ActionEvent evt) {
                    checkPerformed(evt, feature);
                }
            });

            return featureCheck;
        } else {
            JButton but = new JButton(feat_, XIPUIUtilities.getIcon(UIConstants.SHOW_SOURCE));
            but.setFont(UIConstants.DEFAULT_FONT);
            but.setToolTipText("Goto to rule " + feature.getValue());
            but.setRolloverEnabled(true);
            but.addActionListener(new java.awt.event.ActionListener() {

                public void actionPerformed(java.awt.event.ActionEvent evt) {
                    gotoRule(feature.getValue());
                    JComponent c = getRootComponent();
                    if (c instanceof IModule) {
                        ((IModule) c).close();
                    }
                }
            });
            return but;
        }
    }

    private void gotoRule(String rule) {
        try {
            int nRule = Integer.parseInt(rule);
            Project p = getProject();
            RuleMap ruleMap = p.getRuleMap();
            editFileAction.setFile(ruleMap.getFileByRuleNb(nRule));
            editFileAction.putValue(EditFileAction.CARET, ruleMap.getOffsetByRuleNb(nRule) + 1);
            editFileAction.executeWithoutUI();

        } catch (Exception e) {
            warning(e.toString());
        }
    }

    private void checkPerformed(ActionEvent evt, XipFeature feature) {
        SessionGraph session = getModuleContext().getSessionGraph(false, id);
        JCheckBox cb = (JCheckBox) evt.getSource();
        if (session.isSelectable()) {
            if (cb.isSelected()) {
                session.addNodeGraph(this);
                cb.setBackground(Color.lightGray);
                cb.setForeground(Color.red);
                node.addSelectedFeature(feature);
            } else {
                cb.setBackground(Color.white);
                cb.setForeground(Color.black);
                node.removeSelectedFeature(feature);
            }
            session.refreshViewer();
        }
    }

    private JLabel createJLabelDependency(XipDependency dep) {
        JLabel label = new JLabel(dep.toString() + "#" + dep.getIndex());
        label.setIcon(XIPUIUtilities.getIcon(UIConstants.DEPENDENCY));
        label.setFont(UIConstants.DEFAULT_FONT);
        return label;
    }

    private boolean containsFeature(XipFeature feat) {

        return XipUtilities.containsFeature(this.features, feat);
//        Iterator ite = features.iterator();
//        boolean stop = false;
//        String toCompare = feat.getName() + ":" + feat.getValue();
//        while ( ite.hasNext() && !stop) {
//            XipFeature curFeat = (XipFeature)ite.next();
//            String value = curFeat.getName() + ":" + curFeat.getValue();
//            if ( value.equals(toCompare) ) {
//                stop = true;
//            }
//        }
//        return stop;
    }

    public SelectedObject getSelectedObject() {
        return this.node;
    }

    public Vector getSelectedFeatures() {
        return this.node.getSelectedFeatures();
    }

    public boolean isAny() {
        return this.node.getAny();
    }

    public void setAny(boolean b) {
        this.node.setAny(b);
    }

    public boolean equals(Object object) {
        if (object instanceof NodeGraph) {
            NodeGraph toCompare = (NodeGraph) object;
            return getName().equals(toCompare.getName());
        } else {
            return false;
        }
    }
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JPanel DependencyPane;
    private javax.swing.JPanel featuresPane;
    private javax.swing.JLabel labelDependency;
    private javax.swing.JLabel labelFeatures;
    // End of variables declaration//GEN-END:variables
}
