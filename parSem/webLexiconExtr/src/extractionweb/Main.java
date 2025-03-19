/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package extractionweb;

import java.awt.Dimension;
import java.awt.Frame;
import java.awt.Toolkit;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Vector;
import javax.swing.JOptionPane;
import org.apache.commons.lang.StringEscapeUtils;
import org.apache.commons.lang.StringUtils;
import org.htmlparser.Node;
import org.htmlparser.http.ConnectionManager;
import org.htmlparser.nodes.TagNode;
import org.htmlparser.nodes.TextNode;
import org.htmlparser.tags.Bullet;
import org.htmlparser.tags.BulletList;
import org.htmlparser.tags.ParagraphTag;
import org.htmlparser.tags.TableColumn;
import org.htmlparser.tags.TableHeader;
import org.htmlparser.tags.TableRow;
import org.htmlparser.tags.TableTag;
import org.htmlparser.util.NodeIterator;
import org.htmlparser.util.NodeList;
import org.htmlparser.util.ParserException;

/**
 * Main class of Xerox Web Data Extraction.
 * @author Cédric Tarsitano
 */
public class Main {

    /**
     * Last URL typed.
     */
    protected static String lastUrlTyped;
    /**
     * The URL we want to connect to.
     */
    protected static URL url;
    /**
     * The connection used to connect to <i>url</i>.
     */
    protected static ConnectionManager connection;
    /**
     * Vector containing the current list of parsed <i>Data</i>.
     */
    protected static Vector<Data> tempList;
    /**
     * Vector containing all the extracted data.
     */
    protected static Vector<Vector<Data>> finalLists;
    /**
     * True if the parsing process is within a LIST (UL or OL), false otherwise.
     */
    protected static boolean inList;
    /**
     * The current <i>Data</i> extracted.
     */
    protected static Data tempData;
    /**
     * The proxy host affected to <i>connection</i>.
     */
    protected static String proxyHost;
    /**
     * The proxy port affected to <i>connection</i>.
     */
    protected static int proxyPort;
    /**
     * The file containing the configuration information.
     */
    protected static File configFile;
    /**
     * True if a proxy is used for the connection, false otherwise.
     */
    protected static boolean proxy;
    /**
     * True if the configuration frame <i>toolsFrame</i> is closed, false otherwise.
     */
    protected static boolean configIsClosed;
    /**
     * The number of visited web sites.
     */
    protected static int numberRequest;
    /**
     * The screen size.
     */
    protected static Dimension screenSize;
    /**
     * The separator string used to separate the columns of a table in the file where the data is exported.
     */
    protected static String separator;
    /**
     * Frame displaying the extracted data.
     */
    protected static FExtraParsingTools toolsFrame;

    /**
     * Preinitialization of some fields.
     */
    protected static void preInit() {
        tempList = new Vector<Data>();
        finalLists = new Vector<Vector<Data>>();
        inList = false;
        configIsClosed = true;
        proxy = true;
    }

    /**
     * This method initializes the main fields.
     */
    protected static void init() {
        FileInputStream config;
        preInit();
        try {
            //configFile = new File(Main.class.getResource("/config").getFile());
            configFile = new File(System.getProperty("user.home") + System.getProperty("file.separator") + ".webDataExtractionConfig");
            config = new FileInputStream(configFile);
            readConfigFile(config);
        } catch (FileNotFoundException e) {
            JOptionPane.showMessageDialog(null, "Cannot find or open \"" + configFile.getAbsolutePath() + "\".\nPlease configure it.", "Error", JOptionPane.ERROR_MESSAGE);
            configIsClosed = false;
        }
    }

    /**
     * This method is used to set the URL we want to explore.
     * It also sets the proxy parameters read in the configuration file (if any).
     * @param urlName <I>String</I> reprensenting the name of the url we want to connect to.
     */
    @SuppressWarnings("empty-statement")
    protected static void initProxy(String urlName) {
        try {
            lastUrlTyped = urlName;
            url = new URL(urlName);
        } catch (MalformedURLException e) {
            JOptionPane.showMessageDialog(null, "The URL you typed is incorrect !\nThe application will look for a file with the same name.", "Error", JOptionPane.ERROR_MESSAGE);
        }

        connection = new ConnectionManager();
        while (!configIsClosed);
        if (proxy) {
            connection.setProxyHost(proxyHost);
            connection.setProxyPort(proxyPort);
        }
    }

