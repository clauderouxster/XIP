package xip2jena_risk;

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
import com.hp.hpl.jena.rdf.model.Model;
import com.hp.hpl.jena.rdf.model.ModelFactory;
import com.xerox.jxip.JXip;
import com.xerox.xrce.xip2jena.ParsemGenericOntology;
import com.xerox.xrce.xip2jena.RdfDiff;
import com.xerox.xrce.xip2jena.coref.Coref2Jena;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.InputStream;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author nlagos
 */
public class Coref2JenaTest {

    public Coref2JenaTest() {
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
    public void testPopulate() throws Exception {

        Model targetModel = ModelFactory.createDefaultModel();
        Model referenceModel = ModelFactory.createDefaultModel();
        for (Model m : new Model[]{targetModel, referenceModel}) {
            m.setNsPrefix("parsem", ParsemGenericOntology.NS);
            m.setNsPrefix("parsem", ParsemGenericOntology.NS);
        }
        JXip jxip = new JXip();

        Coref2Jena coref2Jena = new Coref2Jena(jxip, targetModel);

        InputStream s = RunTemporalTest.class.getResourceAsStream("Coref2JenaTestElBaradei.n3");
        assertNotNull(s);
        referenceModel.read(s, null, "N3");
        for (int i = 0; i < 20; i++) {
            File f = new File("/home/nlagos/code/parSem/kb/xip2jena-multigrammar/Xip2Jena_tobacco/test/xip2jena_risk/Coref2JenaTest.testPopulate1_1.txt");

            coref2Jena.populate(f);

            boolean same = RdfDiff.compareModels(referenceModel, targetModel).isomorphic();
            if (!same) {
                referenceModel.write(System.out, "N3");
                System.out.println("--------------------------------------------------");
                targetModel.write(System.out, "N3");
                fail();
            }
            targetModel.removeAll();
        }
    }
}
