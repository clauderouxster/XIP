/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * DebugViewer.java
 *
 * Created on January 17, 2006, 10:47 AM
 */
package com.xerox.xrce.xipui.ide.ui;

import com.xerox.xrce.xipui.ui.editor.EditFileAction;
import com.xerox.xrce.xipui.ide.graph.ResultGraphBar;
import com.xerox.xrce.xipui.ui.AbstractResult;
import com.xerox.xrce.xipui.ui.RunManager;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.UIConstants;
import java.awt.event.*;
import java.awt.*;
import java.util.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.table.*;
import java.io.*;
import com.xerox.jxip.*;
import com.xerox.xrce.xipui.RunContext;
import com.xerox.xrce.xipui.Xip;
import com.xerox.xrce.xipui.ide.DebugXip;
import com.xerox.xrce.xipui.ide.grammar.GrammarEdit;
import com.xerox.xrce.xipui.ui.graph.ChunkGraph;
import com.xerox.xrce.xipui.ui.graph.SessionGraph;
import com.xerox.xrce.xipui.ui.graph.TableDependency;
import java.util.concurrent.ExecutionException;

/**
 *
 * @author  vgrassau
 */
public class DebugViewer extends AbstractResult implements TableModelListener {

    protected Color mainColor = new Color(153, 255, 153);
    protected EditFileAction editFileAction;
    public final String TITLE = " ";
    /**
     *Label par defaut pour afficher le nombre de regles
     *sur une phrase
     */
    private final String labelApplyed = "Rules which can be observed on sentence";
    private String indexTitle = "";
    /**
     *Numero de la phrase en cours de debogue
     */
    protected int sentenceSelected = 0;
    /**
     *
     */
    protected boolean sentenceHasChanged = false;
    /**
     *Manager de regle...todo : replace this object by the RuleMap object
     */
    protected RuleLoadedManager ruleManager; //manager des regles
    /**
     *Regle limite
     */
    protected RuleLoaded limit; // regle limite a atteindre
    /**
     *Liste des regles appliquees
     */
    protected Vector appliedRulesList;
    /**
     *ecouteur sur la liste des regles appliquee
     */
    protected ActionListener ruleSetListener;
    /***
     *The chunk graph
     */
    protected ChunkGraph chunkGraph;
    /**
     *the table of dependencies
     */
    protected TableDependency dependencyTable;
    /**
     *the menubar
     */
    protected ResultGraphBar menuBar;

    /**
     *Creates an instance of DebugViewer.
     *@param properties 
     * @param id the id of the result
     *   
     */
    public DebugViewer(ModuleContext properties, int id) {
        super(properties, id);
        try {
            editFileAction = new EditFileAction(getModuleContext());
            editFileAction.putValue(EditFileAction.ICON, XIPUIUtilities.getIcon(UIConstants.CODE));
            editFileAction.setType(EditFileAction.GRAMMAR_EDITOR);
            setIcon(XIPUIUtilities.getIcon(UIConstants.DEBUG));
            appliedRulesList = new Vector();
            indexTitle = getName() + TITLE;
            Project project = getProject();
            ruleManager = new RuleLoadedManager();
            ruleManager.init(project.getGhandler());
            listCorpusElement = new Vector();
            initComponents();
            //bouton on the left panel
            run.setIcon(XIPUIUtilities.getIcon(UIConstants.START));
            run.setText("");
            edit.setAction(editFileAction);
            edit.setIcon(XIPUIUtilities.getIcon(UIConstants.SHOW_SOURCE));
            edit.setText("");
            ruleSlider.setEnabled(false);
            tableInfo.getModel().addTableModelListener(this);
            tablePanel.add(tableInfo.getTableHeader(), BorderLayout.PAGE_START);
            initDebugManager();
            //set the rule listener
            ruleSetListener = new ActionListener() {

                public void actionPerformed(ActionEvent event) {
                    cbRuleSet1ActionPerformed(event);
                }
            };
            //create the graph session for the chunk graph and table of dependencies
            SessionGraph session = getModuleContext().getSessionGraph(getId());
            //instanciation of the ChunkGraph with the properties and session
            chunkGraph = new ChunkGraph(getModuleContext(), id);
            //instanciation of the table of dependencies
            dependencyTable = new TableDependency(getModuleContext(), id);
            //we put some parameter to the session
            session.setProperty(session.NAME, getResultName());
            menuBar = new ResultGraphBar();
            menuBar.setBuildAction(changeSentenceAction());
            //add the menubar to the toolbar of the result
            toolBar.add(menuBar);
            add(toolBar, BorderLayout.PAGE_START);
        } catch (Exception e) {
            severe("Could not open Debug module", e);
        }
    }

