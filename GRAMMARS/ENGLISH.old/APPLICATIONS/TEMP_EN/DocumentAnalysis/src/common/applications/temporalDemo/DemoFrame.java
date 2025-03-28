/*
 * DemoFrame.java
 *
 * Created on May 31, 2007, 1:37 PM
 */

package common.applications.temporalDemo;

import common.Options;
import common.corpora.UnknownCorpusException;
import common.corpora.Corpus;
import java.awt.Point;
import java.io.File;
import java.util.Vector;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.text.Caret;
import javax.swing.text.DefaultStyledDocument;
import javax.swing.text.StyledDocument;

/**
 *
 * @author  xtannier
 */
public class DemoFrame extends JFrame {
    
    private static final long serialVersionUID = 1L;
    
    private Vector<String> corpusNames;
    private Options options;
    private TemporalDemo application;
    
    /**
     * Creates new form DemoFrame
     */
    public DemoFrame(Vector<String> corpusNames, Options options, TemporalDemo application) {
        this.corpusNames = corpusNames;
        this.setOptions(options);
        this.application = application;
        
        // corpus
        String corpusName = options.getCorpusName();
        if (corpusName != null) {
            corpusNames.add(0, corpusName);
        }
        
        initComponents();
        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE); //On dit à l’application de se fermer lors du clic sur la croix
        
