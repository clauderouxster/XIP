package com.xerox.xrce.xip2jena.coref;

import com.hp.hpl.jena.ontology.Individual;
import com.hp.hpl.jena.ontology.OntClass;
import com.hp.hpl.jena.rdf.model.Model;
import com.hp.hpl.jena.rdf.model.Property;
import com.xerox.jxip.Const;
import com.xerox.xrce.xip2jena.Xip2Jena;
import com.xerox.xrce.xip2jena.Xip2JenaException;
import java.io.File;
import com.xerox.jxip.JXip;
import com.xerox.xrce.xip2jena.ParsemGenericOntology;
import com.xerox.xrce.xip2jena.Util;
import java.net.URL;
import java.util.HashMap;
import java.util.Map;
import jcoref.CorefResolver;
import jcoref.ReferentList;
import jcoref.Referent;
import jcoref.Referent.Properties;
import org.apache.log4j.Logger;

public class Coref2Jena {

    static final String corefGrmUri = Xip2Jena.grammarBase + "coref";
    static final String grmRelResourceName = "grammar/coref_kr.grm";

    CorefResolver corefResolver;

    static Logger log = Logger.getLogger(Coref2Jena.class);
    private Xip2Jena xip2jena;

    public Coref2Jena(JXip jxip, Model destModel) throws Exception {

        URL ontUrl = ParsemGenericOntology.class.getResource("ParsemGenericOntology.owl");
        if (ontUrl == null) {
            throw new Exception("Couldn't find ParsemGenericOntology.owl");
        }

        xip2jena = new Xip2Jena(ontUrl, corefGrmUri, destModel);

        File grmFile = Util.findGrammarFile(Coref2Jena.class, grmRelResourceName);
        int handler = jxip.XipGrmFile(grmFile.getCanonicalPath(), 0, true);
        jxip.XipAddFlagDisplay(handler, Const.UTF8_INPUT, 80);
        corefResolver = new CorefResolver(jxip, handler);
        corefResolver.setTrace(false);
    }

    public void populate (File infile) throws Xip2JenaException {
        try {
            xip2jena.clear();
            URL inputUrl = infile.toURI().toURL();
            ReferentList referents = corefResolver.getReferentListFromFile(infile.getPath(), true, false);
            Map<Referent,Individual> entityMap = new HashMap<Referent, Individual>();
            Map<Individual,Individual> personNamesMap = new HashMap <Individual, Individual>();

            for (Referent referent : referents) {
                Referent antecedent = referent.getAntecedent();
                Individual entity;
                entity = getEntity(entityMap, antecedent != null ? antecedent : referent);
                if (!referent.getNode().isFicticious()) {
                    xip2jena.addMention(referent.getNode(), inputUrl, entity);
                }
                if (referent.getSemanticType().equals(Referent.SemanticType.PERSON)) {
                    entity.addRDFType(ParsemGenericOntology.Person);
                    if (referent.getNode().hasDependency("PERSON")) {
                        addPersonName(entity, referent, personNamesMap);
                    }
                }
                if (referent.getSemanticType().equals(Referent.SemanticType.ORG)) {
                    entity.addRDFType(ParsemGenericOntology.Organisation);
                    if (antecedent == null && referent.getNode().hasDependency("ORGANISATION")) {
                        addName(entity, referent);
                    }
                }
                xip2jena.commit();
            }
        } catch (Exception ex) {
            throw new Xip2JenaException("Error in " + infile.getPath(), ex);
        }
    }

    private void addPersonName(Individual indiv, Referent referent,
            Map<Individual, Individual> personNamesMap) throws Xip2JenaException {
        Individual personName = personNamesMap.get(indiv);
        if (personName == null) {
            personName = createPersonName(personNamesMap, referent, indiv);
        }
        personName.addProperty(ParsemGenericOntology.hasNameForm, referent.getNode().getSurface());
    }

    private Individual createPersonName(Map<Individual, Individual> personNamesMap,
            Referent referent, Individual indiv) throws Xip2JenaException {

        //Create individual for the name of the person
        Individual personName = xip2jena.createIndividual(ParsemGenericOntology.PersonName);

        //Link the person to his name
        indiv.addProperty(ParsemGenericOntology.hasPersonName, personName);

        //Link the person to his gender
        if (getPersonAttributeValue(referent, Properties.hasGender).equals("F")) {
            indiv.addProperty(ParsemGenericOntology.hasGender, xip2jena.createIndividual(ParsemGenericOntology.Female));
        }
        if (getPersonAttributeValue(referent, Properties.hasGender).equals("M")) {
            indiv.addProperty(ParsemGenericOntology.hasGender, xip2jena.createIndividual(ParsemGenericOntology.Male));
        }

        //Create the structure describing a person's name
        personName.addProperty(ParsemGenericOntology.hasNameForm, referent.getNode().getSurface());

        personNamesMap.put(indiv, personName);

        createPersonNameProperty(ParsemGenericOntology.FirstName, personName, ParsemGenericOntology.hasFirstName,
                referent, Properties.hasFirstname);

        createPersonNameProperty(ParsemGenericOntology.LastName, personName, ParsemGenericOntology.hasLastName,
                referent, Properties.hasLastname);

        createPersonNameProperty(ParsemGenericOntology.MiddleName, personName, ParsemGenericOntology.hasMiddleName,
                referent, Properties.hasMiddlename);

        //createPersonNameProperty(ParsemGenericOntology.Profession, personName, ParsemGenericOntology.hasProfession,
                //referent, Properties.hasPTitle);
        
        return personName;
        /*personName.addProperty(ParsemGenericOntology.hasCanonicalForm,
        getPersonAttributeValue(antecedent, Properties.hasFirstname) + " "+
        getPersonAttributeValue(antecedent, Properties.hasLastname) + " "+
        getPersonAttributeValue(antecedent,Properties.hasMiddlename) + " "+
        getPersonAttributeValue(antecedent, Properties.hasPTitle));*/

    }

     private void addName(Individual indiv, Referent referent) throws Xip2JenaException {
            Individual name;
            name = xip2jena.createIndividual(ParsemGenericOntology.Name);
            indiv.addProperty(ParsemGenericOntology.hasName, name);
            name.addProperty(ParsemGenericOntology.hasNameForm, referent.getNode().getSurface());
         }

     private void createPersonNameProperty (OntClass ontClass, Individual indiv, Property ontProp, Referent referent,
             Referent.Properties property) throws Xip2JenaException{
         Individual createdIndiv;
         if (referent.getStringProperty(property)!=null){
            createdIndiv = xip2jena.createIndividual(ontClass);
            indiv.addProperty(ontProp, createdIndiv);
            createdIndiv.addProperty(ParsemGenericOntology.hasPersonNameAtomForm, getPersonAttributeValue(referent,property));
         }
     }

     private String getPersonAttributeValue (Referent referent, Referent.Properties property){
         if (referent.getStringProperty(property)!=null){
             return referent.getStringProperty(property);
         } else {
             return "";
         }
     }
     private Individual getEntity(Map<Referent, Individual> entityMap,
            Referent referent) throws Xip2JenaException {
        Individual entity = entityMap.get(referent);
        if (entity == null) {
                entity = xip2jena.createIndividual(ParsemGenericOntology.Entity);
            entityMap.put(referent, entity);
        }
        return entity;
    }

}
