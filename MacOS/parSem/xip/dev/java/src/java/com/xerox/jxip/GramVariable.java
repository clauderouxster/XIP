/*
 *   Xerox Research Centre Europe - Grenoble Laboratory
 *           project XIP (Xerox Incremental Parser)
 *  Copyright (C) 2004 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved
 *
 */
package com.xerox.jxip;

/**
 * Description of the Class
 *
 * @author grondeau @created 17 janvier 2006
 */
public class GramVariable implements Comparable {

    //public static enum VariableType {UNKNOW, REAL, INTEGER, STRING, STRUCTURE, PYTHONOBJECT, XMLOBJECT};
    public static final int UNKNOW = 0;
    public static final int REAL = 1;
    public static final int INTEGER = 2;
    public static final int STRING = 3;
    public static final int STRUCTURE = 4;
    public static final int PYTHONOBJECT = 5;
    public static final int XMLOBJECT = 6;
    private String name;
    //private VariableType type;
    private int type;
    private long offset;
    private String fileName;

    /**
     * Constructor for the GramVariable object
     *
     * @param nam
     * @param typ
     */
    public GramVariable(String nam, int typ) {
        name = nam;
        type = typ;
        /*
         * switch (typ) { case 0: type = VariableType.UNKNOW; break; case 1:
         * type = VariableType.REAL; break; case 2: type = VariableType.INTEGER;
         * break; case 3: type = VariableType.STRING; break; case 4: type =
         * VariableType.STRUCTURE; break; case 5: type =
         * VariableType.PYTHONOBJECT; break; case 6: type =
         * VariableType.XMLOBJECT; break; }
         */
    }

    /**
     * Gets the name attribute of the GramVariable object
     *
     * @return The name value
     */
    public synchronized String getName() {
        return name;
    }

    /**
     * Gets the type attribute of the GramVariable object
     *
     * @return The type value
     */
    //public synchronized VariableType getType() {return type;}
    public synchronized int getType() {
        return type;
    }

    /**
     *
     *
     * @return
     */
    @Override
    public synchronized String toString() {
        return name + "{" + type + "}";
    }

    /**
     *
     * @param obj
     * @return
     */
    @Override
    public synchronized int compareTo(java.lang.Object obj) {
        String str1 = ((GramVariable) obj).getName();
        return name.compareTo(str1);
    }
}