        // auto-fill
        if (options.getInputFileName() != null) {
            this.jTextFieldFileName.setText(options.getInputFileName());
        }
    }
    
    /**
     * Clears the UI.
     */
    public void clear(){
        this.jTextPaneInfos.setStyledDocument(new DefaultStyledDocument());
        this.jTextPaneView.setStyledDocument(new DefaultStyledDocument());
    }
    
    /**
     * Launches parsing as if the parse button was clicked
     */
    public void launchParsing(){
        if (!this.jTextFieldFileName.getText().equals(""))
            this.jButtonParseMouseClicked(null);
    }
    
    /**
     * Tests whether Temporal Annotation is enabled.
     *
     * @return true if Temporal Annotation is enabled, false otherwise
     */
    public boolean isTempexprEnabled() {
        return this.jCheckBoxMenuItemTemporalAnnotation.getState();
    }
    
    
    /**
     * Tests whether Named Entities are enabled.
     *
     * @return true if Named Entities are enabled.
     */
    public boolean isNamedEntitiesEnabled() {
        return this.jCheckBoxMenuItemNamedEntities.getState();
    }
    
    
    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc=" Generated Code ">//GEN-BEGIN:initComponents
    private void initComponents() {
        jPanel1 = new javax.swing.JPanel();
        jLabel1 = new javax.swing.JLabel();
        jTextFieldFileName = new javax.swing.JTextField();
        jButtonBrowse = new javax.swing.JButton();
        jLabel2 = new javax.swing.JLabel();
        jComboBoxCorpus = new javax.swing.JComboBox();
        jButtonParse = new javax.swing.JButton();
        jSeparator1 = new javax.swing.JSeparator();
        jSplitPane1 = new javax.swing.JSplitPane();
        jScrollPaneLeft = new javax.swing.JScrollPane();
        jTextPaneView = new javax.swing.JTextPane();
        jScrollPaneRight = new javax.swing.JScrollPane();
        jTextPaneInfos = new javax.swing.JTextPane();
        jLabel3 = new javax.swing.JLabel();
        jLabel4 = new javax.swing.JLabel();
        jMenuBar1 = new javax.swing.JMenuBar();
        jMenu1 = new javax.swing.JMenu();
        jCheckBoxMenuItemTemporalAnnotation = new javax.swing.JCheckBoxMenuItem();
        jCheckBoxMenuItemNamedEntities = new javax.swing.JCheckBoxMenuItem();

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
        setTitle("Extraction Demo");
        jPanel1.setFocusable(false);
        jLabel1.setText("file:");

        jButtonBrowse.setText("Browse");
        jButtonBrowse.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jButtonBrowseMouseClicked(evt);
            }
        });
        jButtonBrowse.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButtonBrowseActionPerformed(evt);
            }
        });

        jLabel2.setText("Corpus:");

        Vector<String> corpusNames = this.getCorpusNames();
        jComboBoxCorpus.setModel(new javax.swing.DefaultComboBoxModel(corpusNames));

        jButtonParse.setText("PARSE");
        jButtonParse.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jButtonParseMouseClicked(evt);
            }
        });

        javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
        jPanel1.setLayout(jPanel1Layout);
        jPanel1Layout.setHorizontalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel1Layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jLabel1)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jTextFieldFileName, javax.swing.GroupLayout.DEFAULT_SIZE, 586, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jButtonBrowse)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jLabel2)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jComboBoxCorpus, javax.swing.GroupLayout.PREFERRED_SIZE, 167, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jButtonParse)
                .addContainerGap())
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel1Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel1)
                    .addComponent(jButtonParse)
                    .addComponent(jComboBoxCorpus, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabel2)
                    .addComponent(jButtonBrowse)
                    .addComponent(jTextFieldFileName, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        jSplitPane1.setDividerLocation(500);
        jTextPaneView.setEditable(false);
        jTextPaneView.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jTextPaneViewMouseClicked(evt);
            }
        });

        jScrollPaneLeft.setViewportView(jTextPaneView);

        jSplitPane1.setLeftComponent(jScrollPaneLeft);

        jTextPaneInfos.setEditable(false);
        jTextPaneInfos.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jTextPaneInfosMouseClicked(evt);
            }
        });

        jScrollPaneRight.setViewportView(jTextPaneInfos);

        jSplitPane1.setRightComponent(jScrollPaneRight);

        jLabel3.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        jLabel3.setText("File Text");
        jLabel3.setFocusable(false);

        jLabel4.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        jLabel4.setText("Informations");
        jLabel4.setFocusable(false);

        jMenu1.setText("Options");
        jCheckBoxMenuItemTemporalAnnotation.setSelected(true);
        jCheckBoxMenuItemTemporalAnnotation.setText("Temporal Annotation");
        jCheckBoxMenuItemTemporalAnnotation.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                jCheckBoxMenuItemTemporalAnnotationMouseClicked(evt);
            }
        });
        jCheckBoxMenuItemTemporalAnnotation.addItemListener(new java.awt.event.ItemListener() {
            public void itemStateChanged(java.awt.event.ItemEvent evt) {
                jCheckBoxMenuItemTemporalAnnotationItemStateChanged(evt);
            }
        });

        jMenu1.add(jCheckBoxMenuItemTemporalAnnotation);

        jCheckBoxMenuItemNamedEntities.setSelected(true);
        jCheckBoxMenuItemNamedEntities.setText("Named Entities");
        jCheckBoxMenuItemNamedEntities.addItemListener(new java.awt.event.ItemListener() {
            public void itemStateChanged(java.awt.event.ItemEvent evt) {
                jCheckBoxMenuItemNamedEntitiesItemStateChanged(evt);
            }
        });
        jCheckBoxMenuItemNamedEntities.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jCheckBoxMenuItemNamedEntitiesActionPerformed(evt);
            }
        });

        jMenu1.add(jCheckBoxMenuItemNamedEntities);

        jMenuBar1.add(jMenu1);

        setJMenuBar(jMenuBar1);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jPanel1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addComponent(jSplitPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 1048, Short.MAX_VALUE)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addComponent(jLabel3, javax.swing.GroupLayout.PREFERRED_SIZE, 496, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 15, Short.MAX_VALUE)
                .addComponent(jLabel4, javax.swing.GroupLayout.PREFERRED_SIZE, 537, javax.swing.GroupLayout.PREFERRED_SIZE))
            .addComponent(jSeparator1, javax.swing.GroupLayout.DEFAULT_SIZE, 1048, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jPanel1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jSeparator1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 16, Short.MAX_VALUE)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel4)
                    .addComponent(jLabel3))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jSplitPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 608, Short.MAX_VALUE))
        );
        pack();
    }// </editor-fold>//GEN-END:initComponents
    
    private void jCheckBoxMenuItemNamedEntitiesActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jCheckBoxMenuItemNamedEntitiesActionPerformed
