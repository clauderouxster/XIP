/*
 * CorpusElementTest.java
 *
 * Created on March 6, 2007, 2:27 PM
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
public class CorpusElementTest {
   
   static private File file;
   static private File directory;
    
   @BeforeClass 
   static public void initFiles() throws Exception {
       file = File.createTempFile("toto","txt");
       file.deleteOnExit();
       directory = new File("//smbhost/parsem/corpora/english/reutersexpanded");
       
       //directory.mkdir();
       //directory.deleteOnExit();
   }
    
    
   @AfterClass 
   static public void freeFiles() {
        file.delete();
        file = null;
       // directory.delete();
        directory = null;
   }
    
    @Test 
    public void testGetFiles() {
        CorpusElement ce1 = new CorpusElement(file);
        CorpusElement ce2 = new CorpusElement(directory);
        assertFalse(ce1.isDirectory());
        assertTrue(ce2.isDirectory());
        long start = System.currentTimeMillis();
        long size = ce2.getCountFiles();
        long end  = System.currentTimeMillis();
        long time = end - start;
        System.out.println(size  + " " + time + " ms");
        start = System.currentTimeMillis();
        long size2 = ce2.getCountFiles();
        end  = System.currentTimeMillis();
        long time2 = end - start;
        System.out.println(size  + " " + time2 + " ms");
        assertEquals(size,size2);
        assertTrue(time2 < time);
//        Collection<File> c = ce1.getFiles();
//        assertFalse(c.isEmpty());
//        Vector v = new Vector(c);
//        assertSame(file,v.firstElement());
//        Collection<File> c2 = ce2.getFiles();
//        assertFalse(c2.isEmpty());
        
    }
    

    public static junit.framework.Test suite() {
        return new JUnit4TestAdapter(CorpusElementTest.class);
    }

}
