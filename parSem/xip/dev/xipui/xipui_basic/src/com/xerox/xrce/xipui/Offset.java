/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 * 
 * Copyright (c) 2006 - 2007 Xerox Corporation.  All Rights Reserved.
 * 
 ****************************************************************************************
 * Copyright (c) 2005 Xerox Corporation                                                 *
 * Xerox Research Centre Europe - Grenoble                                              *
 * All Rights Reserved                                                                  *
 * Copyright protection claimed includes all forms and matters of copyrightable         *
 * material and information now allowed by statutory or judicial law or                 *
 * hereinafter granted, including without limitation, material generated from           *
 * the software programs which are displayed on the screen such as icons,               *
 * screen display looks, etc. All rights reserved. No part of this document             *
 * may be reproduced, stored in a retrieval system, or transmitted, in any              *
 * form or by any means, electronic, mechanical, photocopied, recorded, or              *
 * otherwise, without prior written consent from XEROX corporation.                     *
 ****************************************************************************************
 */
package com.xerox.xrce.xipui;

/**
 *
 * @author Vianney Grassaud
 */
public class Offset implements Comparable {

    protected long left;
    protected long right;

    /** Creates a new instance of Offset */
    public Offset() {
        left = 0;
        right = 0;
    }

    public Offset(long left_, long right_) {
        setLeft(left_);
        setRight(right_);
    }

    public void setLeft(long left_) {
        this.left = left_;
    }

    public void setRight(long right_) {
        this.right = right_;
    }

    public long getRight() {
        return this.right;
    }

    public long getLeft() {
        return this.left;
    }

    public boolean equals(Object o) {
        if (o instanceof Offset) {
            Offset off = (Offset) o;
            if (off.getRight() == this.right && off.getLeft() == this.left) {
                return true;
            }
        }
        return false;
    }

    public int compareTo(Object o) {
        Integer left_ = new Integer((int) getLeft());

        if (o instanceof Offset) {
            Offset o2 = (Offset) o;
            return left_.compareTo((int) o2.getLeft());
        } else if (o instanceof Integer) {
            return left_.compareTo((Integer) o);
        } else {
            return -1;
        }
    }

    public String toString() {
        return "[" + this.left + " : " + this.right + "]";
    }
}
