package classificationExplorer;

import java.io.File;
import java.util.List;

public class Test {

    private ClassificationExplorer classificationExplorer;
    private String classificationID1;
    private String classificationID2;
    private static final String topId1 = "topId1";
    private static final String topId2 = "topId2";

    void test1() throws Exception {
//        classificationExplorer = new ClassificationExplorer(Const.NOT_DROPANDCREATETABLES);
        classificationExplorer = new ClassificationExplorer("../config/classificationExplorer.properties",
                Const.DROPANDCREATETABLES);
        cleanDB();
        classificationExplorer.setTopParentId(topId1);
        classificationID1 = importClassification("../xmlClassification/classificationDewey.xml");
        classificationExplorer.setTopParentId(topId2);
        classificationID2 = importClassification("../xmlClassification/classification1.xml");

        getTopLevelCategoryItems(classificationID1);
        getTopLevelCategories(classificationID1);
        getTopLevelCategories(classificationID2);

        getCategoryItems(classificationID1, "500", Const.RECURSIVELY);
        getCategoryItems(classificationID1, "500", Const.NOT_RECURSIVELY);
        getCategoryLabels(classificationID1, "500", Const.RECURSIVELY);
        getCategoryLabels(classificationID1, "500", Const.NOT_RECURSIVELY);
        getParents(classificationID1, "500", Const.RECURSIVELY);
        getParents(classificationID1, "500", Const.NOT_RECURSIVELY);

        getChildrenItems(classificationID1, "500", Const.RECURSIVELY);
        getChildrenItems(classificationID1, "500", Const.NOT_RECURSIVELY);
        getChildren(classificationID1, "500", Const.RECURSIVELY);
        getChildren(classificationID1, "500", Const.NOT_RECURSIVELY);

        removeClassification(classificationID1);
        getTopLevelCategories(classificationID1);
        getTopLevelCategories(classificationID2);
    }

    /**
     * 
     * @param args
     * @throws java.lang.Exception
     */
    public static void main(String[] args) throws Exception {
        Test test = new Test();
        test.test1();
    }

    private void cleanDB() throws Exception {
        System.out.println("--------------- cleanDB -------");
        classificationExplorer.cleanDB();
    }

    private void removeClassification(String classificationName) throws Exception {
        System.out.println("--------------- removeClassification: " + classificationName + " -------");
        classificationExplorer.removeClassification(classificationName);
    }

    private String importClassification(String classificationFileName) throws Exception {
        System.out.println("--------------- testImportCategory: " + classificationFileName + " -------");
        return classificationExplorer.importClassification(new File(classificationFileName));
    }

    private void getTopLevelCategoryItems(String classificationName) {
        System.out.println("--------------- getTopLevelCategoryItems: " + classificationName + " -------");
        List<Item> items = classificationExplorer.getTopLevelCategoryItems(classificationName);
        printListItemDB(items);
    }

    private void getTopLevelCategories(String classificationName) {
        System.out.println("--------------- getTopLevelCategories: " + classificationName + " -------");
        List<String> topLevelCategories = classificationExplorer.getTopLevelCategories(classificationName);
        System.out.println(topLevelCategories);
    }

    private void getParents(String classificationName, String catID, boolean recursively) {
        System.out.println("--------------- getParents : " + classificationName + " " +
                catID + " recursively=" + recursively + " -------");
        List<String> parents = classificationExplorer.getParents(classificationName, catID, recursively);
        System.out.println(parents);
    }

    private void getChildrenItems(String classificationName, String catID, boolean recursively) {
        System.out.println("--------------- getChildrenItems : " + classificationName + " " +
                catID + " recursively=" + recursively + " -------");
        List<Item> items = classificationExplorer.getChildrenItems(classificationName, catID, recursively);
        printListItemDB(items);
    }

    private void getChildren(String classificationName, String catID, boolean recursively) {
        System.out.println("--------------- getChildren : " + classificationName + " " + catID +
                " recursively=" + recursively + " -------");
        List<String> children = classificationExplorer.getChildren(classificationName, catID, recursively);
        System.out.println(children);
    }

    private void getCategoryItems(String classificationName, String catID, boolean recursively) {
        System.out.println("--------------- getCategoryItems : " + classificationName + " " + catID +
                " recursively=" + recursively + " -------");
        List<Item> items = classificationExplorer.getCategoryItems(classificationName, catID, recursively);
        printListItemDB(items);
    }

    private void getCategoryLabels(String classificationName, String catID, boolean recursively) {
        System.out.println("--------------- getCategoryLabels : " + classificationName + " " + catID +
                " recursively=" + recursively + " -------");
        List<String> categories = classificationExplorer.getCategoryLabels(classificationName, catID, recursively);
        System.out.println(categories);
    }

    private void printListItemDB(List<Item> items) {
        for (Item item : items) {
            System.out.println(item);
        }
    }
}
