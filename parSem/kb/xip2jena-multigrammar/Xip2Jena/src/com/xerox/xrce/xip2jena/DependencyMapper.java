package com.xerox.xrce.xip2jena;


import com.hp.hpl.jena.ontology.Individual;
import com.hp.hpl.jena.ontology.OntClass;
import com.hp.hpl.jena.rdf.model.Model;
import com.hp.hpl.jena.rdf.model.Property;
import com.hp.hpl.jena.util.iterator.ExtendedIterator;
import com.xerox.jxip.Const;
import com.xerox.jxip.JXip;
import com.xerox.jxip.XipDependency;
import com.xerox.jxip.XipNode;
import com.xerox.jxip.XipResultManager;
import com.xerox.jxip.XipUnit;
import java.io.File;
import java.net.URL;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import org.apache.log4j.Logger;

public class DependencyMapper extends Xip2Jena {

    private static Logger log = Logger.getLogger(DependencyMapper.class);
    private int handler;
    private Set<String> ignoredDeps = new HashSet<String>();
    private Map<String, OntClass> depName2OntClass;
    private Map<String, Property> depName2Property;
    public OntClass depName2OntClass(String depName) { return depName2OntClass.get(depName); }
    public Property depName2Property(String depName) { return depName2Property.get(depName); }

    public DependencyMapper(URL ontUrl, String grmUri, File grmFile, JXip jxip,
            Model destModel) throws Exception {
        super(ontUrl, grmUri, destModel);
        depName2OntClass = new HashMap<String, OntClass>();
        for (ExtendedIterator i = ontology.listNamedClasses(); i.hasNext();) {
            OntClass ontClass = (OntClass) i.next();
            depName2OntClass.put(ontClass.getLocalName().toUpperCase(), ontClass);
        }

        depName2Property = new HashMap<String, Property>();
        for (ExtendedIterator i = ontology.listOntProperties(); i.hasNext();) {
            Property property = (Property) i.next();
            depName2Property.put(property.getLocalName().toUpperCase(), property);
        }

        handler = jxip.XipGrmFile(grmFile.getCanonicalPath(), 0, true);
        // todo: this isn't really the right place to be setting this flag.
        jxip.XipAddFlagDisplay(handler, Const.UTF8_INPUT, 80);
    }

    private void processXipUnit(XipUnit unit, URL docUrl, Set<String> ignoredDeps)
            throws Xip2JenaException {
        clear();
        log.debug(unit.getSentenceString());

        for (XipDependency dep : unit.getDependencies()) {
            if (dep.getParameters().size() == 1) {
                OntClass ontClass = depName2OntClass(dep.getName());
                if (ontClass == null) {
                    ignoredDeps.add(dep.getName() + "(.)");
                } else {
                    log.debug(dep.getName() + "(" + dep.getArg(1).getSurface() + "#" + dep.getArg(1).getCategory() + ")");
                    XipNode node = dep.getArg(1);
                    handleIndiv(node, ontClass, docUrl);
                }
            } else if (dep.getParameters().size() == 2) {
                Property prop = depName2Property(dep.getName());
                if (prop == null) {
                    ignoredDeps.add(dep.getName() + "(.,.)");
                } else {
                    log.debug(dep.getName() + "(" + dep.getArg(1).getSurface() + "#" + dep.getArg(1).getCategory() + ", " + dep.getArg(2).getSurface() + "#" + dep.getArg(2).getCategory() + ")");
                    Individual indiv1 =
                            xipNode2Individual(dep.getArg(1), ParsemGenericOntology.Entity, docUrl);
                    Individual indiv2 =
                            xipNode2Individual(dep.getArg(2), ParsemGenericOntology.Entity, docUrl);
                    // todo: add provenance information, using docUrl.
                    indiv1.addProperty(prop, indiv2);
                }

            }
        }
        commit();
    }

    public void populate(File infile) throws Xip2JenaException {
        try {
            URL inputUrl = infile.toURI().toURL();
            XipResultManager xiprm = new XipResultManager(); //expensive? could make static.
            int result = xiprm.buildXipResultFromFile(handler, infile.getPath());
            if (result != 0) {
                throw new Exception("buildXipResultFromFile failed");
            }
            for (XipUnit xipUnit : xiprm.getXipResult().getXipUnits()) {
                try {
                    processXipUnit(xipUnit, inputUrl, ignoredDeps);
                } catch (Xip2JenaException e) {
                    log.warn("Sentence failed: " + e.getMessage() + " \"" + xipUnit.getSentenceString() + "\"");
                }
            }
        } catch (Exception ex) {
            throw new Xip2JenaException(ex);
        }
    }

}