    /**
     *Returns the rule number of an Applyed rule of a given index among the complete list of applyed rules.
     *@param index the index of the applied rules 0 is the first apllyed rule
     *@return the rule number the rule max number of the grammar if problem
     */
    private int getAppliedRuleFromRank(int index) {
        int indexInApplyed = (this.appliedRulesList.size() - 1) - index;
        Integer ruleNumber = this.ruleManager.getRuleCounter() - 1;
        try {
            ruleNumber = (Integer) appliedRulesList.get(indexInApplyed);
        } catch (Exception e) {
            log.warn("warning", e);
        }
        // if the index is not found return the last rule of the grammar
        if (ruleNumber.intValue() == -1) {
            ruleNumber = this.ruleManager.getRuleCounter() - 1;
        }
        return ruleNumber.intValue();
    }

    /**
     *Returns the rule limit
     *@return a RuleLoaded
     */
    public RuleLoaded getRuleLimit() {
        return limit;
    }

    /**
     *Apply the background color on the label objects
     *@param color the color to apply
     */
    public void applyColor(Color color) {
        if (color == null) {
            color = mainColor;
        } else {
            mainColor = color;
        }
        this.sourceLabel.setBackground(mainColor);
        this.rulesAppliedLabel.setBackground(mainColor);
        this.rulesInformationLabel.setBackground(mainColor);
        this.selectorLabel.setBackground(mainColor);
        this.menuBar.setBackground(mainColor);
    }

    /**
     *Sets the name of this result
     */
    public void setResultName() {
        this.resultName = "Debug (" + getId() + ")";
    }

    /**
     *Moves the cursor of the slidebar to the max.
     */
    public void setSliderMax() {
        ruleSlider.setValue(ruleSlider.getMaximum());
    }

    /**
     *Sets the number of the sentence which is analysed
     *this method  changes only the value of a label like this :
     *the global variable labelApplyed + ":" + num
     *@param num the number of a sentence
     *
     */
    public void setSentence(int num) {
        this.rulesAppliedLabel.setText(labelApplyed + ": " + num);
        this.sentenceSelected = num;
    }

    /**
     *Event when the description table of a rule change
     *the table can be changed by a ruleSlider state changing or by a user.
     *Note : if a user changes a value then the rule slider has to be update too
     *@param e the event
     */
    public void tableChanged(TableModelEvent e) {
        int row = e.getFirstRow();
        int column = e.getColumn();
        TableModel model = (TableModel) e.getSource();
        if (column == 1 && row == 1) {
            Object data = model.getValueAt(row, column);
            try {
                int nb = Integer.parseInt(data.toString());
                //get the limit min and limit max for the rules
                // int max = this.getAppliedRuleFromRank(ruleSlider.getMaximum());
                //int min = this.getAppliedRuleFromRank(ruleSlider.getMinimum());
                if (appliedRulesList.contains(nb) || nb == ruleManager.getRuleCounter() - 1) {
                    // if the rule in the applyed rules :
                    RuleLoaded newLimit = ruleManager.getRulebyRuleNb(nb);
                    //if the limit is really new
                    if (newLimit.getRuleNb() != limit.getRuleNb()) {
                        limit = newLimit;
                        updateSlider();
                    }
                } else {
                    warning("Your number is incorrect : " + data);
                }
            } catch (Exception ex) {
                log.warn("warning : ", ex);
                warning("Your number is incorrect : " + data);
            }
        } else if (column == 1 && row == 4) {
            // pour le label
        }
    }

    /**
     *Add or show the debugmanager panel on the left side of TemplateGUI
     */
    protected void initDebugManager() {
        //nothing for the debuger
    }

    /**
     *launch an anlysis with all rules
     *@param elements the debug element
     */
    public void init(Collection<DebugElement> elements) {
        Iterator<DebugElement> ite = elements.iterator();
        while (ite.hasNext()) {
            menuBar.addItem(ite.next());
        }
        menuBar.setSpinner();
        int ruleLimNumber = getProject().getRunContext().getGrammarLimit();
        load(ruleLimNumber, true);
        showPanels(true);
    }

    /**
     *Launch an analysis to a rule limit if full or use an index of an applyed rules if not full
     *@param l the rule limit or the index
     *@param full true to analyse all the rules or use a rule limit, false to use index of the applyed rules
     *Note : the analysis is launched in a background thread
     */
    private void load(int l, boolean full) {
        ruleSlider.setEnabled(true);
        setInWait(true);
        LoadTask task = new LoadTask(l, full);
        task.execute();
    }

    /**
     *Sets wait the deboguer until the analysis is finished, some events become disabled and cursors become in WAIT
     *@param b true to set in wait, false otherwise
     */
    private void setInWait(boolean b) {
        run.setEnabled(!b);
        plus.setEnabled(!b);
        minus.setEnabled(!b);
        if (b) {
            scrollManage.setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
            splitter.setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
        } else {
            scrollManage.setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
            splitter.setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
        }
    }

