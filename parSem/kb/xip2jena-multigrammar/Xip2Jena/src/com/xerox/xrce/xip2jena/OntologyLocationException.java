package com.xerox.xrce.xip2jena;

public class OntologyLocationException extends Exception {

    public OntologyLocationException(Exception e) {
        super(e);
    }

    public OntologyLocationException(String s, Exception e) {
        super(s, e);
    }

    public OntologyLocationException(String s) {
        super(s);
    }
}
