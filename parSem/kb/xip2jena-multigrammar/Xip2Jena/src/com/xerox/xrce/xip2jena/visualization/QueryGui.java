/*
 * Copyright Xerox 2009
 */

package com.xerox.xrce.xip2jena.visualization;

import com.hp.hpl.jena.ontology.OntClass;
import com.hp.hpl.jena.ontology.OntModel;
import com.hp.hpl.jena.ontology.OntModelSpec;
import com.hp.hpl.jena.rdf.model.ModelFactory;
import com.xerox.xrce.xip2jena.DataStore;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.sql.SQLException;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JTextPane;

public class QueryGui extends JPanel implements ActionListener {

    private DataStore db;
    private OntModel ontology;
    private OntClass[] excludeClasses;
    private JTextPane textPane;

    private static final String initialQuery =
            "PREFIX parsem:<http://www.xrce.xerox.com/parsem/ParsemGenericOntology.owl#>\n"
            + "DESCRIBE ?indiv ?mention\n"
            + "WHERE {\n"
            + "?indiv parsem:hasMention ?mention .\n"
            + "}";

    public QueryGui(DataStore db, OntModel ontology, OntClass[] excludeClasses) {
        this.db = db;
        this.ontology = ontology;
        this.excludeClasses = excludeClasses;

        textPane = new JTextPane();
        textPane.setMinimumSize(new Dimension(100, 100));
        textPane.setText(initialQuery);
        textPane.setEditable(true);
        add(textPane);

        JButton goButton = new JButton("Go");
        goButton.addActionListener(this);
        add(goButton);

    }

    public void doQuery(String queryString, OntClass[] excludeClasses)
            throws ClassNotFoundException, SQLException {
        OntModel data = ModelFactory.createOntologyModel(OntModelSpec.OWL_MEM);
        db.queryDescribe(queryString, data);
        OntModel m =  ModelFactory.createOntologyModel(OntModelSpec.OWL_MEM_RDFS_INF);
        m.addSubModel(ontology);
        m.addSubModel(data);
        InstanceBrowser b = new InstanceBrowser(m, excludeClasses);
        b.showInJFrame(JFrame.DISPOSE_ON_CLOSE);

    }

    public static void queryDB(DataStore db, OntModel ontology, OntClass[] excludeClasses) {

        QueryGui qgui = new QueryGui(db, ontology, excludeClasses);

        JFrame jframe = new JFrame();
        jframe.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        jframe.setContentPane(qgui);
        jframe.pack();
        jframe.setVisible(true);
        
    }

    public void actionPerformed(ActionEvent e) {
        try {
            doQuery(textPane.getText(), excludeClasses);
        } catch (ClassNotFoundException ex) {
            //todo
            throw new RuntimeException(ex);
        } catch (SQLException ex) {
            //todo
            throw new RuntimeException(ex);
        }
    }


}
