package classificationExplorer;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Properties;
import javax.persistence.*;
import oracle.toplink.essentials.config.TopLinkProperties;
import oracle.toplink.essentials.ejb.cmp3.EntityManagerFactoryProvider;
import org.apache.log4j.Logger;

/**
 *
 * @author Gilbert
 */
public class ManagerDB {

    private EntityManagerFactory emf;
    private EntityManager em;
    private Classification classification;
    private HashMap<String, String> propertiesMap;
    private static final Logger logger = Logger.getLogger(classificationExplorer.ManagerDB.class.getName());

    /**
     * 
     * @param properties
     * @param dropAndCreateTables if true drop and create tables
     */
    public ManagerDB(Properties properties, boolean dropAndCreateTables) {
        //logger.debug("ManagerDB constructor");
        propertiesMap = new HashMap<String, String>();
        String default_jdbc_url = "";
        String default_jdbc_driver = "";
        String default_target_database = "";
        String default_jdbc_user = "";
        String default_jdbc_password = "";
        String jdbc_url, jdbc_driver, target_database, jdbc_user, jdbc_password, logging_level;
        boolean derby = false;
        boolean hsqldb = false;
        boolean h2 = true;
        boolean embedded = true;
        // create the dir where the DB files will be stored. By default dbPath = "db"
        String dbPath = dbPath = properties.getProperty("dbPath", "db");
        File file = new File(dbPath);
        if (!file.isDirectory()) {
            file.mkdir();
        }
        logger.info("DB files will be store in: " + dbPath);
        //set default value for DB connection
        if (derby) {
            //derby DB
            if (embedded) {
                default_jdbc_url = "jdbc:derby:" + Const.dbName + ";create=true";
                default_jdbc_driver = "org.apache.derby.jdbc.EmbeddedDriver";
            } else {
                default_jdbc_url = "jdbc:derby://localhost:1527/" + Const.dbName;
                default_jdbc_driver = "org.apache.derby.jdbc.ClientDriver";
            }
            //http://www.oracle.com/technology/products/ias/toplink/JPA/essentials/toplink-jpa-extensions.html#TopLinkDBSessionAppServer
            default_target_database = "Derby";
            default_jdbc_user = "root";
            default_jdbc_password = "root";
            // set the dir where the DB files will be stored. By default dbPath = "db"
            System.setProperty("derby.system.home", dbPath);
        } else if (hsqldb) {
            //http://hsqldb.org/doc/guide/ch04.html
            if (embedded) {
                default_jdbc_url = "jdbc:hsqldb:file:" + Const.dbName;
            } else {
                //default port = 9001
                default_jdbc_url = "jdbc:hsqldb:hsql://localhost/" + Const.dbName;
            }
            default_jdbc_driver = "org.hsqldb.jdbcDriver";
            default_target_database = "HSQL";
            default_jdbc_user = "sa";
            default_jdbc_password = "";
        } else if (h2) {
            if (embedded) {
                //String options=";AUTO_SERVER=TRUE"; //ne marche pas
                String options = "";
                default_jdbc_url = "jdbc:h2:file:" + "./" + dbPath + "/" + Const.dbName + options;
            } else {
                String options = "";
                default_jdbc_url = "jdbc:h2:tcp://localhost/" + Const.dbName + options;
            }
            default_jdbc_driver = "org.h2.Driver";
            default_target_database = "oracle.toplink.essentials.platform.database.H2Platform";
            default_jdbc_user = "sa";
            default_jdbc_password = "";
        } else {
            // mysql
            default_jdbc_url = "jdbc:mysql://salses:3306/" + Const.dbName;
            default_jdbc_driver = "com.mysql.jdbc.Driver";
            default_target_database = "Auto";
            default_jdbc_user = "root";
            default_jdbc_password = "root";
        }
        // get properies values defined in classificationExplorer.properties or use default value
        jdbc_url = properties.getProperty("toplink.jdbc.url", default_jdbc_url);
        jdbc_driver = properties.getProperty("toplink.jdbc.driver", default_jdbc_driver);
        target_database = properties.getProperty("toplink.target-database", default_target_database);
        jdbc_user = properties.getProperty("toplink.jdbc.user", default_jdbc_user);
        jdbc_password = properties.getProperty("toplink.jdbc.password", default_jdbc_password);
        String logLevel = "OFF";
        //String logLevel = "FINE";
        //String logLevel = "INFO";
        logging_level = properties.getProperty("toplink.logging.level", logLevel);
        //init TopLink
        propertiesMap.put(TopLinkProperties.JDBC_URL, jdbc_url);
        propertiesMap.put(TopLinkProperties.JDBC_DRIVER, jdbc_driver);
        propertiesMap.put(TopLinkProperties.TARGET_DATABASE, target_database);
        propertiesMap.put(TopLinkProperties.JDBC_USER, jdbc_user);
        propertiesMap.put(TopLinkProperties.JDBC_PASSWORD, jdbc_password);
        propertiesMap.put(TopLinkProperties.LOGGING_LEVEL, logging_level);
        logger.info("using the following jdbc.url: " + propertiesMap.get(TopLinkProperties.JDBC_URL));
        logger.info("using the following jdbc.driver: " + propertiesMap.get(TopLinkProperties.JDBC_DRIVER));
        logger.info("using the following target-database: " + propertiesMap.get(TopLinkProperties.TARGET_DATABASE));
        logger.info("using the following logging.level: " + propertiesMap.get(TopLinkProperties.LOGGING_LEVEL));
        if (dropAndCreateTables) {
            logger.info("DB tables will be dropped and created");
            //http://www.oracle.com/technology/products/ias/toplink/JPA/essentials/toplink-jpa-extensions.html
            // specify where dropDDL.jdbc and createDDl.jdbc whill be created
            propertiesMap.put(EntityManagerFactoryProvider.APP_LOCATION, dbPath);
            // ask to generate dropDDL.jdbc and createDDl.jdbc
            propertiesMap.put(EntityManagerFactoryProvider.DDL_GENERATION, EntityManagerFactoryProvider.DROP_AND_CREATE);
            // generate dropDDL.jdbc and createDDl.jdbc files and execute them on the database
            propertiesMap.put(EntityManagerFactoryProvider.DDL_GENERATION_MODE, EntityManagerFactoryProvider.DDL_BOTH_GENERATION);
        } else {
            logger.info("DB tables will be created if they do not exist");
            // specify where createDDl.jdbc whill be created
            propertiesMap.put(EntityManagerFactoryProvider.APP_LOCATION, dbPath);
            // generate createDDl.jdbc for non-existent tables; leave existing tables unchanged
            propertiesMap.put(EntityManagerFactoryProvider.DDL_GENERATION, EntityManagerFactoryProvider.CREATE_ONLY);
            // generate createDDl.jdbc files and execute it on the database if DB tables don't exist
            propertiesMap.put(EntityManagerFactoryProvider.DDL_GENERATION_MODE, EntityManagerFactoryProvider.DDL_BOTH_GENERATION);
        }
        try {
            String persitenceUnitName = "default";
            emf = Persistence.createEntityManagerFactory(persitenceUnitName, propertiesMap);
            // just to create (and drop) the table if necessary
            em = emf.createEntityManager();
            em.close();
        } catch (Exception Ex) {
            System.out.println(Ex.toString());
            System.exit(-1);
        }
    }

