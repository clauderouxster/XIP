/*
 * XipTest.java
 *
 * Created on February 26, 2007, 5:14 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.xerox.xrce.xipui;

import static org.junit.Assert.*;


import org.junit.Test;
import org.junit.*;
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
 
public class XipTest {
    
    static private Xip xip;
    
    static private  String root_xip = "h:/parSem/xip/dev/GRAMMARS";
    
    private int globalGhandler = -1;
    
    private File input;
    private File inputXML;
    private File output;
    private File outputXML;
    private String text;
    private String textXML;
    
    /**
     *Create an instance of JXIP and JXIP2
     */
    @BeforeClass
    static public void initTest() throws Exception  {
        String os =System.getProperty("os.name") ;
        if ( !os.startsWith("Windows")) {
            root_xip = "/home/vgrassau/parSem/xip/dev/GRAMMARS";
        }
           
        xip = new Xip();
           
        System.out.println("XIP version : " + xip.getVersion());
        
    }
    
    
    @AfterClass
    static public void free()  {
        
        xip = null;
        
    }
    
    
    @Ignore
    @Test public  void setEnvRootXip() {
        
        boolean success = false;
        try {
            xip.setEnv("ROOT_XIP",root_xip);
            success = true;
        } catch (Exception e) {
            e.printStackTrace();
            success = false;
        }
        assertTrue("setenv failed",success);
    }
    
    
    @Ignore
    @Test public void loadAllGrammars() throws Exception  {
        
        loadGrammar(english);
        loadGrammar(englishEntity);
        loadGrammar(coreference);
        loadGrammar(englishNormEntity);
        loadGrammar(englishNorm);
        //loadGrammar(french);
        loadGrammar(frenchPE);
        
        loadGrammar(englishEntityOnly);
        //loadGrammar(frenchPE);
        loadGrammar(spanish);
        loadGrammar(italian);
        loadGrammar(portuguese);
        loadGrammar(german);
        
        
        
    }
    
    
    /**
     * Load a grammar
     * @param grammar the grammar to load
     */
    protected int loadGrammar(String grammar) throws Exception {
        
        int gHandler = -1;
        gHandler = xip.loadGrammar(root_xip + "/" + grammar, 0, true);
        assertTrue("Grammar not loaded",gHandler != -1);
        return gHandler ;
    }
    
    
    /**
     *Load a grammar and add some flag display
     *then you can use the globalGhandler property
     */
    @Before public void setGlobalGhandler() throws Exception  {
        String gram = french;
        System.out.println("Load grammar  : " + gram + " for test ");
        this.globalGhandler = loadGrammar(gram);
        xip.addFlag(globalGhandler,xip.DISPLAY_LEMMA);
        xip.addFlag(globalGhandler,xip.DISPLAY_REDUCED);
        xip.addFlag(globalGhandler,xip.DISPLAY_CHUNK_TREE);
        System.out.println("grammar  : " + gram + " loaded");
    }
    
    @After public void unsetGlobalGhandler() throws Exception  {
        
        this.globalGhandler = -1;
    }
    
    
    
    
    @Before public void setCorpusFile() throws Exception {
        URI uri = getClass().getResource("/com/xerox/xrce/xipui/test/corpus.txt").toURI();
        URI uriXML = getClass().getResource("/com/xerox/xrce/xipui/test/corpus.xml").toURI();
        input = new File(uri);
        inputXML = new File(uriXML);
        output = File.createTempFile(input.getName(), "res");
        outputXML = File.createTempFile(inputXML.getName(), "res");
        
        text = FileTools.read(input,"ISO-8859-1");
        textXML = FileTools.read(inputXML,"UTF-8");
    }
    
    
    @After public void unsetCorpusFile() {
        input = null;
        inputXML = null;
        output.delete();
        outputXML.delete();
        text = null;
        textXML = null;
    }
    
    /**
     *This method test if xip return correctly some list of grammar object
     *as GramCateroy, GramDependency, GramVariable...
     */
    //@Ignore
    @Test public void testGetGramObject() throws Exception {
        testCollection(xip.getVariables(globalGhandler));
        testCollection(xip.getFeatureTree(globalGhandler));
        testCollection(xip.getCategories(globalGhandler));
        testCollection(xip.getDependencies(globalGhandler));
    }
    
    /**
     *This method test the parsing of a file and a XML document with a standard output.
     *The test doesn't check if analyses are valid or not, just if xip can parse
     *
     */
    //@Ignore
    @Test public void testParsing() throws Exception {
              
        Collection analyses = new Vector();
        
        //some parsing with grammar apply
        analyses.add(xip.parseFile(globalGhandler,input,output,xip.XML_NONE,true,"ISO-8859-1"));
        analyses.add(xip.parseFileXML(globalGhandler,inputXML,outputXML,2,xip.XML_NONE,true,"UTF-8"));
        analyses.add(xip.parseFileXML(globalGhandler,inputXML,outputXML,0,xip.XML_NONE,true,"UTF-8")); 
        analyses.add(xip.parseFileXmlOS(globalGhandler,inputXML,2,xip.XML_NONE,true,"UTF-8"));
        analyses.add(xip.parseFileXmlOS(globalGhandler,inputXML,0,xip.XML_NONE,true,"UTF-8"));
        
        analyses.add(xip.parseString(globalGhandler,text,xip.XML_NONE,true));
        analyses.add(xip.parseStringCharSet(globalGhandler,text,"ISO-8859-1",xip.XML_NONE,true));
        analyses.add(xip.parseStringXmlOS(globalGhandler,textXML,2,xip.XML_NONE,true));
        analyses.add(xip.parseStringXmlOS(globalGhandler,textXML,0,xip.XML_NONE,true));
        //some parsing with no grammar apply
        analyses.add(xip.parseFile(globalGhandler,input,output,xip.XML_NONE,false,"ISO-8859-1"));
        analyses.add(xip.parseFileXML(globalGhandler,inputXML,outputXML,2,xip.XML_NONE,false,"UTF-8"));
        analyses.add(xip.parseFileXML(globalGhandler,inputXML,outputXML,0,xip.XML_NONE,false,"UTF-8"));
        analyses.add(xip.parseFileXmlOS(globalGhandler,inputXML,2,xip.XML_NONE,false,"UTF-8"));
        analyses.add(xip.parseFileXmlOS(globalGhandler,inputXML,0,xip.XML_NONE,false,"UTF-8"));
        
        analyses.add(xip.parseString(globalGhandler,text,xip.XML_NONE,false));
        analyses.add(xip.parseStringCharSet(globalGhandler,text,"ISO-8859-1",xip.XML_NONE,false));
        analyses.add(xip.parseStringXmlOS(globalGhandler,textXML,2,xip.XML_NONE,false));
        analyses.add(xip.parseStringXmlOS(globalGhandler,textXML,0,xip.XML_NONE,false));
        
        //other parsing
        analyses.add(xip.parseStringXml(globalGhandler,textXML));
        analyses.add(xip.parseStringXmlOS(globalGhandler,textXML));
                
        for ( Iterator it = analyses.iterator(); it.hasNext() ;) {
            Object res= it.next();
            if ( res instanceof Boolean ) {
              assertTrue((Boolean)res);
            } else {
                assertNotNull(res);
                assertFalse(res.equals(""));
            }
        }
        
    }
    
    
    
    @Test
    @Ignore
    public void testReload() throws Exception {
          loadGrammar(englishEntity);
          int gHandler = xip.reloadGrammar(this.globalGhandler,true,root_xip + "/" + french);
          assertTrue(gHandler > -1);
    }
    
    
    @Ignore
    @Test public void lemmatize()throws Exception  {
       Collection s =  XipUtilities.toLemma(xip,globalGhandler,"voles");
       assertNotNull(s);
       assertTrue(s.size() >= 1);
       System.out.println(s);
    }
    
    
    @Test public void testMaxXipUnit()throws Exception  {
       xip.setMaxXipUnit(globalGhandler,0);
       String res = xip.parseString(globalGhandler,"Le chien mange un os. Le chat le regarde attentivement.",xip.XML_NONE,true);
       assertNotNull(res);
       System.out.println(res);
    }
    
    
    
    /**
     *This method test the parsing of a file and a XML document with a standard output.
     *The test doesn't check if analyses are valid or not, just if xip can parse
     *
     */
    //@Ignore
    @Test public void testParseFileXMLOut() throws Exception {
             
        Collection analyses = new Vector();
        
        //some parsing with grammar apply
        analyses.add(xip.parseFile(globalGhandler,input,output,xip.XML_OUTPUT,true,"ISO-8859-1"));
        analyses.add(xip.parseFileXML(globalGhandler,inputXML,outputXML,2,xip.XML_OUTPUT,true,"UTF-8"));
        analyses.add(xip.parseFileXML(globalGhandler,inputXML,outputXML,0,xip.XML_OUTPUT,true,"UTF-8")); 
        analyses.add(xip.parseFileXmlOS(globalGhandler,inputXML,2,xip.XML_OUTPUT,true,"UTF-8"));
        analyses.add(xip.parseFileXmlOS(globalGhandler,inputXML,0,xip.XML_OUTPUT,true,"UTF-8"));
        
        analyses.add(xip.parseString(globalGhandler,text,xip.XML_OUTPUT,true));
        analyses.add(xip.parseStringCharSet(globalGhandler,text,"ISO-8859-1",xip.XML_OUTPUT,true));
        analyses.add(xip.parseStringXmlOS(globalGhandler,textXML,2,xip.XML_OUTPUT,true));
        analyses.add(xip.parseStringXmlOS(globalGhandler,textXML,0,xip.XML_OUTPUT,true));
        //some parsing with no grammar apply
        analyses.add(xip.parseFile(globalGhandler,input,output,xip.XML_OUTPUT,false,"ISO-8859-1"));
        analyses.add(xip.parseFileXML(globalGhandler,inputXML,outputXML,2,xip.XML_OUTPUT,false,"UTF-8"));
        analyses.add(xip.parseFileXML(globalGhandler,inputXML,outputXML,0,xip.XML_OUTPUT,false,"UTF-8"));
        analyses.add(xip.parseFileXmlOS(globalGhandler,inputXML,2,xip.XML_OUTPUT,false,"UTF-8"));
        analyses.add(xip.parseFileXmlOS(globalGhandler,inputXML,0,xip.XML_OUTPUT,false,"UTF-8"));
        
        analyses.add(xip.parseString(globalGhandler,text,xip.XML_OUTPUT,false));
        analyses.add(xip.parseStringCharSet(globalGhandler,text,"ISO-8859-1",xip.XML_OUTPUT,false));
        analyses.add(xip.parseStringXmlOS(globalGhandler,textXML,2,xip.XML_OUTPUT,false));
        analyses.add(xip.parseStringXmlOS(globalGhandler,textXML,0,xip.XML_OUTPUT,false));
        
        
                
        for ( Iterator it = analyses.iterator(); it.hasNext() ;) {
            Object res= it.next();
            if ( res instanceof Boolean ) {
              assertTrue((Boolean)res);
            } else {
                assertNotNull(res);
                assertFalse(res.equals(""));
            }
        }
    }
    
    
    private void testCollection(Vector v ) {
        assertNotNull("the list is null",v );
        assertFalse("the list is empty",v.isEmpty());
    }
    
    
    
    public static junit.framework.Test suite() {
        return new JUnit4TestAdapter(XipTest.class);
    }
    
        
    
    public static void main(String[] args) {
        JUnitCore core = new JUnitCore();
        core.addListener(new XipUITestRunListener());
        core.run(XipTest.class);
        
        
        
    }
    
    
    
}
