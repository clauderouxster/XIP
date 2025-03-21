/*
 * ProjectFactoryTest.java
 *
 * Created on February 28, 2007, 5:11 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.xerox.xrce.xipui;



import com.xerox.xrce.xipui.util.FileTools;
import java.util.*;
import junit.framework.JUnit4TestAdapter;
import org.junit.*;
import java.io.*;
import java.net.*;
import static org.junit.Assert.*;
/**
 *
 * @author vgrassau
 */
public class ProjectFactoryTest {
    
    private Xip xip;
    static private  String root_xip = "h:/parSem/xip/dev/GRAMMARS";
    
    @BeforeClass
    /** init test in order to have a grm file */
    static public void initTest() {
        String os =System.getProperty("os.name") ;
        if ( !os.startsWith("Windows")) {
            root_xip = "/home/vgrassau/parSem/xip/dev/GRAMMARS";
        }
    }
    
    @AfterClass
    static public void freeTest() {
        root_xip = null;
    }
    /**
     *Init the Xip parser
     */
    @Before
    public void init() throws Exception {
        xip = new Xip();
    }
    
    /**
     *free the parser
     */
    @After
    public void free() {
        xip = null;
    }
    
    /**
     *Test if the Factory construc a New project
     *creates necessary directoires , check if the properties are the same before creation and after.
     *at the end, the new project will be deleted
     *test also if the project can be loaded correctly
     *NB : this test is too dependent from the grm file 
     */
    //@Ignore
    @Test
    public void testCreateProjectFromMap() throws Exception  {
        String name  = "projectTest";
        String folder = "tmpProject";
        String author = "grassaud";
        String version = "1.0";
        String comment = "comment";
        Grammar grammar = null;
        Grm grm = null;
        ProjectContext context = new ProjectContext(folder,name);
        Hashtable<String,Object> map = new Hashtable();
        
        try {
            grm = new Grm(xip,context);
            grm.setGrmFile(root_xip + "/" + Grammars.english);
            //grm.read();
            grammar = new Grammar(xip);
            grammar.loadGrammar(grm.getAbsolutePath());
            map.put(ProjectFactory.GRM,grm);
            map.put(ProjectFactory.GRAMMAR,grammar);
        } catch (Exception e) {
            fail("can't create objects to execute the test");
        }
        
        map.put(ProjectFactory.AUTHOR,author);
        map.put(ProjectFactory.VERSION,version);
        map.put(ProjectFactory.NAME,name);
        map.put(ProjectFactory.FOLDER,folder);
        map.put(ProjectFactory.COMMENT,comment);
        map.put(ProjectFactory.project_context,context);
        
        ProjectFactory factory = ProjectFactory.getInstance(map,xip);
        
        assertFalse(factory.isXMLDocument());
        Project p = factory.createProject();
        testProject(p);
        File f = new File(folder);
        assertEquals(f.getAbsolutePath(),p.getDirectory());
        assertEquals(name,p.getName());
        assertEquals(author,p.getAuthor());
        assertEquals(comment,p.getComment());
        assertEquals(version,p.getVersion());
        assertSame(grm,p.getGrmFile());
        assertSame(grammar,p.getGrammar());
        assertSame(context,p.getProjectContext());
        //these instructions belows can be dangerous, because we delete directories and sub directories
        //so if the test passed until here, f is the same directory than the project directory.
        // we can delete the f directory
        try {
            FileTools.deleteFiles(f);
        } catch (Exception e) {
            e.printStackTrace();
        }
        
    }
        
    /**
     *Test the project 
     *test if p is not null and if it can be loaded
     */
    private void testProject(Project p ) throws Exception  {
        
        assertNotNull(p);
        boolean success = false;
        try {
            p.load();
            success = true;
        } catch (Exception e) {
            success = false;
        }
        assertTrue(success);
        
        
    }
    
    /**
     *Test if a project can be created from an XML document describing a Project
     *test also if the project can be loaded
     *This test is too dependent from the xml document and the grm file.
     */
    @Test
    public void testCreateProjectFromXml() throws Exception {
        URI uri = getClass().getResource("/com/xerox/xrce/xipui/test/project-config.xml").toURI();
        File xml  = new File(uri);
        ProjectFactory factory = ProjectFactory.getInstance(xml,xip);
        assertNotNull(factory);
        assertTrue("must be instanciate by XML document", factory.isXMLDocument());
        testProject(factory.createProject());
    }
    
    /**
     *This test check if an exception is well thrown if the xml document doesn't exist
     */
    @Test(expected=Exception.class)
    public void testCreateProjectFailed() throws Exception {
        ProjectFactory factory = ProjectFactory.getInstance(new File("_"),xip);
        assertNotNull(factory);
        assertNotNull(factory);
        assertTrue("must be instanciate by XML document", factory.isXMLDocument());
        Project p = factory.createProject();
        assertNull(p);
    }
    
    /**
     *use for JUnit with a version earlier than JUnit 4.0
     */
    public static junit.framework.Test suite() {
        return new JUnit4TestAdapter(ProjectFactoryTest.class);
    }
    
}