    /**
     * clean the DB Tables
     */
    protected void cleanDB() {
        logger.debug("cleanDB()");
        createTransactionalEntityManager();
        try {
            em.createNativeQuery("delete from " + Const.itemTableName).executeUpdate();
        } catch (Exception ex) {
            logger.warn(ex.getCause());
        }
        try {
            em.createNativeQuery("delete from " + Const.classificationTableName).executeUpdate();
        } catch (Exception ex) {
            logger.warn(ex.getCause());
        }
        closeTransactionalEntityManager();
    }

    /**
     * remove the items of the classification identified by classificationName
     * @param classificationName the classification name
     */
    protected void removeClassification(String classificationName) {
        logger.debug("removeClassification() " + classificationName);
        //System.out.println("removeClassification() " + classificationName);
        createTransactionalEntityManager();
        //get classification_id
        String queryStr = "SELECT classification FROM Classification classification";
        queryStr += " WHERE classification.name= :classifName ";
        Query query = em.createQuery(queryStr);
        query.setParameter("classifName", classificationName);
        classification = (Classification) query.getSingleResult();
        // remove all the item from this Classification
        queryStr = "DELETE FROM CLASSIFICATIONEXPLORER_ITEM";
        queryStr += " WHERE classification_id = " + classification.getId();
        query = em.createNativeQuery(queryStr);
        query.executeUpdate();
        //remove Classification in the DB
        queryStr = "DELETE FROM Classification classification";
        queryStr += " WHERE classification.name= :classifName ";
        query = em.createQuery(queryStr);
        query.setParameter("classifName", classificationName);
        query.executeUpdate();
        closeTransactionalEntityManager();
    }

