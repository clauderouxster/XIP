/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package db_testsimple_3_ws;

import java.util.Hashtable;
import java.util.Vector;

/**
 *
 * @author ctrojahn
 */
public class Constant {

    /* the name of the DB */
    protected static final String dbName = "advUI";
    /* the name of item table in the DB */
    protected static final String docTableName = "Document";
    /*  */
    public static final boolean RECURSIVELY = true;
    /*  */
    public static final boolean NOT_RECURSIVELY = false;
    /*  */
    public static final boolean DROPANDCREATETABLES = true;
    
    public static final int PAGINIG = 20;
    /*  */
    public static final int MAX_DOCS_SEARCH = 200;
    /*  */
    public static final int NUM_THREAD = 5;
        /*  */
    public static int TRUNC_TITLE = 50;
    public static final String[] FACETS = {"language","libraryID"};

    // Position 0: field Position 1: correponding facet
    public static Hashtable MAPPING = new Hashtable();
    public static Hashtable FACETS_TEMP = new Hashtable();

    public static void initializeMapping() {
           MAPPING.put("Score","score");
           MAPPING.put("LibraryID", "libraryID");
           MAPPING.put("Title", "title");
           MAPPING.put("Publisher", "publisher");
           MAPPING.put("URL","url");
           MAPPING.put("ISSN", "issn");
           MAPPING.put("ISBN","isbn");
           MAPPING.put("TelRecordID_URL","relRecordID_URL");
           MAPPING.put("Subjects","subjects");
           MAPPING.put("TableOfContents","tableOfContents");
           MAPPING.put("Languages","language");

           FACETS_TEMP.put("Languages","language");
           FACETS_TEMP.put("LibraryID", "libraryID");

    }
}
