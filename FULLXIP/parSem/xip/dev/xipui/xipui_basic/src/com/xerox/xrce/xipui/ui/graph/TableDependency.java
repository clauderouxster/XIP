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
package com.xerox.xrce.xipui.ui.graph;

import com.xerox.xrce.xipui.modules.ModuleContext;
import com.xerox.xrce.xipui.util.TableSorter;
import com.xerox.jxip.*;
import javax.swing.*;
import javax.swing.table.*;
import java.awt.*;
import java.util.*;
import org.apache.log4j.*;
import java.awt.event.*;

/**
 *
 * @author  Vianney Grassaud
 */
public class TableDependency extends JPanel implements MouseListener, MouseMotionListener {

    private Logger log = Logger.getLogger("debug");
    private ModuleContext context;
    private int width = 0;
    private int height = 0;
    private int id;
    private int[] sizeColumn;
    /**
     *Arite maximale pour la table des dependances.
     *NB : l'arite est le nombre de parametres pour une dependances
     */
    private static int ARITY = 0;

    /**
     *Constructeur de la table des dependances. Le table des dependances et liee a l'arbre des chunks.
     *Pour partager leur ressources on utilie une session
     *
     * @param properties
     * @param id 
     */
    public TableDependency(ModuleContext properties, int id) {
        this.id = id;
        this.context = properties;
        setBackground(Color.white);
        setLayout(new BorderLayout());
        setBorder(null);
    }

    /**
     *Ajoute une correspondance XipDependency --> XipNode
     *on insere uniquement les index des objets
     * On insere uniquement si la dependance est deja dans la table
     *@param indexDep   dependance
     *@param indexNode  XipNode
     *
     */
    private void addDependencyNode(long indexDep, long indexNode) {
//         Vector v = (Vector)tableDependencies.get(indexDep);
//         if (v !=null) {
//             v.add(indexNode);
//         }
    }

