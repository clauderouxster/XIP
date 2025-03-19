/*
 * ProjectContextTest.java
 *
 * Created on February 28, 2007, 9:05 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.xerox.xrce.xipui;


import junit.framework.JUnit4TestAdapter;
import org.junit.*;
import java.net.*;
import java.io.*;
import java.util.*;
import static org.junit.Assert.*;

/**
 *
 * @author vgrassau
 */
public class ProjectContextTest {
    
    private static File setup;
    private ProjectContext context;
    
    /**
     *Init the setup file to read for the context 
     *
     */
    @BeforeClass static public void initTest() throws Exception {
        
        URI uri = Class.forName("com.xerox.xrce.xipui.ProjectContextTest").getResource("/com/xerox/xrce/xipui/test/project-config.xml").toURI();
        setup = new File(uri);
        assertTrue(setup.exists());
    }
    
    /**
     *free memory
     */
    @AfterClass 
    static public void feeTest() {
        
        setup = null;
    }
    
    /**
     *Create an init the projectContext before each test
     */
    @Before 
    public void createProjectContext() {
        context = new ProjectContext(setup);
        boolean res = context.initContext();
        assertTrue(res);
    }
    
    @After 
    public void tearDown() {
        context = null;
    }
    
    /**
     *tests the method getVariable(String)
     *get the collection of all variables and check if the getVariable(String) method return the same 
     *variable. 
     */
    @Test public void testGetVariable() {
        Collection<VarEnv> c = context.getVariables();
        assertFalse(c.isEmpty());
        for (Iterator<VarEnv> it = c.iterator(); it.hasNext();) {
            VarEnv var = it.next();
            VarEnv test = context.getVariable(var.getName());
            assertEquals(var,test);
            assertSame(var,test);
        }
    }
    
    
    /**
     *tests the method addVariable(String,String) 
     *test if the variable had been added  correctly
     *and if an update of the new variable is well done.
     *
     */
    @Test public void testAddVariable() {
        // to valid this test we have to valid the getVariable method 
        testGetVariable();
        String name = "TEST_VARIABLE";
        String value = "testValue";
        VarEnv newVar = new VarEnv(name,value);
        context.addVariable(name,value);
        VarEnv added = context.getVariable(name);
        assertEquals(name,added.getName());
        assertEquals(value,added.getValue());
        //check if after we had added a new var, we can change its value
        String newValue = "testValue2";
        context.addVariable(name,newValue);
        VarEnv changed = context.getVariable(name);
        assertEquals(name,changed.getName());
        assertEquals(newValue,changed.getValue());
        
    }
    
    /**
     *test the method  getFileSetup()
     *check if the file is not null and if it the same that the global property setup
     * of this test class
     */
    @Test public  void testGetFileSetup() {
        File f = context.getSetupFile();
        assertNotNull(f);
        assertSame(f,setup);
    }
    
    /**
     *test the method getVariables() 
     *check if the collection of VarEnv is not null, and 
     *if the collection return by the property method of a context is the same collection.
     */
    @Test public void testGetVariables() {
        Vector v = context.getVariables();
        Object v2 = context.getProperty(context.variables);
        assertNotNull(v);
        assertNotNull(v2);
        assertSame(v,v2);
        
    }
  
    
      /**
     *use for JUnit with a version earlier than JUnit 4.0 
     */
   public static junit.framework.Test suite() {
        return new JUnit4TestAdapter(ProjectContextTest.class);
    }
}
