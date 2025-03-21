/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package extractionweb;

import java.util.Vector;
import org.apache.commons.lang.StringUtils;

/**
 * Class used to collect several informations about the data we want to extract.
 * Note: this class has a natural ordering that is inconsistent with equals.
 * @author Cédric Tarsitano
 */
public class Data implements Comparable <Data>{
    
    public static final int TABLE = 0;
    public static final int LIST = 1;
    public static final int PARAGRAPH = 2;
    public static final int EMBED = 3;

    private int mType;
    private int mLine;
    private int mColumn;
    private String mContent;
    private String mSource;
    private Vector <Data> mMetaContent;
    
    /**
     * Creates a new instance of Data.
     * @param type <I>int</I> reprensenting the type of data. Either content of a list or content of a table.
     * @param line <I>int</I> the line number where the data appears.
     * @param column <I>int</I> the column number where the data appears. 0 if the type is a list.
     * @param content <I>String</I> reprensenting the data itself.
     */
    public Data(int type, int line, int column, String content){
        mType = type;
        mLine = line;
        if(/*type == LIST ||*/ type == PARAGRAPH)
            mColumn = 0;
        else
            mColumn = column;
        mContent = content;
        mSource = Main.lastUrlTyped;
        mMetaContent = null;
        if(type == EMBED)
            mMetaContent = new Vector <Data> ();
    }
    
    /**
     * Creates a new instance of Data.
     * @param type <I>int</I> reprensenting the type of data. Either content of a list or content of a table.
     * @param line <I>int</I> the line number where the data appears.
     * @param column <I>int</I> the column number where the data appears. 0 if the type is a list.
     * @param metaContent <I>Vector <Data></I> reprensenting the data embedded.
     */
    public Data(int type, int line, int column, Vector <Data> metaContent){
        mType = type;
        mLine = line;
        if(/*type == LIST ||*/ type == PARAGRAPH)
            mColumn = 0;
        else
            mColumn = column;
        mContent = "";
        mSource = Main.lastUrlTyped;
        mMetaContent = null;
        if(type == EMBED)
            mMetaContent = new Vector <Data> (metaContent);
    }
    
    /**
     * Creates a new instance of Data.
     * @param type <I>int</I> reprensenting the type of data. Either content of a list or content of a table.
     * @param line <I>int</I> the line number where the data appears.
     * @param column <I>int</I> the column number where the data appears. 0 if the type is a list.
     * @param content <I>String</I> reprensenting the data itself.
     * @param source <I>String</I> reprensenting the url where the data comes from.
     */
    public Data(int type, int line, int column, String content, String source){
        mType = type;
        mLine = line;
        if(/*type == LIST ||*/ type == PARAGRAPH)
            mColumn = 0;
        else
            mColumn = column;
        mContent = content;
        mSource = source;
        mMetaContent = null;
        if(type == EMBED)
            mMetaContent = new Vector <Data> ();
    }
    
    /**
     * Creates a new instance of Data.
     * @param data <I>Data</I> reprensenting the data.
     */
    public Data(Data data){
        mType = data.getType();
        mLine = data.getLine();
        mColumn = data.getColumn();
        mContent = data.getContent();
        mSource = data.getSource();
        // Not sure, maybe new Vector <Data> (data.getMetaContent());
        mMetaContent = data.getMetaContent();
    }
    
    @Override
    public int compareTo(Data d){
        System.out.println("Je passe");
        if(StringUtils.isNumeric(d.getContent()) && StringUtils.isNumeric(mContent)){
            System.out.println("Je passe 2 !");
            return Integer.parseInt(mContent) - Integer.parseInt(d.getContent());}
        return mContent.compareTo(d.getContent());
    }
    
    public int getType(){
        return mType;
    }
    
    public void setType(int type){
        mType = type;
    }
    
    public int getLine(){
        return mLine;
    }
    
    public void setLine(int line){
        mLine = line;
    }
    
    public int getColumn(){
        return mColumn;
    }
    
    public void setColumn(int column){
        mColumn = column;
    }
    
    public String getContent(){
        return mContent;
    }
    
    public void setContent(String content){
        mContent = content;
    }
    
    public String getSource(){
        return mSource;
    }
    
    public void setSource(String source){
        mSource = source;
    }
    
    public Vector <Data> getMetaContent(){
        return mMetaContent;
    }
    
    public void setMetaContent(Vector <Data> metaContent){
        mMetaContent = new Vector<Data> (metaContent);
    }
    
    @Override
    public String toString(){
        if(mType == EMBED){
            String s = "";
            for (Data data : mMetaContent) 
                s += "\tType : " + data.getType() +
                     "\n\tLine : " + data.getLine() +
                     "\n\tColumn : " + data.getColumn() +
                     "\n\tContent : " + data.getContent() +
                     "\n\tSource : " + data.getSource() + "\n\n";
            return "Type : " + mType +
                   "\nLine : " + mLine +
                   "\nColumn : " + mColumn +
                   "\nContent : " + mContent +
                   "\nSource : " + mSource +
                   "\nChildren : \n" +
                   s + "\n";
        }
        return "Type : " + mType + "\nLine : " + mLine + "\nColumn : " + mColumn + "\nContent : " + mContent + "\nSource : " + mSource + "\n";
    }
}