    /**
     *
     * @param classificationName
     */
    protected void createClassification(String classificationName) {
        logger.debug("createClassification() " + classificationName);
        classification = new Classification();
        classification.setName(classificationName);
        em.persist(classification);
    }

    /**
     *
     * @param itemId
     * @param label
     * @param depth
     * @param parentId
     * @return
     */
    protected Item addItemInClassification(String itemId, String label, int depth, String parentId) {
//        logger.debug("addItemInClassification() " + itemId + " " + label + " " + depth + " " + parentId);
        Item item = new Item();
        item.setItemId(itemId);
        item.setLabel(label);
        item.setDepth(depth);
        item.setParentId(parentId);
        item.setClassification(classification);
        em.persist(item);
        return item;
    }

    /**
     *
     * @param classificationID
     * @return
     */
    protected List<Item> getTopLevelCategoryItems(String classificationID) {
        logger.debug("getTopLevelCategoryItems() " + classificationID);
        em = emf.createEntityManager();
        String queryStr = "SELECT item FROM Item item";
        queryStr += " JOIN item.classification classification";
        queryStr += " WHERE item.depth='1' AND classification.name= :classificationName";
        queryStr += " ORDER BY item.itemId";
        Query query = em.createQuery(queryStr);
        query.setParameter("classificationName", classificationID);
        @SuppressWarnings("unchecked")
        List<Item> categories = query.getResultList();
        em.close();
        return categories;
    }

    /**
     *
     * @param classificationID
     * @return
     */
    protected List<String> getTopLevelCategories(String classificationID) {
        logger.debug("getTopLevelCategories() " + classificationID);
        List<Item> items = getTopLevelCategoryItems(classificationID);
        List<String> topLevelCategories = new ArrayList<String>();
        for (Item item : items) {
            topLevelCategories.add(item.getItemId());
        }
        return topLevelCategories;
    }

    /**
     *
     * @param classificationID
     * @param categoryID
     * @param recursively
     * @return
     */
    protected List<Item> getCategoryItems(String classificationID, String categoryID, boolean recursively) {
        logger.debug("getCategoryItems() " + classificationID + " " + categoryID + " " + recursively);
        return getCategoryItemsInt(classificationID, categoryID, recursively, false);
    }

