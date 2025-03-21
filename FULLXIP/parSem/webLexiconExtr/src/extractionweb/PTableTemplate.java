/*
 * PanelTemplateTable.java
 *
 * Created on April 29, 2008, 4:56 PM
 */

package extractionweb;

import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Vector;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.JTabbedPane;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.RowSorterEvent;
import javax.swing.event.RowSorterListener;
import javax.swing.event.TableColumnModelEvent;
import javax.swing.event.TableColumnModelListener;
import javax.swing.event.TableModelEvent;
import javax.swing.event.TableModelListener;
import javax.swing.filechooser.FileSystemView;
import org.apache.commons.lang.StringUtils;

/**
 * JPanel displaying a LIST or a TABLE.
 * @author  Cédric Tarsitano
 */
public class PTableTemplate extends javax.swing.JPanel {
    
    /**
     * The parent container of this JPanel.
     */
    private JTabbedPane mParent;
    /**
     * The index of this JPanel in the tab list in FExtraParsingTools.
     */
    private int mIndex;
    /**
     * Type of the Data (LIST or TABLE)
     */
    private int mType;
    /**
     * <i>Vector</i> of <i>Data</i> containing the data to display.
     */
    private Vector <Data> mList;
    /**
     * The number of columns of this JTable.
     */
    private int mNbColumns;
    /**
     * <i>Vector</i> used to provide the matching between the index of the columns in the Table's view display order and the TableModel's order.
     */
    private Vector <Integer> mMatching;
    /**
     * The number of added column(s).
     */
    private int mColumnAdded;
    /**
     * All the saved states of this JPanel.
     */
    private Vector <Vector <Data>> mStates;
    /**
     * Index of the previous state in <i>mStates</i>.
     */
    private int mPreviousState;
    /**
     * Index of the last state saved in <i>mStates</i>.
     */
    private int mMaxState;
    
    /** Creates new form PanelTemplateTable */
    public PTableTemplate(JTabbedPane pane, int index, Vector <Data> list, int type) {
        mParent = pane;
        mIndex = index;
        mList = list;
        mType = type;
        mNbColumns = mColumnAdded = Main.columnCount(mList);
        initComponents();
        mStates = new Vector <Vector <Data>> ();
        mPreviousState = mMaxState = 0;
        saveState();
        mPreviousState = mMaxState = 0;
        initTable(true);
    }

