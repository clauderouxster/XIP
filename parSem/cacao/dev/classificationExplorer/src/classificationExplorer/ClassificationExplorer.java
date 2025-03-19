package classificationExplorer;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.IOException;
import java.util.List;
import java.util.Properties;
import java.util.Vector;
import javax.xml.stream.XMLInputFactory;
import javax.xml.stream.XMLStreamConstants;
import javax.xml.stream.XMLStreamReader;
import org.apache.log4j.BasicConfigurator;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;

/**
 *
 * @author grondeau
 */
public class ClassificationExplorer {

    private ManagerDB managerDB;
    private String topParentId;
    private String propertiesFileName;
    private Properties properties;
    private static final Logger logger = Logger.getLogger(classificationExplorer.ClassificationExplorer.class.getName());

    /**
     *
     * @param dropAndCreateTables
     */
    public ClassificationExplorer(boolean dropAndCreateTables) {
        properties = new Properties();
        init(dropAndCreateTables);
    }

    /**
     *
     * @param propertiesFileName
     * @param dropAndCreateTables
     */
    public ClassificationExplorer(String propertiesFileName, boolean dropAndCreateTables) {
        this.propertiesFileName = propertiesFileName;
        properties = new Properties();
        try {
            properties.load(new FileInputStream(propertiesFileName));
            if (properties.containsKey("log4j.properties")) {
                BasicConfigurator.resetConfiguration();
                PropertyConfigurator.configure(properties.getProperty("log4j.properties"));
                logger.info("You are using the following log4j.properties file: " + properties.getProperty("log4j.properties"));
            }
            logger.info("properties file used: " + propertiesFileName);
        } catch (IOException ex) {
            logger.fatal("Cant't read the classificationExplorer.properties file: " + propertiesFileName + "\n" + ex);
            System.exit(-1);
        }
        init(dropAndCreateTables);
    }

    private void init(boolean dropAndCreateTables) {
        setTopParentId("000");
        managerDB = new ManagerDB(properties, dropAndCreateTables);
    }

    /**
     * clean all the tables in the DB
     */
    public void cleanDB() {
        managerDB.cleanDB();
    }