    /**
     * Reads the configuration file and sets the proxy's parameters.
     * @param config The stream containing the configuration information.
     */
    private static void readConfigFile(FileInputStream config) {
        String inputString;
        String[] inputStrings;
        int optOk = 0;
        try {
            if (null == config) {
                throw new IOException();
            }
            DataInputStream dis = new DataInputStream(config);
            inputString = dis.readUTF();
            dis.close();

            inputStrings = inputString.split("\n");
            for (int i = 0; i < inputStrings.length; ++i) {
                String[] s = inputStrings[i].split(" : ");
                for (int j = 0; j < s.length; ++j) {
                    if (s[j].equals("Proxy host")) {
                        try {
                            proxyHost = s[j + 1];
                        } catch (ArrayIndexOutOfBoundsException e) {
                            proxy = false;
                            proxyHost = "";
                        }
                        ++optOk;
                        break;
                    }
                    if (proxy && s[j].equals("Proxy port")) {
                        try {
                            proxyPort = Integer.parseInt(s[j + 1]);
                        } catch (NumberFormatException e) {
                            proxyPort = 8000;
                        } catch (ArrayIndexOutOfBoundsException e) {
                            proxyPort = 8000;
                        }
                        ++optOk;
                        break;
                    }
                    if (s[j].equals("Separator")) {
                        try {
                            separator = s[j + 1];
                        } catch (ArrayIndexOutOfBoundsException e) {
                            separator = "";
                        }
                        ++optOk;
                        break;
                    }
                }
            }
            if (optOk != 3) {
                throw new IOException();
            }
        } catch (IOException e) {
            JOptionPane.showMessageDialog(null, "A problem occured while reading the configuraton file \"" + configFile.getAbsolutePath() + "\".\nPlease correct it.", "Error", JOptionPane.ERROR_MESSAGE);
            configIsClosed = false;
            new FConfiguration(false).setVisible(true);
        }
    }

    /**
     * This method is used to center a frame on the screen.
     * @param f Frame we want to center.
     */
    public static void centerFramePosition(Frame f) {
        int positionX, positionY;

        positionX = ((int) screenSize.getWidth() - f.getWidth()) / 2;
        positionY = ((int) screenSize.getHeight() - f.getHeight()) / 2;
        if (positionX < 0 || positionY < 0) {
            positionX = 0;
            positionY = 0;
        }
        f.setBounds(positionX, positionY, f.getWidth(), f.getHeight());
    }

    /**
     * This method visits recursively the children of <i>node</i>, if any.
     * The text contained within <i>node</i> (and its children) is added to the <i>Vector</i> of <i>Data tempList</i>.
     * @param node The node to visit.
     * @param line int representing the line number of the new data to be created if needed.
     * @param column int representing the column number of the new data to be created if needed.
     * @throws org.htmlparser.util.ParserException
     */
    protected static void visitNode(Node node, int line, int column) throws ParserException {
        String tagText;
        if (node instanceof TextNode) {
            TextNode text = (TextNode) node;
            tagText = text.getText();
            //if(!tagText.matches("\\s*"))
            // Pas mal mais trouver une meilleure condition !
            if (StringUtils.isNotBlank(tagText)) {
                tempData.setContent(tempData.getContent() + StringEscapeUtils.unescapeHtml(tagText));
            }
        } else if (node instanceof TagNode) {
            TagNode tag = (TagNode) node;
            NodeList nl = tag.getChildren();
            if (null != nl) {
                for (NodeIterator i = nl.elements(); i.hasMoreNodes();) {
                    Node cur = i.nextNode();

                    // For embedded data
                    if (cur instanceof BulletList || cur instanceof TableTag) {
                        boolean tmpInList = inList;
                        int tmpLine = line;
                        tempData.setType(Data.EMBED);
                        Data tmpData = new Data(tempData);
                        inList = cur instanceof BulletList;

                        Vector<Data> v = new Vector<Data>(parseEmbeddedData(cur, 0, 0));

                        tempData = new Data(tmpData);
                        tempData.setMetaContent(v);
                        inList = tmpInList;
                        line = tmpLine;
                    } else {
                        visitNode(cur, line, column);
                    }
                    if (inList && (node instanceof BulletList) && (cur instanceof Bullet)) {
                        tempList.addElement(new Data(tempData));
                        if (node instanceof ParagraphTag) {
                            tempData = new Data(Data.PARAGRAPH, ++line, column, "");
                        } else {
                            tempData = new Data(Data.LIST, ++line, column, "");
                        }
                    }
                }
            }
        }
    }

