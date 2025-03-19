/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package extractionweb;

/**
 * This class is useful when the data extracted is a table and contains rowspans.
 * If the table contains rowspans then the content is duplicated into the cell(s) supposed to be merged.
 * @author Cédric Tarsitano
 */
public class Rowspan {

    /**
     * Field representing the remaining number of row(s) this cell has to be duplicated in. Given by the rowspan argument value.
     */
    private int mCount;
    /**
     * Field representing the content to duplicate.
     */
    private String mContent;
    /**
     * representing the remaining number of column(s) this cell has to be duplicated in the same row. Given by the colspan argument value.
     */
    private int mColspan;
    
    /**
     * Creates a new instance of Rowspan.
     * @param count <i>int</i> representing the remaining number of row(s) this cell has to be duplicated in. Given by the rowspan argument value.
     * @param content <i>String</i> representing the content to duplicate.
     * @param colspan <i>int</i> representing the remaining number of column(s) this cell has to be duplicated in the same row. Given by the colspan argument value.
     */
    public Rowspan(int count, String content, int colspan){
        mCount = count;
        mContent = content;
        mColspan = colspan;
    }
    
    /**
     * Returns the remaining number of row(s) this cell has to be duplicated in.
     * @return an <i>int</i> representing the remaining number of row(s) this cell has to be duplicated in.
     */
    protected int getCount(){
        return mCount;
    }
    
    /**
     * Sets the remaining number of row(s) this cell has to be duplicated in to <i>count</i>.
     * @param count <i>int</i> representing the new value of the remaining number of row(s) this cell has to be duplicated in.
     */
    protected void setCount(int count){
        mCount = count;
    }
    
    /**
     * Returns the content to be duplicated.
     * @return a <i>String</i> representing the content to be duplicated.
     */
    protected String getContent(){
        return mContent;
    }
    
    /**
     * Sets the content to be duplicated to <i>content</i>.
     * @param content a <i>String</i> representing the new value of the content to be duplicated.
     */
    protected void setContent(String content){
        mContent = content;
    }
    
    /**
     * Returns the remaining number of column(s) this cell has to be duplicated in the same row.
     * @return an <i>int</i> representing the remaining number of column(s) this cell has to be duplicated in the same row.
     */
    protected int getColspan(){
        return mColspan;
    }
    
    /**
     * Sets the remaining number of column(s) this cell has to be duplicated in the same row to <i>count</i>.
     * @param colspan an <i>int</i> representing the remaining number of column(s) this cell has to be duplicated in the same row.
     */
    protected void setColspan(int colspan){
        mColspan = colspan;
    }
}
