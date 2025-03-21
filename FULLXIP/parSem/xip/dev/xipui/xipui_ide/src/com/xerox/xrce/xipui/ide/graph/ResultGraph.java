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
package com.xerox.xrce.xipui.ide.graph;

import com.xerox.xrce.xipui.ui.graph.*;
import com.xerox.xrce.xipui.ide.ui.*;
import com.xerox.xrce.xipui.Project;
import com.xerox.xrce.xipui.RunContext;
import com.xerox.xrce.xipui.Xip;
import com.xerox.xrce.xipui.ui.grammar.IndentFile;

import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.ui.AbstractResult;
import java.util.*;

import javax.swing.*;
import java.awt.*;

import com.xerox.jxip.*;
import com.xerox.xrce.xipui.ide.DebugXip;

/**
 * Represente le resultat de XIP sous format graphique <u>pour une phrase parsee</u>
 * Le resultat est represente par un le graphe de chunk ainsi que le graphe des dependances.
 * @author Vianney Grassaud
 */
public class ResultGraph extends AbstractResult {

    /**
     *Vector of applyed rule, vector of rules
     */
    private Vector appliedRules;
    // the chunk graph
    protected ChunkGraph chunk;
    //the table of dependencies
    protected TableDependency dependencyTable;
    JScrollPane scrollerDep;
    JScrollPane scrollerChunk;

    /**
     *Create a new instance of a ResultGraph
     *@param properties table of properties to exchange some object
     *@param id the identifier for this result graph
     * @throws java.lang.Exception 
     */
    public ResultGraph(ModuleContext properties, int id) throws Exception {
        super(properties, id);
        try {
            setLayout(new BorderLayout());
            initComponents();
            //removeAll();
            //get the graphical session for this resultGraph
            //if session was null, a new session is returned
            SessionGraph session = getModuleContext().getSessionGraph(id);
            this.listCorpusElement = new Vector();
            this.appliedRules = new Vector();
            setResultName();
            //instanciation of the ChunkGraph with the properties and session
            chunk = new ChunkGraph(getModuleContext(), id);
            //instanciation of the table of dependencies
            dependencyTable = new TableDependency(getModuleContext(), id);
            //we put some parameter to the session
            session.setProperty(session.NAME, getResultName());
            setBorder(null);
        } catch (Exception e) {
            throw e;
        }
    }

    private void initComponents() {
        java.awt.GridBagConstraints gridBagConstraints;
        remove(toolBar);
        scrollerDep = new JScrollPane();
        scrollerChunk = new JScrollPane();
        setLayout(new java.awt.GridBagLayout());
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 4, 2);
        add(scrollerChunk, gridBagConstraints);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(4, 2, 2, 2);
        add(scrollerDep, gridBagConstraints);
    }

    public void setResultName() {
        this.resultName = "Parse (" + getId() + ")";
    }

    /**
     *Ferme tout interactivite avec se composant c a d supprime tous les panneaux
     * graphgique permettant d'afficher un graphe
     * L'arbre de chunk n'est plus affiche ainsi que la table des dependances
     * le ruleviewer pour la generation de code n'est plus disponible pour cette execution
     * les onglets optionnels pour la generation de code ne sont plus disponibles
     * on vide la session.
     */
    public void closeModule() {
        // suppression des objets graphiques dans le target et l'input :
        SessionGraph session = getModuleContext().getSessionGraph(false, getId());
        this.isClosed = true;
        //vidage memoire de la session :
        if (session != null) {
            session.invalidate();
        }
        this.chunk = null;
        this.toolBar = null;
        this.dependencyTable = null;
        this.appliedRules = null;
        this.removeNotify();
    }

    /**
     *Put the chunk graph and the table of dependencies in the splitter
     */
    protected void putGraphInSplitter() {
        scrollerDep.setViewportView(dependencyTable);
        scrollerChunk.setViewportView(chunk.getGraph());
        validate();
        update(getGraphics());
        paintAll(getGraphics());
        printAll(getGraphics());
        repaint();
    }

    /**
     *This methode construc the chunk graph and the table of dependency.
     *NB : the buildGraph don't reload the grammar !!! so don't use this
     *methode if the grammar has some errors.
     *
     *@param toParse 
     * @param isIndexLimit 
     * @param ruleLimit 
     * @see com.xerox.xrce.lingdexip.ui.run.ResultGraph#buildGraph(Vector)
     */
    public void buildGraph(DebugElement toParse, boolean isIndexLimit, int ruleLimit) {
        try {
            //we get the runContext
            Project project = getProject();
            RunContext rctx = project.getRunContext();
            IndentFile indent = (IndentFile) getProperty("indent_file");
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
                debug.debug(gHandler, toParse, isIndexLimit, ruleLimit);
                //we re-put the display options
                rctx.loadOptions();
                appliedRules = debug.getAppliedRules();
                //xip.buildXipResultFromFile(gHandler,toParse.getFileForParse(),toParse.getEncoding());
                //XipResult xipResult = xip.getXipResult();
                XipResult xipResult = debug.getResultBreaked();
                // we get the applied rules for this sentence
                // appliedRules = setAppliedRules(gHandler);
                // we reinit the session
                session.initContext();
                session.setProperty(session.NAME, getName());
                session.refreshViewer();
                //refresh
                //initContext the graph, read the xipresult
                chunk.init(xipResult);
                //paint the graph
                chunk.run();
                dependencyTable.removeAll();
                dependencyTable.init(xipResult);
                //we put the chunkgraph and the table in the resultGraph
                putGraphInSplitter();
            //  chunk.repaint();
            }

        } catch (Exception e) {
            severe("ERROR", e);
        }
    }

    /**
     *Renvoie la liste des regles appliquee sur la phrase.
     *@return une liste d'entier correspondant au numeros des regles appliquees
     */
    public Vector getAppliedRules() {
        return this.appliedRules;
    }
}
