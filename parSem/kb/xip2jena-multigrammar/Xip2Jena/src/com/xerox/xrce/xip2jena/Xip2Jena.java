/*
 * Copyright Xerox 2009
 */

package com.xerox.xrce.xip2jena;

import com.hp.hpl.jena.datatypes.RDFDatatype;
import com.hp.hpl.jena.ontology.Individual;
import com.hp.hpl.jena.ontology.OntClass;
import com.hp.hpl.jena.ontology.OntModel;
import com.hp.hpl.jena.rdf.model.Literal;
import com.hp.hpl.jena.rdf.model.Model;
import com.hp.hpl.jena.rdf.model.ModelFactory;
import com.hp.hpl.jena.util.iterator.ExtendedIterator;
import com.xerox.jxip.XipNode;
import java.io.UnsupportedEncodingException;
import java.net.URL;
import java.net.URLEncoder;
import java.util.HashMap;
import java.util.Map;
import org.apache.log4j.Logger;
import org.mindswap.pellet.jena.PelletReasonerFactory;
import static com.xerox.xrce.xip2jena.Util.registerOntology;

/**
 * A tool for building Jena Models from XIP nodes and dependencies.
 */
public class Xip2Jena {
    private static final String parsemBase = "http://xrce.xerox.com/parsem/";
    public static final String indivBase = parsemBase + "entities/";
    public static final String mentionBase = indivBase + "mentions/";
    public static final String grammarBase = parsemBase + "grammars/";

    private static Logger log = Logger.getLogger(Xip2Jena.class);
    
    protected OntModel ontology;
    public OntModel getOntology(){return ontology;}

    private String grmUri;
    public String getGrmUri() { return grmUri; }

    private Map<Long, Individual> offset2Individual;
    private Map<XipNode, Individual> xipNode2Individual;
    private Map<OntClass, IndivHandler> indivHandlers;
    private BufferModel bufferModel;

    public Xip2Jena(URL ontUrl, String aGrmUri, Model destModel) throws OntologyLocationException {
        grmUri = aGrmUri;
        ontology = ModelFactory.createOntologyModel(PelletReasonerFactory.THE_SPEC);

        registerOntology(ParsemGenericOntology.class);

        ontology.read(ontUrl.toString());
        bufferModel = new BufferModel(ontology, destModel);
        offset2Individual = new HashMap<Long, Individual>();
        xipNode2Individual = new HashMap<XipNode, Individual>();
        indivHandlers = new HashMap<OntClass, IndivHandler>();

    }

    public Individual addMention(XipNode node, URL docUrl, Individual entity) throws Xip2JenaException {
        String mentionUrl;
        try {
            mentionUrl = mentionBase +
                    URLEncoder.encode(node.getLeftChar() + "-" +
                                         node.getRightChar() + ":" +
                                         docUrl.toString(),
                                      "UTF8");
            // put offsets before doc URL because otherwise we'd risk collisions:
            // there could be two documents A and B where B's URL was
            // A's URL + ":" + two integers separated by a "-".
        } catch (UnsupportedEncodingException ex) {
            // won't happen
            throw new RuntimeException(ex);
        }
        Individual mention = createIndividual(mentionUrl, ParsemGenericOntology.Mention);
        mention.addProperty(ParsemGenericOntology.hasUrl, bufferModel.tempModel.createTypedLiteral(docUrl));
        mention.addProperty(ParsemGenericOntology.hasStart, bufferModel.tempModel.createTypedLiteral(node.getLeftChar()));
        mention.addProperty(ParsemGenericOntology.hasEnd, bufferModel.tempModel.createTypedLiteral(node.getRightChar()));
        mention.addProperty(ParsemGenericOntology.hasForm, node.getSurface());
        mention.addProperty(ParsemGenericOntology.detectedBy, grmUri.toString());
        entity.addProperty(ParsemGenericOntology.hasMention, mention);
        return mention;
    }

    /**
     * Returns an Individual in the Xip2Jena internal model corresponding to the
     * XipNode node.  If such an Individual already existed,
     * the same one is returned.  If not, the Individual and a corresponding
     * Mention are created.  If ontClass is non-null, the individual is asserted
     * to be of that type, regardless of whether it existed previously or not, and
     * in addition to any previous type assertions.
     * @param node
     * @param ontClass
     * @param docUrl used to construct the Mention if necessary
     * @return
     * @throws com.xerox.xrce.xip2jena.Xip2JenaException
     */
    public Individual xipNode2Individual(XipNode node, OntClass ontClass, URL docUrl) throws Xip2JenaException {
        Individual result = null;
        long offset = node.getRightChar();
        if (offset == 0) {
            result = xipNode2Individual.get(node);
        } else {
            result = offset2Individual.get(offset);
        }
        if (result == null) {
            if (ontClass == null) {
                ontClass = ParsemGenericOntology.Entity;
            }
            result = createIndividual(ontClass);
            xipNode2Individual.put(node, result);
            if (offset != 0) {
                addMention(node, docUrl, result);
                offset2Individual.put(node.getRightChar(), result);
            }
        }
        if (ontClass != null) {
            result.addRDFType(ontClass);
        }
        return result;
    }


    // TODO: I'm not currently using the invididual handler mechanism.  Maybe
    // get rid of it, replace the one call to handleIndiv (in DependencyMapper)
    // with a call to xipNode2Individual.
    public void registerIndivHandler(OntClass ontClass, IndivHandler handler) {
        indivHandlers.put(ontClass, handler);
    // todo: This replaces any previous handler for the class.  Would it be better to
    // allow multiple handlers for the same class?
    }

    public void handleIndiv(XipNode xipNode, OntClass ontClass, URL docUrl)
            throws Xip2JenaException {
        IndivHandler handler;
        Individual indiv = xipNode2Individual(xipNode, ontClass, docUrl);
        handler = indivHandlers.get(ontClass);
        if (handler != null) {
            handler.handleIndiv(indiv, xipNode, ontClass, docUrl);
        }
        for (ExtendedIterator i = ontClass.listSuperClasses(); i.hasNext();) {
            handler = indivHandlers.get((OntClass) i.next());
            if (handler != null) {
                handler.handleIndiv(indiv, xipNode, ontClass, docUrl);
            }
        }

    }

    /**
     * Creates an Individual of type ontClass in the Xip2Jena internal model.
     * Does not create a corresponding Mention.  If the individual you're creating
     * corresponds to a XIP node, you should probably use xipNode2Individual() instead.
     * @param ontClass
     * @throws com.xerox.xrce.xip2jena.Xip2JenaException
     */
    public Individual createIndividual(OntClass ontClass) throws Xip2JenaException {
        return bufferModel.tempModel.createIndividual(ontClass);
    }
    public Individual createIndividual(String url, OntClass ontClass) throws Xip2JenaException {
        return bufferModel.tempModel.createIndividual(url, ontClass);
    }


    public void clear() {
        offset2Individual.clear();
        xipNode2Individual.clear();
    }

    public Literal createTypedLiteral(String lex, RDFDatatype dtype) {
        return bufferModel.tempModel.createTypedLiteral(lex, dtype);
    }
    
    public Literal createTypedLiteral(Object value) {
        return bufferModel.tempModel.createTypedLiteral(value);
    }

    public boolean commit() { return bufferModel.commit(); }

    }