    /**
     * This method parses the embedded data of <i>node</i>, if any.
     * The text contained within <i>node</i> (and its children) is added to the <i>Vector</i> of <i>Data tempList</i>.
     * @param node The node to visit.
     * @param line int representing the line number of the new data to be created if needed.
     * @param column int representing the column number of the new data to be created if needed.
     * @return a <i>Vector of Data</i> containing the embedded data of <i>node</i>.
     * @throws org.htmlparser.util.ParserException
     */
    protected static Vector<Data> parseEmbeddedData(Node node, int line, int column) throws ParserException {
        int tmpLine = line, tmpColumn = column;
        boolean tmpInList = inList;
        Vector<Data> tmpList = new Vector<Data>(tempList);

        tempList.removeAllElements();
        // Process embedded data
        if (node instanceof TableTag) {
            int col = line = 0;
            TableTag tt = (TableTag) node;
            TableRow[] tableRow = tt.getRows();
            for (TableRow tr : tableRow) {
                col = 0;
                TableHeader[] tableHeader = tr.getHeaders();
                for (TableHeader th : tableHeader) {
                    // Begining of the row
                    tempData = new Data(Data.TABLE, line, col, "");
                    visitNode(th, line, col);
                    tempList.addElement(new Data(tempData));
                    ++col;
                }

                TableColumn[] tableColumn = tr.getColumns();
                for (TableColumn tc : tableColumn) {
                    // Begining of the row (after the headers if any)
                    tempData = new Data(Data.TABLE, line, col, "");
                    visitNode(tc, line, col);
                    tempList.addElement(new Data(tempData));
                    ++col;
                }
                // End of the row
                ++line;
            }
        } else {
            line = 0;
            if (node instanceof ParagraphTag) {
                tempData = new Data(Data.PARAGRAPH, line, 0, "");
            } else {
                tempData = new Data(Data.LIST, line, 0, "");
            }
            inList = node instanceof BulletList;
            visitNode(node, line, 0);
            if (!(node instanceof BulletList)) {
                tempList.addElement(new Data(tempData));
            }
            inList = false;
        }

        line = tmpLine;
        column = tmpColumn;
        inList = tmpInList;
        Vector<Data> v = new Vector<Data>(tempList);
        tempList = new Vector<Data>(tmpList);
        return v;
    }

