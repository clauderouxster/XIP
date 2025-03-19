package com.xerox.xrce.xip2jena;

import com.hp.hpl.jena.ontology.Individual;
import com.hp.hpl.jena.ontology.OntClass;
import com.hp.hpl.jena.ontology.OntDocumentManager;
import com.hp.hpl.jena.ontology.OntModel;
import com.hp.hpl.jena.rdf.model.InfModel;
import com.hp.hpl.jena.rdf.model.Model;
import com.hp.hpl.jena.rdf.model.ModelFactory;
import com.hp.hpl.jena.rdf.model.Property;
import com.hp.hpl.jena.rdf.model.Resource;
import com.hp.hpl.jena.rdf.model.ResourceFactory;
import com.hp.hpl.jena.rdf.model.Selector;
import com.hp.hpl.jena.rdf.model.SimpleSelector;
import com.hp.hpl.jena.rdf.model.Statement;
import com.hp.hpl.jena.reasoner.Reasoner;
import com.hp.hpl.jena.reasoner.ValidityReport;
import com.hp.hpl.jena.sdb.SDBFactory;
import com.hp.hpl.jena.sdb.Store;
import com.hp.hpl.jena.util.iterator.ExtendedIterator;
import com.xerox.jxip.Const;
import com.xerox.jxip.JXip;
import com.xerox.jxip.XipDependency;
import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipResult;
import com.xerox.jxip.XipResultManager;
import com.xerox.jxip.XipUnit;
import java.io.File;
import java.net.MalformedURLException;
import java.io.UnsupportedEncodingException;
import java.net.URL;
import java.net.URLEncoder;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;
import org.apache.log4j.Logger;
import org.mindswap.pellet.jena.PelletReasonerFactory;

public class Xip2Jena {

    // todo: use the ontology namespace?
    private String indivBase = "http://xrce.xerox.com/parsem/";
    private static String genericOntUri = "http://www.xrce.xerox.com/parsem/ParsemGenericOntology.owl";

    private JXip jxip;
    private XipResultManager xiprm;
    private int ghandler;
    private int counter = 0;
    private Reasoner reasoner;
    private OntModel ontology;
    private Model textModel; // most methods take this as an argument rather
    // than referencing the field, but the callback processXipResult can't accept
    // arguments, so it uses this instead.
    private OntModel tempModel;
    private InfModel infModel;
    private Store dbStore;
    
    private Map<Long, Individual> offset2Individual;
    private Map<XipNode, Individual> xipNode2Individual;
    
    static Logger log = Logger.getLogger(Xip2Jena.class);
    private Map<String,OntClass> depName2OntClass;
    private Map<String,Property> depName2Property;
    private Map<String,Individual> string2Individual;
    private Set<String> ignoredDeps;
   
    private Map<OntClass,IndivHandler> indivHandlers;
    private URL inputUrl;
    private Model data;
    
    private Statement stat;
    private URL newURL;
       
    private Map <Long, Long> rectorEventAndCoreEvent = new HashMap <Long, Long>();
    private Map <Long, Long> coreEventAndRectorEvent = new HashMap <Long, Long>();
    private Map <Statement, String> namedGraphsAndStatements = new HashMap <Statement, String>();
    private Map <Long, URL> namedGraphsAndDependencies = new HashMap <Long, URL>();
   
