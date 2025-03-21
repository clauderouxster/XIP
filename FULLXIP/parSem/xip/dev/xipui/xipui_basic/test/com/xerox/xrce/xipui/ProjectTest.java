/*
 * ProjectTest.java
 *
 * Created on March 5, 2007, 11:58 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.xerox.xrce.xipui;



import static org.junit.Assert.*;


import org.junit.Test;
import org.junit.*;
import com.xerox.xrce.xipui.exception.*;
import static com.xerox.xrce.xipui.Grammars.*;
import com.xerox.xrce.xipui.util.FileTools;
import java.util.Vector;

import junit.framework.JUnit4TestAdapter;
import java.io.*;
import java.net.URI;
import java.util.Collection;
import java.util.Iterator;
import org.junit.runner.JUnitCore;
import org.junit.runner.RunWith;

/**
 *
 * @author vgrassau
 */
public class ProjectTest {
   
    private Project project;
    private static Xip xip;
    
    @BeforeClass 
    static public void setXip() throws Exception {
        xip = new Xip();
    }
    
    @AfterClass 
    static public void freeXip()  {
      xip = null;       
    }
    
    @Before 
    public void setProject() throws Exception {
        URI uri = getClass().getResource("/com/xerox/xrce/xipui/test/project-config.xml").toURI();
        File xml  = new File(uri);
        ProjectFactory factory = ProjectFactory.getInstance(xml,xip);
        assertNotNull(factory);
        assertTrue("must be instanciate by XML document", factory.isXMLDocument());
        project = factory.createProject();
        assertNotNull(project);
    }
      
    @After 
    public void freeProject() {
        project = null;
    }
    
    @Test public void testName() throws Exception  {
        String name = "toto";
        project.setName(name);
        assertEquals(name,project.getName());
        
    }
    
    @Test(expected=ProjectException.class)
    public void setNameFailed() throws ProjectException  {
        project.setName(null);
    }
    
    
    @Test public void testAuthor()   {
        String name = "toto";
        project.setAuthor(name);
        assertEquals(name,project.getAuthor());
        
    }
    
    @Test(expected=ProjectException.class)
    public void testDirectoryFailed() throws ProjectException  {
        project.setDirectory(null);
    }
    
    @Test
    public void testDirectory() throws ProjectException {
        File  directory = new File("c:/truc");
        project.setDirectory(directory.getAbsolutePath());
        // the project return an absolute path
         assertEquals(directory.getAbsolutePath(),project.getDirectory());
    }
    
    @Test 
    public void testComment() {
        String comment = " some comment ";
        project.setComment(comment);
        assertEquals(comment,project.getComment());
        
    }
    
    
    @Test
    public void testXip() throws ProjectException {
        project.setXip(xip);
        assertSame(xip,project.getXip());
    }
    
    @Test(expected=ProjectException.class) 
    public void testXipFailed() throws ProjectException {
        project.setXip(null);
        
    }
    
    
    
    
    
    
    
    
    public static junit.framework.Test suite() {
        return new JUnit4TestAdapter(ProjectTest.class);
    }
    
        
    
}