    /**
     * Action when an other sentence is selected
     * the applyed rule has to be recalculated for the new sentence
     * the slider is put on max
     * @see load(int,boolean)
     * @see setsliderMax()
     * @see setSentence(int)
     *@return the action
     */
    public Action changeSentenceAction() {
        Action buildAction = new AbstractAction("parse") {

            public void actionPerformed(java.awt.event.ActionEvent evt) {
                try {
                    DebugElement sentence = menuBar.getSelectedSentence();
                    int sentenceNumber = sentence.getIndex();
                    sentenceHasChanged = (sentenceNumber != sentenceSelected);
                    if (sentenceHasChanged) {
                        info("sentence to debug had changed, new number : " + sentenceNumber);
                        Project project = getProject();
                        project.getRunContext().applyGrammarLimit();
                        int ruleLimNumber = getProject().getRunContext().getGrammarLimit();
                        setSliderMax();
                        setSentence(sentenceNumber);
                        load(ruleLimNumber, true);
                    }
                } catch (Exception e) {
                    severe("An error was occured ", e);
                }
            }
        };
        return buildAction;
    }

    /**
     *Update the applied rules.
     *Reads the collection of applyed rules and insert the index in the combobox.
     *Add an action listener when a user change the selection, the parsing will be automatically loaded
     */
    public void updateAppliedRules() {
        // the renderer for the combobox
        this.cbRuleSet1.setRenderer(new AppliedRuleRenderer());
        //we construct the tooltip ie all the rules in the
        //same time that we update the combobox
        StringBuffer toolTip = new StringBuffer();
        toolTip.append("<html><body>");
        //we remove the actionlistener because when you put
        //some item in combobox with an actionlistener, the action is executing
        //so we need to remove it and reinsert it
        this.cbRuleSet1.removeActionListener(ruleSetListener);
        this.cbRuleSet1.removeAllItems();
        // the update begins
        for (int i = 0; i < appliedRulesList.size(); i++) {
            // we put only the index in the combox
            // like is we have unique item and the methode getSeleted will work well
            // for our needs
            this.cbRuleSet1.addItem(new Integer(i));
            Object obj = appliedRulesList.get(i); //obj is a rule
            toolTip.append("<p>" + obj + "</p>");
        }
        toolTip.append("</html></body>");
        this.cbRuleSet1.setToolTipText(toolTip.toString());
        //we reinsert the action listener
        this.cbRuleSet1.addActionListener(ruleSetListener);
    }

    /***
     *Show some panels, the rules source and the scollmanage
     *@param b not use...
     */
    protected void showPanels(boolean b) {
        JTabbedPane leftTab = (JTabbedPane) getProperty(LEFT_TABBED);
        XIPUIUtilities.addComponentOnTab(leftTab, scrollManage, getIcon(), indexTitle, indexTitle);
        JTabbedPane outputTab = (JTabbedPane) getProperty(DebugViewer.OUTPUT_TABBED);
        XIPUIUtilities.addComponentOnTab(outputTab, sourcePanel, getIcon(), getResultName(), getResultName());
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc=" Generated Code ">//GEN-BEGIN:initComponents
    private void initComponents() {
        java.awt.GridBagConstraints gridBagConstraints;

        scrollManage = new javax.swing.JScrollPane();
        debugManager = new javax.swing.JPanel();
        rulesInformationLabel = new javax.swing.JLabel();
        tablePanel = new javax.swing.JPanel();
        tableInfo = new javax.swing.JTable();
        rulesAppliedLabel = new javax.swing.JLabel();
        cbRuleSet1 = new javax.swing.JComboBox();
        minus = new javax.swing.JButton();
        debugToolBar = new javax.swing.JToolBar();
        run = new javax.swing.JButton();
        ruleSlider = new javax.swing.JSlider();
        selectorLabel = new javax.swing.JLabel();
        plus = new javax.swing.JButton();
        sourcePanel = new javax.swing.JPanel();
        scroller = new javax.swing.JScrollPane();
        sourceText = new javax.swing.JTextArea();
        toolBarSource = new javax.swing.JToolBar();
        sourceLabel = new javax.swing.JLabel();
        fileSource = new javax.swing.JLabel();
        edit = new javax.swing.JButton();
        splitter = new javax.swing.JSplitPane();
        scrollChunk = new javax.swing.JScrollPane();
        scrollTable = new javax.swing.JScrollPane();

        scrollManage.setBorder(null);
        scrollManage.addComponentListener(new java.awt.event.ComponentAdapter() {
            public void componentShown(java.awt.event.ComponentEvent evt) {
                scrollManageComponentShown(evt);
            }
        });
        scrollManage.addFocusListener(new java.awt.event.FocusAdapter() {
            public void focusGained(java.awt.event.FocusEvent evt) {
                scrollManageFocusGained(evt);
            }
        });

        debugManager.setLayout(new java.awt.GridBagLayout());

        debugManager.setBackground(new java.awt.Color(255, 255, 255));
        debugManager.addComponentListener(new java.awt.event.ComponentAdapter() {
            public void componentShown(java.awt.event.ComponentEvent evt) {
                debugManagerComponentShown(evt);
            }
        });

        rulesInformationLabel.setBackground(mainColor);
        rulesInformationLabel.setFont(new java.awt.Font("Arial", 0, 11));
        rulesInformationLabel.setText("Rule information");
        rulesInformationLabel.setBorder(javax.swing.BorderFactory.createMatteBorder(0, 0, 1, 0, new java.awt.Color(255, 102, 0)));
        rulesInformationLabel.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.gridwidth = java.awt.GridBagConstraints.REMAINDER;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        debugManager.add(rulesInformationLabel, gridBagConstraints);

        tablePanel.setLayout(new java.awt.BorderLayout());

        tableInfo.setBorder(new javax.swing.border.LineBorder(new java.awt.Color(0, 0, 0), 1, true));
        tableInfo.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {
                {"Type", null},
                {"Number", null},
                {"Absolute Layer", null},
                {"Relative Layer", null},
                {"Label", null}
            },
            new String [] {
                "Properties", "Value"
            }
        ) {
            boolean[] canEdit = new boolean [] {
                false, true
            };

            public boolean isCellEditable(int rowIndex, int columnIndex) {
                return canEdit [columnIndex];
            }
        });
        tablePanel.add(tableInfo, java.awt.BorderLayout.CENTER);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.gridwidth = java.awt.GridBagConstraints.REMAINDER;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        debugManager.add(tablePanel, gridBagConstraints);

