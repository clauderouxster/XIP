/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * TaskBar.java
 *
 * Created on November 7, 2006, 11:15 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.task;

import com.xerox.xrce.xipui.modules.TaskModule;
import com.xerox.xrce.xipui.ui.Tracer;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

/**
 *
 * @author vgrassau
 */
public class TaskProgressBar extends JPanel implements PropertyChangeListener {

    private JProgressBar progressBar;
    private TaskModule task;
    private MouseListener ml;
    private int width = 150;
    private int height = 50;

    /** Creates a new instance of TaskBar */
    public TaskProgressBar() {
        initComponents();
    }

    private void setProgressBar() {
        //get space for the string
        progressBar.setStringPainted(true);
        //but don't paint it
        progressBar.setString("");
        progressBar.setToolTipText("Press right mouse button to stop the process");
        progressBar.addMouseListener(ml);
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
    }

    private void removeMouseListener() {
        MouseListener[] tab = progressBar.getMouseListeners();
        for (int i = 0; i < tab.length; i++) {
            MouseListener ml_tab = tab[i];
            if (ml_tab.equals(ml)) {
                progressBar.removeMouseListener(ml_tab);
            }
        }
    }

    public void startProcess(TaskModule task_) {
        this.setVisible(true);
        this.task = task_;
        this.task.addPropertyChangeListener(this);
        setProgressBar();
        this.task.execute();
    }

    /**
     * Init of graphical components
     */
    private void initComponents() {
        setLayout(new BorderLayout());
        progressBar = new JProgressBar();
        add(progressBar, BorderLayout.CENTER);
        ml = new MouseAdapter() {

            public void mouseClicked(MouseEvent event) {
                if (event.getButton() == MouseEvent.BUTTON3) {
                    JPopupMenu popup = new JPopupMenu();
                    popup.add("Options : ");
                    popup.addSeparator();
                    Icon icon = XIPUIUtilities.getIcon(UIConstants.EXIT);
                    Action action = new AbstractAction("Canceled", icon) {

                        public void actionPerformed(ActionEvent evt) {
                            stopTask();
                        }
                    };
                    action.putValue(Action.SHORT_DESCRIPTION, "Stop the task");
                    popup.add(action);
                    popup.show(progressBar, event.getX(), event.getY());
                }
            }
        };
    }

    private void stopTask() {
        //if ( evt.isControlDown() && evt.getKeyCode() == evt.VK_C) {
        int res = JOptionPane.showConfirmDialog(this, "Do you really want to stop this process ? ", "", JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE);
        if (res == JOptionPane.YES_OPTION) {
            progressBar.setString("Task canceled");
            reset();
            Tracer.warning_("The process was stopped");
            task.cancel(true);
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
                        reset();
                    }
                    break;
                }
            }
        } else if (evt.getPropertyName().equals("progress")) {
            if (!task.isDone()) {
                int progress = (Integer) evt.getNewValue();
                progressBar.setValue(progress);
                progressBar.setString(task.getMessage());
            }
        }
    }

    /**
     *
     */
    public void reset() {
        progressBar.setIndeterminate(false);
        progressBar.setToolTipText(null);
        progressBar.setValue(0);
        progressBar.setString("");
        setVisible(true);
        MouseListener[] mltab = progressBar.getMouseListeners();
        removeMouseListener();
    }
}//end of class
