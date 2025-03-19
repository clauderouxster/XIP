/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package wsTest1;

import eu.project.Cacao.Components.Utils.DocsSearchResult;
import eu.project.Cacao.Components.Utils.FacetsSearchResult;
import java.util.Vector;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author ctrojahn
 */
public class Ws_client1Test {

    public Ws_client1Test() {
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

    /**
     * Test of getFacets1 method, of class Ws_client1.
     */
    @Test
    public void testGetFacets1() {
        System.out.println("getFacets1");
        Ws_client1 instance = new Ws_client1();
        Vector<FacetsSearchResult> expResult = null;
        Vector<FacetsSearchResult> result = instance.getFacets1();
        assertEquals(expResult, result);
        // TODO review the generated test code and remove the default call to fail.
        fail("The test case is a prototype.");
    }

    /**
     * Test of getRecords1 method, of class Ws_client1.
     */
    @Test
    public void testGetRecords1() {
        System.out.println("getRecords1");
        Ws_client1 instance = new Ws_client1();
        Vector<DocsSearchResult> expResult = null;
        Vector<DocsSearchResult> result = instance.getRecords1();
        assertEquals(expResult, result);
        // TODO review the generated test code and remove the default call to fail.
        fail("The test case is a prototype.");
    }
}