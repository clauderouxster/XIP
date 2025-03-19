package tests;
//package com.xmlns.foaftown;

import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.Iterator;
import java.util.Vector;

import javax.swing.JFrame;
import javax.swing.JTable;
import javax.swing.table.DefaultTableModel;

import com.hp.hpl.jena.query.QuerySolution;
import com.hp.hpl.jena.query.ResultSet;
import com.hp.hpl.jena.rdf.model.RDFNode;
import java.util.List;
import javax.swing.JScrollPane;

/**
 * Quick and dirty SPARQL results viewer. Cribbed from related code 
 * in Twinkle
 * 
 * Use it as follows:
 * 
 * <code>
 * ResultViewer viewer = new ResultsViewer("Query Results, Packet=" + package.getPacketID());
 * viewer.show();
 * viewer.display(results);
 * </code>
 * 
 * Closing the window should dispose of it correctly
 * 
 * @author Leigh Dodds
 */
public class ResultsViewer extends JFrame 
{
    private JTable _resultsTable;
    private JScrollPane _scrollPane;
    
    public ResultsViewer(String title)
    {
        super(title);
        _resultsTable = new JTable();   
        _scrollPane = new JScrollPane(_resultsTable);
        getContentPane().add(_scrollPane);
        setDefaultWindowSize();
        addWindowListener();
    }
    
    private void addWindowListener()
    {
        addWindowListener(new WindowAdapter() {
            @Override
            @SuppressWarnings("deprecation")
            public void windowClosing(WindowEvent e) 
            {               
                //System.exit(0);
                e.getWindow().hide();
                e.getWindow().dispose();
            }
        });
    }
    
    /**
     * 
     */
    private void setDefaultWindowSize()
    {
        int inset = 400;
        Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
        setBounds(inset, inset, 
                  screenSize.width - inset*2, 
                  screenSize.height-inset*2);
    }
    
    public void display(ResultSet results)
    {
        DefaultTableModel tableModel = new DefaultTableModel(getData(results), getColumns(results));
        _resultsTable.setModel(tableModel);
        _resultsTable.repaint();
    }

    public Vector<Vector> getData(ResultSet data)
    {
        Vector <Vector> results = new Vector <Vector>();
        System.out.println(data.toString());
        for (; data.hasNext(); )
        {
            QuerySolution qs = data.nextSolution();
            results.add( getRowData(data, qs) );
        }
        return results;
    }
    
    private Vector<String> getRowData(ResultSet results, QuerySolution qs)
    {
        Vector <String> row = new Vector <String> ();
        for (Iterator iter = results.getResultVars().iterator() ; iter.hasNext() ; )
        {
            String var = (String)iter.next();
            row.add( getValueAsString(qs, var) );
        }
        return row;
    }
    
    private String getValueAsString(QuerySolution qs, String var)
    {
        RDFNode result = qs.get(var);
        if (result == null)
        {
            return "";
        }
        return result.toString();
    }
    private Vector<String> getColumns(ResultSet results)
    {
        Vector <String> cols = new Vector <String> ();
        for (Iterator iter = results.getResultVars().iterator() ; iter.hasNext() ; )
        {
            String var = (String)iter.next();
            cols.add( var );
        }
        return cols;
    }
    
}
