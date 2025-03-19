package db_testsimple_2;

import java.io.File;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Properties;
import javax.persistence.*;

import oracle.toplink.essentials.config.TopLinkProperties;
import oracle.toplink.essentials.ejb.cmp3.EntityManagerFactoryProvider;
import org.apache.log4j.Logger;

/**
 *
 * @author ctrojahn
 */
public class ManagerDB {
    
    private EntityManagerFactory emf;
    private EntityManager em;
    private HashMap<String, String> propertiesMap;
    private static final Logger logger = Logger.getLogger(ManagerDB.class.getName());
    
    /**
     *
     * @param properties
     * @param dropAndCreateTables
     */
    public ManagerDB(Properties properties, boolean dropAndCreateTables) {

          System.out.println("ManagerDB constructor");
          propertiesMap = new HashMap<String, String>();
          String default_jdbc_url = "";
          String default_jdbc_driver = "";
          String default_target_database = "";
          String default_jdbc_user = "";
          String default_jdbc_password = "";
          String jdbc_url, jdbc_driver, target_database, jdbc_user, jdbc_password, logging_level;
          boolean h2 = true;
          boolean embedded = false;

          String dbPath ="/opt/ParSem/users/cassiat/parSem/cacao/dev/advUI/libExt/h2/db";
          File file = new File(dbPath);
          if (!file.isDirectory()) {
               file.mkdir();
          }
          

          if (h2) {
            if (embedded) { // ok with ;MVCC=TRUE
                default_jdbc_url = "jdbc:h2:file:"  + dbPath + "/" + Constant.dbName +  ";MULTI_THREADED=1;LOCK_TIMEOUT=2000;AUTO_SERVER=TRUE";
                  System.out.println("CREATED MANAGER 1");
            } else {
                default_jdbc_url = "jdbc:h2:tcp://localhost/" + Constant.dbName + ";MULTI_THREADED=1;LOCK_TIMEOUT=2000;AUTO_SERVER=TRUE";
                  System.out.println("CREATED MANAGER 2");
            }
            logger.info(default_jdbc_url);
            default_jdbc_driver = "org.h2.Driver";
            default_target_database = "oracle.toplink.essentials.platform.database.H2Platform";
            default_jdbc_user = "sa";
            default_jdbc_password = "";
          } else {
            // mysql
            default_jdbc_url = "jdbc:mysql://salses:3306/" + Constant.dbName;
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
        //logging_level = properties.getProperty("toplink.logging.level", "FINE");
        logging_level = properties.getProperty("toplink.logging.level", "FINE");
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
        System.out.println("CREATED MANAGER 3 ");
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
               System.out.println("CREATED MANAGER 4");
               String persitenceUnitName = "default";
               emf = Persistence.createEntityManagerFactory(persitenceUnitName, propertiesMap);
               // just to create (and drop) the table if necessary
               em = emf.createEntityManager();
               em.close();
               System.out.println("CREATED MANAGER 5 ");
        } catch(Exception e) {
            System.out.println(e);
        }
    }

    public void cleanDB() {
        logger.info("cleanDB()");
        createTransactionalEntityManager();
        String tableName = Constant.docTableName;
        try {
            em.createNativeQuery("delete from " + tableName).executeUpdate();
        } catch (Exception ex) {
            logger.warn(ex.getCause());
        }
        closeTransactionalEntityManager();
    }

    public void persistFacet(Facet facet) {
        createTransactionalEntityManager();
        em.persist(facet);
        closeTransactionalEntityManager();
    }


    public void persistDoc(Document doc) {
        createTransactionalEntityManager();
        em.persist(doc);
        closeTransactionalEntityManager();
    }

    public FacetValue getFacetValue(String v) {
        em = emf.createEntityManager();
        String queryStr = "SELECT fv FROM FacetValue fv where fv.value = :v";
        Query query = em.createQuery(queryStr);
        query.setParameter("v", v);
        FacetValue fv = (FacetValue) query.getResultList().get(0);
        em.close();
        return fv;
    }

    List<FacetValue> getListFacetValues(HashSet hash) {
        em = emf.createEntityManager();
        Iterator itr = hash.iterator();
        int p =0;
        String params = "";
        String paramsValues = "";
        while(itr.hasNext()) {
              params = params + ":v" + p;
              itr.next();
              if (itr.hasNext()) {
                  params = params + ",";
              }
              p++;
        }
        String queryStr = "SELECT fv FROM FacetValue fv where fv.value IN (" + params + ")";
        Query query = em.createQuery(queryStr);
        itr = hash.iterator();
        p =0;
        while(itr.hasNext()) {
              query.setParameter("v" + p,itr.next());
              p++;
        }
        @SuppressWarnings("unchecked")
        List<FacetValue> fvs = query.getResultList();
        em.close();
        return fvs;
    }

     public List getListDocs() {
        em = emf.createEntityManager();
        String queryStr = "SELECT DISTINCT doc FROM Document doc JOIN doc.facetValues facetsValues JOIN facetsValues.facet facet";
        Query query = em.createQuery(queryStr);
        List list = query.getResultList();
        em.close();
        return list;
    }

    private void createTransactionalEntityManager() {
        logger.info("createTransactionalEntityManager()");
        em = emf.createEntityManager();
        em.getTransaction().begin();
    }

    private void closeTransactionalEntityManager() {
        logger.info("closeTransactionalEntityManager()");
        em.getTransaction().commit();
        em.close();
    }

    private void createEntityManager() {
        logger.info("createEntityManager()");
        em = emf.createEntityManager();
    }

    private void closeEntityManager() {
        logger.info("closeEntityManager()");
        em.close();
    }
}
