/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * DefaultEditor.java
 *
 * Created on January 12, 2006, 1:32 PM
 */
package com.xerox.xrce.xipui.ui.editor;

import com.xerox.xrce.xipui.Offset;
import com.xerox.xrce.xipui.modules.*;
import com.xerox.xrce.xipui.ui.*;
import com.xerox.xrce.xipui.util.FileTools;
import java.awt.*;
import java.awt.event.KeyEvent;
import javax.swing.*;
import javax.swing.text.*;
import javax.swing.event.*;
import javax.swing.undo.*;
import java.io.*;

/**
 * TODO : use the ChangePropertySupport in futur implementation
 * @author  vgrassau
 */
public class DefaultEditor extends PanelModule implements Editor {

    protected JTextPane text;
    // fichier a editer
    protected File file;
    // la representation du document
    protected DefaultStyledDocument doc;
    private String encoding;
    protected AttributeSet currentAttr;
    private UndoAction undoAction;
    private RedoAction redoAction;
    protected UndoManager undoM;
    protected LineColor lineColor;
    protected EditMenu editMenu;
    protected FileMenu fileMenu;
    protected UndoableEditListener undoListener;
    protected EditorListener editorListener;
    protected Icon icon;
    protected boolean withFileMenu = true;
    protected boolean withEditMenu = true;

    public DefaultEditor(ModuleContext properties, String encoding, boolean wrap) {
        this(properties, encoding, wrap, true, true);
    }

    /** Creates new form DefaultEditor
     * @param properties
     * @param encoding
     * @param wrap
     * @param withFileMenu
     * @param withEditMenu 
     */
    public DefaultEditor(ModuleContext properties, String encoding, boolean wrap, boolean withFileMenu, boolean withEditMenu) {
        super(properties);
        this.isClosed = true;
        /// init module :
        this.encoding = encoding;
        this.withEditMenu = withEditMenu;
        this.withFileMenu = withFileMenu;
        this.setName(getClass().getName());
        JMenuBar mb = (JMenuBar) properties.get(DefaultEditor.MENUBAR);
        editMenu = (EditMenu) mb.getComponent(1);
        fileMenu = (FileMenu) mb.getComponent(0);
        lineColor = new LineColor();
        initComponents();
        undoM = new UndoManager();
        undoAction = new UndoAction(undoM, redoAction);
        redoAction = new RedoAction(undoM, undoAction);
        undoAction.setRedoAction(redoAction);
        this.saveAs.setIcon(XIPUIUtilities.getIcon(UIConstants.SAVE_ALL));
        this.saveAs.setToolTipText("Save As");
        this.saveAs.setText("");
        // initialisation des composants pour editer
        if (wrap) {
            text = new JTextPane();
        } else {
            text = new JTextPane() {

                public void setSize(Dimension d) {
                    if (getParent() != null) {
                        if (d.width < getParent().getSize().width) {
                            d.width = getParent().getSize().width;
                        }
                    }
                    super.setSize(d);
                }

                public boolean getScrollableTracksViewportWidth() {
                    return false;
                }
            };
        }
        scroller.add(text);
        //scroller.setViewportView(text);
        // SyntaxSupport support =  SyntaxSupport.getInstance();
        //SyntaxSupport.getInstance().addSupport(SyntaxSupport.JAVA_LEXER , text);
        //text.getDocument().putProperty(SyntaxDocument.tabSizeAttribute, new Integer(2));
        scroller.setViewportView(text);
        scroller.repaint();
        text.addCaretListener(this);
        text.setVisible(true);
        text.setEditorKit(lineColor);
        text.setFocusable(true);
        doc = (DefaultStyledDocument) text.getStyledDocument();
        reloadMenu();
        text.addFocusListener(new java.awt.event.FocusAdapter() {

            public void focusGained(java.awt.event.FocusEvent evt) {
                formFocusGained(evt);
            }
        });
        text.addKeyListener(new java.awt.event.KeyAdapter() {

            public void keyReleased(java.awt.event.KeyEvent evt) {
                formKeyReleased(evt);
            }
        });
        currentAttr = Styles.getStyle(Styles.STYLE_NORMAL);
        Font font = new Font(StyleConstants.getFontFamily(currentAttr), Font.PLAIN, StyleConstants.getFontSize(currentAttr));
        text.setFont(font);
        // eviter confusion entre numero de ligne effectif et numero de ligne "wrappee" (affichee)
        if (!wrap) {
            scroller.setRowHeaderView(new Gutter(text, scroller));
        }
        scroller.getVerticalScrollBar().setUnitIncrement(10);
        scroller.getVerticalScrollBar().setBlockIncrement(1);
    }

