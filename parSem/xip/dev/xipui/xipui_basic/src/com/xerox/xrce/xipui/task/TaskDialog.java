/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 ****************************************************************************************
 * Copyright (c) 2007 Xerox Corporation                                                 *
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
package com.xerox.xrce.xipui.task;

import com.xerox.xrce.xipui.ui.Tracer;
import com.xerox.xrce.xipui.modules.TaskModule;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;
import java.awt.event.KeyEvent;
import javax.swing.*;
import java.awt.*;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

/**
 *
 * @author vgrassau
 */
public class TaskDialog extends JDialog implements PropertyChangeListener {

    private JProgressBar progressBar;
    private TaskModule task;

    /** Creates a new instance of TaskDialog
     * @param task
     * @param withLogo
     * @param modal 
     */
    public TaskDialog(TaskModule task, boolean withLogo, boolean modal) {
        this.task = task;
        setModal(modal);
        progressBar = new JProgressBar();
        //get space for the string
        progressBar.setStringPainted(true);
        //but don't paint it
        progressBar.setString("");
        progressBar.setToolTipText("CTRL+C to stop the process");
        addKeyListener(new java.awt.event.KeyAdapter() {

            public void keyPressed(java.awt.event.KeyEvent evt) {
                stopTask(evt);
            }
        });
        if (task != null) {
            if (!task.isIndeterminate()) {
                progressBar.setValue(0);
                progressBar.setIndeterminate(false);
                progressBar.setMinimum(0);
                progressBar.setMaximum(100);
            } else {
                progressBar.setValue(0);
                progressBar.setIndeterminate(true);
            }
        }
        initComponents(withLogo);
    }

    public void initComponents(boolean withLogo) {
        setUndecorated(true);
        setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);
        setResizable(true);
        JPanel panel = new JPanel();
        if (withLogo) {
            panel.setBackground(Color.WHITE);
            panel.setLayout(new BorderLayout());
            JLabel label = new JLabel();
            label.setIcon(XIPUIUtilities.getIcon(UIConstants.XIPUI));
            panel.add(label, BorderLayout.CENTER);
            panel.add(progressBar, BorderLayout.PAGE_END);
            setSize(400, 325);
        } else {
            panel.setLayout(new BorderLayout());
            panel.add(progressBar, BorderLayout.CENTER);
            setSize(400, 32);
        }
        this.setContentPane(panel);
        setLocationRelativeTo(null);
    }

    public void start() {
        //content pane must be opaque
        task.addPropertyChangeListener(this);
        task.execute();
        setVisible(true);
    }

    private void stopTask(java.awt.event.KeyEvent evt) {
        if (evt.isControlDown() && evt.getKeyCode() == KeyEvent.VK_C) {
            int res = JOptionPane.showConfirmDialog(this, "Do you really want to stop this process ? ", "", JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE);
            if (res == JOptionPane.YES_OPTION) {
                progressBar.setString("Task canceled");
                task.cancel(true);
                setVisible(false);
                dispose();
                Tracer.warning_("The process was stopped");
            }
        }
    }

    /**
     * Invoked when task's state property changes.
     */
    public void propertyChange(PropertyChangeEvent evt) {
        if (evt.getPropertyName().equals("state")) {
            SwingWorker.StateValue stateValue = task.getState();
            switch (stateValue) {
                case STARTED: {
                    progressBar.setString(task.getMessage());
                    progressBar.setValue(task.getProgress());
                    break;
                }
                case DONE: {
                    if (task.isDone()) {
                        progressBar.setString("Task completed");
                        progressBar.setValue(0);
                        setVisible(false);
                        dispose();
                    }
                    break;
                }
            }
        } else if (evt.getPropertyName().equals("progress")) {
            int progress = (Integer) evt.getNewValue();
            progressBar.setValue(progress);
            progressBar.setString(task.getMessage());
        }
    }
}
