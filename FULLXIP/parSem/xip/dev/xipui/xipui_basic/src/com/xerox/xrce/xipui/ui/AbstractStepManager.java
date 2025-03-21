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
package com.xerox.xrce.xipui.ui;

import com.xerox.xrce.xipui.modules.DialogModule;
import com.xerox.xrce.xipui.modules.ModuleContext;
import javax.swing.*;
import java.awt.*;
import java.util.*;
import java.awt.event.*;

/**
 *
 * @author  vgrassau
 */
public abstract class AbstractStepManager extends DialogModule {

    protected Hashtable managerProperties;
    protected boolean canceled = false;
    static public final String manager_properties = "manager_properties";
    public final Color INFO_STEP_COLOR = new java.awt.Color(204, 255, 255);
    public final String NEXT_STEP = "     ...     ";
    public final int WIDTH_MANAGER = 560;
    public final int HEIGHT_MANAGER = 400;
    public final Font ON_STEP_FONT = UIConstants.DEFAULT_FONT;
    public final Font OFF_STEP_FONT = UIConstants.DEFAULT_FONT;
    protected AbstractStep[] steps;
    protected JLabel[] stepsLabel;
    protected int currentStep = 0;
    protected int nbStep = 0;

    /**
     *Creates a new manager with a moduleContext
     *@param properties ModuleContext
     * @param nbStep 
     */
    public AbstractStepManager(ModuleContext properties, int nbStep) {
        super(properties);
        this.managerProperties = new Hashtable();
        this.nbStep = nbStep;
        steps = new AbstractStep[nbStep];
        stepsLabel = new JLabel[nbStep];
        //create the empty label
        for (int i = 0; i < stepsLabel.length; i++) {
            stepsLabel[i] = new JLabel();
        }
        //we put the managerproperties into the context
        setProperty(manager_properties, managerProperties);
        initComponents();
        setSize(WIDTH_MANAGER, HEIGHT_MANAGER);
        initButtonListeners();
    }

    /**
     *Construction des descritpions pour les etapes
     *@param stepLab 
     * @param keyResources 
     *@param desc descritpion de l'etape
     *
     */
    protected void initDescription(int stepLab, String keyResources, String desc) {
        if (stepsLabel != null && stepLab < stepsLabel.length) {
            JLabel descriptionStep = stepsLabel[stepLab];
            if (descriptionStep != null) {
                descriptionStep.setForeground(INFO_STEP_COLOR);
                descriptionStep.setToolTipText(XIPUIUtilities.getApplicationResource(keyResources));
                descriptionStep.setText(desc);
                stepPanel.add(descriptionStep);
            }
        }
    }

    /**
     *check if the manager is or was canceled
     *@return true if is or was canceled false sinon
     */
    public boolean isCanceled() {
        return canceled;
    }

    /**
     *Close the manager module
     *removeall components
     */
    public void close() {
        if (!isClosed) {
            this.dispose();
        }
        this.isClosed = true;
    }

    /***
     *This method finishes the manager with the last step.
     *@return true, if the manager finished correctly, false otherwise
     */
    protected abstract boolean finish();

    // <editor-fold defaultstate="collapsed" desc=" Generated Code ">//GEN-BEGIN:initComponents
    private void initComponents() {
        java.awt.GridBagConstraints gridBagConstraints;

        mainPanel = new javax.swing.JPanel();
        jPanel1 = new javax.swing.JPanel();
        stepPanel = new javax.swing.JPanel();
        infostep = new javax.swing.JLabel();
        jSeparator1 = new javax.swing.JSeparator();
        entryPanel = new javax.swing.JPanel();
        buttonPanel = new javax.swing.JPanel();
        backButton = new javax.swing.JButton();
        nextButton = new javax.swing.JButton();
        finishButton = new javax.swing.JButton();
        cancelButton = new javax.swing.JButton();

        getContentPane().setLayout(new java.awt.GridBagLayout());

        setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);
        setTitle("Manager");
        setLocationByPlatform(true);
        setModal(true);
        addWindowListener(new java.awt.event.WindowAdapter() {
            public void windowClosed(java.awt.event.WindowEvent evt) {
                formWindowClosed(evt);
            }
        });

        mainPanel.setLayout(new java.awt.GridBagLayout());

