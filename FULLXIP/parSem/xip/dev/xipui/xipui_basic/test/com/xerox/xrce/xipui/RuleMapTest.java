/*
 * RuleMapTest.java
 *
 * Created on February 27, 2007, 3:31 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.xerox.xrce.xipui;

import java.util.Map;
import junit.framework.JUnit4TestAdapter;
import org.junit.*;
import java.io.*;
import java.util.Random;
import static org.junit.Assert.*;
/**
 *
 * @author vgrassau
 */
public class RuleMapTest {
      
    private static Xip xip;
    private static int gHandler = -1;
    
    private Map currentTable;
    private RuleMap ruleMap;
    
    /**
     * Init the test, create an instance of Xip and load a grammar. 
     * the Xip and the grammar handler are static for this test.
     * test also if the grammar handler are different than -1 after the loading.
     * 
     */
    @BeforeClass
    static public void initTest() throws Exception  {
        String os =System.getProperty("os.name") ;
        String   root_xip = "H:/parSem/xip/dev/GRAMMARS";
        if ( !os.startsWith("Windows")) {
           root_xip = "/home/vgrassau/parSem/xip/dev/GRAMMARS";
        } 
              
         xip = new Xip();
       
        gHandler = xip.loadGrammar(root_xip + "/" + Grammars.englishEntity,0);
        
        assertFalse(gHandler == -1);
        
   }
    
    /**
     *Free the memory 
     */
    @AfterClass 
    static public void freeTest() throws Exception {
        xip.freeGrammar(gHandler);
        xip = null;
        gHandler = -1;
    }
    
    /**
     *Init a ruleMap with the global gHanlder
     *@see initTest()
     *@see tearDown()
     */
    @Before public void setUp() {
        ruleMap = new RuleMap();
        ruleMap.init(gHandler);
        this.currentTable = ruleMap.getFileTable();
        
    }
    
    /**
     *Free memory taken by the ruleMap and the currentMap
     */
    @After public void tearDown() {
        this.currentTable = null;
        ruleMap = null;
    }
    
    /**
     *Test the file map
     *@see testMap()
     */
    @Test public void testGetFileTable() {
        testMap(ruleMap.getFileTable());
    }
    
    /**
     *Test the layer absolute map
     *@see testMap()
     */
    @Test public void testGetLayerAbsTable() {
        testMap(ruleMap.getLayerAbsTable());
    }
    
    /**
     *Test the layer relative map
     *@see testMap()
     */
    @Test public void testGetLayerRelTable() {
        testMap(ruleMap.getLayerRelTable());
    }
    
    /**
     *Test the Type map
     *@see testMap()
     */
    @Test public void testGetTypeTable() {
        testMap(ruleMap.getTypeTable());
    }
    
    /**
     *Test the number map
     *@see testMap()
     */
    @Test public void testGetNumberTable() {
        testMap(ruleMap.getNumberTable());
    }
    
    /**
     *Test the method getFileByRuleNb() 
     *for each rules number we check if the returned file is not null and if it exists
     */
    @Test public void testGetFileByRuleNb() {
        testGetRuleMax();
        testGetCountRules();
        for ( int i=0; i < ruleMap.getCountRules(); i++) {
           File f = ruleMap.getFileByRuleNb(i);
           assertNotNull(f);
           assertTrue(f.exists());
        }
    }
    
    /*
     *Test the getRuleLoaded() method
     *for each RuleLoaded, we check if we have exactly a RuleLoaded object and if it is not null
     */
    @Test public void testGetRuleLoaded() {
        testGetRuleMax();
        testGetCountRules();
        for ( int i=0; i < ruleMap.getCountRules(); i++) {
           Object rule = ruleMap.getRuleLoaded(i);
           assertNotNull(rule);
           assertTrue(rule instanceof com.xerox.jxip.RuleLoaded);
        }
    }
    
    /**
     * Tests the isAFileLimit() method, 
     * we put randomly a rule limit, get the associated file to this limit.
     * And check if the isAFileLimit return true.
     * 
     */
    @Test public void testIsAFileLimit() throws Exception {
        Random random = new Random();
        int limit = random.nextInt(ruleMap.getRuleMax());
        xip.setRuleLimit(gHandler,limit);
        File file = ruleMap.getFileByRuleNb(limit);
        assertNotNull(file);
        assertTrue(ruleMap.isAFileLimit(limit,file));
        xip.setRuleLimit(gHandler,-1);
       
    }
    
    /**
     *Test the getRuleNumber(String,long) get the same result if we use 
     * the getOffsetByRuleNb() and the getFileByRuleNb() methods
     */
    @Test public void testOffsetAndRuleNumber() {
        Random random = new Random();
        int ruleRandom = random.nextInt(ruleMap.getRuleMax());
        

        long offset = ruleMap.getOffsetByRuleNb(ruleRandom);
        File file = ruleMap.getFileByRuleNb(ruleRandom);
        assertTrue(offset >=0);
        assertNotNull(file);
        int ruleNumber = ruleMap.getRuleNumber(file.getName(),offset);
        assertEquals(ruleRandom,ruleNumber);
        
        
    }
    
    /**
     *Test the getRuleMax method
     *test if the rule max is greater than or equals to 0
     */
    @Test public void testGetRuleMax() {
        int max = ruleMap.getRuleMax();
        assertTrue(max>=0);
    }
    
    /**
     *Tests the getCountRules method
     *test if result is strictly greater than 0
     */
    @Test public void testGetCountRules() {
        int count = ruleMap.getCountRules();
        assertTrue(count>0);
    }
    
    /**
     *test a getMap method of the RuleMap object
     *The test is ok if the map is not null, is the same object than the currentMap
     *and the map is not empty
     *@param map the map to test
     */
    private void testMap(Map map) {
        assertNotNull(map);
        assertSame(map,currentTable);
        assertFalse(map.isEmpty());
    }
    
    
    /**
     *use for JUnit with a version earlier than JUnit 4.0 
     */
   public static junit.framework.Test suite() {
        return new JUnit4TestAdapter(RuleMapTest.class);
    }

}
