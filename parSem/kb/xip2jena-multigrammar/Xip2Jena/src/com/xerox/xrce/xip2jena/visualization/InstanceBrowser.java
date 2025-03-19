/*
 * Copyright Xerox 2009
 */
package com.xerox.xrce.xip2jena.visualization;

import com.hp.hpl.jena.ontology.Individual;
import com.hp.hpl.jena.ontology.OntClass;
import com.hp.hpl.jena.ontology.OntModel;
import com.hp.hpl.jena.ontology.OntModelSpec;
import com.hp.hpl.jena.rdf.model.Model;
import com.hp.hpl.jena.rdf.model.ModelFactory;
import com.hp.hpl.jena.rdf.model.RDFNode;
import com.hp.hpl.jena.rdf.model.StmtIterator;
import com.xerox.xrce.xip2jena.DataStore;
import com.xerox.xrce.xip2jena.ParsemGenericOntology;
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.HeadlessException;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.InputStreamReader;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import javax.swing.AbstractListModel;
import javax.swing.JFrame;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextPane;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.text.BadLocationException;
import javax.swing.text.DefaultHighlighter;
import javax.swing.text.Highlighter;
import javax.swing.text.LayeredHighlighter;
import org.apache.log4j.Logger;

public class InstanceBrowser extends JPanel {

    private JList mentionList;
    private JTextPane textPane;
    private LayeredHighlighter.LayerPainter highlightPainter;
    private InstanceDisplay graphDisplay;
    private Logger log = Logger.getLogger(InstanceBrowser.class);

    public InstanceBrowser(OntModel m, OntClass[] excludeClasses) {
        setLayout(new BorderLayout());
        graphDisplay = new InstanceDisplay(m, excludeClasses, new TreeSelectCallback());

        mentionList = new JList(new IndividualListModel(new ArrayList<Individual>(0)));
        mentionList.addListSelectionListener(new ListSelectionListener() {

            @Override
            public void valueChanged(ListSelectionEvent e) {
                int index = -1;
                for (int i = 0; i < mentionList.getModel().getSize(); i++) {
                    if (mentionList.getSelectionModel().isSelectedIndex(i)) {
                        index = i;
                        break;
                    }
                }
                if (index == -1) {
                    textPane.setText("");
                } else {
                    String urlString;
                    Individual mention = (Individual) mentionList.getModel().getElementAt(index);
                    try {
                        urlString = mention.getProperty(ParsemGenericOntology.hasUrl).getString();
                        URL url = new URL(urlString);
                        BufferedReader reader =
                                new BufferedReader(new InputStreamReader(url.openStream()));
                        StringBuilder sb = new StringBuilder();
                        String line;
                        while ((line = reader.readLine()) != null) {
                            sb.append(line);
                        }
                        textPane.setText(sb.toString());
                    } catch (Exception ex) {
                        log.warn("Couldn't retrieve document text for mention " + mention, ex);
                        return;
                    }
                    for (int i = 0; i < mentionList.getModel().getSize(); i++) {
                        if (mentionList.getSelectionModel().isSelectedIndex(i)) {
                            mention = (Individual) mentionList.getModel().getElementAt(i);
                            if (mention.
                                    getProperty(ParsemGenericOntology.hasUrl).
                                    getString().
                                    equals(urlString)) {
                                    long start = mention.getProperty(ParsemGenericOntology.hasStart).getLong();
                                    long end = mention.getProperty(ParsemGenericOntology.hasEnd).getLong();
                                    Highlighter h = textPane.getHighlighter();
                                try {
                                    h.addHighlight((int) start, (int) end, highlightPainter);
                                } catch (BadLocationException ex) {
                                    log.warn(ex);
                                }
                            }
                        }
                    }

                }
            }
        });
        JScrollPane listScrollPane = new JScrollPane();
        listScrollPane.setViewportView(mentionList);
        mentionList.setPreferredSize(new Dimension(100,100));

        textPane = new JTextPane();
        textPane.setEditable(false);
        textPane.setMinimumSize(new Dimension(0,100));
        highlightPainter = new DefaultHighlighter.DefaultHighlightPainter(new Color(0, 0, 255, 50));
        JScrollPane textScrollPane = new JScrollPane();
        textScrollPane.setViewportView(textPane);

        add(graphDisplay, BorderLayout.CENTER);
        add(textScrollPane, BorderLayout.PAGE_END);
        add(listScrollPane, BorderLayout.LINE_END);

    }