        jPanel1.setLayout(new java.awt.GridBagLayout());

        jPanel1.setMaximumSize(new java.awt.Dimension(550, 280));
        jPanel1.setMinimumSize(new java.awt.Dimension(550, 280));
        jPanel1.setPreferredSize(new java.awt.Dimension(550, 280));
        stepPanel.setLayout(new javax.swing.BoxLayout(stepPanel, javax.swing.BoxLayout.Y_AXIS));

        stepPanel.setBackground(new java.awt.Color(102, 0, 102));
        stepPanel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
        stepPanel.setMaximumSize(new java.awt.Dimension(150, 280));
        stepPanel.setMinimumSize(new java.awt.Dimension(150, 280));
        stepPanel.setPreferredSize(new java.awt.Dimension(150, 280));
        infostep.setFont(new java.awt.Font("Arial", 1, 14));
        infostep.setForeground(new java.awt.Color(204, 255, 255));
        infostep.setText("Steps\n");
        stepPanel.add(infostep);

        jSeparator1.setForeground(new java.awt.Color(204, 255, 255));
        jSeparator1.setMaximumSize(new java.awt.Dimension(32767, 2));
        jSeparator1.setMinimumSize(new java.awt.Dimension(0, 2));
        stepPanel.add(jSeparator1);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.weighty = 1.0;
        jPanel1.add(stepPanel, gridBagConstraints);

        entryPanel.setLayout(new java.awt.BorderLayout());

        entryPanel.setMaximumSize(new java.awt.Dimension(1000, 1000));
        entryPanel.setMinimumSize(new java.awt.Dimension(410, 280));
        entryPanel.setPreferredSize(new java.awt.Dimension(410, 280));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        jPanel1.add(entryPanel, gridBagConstraints);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTH;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        mainPanel.add(jPanel1, gridBagConstraints);

        buttonPanel.setMaximumSize(new java.awt.Dimension(515, 40));
        buttonPanel.setMinimumSize(new java.awt.Dimension(515, 40));
        buttonPanel.setPreferredSize(new java.awt.Dimension(515, 40));
        backButton.setFont(new java.awt.Font("Arial", 0, 11));
        backButton.setText("Back ");
        backButton.setEnabled(false);
        buttonPanel.add(backButton);

        nextButton.setFont(new java.awt.Font("Arial", 0, 11));
        nextButton.setText("Next");
        buttonPanel.add(nextButton);

        finishButton.setFont(new java.awt.Font("Arial", 0, 11));
        finishButton.setText("Finish");
        finishButton.setEnabled(false);
        buttonPanel.add(finishButton);

