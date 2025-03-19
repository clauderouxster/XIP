package com.xerox.xrce.xip2jena;

import com.hp.hpl.jena.ontology.Individual;
import com.hp.hpl.jena.ontology.OntClass;
import com.xerox.jxip.XipNode;
import java.net.URL;

public interface IndivHandler {
    public void handleIndiv(Individual indiv, XipNode xipNode, OntClass ontClass, URL docUrl)
            throws Xip2JenaException;
}