        rulesAppliedLabel.setBackground(mainColor);
        rulesAppliedLabel.setFont(new java.awt.Font("Arial", 0, 11));
        rulesAppliedLabel.setText("Rules which can be observed on sentence");
        rulesAppliedLabel.setBorder(javax.swing.BorderFactory.createMatteBorder(0, 0, 1, 0, new java.awt.Color(255, 102, 0)));
        rulesAppliedLabel.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.gridwidth = java.awt.GridBagConstraints.REMAINDER;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        debugManager.add(rulesAppliedLabel, gridBagConstraints);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 5;
        gridBagConstraints.gridwidth = java.awt.GridBagConstraints.REMAINDER;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 4, 2, 2);
        debugManager.add(cbRuleSet1, gridBagConstraints);

        minus.setText("-");
        minus.setMargin(new java.awt.Insets(2, 4, 2, 4));
        minus.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                minusActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 6;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHEAST;
        gridBagConstraints.weightx = 0.5;
        gridBagConstraints.weighty = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 4);
        debugManager.add(minus, gridBagConstraints);

        debugToolBar.setBackground(new java.awt.Color(255, 255, 255));
        debugToolBar.setFloatable(false);
        debugToolBar.setRollover(true);
        debugToolBar.setAutoscrolls(true);
        debugToolBar.setMaximumSize(new java.awt.Dimension(278, 27));
        run.setText("Run");
        run.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                runActionPerformed(evt);
            }
        });

        debugToolBar.add(run);

        ruleSlider.setBackground(new java.awt.Color(255, 255, 255));
        ruleSlider.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                ruleSliderStateChanged(evt);
            }
        });

        debugToolBar.add(ruleSlider);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 4, 2, 2);
        debugManager.add(debugToolBar, gridBagConstraints);

        selectorLabel.setBackground(mainColor);
        selectorLabel.setFont(new java.awt.Font("Arial", 0, 11));
        selectorLabel.setText("Rule Selector");
        selectorLabel.setBorder(javax.swing.BorderFactory.createMatteBorder(0, 0, 1, 0, new java.awt.Color(255, 102, 0)));
        selectorLabel.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTH;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        debugManager.add(selectorLabel, gridBagConstraints);

        plus.setText("+");
        plus.setMargin(new java.awt.Insets(2, 4, 2, 4));
        plus.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                plusActionPerformed(evt);
            }
        });

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 6;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 0.5;
        gridBagConstraints.weighty = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 4, 2, 2);
        debugManager.add(plus, gridBagConstraints);

        scrollManage.setViewportView(debugManager);

        sourcePanel.setLayout(new java.awt.GridBagLayout());

        sourcePanel.setBackground(new java.awt.Color(255, 255, 255));
        scroller.setBorder(null);
        sourceText.setColumns(20);
        sourceText.setEditable(false);
        sourceText.setFont(new java.awt.Font("Arial", 0, 13));
        sourceText.setRows(5);
        scroller.setViewportView(sourceText);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(4, 4, 4, 4);
        sourcePanel.add(scroller, gridBagConstraints);

        toolBarSource.setFloatable(false);
        toolBarSource.setRollover(true);
        sourceLabel.setBackground(mainColor);
        sourceLabel.setFont(new java.awt.Font("Arial", 0, 11));
        sourceLabel.setText("Rule source   ");
        sourceLabel.setBorder(javax.swing.BorderFactory.createMatteBorder(0, 0, 1, 0, new java.awt.Color(255, 102, 0)));
        sourceLabel.setOpaque(true);
        toolBarSource.add(sourceLabel);

        fileSource.setFont(new java.awt.Font("Arial", 0, 11));
        fileSource.setForeground(new java.awt.Color(255, 51, 51));
        fileSource.setText("file");
        toolBarSource.add(fileSource);

        edit.setFont(new java.awt.Font("Arial", 0, 11));
        edit.setText("edit rule");
        toolBarSource.add(edit);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        sourcePanel.add(toolBarSource, gridBagConstraints);

        setLayout(new java.awt.BorderLayout());

        addComponentListener(new java.awt.event.ComponentAdapter() {
            public void componentShown(java.awt.event.ComponentEvent evt) {
                formComponentShown(evt);
            }
        });
        addFocusListener(new java.awt.event.FocusAdapter() {
            public void focusGained(java.awt.event.FocusEvent evt) {
                formFocusGained(evt);
            }
        });

        splitter.setBackground(new java.awt.Color(255, 255, 255));
        splitter.setBorder(null);
        splitter.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
        splitter.setResizeWeight(0.5);
        splitter.setAutoscrolls(true);
        splitter.setContinuousLayout(true);
        splitter.setOneTouchExpandable(true);
        splitter.setTopComponent(scrollChunk);

        splitter.setRightComponent(scrollTable);

        add(splitter, java.awt.BorderLayout.CENTER);

    }// </editor-fold>//GEN-END:initComponents
    private void scrollManageComponentShown(java.awt.event.ComponentEvent evt) {//GEN-FIRST:event_scrollManageComponentShown
        scrollManage.requestFocusInWindow();
    }//GEN-LAST:event_scrollManageComponentShown

    private void scrollManageFocusGained(java.awt.event.FocusEvent evt) {//GEN-FIRST:event_scrollManageFocusGained
        JTabbedPane tab = (JTabbedPane) getProperty(CENTER_TABBED);
        int index = tab.indexOfTab(RunManager.TITLE);
        if (index != -1) {
            RunManager run_ = (RunManager) tab.getComponentAt(index);
            run_.setSelectedResult(getResultName());
            tab.setSelectedIndex(index);
            tab.repaint();
        }
        repaint();
    }//GEN-LAST:event_scrollManageFocusGained

    private void formFocusGained(java.awt.event.FocusEvent evt) {//GEN-FIRST:event_formFocusGained
        showPanels(true);
    }//GEN-LAST:event_formFocusGained

    private void formComponentShown(java.awt.event.ComponentEvent evt) {//GEN-FIRST:event_formComponentShown
        requestFocusInWindow();
        repaint();
    }//GEN-LAST:event_formComponentShown

    private void debugManagerComponentShown(java.awt.event.ComponentEvent evt) {//GEN-FIRST:event_debugManagerComponentShown
    }//GEN-LAST:event_debugManagerComponentShown

    private void cbRuleSet1ActionPerformed(java.awt.event.ActionEvent evt) {
        updateRuleSet(cbRuleSet1);
    }

    /**
     *Calls whend the slider changed
     *updates some info about the selected rule, and fix the limit to this selected rule
     *@see updateInfo()
     */
    private void sliderChanged() {
        if (!this.appliedRulesList.isEmpty()) {
            int ruleNumber = getAppliedRuleFromRank(ruleSlider.getValue());
            RuleLoaded newLimit = ruleManager.getRulebyRuleNb(ruleNumber);
            if (limit == null || (newLimit.getRuleNb() != limit.getRuleNb())) {
                limit = newLimit;
                updateInfo();
            }
        }
    }

    private void ruleSliderStateChanged(javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_ruleSliderStateChanged
        sliderChanged();
    }//GEN-LAST:event_ruleSliderStateChanged

    /**
     *Go to the next step in rules application, and launch the analysis
     */
    private void plusActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_plusActionPerformed
        try {
            // on cherche la regle courant selectionnee dans la liste des regles appliquees :
            int currentAppliedRule = cbRuleSet1.getSelectedIndex();
            int max = cbRuleSet1.getItemCount();
            if (currentAppliedRule > 0) {
                int newIndex = currentAppliedRule - 1;
                Integer newRuleNumber = (Integer) cbRuleSet1.getItemAt(newIndex);
                cbRuleSet1.setSelectedIndex(newIndex);
            }
        } catch (Exception e) {
            severe("ERROR", e);
        }
    }//GEN-LAST:event_plusActionPerformed

    /**
     *lanchs an analysis to the rule limit
     */
    private void runActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_runActionPerformed
        try {
            int index = ruleSlider.getValue();
            if (index == ruleSlider.getMaximum()) {
                int ruleLimNumber = getProject().getRunContext().getGrammarLimit();
                load(ruleLimNumber, true);
            } else {
                load(index, false);
            }
        } catch (Exception e) {
            severe("ERROR", e);
        }
    }//GEN-LAST:event_runActionPerformed

    /**
     *Launch analysis at the previous applied rule
     *@param evt the event source of this action
     *@see plusActionPerformed(ActionEvent)
     */
    private void minusActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_minusActionPerformed
        try {
            //we search the seleceted applyed rule
            int currentAppliedRule = cbRuleSet1.getSelectedIndex();
            int max = cbRuleSet1.getItemCount();
            // if the rule is not the last we can do the action
            if (currentAppliedRule < max - 1) {
                int newIndex = currentAppliedRule + 1;
                Integer newRuleNumber = (Integer) cbRuleSet1.getItemAt(newIndex);
                // this instruction starts an action for the combobox
                cbRuleSet1.setSelectedIndex(newIndex);
            }
        } catch (Exception e) {
            warning("Problem : " + e.toString());
        }
    }//GEN-LAST:event_minusActionPerformed

    private void updateInfo() {
        editFileAction.putValue(EditFileAction.CARET, limit.getOffset() + 1);
        editFileAction.setFile(limit.getFile());
        updateRuleNumber();
        updateRuleSource();
        updateTypeRule();
        updateLayerRule();
        ruleSlider.setToolTipText("rule : " + limit.getRuleNb());
        this.run.setToolTipText("Parso to rule : " + limit.getRuleNb());
        this.edit.setToolTipText("Edit rule : " + limit.getRuleNb());
        this.cbRuleSet1.removeActionListener(ruleSetListener);
        this.cbRuleSet1.setSelectedIndex(ruleSlider.getMaximum() - ruleSlider.getValue());
        this.cbRuleSet1.addActionListener(ruleSetListener);
    }

    /**
     *Update the slider bar, changes the value of the slide bar if it's necessary.
     *Update also the values in the table
     */
    private void updateSlider() {
        if (limit != null) {
            int lastIndex = this.appliedRulesList.lastIndexOf(new Integer(limit.getRuleNb()));
            if (lastIndex != -1 && (ruleSlider.getMaximum() - lastIndex) != ruleSlider.getValue()) {
                ruleSlider.setValue(ruleSlider.getMaximum() - lastIndex);
            }
            updateInfo();
        }
    }

    /**
     *Update the rule source limit
     *show the source, the file and file path, if the file is crypted the sources cannot be shown
     */
    private void updateRuleSource() {
        try {
            Project project = getProject();
            File file = limit.getFile();
            fileSource.setText(file.getName());
            fileSource.setToolTipText(file.getAbsolutePath());
            sourceText.setToolTipText(file.getAbsolutePath());
            if (!project.getGrammar().isCryptedFile(file)) {
                sourceText.setText(limit.getRuleSrc(true));
            } else {
                sourceText.setText("The file is crypted");
            }
            sourceText.setCaretPosition(0);
        } catch (Exception e) {
            sourceText.setText("ERROR : Could not display the rule source for " + limit.getRuleNb());
            error("ERROR : Could not display the rule source for " + limit.getRuleNb());
        }
    }

    /**
     *Updates the rule number in the table with the rule limit
     */
    private void updateRuleNumber() {
        tableInfo.setValueAt(limit.getRuleNb(), 1, 1);
    }

    /**
     * updates the rule type in the table with the rule limit
     */
    private void updateTypeRule() {
        tableInfo.setValueAt(limit.getStringRuleType(), 0, 1);
    }

    /**
     *Updates the layer ie aboslute and relative  in the table
     *with the rule limit
     */
    private void updateLayerRule() {
        tableInfo.setValueAt(limit.getAbsLayer(), 2, 1);
        tableInfo.setValueAt(limit.getRelLayer(), 3, 1);
    }

    /**
     *Update the information about a rule with the list of applied rules
     *this methode is called when an action is detected on the combobox
     *@param list combobox containing the index of the applyed rules
     */
    private void updateRuleSet(JComboBox list) {
        //the index in applyed rules list
        Object o = list.getSelectedItem();
        int ruleLimNumber = getProject().getRunContext().getGrammarLimit();
        if (o instanceof Integer) {
            Integer index = ((Integer) o).intValue();
            Integer ruleNumber = (Integer) appliedRulesList.get(index);
            if (ruleNumber.intValue() != -1) {
                limit = ruleManager.getRulebyRuleNb(ruleNumber.intValue());
                ruleSlider.setValue(ruleSlider.getMaximum() - index);
                updateInfo();
                load(ruleSlider.getMaximum() - index, false);
            } else {
                load(ruleLimNumber, true);
            }
        }
    }

    /**
     *Close the debugViewer
     */
    public void closeModule() {
        info("close " + getResultName());
        JTabbedPane leftTab = (JTabbedPane) getProperty(LEFT_TABBED);
        JTabbedPane outputTab = (JTabbedPane) getProperty(OUTPUT_TABBED);
        int index = leftTab.indexOfTab(indexTitle);
        int index2 = outputTab.indexOfTab(getResultName());
        if (index != -1) {
            leftTab.remove(index);
        }
        if (index2 != -1) {
            outputTab.remove(index2);
        }
        outputTab.setSelectedIndex(0);
        SessionGraph session = getModuleContext().getSessionGraph(false, getId());

        //vidage memoire de la session :
        if (session != null) {
            session.invalidate();
        }
        this.chunkGraph = null;
        this.toolBar = null;
        this.dependencyTable = null;
        this.menuBar = null;
        this.isClosed = true;
        this.removeNotify();
    }
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JComboBox cbRuleSet1;
    private javax.swing.JPanel debugManager;
    private javax.swing.JToolBar debugToolBar;
    private javax.swing.JButton edit;
    private javax.swing.JLabel fileSource;
    private javax.swing.JButton minus;
    private javax.swing.JButton plus;
    private javax.swing.JSlider ruleSlider;
    private javax.swing.JLabel rulesAppliedLabel;
    private javax.swing.JLabel rulesInformationLabel;
    private javax.swing.JButton run;
    private javax.swing.JScrollPane scrollChunk;
    private javax.swing.JScrollPane scrollManage;
    private javax.swing.JScrollPane scrollTable;
    private javax.swing.JScrollPane scroller;
    private javax.swing.JLabel selectorLabel;
    private javax.swing.JLabel sourceLabel;
    private javax.swing.JPanel sourcePanel;
    private javax.swing.JTextArea sourceText;
    private javax.swing.JSplitPane splitter;
    private javax.swing.JTable tableInfo;
    private javax.swing.JPanel tablePanel;
    private javax.swing.JToolBar toolBarSource;
    // End of variables declaration//GEN-END:variables

    /**
     *Class to make a renderer for the list of applied rules.
     *a rule is rendered by a label
     *rule icon, rule number for tje label and the rule type for the tooltip text
     *
     */
    private class AppliedRuleRenderer implements ListCellRenderer {

        public AppliedRuleRenderer() {
            super();
        }

        /**
         *Return the render for an applied rule
         *@param value must be a Integer, this the index in the vector of applied rules.
         *@return a JLabel, if the value parameter is not a Integer then we return an empty label
         *
         */
        public Component getListCellRendererComponent(JList list, Object value, int index, boolean isSelected, boolean cellHasFocus) {
            JLabel label = new JLabel();
            if (value != null) {
                label.setBackground(isSelected ? Color.lightGray : Color.white);
                label.setForeground(isSelected ? Color.red : Color.black);
                label.setOpaque(true);
                if (value instanceof Integer) {
                    Integer index_ = (Integer) value;
                    Integer ruleNumber = (Integer) appliedRulesList.get(index_);
                    label.setText(ruleNumber.toString());
                    label.setIcon(XIPUIUtilities.getIcon(UIConstants.RULE));
                    if (ruleNumber.intValue() != -1) {
                        RuleLoaded rule = ruleManager.getRulebyRuleNb(ruleNumber.intValue());
                        label.setToolTipText(rule.getStringRuleType());
                    } else {
                        label.setText("All rules");
                        label.setToolTipText("All rules");
                    }
                }
                return label;
            } else {
                return new JLabel();
            }
        }
    }

    /**
     *Load analysis
     */
    private class LoadTask extends SwingWorker<XipResult, Object> {

        private boolean full = false;
        private int limitNumber = -1;

        /**
         *Creates the task
         *@param limitNumber the rule limit of the index of a rule among the applyed rules list.
         *@param full if true the applied rules list will be changed, limitNumber is a rule limit and not an index
         */
        public LoadTask(int limitNumber, boolean full) {
            this.full = full;
            this.limitNumber = limitNumber;
        }

        /**
         *Analize a DebugElement, creates the chunk graph.
         *The grammar is reloaded if necessary
         *@return XipResult the xip result of the analysis or null if problem
         *@see setRuleLimit(int)
         *@see setRuleLimitRank(int)
         *@see buildGraph(DebugElement)
         */
        @Override
        protected XipResult doInBackground() {
            Boolean reload = false;
            boolean needReload = false;
            XipResult result = null;
            boolean isIndexLimit = false;
            Project project = getProject();
            try {
                if (project.getGrammar().isModified()) {
                    full = true;
                    needReload = true;
                    if (project.isGrammarLoaded()) {
                        project.reload();
                        publish(true);
                        reload = true;
                    } else {
                        project.load();
                        publish(true);
                        reload = true;
                    }
                } else {
                    reload = true;
                }
                if (reload) {
                    if (full) {
                        if (needReload) {
                            limitNumber = getProject().getRunContext().getGrammarLimit();
                            isIndexLimit = false;
                        //setRuleLimit(ruleLimNumber);
                        } else {
                            isIndexLimit = false;
                        // setRuleLimit(limitNumber);
                        }
                    } else {
                        isIndexLimit = true;
                    //setRuleLimitRank(limitNumber);
                    }
                    DebugElement toParse = menuBar.getSelectedSentence();
                    publish(toParse);
                    result = buildGraph(toParse, isIndexLimit, limitNumber);
                    if (result != null) {
                        chunkGraph.init(result);
                        chunkGraph.run();
                    }
                }
            } catch (XipException e) {
                publish(e);
            } catch (Exception e) {
                publish(e);
            }
            return result;
        }

        @Override
        protected void done() {
            //success or not
            XipResult result = null;
            try {
                result = get();
            } catch (ExecutionException ex) {
                log.error(ex);
                result = null;
            } catch (InterruptedException ex) {
                log.error(ex);
                result = null;
            }
            if (result != null) {
                SessionGraph session = getModuleContext().getSessionGraph(false, getId());
                session.refreshViewer();
                dependencyTable.removeAll();
                dependencyTable.init(result);
                //paint the graph
                if (full) {
                    updateAppliedRules();
                    ruleSlider.setMaximum(appliedRulesList.size() - 1);
                    ruleSlider.setMinimum(0);
                    ruleSlider.setValue(ruleSlider.getMaximum());
                    rulesAppliedLabel.setText(appliedRulesList.size() + " " + labelApplyed + " " + sentenceSelected);
                }
                JComponent component = chunkGraph.getGraph();
                scrollChunk.setViewportView(component);
                scrollChunk.update(scrollChunk.getGraphics());
                scrollChunk.paintAll(scrollChunk.getGraphics());
                scrollChunk.printAll(scrollChunk.getGraphics());
                scrollTable.setViewportView(dependencyTable);
            } else {
                JLabel label = new JLabel("no graph");
                label.setForeground(Color.RED);
                JLabel label2 = new JLabel("no Table");
                label2.setForeground(Color.RED);
                scrollChunk.setViewportView(label);
                scrollTable.setViewportView(label2);
            }
            setInWait(false);
            scrollChunk.repaint();
            splitter.repaint();
        }

        /**
         *This methode construc the chunk graph and the table of dependency.
         *NB : the buildGraph don't reload the grammar !!! so don't use this
         *method if the grammar has some errors.
         */
        private XipResult buildGraph(DebugElement toParse, boolean isIndexLimit, int ruleNumber) throws Exception {
            XipResult xipResult = null;
            //we get the runContext
            Project project = getProject();
            RunContext rctx = project.getRunContext();
            //IndentFile indent = (IndentFile)getProperty("indent_file");
            SessionGraph session = getModuleContext().getSessionGraph(false, getId());
            int gHandler = project.getGhandler();
            //we get xip in order to parse
            Xip xip = project.getXip();
            //We get the DebugElement to parse from the menubar
            //to compare a DebugElement we need to compore with the sentence
            if (toParse != null && !toParse.equals("")) {
                //now we build the xipResult, important the encoding
                DebugXip debug = new DebugXip(xip);
                // for some reasons, xip writes on stdout the chunktree when the method debug is using
                //we unset the display
                xip.setDisplayMode(gHandler, 0);
                xip.addFlag(gHandler, Xip.DISPLAY_SENTENCE_NUMBER);
                xip.addFlag(gHandler, Xip.DISPLAY_DEPENDENCY_NUMBER);
                debug.debug(gHandler, toParse, isIndexLimit, ruleNumber);
                //we re-put the display options
                rctx.loadOptions();
                if (full) {
                    appliedRulesList = debug.getAppliedRules();
                }
                xipResult = debug.getResultBreaked();
                session.initContext();
                session.setProperty(session.NAME, getName());
            }
            return xipResult;
        }

        // @Override not accpeted on mac
        public void process(java.util.List list) {
            Object object = list.get(0);
            if (object instanceof XipException) {
                XipException error = (XipException) object;
                RunManager runner = (RunManager) getProperty(module_run);
                runner.setEnabled(false);
                EditFileAction editFileAction = new EditFileAction(getModuleContext());
                File file = new File(error.getFileName().replace('\\', '/'));
                editFileAction.setFile(file);
                editFileAction.putValue(EditFileAction.ICON, XIPUIUtilities.getIcon(UIConstants.CODE_ERROR));
                editFileAction.putValue(EditFileAction.CARET, error.getPosition());
                editFileAction.setType(EditFileAction.GRAMMAR_EDITOR);
                editFileAction.executeWithoutUI();
            } else if (object instanceof Boolean) {
                GrammarEdit gEdit = (GrammarEdit) getProperty(module_grammar);
                if (gEdit != null) {
                    gEdit.reload();
                }
            } else if (object instanceof DebugElement) {
                DebugElement sentence = (DebugElement) object;
                menuBar.setLabelSentence(sentence.getSentence(), sentence.getFileName() + " : " + sentence.getSentence());
            } else if (object instanceof Exception) {
                severe("ERROR", (Exception) object);
            }
        }
    }
}
