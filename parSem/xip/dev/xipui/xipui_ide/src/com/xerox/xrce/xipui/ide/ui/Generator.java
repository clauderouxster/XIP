/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * Generator.java
 *
 * Created on May 22, 2006, 11:57 AM
 */
package com.xerox.xrce.xipui.ide.ui;

import com.xerox.xrce.xipui.ide.generator.ui.SelectionRulePanel;
import com.xerox.xrce.xipui.ui.graph.*;
import com.xerox.xrce.xipui.ide.graph.*;
import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.UIConstants;
import com.xerox.xrce.xipui.ui.XIPUIUtilities;
import java.awt.*;
import java.util.*;
import javax.swing.*;

/**
 *
 * @author  vgrassau
 */
public class Generator extends DebugViewer {

    public static final String generator_module = "generator_module";
    private ViewerSelectorGraph selectorGraph;

    /** Creates new form BeanForm
     * @param context
     * @param id 
     */
    public Generator(ModuleContext context, int id) {
        super(context, id);
        mainColor = new Color(177, 183, 255);
        initComponents();
        applyColor(mainColor);
        SessionGraph session = getModuleContext().getSessionGraph(false, id);
        if (session == null) {
            warning("Session is null");
        } else {
            //we must set visible the option panel tab  
            selectorGraph = new ViewerSelectorGraph(getModuleContext(), id);
            JComponent c = (JComponent) getProperty(OPTION_TABBED);
            selectorGraph.setRootComponent(c);
            session.setViewer(selectorGraph);
        }
        JTabbedPane output = (JTabbedPane) getProperty(Generator.OUTPUT_TABBED);
        SelectionRulePanel selection = new SelectionRulePanel(getModuleContext(), id);
        output.addTab("Generator(" + getId() + ")", XIPUIUtilities.getIcon(UIConstants.GENERATOR), selection);
        initPropRuleContext();
        output.setSelectedComponent(selection);
        setProperty(generator_module, this);
    }

    /**
     *Init the debugManager, we need to rewrite this method because
     *we need to have a specific icon for the generator.
     *The super method initDebugManager is called
     */
    protected void initDebugManager() {
        setIcon(XIPUIUtilities.getIcon(UIConstants.GENERATOR));
        super.initDebugManager();
    }

    protected void initPropRuleContext() {
        this.setProperty("choiceRule", "Dependency");
        this.setProperty("dependency", "");
        this.setProperty("arity", "1");
        this.setProperty("vectDependencySelect", new Vector());
        this.setProperty("vectFocusSelect", new Vector());
        this.setProperty("category", "");
        this.setProperty("vectBodySelect", new Vector());
        this.setProperty("vecFeature", new Vector());
        this.setProperty("vectHeadSelect", new Vector());
        // vector de NodeGraph
        this.setProperty("vectKeepNodeGraph", new Vector());
        // vector de SelectedObject
        this.setProperty("vectKeepSelect", new Vector());
        this.setProperty("vectLeftSelect", new Vector());
        this.setProperty("vectRightSelect", new Vector());
        this.setProperty("vectMarkSelect", new Vector());
        this.setProperty("longestMatch", "false");
        this.setProperty("indexlastword", "false");
        this.setProperty("leftBody", "");
        this.setProperty("rightBody", "");
        this.setProperty("minBody", "");
        this.setProperty("leftHead", "");
        this.setProperty("rightHead", "");
        int[] indexbody = {};
        this.setProperty("bodySelect", indexbody);
        int[] headtag = {};
        this.setProperty("headSelect", headtag);
        int[] keeptag = {};
        this.setProperty("keepSelect", keeptag);
        int[] indexleft = {};
        this.setProperty("leftSelect", indexleft);
        int[] indexright = {};
        this.setProperty("rightSelect", indexright);
        int[] indexdep = {};
        this.setProperty("depSelect", indexdep);
        int[] indexmark = {};
        this.setProperty("markSelect", indexmark);
        this.setProperty("stop", "false");
        String rule = "";
        this.setProperty("rule", rule);
    }

    /**
     *The name for this result. We rewrite the super methode
     *
     */
    public void setResultName() {
        this.resultName = "Generator (" + getId() + ")";
    }

    protected void showPanels(boolean b) {
        super.showPanels(b);
        JTabbedPane optionTabbed = (JTabbedPane) getProperty(OPTION_TABBED);
        JSplitPane leftSplit = (JSplitPane) getProperty(LEFT_SPLIT);
        if (!b) {
            leftSplit.setDividerSize(0);
        } else {
            Rectangle rec = leftSplit.getBounds();
            leftSplit.setDividerLocation((int) (rec.getHeight() / 2));
            leftSplit.setDividerSize(3);
        }
        optionTabbed.setVisible(b);
        DebugElement de = menuBar.getSelectedSentence();
        if (de != null) {
            String sentence = de.getSentence();
            this.setProperty("sentence", sentence);
        }
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc=" Generated Code ">//GEN-BEGIN:initComponents
    private void initComponents() {

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

    }// </editor-fold>//GEN-END:initComponents

    private void formFocusGained(java.awt.event.FocusEvent evt) {//GEN-FIRST:event_formFocusGained
        showPanels(true);
    }//GEN-LAST:event_formFocusGained

    private void formComponentShown(java.awt.event.ComponentEvent evt) {//GEN-FIRST:event_formComponentShown
        requestFocusInWindow();
    }//GEN-LAST:event_formComponentShown

    /**
     *Close the debugViewer
     */
    public void closeModule() {
        showPanels(false);
        super.closeModule();
        this.selectorGraph.close();
        initPropRuleContext();
        JTabbedPane output = (JTabbedPane) getProperty(OUTPUT_TABBED);
        int index = output.indexOfTab("Generator(" + getId() + ")");
        if (index != -1) {
            output.remove(index);
        } else {
            warning("index not found");
        }
    }
    // Variables declaration - do not modify//GEN-BEGIN:variables
    // End of variables declaration//GEN-END:variables
}
