/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 * ListTools.java
 *
 * Created on August 10, 2005, 10:30 AM
 *
 * To change this template, choose Tools | Options and locate the template under
 * the Source Creation and Management node. Right-click the template and choose
 * Open. You can then make changes to the template in the Source Editor.
 */
package com.xerox.xrce.xipui.util;

import java.util.*;

/**
 *
 * @author vgrassau
 */
public class ListTools {

    /** Creates a new instance of ListTools */
    public ListTools() {
    }

    static public Vector inter(Vector u, Vector v) {
        Vector inter = new Vector();
        Enumeration enumV = v.elements();
        while (enumV.hasMoreElements()) {
            Object o = enumV.nextElement();
            if (u.contains(o)) {
                inter.add(o);
            }
        }
        return inter;
    }

    static public Vector union(Vector u, Vector v) {
        Vector union = new Vector();
        union.addAll(u);
        Enumeration enumV = v.elements();
        while (enumV.hasMoreElements()) {
            Object o = enumV.nextElement();
            if (!u.contains(o)) {
                union.add(o);
            }
        }
        return union;
    }

    static public Vector interVector(Vector v) {
        Vector res = new Vector();
        if (v.size() > 0) {
            res = (Vector) v.get(0);
            for (int i = 1; i < v.size(); i++) {
                res = ListTools.inter(res, (Vector) v.get(i));
            }
        }
        return res;
    }

    static public Vector sortString(Vector v) {
        //on tri le vector du plus petit au plus grand.
        Vector sorted = new Vector();
        Enumeration e = v.elements();
        while (e.hasMoreElements()) {
            String id = (String) e.nextElement();
            int indexToInsert = 0;
            boolean stop = false;
            for (int i = 0; i < sorted.size() && !stop; i++) {
                String idSorted = (String) sorted.get(i);
                if (id.compareTo(idSorted) < 0) {
                    indexToInsert = i;
                    stop = true;
                } else {
                    indexToInsert = i + 1;
                }
            }
            sorted.add(indexToInsert, id);
        }
        return sorted;
    }

    static public Vector sortEnumeration(Enumeration e, boolean increasing) {
        try {
            Vector sorted = new Vector();
            while (e.hasMoreElements()) {
                Object key = e.nextElement();
                Comparable cKey = (Comparable) key;
                int indexToInsert = 0;
                boolean stop = false;
                for (int i = 0; i < sorted.size() && !stop; i++) {
                    boolean res = false;
                    if (increasing) {
                        res = cKey.compareTo(sorted.get(i)) < 0;
                    } else {
                        res = cKey.compareTo(sorted.get(i)) > 0;
                    }
                    if (res) {
                        indexToInsert = i;
                        stop = true;
                    } else {
                        indexToInsert = i + 1;
                    }
                }
                sorted.add(indexToInsert, key);
            }
            return sorted;
        } catch (Exception ex) {
            return createVector(e);
        }
    }

    static private Vector createVector(Enumeration e) {
        Vector v = new Vector();
        while (e.hasMoreElements()) {
            v.add(e.nextElement());
        }
        return v;
    }
}