    /**
     *Returns the caret of the editor
     *@return Caret
     */
    public Caret getCaret() {
        return this.text.getCaret();
    }

    /**
     *Returns the set of Attributes
     *@return the set of the Attribute
     */
    public AttributeSet getCharacterAttributes() {
        return this.text.getCharacterAttributes();
    }

    /**
     *Returns the TextComponent used by this editor
     *@return a JTextPane
     */
    public JTextComponent getTextComponent() {
        return this.text;
    }

    /**
     *Returns the StyledDocument
     * @return 
     */
    public StyledDocument getStyledDocument() {
        return this.text.getStyledDocument();
    }

    /**
     *Returns the caret position
     *@return a caret position
     */
    public int getCaretPosition() {
        return text.getCaretPosition();
    }

    /**
     *Sets the encoding to use to read and write files
     *@param encoding ISO-8859-1, UTF-8...
     */
    public void setEncoding(String encoding) {
        this.encoding = encoding;
    }

    /**
     *Requests the focus on text component and reload the Menus if there are used
     *@return true if the focus was requested
     */
    @Override
    public boolean requestFocusInWindow() {
        reloadMenu();
        return super.requestFocusInWindow() && text.requestFocusInWindow();
    }

    /**
     *Returns the encoding used
     *@return ISO-8859-1, UTF-8....
     */
    public String getEncoding() {
        return this.encoding;
    }

    /**
     *Close this editor.
     *If the text was modified, ask if it should be saved in a file.
     */
    public void close() {
        if (wasModified()) {
            int res = JOptionPane.showConfirmDialog(this, "Save your file " + getFile() + " ? ", "WARNING", JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE);
            if (res == JOptionPane.YES_OPTION) {
                save();
                isClosed = true;
                removeAll();
            } else if (res == JOptionPane.CLOSED_OPTION) {
                isClosed = false;
            } else if (res == JOptionPane.NO_OPTION) {
                isClosed = true;
                removeAll();
            }
        } else {
            removeAll();
            isClosed = true;
        }
    }

    /**
     *Checks if this module had been closed
     *@return true if it is closed
     */
    @Override
    public boolean isClosedModule() {
        return isClosed;
    }

    /**
     *Checks if the text was modified
     *@return true if modified
     */
    public boolean wasModified() {
        boolean modified = false;
        if (editorListener != null) {
            modified = this.editorListener.isModified();
        }
        return modified;
    }