    /***
     *Initialistation de la table des dependances a partir d'une analyse
     *@param result XipResult permettant de lire l'analyse de XIP.
     *
     *Dans cette methode la table est cree et est prete a etre affichee. La table contient
     *des objects ObjectTab
     *
     *NB : on prend toujours la premiere unite du XipResult
     *
     *@see com.xerox.jxip.XipUnit
     *@see com.xerox.jxip.XipDependency
     *@see com.xerox.jxip.XipNode
     *@see com.xerox.xrce.lingdexip.ui.run.graph.TableDependency#addDependencyNode(long, long)
     *@see com.xerox.xrce.lingdexip.ui.run.graph.ObjectTab
     */
    public void init(XipResult result) {
        SessionGraph session = context.getSessionGraph(false, id);
        JTable table = (JTable) session.getProperty(session.TABLE);
        //!! Vector d'ObjecTab
        Vector depRegistry = (Vector) session.getProperty(session.DEP_REGISTRY);
        Hashtable dependencies = new Hashtable();
        session.setDependencies(dependencies);
        XipUnit root = (result.getXipUnits().get(0));
        Iterator ite = root.getDependencies().iterator();
        while (ite.hasNext()) {
            XipDependency dep = (XipDependency) ite.next();
            int arity = dep.getParameters().size();
            if (ARITY <= arity) {
                ARITY = arity;
            }
        }
        Enumeration e = root.getDependencies().elements();
        //instacition des donnees pour la table graphique, uniquement des chaines vides
        Object[][] data = new Object[root.getDependencies().size()][ARITY + 1];
        sizeColumn = new int[ARITY + 1];
        for (int u = 0; u < root.getDependencies().size(); u++) {
            for (int v = 0; v < ARITY + 1; v++) { //on rajoute 1 a l'arite pour la dependance elle meme

                data[u][v] = new Object();
            }
        }
        int i = 0;
        // enumeration des dependances
        while (e.hasMoreElements()) {
            XipDependency dep = (XipDependency) e.nextElement();
            int widthTmp = ((dep.getNameAndParamName().length()) * 12);
            if (width <= widthTmp) {
                width = widthTmp;
            }
            int depLength = dep.getName().toString().length() * 12;
            if (sizeColumn[0] <= depLength) {
                sizeColumn[0] = depLength;
            }
            height += 23;
            // ajout de la dependances pour la table xipdep vers xipnode
            //tableDependencies.put(dep.getIndex(),new Vector());
            //ajout de la dep a la table des dependances
            dependencies.put(dep.getIndex(), dep);
            //creation de l'objectTab que l'on insere dans la table graphique
            ObjectTab o = new ObjectTab(context, this.id, dep);
            //position de la l'objet dans la table
            o.setPosition(i, 0);
            //ajout de l'ObjectTab dans le registre des dependances (necessaire pour communiquer avec l'arbre de chunk)
            depRegistry.add(o);
            //ajout de l'objectTab dans les donnees de la JTable
            data[i][0] = o;
            // on insere maintenant les parametres de la dependance.
            // les parametres sont des XipNode
            Vector params = dep.getParameters();
            for (int j = 0; j < params.size(); j++) {
                XipNode node_ = (XipNode) params.get(j);
                int nodeLength = node_.getSurface().toString().length() * 7;
                if (sizeColumn[j + 1] <= nodeLength) {
                    sizeColumn[j + 1] = nodeLength;
                }
                //creation de l'objectTab
                ObjectTab o_ = new ObjectTab(context, this.id, node_);
                o_.setPosition(i, j + 1);
                //ajout dans le registre
                depRegistry.add(o_);
                data[i][j + 1] = o_;
            }
            i++;
        }
        try {
            TableSorter sorter = new TableSorter(new DependencyModel(data));
            table.setModel(sorter);
            table.setDefaultRenderer(com.xerox.xrce.xipui.ui.graph.ObjectTab.class, new CellsRenderer());
            table.setDefaultRenderer(java.lang.Object.class, new CellsRenderer());
            table.setDefaultEditor(com.xerox.xrce.xipui.ui.graph.ObjectTab.class, new DependencyEditor());
            table.setBounds(5, 5, width, height);
            //  table.setPreferredSize(new Dimension(width,height));
            table.setCellSelectionEnabled(true);
            sorter.setTableHeader(table.getTableHeader());
        } catch (Exception ex) {
            log.warn("error in table of dependencies " + ex);
        }
        //table.setAutoResizeMode(JTable.AUTO_RESIZE_SUBSEQUENT_COLUMNS);
        for (int k = 0; k < table.getColumnCount(); k++) {
            TableColumn tc = table.getColumnModel().getColumn(k);
            int lengthColumnName = table.getColumnName(k).length() * 12;
            if (lengthColumnName >= sizeColumn[k]) {
                tc.setPreferredWidth(lengthColumnName);
                tc.setMaxWidth(lengthColumnName + 20);
            } else {
                tc.setPreferredWidth(sizeColumn[k]);
                tc.setMaxWidth(sizeColumn[k] + 20);
            }
        }
        table.getTableHeader().setBackground(Color.WHITE);
        add(table.getTableHeader(), BorderLayout.PAGE_START);
        add(table, BorderLayout.WEST);
        table.addMouseListener(this);
    //  setBounds(new Rectangle(0,0,width,height));
    //  setPreferredSize(new Dimension(width,height));
    }

    public void mouseEntered(MouseEvent e) {
        SessionGraph session = context.getSessionGraph(false, id);
        JTable table = (JTable) session.getProperty(session.TABLE);
        table.addMouseMotionListener(this);
    }

    public void mouseMoved(MouseEvent e) {
        SessionGraph session = context.getSessionGraph(false, id);
        JTable table = (JTable) session.getProperty(session.TABLE);
        Point point = e.getPoint();
        int column = table.columnAtPoint(point);
        int row = table.rowAtPoint(point);
        if (column != -1 && row != -1) {
            table.editCellAt(row, column);
        }
    }

    public void mouseDragged(MouseEvent e) {
    }

    public void mouseClicked(MouseEvent e) {
    }

    public void mouseExited(MouseEvent e) {
        SessionGraph session = context.getSessionGraph(false, id);
        JTable table = (JTable) session.getProperty(session.TABLE);
        table.removeMouseMotionListener(this);
    }

    public void mousePressed(MouseEvent e) {
    }

    public void mouseReleased(MouseEvent e) {
    }

    public int getTableHeight() {
        return this.height;
    }

    public int getTableWidth() {
        return this.width;
    }

    /**
     *Classe representant le model d'une table de dependances : <br>
     *la premiere colonne correspond aux noms des dependances.<br>
     *les parametres sont au maximum au nombre de 5.
     *
     *
     */
    private class DependencyModel extends AbstractTableModel {

        private String[] columnNames;
        private Object[][] data;

        public DependencyModel(Object[][] data_) {
            super();
            columnNames = constructColumnNames(ARITY);
            this.data = data_;
        }

