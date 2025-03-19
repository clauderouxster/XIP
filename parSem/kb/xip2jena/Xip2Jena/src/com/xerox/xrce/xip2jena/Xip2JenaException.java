package com.xerox.xrce.xip2jena;

/**
 *
 * @author akaplan
 */
public class Xip2JenaException extends Exception {

    public Xip2JenaException(String string) {
        super(string);
    }
    public Xip2JenaException(Exception e) {
        super(e);
    }

}