    /**
     * This method fills the JTable.
     * @param firstTime <i>boolean</i> telling if this method is called for the first time.
     */
    private void initTable(boolean firstTime){
        if(firstTime && mType == Data.LIST){
            for(Data data : mList)
                data.setLine(data.getLine() + 1);
            mList.add(0, new Data(Data.LIST, 0, 0, "Column 0"));
        }
        jTable1.setModel(new javax.swing.table.DefaultTableModel(Main.getDataContent(mList), new Vector <String> (Main.getHeadersAsString(mList))));
        jTable1.setColumnSelectionAllowed(true);
        jTable1.setRowSelectionAllowed(true);
        jScrollPane1.setViewportView(jTable1);
        
        mMatching = new Vector <Integer>(mNbColumns, 1);
        for (int i = 0; i < mNbColumns; ++i)
            mMatching.addElement(i);
        
        jTable1.getModel().addTableModelListener(new TableModelListener() {

            public synchronized void tableChanged(TableModelEvent e) {
                if(e.getType() == TableModelEvent.UPDATE){
                    int row = e.getFirstRow();
                    int col = mMatching.indexOf(e.getColumn());
                    mList.elementAt((row + 1) * mNbColumns + col).setContent(jTable1.getValueAt(row, col).toString());
                    saveState();
                    Main.toolsFrame.requestFocus();
                }
            }
        });
        jTable1.setAutoCreateRowSorter(true);
        jTable1.getRowSorter().addRowSorterListener(new RowSorterListener() {

            public synchronized void sorterChanged(RowSorterEvent e) {
                if(e.getType().equals(RowSorterEvent.Type.SORTED)){
                    if(mNbColumns < 1)
                        return;
                    int rowCount = mList.size() / mNbColumns;
                    Vector <String> content = new Vector <String> ();
                    for(int i = 0; i < mNbColumns; ++i){
                        content.removeAllElements();
                        for(int j = 0; j < rowCount - 1; ++j)
                            content.addElement(jTable1.getValueAt(j, i).toString());
                        for(int j = 0; j < rowCount - 1; ++j)
                            mList.elementAt((j + 1) * mNbColumns + i).setContent(content.elementAt(j));
                    }
                    saveState();
                    Main.toolsFrame.requestFocus();
                }
            }
        });

        if(!firstTime)
            return;
        
        jTable1.getColumnModel().addColumnModelListener(new TableColumnModelListener() {

            public void columnAdded(TableColumnModelEvent e) {}

            public void columnRemoved(TableColumnModelEvent e) {}

            public synchronized void columnMoved(TableColumnModelEvent e) {
                int fromIndex = e.getFromIndex();
                int toIndex = e.getToIndex();
                if(fromIndex != toIndex){
                    
                    Vector <Data> from = Main.getElementsAtColumn(mList, fromIndex);
                    Vector <Data> to = Main.getElementsAtColumn(mList, toIndex);
                    for (Data data : from)
                        data.setColumn(toIndex);
                    for (Data data : to)
                        data.setColumn(fromIndex);
                    Main.sortColumns(mList, fromIndex, toIndex);
                    int tmp = mMatching.elementAt(fromIndex);
                    mMatching.setElementAt(mMatching.elementAt(toIndex), fromIndex);
                    mMatching.setElementAt(tmp, toIndex);
                    saveState();
                    Main.toolsFrame.requestFocus();
                }
            }

            public void columnMarginChanged(ChangeEvent e) {}

            public void columnSelectionChanged(ListSelectionEvent e) {}
        });
    }
    
    /**
     * Return the index of this JPanel in the tab list of FExtraParsingTools.
     * @return the index of this JPanel in the tab list of FExtraParsingTools. 
     */
    protected int getIndex(){
        return mIndex;
    }
    
    /**
     * Sets the index of this JPanel in the tab list of FExtraParsingTools to <i>index</i>.
     * @param index the index of this JPanel in the tab list of FExtraParsingTools.
     */
    protected void setIndex(int index){
        mIndex = index;
    }
    
    /**
     * Method that provides the "redo" function (Ctrl-Y).
     */
    protected Vector <Data> redo(){
        if(mPreviousState < mMaxState){
            Vector <Data> tmp = new Vector <Data> ();
            Vector <Data> tmp2 = new Vector <Data> ();
            for(Data data : mList)
                tmp.addElement(new Data(data));
            for(Data data : mStates.elementAt(mPreviousState + 1))
                tmp2.addElement(new Data(data));

            mStates.setElementAt(tmp, mPreviousState++);
            mList = new Vector <Data> (tmp2);
            initTable(false);
        }
        return mList;
    }
    
    /**
     * Method that provides the "undo" function (Ctrl-Z).
     */
    protected Vector <Data> undo(){
        if(mPreviousState > 0){
            Vector <Data> tmp = new Vector <Data> ();
            Vector <Data> tmp2 = new Vector <Data> ();
            for(Data data : mList)
                tmp.addElement(new Data(data));
            for(Data data : mStates.elementAt(mPreviousState - 1))
                tmp2.addElement(new Data(data));
            mList = new Vector <Data> (tmp2);
            mStates.setElementAt(tmp, mPreviousState--);
            initTable(false);
        }
        return mList;
    }
    