    /**
     * delete a classification in the DB
     * @param classificationName
     */
    public void removeClassification(String classificationName) {
        managerDB.removeClassification(classificationName);
    }

//    /**
//     * Read a classification from the xmlFile and store the data in the database
//     *
//     * @param xmlFile the classification file (XML format)
//     * @return the classification name
//     * @throws java.lang.Exception
//     */
//    public String importClassification(File xmlFile) throws Exception {
//        logger.debug("importClassification()");
//        System.out.println("importClassification");
//        //requires jaxb.index file in package classificationExplorer
//        JAXBContext jc = JAXBContext.newInstance("classificationExplorer");
//        Unmarshaller unmarshaller = jc.createUnmarshaller();
//        classificationExplorer.importClassification.Classification impClassification;
//        System.out.println("start unmarshal");
//        impClassification = (classificationExplorer.importClassification.Classification) unmarshaller.unmarshal(xmlFile);
//        System.out.println("end unmarshal");
//        logger.debug("ClassificationName: " + impClassification.getName());
//        System.out.println("ClassificationName: " + impClassification.getName());
//        List<classificationExplorer.importClassification.Item> items = impClassification.getItem();
//        System.out.println("nb items: " + items.size());
//        managerDB.createClassification(impClassification.getName(), items, topParentId);
//        return impClassification.getName();
//    }
    /**
     * Read a classification from the xmlFile and store the data in the database
     *
     * @param xmlFile the classification file (XML format)
     * @return the classification name
     * @throws java.lang.Exception
     */
    public String importClassification(File xmlFile) throws Exception {
        logger.debug("importClassification() " + xmlFile);
        //long startTime = System.currentTimeMillis();
        XMLInputFactory xmlif = XMLInputFactory.newInstance();
        FileReader fileReader = new FileReader(xmlFile);
        BufferedReader bufferedReader = new BufferedReader(fileReader);
        XMLStreamReader xmlStreamReader = xmlif.createXMLStreamReader(bufferedReader);
        String classificationName = "";
        boolean isItemOpened = false;
        Vector<String> parentIds = new Vector<String>();
        int level = 0;
        String previousItemID = topParentId;
        parentIds.add(level, previousItemID);
        String itemId;
        String label;
        int eventType;
        String evtName;
        int depth;
        managerDB.createTransactionalEntityManager();
        int itemCnt = 0;
        while (xmlStreamReader.hasNext()) {
            eventType = xmlStreamReader.next();
            if (eventType == XMLStreamConstants.START_ELEMENT) {
                evtName = xmlStreamReader.getLocalName();
                if (evtName.equals("Classification")) {
                    if (xmlStreamReader.getAttributeLocalName(0).equals("name")) {
                        classificationName = xmlStreamReader.getAttributeValue(0);
                        managerDB.createClassification(classificationName);
                    }
                } else if (evtName.equals("item")) {
                    if (isItemOpened) {
                        level++;
                        parentIds.add(level, previousItemID);
                    }
                    isItemOpened = true;
                    itemId = "";
                    label = "";
                    depth = 0;
                    for (int i = 0; i < xmlStreamReader.getAttributeCount(); i++) {
                        if (xmlStreamReader.getAttributeLocalName(i).equals("id")) {
                            itemId = xmlStreamReader.getAttributeValue(i);
                            previousItemID = itemId;
                        } else if (xmlStreamReader.getAttributeLocalName(i).equals("label")) {
                            label = xmlStreamReader.getAttributeValue(i);
                        } else if (xmlStreamReader.getAttributeLocalName(i).equals("depth")) {
                            depth = Integer.parseInt(xmlStreamReader.getAttributeValue(i));
                        }
                    }
                    Item item = managerDB.addItemInClassification(itemId, label, depth, parentIds.get(level));
                    if (itemCnt++ > 10000) {
                        //flush the Items to the DB to avoid out of memory error
                        itemCnt = 0;
                        managerDB.closeTransactionalEntityManager();
                        managerDB.createTransactionalEntityManager();
                    }
                }
            } else if (eventType == XMLStreamConstants.END_ELEMENT) {
                if (!isItemOpened) {
                    parentIds.remove(level);
                    level--;
                }
                isItemOpened = false;
            }
        }
        managerDB.closeTransactionalEntityManager();
        //System.out.println("importClassification duration: " + (System.currentTimeMillis() - startTime));
        return classificationName;
    }

    /**
     * set the parentId value for the top classification element (by default it is "000")
     * @param topParentId
     */
    public void setTopParentId(String topParentId) {
        this.topParentId = topParentId;
    }

    /**
     * This method returns the list of top level categories for a given classification
     * @param classificationID a classification identifier
     * @return the top level categories
     */
    public List<Item> getTopLevelCategoryItems(String classificationID) {
        //long startTime = System.currentTimeMillis();
        List<Item> topLevelCategories = managerDB.getTopLevelCategoryItems(classificationID);
        //System.out.println("getTopLevelCategoryItems duration: " + (System.currentTimeMillis() - startTime));
        return topLevelCategories;
    }

    /**
     * This method returns the list of top level categories for a given classification
     * @param classificationID a classification identifier
     * @return the top level categories
     */
    public List<String> getTopLevelCategories(String classificationID) {
        //long startTime = System.currentTimeMillis();
        List<String> topLevelCategories = managerDB.getTopLevelCategories(classificationID);
        //System.out.println("getTopLevelCategories duration: " + (System.currentTimeMillis() - startTime));
        return topLevelCategories;
    }

