package classificationExplorer;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author grondeau
 */
public class ClassificationExplorerTest {

    private static final String classificationID1 = "Dewey";
    private static final String classificationID2 = "clas1";
    private static final String topId1 = "topId1";
    private static final String topId2 = "topId2";

    /**
     *
     */
    public ClassificationExplorerTest() {
    }

    /**
     *
     * @throws java.lang.Exception
     */
    @BeforeClass
    public static void setUpClass() throws Exception {
    }

    /**
     *
     * @throws java.lang.Exception
     */
    @AfterClass
    public static void tearDownClass() throws Exception {
    }

    /**
     *
     */
    @Before
    public void setUp() {
    }

    /**
     *
     */
    @After
    public void tearDown() {
    }

    /**
     * Test of cleanDB method
     * @throws Exception
     */
    @Test
    public void testCleanDB() throws Exception {
        System.out.println("cleanDB");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.DROPANDCREATETABLES);
        File xmlFile = new File("xmlClassification/classificationDewey.xml");
        classificationExplorer.importClassification(xmlFile);
        classificationExplorer.cleanDB();
        assertEquals(new ArrayList<String>(), classificationExplorer.getTopLevelCategories(classificationID1));
        assertEquals(new ArrayList<Item>(), classificationExplorer.getTopLevelCategoryItems(classificationID2));
        assertEquals(new ArrayList<String>(), classificationExplorer.getParents(classificationID1, "100", Const.RECURSIVELY));
        assertEquals(new ArrayList<Item>(), classificationExplorer.getChildrenItems(classificationID1, "200", Const.RECURSIVELY));
        assertEquals(new ArrayList<String>(), classificationExplorer.getChildren(classificationID1, "300", Const.RECURSIVELY));
        assertEquals(new ArrayList<String>(), classificationExplorer.getCategoryLabels(classificationID1, "400", Const.RECURSIVELY));
        assertEquals(new ArrayList<Item>(), classificationExplorer.getCategoryItems(classificationID1, "500", Const.RECURSIVELY));
    }

    /**
     * Test of importClassification method
     * @throws Exception
     */
    @Test
    public void testImportClassification1() throws Exception {
        System.out.println("importXmlClassification1:");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.DROPANDCREATETABLES);
        classificationExplorer.setTopParentId(topId1);
        File xmlFile = new File("xmlClassification/classificationDewey.xml");
        String classID1 = classificationExplorer.importClassification(xmlFile);
        assertEquals("Bad classification name", classID1, classificationID1);
    }

    /**
     *
     * @throws java.lang.Exception
     */
    @Test
    public void testImportClassification2() throws Exception {
        System.out.println("importXmlClassification2:");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        classificationExplorer.setTopParentId(topId2);
        File xmlFile = new File("xmlClassification/classification1.xml");
        String classID2 = classificationExplorer.importClassification(xmlFile);
        assertEquals("Bad classification name", classID2, classificationID2);
    }

    /**
     * Test of getTopLevelCategories method:    classification ID unknow
     */
    @Test
    public void testGetTopLevelCategories1() {
        System.out.println("getTopLevelCategories1:");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        List<String> result = classificationExplorer.getTopLevelCategories("classificationIdUnknown");
        assertEquals(new ArrayList<String>(), result);
    }

    /**
     * Test of getTopLevelCategories method on classificationID1
     */
    @Test
    public void testGetTopLevelCategories2() {
        System.out.println("getTopLevelCategories2:");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        String[] expResult = {"000", "100", "200", "300", "400", "500", "600", "700", "800", "900"};
        List<String> result = classificationExplorer.getTopLevelCategories(classificationID1);
        assertArrayEquals(expResult, result.toArray());
    }

    /**
     * Test of getTopLevelCategories method  on classificationID2
     */
    @Test
    public void testGetTopLevelCategories3() {
        System.out.println("getTopLevelCategories3:");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        String[] expResult = {"000", "100", "200", "300", "400", "500", "600", "700", "800", "900"};
        List<String> result = classificationExplorer.getTopLevelCategories(classificationID2);
        assertArrayEquals(expResult, result.toArray());
    }

    /**
     * Test of getTopLevelCategoryItems method
     */
    @Test
    public void testGetTopLevelCategoryItems1() {
        System.out.println("getTopLevelCategoryItems1:");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        List<Item> result = classificationExplorer.getTopLevelCategoryItems(classificationID1);
        assertEquals(10, result.size());
        assertEquals("000", (result.get(0)).getItemId());
        assertEquals(1, (result.get(0)).getDepth());
        assertEquals(topId1, (result.get(0)).getParentId());
        assertEquals(classificationID1, (result.get(0)).getClassification().getName());
        assertEquals("Computer science, information & general works", (result.get(0)).getLabel());

        assertEquals("100", (result.get(1)).getItemId());
        assertEquals(1, (result.get(1)).getDepth());
        assertEquals(topId1, (result.get(1)).getParentId());
        assertEquals(classificationID1, (result.get(1)).getClassification().getName());
        assertEquals("Philosophy & psychology", (result.get(1)).getLabel());

        assertEquals("200", (result.get(2)).getItemId());
        assertEquals(1, (result.get(2)).getDepth());
        assertEquals(topId1, (result.get(2)).getParentId());
        assertEquals(classificationID1, (result.get(2)).getClassification().getName());
        assertEquals("Religion", (result.get(2)).getLabel());

        assertEquals("300", (result.get(3)).getItemId());
        assertEquals(1, (result.get(3)).getDepth());
        assertEquals(topId1, (result.get(3)).getParentId());
        assertEquals(classificationID1, (result.get(3)).getClassification().getName());
        assertEquals("Social sciences", (result.get(3)).getLabel());

        assertEquals("400", (result.get(4)).getItemId());
        assertEquals(1, (result.get(4)).getDepth());
        assertEquals(topId1, (result.get(4)).getParentId());
        assertEquals(classificationID1, (result.get(4)).getClassification().getName());
        assertEquals("Language", (result.get(4)).getLabel());

        assertEquals("500", (result.get(5)).getItemId());
        assertEquals(1, (result.get(5)).getDepth());
        assertEquals(topId1, (result.get(5)).getParentId());
        assertEquals(classificationID1, (result.get(5)).getClassification().getName());
        assertEquals("Science", (result.get(5)).getLabel());

        assertEquals("600", (result.get(6)).getItemId());
        assertEquals(1, (result.get(6)).getDepth());
        assertEquals(topId1, (result.get(6)).getParentId());
        assertEquals(classificationID1, (result.get(6)).getClassification().getName());
        assertEquals("Technology", (result.get(6)).getLabel());

        assertEquals("700", (result.get(7)).getItemId());
        assertEquals(1, (result.get(7)).getDepth());
        assertEquals(topId1, (result.get(7)).getParentId());
        assertEquals(classificationID1, (result.get(7)).getClassification().getName());
        assertEquals("Arts & recreation", (result.get(7)).getLabel());

        assertEquals("800", (result.get(8)).getItemId());
        assertEquals(1, (result.get(8)).getDepth());
        assertEquals(topId1, (result.get(8)).getParentId());
        assertEquals(classificationID1, (result.get(8)).getClassification().getName());
        assertEquals("Literature", (result.get(8)).getLabel());

        assertEquals("900", (result.get(9)).getItemId());
        assertEquals(1, (result.get(9)).getDepth());
        assertEquals(topId1, (result.get(9)).getParentId());
        assertEquals(classificationID1, (result.get(9)).getClassification().getName());
        assertEquals("History & geography", (result.get(9)).getLabel());
    }

    /**
     * Test of getTopLevelCategoryItems method
     */
    @Test
    public void testGetTopLevelCategoryItems2() {
        System.out.println("getTopLevelCategoryItems2:");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        List<Item> result = classificationExplorer.getTopLevelCategoryItems(classificationID2);
        assertEquals(10, result.size());
        assertEquals("000", (result.get(0)).getItemId());
        assertEquals(1, (result.get(0)).getDepth());
        assertEquals(topId2, (result.get(0)).getParentId());
        assertEquals(classificationID2, (result.get(0)).getClassification().getName());
        assertEquals("Computer science, information & general works nnnnnnnnnnnnnnnn", (result.get(0)).getLabel());

        assertEquals("100", (result.get(1)).getItemId());
        assertEquals("200", (result.get(2)).getItemId());
        assertEquals("300", (result.get(3)).getItemId());
        assertEquals("400", (result.get(4)).getItemId());
        assertEquals("500", (result.get(5)).getItemId());
        assertEquals("600", (result.get(6)).getItemId());
        assertEquals("700", (result.get(7)).getItemId());
        assertEquals("800", (result.get(8)).getItemId());

        assertEquals("900", (result.get(9)).getItemId());
        assertEquals(1, (result.get(9)).getDepth());
        assertEquals(topId2, (result.get(9)).getParentId());
        assertEquals(classificationID2, (result.get(9)).getClassification().getName());
        assertEquals("History & geography", (result.get(9)).getLabel());
    }

    /**
     * Test of getParents method  categoryId unknow
     */
    @Test
    public void testGetParents1() {
        System.out.println("getParents1");
        String categoryID = "unknown";
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        String[] expResult = {};
        List<String> result = classificationExplorer.getParents(classificationID1, categoryID, Const.RECURSIVELY);
        assertArrayEquals(expResult, result.toArray());
    }

    /**
     * Test of getParents method
     */
    @Test
    public void testGetParents2() {
        System.out.println("getParents2");
        String categoryID = "000";
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        String[] expResult = {topId1, "000", "000"};
        List<String> result = classificationExplorer.getParents(classificationID1, categoryID, Const.RECURSIVELY);
        //System.out.println(result);
        assertArrayEquals(expResult, result.toArray());
    }

    /**
     * Test of getParents method
     */
    @Test
    public void testGetParents3() {
        System.out.println("getParents3");
        String categoryID = "500";
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        String[] expResult = {topId1, "500", "500"};
        List<String> result = classificationExplorer.getParents(classificationID1, categoryID, Const.RECURSIVELY);
        //System.out.println(result);
        assertArrayEquals(expResult, result.toArray());
    }

    /**
     * Test of getParents method
     */
    @Test
    public void testGetParents4() {
        System.out.println("getParents4");
        String categoryID = "500";
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        String[] expResult = {topId1};
        List<String> result = classificationExplorer.getParents(classificationID1, categoryID, Const.NOT_RECURSIVELY);
        //System.out.println(result);
        assertArrayEquals(expResult, result.toArray());
    }

    /**
     * Test of getParents method
     */
    @Test
    public void testGetParents5() {
        System.out.println("getParents5");
        String categoryID = "515";
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        String[] expResult = {"510"};
        List<String> result = classificationExplorer.getParents(classificationID1, categoryID, Const.RECURSIVELY);
        //System.out.println(result);
        assertArrayEquals(expResult, result.toArray());
    }

    /**
     * Test of getParents method
     */
    @Test
    public void testGetParents6() {
        System.out.println("getParents6");
        String categoryID = "515";
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        String[] expResult = {"510"};
        List<String> result = classificationExplorer.getParents(classificationID1, categoryID, Const.NOT_RECURSIVELY);
        assertArrayEquals(expResult, result.toArray());
    }

    /**
     *
     */
    @Test
    public void testGetChildrenItems1() {
        System.out.println("getChildrenItems1");
        String categoryID = "600";
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        List<classificationExplorer.Item> result = classificationExplorer.getChildrenItems(classificationID1,
                categoryID, Const.RECURSIVELY);
        //System.out.println(result);
        assertEquals(20, result.size());
        assertEquals("Technology", (result.get(0)).getLabel());
        assertEquals("600", (result.get(0)).getItemId());
        assertEquals("Technology", (result.get(1)).getLabel());
        assertEquals("600", (result.get(1)).getItemId());
        assertEquals("Philosophy & theory", (result.get(2)).getLabel());
        assertEquals("601", (result.get(2)).getItemId());
    }

    /**
     *
     */
    @Test
    public void testGetChildrenItems2() {
        System.out.println("getChildrenItems2");
        String categoryID = "600";
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        List<classificationExplorer.Item> result = classificationExplorer.getChildrenItems(classificationID1,
                categoryID, Const.NOT_RECURSIVELY);
        assertEquals(10, result.size());
        assertEquals("Technology", (result.get(0)).getLabel());
        assertEquals("600", (result.get(0)).getItemId());
        assertEquals("Medicine & health", (result.get(1)).getLabel());
        assertEquals("610", (result.get(1)).getItemId());
        assertEquals("Engineering", (result.get(2)).getLabel());
        assertEquals("620", (result.get(2)).getItemId());
        assertEquals("630", (result.get(3)).getItemId());
        assertEquals("640", (result.get(4)).getItemId());
        assertEquals("650", (result.get(5)).getItemId());
        assertEquals("660", (result.get(6)).getItemId());
        assertEquals("670", (result.get(7)).getItemId());
        assertEquals("680", (result.get(8)).getItemId());
        assertEquals("Building & construction", (result.get(9)).getLabel());
        assertEquals("690", (result.get(9)).getItemId());
    }

    /**
     * Test of getChildren method
     */
    @Test
    public void testGetChildren1() {
        System.out.println("getChildren1");
        String categoryID = "000";
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        String[] expResult = {"000", "000", "001", "002", "003", "004", "005", "006",
            "007", "008", "009", "010", "020", "030", "040", "050", "060", "070", "080", "090"};
        List<String> result = classificationExplorer.getChildren(classificationID1, categoryID, Const.RECURSIVELY);
        //System.out.println(result);
        assertArrayEquals(expResult, result.toArray());
    }

    /**
     * Test of getChildren method.
     */
    @Test
    public void testGetChildren2() {
        System.out.println("getChildren2");
        String categoryID = "000";
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        String[] expResult = {"000", "010", "020", "030", "040", "050", "060", "070", "080", "090"};
        List<String> result = classificationExplorer.getChildren(classificationID1, categoryID, Const.NOT_RECURSIVELY);
        //System.out.println(result);
        assertArrayEquals(expResult, result.toArray());
    }

    /**
     * Test of getChildren method
     */
    @Test
    public void testGetChildren3() {
        System.out.println("getChildren3");
        String categoryID = "600";
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        String[] expResult = {"600", "600", "601", "602", "603", "604", "605", "606",
            "607", "608", "609", "610", "620", "630", "640", "650", "660", "670", "680", "690"};
        List<String> result = classificationExplorer.getChildren(classificationID1, categoryID, Const.RECURSIVELY);
        assertArrayEquals(expResult, result.toArray());
    }

    /**
     * Test of getChildren method
     */
    @Test
    public void testGetChildren4() {
        System.out.println("getChildren4");
        String categoryID = "600";
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        String[] expResult = {"600", "610", "620", "630", "640", "650", "660", "670", "680", "690"};
        List<String> result = classificationExplorer.getChildren(classificationID1, categoryID, Const.NOT_RECURSIVELY);
        assertArrayEquals(expResult, result.toArray());
    }

    /**
     * Test of getChildren method
     */
    @Test
    public void testGetChildren5() {
        System.out.println("getChildren5");
        String categoryID = "515";
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        String[] expResult = {};
        List<String> result = classificationExplorer.getChildren(classificationID1, categoryID, Const.NOT_RECURSIVELY);
        assertArrayEquals(expResult, result.toArray());
    }

    /**
     * Test of getCategoryLevels method
     */
    @Test
    public void testGetCategoryLabels1() {
        System.out.println("getCategoryLabels1");
        String categoryID = "800";
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        String[] expResult = {"Literature", "Literature, rhetoric & criticism", "Literature & rhetoric"};
        List<String> result = classificationExplorer.getCategoryLabels(classificationID1, categoryID, Const.RECURSIVELY);
        assertArrayEquals(expResult, result.toArray());
    }

    /**
     *
     */
    @Test
    public void testGetCategoryLabels2() {
        System.out.println("getCategoryLabels2");
        String categoryID = "500";
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        String[] expResult = {"Science", "Science", "Natural sciences & mathematics"};
        List<String> result = classificationExplorer.getCategoryLabels(classificationID1, categoryID, Const.RECURSIVELY);
        assertArrayEquals(expResult, result.toArray());
    }

    /**
     *
     */
    @Test
    public void testGetCategoryLabels3() {
        System.out.println("getCategoryLabels3");
        String categoryID = "200";
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        String[] expResult = {"Religion", "Religion", "Religion"};
        List<String> result = classificationExplorer.getCategoryLabels(classificationID1, categoryID, Const.RECURSIVELY);
        assertArrayEquals(expResult, result.toArray());
    }

    /**
     *
     */
    @Test
    public void testGetCategoryLabels4() {
        System.out.println("getCategoryLabels4");
        String categoryID = "200";
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        String[] expResult = {"Religion", "Religion", "Religion"};
        List<String> result = classificationExplorer.getCategoryLabels(classificationID2, categoryID, Const.RECURSIVELY);
        assertArrayEquals(expResult, result.toArray());
    }

    /**
     *
     */
    @Test
    public void testGetCategoryLabels5() {
        System.out.println("getCategoryLabels5");
        String categoryID = "000";
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        String[] expResult = {"Computer science, information & general works nnnnnnnnnnnnnnnn",
            "Computer science, knowledge & systems", "Computer science, information & general works"};
        List<String> result = classificationExplorer.getCategoryLabels(classificationID2, categoryID, Const.RECURSIVELY);
        assertArrayEquals(expResult, result.toArray());
    }

    /**
     *
     */
    @Test
    public void testGetCategoryItems1() {
        System.out.println("getCategoryItems1");
        String categoryID = "000";
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        List<Item> result = classificationExplorer.getCategoryItems(classificationID1, categoryID, Const.RECURSIVELY);
        assertEquals(3, result.size());
        //System.out.println(result);
        assertEquals("000", (result.get(0)).getItemId());
        assertEquals(1, (result.get(0)).getDepth());
        assertEquals("Computer science, information & general works", (result.get(0)).getLabel());
        assertEquals(topId1, (result.get(0)).getParentId());
        assertEquals(classificationID1, (result.get(0)).getClassification().getName());

        assertEquals("000", (result.get(1)).getItemId());
        assertEquals(2, (result.get(1)).getDepth());
        assertEquals("Computer science, knowledge & systems", (result.get(1)).getLabel());
        assertEquals("000", (result.get(1)).getParentId());
        assertEquals(classificationID1, (result.get(1)).getClassification().getName());

        assertEquals("000", (result.get(2)).getItemId());
        assertEquals(3, (result.get(2)).getDepth());
        assertEquals("Computer science, information & general works", (result.get(2)).getLabel());
        assertEquals("000", (result.get(2)).getParentId());
        assertEquals(classificationID1, (result.get(2)).getClassification().getName());
    }

    /**
     *
     */
    @Test
    public void testGetCategoryItems2() {
        System.out.println("getCategoryItems2");
        String categoryID = "000";
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        List<Item> result = classificationExplorer.getCategoryItems(classificationID1, categoryID, Const.NOT_RECURSIVELY);
        assertEquals(1, result.size());
        //System.out.println(result);
        assertEquals("000", (result.get(0)).getItemId());
        assertEquals(1, (result.get(0)).getDepth());
        assertEquals("Computer science, information & general works", (result.get(0)).getLabel());
        assertEquals(topId1, (result.get(0)).getParentId());
        assertEquals(classificationID1, (result.get(0)).getClassification().getName());
    }

    /**
     * Test of removeClassification().
     */
    @Test
    public void testRemoveClassification() {
        System.out.println("removeClassification");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        classificationExplorer.removeClassification(classificationID1);
        String[] expResult = {};
        List<String> result = classificationExplorer.getTopLevelCategories(classificationID1);
        assertArrayEquals(expResult, result.toArray());
    }

    /**
     * Test of cleanDB method
     * @throws Exception
     */
    @Test
    public void testCleanDB1() throws Exception {
        System.out.println("cleanDB1");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        classificationExplorer.cleanDB();
        String categoryID = "000";
        String[] expResult = {};
        List<String> result = classificationExplorer.getCategoryLabels(classificationID2, categoryID, Const.RECURSIVELY);
        assertArrayEquals(expResult, result.toArray());
    }

    @Test
    public void testancestorOf1() throws Exception {
        System.out.println("ancestorOf1");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        classificationExplorer.cleanDB();
        File xmlFile = new File("xmlClassification/t2.xml");
        String classID3 = new String(classificationExplorer.importClassification(xmlFile));
        Item result = classificationExplorer.ancestorOf(classID3, "628.100", -1);
        assertEquals("label-628-3", result.getLabel());
    }

    @Test
    public void testancestorOf2() throws Exception {
        System.out.println("ancestorOf2");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        classificationExplorer.cleanDB();
        File xmlFile = new File("xmlClassification/t2.xml");
        String classID3 = classificationExplorer.importClassification(xmlFile);
        Item result = classificationExplorer.ancestorOf(classID3, "629", -1);
        assertEquals("label-620-2", result.getLabel());
    }

    @Test
    public void testancestorOf3() throws Exception {
        System.out.println("ancestorOf3");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        classificationExplorer.cleanDB();
        File xmlFile = new File("xmlClassification/t2.xml");
        String classID3 = classificationExplorer.importClassification(xmlFile);
        Item result = classificationExplorer.ancestorOf(classID3, "611", -1);
        assertEquals("label-610-2", result.getLabel());
    }

    @Test
    public void testancestorOf4() throws Exception {
        System.out.println("ancestorOf4");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        classificationExplorer.cleanDB();
        File xmlFile = new File("xmlClassification/t2.xml");
        String classID3 = classificationExplorer.importClassification(xmlFile);
        Item result = classificationExplorer.ancestorOf(classID3, "601", -1);
        assertEquals("label-600-1", result.getLabel());
    }

    @Test
    public void testancestorOf5() throws Exception {
        System.out.println("ancestorOf5");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        classificationExplorer.cleanDB();
        File xmlFile = new File("xmlClassification/t2.xml");
        String classID3 = classificationExplorer.importClassification(xmlFile);
        Item result = classificationExplorer.ancestorOf(classID3, "600", -1);
        assertEquals("label-top-0", result.getLabel());
    }

    @Test
    public void testancestorOf6() throws Exception {
        System.out.println("ancestorOf6");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        classificationExplorer.cleanDB();
        File xmlFile = new File("xmlClassification/t2.xml");
        String classID3 = classificationExplorer.importClassification(xmlFile);
        Item result = classificationExplorer.ancestorOf(classID3, "620", -1);
        assertEquals("label-600-1", result.getLabel());
    }

    @Test
    public void testancestorOf7() throws Exception {
        System.out.println("ancestorOf7");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        classificationExplorer.cleanDB();
        File xmlFile = new File("xmlClassification/t2.xml");
        String classID3 = classificationExplorer.importClassification(xmlFile);
        Item result = classificationExplorer.ancestorOf(classID3, "628", -1);
        assertEquals("label-620-2", result.getLabel());
    }

    @Test
    public void testancestorOf8() throws Exception {
        System.out.println("ancestorOf8");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        classificationExplorer.cleanDB();
        File xmlFile = new File("xmlClassification/t2.xml");
        String classID3 = classificationExplorer.importClassification(xmlFile);
        Item result = classificationExplorer.ancestorOf(classID3, "631", -1);
        assertEquals("label-600-1", result.getLabel());
    }

    @Test
    public void testancestorOf9() throws Exception {
        System.out.println("ancestorOf9");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        classificationExplorer.cleanDB();
        File xmlFile = new File("xmlClassification/t2.xml");
        String classID3 = classificationExplorer.importClassification(xmlFile);
        Item result = classificationExplorer.ancestorOf(classID3, "500.100", -1);
        assertEquals("label-500-3", result.getLabel());
    }

    @Test
    public void testancestorOf10() throws Exception {
        System.out.println("ancestorOf10");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        classificationExplorer.cleanDB();
        File xmlFile = new File("xmlClassification/t2.xml");
        String classID3 = classificationExplorer.importClassification(xmlFile);
        Item result = classificationExplorer.ancestorOf(classID3, "500.101", -1);
        assertEquals("label-500-3", result.getLabel());
    }

    @Test
    public void testancestorOf11() throws Exception {
        System.out.println("ancestorOf11");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        classificationExplorer.cleanDB();
        File xmlFile = new File("xmlClassification/t2.xml");
        String classID3 = classificationExplorer.importClassification(xmlFile);
        Item result = classificationExplorer.ancestorOf(classID3, "500", 3);
        assertEquals("label-500-2", result.getLabel());
    }

    @Test
    public void testancestorOf12() throws Exception {
        System.out.println("ancestorOf12");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        classificationExplorer.cleanDB();
        File xmlFile = new File("xmlClassification/t2.xml");
        String classID3 = classificationExplorer.importClassification(xmlFile);
        Item result = classificationExplorer.ancestorOf(classID3, "500", 2);
        assertEquals("label-500-1", result.getLabel());
    }

    @Test
    public void testancestorOf13() throws Exception {
        System.out.println("ancestorOf13");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        classificationExplorer.cleanDB();
        File xmlFile = new File("xmlClassification/t2.xml");
        String classID3 = classificationExplorer.importClassification(xmlFile);
        Item result = classificationExplorer.ancestorOf(classID3, "501", -1);
        assertEquals("label-500-2", result.getLabel());
    }

    //return 500-3 arther than top-0