    /**
     *
     * @param classificationID
     * @param categoryID
     * @param recursively
     * @param max
     * @return
     */
    protected List<Item> getCategoryItemsInt(String classificationID, String categoryID, boolean recursively, boolean max) {
        logger.debug("getCategoryItems() " + classificationID + " " + categoryID + " " + recursively + " " + max);
        em = emf.createEntityManager();
        Integer depthValue = 0;
        String queryStr = "SELECT item FROM Item item";
        queryStr += " JOIN item.classification classification";
        queryStr += " WHERE item.itemId = :categoryName AND classification.name= :classificationName";
        if (recursively) {
            queryStr += " ORDER BY item.itemId, item.depth";
        } else {
            // compute the depthValue: take the lower value if max == false otherwise take the higher value
            queryStr += " AND item.depth = :depthValue";
            queryStr += " ORDER BY item.itemId";
            // compute depthValue: the lower value of the item.depth values
            String op = "";
            if (max) {
                op = "MAX";
            } else {
                op = "MIN";
            }
            String queryStrMin = "SELECT " + op + " (item.depth) FROM Item item";
            queryStrMin += " JOIN item.classification classification";
            queryStrMin += " WHERE item.itemId = :categoryName AND classification.name= :classificationName";
            Query queryMin = em.createQuery(queryStrMin);
            queryMin.setParameter("categoryName", categoryID);
            queryMin.setParameter("classificationName", classificationID);
            depthValue = (Integer) queryMin.getSingleResult();
        }
        Query query = em.createQuery(queryStr);
        if (!recursively) {
            // return the item with the lower or higher (see max) value of depth
            // in the case where there are items with the same categoryID
            query.setParameter("depthValue", depthValue);
        }
        query.setParameter("categoryName", categoryID);
        query.setParameter("classificationName", classificationID);
        List<Item> categories = query.getResultList();
        em.close();
        return categories;
    }

    /**
     *
     * @param classificationID
     * @param categoryID
     * @param recursively
     * @return
     */
    protected List<String> getCategoryLabels(String classificationID, String categoryID, boolean recursively) {
        logger.debug("getCategoryLabels() " + classificationID + " " + categoryID);
        List<Item> items = getCategoryItems(classificationID, categoryID, recursively);
        List<String> labels = new ArrayList<String>();
        for (Item item : items) {
            labels.add(item.getLabel());
        }
        return labels;
    }

    /**
     *
     * @param classificationID
     * @param categoryID
     * @param recursively
     * @return
     */
    protected List<String> getParents(String classificationID, String categoryID, boolean recursively) {
        logger.debug("getParents() " + classificationID + " " + categoryID + " " + recursively);
        List<Item> items = getCategoryItems(classificationID, categoryID, recursively);
        List<String> parents = new ArrayList<String>();
        for (Item item : items) {
            parents.add(item.getParentId());
        }
        return parents;
    }

    /**
     *
     * @param classificationID
     * @param categoryID
     * @param recursively
     * @param max
     * @return
     */
    protected List<String> getParentsInt(String classificationID, String categoryID, boolean recursively, boolean max) {
        logger.debug("getParentsInt() " + classificationID + " " + categoryID + " " + recursively + " " + max);
        List<Item> items = getCategoryItemsInt(classificationID, categoryID, recursively, max);
        List<String> parents = new ArrayList<String>();
        for (Item item : items) {
            parents.add(item.getParentId());
        }
        return parents;
    }