// TODO add your handling code here:
    }//GEN-LAST:event_jCheckBoxMenuItemNamedEntitiesActionPerformed
    
    private void jCheckBoxMenuItemNamedEntitiesItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_jCheckBoxMenuItemNamedEntitiesItemStateChanged
        this.application.SetNEAnnotation(jCheckBoxMenuItemNamedEntities.getState());
        if (jCheckBoxMenuItemNamedEntities.getState()){
            this.application.updateStyledDocumentWithNamedEntities(this.options,
                    this.jTextPaneView.getStyledDocument());
        } else {
            this.application.clear();
            this.jTextPaneView.setStyledDocument(this.application.getStyledDocument(this.options));
        }
        this.jTextPaneInfos.setStyledDocument(new DefaultStyledDocument());
    }//GEN-LAST:event_jCheckBoxMenuItemNamedEntitiesItemStateChanged
    
    private void jCheckBoxMenuItemTemporalAnnotationItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_jCheckBoxMenuItemTemporalAnnotationItemStateChanged
        this.application.SetTemporalAnnotation(jCheckBoxMenuItemTemporalAnnotation.getState());
        if (jCheckBoxMenuItemTemporalAnnotation.getState()){
            this.application.updateStyledDocumentWithTemporalExpressions(this.options,
                    this.jTextPaneView.getStyledDocument());
        } else {
            this.application.clear();
            this.jTextPaneView.setStyledDocument(this.application.getStyledDocument(this.options));
        }
        this.jTextPaneInfos.setStyledDocument(new DefaultStyledDocument());
    }//GEN-LAST:event_jCheckBoxMenuItemTemporalAnnotationItemStateChanged
    
    private void jCheckBoxMenuItemTemporalAnnotationMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_jCheckBoxMenuItemTemporalAnnotationMouseClicked
        
    }//GEN-LAST:event_jCheckBoxMenuItemTemporalAnnotationMouseClicked
    
    
    
    
    private void jTextPaneInfosMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_jTextPaneInfosMouseClicked
        Point p = evt.getPoint();
        Caret caret = this.jTextPaneInfos.getCaret();
        caret.setMagicCaretPosition(p);
        int pos = caret.getDot();
//        StyledDocument doc = this.jTextPaneInfos.getStyledDocument();
//        Element el = doc.getCharacterElement(pos);
//        Offset off = new Offset(el.getStartOffset(),el.getEndOffset());
        StyledDocument docView =
                this.application.highlightStyledDocumentFromViewPosition((long)pos,
                this.jTextPaneView.getStyledDocument());
        this.jTextPaneView.setStyledDocument(docView);
    }//GEN-LAST:event_jTextPaneInfosMouseClicked
    
    private void jTextPaneViewMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_jTextPaneViewMouseClicked
        Point p = evt.getPoint();
        Caret caret = this.jTextPaneView.getCaret();
        caret.setMagicCaretPosition(p);
        int pos = caret.getDot();
//        StyledDocument doc = this.jTextPaneView.getStyledDocument();
//        Element el = doc.getCharacterElement(pos);
//        Offset off = new Offset(el.getStartOffset(),el.getEndOffset());
        
        
        // UNhighlight everything in the view document
//        SimpleAttributeSet attr = new SimpleAttributeSet();
//        StyleConstants.setBackground(attr, Color.WHITE);
//        this.jTextPaneView.getStyledDocument().setCharacterAttributes(0,
//                this.jTextPaneView.getStyledDocument().getLength(), attr, false);
        
        DefaultStyledDocument docInfos = this.application.getStyledInformations((long)pos, this.options);
        this.jTextPaneInfos.setStyledDocument(docInfos);
    }//GEN-LAST:event_jTextPaneViewMouseClicked
    
    private void jButtonParseMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_jButtonParseMouseClicked
        
        // disable Parse button
        this.jButtonParse.setEnabled(false);
        
        String fileName = this.jTextFieldFileName.getText();
