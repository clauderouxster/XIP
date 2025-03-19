/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package classificationExplorer;

import java.io.File;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author grondeau
 */
public class Test {

    private ClassificationExplorer classificationExplorer;
    private String classificationID1;
    private String classificationID2;
    private static final String topId1 = "topId1";
    private static final String topId2 = "topId2";

    /**
     *
     */
    public Test() {
    }

    void test1() {
        getTopLevelCategoryItems(classificationID1);
        getTopLevelCategories(classificationID1);
    }

    void test2() {
        getCategoryItems(classificationID1, "500", Const.RECURSIVELY);
        getParents(classificationID1, "500", Const.RECURSIVELY);
        getCategoryItems(classificationID1, "0", Const.RECURSIVELY);
        getParents(classificationID1, "0", Const.RECURSIVELY);
        getCategoryItems(classificationID1, "000", Const.RECURSIVELY);
        getParents(classificationID1, "000", Const.RECURSIVELY);
        getCategoryItems(classificationID1, "716", Const.RECURSIVELY);
        getParents(classificationID1, "716", Const.RECURSIVELY);
    }

    void test3() {
        getCategoryItems(classificationID1, "000", Const.RECURSIVELY);
        getCategoryLabels(classificationID1, "000", Const.RECURSIVELY);
        getCategoryItems(classificationID1, "500", Const.RECURSIVELY);
        getCategoryLabels(classificationID1, "500", Const.RECURSIVELY);
        getCategoryItems(classificationID1, "200", Const.RECURSIVELY);
        getCategoryLabels(classificationID1, "200", Const.RECURSIVELY);
        getCategoryItems(classificationID1, "800", Const.RECURSIVELY);
        getCategoryLabels(classificationID1, "800", Const.RECURSIVELY);
    }

    void test41() {
        getChildrenItems(classificationID1, "000", Const.RECURSIVELY);
        getChildren(classificationID1, "000", Const.RECURSIVELY);
        getChildrenItems(classificationID1, "000", Const.NOT_RECURSIVELY);
        getChildren(classificationID1, "000", Const.NOT_RECURSIVELY);
    }

    void test42() {
        getChildrenItems(classificationID1, "100", Const.RECURSIVELY);
        getChildren(classificationID1, "100", Const.RECURSIVELY);
        getChildrenItems(classificationID1, "100", Const.NOT_RECURSIVELY);
        getChildren(classificationID1, "100", Const.NOT_RECURSIVELY);
    }

    void test43() {
        getChildrenItems(classificationID1, "600", Const.RECURSIVELY);
        getChildren(classificationID1, "600", Const.RECURSIVELY);
        getChildrenItems(classificationID1, "600", Const.NOT_RECURSIVELY);
        getChildren(classificationID1, "600", Const.NOT_RECURSIVELY);
    }

    void test4() {
        test41();
        test42();
        test43();
    }

    void test5() {
        getTopLevelCategoryItems(classificationID2);
        getTopLevelCategories(classificationID2);
        getCategoryItems(classificationID2, "000", Const.RECURSIVELY);
        getCategoryLabels(classificationID2, "000", Const.RECURSIVELY);

    }

    void test04() throws Exception {
        classificationExplorer = new ClassificationExplorer("config/classificationExplorer.properties",
                Const.NOT_DROPANDCREATETABLES);
        cleanDB();
        classificationExplorer.setTopParentId(topId1);
        classificationID1 = importClassification("t2.xml");
//        ancestorOf(classificationID1, "628.100", -1);
//        ancestorOf(classificationID1, "629", -1);
//        ancestorOf(classificationID1, "611", -1);
//        ancestorOf(classificationID1, "601", -1);
//        ancestorOf(classificationID1, "600", -1);
//        ancestorOf(classificationID1, "500.100", -1);
//        ancestorOf(classificationID1, "500.101", -1);
//        ancestorOf(classificationID1, "500", 3);
//        ancestorOf(classificationID1, "500", 2);
//        ancestorOf(classificationID1, "500", 1);
//        ancestorOf(classificationID1, "500.100", 1);
        ancestorOf(classificationID1, "501", -1);
//        childOf(classificationID1, "500", -1);
//        childOf(classificationID1, "500", 1);
//        childOf(classificationID1, "500", 2);
//        childOf(classificationID1, "500.100", 1);
//       childOf(classificationID1, "600", -1);
//        childOf(classificationID1, "630", -1);
//        childOf(classificationID1, "721", 1);
//        childOf(classificationID1, "800", 1);
    }

    void test03() throws Exception {
        classificationExplorer = new ClassificationExplorer("config/classificationExplorer.properties",
                Const.NOT_DROPANDCREATETABLES);
        cleanDB();
        classificationExplorer.setTopParentId(topId1);
        classificationID1 = importClassification("classificationDewey.xml");
        //test1();
        String categoryID = "000";
        getCategoryItems(classificationID1, categoryID, Const.RECURSIVELY);
        getChildrenItems(classificationID1, categoryID, Const.RECURSIVELY);
        //getParents(classificationID1, categoryID, Const.RECURSIVELY);
    }