    public void init(String grmFileName, URL ontFileUrl) throws Exception {
         
       
        jxip = new JXip();
        xiprm = new XipResultManager();
        ghandler = jxip.XipGrmFile(grmFileName, 0, true);
        jxip.XipAddFlagDisplay(ghandler, Const.UTF8_INPUT, 80);
        ontology = ModelFactory.createOntologyModel(PelletReasonerFactory.THE_SPEC);

        // Before loading the ontology, set up a mapping so that the
        // ParsemGenericOntology can be read locally rather than from its URL.
        OntDocumentManager dm = ontology.getDocumentManager();
        dm.getFileManager().addLocatorClassLoader(Xip2Jena.class.getClassLoader());
        String path = Xip2Jena.class.getPackage().getName().replaceAll("\\.", "/");
        dm.addAltEntry(genericOntUri, path + "/ParsemGenericOntology.owl" );
        // Would rather find the ontology as follows, but it doesn't work when
        // running from a jar because the Jena FileManager doesn't understand
        // jar URLs.
        //   URL genericOntLocal = Xip2Jena.class.getResource("ParsemGenericOntology.owl");
        //   dm.addAltEntry(genericOntUri, genericOntLocal );
       
        ontology.read(ontFileUrl.toString());
        reasoner = PelletReasonerFactory.theInstance().create();
        // I originally allocated a new tempModel and infModel on the stack in
        // processXipUnit(), but there was a memory leak somewhere that caused
        // the heap to be exhausted rather quickly.  So now I'm reusing a single
        // infoModel, calling removeAll() on it each time.
        tempModel = ModelFactory.createOntologyModel();
        infModel = ModelFactory.createInfModel(reasoner, ontology, tempModel);
        //depTrans = new DependencyTranslation(transFileName);
        offset2Individual = new HashMap<Long, Individual>();
        xipNode2Individual = new HashMap<XipNode, Individual>();
       
        depName2OntClass = new HashMap<String,OntClass>();
        for (ExtendedIterator i = ontology.listNamedClasses(); i.hasNext();) {
            OntClass ontClass = (OntClass) i.next();
            depName2OntClass.put(ontClass.getLocalName().toUpperCase(), ontClass);
        }
       
        depName2Property = new HashMap<String,Property>();
        for (ExtendedIterator i = ontology.listOntProperties(); i.hasNext();) {
            Property property = (Property) i.next();
            depName2Property.put(property.getLocalName().toUpperCase(), property);
        }
       
        string2Individual = new HashMap<String,Individual>();
        for (ExtendedIterator i = ontology.listIndividuals(); i.hasNext();) {
            Individual individual = (Individual) i.next();
            string2Individual.put(individual.getLocalName().toUpperCase(), individual);
        }
       
        ignoredDeps = new HashSet<String>();
        indivHandlers = new HashMap<OntClass,IndivHandler>();
        indivHandlers.put(ParsemGenericOntology.Entity, new GenericEntityHandler());
       
    }

    public void processFile(File file, Model model, Store store) throws Exception {
        textModel = model;
        inputUrl = file.toURI().toURL();
        dbStore = store;
        xiprm.xipResultBuilderFromTextFile(ghandler, file.getPath(), this, "processXipResult");
    }

    public void processString(String text, Model model, Store store) throws Exception {
        textModel = model;
        dbStore = store;
        int res = xiprm.xipResultBuilderFromString(ghandler, text, this, "processXipResult");       
        if (res != 0) {
            throw new Exception("buildXipResult returned " + res);
            // I'd like to throw something more specific than Exception, but
            // that's what the XIP API throws no matter what I do in my own code,
            // so there's not much point.
        }
    }

    /*
     * This is only public so that the screwy XIP API callback mechanism can call
     * it.  Clients of this class shouldn't use it, they should use processFile()
     * or processString().
     */
    public void processXipResult() {
        XipResult xipres = xiprm.getXipResult();
        for (XipUnit unit : xipres.getXipUnits()) {
           
            //System.out.println("The sentence is "+unit.getSentenceString());
            try {
                processXipUnit(unit, textModel, inputUrl);
            } catch (Xip2JenaException e) {
                log.warn("Sentence failed: "  + e.getMessage() + "\"" + unit.getSentenceString() + "\"");
            }
        }
    }