//            fileName = "/home/xtannier/Corpus/Biographies_VIKEF/Cysneiros_2004.txt";
        
        // test file name
        File file = new File(fileName);
        if (!file.isFile()) {
            JOptionPane.showMessageDialog(this, new String("File not found!"),
                    "Error", JOptionPane.WARNING_MESSAGE);
            this.jButtonParse.setEnabled(true);
            return;
        }
//            if (!(new File(fileName)).)
        if (evt != null)
            this.application.clear();
        
        // corpus
        String corpusName = this.jComboBoxCorpus.getSelectedItem().toString();
        
//            corpusName = "Biographies";
        
        if (!corpusName.startsWith("None")){
            this.options.setProperty(Options.CORPUS, corpusName);
            if (this.options.getProperty(Options.CORPUS) != null)
                
                try {
                    Corpus corpus = Corpus.getNewCorpus(options);
                    this.options.setCorpus(corpus);                    
                } catch (UnknownCorpusException ex) {
                    ex.printStackTrace();
                    this.jButtonParse.setEnabled(true);
                }
        } else {
            this.options.setCorpus(new Corpus());
        }
        
        // file name
        this.options.setInputFileName(fileName);
        
        // parse
        this.application.launchParsing(this.options);
        
        
        // document view
        StyledDocument docView = this.application.getStyledDocument(options);
        this.jTextPaneView.setStyledDocument(docView);
        
        // information view
        DefaultStyledDocument docInfos = this.application.getStyledInformations(-1, options);
        this.jTextPaneInfos.setStyledDocument(docInfos);
        
        // enable Parse button
        this.jButtonParse.setEnabled(true);
        
        
        
    }//GEN-LAST:event_jButtonParseMouseClicked
    
    private void jButtonBrowseMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_jButtonBrowseMouseClicked
        JFileChooser chooser = new JFileChooser();//création dun nouveau filechosser
        chooser.setApproveButtonText("Select"); //intitulé du bouton
//        chooser.showOpenDialog(null); //affiche la boite de dialogue
        if (chooser.showOpenDialog(null) == JFileChooser.APPROVE_OPTION) {
            this.jTextFieldFileName.setText(chooser.getSelectedFile().getAbsolutePath()); //si un fichier est selectionné, récupérer le fichier puis sont path et l'afficher dans le champs de texte
        }
    }//GEN-LAST:event_jButtonBrowseMouseClicked
    
    
    
    
    
    
    private void jButtonBrowseActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonBrowseActionPerformed
        
    }//GEN-LAST:event_jButtonBrowseActionPerformed
    
    /**
     * @param args the command line arguments
     */
    public static void main(String args[]) {
        java.awt.EventQueue.invokeLater(new Runnable() {
            public void run() {
                new DemoFrame(new Vector<String>(), null, null).setVisible(true);
            }
        });
    }
    
    
    private Vector<String> getCorpusNames(){
        return this.corpusNames;
    }
    
    /**
     * Sets the demo frame parser options
     *
     * @param options the parser options
     */
    public void setOptions(Options options) {
        this.options = options;
    }
    
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton jButtonBrowse;
    private javax.swing.JButton jButtonParse;
    private javax.swing.JCheckBoxMenuItem jCheckBoxMenuItemNamedEntities;
    private javax.swing.JCheckBoxMenuItem jCheckBoxMenuItemTemporalAnnotation;
    private javax.swing.JComboBox jComboBoxCorpus;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JLabel jLabel3;
    private javax.swing.JLabel jLabel4;
    private javax.swing.JMenu jMenu1;
    private javax.swing.JMenuBar jMenuBar1;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JScrollPane jScrollPaneLeft;
    private javax.swing.JScrollPane jScrollPaneRight;
    private javax.swing.JSeparator jSeparator1;
    private javax.swing.JSplitPane jSplitPane1;
    private javax.swing.JTextField jTextFieldFileName;
    private javax.swing.JTextPane jTextPaneInfos;
    private javax.swing.JTextPane jTextPaneView;
    // End of variables declaration//GEN-END:variables
    
}