//    @Test
//    public void testancestorOf14() throws Exception {
//        System.out.println("ancestorOf14");
//        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
//        classificationExplorer.cleanDB();
//        File xmlFile = new File("xmlClassification/t2.xml");
//        String classID3 = classificationExplorer.importClassification(xmlFile);
//        Item result = classificationExplorer.ancestorOf(classID3, "500", 1);
//        assertEquals("label-top-0", result.getLabel());
//    }
    @Test
    public void testchildOf1() throws Exception {
        System.out.println("childOf1");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        classificationExplorer.cleanDB();
        File xmlFile = new File("xmlClassification/t2.xml");
        String classID3 = classificationExplorer.importClassification(xmlFile);
        List<Item> child = classificationExplorer.childOf(classID3, "500", -1);
        assertEquals(2, child.size());
        assertEquals("500", (child.get(0)).getItemId());
        assertEquals(2, (child.get(0)).getDepth());
        assertEquals("510", (child.get(1)).getItemId());
        assertEquals(2, (child.get(1)).getDepth());
    }

    @Test
    public void testchildOf2() throws Exception {
        System.out.println("childOf2");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        classificationExplorer.cleanDB();
        File xmlFile = new File("xmlClassification/t2.xml");
        String classID3 = classificationExplorer.importClassification(xmlFile);
        List<Item> child = classificationExplorer.childOf(classID3, "500", 1);
        assertEquals(2, child.size());
        assertEquals("500", (child.get(0)).getItemId());
        assertEquals(2, (child.get(0)).getDepth());
        assertEquals("510", (child.get(1)).getItemId());
        assertEquals(2, (child.get(1)).getDepth());
    }

    @Test
    public void testchildOf3() throws Exception {
        System.out.println("childOf3");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        classificationExplorer.cleanDB();
        File xmlFile = new File("xmlClassification/t2.xml");
        String classID3 = classificationExplorer.importClassification(xmlFile);
        List<Item> child = classificationExplorer.childOf(classID3, "500", 2);
        assertEquals(3, child.size());
        assertEquals("500", (child.get(0)).getItemId());
        assertEquals(3, (child.get(0)).getDepth());
        assertEquals("501", (child.get(1)).getItemId());
        assertEquals(3, (child.get(1)).getDepth());
        assertEquals("502", (child.get(2)).getItemId());
        assertEquals(3, (child.get(2)).getDepth());
    }

    @Test
    public void testchildOf4() throws Exception {
        System.out.println("childOf4");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        classificationExplorer.cleanDB();
        File xmlFile = new File("xmlClassification/t2.xml");
        String classID3 = classificationExplorer.importClassification(xmlFile);
        List<Item> child = classificationExplorer.childOf(classID3, "500.100", -1);
        assertEquals(0, child.size());
    }

    @Test
    public void testchildOf5() throws Exception {
        System.out.println("childOf5");
        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        classificationExplorer.cleanDB();
        File xmlFile = new File("xmlClassification/t2.xml");
        String classID3 = classificationExplorer.importClassification(xmlFile);
        List<Item> child = classificationExplorer.childOf(classID3, "600", -1);
        assertEquals(2, child.size());
        assertEquals("610", (child.get(0)).getItemId());
        assertEquals(2, (child.get(0)).getDepth());
        assertEquals("620", (child.get(1)).getItemId());
        assertEquals(2, (child.get(1)).getDepth());
    }

//    @Test
//    public void testchildOf6() throws Exception {
//        System.out.println("childOf6");
//        ClassificationExplorer classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
//        classificationExplorer.cleanDB();
//        File xmlFile = new File("xmlClassification/t2.xml");
//        String classID3 = classificationExplorer.importClassification(xmlFile);
//        List<Item> child = classificationExplorer.childOf(classID3, "630", -1);
//        assertEquals(1, child.size());
//        assertEquals("631", (child.get(0)).getItemId());
//        assertEquals(3, (child.get(0)).getDepth());
//    }
}
