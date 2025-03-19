/*
 * Copyright Xerox 2009
 */

package com.xerox.xrce.xip2jena;

import com.hp.hpl.jena.ontology.OntModel;
import com.hp.hpl.jena.rdf.model.Model;
import com.hp.hpl.jena.rdf.model.ModelFactory;
import com.xerox.jxip.JXip;
import com.xerox.xrce.xip2jena.coref.Coref2Jena;
import com.xerox.xrce.xip2jena.corpus.Corpus;
import com.xerox.xrce.xip2jena.corpus.Document;
import com.xerox.xrce.xip2jena.storageconfigs.DataStoreTdb;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.InputStream;
import java.net.URL;
import java.util.Properties;
import org.apache.log4j.Level;
import org.apache.log4j.PropertyConfigurator;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author akaplan
 */
public class PersonsEntityMergerTest {

    public PersonsEntityMergerTest() {
    }

    @BeforeClass
    public static void setUpClass() throws Exception {
    }

    @AfterClass
    public static void tearDownClass() throws Exception {
    }

    @Before
    public void setUp() {
    }

    @After
    public void tearDown() {
    }
    
    @Test
    public void testMerge1() throws Exception {
        RdfDiff.log.setLevel(Level.DEBUG);
        Model targetModel = ModelFactory.createDefaultModel();
        Model referenceModel = ModelFactory.createDefaultModel();
        Model inputModel = ModelFactory.createDefaultModel();
        OntModel ontologyModel = ModelFactory.createOntologyModel();
        for (Model m : new Model[]{targetModel, referenceModel}) {
            m.setNsPrefix("parsem", ParsemGenericOntology.NS);
            m.setNsPrefix("parsem", ParsemGenericOntology.NS);
        }
        InputStream s = PersonsEntityMerger.class.getResourceAsStream("ParsemGenericOntology.owl");
        assertNotNull(s);
        ontologyModel.read(s, null);
        PersonsEntityMerger merger = new PersonsEntityMerger(targetModel, ontologyModel);

        for (int i = 0; i < 20; i++) {
            System.out.println(i);

            s = PersonsEntityMergerTest.class.getResourceAsStream("PersonsEntityMergerTest.testMerge4.n3");
            assertNotNull(s);
            inputModel.read(s, null, "N3");
            merger.merge(inputModel);

            if (i == 0) {
                referenceModel.add(targetModel);
            } else {
                boolean same = RdfDiff.compareModels(referenceModel, targetModel).isomorphic();
                if (!same) {
                    referenceModel.write(System.out, "N3");
                    System.out.println("--------------------------------------------------");
                    targetModel.write(System.out, "N3");
                    fail();
                }
            }
            targetModel.removeAll();
            inputModel.removeAll();
        }
        
    }

    @Test
    public void testMerge2() throws Exception {
        RdfDiff.log.setLevel(Level.DEBUG);
        Model targetModel = ModelFactory.createDefaultModel();
        Model referenceModel = ModelFactory.createDefaultModel();
        Model srcModel = ModelFactory.createDefaultModel();
        OntModel ontologyModel = ModelFactory.createOntologyModel();
        for (Model m : new Model[]{targetModel, referenceModel}) {
            m.setNsPrefix("parsem", ParsemGenericOntology.NS);
            m.setNsPrefix("parsem", ParsemGenericOntology.NS);
        }
        InputStream s = PersonsEntityMerger.class.getResourceAsStream("ParsemGenericOntology.owl");
        assertNotNull(s);
        ontologyModel.read(s, null);
        PersonsEntityMerger merger = new PersonsEntityMerger(targetModel, ontologyModel);

        for (int i = 0; i < 20; i++) {
            System.out.println(i);

            s = PersonsEntityMergerTest.class.getResourceAsStream("PersonsEntityMergerTest.testMerge2.n3");
            assertNotNull(s);
            srcModel.read(s, null, "N3");
            merger.merge(srcModel);
            boolean same = RdfDiff.compareModels(srcModel, targetModel).isomorphic();
            if (!same) {
                srcModel.write(System.out, "N3");
                System.out.println("--------------------------------------------------");
                targetModel.write(System.out, "N3");

                fail();
            }
            targetModel.removeAll();
            srcModel.removeAll();
        }

    }

}