    /**
     * This method parses <i>nodeList</i>.
     * @param nodeList the list of the filtered nodes we want to extract.
     * @throws org.htmlparser.util.ParserException
     */
    protected static void parseNodeList(NodeList nodeList) throws ParserException {
        int line, nbHeaders;
        boolean headers = true;
        Vector<Rowspan> rowspans = new Vector<Rowspan>();
        nbHeaders = 0;

        for (NodeIterator ni = nodeList.elements(); ni.hasMoreNodes();) {
            Node node = ni.nextNode();
            tempList.removeAllElements();
            if (node instanceof TableTag) {
                int col = line = 0;
                rowspans.removeAllElements();
                TableTag tt = (TableTag) node;
                TableRow[] tableRow = tt.getRows();

                // Column count
                int colCount = tableRow[0].getColumnCount() + tableRow[0].getHeaderCount();
                for (TableColumn tc : tableRow[0].getColumns()) {
                    if (null != tc.getAttribute("colspan")) {
                        try {
                            colCount += (Integer.parseInt(tc.getAttribute("colspan")) - 1);
                        } catch (NumberFormatException e) {
                            colCount += 1;
                        }
                    }
                }
                for (TableHeader th : tableRow[0].getHeaders()) {
                    if (null != th.getAttribute("colspan")) {
                        try {
                            colCount += (Integer.parseInt(th.getAttribute("colspan")) - 1);
                        } catch (NumberFormatException e) {
                            colCount += 1;
                        }
                    ///////////////
                    }
                }
                for (int i = 0; i < colCount; ++i) {
                    rowspans.addElement(new Rowspan(0, "", 0));
                }
                for (TableRow tr : tableRow) {
                    col = 0;

                    // Begining of the row
                    TableHeader[] tableHeader = tr.getHeaders();
                    for (TableHeader th : tableHeader) {
                        tempData = new Data(Data.TABLE, line, col, "");
                        visitNode(th, line, col);
                        tempList.addElement(new Data(tempData));
                        ++col;

                        String s2 = th.getAttribute("colspan");
                        if (null != s2) {
                            int count;
                            try {
                                count = Integer.parseInt(s2) - 1;
                            } catch (NumberFormatException e) {
                                count = 1;
                            }
                            for (int i = 0; i < count; ++i) {
                                tempList.addElement(new Data(Data.TABLE, line, col, tempData.getContent()));
                                ++col;
                            }
                        }
                    }
                    if (line == 0) {
                        nbHeaders = col;
                        headers = nbHeaders != 0;
                    }

                    // Begining of the row (after the headers if any)
                    TableColumn[] tableColumn = tr.getColumns();
                    for (TableColumn tc : tableColumn) {
                        String s = tc.getAttribute("rowspan");
                        while (col < nbHeaders) {
                            if (rowspans.elementAt(col).getCount() > 0) {
                                Rowspan r = rowspans.elementAt(col);
                                tempList.addElement(new Data(Data.TABLE, line, col, r.getContent()));
                                r.setCount(r.getCount() - 1);
                                for (int i = 0; i < r.getColspan(); ++i) {
                                    tempList.addElement(new Data(Data.TABLE, line, col + 1, r.getContent()));
                                    ++col;
                                }
                                ++col;
                            } else {
                                break;
                            }
                        }

                        tempData = new Data(Data.TABLE, line, col, "");
                        visitNode(tc, line, col);
                        tempList.addElement(new Data(tempData));

                        int tmpCol = col;
                        String s2 = tc.getAttribute("colspan");
                        int count = 0;
                        if (null != s2) {
                            try {
                                count = Integer.parseInt(s2) - 1;
                            } catch (NumberFormatException e) {
                                count = 1;
                            }
                            for (int i = 0; i < count; ++i) {
                                tempList.addElement(new Data(Data.TABLE, line, tmpCol + 1, tempData.getContent()));
                                ++tmpCol;
                            }
                        }

                        if (null != s) {
                            try {
                                rowspans.elementAt(col).setCount(Integer.parseInt(s) - 1);
                                rowspans.elementAt(col).setContent(tempData.getContent());
                            } catch (NumberFormatException e) {
                                rowspans.elementAt(col).setCount(1);
                                rowspans.elementAt(col).setContent(tempData.getContent());
                            }
                            rowspans.elementAt(col).setColspan(count);
                        }
                        col = tmpCol;
                        ++col;

                        while (col < nbHeaders) {
                            if (rowspans.elementAt(col).getCount() > 0) {
                                Rowspan r = rowspans.elementAt(col);
                                tempList.addElement(new Data(Data.TABLE, line, col, r.getContent()));
                                r.setCount(r.getCount() - 1);
                                for (int i = 0; i < r.getColspan(); ++i) {
                                    tempList.addElement(new Data(Data.TABLE, line, col + 1, r.getContent()));
                                    ++col;
                                }
                                ++col;
                            } else {
                                break;
                            }
                        }
                    }
                    if (line == 0 && !headers) {
                        nbHeaders = col;                    // If some columns are missing
                    }
                    for (int i = 0; i < nbHeaders - col; ++i) {
                        tempList.addElement(new Data(Data.TABLE, line, col, ""));
                        ++col;
                    }
                    // End of the row
                    ++line;
                }
            } else {
                line = 0;
                if (node instanceof ParagraphTag) {
                    tempData = new Data(Data.PARAGRAPH, line, 0, "");
                } else {
                    tempData = new Data(Data.LIST, line, 0, "");
                }
                inList = node instanceof BulletList;
                visitNode(node, line, 0);
                if (!(node instanceof BulletList)) {
                    tempList.addElement(new Data(tempData));
                }
                inList = false;
            }
            // End of the node
            if (!tempList.firstElement().getContent().isEmpty()) {
                finalLists.addElement(new Vector<Data>(tempList));
            }
        }
    }

    /**
     * This method creates a new <i>Vector of Data</i> from <i>elements</i> and then inserts it into <i>finalLists</i> at <i>index</i>.
     * This method is used when a new tab is added to the tab list of FExtraParsingTools.
     * @param elements a <i>String</i> array containing the data of the new tab.
     * @param index the inde where the new tab will be inserted.
     * @return the <i>Vector of Data</i> created and inserted.
     */
    protected static Vector<Data> insertData(String[] elements, int index) {
        Vector<Data> v = new Vector<Data>();
        for (int i = 0; i < elements.length; ++i) {
            v.addElement(new Data(Data.LIST, i, 0, elements[i]));
        }
        finalLists.insertElementAt(v, index);
        return v;
    }

