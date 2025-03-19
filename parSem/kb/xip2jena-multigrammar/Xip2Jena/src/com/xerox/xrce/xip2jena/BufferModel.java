/*
 * Copyright Xerox 2009
 */

package com.xerox.xrce.xip2jena;

import com.hp.hpl.jena.ontology.OntModel;
import com.hp.hpl.jena.rdf.model.InfModel;
import com.hp.hpl.jena.rdf.model.Model;
import com.hp.hpl.jena.rdf.model.ModelFactory;
import com.hp.hpl.jena.reasoner.Reasoner;
import com.hp.hpl.jena.reasoner.ValidityReport;
import java.util.Iterator;
import org.apache.log4j.Logger;
import org.mindswap.pellet.jena.PelletReasonerFactory;

/**
 * Make assertions into tempModel.  From time to time, call commit().  The
 * contents of tempModel will be checked for consistency with the ontology.
 * If they're consistent, they will be copied into destModel.
 */
// What I really wanted to do was implement the Model interface, so I could use
// this transparently like any other model.  But first of all, adding the commit()
// method changes the semantics of assertion methods, and that would arguably
// violate the contract of the interface, and second, Model is an enormous
// interface that would be a pain to implement.
public class BufferModel {
    public OntModel tempModel;

    private Model destModel;
    private InfModel consistencyCheckingModel;
    private Logger log = Logger.getLogger(BufferModel.class);

    public BufferModel(OntModel ontologyModel, Model theDestModel) {
        // I originally allocated a new sentenceModel and infModel on the stack in
        // processXipUnit(), but there was a memory leak somewhere that caused
        // the heap to be exhausted rather quickly.  So now I'm reusing them and
        // calling removeAll() each time.
        //
        // Note that we have an InfModel as a submodel of another InfModel,
        // both with the same reasoner.  I'm not sure this is a good idea, in terms
        // of efficiency.  But it allows us to clear the sentenceModel without clearing
        // or re-"classifying" the ontology.
        destModel = theDestModel;
        tempModel = ModelFactory.createOntologyModel();
        Reasoner reasoner = PelletReasonerFactory.theInstance().create();
        consistencyCheckingModel = ModelFactory.createInfModel(reasoner, ontologyModel, tempModel);
    }

    /**
     * Check tempModel for consistency.  If consistent, copy contents into destModel
     * and return true, else return false.
     */
    public boolean commit() {
        ValidityReport validity = consistencyCheckingModel.validate();
        boolean success;
        if (validity.isValid()) {
            destModel.add(tempModel.getRawModel());
            success = true;
        } else {
            log.warn("Model inconsistent with ontology.");
            if (log.isDebugEnabled()) {
                for (Iterator i = tempModel.getRawModel().listStatements(); i.hasNext();) {
                    log.debug(i.next());
                }
            }
            success =false;
        }
        consistencyCheckingModel.removeAll();
        tempModel.removeAll();
        return success;
    }
}