    private class TreeSelectCallback implements SelectCallback {

        @Override
        public void select(Individual i) {
            List<Individual> mentions = new ArrayList<Individual>();
            StmtIterator stmts = i.listProperties(ParsemGenericOntology.hasMention);
            while (stmts.hasNext()) {
                RDFNode n = stmts.nextStatement().getObject();
                if (n.canAs(Individual.class)) {
                    Individual mention = (Individual) n.as(Individual.class);
                    if (mention.hasOntClass(ParsemGenericOntology.Mention)) {
                        mentions.add(mention);
                    } else {
                        log.warn(i.getURI() + " is not a Mention");
                    }
                } else {
                    log.warn(n.toString() + " is not a Mention");
                }
            }
            mentionList.setModel(new IndividualListModel(mentions));
            mentionList.getSelectionModel().addSelectionInterval(0, mentionList.getModel().getSize());
        }
    }

    private class IndividualListModel extends AbstractListModel {

        private List<Individual> individuals;

        public IndividualListModel(List<Individual> individuals) {
            this.individuals = individuals;
        }

        @Override
        public int getSize() {
            return individuals.size();
        }

        @Override
        public Object getElementAt(int index) {
            return individuals.get(index);
        }
    }

    public void showInJFrame(int closeAction) throws HeadlessException {
        JFrame jframe = new JFrame();
        jframe.setContentPane(this);
        jframe.setTitle("Xip2Jena instance browser");
        jframe.setDefaultCloseOperation(closeAction);
        jframe.pack();
        jframe.setVisible(true);
    }

    public static OntModel buildModel(Model data, Class schemagenClass) {
        OntModel ont = ModelFactory.createOntologyModel(OntModelSpec.OWL_MEM);
        OntModel m = ModelFactory.createOntologyModel(OntModelSpec.OWL_MEM_RDFS_INF);
        ont.read(schemagenClass.getResource(schemagenClass.getSimpleName() + ".owl").toString());
        m.addSubModel(ont);
        m.addSubModel(data);
        return m;
    }

    public static void visualizeFile(File file, Class schemagenClass,
            OntClass[] excludeClasses) throws FileNotFoundException {
        visualizeFile(file, schemagenClass, excludeClasses, JFrame.EXIT_ON_CLOSE);
    }

    public static void visualizeFile(File file, Class schemagenClass, 
            OntClass[] excludeClasses, int closeAction)
            throws FileNotFoundException {
        OntModel data = ModelFactory.createOntologyModel(OntModelSpec.OWL_MEM);
        data.read(new FileInputStream(file), null);
        OntModel m = buildModel(data, schemagenClass);
        InstanceBrowser b = new InstanceBrowser(m, excludeClasses);
        b.showInJFrame(closeAction);
    }

    public static void visualizeDB(DataStore db, Class schemagenClass,
            OntClass[] excludeClasses) throws FileNotFoundException {
        visualizeDB(db, schemagenClass, excludeClasses, JFrame.EXIT_ON_CLOSE);
    }

    public static void visualizeDB(DataStore db, Class schemagenClass,
            OntClass[] excludeClasses, int closeAction) {
        // copy the DB model into a memory model, otherwise everything is real slow.
        OntModel data = ModelFactory.createOntologyModel(OntModelSpec.OWL_MEM);
        data.add(db.getDefaultModel());
        OntModel m = buildModel(data, schemagenClass);
        InstanceBrowser b = new InstanceBrowser(m, excludeClasses);
        b.showInJFrame(closeAction);
    }
    
}

