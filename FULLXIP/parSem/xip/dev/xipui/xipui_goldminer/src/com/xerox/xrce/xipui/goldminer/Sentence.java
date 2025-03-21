/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 *
 * Sentence.java
 *
 * Created on October 4, 2006, 10:11 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer;

import java.util.*;

/**
 *
 * @author vgrassau
 */
public class Sentence {

    private int idsent = -1;
    private Collection<String> dependencies;
    private Map<String, Parameter> mapParameter;
    private int priority = 1;
    private boolean hasConstraint = false;
    public static final int NORMAL = 0;
    public static final int MEDIUM = 1;
    public static final int STRONG = 2;
    private int level = NORMAL;

    /** Creates a new instance of Sentence
     * @param idsent 
     */
    public Sentence(int idsent) {
        this.idsent = idsent;
        mapParameter = new Hashtable();
        dependencies = new Vector();

    }

    public boolean hasConstraint() {
        return this.hasConstraint;
    }

    public Collection<String> getDependencies() {
        return this.dependencies;
    }

    public void addDependency(String iddep) {
        if (!dependencies.contains(iddep)) {
            dependencies.add(iddep);
        }
    }

    public void setLevel(int level_) {
        switch (level_) {
            case STRONG: {
                level = STRONG;
                break;
            }
            case MEDIUM: {
                if (level < STRONG) {
                    level = MEDIUM;
                }
                break;
            }
            case NORMAL: {
                if (level < STRONG && level < MEDIUM) {
                    level = NORMAL;
                }
                break;
            }
            default: {
                this.level = NORMAL;
                break;
            }
        }
    }

    public int getLevel() {
        return this.level;
    }

    public int addPriority(int priority_) {
        priority += priority_;
        return priority;
    }

    public void addParameter(Parameter parameter) {
        mapParameter.put(parameter.getId(), parameter);
    }

    public int getIdSent() {
        return this.idsent;
    }

    public Collection getAllIdParameter() {
        return this.mapParameter.keySet();
    }

    public int getPriority() {
        return this.priority;
    }

    public boolean contains(int iddep) {
        return this.dependencies.contains(iddep);
    }

    public Parameter getParameter(String id_param) {
        return this.mapParameter.get(id_param);
    }

    public String toString() {
        return "sent:" + idsent + "(" + priority + ")" + "concept :" + this.mapParameter.values();
    }

    public boolean equals(Object o) {
        boolean res = false;
        if (o instanceof Sentence) {
            res = this.getIdSent() == ((Sentence) o).getIdSent();
        } else if (o instanceof Integer) {
            res = this.getIdSent() == ((Integer) o).intValue();
        }
        return res;
    }
}//
