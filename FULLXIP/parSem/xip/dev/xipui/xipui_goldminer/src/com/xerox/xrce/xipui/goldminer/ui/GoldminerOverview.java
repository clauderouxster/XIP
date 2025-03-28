/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * GoldminerOverview.java
 *
 * Created on March 13, 2007, 9:58 AM
 */
package com.xerox.xrce.xipui.goldminer.ui;

import com.xerox.xrce.xipui.modules.*;
import com.xerox.xrce.xipui.ui.*;

/**
 *
 * @author  vgrassau
 */
public class GoldminerOverview extends AbstractResult {

    private GeneralOverview general;
    private TypeOverview typeOverview;

    /** Creates new form GoldminerOverview
     * @param context 
     */
    public GoldminerOverview(ModuleContext context) {
        super(context, 0);
        general = new GeneralOverview(getModuleContext());
        typeOverview = new TypeOverview(getModuleContext());
        initComponents();
        panelTab.addTab("General", XIPUIUtilities.getIcon(UIConstants.DATA_2), general);
        panelTab.addTab("Types Overview", XIPUIUtilities.getIcon(UIConstants.RELATION_TYPE), typeOverview);
        this.refreshButton.setIcon(XIPUIUtilities.getIcon(UIConstants.REFRESH));
        close.setEnabled(false);
        this.remove(this.toolBar);
        this.icon = XIPUIUtilities.getIcon(UIConstants.PROPERTIES);
        refresh();
    }

    public void setResultName() {
        resultName = "Database info";
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc=" Generated Code ">//GEN-BEGIN:initComponents
    private void initComponents() {
        java.awt.GridBagConstraints gridBagConstraints;

        description = new javax.swing.JLabel();
        refreshButton = new javax.swing.JButton();
        panelTab = new javax.swing.JTabbedPane();

        setLayout(new java.awt.GridBagLayout());

        description.setFont(new java.awt.Font("Arial", 0, 11));
        description.setForeground(new java.awt.Color(0, 153, 51));
        description.setToolTipText("Description of the selected panel");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(6, 6, 6, 6);
        add(description, gridBagConstraints);

        refreshButton.setText("Refresh");
        refreshButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                refreshButtonActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(8, 8, 8, 8);
        add(refreshButton, gridBagConstraints);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(6, 6, 6, 6);
        add(panelTab, gridBagConstraints);

    }// </editor-fold>//GEN-END:initComponents

    private void refreshButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_refreshButtonActionPerformed
        refresh();
    }//GEN-LAST:event_refreshButtonActionPerformed

    private void refresh() {
        general.init();
        typeOverview.init();
        repaint();
        panelTab.setSelectedIndex(0);
    }
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JLabel description;
    private javax.swing.JTabbedPane panelTab;
    private javax.swing.JButton refreshButton;
    // End of variables declaration//GEN-END:variables

    public void closeModule() {
    }
}
