/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 *
 * GoldminerOffset.java
 *
 * Created on October 4, 2006, 2:28 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.xerox.xrce.xipui.goldminer;

import com.xerox.xrce.xipui.Offset;
import java.util.*;

/**
 *
 * @author vgrassau
 */
public class GoldminerOffset {

    private boolean isConstraint = false;
    private int type = Sentence.NORMAL;
    private Offset offset;
    private Collection<BasicElement> elements;
    private boolean isSentence = false;
    private Collection<String> concepts;

    /** Creates a new instance of GoldminerOffset
     *@param offset an Offset
     */
    public GoldminerOffset(Offset offset) {
        this.offset = offset;
        elements = new Vector();
        concepts = new Vector();
    }

    /***
     *Adds some concepts
     * @param list 
     */
    public void addConcepts(Collection<String> list) {
        for (Iterator<String> it = list.iterator(); it.hasNext();) {
            addConcept(it.next());
        }
    }

    /**
     *Adds a concept the lemma form
     *@param concept the concept to add
     */
    public void addConcept(String concept) {
        if (!concepts.contains(concept)) {
            concepts.add(concept);
        }
    }

    /**
     *Returns the
     * @return 
     */
    public Collection<String> getConcepts() {
        return this.concepts;
    }

    public boolean isConstraint() {
        return this.isConstraint;
    }

    public void setIsConstraint(boolean b) {
        this.isConstraint = b;
    }

    public void setType(int type) {
        this.type = type;
    }

    public int getType() {
        return this.type;
    }

    public void setSentenceOffset(boolean b) {
        this.isSentence = b;
    }

    public boolean isSentenceOffset() {
        return this.isSentence;
    }

    public void addElement(BasicElement element) {
        if (!elements.contains(element)) {
            elements.add(element);
        }
    }

    public void addElements(Collection<BasicElement> elements) {
        if (elements != null) {
            Iterator<BasicElement> ite = elements.iterator();
            while (ite.hasNext()) {
                addElement(ite.next());
            }
        }
    }

    public Offset getOffset() {
        return this.offset;
    }

    public boolean equals(Object o) {
        if (o instanceof GoldminerOffset) {
            return offset.equals(((GoldminerOffset) o).getOffset());
        } else {
            return false;
        }
    }

    public String toString() {
        return this.offset + "\n elements : " + elements + "\n";
    }

    public Collection<BasicElement> getElements() {
        return this.elements;
    }

    public long getRight() {
        return this.offset.getRight();
    }

    public long getLeft() {
        return this.offset.getLeft();
    }
}//end if class