        cancelButton.setFont(new java.awt.Font("Arial", 0, 11));
        cancelButton.setText("Cancel\n");
        buttonPanel.add(cancelButton);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTH;
        gridBagConstraints.weightx = 1.0;
        mainPanel.add(buttonPanel, gridBagConstraints);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        getContentPane().add(mainPanel, gridBagConstraints);

    }// </editor-fold>//GEN-END:initComponents

    /**
     *Method for a window closing event
     *@param evt the event
     *It is the same task than a canceled action
     *calls the cancelClose() method
     */
    private void formWindowClosed(java.awt.event.WindowEvent evt) {//GEN-FIRST:event_formWindowClosed
        cancelClose();
    }//GEN-LAST:event_formWindowClosed

    public short getMode() {
        Short mode = (Short) getProperty(user_mode);
        return mode.shortValue();
    }

    /**
     * Set the ButtonListeners for the next, back, finish, cancel actions
     */
    private void initButtonListeners() {
        /** Ajout des evenements pour la barre de boutons */
        backButton.addActionListener(new java.awt.event.ActionListener() {

            public void actionPerformed(java.awt.event.ActionEvent evt) {
                backButtonActionPerformed(evt);
            }
        });

        nextButton.addActionListener(new java.awt.event.ActionListener() {

            public void actionPerformed(java.awt.event.ActionEvent evt) {
                nextButtonActionPerformed(evt);
            }
        });

        finishButton.addActionListener(new java.awt.event.ActionListener() {

            public void actionPerformed(java.awt.event.ActionEvent evt) {
                finishButtonActionPerformed(evt);
            }
        });

        cancelButton.addActionListener(new java.awt.event.ActionListener() {

            public void actionPerformed(java.awt.event.ActionEvent evt) {
                cancelButtonActionPerformed(evt);
            }
        });
    }

    /**
     *Load the manager with a specific mode
     */
    public abstract void loadMode();

    /**
     *Action back for a manager, return to the previous step
     *@param evt the event
     */
    protected void backButtonActionPerformed(ActionEvent evt) {
        AbstractStep step = steps[currentStep];
        //no back button if the current step is the first
        if (currentStep == 1) {
            backButton.setEnabled(false);
        }
        //if the number of step is greater than 1 we can do a next action
        if (nbStep > 1) {
            nextButton.setEnabled(true);
        }
        //the current step has to be hidden
        step.setVisible(false);
        entryPanel.removeAll();
        //change the label and the text for the currentStep
        stepsLabel[currentStep].setFont(OFF_STEP_FONT);
        stepsLabel[currentStep].setText(NEXT_STEP);
        //go to the previous step
        currentStep = currentStep - 1;
        AbstractStep stepBack = steps[currentStep];
        //init the previous step
        stepBack.initStepManager();
        //add the the previous step to the panel
        entryPanel.add(stepBack);
        //check if we can finish the manager with the previous step
        finishButton.setEnabled(stepBack.couldFinish());
        stepBack.setVisible(true);
        // set the label description for the previous step
        stepsLabel[currentStep].setFont(ON_STEP_FONT);
        stepsLabel[currentStep].setText(stepBack.getDescription());
    }

    /**
     *Next action for the manager, go to the next step...
     *@param evt the event
     */
    protected void nextButtonActionPerformed(ActionEvent evt) {
        //get the current step
        AbstractStep step = steps[currentStep];
        //check if the step is valide , if true we can change the step and go to the next step
        if (step.isValide()) {
            //if the step is before the last
            if (currentStep == (steps.length - 2)) {
                nextButton.setEnabled(false);
            //finishButton.setEnabled(step.couldFinish());
            }
            backButton.setEnabled(true);
            //hide the current step
            step.setVisible(false);
            stepsLabel[currentStep].setFont(OFF_STEP_FONT);
            //NB we keep the text label for the current step
            entryPanel.removeAll();
            //we insert the next step
            currentStep = currentStep + 1;
            AbstractStep stepNext = steps[currentStep];
            stepNext.initStepManager();
            //check if with the next step the manager can be finished
            finishButton.setEnabled(stepNext.couldFinish());
            entryPanel.add(stepNext);
            stepNext.setVisible(true);
            //set the label description for the next AbstractStep
            stepsLabel[currentStep].setFont(ON_STEP_FONT);
            stepsLabel[currentStep].setText(stepNext.getDescription());
        }
    }

    /**
     * The finish action. Finish the task of the manager and close it
     * @param evt the event
     *
     */
    protected void finishButtonActionPerformed(ActionEvent evt) {
        // get the last step not the current step
        AbstractStep step = steps[steps.length - 1];
        //check if the step is valide and call the finish method
        if (step.isValide() && finish()) {
            dispose();
        } else {
            warning("Could not finish this Manager, check parameters...");
        }
    }

    /**
     *Cette methode permet d'annuler et fermer le manager
     *utilisee pour le bouton cancel et fermeture de la fenetre
     */
    private void cancelClose() {
        try {
            currentStep = 0;
            canceled = true;
            close();
        } catch (Exception e) {
            severe("ERROR, cannot cancel the manager", e);
        }
    }

    /**
     *The cancel action.Cancel the manager and close it
     *@param evt the event
     */
    protected void cancelButtonActionPerformed(ActionEvent evt) {
        cancelClose();
    }
    // Variables declaration - do not modify//GEN-BEGIN:variables
    protected javax.swing.JButton backButton;
    protected javax.swing.JPanel buttonPanel;
    protected javax.swing.JButton cancelButton;
    protected javax.swing.JPanel entryPanel;
    protected javax.swing.JButton finishButton;
    private javax.swing.JLabel infostep;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JSeparator jSeparator1;
    private javax.swing.JPanel mainPanel;
    protected javax.swing.JButton nextButton;
    protected javax.swing.JPanel stepPanel;
    // End of variables declaration//GEN-END:variables
}