    /**
     * This method saves the current state of this JPanel at <i>mPreviousState</i> + 1 and removes all the following states.
     */
    private void saveState(){
        if(mMaxState > mPreviousState++)
            for(int i = mPreviousState; i < mStates.size(); ++i)
                mStates.removeElementAt(i);
        mMaxState = mPreviousState;
        Vector <Data> tmp = new Vector <Data>();
        for(Data data : mList)
            tmp.addElement(new Data(data));

        mStates.addElement(new Vector <Data> (tmp));
    }
    
    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jScrollPane1 = new javax.swing.JScrollPane();
        jTable1 = new javax.swing.JTable();
        jPanel1 = new javax.swing.JPanel();
        closeTabButton = new javax.swing.JButton();
        saveTableButton = new javax.swing.JButton();
        buttonsPanel = new javax.swing.JPanel();
        addRowButton = new javax.swing.JButton();
        removeRowButton = new javax.swing.JButton();
        addColButton = new javax.swing.JButton();
        removeColButton = new javax.swing.JButton();
        modifyColButton = new javax.swing.JButton();
        splitColButton = new javax.swing.JButton();

        setBackground(java.awt.Color.white);

        jTable1.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {

            },
            new String [] {

            }
        ));
        jTable1.setColumnSelectionAllowed(true);
        jScrollPane1.setViewportView(jTable1);
        jTable1.getColumnModel().getSelectionModel().setSelectionMode(javax.swing.ListSelectionModel.MULTIPLE_INTERVAL_SELECTION);

        jPanel1.setBackground(java.awt.Color.white);

        closeTabButton.setIcon(new javax.swing.ImageIcon(getClass().getResource("/icon_button_close.gif"))); // NOI18N
        closeTabButton.setToolTipText("Closes the current tab");
        closeTabButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                closeTabButtonActionPerformed(evt);
            }
        });

        saveTableButton.setForeground(java.awt.Color.red);
        saveTableButton.setText("Save this table in a file");
        saveTableButton.setToolTipText("Saves this table in a file");
        saveTableButton.setBorder(javax.swing.BorderFactory.createBevelBorder(javax.swing.border.BevelBorder.RAISED));
        saveTableButton.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        saveTableButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                saveTableButtonActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
        jPanel1.setLayout(jPanel1Layout);
        jPanel1Layout.setHorizontalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel1Layout.createSequentialGroup()
                .addGap(327, 327, 327)
                .addComponent(saveTableButton, javax.swing.GroupLayout.PREFERRED_SIZE, 220, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 344, Short.MAX_VALUE)
                .addComponent(closeTabButton, javax.swing.GroupLayout.PREFERRED_SIZE, 27, javax.swing.GroupLayout.PREFERRED_SIZE))
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel1Layout.createSequentialGroup()
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(closeTabButton)
                    .addComponent(saveTableButton, javax.swing.GroupLayout.DEFAULT_SIZE, 34, Short.MAX_VALUE))
                .addContainerGap())
        );

        buttonsPanel.setBackground(java.awt.Color.white);

        addRowButton.setForeground(java.awt.Color.red);
        addRowButton.setText("Add a row");
        addRowButton.setToolTipText("Adds an empty row at the end of the table");
        addRowButton.setBorder(javax.swing.BorderFactory.createBevelBorder(javax.swing.border.BevelBorder.RAISED));
        addRowButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                addRowButtonActionPerformed(evt);
            }
        });

        removeRowButton.setForeground(java.awt.Color.red);
        removeRowButton.setText("Remove selected row(s)");
        removeRowButton.setToolTipText("Removes selected row(s)");
        removeRowButton.setBorder(javax.swing.BorderFactory.createBevelBorder(javax.swing.border.BevelBorder.RAISED));
        removeRowButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                removeRowButtonActionPerformed(evt);
            }
        });

        addColButton.setForeground(java.awt.Color.red);
        addColButton.setText("Add a column");
        addColButton.setToolTipText("Adds an empty column on the right side of the table");
        addColButton.setBorder(javax.swing.BorderFactory.createBevelBorder(javax.swing.border.BevelBorder.RAISED));
        addColButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                addColButtonActionPerformed(evt);
            }
        });

        removeColButton.setForeground(java.awt.Color.red);
        removeColButton.setText("Remove selected column(s)");
        removeColButton.setToolTipText("Removes selected column(s)");
        removeColButton.setBorder(javax.swing.BorderFactory.createBevelBorder(javax.swing.border.BevelBorder.RAISED));
        removeColButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                removeColButtonActionPerformed(evt);
            }
        });

        modifyColButton.setForeground(java.awt.Color.red);
        modifyColButton.setText("Fill selected column");
        modifyColButton.setToolTipText("Fills the selected column (one at a time)");
        modifyColButton.setBorder(javax.swing.BorderFactory.createBevelBorder(javax.swing.border.BevelBorder.RAISED));
        modifyColButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                modifyColButtonActionPerformed(evt);
            }
        });

        splitColButton.setForeground(java.awt.Color.red);
        splitColButton.setText("Split selected column");
        splitColButton.setToolTipText("Splits the content of the selected column in 2 parts");
        splitColButton.setBorder(javax.swing.BorderFactory.createBevelBorder(javax.swing.border.BevelBorder.RAISED));
        splitColButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                splitColButtonActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout buttonsPanelLayout = new javax.swing.GroupLayout(buttonsPanel);
        buttonsPanel.setLayout(buttonsPanelLayout);
        buttonsPanelLayout.setHorizontalGroup(
            buttonsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(buttonsPanelLayout.createSequentialGroup()
                .addGap(38, 38, 38)
                .addGroup(buttonsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(addRowButton, javax.swing.GroupLayout.PREFERRED_SIZE, 220, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(removeRowButton, javax.swing.GroupLayout.PREFERRED_SIZE, 220, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(91, 91, 91)
                .addGroup(buttonsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.CENTER)
                    .addComponent(addColButton, javax.swing.GroupLayout.PREFERRED_SIZE, 220, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(removeColButton, javax.swing.GroupLayout.PREFERRED_SIZE, 220, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(87, 87, 87)
                .addGroup(buttonsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(modifyColButton, javax.swing.GroupLayout.PREFERRED_SIZE, 220, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(splitColButton, javax.swing.GroupLayout.PREFERRED_SIZE, 220, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap(42, Short.MAX_VALUE))
        );
        buttonsPanelLayout.setVerticalGroup(
            buttonsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(buttonsPanelLayout.createSequentialGroup()
                .addGap(23, 23, 23)
                .addGroup(buttonsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(addRowButton, javax.swing.GroupLayout.PREFERRED_SIZE, 26, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(addColButton, javax.swing.GroupLayout.PREFERRED_SIZE, 26, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(modifyColButton, javax.swing.GroupLayout.PREFERRED_SIZE, 26, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(28, 28, 28)
                .addGroup(buttonsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(removeRowButton, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.PREFERRED_SIZE, 26, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, buttonsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                        .addComponent(removeColButton, javax.swing.GroupLayout.PREFERRED_SIZE, 26, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addComponent(splitColButton, javax.swing.GroupLayout.PREFERRED_SIZE, 26, javax.swing.GroupLayout.PREFERRED_SIZE)))
                .addContainerGap(17, Short.MAX_VALUE))
        );

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(buttonsPanel, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addComponent(jPanel1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 918, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jPanel1, javax.swing.GroupLayout.PREFERRED_SIZE, 33, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jScrollPane1, javax.swing.GroupLayout.PREFERRED_SIZE, 500, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(11, 11, 11)
                .addComponent(buttonsPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );
    }// </editor-fold>//GEN-END:initComponents

private void closeTabButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_closeTabButtonActionPerformed
if(JOptionPane.YES_OPTION == JOptionPane.showConfirmDialog(
                                        null,
                                        "The data contained in this tab will be lost.\nDo you really want to loose that data ?",
                                        "Warning",
                                        JOptionPane.YES_NO_OPTION,
                                        JOptionPane.WARNING_MESSAGE)){
        mParent.remove(mIndex);
        FExtraParsingTools.mTabList.removeElementAt(mIndex);
        FExtraParsingTools.refreshIndex(mIndex);
    }
}//GEN-LAST:event_closeTabButtonActionPerformed

private void addRowButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_addRowButtonActionPerformed
    if(mList.isEmpty()){
        if(Main.addEmptyColumn(mList, mType, mType == Data.TABLE ? JOptionPane.showInputDialog(this, "The table is empty so type the title of the new column") : ("Column " + mColumnAdded))){
            mMatching.addElement(mNbColumns);
            ++mNbColumns;
            ++mColumnAdded;
            Main.addEmptyRow(mList, mType);
            saveState();
            Main.toolsFrame.requestFocus();
            initTable(false);
        }
    }
    else{
        Main.addEmptyRow(mList, mType);
        saveState();
        Main.toolsFrame.requestFocus();
        initTable(false);
    }
    
}//GEN-LAST:event_addRowButtonActionPerformed

private void addColButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_addColButtonActionPerformed
    if(Main.addEmptyColumn(mList, mType, mType == Data.TABLE ? JOptionPane.showInputDialog(this, "Type the title of the new column") : ("Column " + mColumnAdded))){
        mMatching.addElement(mNbColumns);
        ++mNbColumns;
        ++mColumnAdded;
        saveState();
        Main.toolsFrame.requestFocus();
        initTable(false);
    }
}//GEN-LAST:event_addColButtonActionPerformed

private void removeColButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_removeColButtonActionPerformed
    int[] selectedCols = jTable1.getSelectedColumns();
    if(selectedCols.length != 0){
        for (int i = 0; i < selectedCols.length; ++i) {
            jTable1.removeColumn(jTable1.getColumn(jTable1.getColumnName(selectedCols[i])));
            mList.removeAll(Main.getElementsAtColumn(mList, selectedCols[i]));
            /*mMatching.removeElement(selectedCols[i]);
            int val;
            for (int j = 0; j < mMatching.size(); ++j) {
                val = mMatching.elementAt(j);
                if(val > selectedCols[i])
                    mMatching.setElementAt(val - 1, j);
            }*/
            --mNbColumns;
            Main.updateDataColumns(mList, selectedCols[i]);
            for(int j = i + 1; j < selectedCols.length; ++j)
                --selectedCols[j];
        }
        saveState();
        Main.toolsFrame.requestFocus();
        initTable(false);
    }
}//GEN-LAST:event_removeColButtonActionPerformed

private void removeRowButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_removeRowButtonActionPerformed
    int[] selectedRows = jTable1.getSelectedRows();
    if(selectedRows.length != 0){
        for (int i = 0; i < selectedRows.length; ++i) {
            int numRow = (mType == Data.TABLE ? selectedRows[i] + 1 : selectedRows[i]);
            mList.removeAll(Main.getElementsAtRow(mList, numRow));
            Main.updateDataRows(mList, numRow);
            for(int j = i + 1; j < selectedRows.length; ++j)
                --selectedRows[j];
        }
        saveState();
        Main.toolsFrame.requestFocus();
        initTable(false);
    }
}//GEN-LAST:event_removeRowButtonActionPerformed

private void saveTableButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_saveTableButtonActionPerformed
    try{
        JFileChooser j = new JFileChooser(FileSystemView.getFileSystemView());
        j.setDialogTitle("Select the file to export this table to");
        int res, err = 0;
        boolean ok = false;
        while(!ok){
            res = j.showSaveDialog(this);
            switch(res){
                case JFileChooser.APPROVE_OPTION :
                    File saveFile = j.getSelectedFile();
                    if(!saveFile.exists())
                        saveFile.createNewFile();
                    FileWriter fw = new FileWriter(saveFile);
                    String out = "";
                    int rowCount = mList.size() / mNbColumns;
                    switch(mType){
                        case Data.LIST:
                            for(int i = 1; i < rowCount; ++i){
                                for (Data data : Main.getElementsAtRow(mList, i))
                                    out += data.getContent() + Main.separator;
                                out += "\n";
                            }
                            break;
                        case Data.TABLE:
                            for(int i = 0; i < rowCount; ++i){
                                for (Data data : Main.getElementsAtRow(mList, i))
                                    out += data.getContent() + Main.separator;
                                out += "\n";
                            }
                            break;
                        case Data.EMBED:
                            break;
                    }
                    fw.write(out);
                    fw.close();
                    ok = true;
                    break;
                case JFileChooser.CANCEL_OPTION :
                    ok = true;
                    break;
                case JFileChooser.ERROR_OPTION :
                default :
                    ++err;
                    if(err > 5)
                        throw new IOException("An error occured while selecting the output file, 5 times.");
                    JOptionPane.showMessageDialog(null, "An error occured, please select the output file again.", "Error", JOptionPane.ERROR_MESSAGE);
            }
        }
    }
    catch (IOException e){
        JOptionPane.showMessageDialog(null, "Please check the file you selected.", "Error", JOptionPane.ERROR_MESSAGE);
        System.err.println(e.getMessage());
        e.printStackTrace();
    }
}//GEN-LAST:event_saveTableButtonActionPerformed

private void modifyColButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_modifyColButtonActionPerformed
    String s = JOptionPane.showInputDialog(this, "Type the content you want to put into the cells of the selected column");
    if(null == s)
        return;
    int[] selectedCols = jTable1.getSelectedColumns();
    if(selectedCols.length == 1){
        Vector <Data> v = Main.getElementsAtColumn(mList, selectedCols[0]);
        String s2 = v.firstElement().getContent();
        for (Data data : v)
            data.setContent(s);
        v.firstElement().setContent(s2);
        saveState();
        Main.toolsFrame.requestFocus();
        initTable(false);
    }
}//GEN-LAST:event_modifyColButtonActionPerformed

private void splitColButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_splitColButtonActionPerformed
    if(jTable1.getSelectedColumnCount() == 1){
        Vector <String> v1 = new Vector <String> ();
        Vector <String> v2 = new Vector <String> ();
        for (Data data : Main.getElementsAtColumn(mList, jTable1.getSelectedColumn())) {
            String s = "";
            String[] strings = StringUtils.splitByCharacterTypeCamelCase(data.getContent());
            if(strings.length == 0)
                return;
            v1.addElement(strings[0]);
            if(strings.length > 1)
                for(int i = 1; i < strings.length; ++i)
                    s += strings[i];
            v2.addElement(s);
        }
        Vector <Data> v = Main.getElementsAtColumn(mList, jTable1.getSelectedColumn());
        int i = 0;
        String title = v.firstElement().getContent();
        for (Data data : v)
            data.setContent(v1.elementAt(i++));
        v.firstElement().setContent(title);
        Main.addColumn(mList, mType, title, v2);
        ++mNbColumns;
        ++mColumnAdded;
        saveState();
        Main.toolsFrame.requestFocus();
        initTable(false);
    }
}//GEN-LAST:event_splitColButtonActionPerformed


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton addColButton;
    private javax.swing.JButton addRowButton;
    private javax.swing.JPanel buttonsPanel;
    private javax.swing.JButton closeTabButton;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JTable jTable1;
    private javax.swing.JButton modifyColButton;
    private javax.swing.JButton removeColButton;
    private javax.swing.JButton removeRowButton;
    private javax.swing.JButton saveTableButton;
    private javax.swing.JButton splitColButton;
    // End of variables declaration//GEN-END:variables

}