    private void processXipUnit(XipUnit unit, Model model, URL docUrl) throws Xip2JenaException {
//        if (++count % 100 == 0) {
//            log.info(count);
//        }
        offset2Individual.clear();
        xipNode2Individual.clear();
        log.debug(unit.getSentenceString());
        infModel.removeAll();
       
        newURL = docUrl;
        rectorEventAndCoreEvent.clear();
        coreEventAndRectorEvent.clear();
        namedGraphsAndStatements.clear();
        namedGraphsAndDependencies.clear();
        
        // process first all unary dependencies, then binary dependencies.
        // This ensures that all entities exist before they are placed in
        // relationships.
        for (XipDependency dep : unit.getDependencies()) {
            if (dep.getParameters().size() == 1) {
                OntClass ontClass = depName2OntClass.get(dep.getName());
                //System.out.println("The entity is "+dep.getName());
                if (ontClass == null) {
                    ignoredDeps.add(dep.getName() + "(.)");
                } else {
                    XipNode node = dep.getArg(1);
                    handleIndiv(node, ontClass, tempModel, docUrl);
                }
            }
        }
        /*
         * Make sure all links between dependencies and named graphs exist before using them further
        */
        for (XipDependency depend : unit.getDependencies()){
            if (depend.getParameters().size() == 2) {

                if (depend.hasFeature("COMPLTHAT", "+")){
                    coreEventAndRectorEvent.put(depend.getArg(1).getRightChar(), depend.getArg(2).getRightChar());
                }                
                
                if (coreEventAndRectorEvent.containsValue(depend.getArg(1).getRightChar()) && depend.getName().equals("HASPARTICIPANT")){
                    try {
                        newURL = new URL (docUrl.toString()+"/"+depend.getArg(2).getSurface()+"/"+depend.getArg(2).getRightChar());                              
                    } catch (MalformedURLException e) {
                        log.error("The URL is wrong "+e);
                    }
                        namedGraphsAndDependencies.put(depend.getArg(1).getRightChar(), newURL);
                }
            }
        }
        for (XipDependency dep : unit.getDependencies()) {
            if (dep.getParameters().size() == 2) {
                Property prop = depName2Property.get(dep.getName());
                
                /*
                 * How to define the dependency? Hard-coded as it is now?
                 * Storing a map of related embedded and core events.
                 */
                
                if (prop == null) {
                    ignoredDeps.add(dep.getName() + "(.,.)");
                } else {
                   
                    Individual indiv1 = xipNode2Individual(dep.getArg(1), tempModel,
                                                           null);
                   
                    Individual indiv2 = xipNode2Individual(dep.getArg(2), tempModel,
                                                           null);
                 
                   if (coreEventAndRectorEvent.containsValue(dep.getArg(1).getRightChar())){
                       if (dep.getName().equals("HASPARTICIPANT")){
                           try {
                               newURL = new URL (docUrl.toString()+"/"+dep.getArg(2).getSurface()+"/"+dep.getArg(2).getRightChar());
                           } catch (MalformedURLException e) {
                               log.error("The URL is wrong "+e);
                           }
                           stat = ResourceFactory.createStatement(indiv2, depName2Property.get("STATE"), 
                                   ResourceFactory.createResource(newURL.toString()));
                       }
                      
                       if (dep.getName().equals("HASTIME") && namedGraphsAndDependencies.containsKey(dep.getArg(1).getRightChar())){
                           stat = ResourceFactory.createStatement(indiv2, depName2Property.get("NAMEDGRAPHHASTIME"), 
                                   ResourceFactory.createResource(namedGraphsAndDependencies.get(dep.getArg(1).getRightChar()).toString()));
                       }
                   } else {
                        stat = ResourceFactory.createStatement(indiv1, prop, indiv2);
                   }
                                      
                   
                   if (namedGraphsAndDependencies.containsKey(coreEventAndRectorEvent.get(dep.getArg(1).getRightChar()))){
                        namedGraphsAndStatements.put(stat,namedGraphsAndDependencies.get(coreEventAndRectorEvent.get(dep.getArg(1).getRightChar())).toString());
                   } else {
                        namedGraphsAndStatements.put(stat, docUrl.toString());
                   }
                       
                }
            }       
        }
        
        ValidityReport validity = infModel.validate();
        if (validity.isValid()) {
            for (Iterator i = namedGraphsAndStatements.keySet().iterator(); i.hasNext();){
                stat = (Statement)i.next(); 
                data = SDBFactory.connectNamedModel(dbStore, namedGraphsAndStatements.get(stat));
                data.add(stat); 
                data.close();
            }
            model.add(tempModel.listStatements(nonRdfSelector));  
        } else {
            if (log.isDebugEnabled()) {
                for (Iterator i = tempModel.listStatements(nonRdfSelector); i.hasNext();) {
                    log.debug(i.next());
                }
            }
            throw new Xip2JenaException("Not consistent with ontology");
        }   
    }