    /**
     *Reload the menus.
     */
    protected void reloadMenu() {
        if (this.withFileMenu) {
            fileMenu.initMenu(this);
            save.setAction(fileMenu.getSaveAction());
            close.setAction(fileMenu.getCloseAction());
            close.setText("");
            close.setToolTipText("Close");
            save.setText("");
            save.setToolTipText("Save");
        } else {
            this.fileMenu = null;
        }
        if (this.withEditMenu) {
            editMenu.initMenu(this, undoAction, redoAction);
            undo.setAction(editMenu.getUndoAction());
            redo.setAction(editMenu.getRedoAction());
            cut.setAction(editMenu.getCutAction());
            copy.setAction(editMenu.getCopyAction());
            paste.setAction(editMenu.getPasteAction());
            find.setAction(editMenu.getFindAction());
            undo.setIcon(XIPUIUtilities.getIcon(UIConstants.UNDO));
            redo.setIcon(XIPUIUtilities.getIcon(UIConstants.REDO));
            cut.setIcon(XIPUIUtilities.getIcon(UIConstants.CUT));
            copy.setIcon(XIPUIUtilities.getIcon(UIConstants.COPY));
            paste.setIcon(XIPUIUtilities.getIcon(UIConstants.PASTE));
            next.setIcon(XIPUIUtilities.getIcon(UIConstants.FIND_NEXT));
            prev.setIcon(XIPUIUtilities.getIcon(UIConstants.FIND_PREV));
            find.setText("");
            find.setToolTipText("Find (CTRL+F)");
            paste.setText("");
            paste.setToolTipText("Paste");
            cut.setText("");
            cut.setToolTipText("Cut");
            copy.setText("");
            copy.setToolTipText("Copy");
            redo.setText("");
            redo.setToolTipText("Redo");
            undo.setText("");
            undo.setToolTipText("Undo");
            next.setText("");
            next.setToolTipText("Next occurence(F3)");
            prev.setText("");
            prev.setToolTipText("Previous occurence(Shift+F3)");
        } else {
            editMenu = null;
        }
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc=" Generated Code ">//GEN-BEGIN:initComponents
    private void initComponents() {
        java.awt.GridBagConstraints gridBagConstraints;

        buttonGroup = new javax.swing.ButtonGroup();
        toolBar = new javax.swing.JToolBar();
        close = new javax.swing.JButton();
        save = new javax.swing.JButton();
        saveAs = new javax.swing.JButton();
        undo = new javax.swing.JButton();
        redo = new javax.swing.JButton();
        cut = new javax.swing.JButton();
        copy = new javax.swing.JButton();
        paste = new javax.swing.JButton();
        jTextField1 = new javax.swing.JTextField();
        find = new javax.swing.JButton();
        prev = new javax.swing.JButton();
        next = new javax.swing.JButton();
        scroller = new javax.swing.JScrollPane();
        status = new javax.swing.JPanel();
        cursorInfo = new javax.swing.JLabel();
        jSeparator1 = new javax.swing.JSeparator();
        fileInfo = new javax.swing.JLabel();
        jSeparator2 = new javax.swing.JSeparator();
        divers = new javax.swing.JLabel();

        setLayout(new java.awt.GridBagLayout());

        setBackground(new java.awt.Color(255, 255, 255));
        setFocusCycleRoot(true);
        setRequestFocusEnabled(false);
        addComponentListener(new java.awt.event.ComponentAdapter() {
            public void componentResized(java.awt.event.ComponentEvent evt) {
                formComponentResized(evt);
            }
            public void componentShown(java.awt.event.ComponentEvent evt) {
                formComponentShown(evt);
            }
        });
        addFocusListener(new java.awt.event.FocusAdapter() {
            public void focusGained(java.awt.event.FocusEvent evt) {
                formFocusGained(evt);
            }
        });
        addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyReleased(java.awt.event.KeyEvent evt) {
                formKeyReleased(evt);
            }
        });

        toolBar.setFloatable(false);
        toolBar.setRollover(true);
        toolBar.setBorderPainted(false);
        toolBar.setFont(new java.awt.Font("Arial", 0, 11));
        toolBar.setMargin(new java.awt.Insets(1, 1, 1, 1));
        toolBar.setMaximumSize(new java.awt.Dimension(307, 23));
        toolBar.setMinimumSize(new java.awt.Dimension(307, 23));
        toolBar.setPreferredSize(new java.awt.Dimension(100, 23));
        toolBar.addFocusListener(new java.awt.event.FocusAdapter() {
            public void focusGained(java.awt.event.FocusEvent evt) {
                toolBarFocusGained(evt);
            }
        });

        close.setText("Close");
        close.setToolTipText("close");
        close.setBorderPainted(false);
        toolBar.add(close);