    /**
     * This method returns the list of parent categories for a given categoryID,
     * within a specific classification.
     * @param classificationID a classification identifier
     * @param categoryID a category identifier
     * @param recursively
     * @return
     */
    public List<String> getParents(String classificationID, String categoryID, boolean recursively) {
        //long startTime = System.currentTimeMillis();
        List<String> parents = managerDB.getParents(classificationID, categoryID, recursively);
        //System.out.println("getParents duration: " + (System.currentTimeMillis() - startTime));
        return parents;
    }

    /**
     * the processing is close to getParents but it is different in the following cases:
     *     - take in account only the first 3 digits of the categoryID
     *     - if there is more than one items with the same categoryID, then the item with the higher depth value is returned
     *     - if no items can be found with the categoryID, then some digits of the categoryID
     *          are removed (521->520->500->top) until one item will be found and returned.
     * @param classificationID
     * @param categoryID
     * @param depth if depth == -1 then it is not take in account
     * @return
     */
    public Item ancestorOf(String classificationID, String categoryID, int depth) {
        //long startTime = System.currentTimeMillis();
        Item parent = managerDB.ancestorOf(classificationID, categoryID, depth);
        //System.out.println("getParents duration: " + (System.currentTimeMillis() - startTime));
        return parent;
    }

    /**
     * This method returns the list of children for a given categoryID,
     * within a specific classification.
     * @param classificationID a classification identifier
     * @param categoryID a category identifier
     * @param recursively
     * @return
     */
    public List<Item> getChildrenItems(String classificationID, String categoryID, boolean recursively) {
        //long startTime = System.currentTimeMillis();
        List<Item> children = managerDB.getChildrenItems(classificationID, categoryID, recursively);
        //System.out.println("getChildrenItems duration: " + (System.currentTimeMillis() - startTime));
        return children;
    }

    /**
     * This method returns the list of children for a given categoryID,
     * within a specific classification.
     * @param classificationID a classification identifier
     * @param categoryID a category identifier
     * @param recursively
     * @return
     */
    public List<String> getChildren(String classificationID, String categoryID, boolean recursively) {
        //long startTime = System.currentTimeMillis();
        List<String> children = managerDB.getChildren(classificationID, categoryID, recursively);
        //System.out.println("getChildren duration: " + (System.currentTimeMillis() - startTime));
        return children;
    }

    /**
     *
     * @param classificationID
     * @param categoryID
     * @param depth if depth == -1 then it is not take in account
     * @return
     */
    public List<Item> childOf(String classificationID, String categoryID, int depth) {
        //long startTime = System.currentTimeMillis();
        List<Item> child = managerDB.childOf(classificationID, categoryID, depth);
        //System.out.println("getParents duration: " + (System.currentTimeMillis() - startTime));
        return child;
    }

    /**
     * This method returns the list of the categories for a given categoryID in the hierarchy,
     * within a specific classification.
     * @param classificationID a classification identifier
     * @param categoryID a category identifier
     * @param recursively
     * @return
     */
    public List<Item> getCategoryItems(String classificationID, String categoryID, boolean recursively) {
        //long startTime = System.currentTimeMillis();
        List<Item> categories = managerDB.getCategoryItems(classificationID, categoryID, recursively);
        //System.out.println("getCategoryItems duration: " + (System.currentTimeMillis() - startTime));
        return categories;
    }

    /**
     * This method returns the list of the labels of a given categoryID in the hierarchy,
     * within a specific classification.
     * @param classificationID a classification identifier
     * @param categoryID a category identifier
     * @param recursively 
     * @return
     */
    public List<String> getCategoryLabels(String classificationID, String categoryID, boolean recursively) {
        //long startTime = System.currentTimeMillis();
        List<String> categoryLabels = managerDB.getCategoryLabels(classificationID, categoryID, recursively);
        //System.out.println("getCategoryLabels duration: " + (System.currentTimeMillis() - startTime));
        return categoryLabels;
    }
}
