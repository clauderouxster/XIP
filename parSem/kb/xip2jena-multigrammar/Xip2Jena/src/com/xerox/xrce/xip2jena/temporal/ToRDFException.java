/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.xerox.xrce.xip2jena.temporal;

/**
 *
 * @author nlagos
 */
public class ToRDFException extends Exception {
    public ToRDFException(String string) {
        super(string);
    }
    public ToRDFException(Exception e) {
        super(e);
    }
}
