/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package db_testsimple;

/**
 *
 * @author ctrojahn
 */
public class Constant {

        /* the name of the DB */
    /**
     *
     */
    protected static final String dbName = "advUI";
        /* the name of item table in the DB */
    /**
     *
     */
    protected static final String docTableName = "Document";
        /*  */
        /**
         *
         */
        public static final boolean RECURSIVELY = true;
        /*  */
        /**
         *
         */
        public static final boolean NOT_RECURSIVELY = false;
        /*  */
        /**
         *
         */
        public static final boolean DROPANDCREATETABLES = true;
        /*  */
        /**
         *
         */
        public static final boolean NOT_DROPANDCREATETABLES = false;
        /*  */
        /**
         *
         */
        public static final int PAGINIG = 20;
        /*  */
        /**
         *
         */
        public static final int MAX_DOCS_SEARCH = 100;
        /*  */
        /**
         *
         */
        public static final String[] FACETS = {"language"};
        /*  */
        /**
         *
         */
        public  static int TRUNC_TITLE = 50;
}