    /**
     * Adds an empty row to <i>v</i>.
     * @param v <i>Vector of Data</i> representing a TABLE or a LIST.
     * @param type The type of the Data contained in <i>v</i> : either Data.TABLE or Data.LIST.
     */
    protected static void addEmptyRow(Vector<Data> v, int type) {
        int colCount = columnCount(v);
        /*if(colCount == 0){
        v.addElement(new Data(type, 0, 0, ""));
        return;
        }*/
        int rowCount = v.size() / colCount;
        for (int i = 0; i < colCount; ++i) {
            v.addElement(new Data(type, rowCount, i, ""));
        }
    }

    /**
     * Adds an empty column to <i>v</i>.
     * @param v <i>Vector of Data</i> representing a TABLE or a LIST.
     * @param type The type of the Data contained in <i>v</i> : either Data.TABLE or Data.LIST.
     * @param newTitle The title of the new column.
     * @return true if column has been added, false otherwise.
     */
    protected static boolean addEmptyColumn(Vector<Data> v, int type, String newTitle) {
        if (null == newTitle) {
            return false;
        }
        int colCount = columnCount(v);
        if (colCount == 0) {
            v.addElement(new Data(type, 0, 0, newTitle));
            return true;
        }
        int rowCount = v.size() / colCount;
        for (int i = 0; i < rowCount; ++i) {
            v.insertElementAt(new Data(type, i, colCount, ""), i * colCount + colCount + i);
        }
        v.elementAt(colCount).setContent(newTitle);
        return true;
    }

    /**
     * Adds a column (containing the data of <i>data</i>) to <i>v</i>.
     * @param v <i>Vector of Data</i> representing a TABLE or a LIST.
     * @param type The type of the Data contained in <i>v</i> : either Data.TABLE or Data.LIST.
     * @param newTitle The title of the new column.
     * @param data The content of the new column.
     * @return true if column has been added, false otherwise.
     */
    protected static boolean addColumn(Vector<Data> v, int type, String newTitle, Vector<String> data) {
        int colCount = columnCount(v);
        if (colCount == 0) {
            v.addElement(new Data(type, 0, 0, ""));
            return true;
        }
        int rowCount = v.size() / colCount;
        for (int i = 0; i < rowCount; ++i) {
            v.insertElementAt(new Data(type, i, colCount, data.elementAt(i)), i * colCount + colCount + i);
        }
        v.elementAt(colCount).setContent(newTitle + "'");
        return true;
    }

    /**
     * Used for debugging.
     */
    protected static void printData() {
        System.out.println("---------------------------------------------------");
        for (Vector<Data> v : finalLists) {
            for (Data data : v) {
                System.out.println(data);
            }
            System.out.println("------------------------ fin ------------------------");
        }
    }

    /**
     * Returns the number of columns of <i>v</i>.
     * @param v <i>Vector of Data</i> representing a TABLE or a LIST.
     * @return The number of columns of <i>v</i>.
     */
    protected static int columnCount(Vector<Data> v) {
        if (v.isEmpty()) {
            return 0;
        }
        int res = 1;
        Data d = v.firstElement();
        for (int i = 1; i < v.size(); ++i) {
            if (d.getLine() != v.elementAt(i).getLine()) {
                break;
            }
            ++res;
        }
        return res;
    }

    /**
     * Returns the headers of <i>v</i>.
     * @param v <i>Vector of Data</i> representing a TABLE or a LIST.
     * @return <i>Vector of Data</i> containing the headers of <i>v</i>.
     */
    protected static Vector<Data> getHeaders(Vector<Data> v) {
        //return getLine(v, 0);
        return getElementsAtRow(v, 0);
    }

    /**
     * Returns the headers of <i>v</i> as String. Useful for the column names.
     * @param v <i>Vector of Data</i> representing a TABLE or a LIST.
     * @return <i>Vector of String</i> containing the headers of <i>v</i>.
     */
    protected static Vector<String> getHeadersAsString(Vector<Data> v) {
        return getLineAsString(v, 0);
    }

    /**
     * Returns the content of the line at <i>lineNumber</i> in <i>v</i>.
     * @param v <i>Vector of Data</i> representing a TABLE or a LIST.
     * @param lineNumber The number of the line we want to get.
     * @return <i>Vector of String</i> containing the content of the line <i>lineNumber</i>.
     */
    protected static Vector<String> getLineAsString(Vector<Data> v, int lineNumber) {
        Vector<String> res = new Vector<String>();
        Vector<Data> temp = getElementsAtRow(v, lineNumber);
        /*boolean found = false;
        for (int i = 0; i < v.size(); ++i) {
        if(v.elementAt(i).getLine() == lineNumber){
        res.addElement(new String(v.elementAt(i).getContent()));
        found = true;
        }
        if(found && v.elementAt(i).getLine() != lineNumber)
        break;
        }*/
        for (Data data : temp) {
            res.addElement(new String(data.getContent()));
        }
        return res;
    }