    void test02() throws Exception {
        classificationExplorer = new ClassificationExplorer("config/classificationExplorer.properties",
                Const.DROPANDCREATETABLES);
        cleanDB();
        classificationExplorer.setTopParentId(topId1);
//        classificationID1 = importClassification("t1.xml");
//        removeClassification("t1");
//        classificationID1 = importClassification("classificationDewey.xml");
        classificationID1 = importClassification("RVK-dummy.xml");
    }

    void test01() throws Exception {
//          classificationExplorer = new ClassificationExplorer(dropAndCreateTables);
        classificationExplorer = new ClassificationExplorer("config/classificationExplorer.properties",
                Const.DROPANDCREATETABLES);
        cleanDB();
        classificationExplorer.setTopParentId(topId1);
        classificationID1 = importClassification("classificationDewey.xml");
        classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        classificationID2 = importClassification("classification1.xml");
//        test1();
//        test2();
//        test3();
        test4();
//        test5();
//        removeClassification(classificationID2);
//        test1();
//        cleanDB();
//        test5();
    }

    private long memUsed() {
        for (int i = 0; i < 20; i++) {
            System.gc();
        }
        return Runtime.getRuntime().totalMemory() - Runtime.getRuntime().freeMemory();
    }

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {

        try {
            Test test = new Test();
            long mem1 = test.memUsed();
//           test.test01();
//            test.test02();
//            test.test03();
            test.test04();
            long memUsed = test.memUsed() - mem1;
            System.out.println("mem used: " + memUsed);
        } catch (Exception ex) {
            Logger.getLogger(ClassificationExplorer.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    private void cleanDB() throws Exception {
        System.out.println("--------------- cleanDB -------");
        classificationExplorer.cleanDB();
    }

    private void removeClassification(String classificationName) throws Exception {
        System.out.println("--------------- removeClassification -------");
        classificationExplorer.removeClassification(classificationName);
    }

    private String importClassification(String classificationFileName) throws Exception {
        System.out.println("--------------- testImportCategory -------");
        return classificationExplorer.importClassification(new File("xmlClassification/" + classificationFileName));
    }

    private void getTopLevelCategoryItems(String classificationName) {
        System.out.println("--------------- getTopLevelCategoryItems -------");
        List<Item> items = classificationExplorer.getTopLevelCategoryItems(classificationName);
        printListItemDB(items);
    }

    private void getTopLevelCategories(String classificationName) {
        System.out.println("--------------- getTopLevelCategories -------");
        List<String> topLevelCategories = classificationExplorer.getTopLevelCategories(classificationName);
        System.out.println(topLevelCategories);
    }

    private void getParents(String classificationName, String catID, boolean recursively) {
        System.out.println("--------------- getParents : " + catID + " recursively=" + recursively + " -------");
        List<String> parents = classificationExplorer.getParents(classificationName, catID, recursively);
        System.out.println(parents);
    }

    private void ancestorOf(String classificationName, String catID, int depth) {
        System.out.println("--------------- ancestorOf : " + catID + " -------");
        Item parent = classificationExplorer.ancestorOf(classificationName, catID, depth);
        System.out.println(parent);
    }

    private void getChildrenItems(String classificationName, String catID, boolean recursively) {
        System.out.println("--------------- getChildrenItems : " + catID + " recursively=" + recursively + " -------");
        List<Item> items = classificationExplorer.getChildrenItems(classificationName, catID, recursively);
        printListItemDB(items);
    }

    private void getChildren(String classificationName, String catID, boolean recursively) {
        System.out.println("--------------- getChildren : " + catID + " recursively=" + recursively + " -------");
        List<String> children = classificationExplorer.getChildren(classificationName, catID, recursively);
        System.out.println(children);
    }

    private void childOf(String classificationName, String catID, int depth) {
        System.out.println("--------------- childOf : " + catID + " -------");
        List<Item> child = classificationExplorer.childOf(classificationName, catID, depth);
        printListItemDB(child);
    }

    private void getCategoryItems(String classificationName, String catID, boolean recursively) {
        System.out.println("--------------- getCategoryItems : " + catID + " -------");
        List<Item> items = classificationExplorer.getCategoryItems(classificationName, catID, recursively);
        printListItemDB(items);
    }

    private void getCategoryLabels(String classificationName, String catID, boolean recursively) {
        System.out.println("--------------- getCategoryLabels : " + catID + " -------");
        List<String> categories = classificationExplorer.getCategoryLabels(classificationName, catID, recursively);
        System.out.println(categories);
    }

    private void printListItemDB(List<Item> items) {
        for (Item item : items) {
            System.out.println(item.toString());
        }
    }
}