    /**
     * the processing is close to getParents but it is different in the following cases:
     *     - take in account only the first 3 digits of the categoryID
     *     - if there is more than one items with the same categoryID, then the item with the higher depth value is returned
     *     - if no items can be found with the categoryID, then some digits of the categoryID are removed: 521->520->500->top
     * @param classificationID
     * @param categoryID
     * @param depth if depth == -1 then it is not take in account
     * @return
     */
    protected Item ancestorOf(String classificationID, String categoryID, int depth) {
        //System.out.println("------------------- ancestorOf() " + classificationID + " " + categoryID + " " + depth);
        //process the case where getParents return only one item
        List<String> parents = getParentsInt(classificationID, categoryID, true, true);
        //System.out.println(parents);
        if (parents != null) {
            if (parents.size() == 1) {
                List<Item> items = getCategoryItemsInt(classificationID, parents.get(0), false, true);
                return items.get(0);
            }
        }
        //process the case where getParents return more than one item or null
        String itemId = "";
        //take in account only the first 3 digits
        if (categoryID.length() > 3) {
            itemId = categoryID.substring(0, 3);
        } else {
            itemId = categoryID;
        }
        em = emf.createEntityManager();
        Integer depthValue = 0;
        Item item = null;
        //depthValue must be computed
        boolean cont = true;
        int count = 3;
        if (depth == -1) {
            while (cont) {
                // compute depthValue: the higher value of the item.depth values
                String queryStrMin = "SELECT MAX (item.depth) FROM Item item";
                queryStrMin += " JOIN item.classification classification";
                queryStrMin += " WHERE item.itemId = :categoryName AND classification.name= :classificationName";
                Query queryMin = em.createQuery(queryStrMin);
                queryMin.setParameter("categoryName", itemId);
                queryMin.setParameter("classificationName", classificationID);
                depthValue = (Integer) queryMin.getSingleResult();
                //System.out.println("depthValue = " + depthValue + " depth = " + depth);
                if (depthValue != null) {
                    break;
                } else {
                    //go up in the hierarchie: remove digit (521->520->500->000)
                    count--;
                    if (count == -1) {
                        break;
                    }
                    char[] tchar = itemId.toCharArray();
                    tchar[count] = '0';
                    itemId = new String(tchar);
                    //System.out.println("itemId = " + itemId);
                }
            }
        } else {
            boolean useDepth = true;
            while (cont) {
                // compute depthValue: the higher value of the item.depth values but with item.depth <depth
                String queryStrMin = "SELECT MAX (item.depth) FROM Item item";
                queryStrMin += " JOIN item.classification classification";
                queryStrMin += " WHERE item.itemId = :categoryName AND classification.name= :classificationName";
                if (useDepth) {
                    queryStrMin += " AND item.depth < :depth";
                }
                Query queryMin = em.createQuery(queryStrMin);
                queryMin.setParameter("categoryName", itemId);
                queryMin.setParameter("classificationName", classificationID);
                if (useDepth) {
                    queryMin.setParameter("depth", depth);
                }
                depthValue = (Integer) queryMin.getSingleResult();
                //System.out.println("depthValue = " + depthValue + " depth = " + depth);
                if (depthValue != null) {
                    break;
                } else {
                    //go up in the hierarchie: remove digit (521->520->500->000)
                    count--;
                    if (count == -1) {
                        break;
                    }
                    String prevItemId = itemId;
                    char[] tchar = itemId.toCharArray();
                    tchar[count] = '0';
                    itemId = new String(tchar);
                    if (!itemId.equals(prevItemId)) {
                        useDepth = false;
                    }
                    //System.out.println("itemId = " + itemId);
                }
            }
        }
        if (depthValue != null) {
            String queryStr = "SELECT item FROM Item item";
            queryStr += " JOIN item.classification classification";
            queryStr += " WHERE item.itemId = :categoryName AND classification.name= :classificationName";
            queryStr += " AND item.depth = :depthValue";
            queryStr += " ORDER BY item.itemId";
            Query query = em.createQuery(queryStr);
            query.setParameter("depthValue", depthValue);
            query.setParameter("categoryName", itemId);
            query.setParameter("classificationName", classificationID);
            List<Item> items = query.getResultList();
            item = items.get(0);
        }
        //System.out.println("item = " + item);
        em.close();
        return item;
    }