    /**
     * Return the content of the whole table except the first line.
     * @param v <i>Vector of Data</i> representing a TABLE or a LIST.
     * @return <i>Vector of Vector of String</i> representing the content of <i>v</i>.
     */
    protected static Vector<Vector<String>> getDataContent(Vector<Data> v) {
        Vector<Vector<String>> res = new Vector<Vector<String>>();
        if (v.isEmpty()) {
            return res;
        }
        int size = v.size() / columnCount(v);
        for (int i = 1; i < size; ++i) {
            res.addElement(new Vector<String>(getLineAsString(v, i)));
        }
        return res;
    }

    /**
     * Returns the elements present in <i>v</i> at <i>numCol</i>. 
     * @param v <i>Vector of Data</i> representing a TABLE or a LIST.
     * @param numCol The column index we want to get.
     * @return <i>Vector of Data</i> containing the elements of <i>v</i> at <i>numCol</i>.
     */
    protected static Vector<Data> getElementsAtColumn(Vector<Data> v, int numCol) {
        Vector<Data> res = new Vector<Data>();
        int colCount = columnCount(v);
        int rowCount = v.size() / colCount;
        /*for(Data data : v)
        if(data.getColumn() == numCol)
        res.addElement(data);*/
        for (int i = 0; i < rowCount; ++i) {
            res.addElement(v.elementAt(i * colCount + numCol));
        }
        return res;
    }

    /**
     * Returns the elements present in <i>v</i> at <i>numRow</i>. 
     * @param v <i>Vector of Data</i> representing a TABLE or a LIST.
     * @param numRow The row index we want to get.
     * @return <i>Vector of Data</i> containing the elements of <i>v</i> at <i>numRow</i>.
     */
    protected static Vector<Data> getElementsAtRow(Vector<Data> v, int numRow) {
        Vector<Data> res = new Vector<Data>();
        int colCount = columnCount(v);
        int base = numRow * colCount;
        /*for(Data data : v)
        if(data.getLine() == numRow)
        res.addElement(data);*/
        for (int i = 0; i < colCount; ++i) {
            res.addElement(v.elementAt(base + i));
        }
        return res;
    }

    /**
     * This method is used when a column is removed in <i>v</i>. It refreshes the column indexes.
     * @param v <i>Vector of Data</i> representing a TABLE or a LIST.
     * @param numCol The index of the removed column.
     */
    protected static void updateDataColumns(Vector<Data> v, int numCol) {
        for (Data data : v) {
            if (data.getColumn() > numCol) {
                data.setColumn(data.getColumn() - 1);
            }
        }
    }

    /**
     * This method is used when a row is removed in <i>v</i>. It refreshes the row indexes.
     * @param v <i>Vector of Data</i> representing a TABLE or a LIST.
     * @param numRow The index of the removed row.
     */
    protected static void updateDataRows(Vector<Data> v, int numRow) {
        for (Data data : v) {
            if (data.getLine() > numRow) {
                data.setLine(data.getLine() - 1);
            }
        }
    }

    /**
     * This method is used when a column is moved. It switches the elements present at <i>fromIndex</i> and <i>toIndex</i>.
     * @param v <i>Vector of Data</i> representing a TABLE or a LIST.
     * @param fromIndex The index of the column moved.
     * @param toIndex The index where the moved column goes.
     */
    protected static void sortColumns(Vector<Data> v, int fromIndex, int toIndex) {
        Data d;
        int colCount = columnCount(v);
        int rowCount = v.size() / colCount;
        for (int i = 0; i < rowCount; ++i) {
            d = v.elementAt(i * colCount + fromIndex);
            v.setElementAt(v.elementAt(i * colCount + toIndex), i * colCount + fromIndex);
            v.setElementAt(d, i * colCount + toIndex);
        }
    }

    /**
     * @param args the command line arguments
     */
    @SuppressWarnings("empty-statement")
    public static void main(String[] args) {
        lastUrlTyped = "";
        screenSize = Toolkit.getDefaultToolkit().getScreenSize();
        init();
        numberRequest = 0;
        FMenu mainFrame = new FMenu();
        mainFrame.setVisible(true);
        if (!configIsClosed) {
            new FConfiguration(false).setVisible(true);
        }
    }
}
