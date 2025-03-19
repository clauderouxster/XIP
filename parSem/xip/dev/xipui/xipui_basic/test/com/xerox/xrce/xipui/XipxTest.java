/*
 * XipxTest.java
 *
 * Created on February 28, 2007, 11:18 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.xerox.xrce.xipui;


import com.xerox.jxip.XipResult;
import com.xerox.xrce.xipui.util.FileTools;
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
public class XipxTest {
    
    private static Xip xip;
    static private  String root_xip = "h:/parSem/xip/dev/GRAMMARS";
    
    private static int handler = -1;
    
    
    private Xipx xipx;
    private XipResult xipResult;
    
    private File input;
    private File inputXML;
    private String text;
    private String textXML;
    
    
    @BeforeClass
    public static void init() throws Exception {
        xip = new Xip();
        handler = xip.loadGrammar(root_xip + "/" + Grammars.englishNorm, 0, true);
        assertTrue("Grammar not loaded",handler != -1);
        
        
    }
    
    /**
     *Free the Xip instance and the grammar
     */
    @AfterClass static public void free() throws Exception {
        xip.freeGrammar(handler);
        xip = null;
        handler = -1;
        
    }
    
    /**
     *Before each test create the corpora to parse, a file, an xml file, String and String in XML representation
     *@throws Exception if file can't be read
     */
    
    @Before
    public void setCorpusFile() throws Exception {
        URI uri = getClass().getResource("/com/xerox/xrce/xipui/test/corpus.txt").toURI();
        URI uriXML = getClass().getResource("/com/xerox/xrce/xipui/test/corpus.xml").toURI();
        input = new File(uri);
        inputXML = new File(uriXML);
        text = FileTools.read(input,"ISO-8859-1");
        textXML = FileTools.read(inputXML,"UTF-8");
    }
    
    
    /**
     *Creates the Xipx instance
     *check if the xipresult is well null
     */
    @Before
    public void createXipx() {
        xipx = new Xipx(xip);
        xipResult = xipx.getXipResult();
        assertNull(xipResult);
    }
    
    /**
     *Free the Xipx instance
     */
    @After
    public void freeXipx() {
        xipx = null;
        xipResult = null;
    }
    
    /**
     * Free the corpus files 
     */
    @After
    public void unsetCorpusFile() {
        input = null;
        inputXML = null;
        text = null;
        textXML = null;
    }
    
    
    /**
     * This method permits to test if a call back is called for each XipUnit
     * the xipresult sould not be null and it should contain only one XipUnit
     */
    public void callBackTest() {
        xipResult = xipx.getXipResult();
        assertNotNull(xipResult);
        int sizeExpected = 1;
        assertEquals(1,xipResult.getXipUnits().size());
        System.out.println("callback ok");
    }
    
    
    /**
     * This method permits to test if a call back XML is called for each tag xml
     * in accordance with the xmlguide of the grammar
     * The method gets the info of each tag and check if they are not null and not empty
     */
    
    public void callBackXMLTest() {
        xipResult = xipx.getXipResult();
        assertNotNull(xipResult);
        Map map = xipx.getXmlTagInfo();
        assertNotNull(map);
        assertTrue(map.size() > 0 );
        System.out.println("callbackXML ok");
        
    }
    
    /**
     *test the callback for a file 
     *@see callBackTest()
     */
    @Test public void testCallMethodForFile() throws Exception  {
        xipx.callMethodOnXipUnitFromFile(handler,input,this,"callBackTest","ISO-8859-1");
    }
    
    /**
     *Test if an error was thrown with a no existing file.
     *for the xipx.callMethodOnXipUnitFromFile() method
     */
    @Test(expected=FileNotFoundException.class)
    public void testCallMethodForFileFailed() throws Exception {
        xipx.callMethodOnXipUnitFromFile(handler,new File(input.hashCode()+"null"),this,"callBackTest","ISO-8859-1");
    }
    
    /**
     *Test if an error was thrown if the caller for the callback is null.
     *for the xipx.callMethodOnXipUnitFromFile() method
     */
    @Test(expected=NullPointerException.class)
    public void testCallMethodForFileFailed2() throws Exception  {
        xipx.callMethodOnXipUnitFromFile(handler,input,null,"callBackTest","ISO-8859-1");
    }
    
    /**
     *Test if an error was thrown if the name of the method callback is not correct.
     *for the xipx.callMethodOnXipUnitFromFile() method
     *
     */
    @Test(expected=Exception.class)
    public void testCallMethodForFileFailed3() throws Exception  {
        xipx.callMethodOnXipUnitFromFile(handler,input,this,"null","ISO-8859-1");
    }
    
    /**
     *Test the callback method and the call back xml was well called for each XipUnit
     *@see callBackTest()
     *@see callBackXMLTest()
     */
    @Test public void testCallMethodForXMLFile() throws Exception  {
        xipx.callMethodOnXipUnitFromXMLFile(handler,inputXML,this,"callBackTest","callBackXMLTest","ISO-8859-1");
    }
    
     /**
     *Test if an error was thrown if the file is null
     *for the xipx.callMethodOnXipUnitFromXMLFile() method
     *
     */
    @Test(expected=Exception.class)
    public void testCallMethodForXMLFileFailed() throws Exception  {
        xipx.callMethodOnXipUnitFromXMLFile(handler,null,this,"callBackTest","callBackXMLTest","ISO-8859-1");
        
    }
    
    /**
     *Test if an error was thrown if the file doesn't exist
     *for the xipx.callMethodOnXipUnitFromXMLFile() method
     *
     */
    @Test(expected=Exception.class)
    public void testCallMethodForXMLFileFailed5() throws Exception  {
        xipx.callMethodOnXipUnitFromXMLFile(handler,new File(inputXML.hashCode() + "null"),this,"callBackTest","callBackXMLTest","ISO-8859-1");
    }
    
    
     /**
     *Test if an error was thrown if the caller for callback is null 
     *for the xipx.callMethodOnXipUnitFromXMLFile() method
     *
     */
    @Test(expected=Exception.class)
    public void testCallMethodForXMLFileFailed2() throws Exception  {
        xipx.callMethodOnXipUnitFromXMLFile(handler,inputXML,null,"callBackTest","callBackXMLTest","ISO-8859-1");
    }
    
     /**
     *Test if an error was thrown if the method callback is not correct
     *for the xipx.callMethodOnXipUnitFromXMLFile() method
     *
     */
    @Test(expected=Exception.class)
    public void testCallMethodForXMLFileFailed3() throws Exception  {
        xipx.callMethodOnXipUnitFromXMLFile(handler,inputXML,this,"null","callBackXMLTest","ISO-8859-1");
    }
    
    /**
     *Test if an error was thrown if the method callbackxml is not correct
     *for the xipx.callMethodOnXipUnitFromXMLFile() method
     *
     */
    @Test(expected=Exception.class)
    public void testCallMethodForXMLFileFailed4() throws Exception  {
        xipx.callMethodOnXipUnitFromXMLFile(handler,inputXML,this,"callBackTest","null","ISO-8859-1");
    }
    
    
    /**
     *Test if the callback is well called for each XipUnit
     *@see callBackTest()
     */    
    @Test public void testCallMethodForText()  throws Exception {
        xipx.callMethodOnXipUnitFromText(handler,text,this,"callBackTest","ISO-8859-1");
    }
    
    /**
     *Test if an error was thrown if the text to parse is null
     *for the xipx.callMethodOnXipUnitFromText() method
     *
     */
    @Test(expected=NullPointerException.class)
    public void testCallMethodForTextFailed()  throws Exception {
        xipx.callMethodOnXipUnitFromText(handler,null,this,"callBackTest","ISO-8859-1");
    }
    
    /**
     *Test if an error was thrown if the caller of the callback is null
     *for the xipx.callMethodOnXipUnitFromText() method
     *
     */
    @Test(expected=NullPointerException.class)
    public void testCallMethodForTextFailed2()  throws Exception {
        xipx.callMethodOnXipUnitFromText(handler,text,null,"callBackTest","ISO-8859-1");
    }
    
    /**
     *Test if an error was thrown if the name of the callback is incorrect
     *for the xipx.callMethodOnXipUnitFromText() method
     *
     */
    @Test(expected=Exception.class)
    public void testCallMethodForTextFailed3()  throws Exception {
        xipx.callMethodOnXipUnitFromText(handler,text,this,"null","ISO-8859-1");
    }
    
    /**
     *Test if a XipResult is not null after 
     *a parsing a Text input
     */
    @Test public void testBuildXipResult() throws Exception {
        boolean success = xipx.buildXipResult(handler,text);
        assertTrue(success);
        xipResult = xipx.getXipResult();
        assertNotNull(xipResult);
        
    }
    
     /**
      *test if an error was thrown if the text to parse is null
      */
    @Test(expected=NullPointerException.class)
    public void testBuildXipResultFailed() throws Exception {
        xipx.buildXipResult(handler,null);
    }
    
    /**
     *Test if a XipResult was not null after the parsing of a file
     */
    @Test public void testBuildXipResultFromFile() throws Exception {
        boolean success = xipx.buildXipResultFromFile(handler,input,"ISO-8859-1");
        assertTrue(success);
        xipResult = xipx.getXipResult();
        assertNotNull(xipResult);
    }
    
    /**
     *test if an error is thrown if the file input is null
     *for the xipx.buildXipResultFromFile() method
     */
    @Test(expected=NullPointerException.class)
    public void testBuildXipResultFromFileFailed() throws Exception {
        
        xipx.buildXipResultFromFile(handler,null,"ISO-8859-1");
    }
    
    /**
     *test if an error is thrown if the file input doesn't exist
     *for the xipx.buildXipResultFromFile() method
     */
    @Test(expected=FileNotFoundException.class)
    public void testBuildXipResultFromFileFailed2() throws Exception {
        //a file that doesn't exist
        xipx.buildXipResultFromFile(handler,new File(input.hashCode() + "null"),"ISO-8859-1");
    }
    
    
    /**
     *test if an error is thrown if the file input doesn't exist
     *for the xipx.buildXipResultFromXmlFile() method
     */
    @Test(expected=FileNotFoundException.class)
    public void testBuildXipResultFromXmlFileFailed() throws Exception {
        xipx.buildXipResultFromXmlFile(handler,new File(input.hashCode() + "null"),"ISO-8859-1");
    }
    
     /**
     *test if an error is thrown if the file input is null
     *for the xipx.buildXipResultFromXmlFile() method
     */
    @Test(expected=NullPointerException.class)
    public void testBuildXipResultFromXmlFileFailed2() throws Exception {
        xipx.buildXipResultFromXmlFile(handler,null,"ISO-8859-1");
    }
      
    
     /**
     *test if a XipResult is not null after a xml file was parsed
     */
    @Test public void testBuildXipResultFromXMLFile() throws Exception{
        boolean success = xipx.buildXipResultFromXmlFile(handler,inputXML,"UTF-8");
        assertTrue(success);
        xipResult = xipx.getXipResult();
        assertNotNull(xipResult);
    }
    
    
    
    /**
     *use for JUnit with a version earlier than JUnit 4.0
     */
    public static junit.framework.Test suite() {
        return new JUnit4TestAdapter(XipxTest.class);
    }
    
}