    /**
     *
     * @param classificationID
     * @param categoryID
     * @param recursively
     * @return
     */
    protected List<Item> getChildrenItems(String classificationID, String categoryID, boolean recursively) {
        logger.debug("getChildrenItems() " + classificationID + " " + categoryID + " " + recursively);
        em = emf.createEntityManager();
        Integer depthValue = 0;
        String queryStr = "SELECT item FROM Item item";
        queryStr += " JOIN item.classification classification";
        queryStr += " WHERE item.parentId = :categoryName AND classification.name= :classificationName";
        if (recursively) {
            queryStr += " ORDER BY item.itemId, item.depth";
        } else {
            queryStr += " AND item.depth = :depthValue";
            queryStr += " ORDER BY item.itemId";
            // compute depthValue: the lower value of the item.depth values
            String queryStrMin = "SELECT MIN (item.depth) FROM Item item";
            queryStrMin += " JOIN item.classification classification";
            queryStrMin += " WHERE item.parentId = :categoryName AND classification.name= :classificationName";
            Query queryMin = em.createQuery(queryStrMin);
            queryMin.setParameter("categoryName", categoryID);
            queryMin.setParameter("classificationName", classificationID);
            depthValue = (Integer) queryMin.getSingleResult();
        }
        Query query = em.createQuery(queryStr);
        if (!recursively) {
            query.setParameter("depthValue", depthValue);
        }
        query.setParameter("categoryName", categoryID);
        query.setParameter("classificationName", classificationID);
        @SuppressWarnings("unchecked")
        List<Item> children = query.getResultList();
        em.close();
        return children;
    }

    /**
     *
     * @param classificationID
     * @param categoryID
     * @param recursively
     * @return
     */
    protected List<String> getChildren(String classificationID, String categoryID, boolean recursively) {
        logger.debug("getChildren() " + classificationID + " " + categoryID + " " + recursively);
        List<Item> items = getChildrenItems(classificationID, categoryID, recursively);
        List<String> children = new ArrayList<String>();
        for (Item item : items) {
            children.add(item.getItemId());
        }
        return children;
    }

    /**
     *
     * @param classificationID
     * @param categoryID
     * @param depth
     * @return
     */
    protected List<Item> childOf(String classificationID, String categoryID, int depth) {
//       System.out.println("------------------- childOf() " + classificationID + " " + categoryID + " " + depth);
//        //process the case where getChildrenItems return no item or null
        String itemId = "";
//        //take in account only the first 3 digits
//        if (categoryID.length() > 3) {
//            itemId = categoryID.substring(0, 3);
//        } else {
//            itemId = categoryID;
//        }
        List<Item> child = new ArrayList<Item>();
        boolean cont = true;
        int count = 3;
        if (depth == -1) {
            child = getChildrenItems(classificationID, categoryID, false);
            if (child != null) {
                if (child.size() != 0) {
                    return child;
                }
            }
//            while (cont) {
//                child = getChildrenItems(classificationID, itemId, false);
//                if (child != null) {
//                    if (child.size() != 0) {
//                        break;
//                    }
//                }
//                //go up in the hierarchie: remove digit (521->520->500->000)
//                count--;
//                if (count == -1) {
//                    child = new ArrayList<Item>();
//                    break;
//                }
//                char[] tchar = itemId.toCharArray();
//                tchar[count] = '0';
//                itemId = new String(tchar);
//                System.out.println("itemId = " + itemId);
//            }
        } else {
            em = emf.createEntityManager();
            String queryStr = "SELECT item FROM Item item";
            queryStr += " JOIN item.classification classification";
            queryStr += " WHERE item.parentId = :categoryName AND classification.name= :classificationName";
            queryStr += " AND item.depth = :depth";
            queryStr += " ORDER BY item.itemId";
            Query query = em.createQuery(queryStr);
            query.setParameter("depth", depth + 1);
            query.setParameter("categoryName", categoryID);
            query.setParameter("classificationName", classificationID);
            child = query.getResultList();
            if (child == null) {
                child = new ArrayList<Item>();
            }
            em.close();
        }
        //     System.out.println("child = " + child);
        return child;
    }

    /**
     *
     */
    protected void createTransactionalEntityManager() {
//        logger.debug("createTransactionalEntityManager()");
        em = emf.createEntityManager();
        em.getTransaction().begin();
    }

    /**
     *
     */
    protected void closeTransactionalEntityManager() {
//        logger.debug("closeTransactionalEntityManager()");
        em.getTransaction().commit();
        em.close();
    }
}