        save.setText("save");
        save.setToolTipText("save");
        save.setBorderPainted(false);
        save.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                saveActionPerformed(evt);
            }
        });

        toolBar.add(save);

        saveAs.setFont(new java.awt.Font("Arial", 0, 11));
        saveAs.setText("Save As");
        saveAs.setBorderPainted(false);
        saveAs.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                saveAsActionPerformed(evt);
            }
        });

        toolBar.add(saveAs);

        undo.setText("undo");
        undo.setToolTipText("undo");
        undo.setBorderPainted(false);
        toolBar.add(undo);

        redo.setText("redo");
        redo.setToolTipText("redo");
        redo.setBorderPainted(false);
        toolBar.add(redo);

        cut.setText("cut");
        cut.setToolTipText("cut");
        cut.setBorderPainted(false);
        toolBar.add(cut);

        copy.setText("copy");
        copy.setToolTipText("copy");
        copy.setBorderPainted(false);
        toolBar.add(copy);

        paste.setText("paste");
        paste.setToolTipText("paste");
        paste.setBorderPainted(false);
        toolBar.add(paste);

        jTextField1.setFont(new java.awt.Font("Arial", 0, 11));
        jTextField1.setHorizontalAlignment(javax.swing.JTextField.CENTER);
        jTextField1.setText("13");
        jTextField1.setToolTipText("Change size of the police");
        jTextField1.setBorder(new javax.swing.border.LineBorder(new java.awt.Color(0, 0, 0), 1, true));
        jTextField1.setMaximumSize(new java.awt.Dimension(23, 19));
        jTextField1.setMinimumSize(new java.awt.Dimension(23, 19));
        jTextField1.addFocusListener(new java.awt.event.FocusAdapter() {
            public void focusLost(java.awt.event.FocusEvent evt) {
                jTextField1FocusLost(evt);
            }
        });
        jTextField1.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyReleased(java.awt.event.KeyEvent evt) {
                jTextField1KeyReleased(evt);
            }
        });

        toolBar.add(jTextField1);

        find.setText("find");
        find.setToolTipText("find");
        find.setBorderPainted(false);
        toolBar.add(find);

        prev.setText("prev");
        prev.setBorderPainted(false);
        prev.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                prevActionPerformed(evt);
            }
        });

        toolBar.add(prev);

        next.setText("next");
        next.setBorderPainted(false);
        next.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                nextActionPerformed(evt);
            }
        });

        toolBar.add(next);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        add(toolBar, gridBagConstraints);

        scroller.setBackground(new java.awt.Color(255, 255, 255));
        scroller.setAutoscrolls(true);
        scroller.setFocusCycleRoot(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = 1.0;
        add(scroller, gridBagConstraints);

        status.setLayout(new java.awt.GridBagLayout());

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        status.add(cursorInfo, gridBagConstraints);

        jSeparator1.setOrientation(javax.swing.SwingConstants.VERTICAL);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.fill = java.awt.GridBagConstraints.VERTICAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        status.add(jSeparator1, gridBagConstraints);

        fileInfo.setForeground(new java.awt.Color(0, 102, 51));
        fileInfo.setHorizontalTextPosition(javax.swing.SwingConstants.LEFT);
        fileInfo.setMaximumSize(new java.awt.Dimension(200, 20));
        fileInfo.setPreferredSize(new java.awt.Dimension(200, 20));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        status.add(fileInfo, gridBagConstraints);

        jSeparator2.setOrientation(javax.swing.SwingConstants.VERTICAL);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.fill = java.awt.GridBagConstraints.VERTICAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        status.add(jSeparator2, gridBagConstraints);

        divers.setBackground(new java.awt.Color(255, 255, 255));
        divers.setForeground(new java.awt.Color(204, 0, 0));
        divers.setOpaque(true);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        status.add(divers, gridBagConstraints);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.SOUTHWEST;
        gridBagConstraints.weightx = 1.0;
        add(status, gridBagConstraints);

    }// </editor-fold>//GEN-END:initComponents

    private void saveAsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_saveAsActionPerformed
        saveAs();
    }//GEN-LAST:event_saveAsActionPerformed

    private void toolBarFocusGained(java.awt.event.FocusEvent evt) {//GEN-FIRST:event_toolBarFocusGained
        this.requestFocusInWindow();
    }//GEN-LAST:event_toolBarFocusGained

    private void jTextField1FocusLost(java.awt.event.FocusEvent evt) {//GEN-FIRST:event_jTextField1FocusLost
        try {
            Integer police = new Integer(jTextField1.getText());
            changeSizePolice(police.intValue());
        } catch (Exception e) {
        }
    }//GEN-LAST:event_jTextField1FocusLost

    private void jTextField1KeyReleased(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_jTextField1KeyReleased
        try {
            if (evt.getKeyCode() == KeyEvent.VK_ENTER) {
                Integer police = new Integer(jTextField1.getText());
                changeSizePolice(police.intValue());
            }
        } catch (Exception e) {
        }
    }//GEN-LAST:event_jTextField1KeyReleased

    /***
     *Changed the size of the policy
     *@param size the new size
     */
    public void changeSizePolice(int size) {
        try {
            changeStyle(Styles.createStyle("Monospaced", size), 0, doc.getLength());
        } catch (Exception e) {
            log.warn(e);
        }
    }

    private void prevActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_prevActionPerformed
        if (editMenu != null) {
            this.editMenu.findPrev();
        }
    }//GEN-LAST:event_prevActionPerformed

    private void nextActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_nextActionPerformed
        if (editMenu != null) {
            this.editMenu.findNext();
        }
    }//GEN-LAST:event_nextActionPerformed

    private void formKeyReleased(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_formKeyReleased
        if (evt.getID() == KeyEvent.KEY_RELEASED) {
            if (evt.isShiftDown() && evt.getKeyCode() == KeyEvent.VK_F3) {
                if (editMenu != null) {
                    this.editMenu.findPrev();
                }
            } else if (evt.getKeyCode() == KeyEvent.VK_F3) {
                if (editMenu != null) {
                    this.editMenu.findNext();
                }
            }
        }
    }//GEN-LAST:event_formKeyReleased

    private void saveActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_saveActionPerformed
        save();
    }//GEN-LAST:event_saveActionPerformed

    /***
     *Save the document,
     *uses the encoding for the file.
     *
     */
    public void save() {
        try {
            FileTools.storeString(getText(), file.getAbsolutePath(), getEncoding());

            if (this.editorListener != null) {
                editorListener.setModified(false);
            }
        } catch (Exception e) {
            severe("ERROR ", e);
        }
    }

    private void formComponentResized(java.awt.event.ComponentEvent evt) {//GEN-FIRST:event_formComponentResized
        repaint();
    }//GEN-LAST:event_formComponentResized

    private void formFocusGained(java.awt.event.FocusEvent evt) {//GEN-FIRST:event_formFocusGained
        reloadMenu();
    }//GEN-LAST:event_formFocusGained

    private void formComponentShown(java.awt.event.ComponentEvent evt) {//GEN-FIRST:event_formComponentShown
        requestFocusInWindow();
    }//GEN-LAST:event_formComponentShown

    /**
     *Event on the caret
     *Show the line number and the caret position
     *@param e the event
     *
     */
    public void caretUpdate(CaretEvent e) {
        LineColor.HIGHLIGHT_OFFSET = e.getDot() + 1;
        int line = doc.getDefaultRootElement().getElementIndex(e.getDot());
        cursorInfo.setText(line + ":" + e.getDot());
        divers.setText("");
        repaint();
    }

    /***
     *Shows the toolbar or not
     *@param b true to show
     */
    public void setToolBarVissible(boolean b) {
        this.toolBar.setVisible(b);
    }

    /**
     *Show the status bar or not
     *@param b boolean true visible false sinon
     */
    public void setStatusVisible(boolean b) {
        this.status.setVisible(b);
    }

    /***
     *Indique que cet editeur peut etre fermable ou non. Util pour les zones de textes.
     *@param b true fermable false sinon
     */
    public void setClosable(boolean b) {
        this.close.setEnabled(b);
        this.close.setVisible(b);
    }

    /***
     *Indique que cet editeur peut etre fermable ou non. Util pour les zones de textes.
     *@param b true fermable false sinon
     */
    public void setSavable(boolean b) {
        this.save.setVisible(b);
    }
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.ButtonGroup buttonGroup;
    private javax.swing.JButton close;
    protected javax.swing.JButton copy;
    protected javax.swing.JLabel cursorInfo;
    protected javax.swing.JButton cut;
    protected javax.swing.JLabel divers;
    protected javax.swing.JLabel fileInfo;
    private javax.swing.JButton find;
    private javax.swing.JSeparator jSeparator1;
    private javax.swing.JSeparator jSeparator2;
    private javax.swing.JTextField jTextField1;
    private javax.swing.JButton next;
    protected javax.swing.JButton paste;
    private javax.swing.JButton prev;
    protected javax.swing.JButton redo;
    protected javax.swing.JButton save;
    private javax.swing.JButton saveAs;
    protected javax.swing.JScrollPane scroller;
    private javax.swing.JPanel status;
    protected javax.swing.JToolBar toolBar;
    protected javax.swing.JButton undo;
    // End of variables declaration//GEN-END:variables

    /// methode pour l'edition :
    /***
     *Fichier a editer.
     *Cette methode ne lit pas le fichier passe en parametre.
     *Elle prepare juste le fichier pour l'edition.
     *c'est util si on veut simplement creer un nouveau fichier vide, ou alors remplacer
     *le contenu d'un fichier existant, sans devoir le lire et effacer son contenu.
     *Si le fichier n'existe pas, alors on l'indique dans la barre de statut.
     *@param file_ fichier a editer
     */
    public void setFile(File file_) {
        this.file = file_;
        if (file.exists()) {
            fileInfo.setText(getEncoding() + " : " + file.getName());
            fileInfo.setToolTipText(file.getAbsolutePath());
            fileInfo.setVisible(true);
        } else {
            fileInfo.setText(file.getName() + " doesn't exist !");
            fileInfo.setToolTipText(file.getAbsolutePath() + " doesn't exist !");
            fileInfo.setBackground(Color.RED);
            fileInfo.setForeground(Color.WHITE);
            fileInfo.setOpaque(true);
            fileInfo.setVisible(true);
        }
    }

    /**
     *Lance l'ecoute de l'edition. C'est a dire permet a l'editeur de gerer les
     *action de undo et redo ainsi que de paste,  cut et copy.
     */
    public void startEditing() {
        if (doc != null) {
            //DEBUG_LOG.trace("undoL : " + doc.getUndoableEditListeners().length  + " docL : " + doc.getDocumentListeners().length );
            if (doc.getUndoableEditListeners().length == 0 && doc.getDocumentListeners().length <= 2) {
                this.editorListener = new EditorListener(fileMenu);
                doc.addDocumentListener(editorListener);
                undoListener = new UndoableEditorListener(undoM, undoAction, redoAction);
                doc.addUndoableEditListener(undoListener);
            }
        }
    }

    /**
     *Lit le fichier et insere le contenu dans le document.
     *Le curseur se place a la position 0.
     *
     */
    public void editFile() {
        try {
            String fileContent = FileTools.read(file, encoding);
            doc.replace(0, doc.getLength(), fileContent, currentAttr);
        } catch (Exception e) {
            severe("ERROR :", e);
        }
        text.setCaretPosition(0);
        text.repaint();
    }

    /**
     *Sets the icon for this editor
     *@param icon the icon to use
     */
    public void setIcon(Icon icon) {
        this.icon = icon;
    }

    /***
     *Returns the icon
     *@return icon, can be null
     */
    public Icon getIcon() {
        return this.icon;
    }

    /**
     *Insere du texte a la fin du document que l'on edite.
     *@param text texte a inserer
     *@param style pour le texte a inserer
     */
    public void write(String text, int style) {
        try {
            doc.insertString(doc.getLength(), text, Styles.getStyle(style));
        } catch (Exception e) {
            severe("ERROR :", e);
        }
    }

    public void replace(long start, long end, String s) {
        try {
            doc.replace((int) start, (int) end, s, this.currentAttr);
        } catch (Exception e) {
            severe("ERROR :", e);
        }
    }

    public void write(long position, String text) {
        try {
            doc.insertString((int) position, text, this.currentAttr);
        } catch (Exception e) {
            severe("ERROR :", e);
        }
    }

    /**
     *Change le style de tout le document
     *@param attr SimpleAttributSet style a adopter
     *
     */
    public void setStyle(SimpleAttributeSet attr) {
        int begin = 0;
        int end = doc.getLength();
        try {
            String docTxt = doc.getText(begin, end);
            doc.replace(begin, end, docTxt, attr);
        } catch (Exception e) {
            warning("ERROR " + e.toString());
        }
    }

    /**
     *Change le style de tout le document
     *@param font  police du texte
     *@param color couleur du texte
     *@param fontSize taille de la police
     */
    public void setStyle(String font, Color color, int fontSize) {
        int begin = 0;
        int end = doc.getLength();
        try {

            String docTxt = doc.getText(begin, end);
            doc.replace(begin, end, docTxt, Styles.createStyle(font, color, fontSize));
        } catch (Exception e) {
            warning("ERROR " + e.toString());
        }
    }

    /**
     *Change le style d'une partie du document :
     *@param style nouveau style
     *@param start debut de la partie a changer.
     *@param end fin de la partie a changer.
     */
    public void changeStyle(int style, int start, int end) {
        try {
            String docTxt = doc.getText(start, end - start);
            currentAttr = Styles.getStyle(style);
            doc.replace(start, end - start, docTxt, currentAttr);
            Font font = new Font(StyleConstants.getFontFamily(currentAttr), Font.PLAIN, StyleConstants.getFontSize(currentAttr));
            text.setFont(font);
        } catch (Exception e) {
            warning("ERROR " + e.toString());
        }
    }

    /**
     *Renvoie le contenu de la derniere ligne de document.
     *@return Renvoie la derniere ligne du document
     * si un probleme est capture on renvoie une chaine vide
     */
    public String getLastRow() {
        String lastRow = new String();
        try {
            int end = Utilities.getRowEnd(text, doc.getLength());
            int start = Utilities.getRowStart(text, doc.getLength());
            text.select(start, end);
            lastRow = doc.getText(start, end - start);
        } catch (Exception e) {
            warning("ERROR :" + e.toString());
        }
        return lastRow;
    }

    /**
     *Renvoie l'offset du debut d'une ligne
     *@param line numero de la ligne
     *@return l'offset du debut de ligne
     */
    public int startOfLine(int line) {
        int position = 0;
        try {
            Element root1 = doc.getDefaultRootElement();
            Element el = root1.getElement(line);
            position = el.getStartOffset();
        } catch (Exception ex) {
            log.warn(ex);
        }
        return position;
    }

    /** renvoie l'offset de la fin de ligne
     * @param line Numero de ligne
     * @return l'offset de la fin de la ligne
     */
    public int endOfLine(int line) {
        int position = 0;
        try {
            Element root = doc.getDefaultRootElement();
            Element el = root.getElement(line);
            position = el.getEndOffset();
        } catch (Exception ex) {
            log.warn(ex);
        }
        return position;
    }

    /**
     * Renvoie le numero de la ligne courante du curseur.
     * @return un entier coorespondant au numero de ligne, les numeros sont indexes de 0 a n-1
     *
     */
    public int getCurrentLine() {
        return doc.getDefaultRootElement().getElementIndex(text.getCaretPosition());
    }

    /**
     *Change le style d'une partie du document :
     *@param style nouveau style
     *@param start debut de la partie a changer.
     *@param end fin de la partie a changer.
     */
    public void changeStyle(SimpleAttributeSet style, int start, int end) {
        try {
            String docTxt = doc.getText(start, end - start);
            doc.removeUndoableEditListener(this.undoListener);
            currentAttr = style;
            doc.replace(start, end - start, docTxt, currentAttr);
            doc.addUndoableEditListener(this.undoListener);
            Font font = new Font(StyleConstants.getFontFamily(currentAttr), Font.PLAIN, StyleConstants.getFontSize(currentAttr));
            text.setFont(font);
        } catch (Exception e) {
            severe("ERROR :", e);
        }
    }

    /**
     * @return le texte contenu dans le document
     */
    public String getText() {
        String docTxt = null;
        try {
            int size = doc.getLength();
            docTxt = doc.getText(0, size);
        } catch (Exception e) {
            severe("ERROR :", e);
            docTxt = text.getText();
        }
        return docTxt;
    }

    /***
     *Selectionne le texte
     *@param begin debut de la selection
     *@param end fin de la selection
     */
    public void setSelectedText(long begin, long end) {
        text.select((int) begin, (int) end);
    }

    /**
     *Positionne le caret a l'endroit pos
     *@param pos offset du caret
     */
    public void setCaretPosition(long pos) {
        text.setCaretPosition((int) pos);
    }

    /**
     *Positionne le caret a l'endroit pos
     *@param pos offset du caret
     */
    public void moveCaretPosition(long pos) {
        text.moveCaretPosition((int) pos);
    }

    /**
     *Returns a part of the text
     *@param start the start offset
     *@param end the end offset
     *@return the sub string between start and end or an empty string if the params are invalids
     */
    public String getText(int start, int end) {
        String docTxt = null;
        try {
            docTxt = doc.getText(start, end - start);
        } catch (Exception e) {
            log.warn("ERROR :", e);
            docTxt = new String();
        }
        return docTxt;
    }

    /*
     *@return renvoie le texte sekectionne
     */
    public String getSelected() {
        return this.text.getSelectedText();
    }

    public int getSelectedEnd() {
        return this.text.getSelectionEnd();
    }

    /***
     *Debut d'une selection
     *@return le debut de la selection
     */
    public int getSelectedStart() {
        return this.text.getSelectionStart();
    }

    /**
     *Renvoie l'offset de selection
     *return un objet Offset indiquant les offsets de selection
     */
    public Offset getSelectionOffset() {
        return new Offset((long) getSelectedStart(), (long) getSelectedEnd());
    }

    /**
     *@return renvoie le fichier editer
     */
    public File getFile() {
        return this.file;
    }

    public int getLenght() {
        return doc.getLength();
    }

    /**
     *@return renvoie true si le document est editable, false sinon
     */
    public boolean isEditable() {
        return this.text.isEditable();
    }

    public String getRowAtCaret() {
        String row = null;
        try {
            int end = Utilities.getRowEnd(text, text.getCaretPosition());
            int start = Utilities.getRowStart(text, text.getCaretPosition());
            text.select(start, end);
            row = doc.getText(start, end - start);
        } catch (Exception e) {
            DEBUG_LOG.debug("ERROR :" + e.toString());
            row = new String();
        }
        return row;
    }

    /**
     * Determine si le document est editable ou non
     * @param b true implique que le document est editable
     *
     */
    public void setEditable(boolean b) {
        this.text.setEditable(b);
        if (b) {
            this.startEditing();
        }
    }

    public void saveAs() {
        JDialog openDialog = new JDialog();
        JFileChooser openChooser = new JFileChooser();
        openDialog.setTitle("Save As");
        openDialog.setAlwaysOnTop(true);
        openDialog.setModal(true);
        openDialog.getContentPane().add(openChooser, java.awt.BorderLayout.CENTER);
        File fileSrc = getFile();
        File fileDst = null;
        openChooser.setCurrentDirectory(fileSrc);
        openChooser.setSelectedFile(fileSrc);
        openChooser.setMultiSelectionEnabled(false);
        int returnVal = openChooser.showSaveDialog(openDialog);
        if (returnVal == JFileChooser.APPROVE_OPTION) {
            fileDst = openChooser.getSelectedFile(); // selection du fichier

            try {
                if (fileDst != null) {
                    if (fileDst.exists()) { // si le fichier existe on demande a ecraser ou non

                        int res = JOptionPane.showConfirmDialog(this, "File already exists : " + fileDst + " Replace it ? ", "WARNING", JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE);
                        if (res == JOptionPane.YES_OPTION) {
                            FileTools.storeString(this.getText(), fileDst.getAbsolutePath(), this.getEncoding());
                            info(fileDst + " was correctly saved in " + getEncoding());
                        } else if (res == JOptionPane.NO_OPTION) {
                            openDialog.setVisible(false);
                            saveAs();
                        }
                    } else {
                        fileDst.createNewFile();
                        FileTools.storeString(this.getText(), fileDst.getAbsolutePath(), this.getEncoding());
                        info(fileDst + " was correctly saved in " + getEncoding());
                    }
                }
            } catch (Exception e) {
                warning("Could not save your file : " + e);
            }
        }
        openDialog.setVisible(false);
    }
}
