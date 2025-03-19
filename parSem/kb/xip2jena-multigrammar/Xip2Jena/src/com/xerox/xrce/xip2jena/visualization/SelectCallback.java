/*
 * Copyright Xerox 2009
 */

package com.xerox.xrce.xip2jena.visualization;

import com.hp.hpl.jena.ontology.Individual;

/**
 *
 * @author akaplan
 */
public interface SelectCallback {
    public void select(Individual i);
}