        /**
         *Cree les noms des colonnes de la table en fonction de l'arite maximale.
         *la premiere colonne a toujours le nom dependency, les autres noms de colonnes sont nomme
         *paramter + num.
         *@param arity arite que l'on veut representer
         *@return tableau contenant les noms des colonnes
         */
        private String[] constructColumnNames(int arity) {
            columnNames = new String[arity + 1];
            columnNames[0] = "Dependency";
            for (int i = 1; i < columnNames.length; i++) {
                columnNames[i] = "parameter " + (i - 1);
            }
            return columnNames;
        }

        /**
         *@return le nombre de colonnes.
         */
        public int getColumnCount() {
            return columnNames.length;
        }

        /**
         *@return le nombre de lignes
         */
        public int getRowCount() {
            return data.length;
        }

        /**
         *@return le nom de la colonne numero i
         */
        public String getColumnName(int col) {
            return columnNames[col];
        }

        /**
         *Renvoie l'objet contenue dans la cellule a la ligne row et a la colonne col
         */
        public Object getValueAt(int row, int col) {
            return data[row][col];
        }

        /*
         * JTable uses this method to determine the default renderer/
         * editor for each cell.  If we didn't implement this method,
         * then the last column would contain text ("true"/"false"),
         * rather than a check box.
         */
        public Class getColumnClass(int c) {
            return getValueAt(0, c).getClass();
        }

        public boolean isCellEditable(int row, int col) {
            //Note that the data/cell address is constant,
            //no matter where the cell appears onscreen.
            Object o = getValueAt(row, col);
            if (col < 0 || !(o instanceof ObjectTab)) {
                return false;
            } else {
                return true;
            }
        }

        public void setValueAt(Object value, int row, int col) {
            data[row][col] = value;
            fireTableCellUpdated(row, col);

        }
    }

    //---------------------------------------------------------------------------------------------------
    //---------------------- Rendu de la Table ----------------------------------------------------------
    /***
     *Classe permmettant de personalise le rendu de la table
     *Une cellule contient un ObjectTab selon les attributs de cet objet on change son rendu
     */
    public class CellsRenderer implements TableCellRenderer {

        /**
         *Le construceur ne fait rien de particulier
         */
        public CellsRenderer() {
        }

        /**
         *Rennoie le composant graphique pour visionner l'objet de la table.
         *Si l'objet est un ObjectTab, alors on le cast et on le retourne. Un object Tab etant un objet graphique
         *sinon on renvoie un JLabel vide;
         *@param obj_ 
         *@return un composant graphique java
         *@see javax.swing.table.TableCellRenderer#getTableCellRendererComponent(JTable, Object ,boolean,boolean,int , int)
         */
        public Component getTableCellRendererComponent(JTable table, Object obj_, boolean isSelected, boolean hasFocus, int row, int column) {
            if (obj_ instanceof ObjectTab) {
                return (ObjectTab) obj_;
            } else {
                JLabel label = new JLabel();
                label.setOpaque(true);
                label.setBackground(new Color(204, 204, 204));
                label.setEnabled(false);
                return label;
            }
        }
    }

    //----------------------------------------------------------------------------------------------------------
    //--------------------------------- Edition sur les elements de la table -----------------------------------
    /***
     *Edition des cellules de la table des dependances
     *
     */
    public class DependencyEditor extends AbstractCellEditor implements TableCellEditor {

        ObjectTab object;//object permettant l'edition

        public DependencyEditor() {
            object = null; //par defaut l'objet est null

        }

        /**
         *Renvoie le composant graphique permettant l'edition
         *@param col 
         *@return Composant graphique
         */
        public Component getTableCellEditorComponent(JTable table, Object value, boolean isSelected, int row, int col) {
            if (value instanceof ObjectTab) {
                object = (ObjectTab) value;
                return object;
            } else {
                JLabel label = new JLabel();
                label.setOpaque(true);
                label.setBackground(new Color(204, 204, 204));
                label.setEnabled(false);
                return label;
            }
        }

        /**
         *renvoie l'objet edite
         *@return ObjectTab ou une chaine de caractere vide si l'objet editer est null
         */
        public Object getCellEditorValue() {
            if (object == null) {
                return new String();
            } else {
                return object;
            }
        }

        /**
         *@see java.lang.Object#toString()
         */
        public String toString() {
            if (object == null) {
                return "null";
            } else {
                return object.toString();
            }
        }
    }
}