    private void addMention(OntModel model, Individual indiv, XipNode node, URL docUrl) throws Xip2JenaException {
        Individual mention = createIndividual(model, ParsemGenericOntology.Mention, node.getSurface());
        mention.addProperty(ParsemGenericOntology.hasUrl, model.createTypedLiteral(docUrl));
        mention.addProperty(ParsemGenericOntology.hasStart, model.createTypedLiteral(node.getLeftChar()));
        mention.addProperty(ParsemGenericOntology.hasEnd, model.createTypedLiteral(node.getRightChar()));
        mention.addProperty(ParsemGenericOntology.hasForm, node.getSurface());
        indiv.addProperty(ParsemGenericOntology.hasMention, mention);
    }

    // todo: was previously considering that there was one entity per XipNode, but
    // that failed on NP{NOUN{Emma Udwin}}: the PERSON dependency is on the NOUN, but the
    // EVENT dependency is on the NP, so I generated two entities instead of one.
    // Current workaround is to use the end offset as the identity criterion,
    // unless it's 0 (noeud fictif) in which case use XIP node identity.
    // Should this ultimately be handled by the coreference mechanism?   
    private Individual xipNode2Individual(XipNode node, OntModel model, OntClass ontClass) throws Xip2JenaException {
        Individual result;
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
            result = createIndividual(model, ontClass, node.getSurface());
            xipNode2Individual.put(node, result);
            if (offset != 0) {
                offset2Individual.put(node.getRightChar(), result);
            }
        }
        if (ontClass != null) {
            result.addRDFType(ontClass);
        }
        return result;
    }
    
    
    /* Just like OntModel.createIndividual(OntClass) except that it gives a
     * meaningful name.
     */
    private Individual createIndividual(OntModel ontModel, OntClass ontClass, String mnemonic)
    throws Xip2JenaException {
        try {
            return ontModel.createIndividual(indivBase + ontClass.getLocalName() + 
                    "-" + counter++ + "-" + URLEncoder.encode(mnemonic, "UTF-8"), ontClass);
        } catch (UnsupportedEncodingException ex) {
            throw new Xip2JenaException(ex);
        }
    }
   
    // I don't understand why, but the inference model is getting filled up with
    // useless statements about rdfs meta-resources.  This filters them out.
    private static Selector nonRdfSelector = new SimpleSelector() {
        @Override
        public boolean selects(Statement s) {
            String sns = s.getSubject().getNameSpace();
            String ons = null;
            if (s.getObject().isURIResource()) {
                ons = ((Resource) s.getObject().as(Resource.class)).getNameSpace();
            }
            return !sns.equals("http://www.w3.org/2000/01/rdf-schema#") &&
                   !sns.equals("http://www.w3.org/1999/02/22-rdf-syntax-ns#") &&
                   (ons == null ||
                       (!sns.equals("http://www.w3.org/2000/01/rdf-schema#") &&
                        !sns.equals("http://www.w3.org/1999/02/22-rdf-syntax-ns#") ));
        }
    };
   
    private void handleIndiv(XipNode xipNode, OntClass ontClass, OntModel ontModel, URL docUrl) throws Xip2JenaException {
        IndivHandler handler;
        Individual indiv = xipNode2Individual(xipNode, ontModel, ontClass);
        handler = indivHandlers.get(ontClass);
        if (handler != null) {
            handler.handleIndiv(indiv, xipNode, ontClass, ontModel, docUrl);
        }
        for (ExtendedIterator i = ontClass.listSuperClasses(); i.hasNext();) {
            handler = indivHandlers.get((OntClass) i.next());
            if (handler != null) {
                handler.handleIndiv(indiv, xipNode, ontClass, ontModel, docUrl);
            }
        }
       
    }

    private class GenericEntityHandler implements IndivHandler {
        public void handleIndiv(Individual indiv, XipNode xipNode,
                                OntClass ontClass, OntModel ontModel,
                                URL docUrl) throws Xip2JenaException {
            addMention(tempModel, indiv, xipNode, docUrl);
        }
    }
   
    public void registerIndivHandler(OntClass ontClass, IndivHandler handler) {
        indivHandlers.put(ontClass, handler);
        // todo: This replaces any previous handler for the class.  Would it be better to
        // allow multiple handlers for the same class?
    }
   
    public Set<String> getIgnoredDeps() { return ignoredDeps; }
}
