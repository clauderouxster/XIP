/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package eu.project.Cacao.Apps;

import java.io.File;
import java.util.List;

import org.apache.log4j.Logger;

import classificationExplorer.ClassificationExplorer;
import classificationExplorer.Item;

/**
 *
 * @author Grondeau
 */
public class ClassificationExplorerTest {

    private ClassificationExplorer classificationExplorer;
    private String classificationName;
    private String importPath;
    private static final Logger logger = Logger.getLogger(ClassificationExplorerTest.class.getName());

    public ClassificationExplorerTest() {
        classificationName = "Dewey";
        boolean dropAndCreateTables = true;
        // use default configuration (all the configuration values are defined inside the jar file)
        //classificationExplorer = new ClassificationExplorer(dropAndCreateTables);
        // use your own configuration
        classificationExplorer = new ClassificationExplorer("configuration/classificationExplorer.properties", dropAndCreateTables);
    }

    void test1() {
        getTopLevelCategoryItems(classificationName);
        getTopLevelCategories(classificationName);
    }

    void test2() {
        getCategoryItems(classificationName, "500");
        getParents(classificationName, "500");
        getCategoryItems(classificationName, "0");
        getParents(classificationName, "0");
    }

    void test3() {
        getChildrenItems(classificationName, "000");
        getChildren(classificationName, "000");
        getChildrenItems(classificationName, "100");
        getChildren(classificationName, "100");
        getChildrenItems(classificationName, "600");
        getChildren(classificationName, "600");
    }

    void test4() {
        getCategoryItems(classificationName, "500");
        getCategoryLabels(classificationName, "500");
        getCategoryItems(classificationName, "200");
        getCategoryLabels(classificationName, "200");
        getCategoryItems(classificationName, "800");
        getCategoryLabels(classificationName, "800");
    }

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        try {
            ClassificationExplorerTest test = new ClassificationExplorerTest();
            test.importPath = "data/classificationExplorer/xmlClassification";
            test.cleanDB();
            test.importClassification("classificationDewey.xml");
            //test.importClassification("classification1.xml");
            //test.removeClassification("Dewey");
            test.test1();
            test.test2();
            test.test3();
            test.test4();
        } catch (Exception ex) {
            logger.fatal(ex.toString());
            System.exit(-1);
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

    private void importClassification(String classificationFileName) throws Exception {
        System.out.println("--------------- testImportCategory -------");
        classificationExplorer.importClassification(new File(importPath + "/" + classificationFileName));
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

    private void getParents(String classificationName, String catID) {
        System.out.println("--------------- getParents : " + catID + " -------");
        List<String> parents = classificationExplorer.getParents(classificationName, catID);
        System.out.println(parents);
    }

    private void getChildrenItems(String classificationName, String catID) {
        System.out.println("--------------- getChildrenItems : " + catID + " -------");
        List<Item> items = classificationExplorer.getChildrenItems(classificationName, catID);
        printListItemDB(items);
    }

    private void getChildren(String classificationName, String catID) {
        System.out.println("--------------- getChildren : " + catID + " -------");
        List<String> children = classificationExplorer.getChildren(classificationName, catID);
        System.out.println(children);
    }

    private void getCategoryItems(String classificationName, String catID) {
        System.out.println("--------------- getCategoryItems : " + catID + " -------");
        List<Item> items = classificationExplorer.getCategoryItems(classificationName, catID);
        printListItemDB(items);
    }

    private void getCategoryLabels(String classificationName, String catID) {
        System.out.println("--------------- getCategoryLabels : " + catID + " -------");
        List<String> categories = classificationExplorer.getCategoryLabels(classificationName, catID);
        System.out.println(categories);
    }

    private void printListItemDB(List<Item> items) {
        for (Item item : items) {
            System.out.println(item);
        //System.out.println("itemId = " + item.getItemId() + " parentId = " + item.getParentId() + " depth = " + item.getDepth() + " "+ item.getLabel());
        }
    }
}
